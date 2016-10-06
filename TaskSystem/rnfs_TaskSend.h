/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
https://twitter.com/Rinifisu
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "rnfs_TaskKeepArray.h"
#include "rnfs_TaskReceive.h"

namespace rnfs
{
	template<class TYPE = std::unordered_map<std::type_index, TaskKeepArray<Task>>>
	class _TaskSend_
	{
	protected:
		static TYPE	m_Send; //送信リストのポインタ
	};

	template<class TYPE>
	TYPE _TaskSend_<TYPE>::m_Send;

	///<summary>
	///<para>───────────────────</para>
	///<para>タスク送信</para>
	///<para>タスク同士のやり取りを行えます。</para>
	///<para>送信を許可する側のクラスです。</para>
	///<para>───────────────────</para>
	///</summary>
	class TaskSend final : public _TaskSend_<>
	{
		friend class		TaskReceive;	//様々な取得や参照で必要

	private:
		Task*				mp_Task;		//コール対象タスクのポインタ

		std::type_index		m_Type;			//識別用の情報
		TaskID				m_ID;			//消去用の管理番号

	public:
		///<summary>
		///<para>────────</para>
		///<para>初期化を行います。</para>
		///<para>────────</para>
		///</summary>
		TaskSend() : mp_Task(nullptr), m_Type(typeid(nullptr)), m_ID(0)
		{

		}

		///<summary>
		///<para>──────────────</para>
		///<para>タスク送信の登録解除を行います。</para>
		///<para>──────────────</para>
		///</summary>
		~TaskSend()
		{
			this->Unregister();
		}

		TaskSend(const TaskSend & taskSend) = delete;
		TaskSend(TaskSend && taskSend) = delete;
		void operator =(const TaskSend & taskSend) = delete;
		void operator =(TaskSend && taskSend) = delete;

		template<class TASK>
		void Register(TASK* p_Task);

		///<summary>
		///<para>──────────────</para>
		///<para>タスク送信の登録解除を行います。</para>
		///<para>──────────────</para>
		///</summary>
		void Unregister()
		{
			//リストから登録を解除する
			if (mp_Task)
			{
				//タスクの解放
				m_Send[m_Type].Free_ID(m_ID);

				//リストが空になったら、消去する
				if (m_Send[m_Type].isEmpty()) m_Send.erase(m_Type);
			}

			//初期化
			mp_Task = nullptr;
			m_Type = typeid(nullptr);
			m_ID = 0;
		}

		///<summary>
		///<para>────────────</para>
		///<para>全体の処理を行う空間です。</para>
		///<para>────────────</para>
		///</summary>
		class All
		{
		public:
			///<summary>
			///<para>────────────</para>
			///<para>関数呼び出しを開始します。</para>
			///<para>────────────</para>
			///</summary>
			static inline void Update()
			{
				TaskReceive* p_Receive = TaskReceive::mp_Begin; //現在のリストポインタ

				//末尾までループする
				while (p_Receive != nullptr)
				{
					//コールが有効であり、コールが設定されていたら
					if (p_Receive->m_Active && p_Receive->m_Call)
					{
						//関数の実行
						for (auto & i : m_Send[p_Receive->m_Type])
						{
							(*p_Receive->mp_Task.*p_Receive->m_Call)(i.second.task());
						}
					}

					//次のリストへ移動
					p_Receive = p_Receive->mp_Next;
				}
			}
		};
	};

	///<summary>
	///<para>─────────────────────────────────────────────</para>
	///<para>タスク送信の登録を行います。</para>
	///<para>登録を行うことで TaskSend::All::Update 呼び出し時に TaskReceive で指定されたタスクの関数に送信されます。</para>
	///<para>─────────────────────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///<para>タスク識別用の名前を取得するため、テンプレートになっています。</para>
	///</param>
	template<class TASK>
	inline void TaskSend::Register(TASK* p_Task)
	{
		//登録されていたら登録解除する
		if (mp_Task)
		{
			//タスクの解放
			m_Send[m_Type].Free_ID(m_ID);

			//リストが空になったら、消去する
			if (m_Send[m_Type].isEmpty()) m_Send.erase(m_Type);
		}

		mp_Task = p_Task;
		m_Type = typeid(TASK);
		
		//追加される位置を取得する
		m_ID = m_Send[m_Type].nextID();

		//カウントを無効にする(assert を回避する為、追加の前に実行)
		m_Send[m_Type].Safety_ID(m_ID, false);

		//追加
		m_Send[m_Type].Keep_Back(mp_Task);
	}
}
/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
https://twitter.com/Rinifisu
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <typeindex> //std::type_index

#include "rnfs_TaskKeepArray.h"

namespace rnfs
{
	//ソースファイル生成回避宣言
	template<class TYPE = std::unordered_map<std::type_index, TaskKeepArray<Task>>>
	class _TaskLink_
	{
	protected:
		static TYPE	m_List; //受信側のリスト
	};

	template<class TYPE>
	TYPE _TaskLink_<TYPE>::m_List;

	///<summary>
	///<para>──────────────</para>
	///<para>タスクリンク</para>
	///<para>タスク同士のやり取りを行えます。</para>
	///<para>──────────────</para>
	///</summary>
	class TaskLink final : public _TaskLink_<>
	{
	private:
		Task*				mp_Task;	//対象タスクのポインタ

		std::type_index		m_Type;		//識別用の情報
		TaskID				m_ID;		//消去用の管理番号

	public:
		///<summary>
		///<para>────────</para>
		///<para>初期化を行います。</para>
		///<para>────────</para>
		///</summary>
		TaskLink() : mp_Task(nullptr), m_Type(typeid(nullptr)), m_ID(0)
		{

		}

		template<class TASK>
		TaskLink(TASK* p_Task);

		///<summary>
		///<para>─────────────</para>
		///<para>送信側の登録解除を行います。</para>
		///<para>─────────────</para>
		///</summary>
		~TaskLink()
		{
			this->Unregister();
		}

		TaskLink(const TaskLink & taskLink) = delete;
		TaskLink(TaskLink && taskLink) = delete;
		void operator =(const TaskLink & taskLink) = delete;
		void operator =(TaskLink && taskLink) = delete;

		template<class TASK>
		void Register(TASK* p_Task);

		///<summary>
		///<para>─────────────</para>
		///<para>送信側の登録解除を行います。</para>
		///<para>─────────────</para>
		///</summary>
		void Unregister()
		{
			//リストから登録を解除する
			if (mp_Task)
			{
				//タスクの解放
				m_List[m_Type].Free_ID(m_ID);

				//リストが空になったら、消去する
				if (m_List[m_Type].isEmpty()) m_List.erase(m_Type);
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
			template<class TARGET, class Func = void(Task::*)(Task &)>
			static inline void Call(Task* p_Task, const Func & callbackFunction);
		};
	};

	///<summary>
	///<para>────────────────────────────────────</para>
	///<para>送信側の登録を行います。</para>
	///<para>登録を行うことで TaskLink::All::Call 呼び出し時に指定されたタスクの関数に送信されます。</para>
	///<para>────────────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///<para>タスク識別用の情報を取得するため、テンプレートになっています。</para>
	///</param>
	template<class TASK>
	inline TaskLink::TaskLink(TASK * p_Task)
		: mp_Task(nullptr), m_Type(typeid(nullptr)), m_ID(0)
	{
		this->Register(p_Task);
	}

	///<summary>
	///<para>────────────────────────────────────</para>
	///<para>送信側の登録を行います。</para>
	///<para>登録を行うことで TaskLink::All::Call 呼び出し時に指定されたタスクの関数に送信されます。</para>
	///<para>────────────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///<para>タスク識別用の情報を取得するため、テンプレートになっています。</para>
	///</param>
	template<class TASK>
	inline void TaskLink::Register(TASK* p_Task)
	{
		//登録されていたら登録解除する
		if (mp_Task)
		{
			//タスクの解放
			m_List[m_Type].Free_ID(m_ID);

			//リストが空になったら、消去する
			if (m_List[m_Type].isEmpty()) m_List.erase(m_Type);
		}

		mp_Task = p_Task;
		m_Type = typeid(TASK);

		//追加される位置を取得する
		m_ID = m_List[m_Type].nextID();

		//カウントを無効にする(assert を回避する為、追加の前に実行)
		m_List[m_Type].Safety_ID(m_ID, false);

		//追加
		m_List[m_Type].Keep_Back(mp_Task);
	}

	///<summary>
	///<para>────────────────────────────────────</para>
	///<para>タスク同士のやり取りを行います。</para>
	///<para>対象のタスクを引数に加えた関数が呼び出されます。</para>
	///<para>テンプレート引数を使用します。&lt;対象のタスク名&gt;</para>
	///<para>────────────────────────────────────</para>
	///<para>呼び出されるものは対象のタスク側で TaskLink::Register で登録を行ったものに限られます。</para>
	///<para>────────────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///</param>
	///
	///<param name="callbackFunction">
	///<para>省略可能</para>
	///<para>呼ばれるコール関数</para>
	///<para>型変換の省略のため、テンプレートになっています。</para>
	///</param>
	template<class TARGET, class Func>
	inline void TaskLink::All::Call(Task * p_Task, const Func & callbackFunction)
	{
		//関数の実行
		for (auto & i : TaskLink::m_List[typeid(TARGET)])
		{
			(*p_Task.*(void(Task::*)(Task &))callbackFunction)(i.second.task());
		}
	}
}
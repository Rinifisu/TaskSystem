/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "rnfs_TaskKeepArray.h"

namespace rnfs
{
	/// <summary>
	/// <para>───────────────────</para>
	/// <para>タスク送信</para>
	/// <para>タスク同士のやり取りを行えます。</para>
	/// <para>送信を許可する側のクラスです。</para>
	/// <para>───────────────────</para>
	/// </summary>
	class TaskSend final
	{
		friend class	TaskReceive;	//様々な取得や参照で必要

	private:
		Task*			mp_Task;		//コール対象タスクのポインタ

		std::string		m_Name;			//識別用の名前
		TaskID			m_ID;			//消去用の管理番号

	private:
		static std::unordered_map<std::string, TaskKeepArray<Task>>	m_Send; //送信リストのポインタ

	public:
		/// <summary>
		/// <para>────────</para>
		/// <para>初期化を行います。</para>
		/// <para>────────</para>
		/// </summary>
		TaskSend();

		/// <summary>
		/// <para>──────────────</para>
		/// <para>タスク送信の登録解除を行います。</para>
		/// <para>──────────────</para>
		/// </summary>
		~TaskSend();

		TaskSend(const TaskSend & taskSend) = delete;
		TaskSend(TaskSend && taskSend) = delete;
		void operator =(const TaskSend & taskSend) = delete;
		void operator =(TaskSend && taskSend) = delete;

		template<class TASK>
		void Register(TASK* p_Task);

		/// <summary>
		/// <para>──────────────</para>
		/// <para>タスク送信の登録解除を行います。</para>
		/// <para>──────────────</para>
		/// </summary>
		void Unregister();

		/// <summary>
		/// <para>────────────</para>
		/// <para>全体の処理を行う空間です。</para>
		/// <para>────────────</para>
		/// </summary>
		class All
		{
		public:
			/// <summary>
			/// <para>────────────</para>
			/// <para>関数呼び出しを開始します。</para>
			/// <para>────────────</para>
			/// </summary>
			static void Update();
		};
	};

	/// <summary>
	/// <para>─────────────────────────────────────────────</para>
	/// <para>タスク送信の登録を行います。</para>
	/// <para>登録を行うことで TaskSend::All::Update 呼び出し時に TaskReceive で指定されたタスクの関数に送信されます。</para>
	/// <para>─────────────────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="p_Task">
	/// <para>自身のポインタ</para>
	/// <para>必ず this を入力してください。</para>
	/// <para>タスク識別用の名前を取得するため、テンプレートになっています。</para>
	/// </param>
	template<class TASK>
	inline void TaskSend::Register(TASK* p_Task)
	{
		//登録されていたら登録解除する
		if (mp_Task)
		{
			//タスクの解放
			m_Send[m_Name].Free_ID(m_ID);

			//リストが空になったら、消去する
			if (m_Send[m_Name].isEmpty()) m_Send.erase(m_Name);
		}

		mp_Task = p_Task;
		m_Name = typeid(TASK).name();
		
		//追加される位置を取得する
		m_ID = m_Send[m_Name].nextID();

		//追加
		m_Send[m_Name].Keep_Back(mp_Task);

		//カウントを無効にする
		m_Send[m_Name].Safety_ID(m_ID, false);
	}
}
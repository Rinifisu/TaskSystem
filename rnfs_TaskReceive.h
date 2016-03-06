//Copyright © 2015-2016 Rinifisu
//http://rinifisu.blog.jp/

#pragma once

#include "rnfs_Task.h"

namespace rnfs
{
	/// <summary>
	/// <para>───────────────────</para>
	/// <para>タスク受信</para>
	/// <para>タスク同士のやり取りを行えます。</para>
	/// <para>受信を許可する側のクラスです。</para>
	/// <para>───────────────────</para>
	/// </summary>
	class TaskReceive final
	{
	private:
		Task*			mp_Task;	//コール対象タスクのポインタ

		std::string		m_Name;		//識別用の名前
		size_t			m_ID;		//消去用の管理番号

	private:
		static std::unordered_map<std::string, TaskKeepArray<Task>>	m_Receive; //受信リストのポインタ

	public:
		TaskReceive();
		~TaskReceive();

		template<class TASK>
		void Register(TASK* p_Task);

		/// <summary>
		/// <para>────────────</para>
		/// <para>タスク受信の解除を行います。</para>
		/// <para>────────────</para>
		/// </summary>
		void Unregister();

		/// <summary>
		/// <para>─────────────────────</para>
		/// <para>タスク送信とタスク受信全体の処理を行う空間です。</para>
		/// <para>─────────────────────</para>
		/// </summary>
		class System
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
	/// <para>──────────────────────────────────────────────────</para>
	/// <para>タスク受信の登録を行います。</para>
	/// <para>──────────────────────────────────────────────────</para>
	/// <para>登録を行うことで TaskConnect::Call 呼び出し時に TaskSend で指定されたタスクの関数が呼び出されます。</para>
	/// <para>そのままのタスクであれば呼び出されませんが、登録を行う事で呼び出されるようになります。</para>
	/// <para>──────────────────────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="p_Task">
	/// <para>自身のポインタ</para>
	/// <para>必ず this を入力してください。</para>
	/// <para>タスク識別用の名前を取得するため、テンプレートになっています。</para>
	/// </param>
	template<class TASK>
	inline void TaskReceive::Register(TASK* p_Task)
	{
		//登録されていたら登録解除する
		if (mp_Task)
		{
			//タスクの解放
			m_Receive[m_Name].Free_ID(m_ID);

			//リストが空になったら、消去する
			if (m_Receive[m_Name].isEmpty()) m_Receive.erase(m_Name);
		}

		mp_Task = p_Task;
		m_Name = typeid(TASK).name();
		
		//追加される位置を取得する
		m_ID = m_Receive[m_Name].nextID();

		//追加
		m_Receive[m_Name].Add_Back(mp_Task);

		//カウントを無効にする
		m_Receive[m_Name].Safety_ID(m_ID, false);
	}
}
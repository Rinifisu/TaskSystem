//Copyright © 2015-2016 Rinifisu
//http://rinifisu.blog.jp/

#pragma once

#include <functional> //std::function

#include "rnfs_Task.h"

namespace rnfs
{
	/// <summary>
	/// <para>───────────────────</para>
	/// <para>タスクコール</para>
	/// <para>タスクの関数を自動呼び出しすることができます。</para>
	/// <para>───────────────────</para>
	/// </summary>
	class TaskCall final
	{
	private:
		Task*			mp_Task;	//コール対象タスクのポインタ

		TaskCall*		mp_Prev;	//自身の前のポインタ
		TaskCall*		mp_Next;	//自身の後のポインタ
		size_t			m_Priority;	//自身のコール優先順位

		std::string		m_Name;		//自身の名前（デバッグ等で使用）
		unsigned char	m_Group;	//リストのグループ番号

		void(Task::*	m_Call)();	//コール関数
		bool			m_Active;	//コールが行われるか

	private:
		static std::unordered_map<unsigned char, TaskCall*>	m_Begin; //コールリストの先頭ポインタ

	private:
		//コールリストへの登録
		void _Register_(const bool pushBack);

		//コールリストから消去
		void _Unregister_();

	public:
		TaskCall();
		~TaskCall();

		template<class TASK, typename FUNC = void(Task::*)(), typename PRIORITY = size_t>
		void Register(TASK* p_Task, const FUNC & callbackFunction = nullptr, const unsigned char group = 0, const PRIORITY & priority = 0, const bool priorityPushBack = true);

		/// <summary>
		/// <para>───────────────</para>
		/// <para>コールリストの登録解除を行います。</para>
		/// <para>───────────────</para>
		/// </summary>
		void Unregister();

		template<typename FUNC>
		void SetCall(const FUNC & callbackFunction);

		/// <summary>
		/// <para>──────────────────────────────</para>
		/// <para>TaskCall::System::Update 呼び出し時に呼ばれるコール関数を消去します。</para>
		/// <para>TaskCall::isCall で false が返されるようになります。</para>
		/// <para>コールを終了する際に使用します。</para>
		/// <para>──────────────────────────────</para>
		/// </summary>
		void ClearCall();

		/// <summary>
		/// <para>─────────────────</para>
		/// <para>コール関数が設定されているか確認します。</para>
		/// <para>─────────────────</para>
		/// <para>true  -> コール関数が設定されている</para>
		/// <para>false -> コール関数が設定されていない</para>
		/// <para>─────────────────</para>
		/// </summary>
		const bool isCall() const;

		/// <summary>
		/// <para>──────────────────────────────</para>
		/// <para>TaskCall::System::Update 呼び出し時にコールが行われるかを設定します。</para>
		/// <para>一時停止などに使用できます。</para>
		/// <para>──────────────────────────────</para>
		/// </summary>
		///
		/// <param name="active">
		/// <para>コールが行われるか</para>
		/// <para>true  -> コール有効</para>
		/// <para>false -> コール無効</para>
		/// </param>
		void SetActive(const bool active);

		/// <summary>
		/// <para>──────────────────────────────</para>
		/// <para>TaskCall::System::Update 呼び出し時にコールが行われるかを確認します。</para>
		/// <para>──────────────────────────────</para>
		/// <para>true  -> コール有効</para>
		/// <para>false -> コール無効</para>
		/// <para>──────────────────────────────</para>
		/// </summary>
		const bool isActive() const;

		template<typename PRIORITY>
		void SetPriority(const PRIORITY & priority, const bool pushBack = true);

		template<typename PRIORITY = size_t>
		const PRIORITY priority() const;

		/// <summary>
		/// <para>─────────────</para>
		/// <para>デバッグ用の名前を変更します。</para>
		/// <para>─────────────</para>
		/// </summary>
		///
		/// <param name="name">
		/// <para>デバッグ用の名前</para>
		/// </param>
		void SetName(const std::string & name);

		/// <summary>
		/// <para>─────────────</para>
		/// <para>デバッグ用の名前を取得します。</para>
		/// <para>─────────────</para>
		/// </summary>
		const std::string & name() const;

		/// <summary>
		/// <para>───────────</para>
		/// <para>グループ番号を変更します。</para>
		/// <para>───────────</para>
		/// </summary>
		///
		/// <param name="group">
		/// <para>グループ番号（TaskCall::System::Update の引数で使われる分類）</para>
		/// </param>
		void SetGroup(const unsigned char group);

		/// <summary>
		/// <para>───────────</para>
		/// <para>グループ番号を取得します。</para>
		/// <para>───────────</para>
		/// </summary>
		const unsigned char group() const;

		/// <summary>
		/// <para>──────────────</para>
		/// <para>コール全体の処理を行う空間です。</para>
		/// <para>──────────────</para>
		/// </summary>
		class System
		{
		public:
			/// <summary>
			/// <para>───────────────────</para>
			/// <para>指定したグループの関数呼び出しを開始します。</para>
			/// <para>───────────────────</para>
			/// </summary>
			///
			/// <param name="group">
			/// <para>グループ番号</para>
			/// </param>
			static void Update(const unsigned char group);
		};
	};

	/// <summary>
	/// <para>────────────────────────────────────────────</para>
	/// <para>コールリストの登録を行います。</para>
	/// <para>────────────────────────────────────────────</para>
	/// <para>登録を行うことで TaskCall::System::Update 呼び出し時に 引数や SetCall で設定した関数が呼び出されます。</para>
	/// <para>────────────────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="p_Task">
	/// <para>自身のポインタ</para>
	/// <para>必ず this を入力してください。</para>
	/// <para>デバッグ用の名前を取得するため、テンプレートになっています。</para>
	/// </param>
	///
	/// <param name="callbackFunction">
	/// <para>省略可能</para>
	/// <para>TaskCall::System::Update 呼び出し時に呼ばれるコール関数</para>
	/// <para>型変換の省略とデフォルト引数の設定のため、テンプレートになっています。</para>
	/// </param>
	///
	/// <param name="group">
	/// <para>グループ番号（TaskCall::System::Update の引数で使われる分類）</para>
	/// </param>
	///
	/// <param name="priority">
	/// <para>省略可能</para>
	/// <para>優先度（値が少なければ少ないほどコールが先に行われる）</para>
	/// <para>型変換の省略（主にenum class）のため、テンプレートになっています。</para>
	/// </param>
	///
	/// <param name="priorityPushBack">
	/// <para>省略可能</para>
	/// <para>同じ優先度の集まりと競合した際の処理</para>
	/// <para>true  -> コールの集まりの末尾に設定</para>
	/// <para>false -> コールの集まりの先頭に設定</para>
	/// </param>
	template<class TASK, typename FUNC, typename PRIORITY>
	inline void TaskCall::Register(TASK* p_Task, const FUNC & callbackFunction, const unsigned char group, const PRIORITY & priority, const bool priorityPushBack)
	{
		//コールが登録されていたら登録解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = p_Task;
		m_Priority = static_cast<size_t>(priority);
		m_Name = typeid(TASK).name();
		m_Group = group;
		m_Call = static_cast<void(Task::*)()>(callbackFunction);
		m_Active = true;

		//登録
		this->_Register_(priorityPushBack);
	}

	/// <summary>
	/// <para>──────────────────────────────</para>
	/// <para>TaskCall::System::Update 呼び出し時に呼ばれるコール関数を設定します。</para>
	/// <para>前回の更新関数リストは消去され、新たに上書きされます。</para>
	/// <para>──────────────────────────────</para>
	/// </summary>
	///
	/// <param name="callbackFunction">
	/// <para>コール関数</para>
	/// </param>
	template<typename FUNC>
	inline void TaskCall::SetCall(const FUNC & callbackFunction)
	{
		m_Call = static_cast<void(Task::*)()>(callbackFunction);
	}

	/// <summary>
	/// <para>────────────────────────────</para>
	/// <para>TaskCall::System::Update 呼び出し時のコール優先度を変更します。</para>
	/// <para>────────────────────────────</para>
	/// </summary>
	///
	/// <param name="priority">
	/// <para>優先度（値が少なければ少ないほどコールが先に行われる）</para>
	/// </param>
	///
	/// <param name="pushBack">
	/// <para>同じ優先度の集まりと競合した際の処理</para>
	/// <para>true  -> コールの集まりの末尾に設定</para>
	/// <para>false -> コールの集まりの先頭に設定</para>
	/// </param>
	template<typename PRIORITY>
	inline void TaskCall::SetPriority(const PRIORITY & priority, const bool pushBack)
	{
		//リストから登録を解除する
		this->_Unregister_();

		//順番を変更
		m_Priority = static_cast<size_t>(priority);

		//リストに再登録する
		this->_Register_(pushBack);
	}

	/// <summary>
	/// <para>──────────────────────────────</para>
	/// <para>優先度を取得します。</para>
	/// <para>テンプレート引数を使用します。＜優先度の型名（デフォルトでは size_t）＞</para>
	/// <para>──────────────────────────────</para>
	/// </summary>
	template<typename PRIORITY>
	inline const PRIORITY TaskCall::priority() const
	{
		return static_cast<PRIORITY>(m_Priority);
	}
}
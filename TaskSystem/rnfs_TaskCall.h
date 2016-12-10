/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
https://twitter.com/Rinifisu
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <deque>			//std::deque
#include <unordered_map>	//std::unordered_map

#include "rnfs_Task.h"

namespace rnfs
{
	//ソースファイル生成回避宣言
	template<class TYPE = std::unordered_map<unsigned char, TaskCall*>>
	class _TaskCall_
	{
	protected:
		static TYPE	m_Begin; //コールリストの先頭ポインタ
	};

	template<class TYPE>
	TYPE _TaskCall_<TYPE>::m_Begin;

	///<summary>
	///<para>───────────────────</para>
	///<para>タスクコール</para>
	///<para>タスクの関数を自動呼び出しすることができます。</para>
	///<para>───────────────────</para>
	///</summary>
	class TaskCall final : public _TaskCall_<>
	{
	private:
		Task*			mp_Task;	//コール対象タスクのポインタ

		TaskCall*		mp_Prev;	//自身の前のポインタ
		TaskCall*		mp_Next;	//自身の後のポインタ

		unsigned char	m_Group;	//リストのグループ番号
		size_t			m_Priority;	//コール優先順位

		void(Task::*	m_Call)();	//コール関数
		bool			m_Active;	//コールが行われるか

		std::string		m_Tag;		//タグ
		bool			m_IsFirst;	//最初のコールであるか

	private:
		//コールリストへの登録
		void _Register_(const bool pushBack)
		{
			//先頭が空の場合は新規リストを設定
			if (m_Begin[m_Group] == nullptr) m_Begin[m_Group] = this;
			//登録処理
			else
			{
				TaskCall* task = m_Begin[m_Group];	//現在のリストポインタ
				TaskCall* prev = nullptr;			//１つ前のリストポインタ

				//末尾までループする
				while (task != nullptr)
				{
					//優先度の関係でこれ以上進めない場合
					if ((pushBack && m_Priority < task->m_Priority)
						|| (!pushBack && m_Priority <= task->m_Priority))
					{
						//入れ替え対象のリストが先頭だった場合は自身で上書き
						if (task == m_Begin[m_Group]) m_Begin[m_Group] = this;

						//次が無いポインタにリストを代入
						mp_Next = task;

						//前が無いポインタに新規リストを代入
						task->mp_Prev = this;

						break;
					}

					//前のリストに現在のリストを代入
					prev = task;

					//次のリストへ移動
					task = task->mp_Next;

					//優先順位が決まらなかった場合は nullptr を代入
					//ここに到達するリストは優先度がリストの中で１番最大である
					if (task == nullptr) mp_Next = nullptr;
				}

				//先頭を変更した場合は前が存在しないので nullptr を保持
				if (prev != nullptr)
				{
					//次が無いポインタに新規リストを代入
					prev->mp_Next = this;

					//前が無いポインタにリストを代入
					mp_Prev = prev;
				}
			}
		}

		//コールリストから消去
		void _Unregister_()
		{
			TaskCall* updateNext = mp_Next; //リストの次
			TaskCall* updatePrev = mp_Prev; //リストの前

			//次に前を代入してつなぎ合わせる
			if (updateNext != nullptr) updateNext->mp_Prev = updatePrev;

			//前に次を代入してつなぎ合わせる
			if (updatePrev != nullptr) updatePrev->mp_Next = updateNext;
			//null の場合は一番先頭にあったリストがなくなったので、次のリストを先頭にする
			else m_Begin[m_Group] = updateNext;

			//次のリストも null の場合はリストが空になったので、消去する
			if (m_Begin[m_Group] == nullptr) m_Begin.erase(m_Group);
		}

	public:
		///<summary>
		///<para>────────</para>
		///<para>初期化を行います。</para>
		///<para>────────</para>
		///</summary>
		TaskCall()
			: mp_Task(nullptr), mp_Prev(nullptr), mp_Next(nullptr)
			, m_Group(0), m_Priority(0)
			, m_Active(false), m_IsFirst(true)
		{

		}

		template<typename FUNC = void(Task::*)(), typename PRIORITY = size_t>
		TaskCall(Task* p_Task, const FUNC & callbackFunction = nullptr, const unsigned char group = 0, const PRIORITY & priority = 0, const bool priorityPushBack = true);

		///<summary>
		///<para>───────────────</para>
		///<para>コールリストの登録解除を行います。</para>
		///<para>───────────────</para>
		///</summary>
		~TaskCall()
		{
			//コールが登録されていたら登録解除する
			if (mp_Task) this->_Unregister_();
		}

		TaskCall(const TaskCall & taskCall) = delete;
		TaskCall(TaskCall && taskCall) = delete;
		void operator =(const TaskCall & taskCall) = delete;
		void operator =(TaskCall && taskCall) = delete;

		template<typename FUNC = void(Task::*)(), typename PRIORITY = size_t>
		void Register(Task* p_Task, const FUNC & callbackFunction = nullptr, const unsigned char group = 0, const PRIORITY & priority = 0, const bool priorityPushBack = true);

		///<summary>
		///<para>───────────────</para>
		///<para>コールリストの登録解除を行います。</para>
		///<para>───────────────</para>
		///</summary>
		void Unregister()
		{
			//リストから登録を解除する
			if (mp_Task) this->_Unregister_();

			//初期化
			mp_Task = nullptr;
			mp_Prev = nullptr;
			mp_Next = nullptr;
			m_Group = 0;
			m_Priority = 0;
			m_Call = nullptr;
			m_Active = false;
		}

		///<summary>
		///<para>───────────</para>
		///<para>グループ番号を変更します。</para>
		///<para>───────────</para>
		///</summary>
		///
		///<param name="group">
		///<para>グループ番号（TaskCall::All::Update の引数で使われる分類）</para>
		///</param>
		void SetGroup(const unsigned char group)
		{
			m_Group = group;
		}

		///<summary>
		///<para>───────────</para>
		///<para>グループ番号を取得します。</para>
		///<para>───────────</para>
		///</summary>
		unsigned char group() const
		{
			return m_Group;
		}

		template<typename PRIORITY>
		void SetPriority(const PRIORITY & priority, const bool pushBack = true);

		template<typename PRIORITY = size_t>
		PRIORITY priority() const;

		template<typename FUNC>
		void SetCall(const FUNC & callbackFunction, const bool active = true, const std::string & tag = "");

		///<summary>
		///<para>─────────────────────────────</para>
		///<para>TaskCall::All::Update 呼び出し時にコールが行われるかを切り替えます。</para>
		///<para>有効と無効が呼び出す度に反転します。</para>
		///<para>一時停止などに利用できます。</para>
		///<para>─────────────────────────────</para>
		///</summary>
		void SetActive()
		{
			m_Active = !m_Active;
		}

		///<summary>
		///<para>────────────────────────────</para>
		///<para>TaskCall::All::Update 呼び出し時にコールが行われるかを設定します。</para>
		///<para>一時停止などに利用できます。</para>
		///<para>────────────────────────────</para>
		///</summary>
		///
		///<param name="active">
		///<para>コールが行われるか</para>
		///<para>true  -> コール有効</para>
		///<para>false -> コール無効</para>
		///</param>
		void SetActive(const bool active)
		{
			m_Active = active;
		}

		///<summary>
		///<para>────────────────────────────</para>
		///<para>TaskCall::All::Update 呼び出し時にコールが行われるかを確認します。</para>
		///<para>────────────────────────────</para>
		///<para>true  -> コール有効</para>
		///<para>false -> コール無効</para>
		///<para>────────────────────────────</para>
		///</summary>
		bool isActive() const
		{
			return m_Active;
		}

		///<summary>
		///<para>────────</para>
		///<para>タグを変更します。</para>
		///<para>────────</para>
		///</summary>
		///
		///<param name="tag">
		///<para>タグ</para>
		///</param>
		void SetTag(const std::string & tag)
		{
			m_Tag = tag;
		}

		///<summary>
		///<para>────────</para>
		///<para>タグを取得します。</para>
		///<para>────────</para>
		///</summary>
		const std::string & tag() const
		{
			return m_Tag;
		}

		///<summary>
		///<para>────────────────────</para>
		///<para>最初のコール状態か確認します。</para>
		///<para>仕様上、2 回以上の確認を行う事ができません。</para>
		///<para>初期化などに利用できます。</para>
		///<para>────────────────────</para>
		///<para>true  -> 最初のコール</para>
		///<para>false -> 既にコールが行われている</para>
		///<para>────────────────────</para>
		///</summary>
		bool isFirst()
		{
			if (m_IsFirst)
			{
				m_IsFirst = false;

				return true;
			}

			return false;
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
			///<para>───────────────────</para>
			///<para>指定したグループの関数呼び出しを開始します。</para>
			///<para>───────────────────</para>
			///</summary>
			///
			///<param name="group">
			///<para>グループ番号</para>
			///</param>
			static inline void Update(const unsigned char group = 0)
			{
				TaskCall* p_TaskCall = m_Begin[group]; //現在のリストポインタ

				//末尾までループする
				while (p_TaskCall != nullptr)
				{
					//コールが有効であり、コールが設定されていたら
					if (p_TaskCall->m_Active && p_TaskCall->m_Call)
					{
						//コールの実行
						(p_TaskCall->mp_Task->*p_TaskCall->m_Call)();
					}

					//次のリストへ移動
					p_TaskCall = p_TaskCall->mp_Next;
				}
			}
		};
	};

	///<summary>
	///<para>──────────────────────────────────────────</para>
	///<para>コールリストの登録を行います。</para>
	///<para>──────────────────────────────────────────</para>
	///<para>登録を行うことで TaskCall::All::Update 呼び出し時に 引数や SetCall で設定した関数が呼び出されます。</para>
	///<para>──────────────────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///</param>
	///
	///<param name="callbackFunction">
	///<para>省略可能</para>
	///<para>TaskCall::All::Update 呼び出し時に呼ばれるコール関数</para>
	///<para>型変換の省略とデフォルト引数の設定のため、テンプレートになっています。</para>
	///</param>
	///
	///<param name="group">
	///<para>グループ番号（TaskCall::All::Update の引数で使われる分類）</para>
	///</param>
	///
	///<param name="priority">
	///<para>省略可能</para>
	///<para>優先度（値が少なければ少ないほどコールが先に行われる）</para>
	///<para>型変換の省略（主にenum class）のため、テンプレートになっています。</para>
	///</param>
	///
	///<param name="priorityPushBack">
	///<para>省略可能</para>
	///<para>同じ優先度の集まりと競合した際の処理</para>
	///<para>true  -> コールの集まりの末尾に設定</para>
	///<para>false -> コールの集まりの先頭に設定</para>
	///</param>
	template<typename FUNC, typename PRIORITY>
	inline TaskCall::TaskCall(Task* p_Task, const FUNC & callbackFunction, const unsigned char group, const PRIORITY & priority, const bool priorityPushBack)
		: mp_Task(p_Task), m_Group(group), m_Priority(static_cast<size_t>(priority))
		, m_Call(static_cast<void(Task::*)()>(callbackFunction))
		, m_Active(true), m_IsFirst(true)
	{
		//登録
		this->_Register_(priorityPushBack);
	}

	///<summary>
	///<para>──────────────────────────────────────────</para>
	///<para>コールリストの登録を行います。</para>
	///<para>──────────────────────────────────────────</para>
	///<para>登録を行うことで TaskCall::All::Update 呼び出し時に 引数や SetCall で設定した関数が呼び出されます。</para>
	///<para>──────────────────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///</param>
	///
	///<param name="callbackFunction">
	///<para>省略可能</para>
	///<para>TaskCall::All::Update 呼び出し時に呼ばれるコール関数</para>
	///<para>型変換の省略とデフォルト引数の設定のため、テンプレートになっています。</para>
	///</param>
	///
	///<param name="group">
	///<para>グループ番号（TaskCall::All::Update の引数で使われる分類）</para>
	///</param>
	///
	///<param name="priority">
	///<para>省略可能</para>
	///<para>優先度（値が少なければ少ないほどコールが先に行われる）</para>
	///<para>型変換の省略（主にenum class）のため、テンプレートになっています。</para>
	///</param>
	///
	///<param name="priorityPushBack">
	///<para>省略可能</para>
	///<para>同じ優先度の集まりと競合した際の処理</para>
	///<para>true  -> コールの集まりの末尾に設定</para>
	///<para>false -> コールの集まりの先頭に設定</para>
	///</param>
	template<typename FUNC, typename PRIORITY>
	inline void TaskCall::Register(Task* p_Task, const FUNC & callbackFunction, const unsigned char group, const PRIORITY & priority, const bool priorityPushBack)
	{
		//コールが登録されていたら登録解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = p_Task;
		m_Group = group;
		m_Priority = static_cast<size_t>(priority);
		m_Call = static_cast<void(Task::*)()>(callbackFunction);
		m_Active = true;
		m_IsFirst = true;

		//登録
		this->_Register_(priorityPushBack);
	}

	///<summary>
	///<para>──────────────────────────</para>
	///<para>TaskCall::All::Update 呼び出し時のコール優先度を変更します。</para>
	///<para>──────────────────────────</para>
	///</summary>
	///
	///<param name="priority">
	///<para>優先度（値が少なければ少ないほどコールが先に行われる）</para>
	///</param>
	///
	///<param name="pushBack">
	///<para>同じ優先度の集まりと競合した際の処理</para>
	///<para>true  -> コールの集まりの末尾に設定</para>
	///<para>false -> コールの集まりの先頭に設定</para>
	///</param>
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

	///<summary>
	///<para>─────────────────────────────</para>
	///<para>優先度を取得します。</para>
	///<para>テンプレート引数を使用します。&lt;優先度の型名（デフォルトでは size_t）&gt;</para>
	///<para>─────────────────────────────</para>
	///</summary>
	template<typename PRIORITY>
	inline PRIORITY TaskCall::priority() const
	{
		return static_cast<PRIORITY>(m_Priority);
	}

	///<summary>
	///<para>────────────────────────────</para>
	///<para>TaskCall::All::Update 呼び出し時に呼ばれるコール関数を設定します。</para>
	///<para>前回の更新関数リストは消去され、新たに上書きされます。</para>
	///<para>────────────────────────────</para>
	///</summary>
	///
	///<param name="callbackFunction">
	///<para>コール関数</para>
	///</param>
	///
	///<param name="active">
	///<para>コールが行われるか</para>
	///<para>true  -> コール有効</para>
	///<para>false -> コール無効</para>
	///</param>
	///
	///<param name="tag">
	///<para>タグ</para>
	///</param>
	template<typename FUNC>
	inline void TaskCall::SetCall(const FUNC & callbackFunction, const bool active, const std::string & tag)
	{
		m_Call = static_cast<void(Task::*)()>(callbackFunction);
		m_Active = active;
		m_Tag = tag;
		m_IsFirst = true;
	}
}
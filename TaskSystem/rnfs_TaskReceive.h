/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <string>

#include "rnfs_Task.h"

namespace rnfs
{
	///<summary>
	///<para>─────────────</para>
	///<para>タスク受信</para>
	///<para>タスク同士のやり取りを行えます。</para>
	///<para>受信を設定する側のクラスです。</para>
	///<para>─────────────</para>
	///</summary>
	class TaskReceive final
	{
		friend class		TaskSend;			//様々な取得や参照で必要

	private:
		Task*				mp_Task;			//コール対象タスクのポインタ

		TaskReceive*		mp_Prev;			//自身の前のポインタ
		TaskReceive*		mp_Next;			//自身の後のポインタ
		size_t				m_Priority;			//自身のコール優先順位

		std::string			m_Name;				//自身の名前（デバッグ等で使用）
		std::type_index		m_Type;				//対象の情報（コール時に使用）

		void(Task::*		m_Call)(Task &);	//コール関数
		bool				m_Active;			//コールが行われるか

	private:
		static TaskReceive*	mp_Begin;			//受信リストの先頭ポインタ

	private:
		//受信側の登録
		void _Register_(const bool pushBack)
		{
			//先頭のポインタが空の場合は新規リストのポインタを設定
			if (mp_Begin == nullptr) mp_Begin = this;
			//登録処理
			else
			{
				TaskReceive* task = mp_Begin;	//現在のリストポインタ
				TaskReceive* prev = nullptr;	//１つ前のリストポインタ

				//末尾までループする
				while (task != nullptr)
				{
					//優先度の関係でこれ以上進めない場合
					if ((pushBack && m_Priority < task->m_Priority)
						|| (!pushBack && m_Priority <= task->m_Priority))
					{
						//入れ替え対象のリストが先頭だった場合は自身で上書き
						if (task == mp_Begin) mp_Begin = this;

						//次が無いポインタにリストのポインタを代入
						mp_Next = task;

						//前が無いポインタに新規リストのポインタを代入
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
					//次が無いポインタに新規リストのポインタを代入
					prev->mp_Next = this;

					//前が無いポインタにリストのポインタを代入
					mp_Prev = prev;
				}
			}
		}

		//受信側の消去
		void _Unregister_()
		{
			TaskReceive* updateNext = mp_Next; //リストの次のポインタ
			TaskReceive* updatePrev = mp_Prev; //リストの前のポインタ

			//次のポインタに前のポインタを代入してつなぎ合わせる
			if (updateNext != nullptr) updateNext->mp_Prev = updatePrev;

			//前のポインタに次のポインタを代入してつなぎ合わせる
			if (updatePrev != nullptr) updatePrev->mp_Next = updateNext;
			//null の場合は一番先頭にあったリストがなくなったので、次のリストを先頭にする
			else mp_Begin = updateNext;
		}

	public:
		///<summary>
		///<para>────────</para>
		///<para>初期化を行います。</para>
		///<para>────────</para>
		///</summary>
		TaskReceive()
			: mp_Task(nullptr), mp_Prev(nullptr), mp_Next(nullptr)
			, m_Priority(0), m_Name(""), m_Type(typeid(nullptr))
			, m_Active(false)
		{

		}

		///<summary>
		///<para>──────────────</para>
		///<para>タスク受信の登録解除を行います。</para>
		///<para>──────────────</para>
		///</summary>
		~TaskReceive()
		{
			//コールが登録されていたら登録解除する
			if (mp_Task) this->_Unregister_();
		}

		TaskReceive(const TaskReceive & taskReceive) = delete;
		TaskReceive(TaskReceive && taskReceive) = delete;
		void operator = (const TaskReceive & taskReceive) = delete;
		void operator = (TaskReceive && taskReceive) = delete;

		template<class TARGET, class TASK, class Func = void(Task::*)(Task &)>
		void Call(TASK* p_Task, const Func & callbackFunction);

		template<class TARGET, class TASK, class Func = void(Task::*)(Task &), typename PRIORITY = size_t>
		void Register(TASK* p_Task, const Func & callbackFunction = nullptr, const PRIORITY & priority = 0, const bool priorityPushBack = true);

		///<summary>
		///<para>──────────────</para>
		///<para>タスク受信の登録解除を行います。</para>
		///<para>──────────────</para>
		///</summary>
		void Unregister()
		{
			//リストから登録を解除する
			if (mp_Task) this->_Unregister_();

			//初期化
			mp_Task = nullptr;
			mp_Prev = nullptr;
			mp_Next = nullptr;
			m_Priority = 0;
			m_Name = "";
			m_Type = typeid(nullptr);
			m_Call = nullptr;
			m_Active = false;
		}

		///<summary>
		///<para>─────────────────────────────</para>
		///<para>TaskSend::All::Update 呼び出し時にコールが行われるかを切り替えます。</para>
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
		///<para>TaskSend::All::Update 呼び出し時にコールが行われるかを設定します。</para>
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
		///<para>TaskSend::All::Update 呼び出し時にコールが行われるかを確認します。</para>
		///<para>────────────────────────────</para>
		///<para>true  -> コール有効</para>
		///<para>false -> コール無効</para>
		///<para>────────────────────────────</para>
		///</summary>
		const bool isActive() const
		{
			return m_Active;
		}

		template<typename PRIORITY>
		void SetPriority(const PRIORITY & priority, const bool pushBack = true);

		template<typename PRIORITY = size_t>
		const PRIORITY priority() const;

		///<summary>
		///<para>─────────────</para>
		///<para>デバッグ用の名前を変更します。</para>
		///<para>─────────────</para>
		///</summary>
		///
		///<param name="name">
		///<para>デバッグ用の名前</para>
		///</param>
		void SetName(const std::string & name)
		{
			m_Name = name;
		}

		///<summary>
		///<para>─────────────</para>
		///<para>デバッグ用の名前を取得します。</para>
		///<para>─────────────</para>
		///</summary>
		const std::string & name() const
		{
			return m_Name;
		}
	};

	///<summary>
	///<para>────────────────────────────────────</para>
	///<para>送信側とのやり取りを行います。</para>
	///<para>対象のタスクを引数に加えた関数が呼び出されます。</para>
	///<para>テンプレート引数を使用します。&lt;対象のタスク名&gt;</para>
	///<para>────────────────────────────────────</para>
	///<para>呼び出されるものは対象のタスク側で TaskSend を宣言し、登録を行ったものに限られます。</para>
	///<para>────────────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///<para>デバッグ用の名前を取得するため、テンプレートになっています。</para>
	///</param>
	///
	///<param name="callbackFunction">
	///<para>省略可能</para>
	///<para>呼ばれるコール関数</para>
	///<para>型変換の省略のため、テンプレートになっています。</para>
	///</param>
	template<class TARGET, class TASK, class Func>
	inline void TaskReceive::Call(TASK * p_Task, const Func & callbackFunction)
	{
		//関数の実行
		for (auto & i : TaskSend::m_Send[typeid(TARGET).name()])
		{
			(*p_Task.*(void(Task::*)(Task &))callbackFunction)(i.second.task());
		}
	}

	///<summary>
	///<para>────────────────────────────────────────────</para>
	///<para>タスク受信の登録を行います。</para>
	///<para>テンプレート引数を使用します。&lt;対象のタスク名&gt;</para>
	///<para>────────────────────────────────────────────</para>
	///<para>登録を行うことで TaskSend::All::Update 呼び出し時に、対象のタスクを引数に加えた関数が呼び出されます。</para>
	///<para>呼び出されるものは対象のタスク側で TaskSend を宣言し、登録を行ったものに限られます。</para>
	///<para>────────────────────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///<para>デバッグ用の名前を取得するため、テンプレートになっています。</para>
	///</param>
	///
	///<param name="callbackFunction">
	///<para>省略可能</para>
	///<para>TaskSend::All::Update 呼び出し時に呼ばれるコール関数</para>
	///<para>型変換の省略とデフォルト引数の設定のため、テンプレートになっています。</para>
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
	template<class TARGET, class TASK, class Func, typename PRIORITY>
	inline void TaskReceive::Register(TASK * p_Task, const Func & callbackFunction, const PRIORITY & priority, const bool priorityPushBack)
	{
		//コールが登録されていたら登録解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = p_Task;
		m_Priority = static_cast<size_t>(priority);
		m_Name = typeid(TASK).name();
		m_Type = typeid(TARGET);
		m_Call = (void(Task::*)(Task &))callbackFunction;
		m_Active = true;

		//登録
		this->_Register_(priorityPushBack);
	}

	///<summary>
	///<para>──────────────────────────</para>
	///<para>TaskSend::All::Update 呼び出し時のコール優先度を変更します。</para>
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
	inline void TaskReceive::SetPriority(const PRIORITY & priority, const bool pushBack)
	{
		//リストから登録を解除する
		this->_Unregister_();

		//順番を変更
		m_Priority = static_cast<size_t>(priority);

		//リストに再登録する
		this->_Register_(pushBack);
	}

	///<summary>
	///<para>─────────</para>
	///<para>優先度を取得します。</para>
	///<para>─────────</para>
	///</summary>
	template<typename PRIORITY>
	inline const PRIORITY TaskReceive::priority() const
	{
		return static_cast<PRIORITY>(m_Priority);
	}
}
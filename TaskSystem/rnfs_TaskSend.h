/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <functional> //std::function

#include "rnfs_TaskReceive.h"

namespace rnfs
{
	/// <summary>
	/// <para>───────────────────</para>
	/// <para>タスク送信</para>
	/// <para>タスク同士のやり取りを行えます。</para>
	/// <para>送信を行いたい関数を指定する側のクラスです。</para>
	/// <para>───────────────────</para>
	/// </summary>
	class TaskSend final
	{
		friend class		TaskReceive;		//様々な取得や参照で必要

	private:
		Task*				mp_Task;			//コール対象タスクのポインタ

		TaskSend*			mp_Prev;			//自身の前のポインタ
		TaskSend*			mp_Next;			//自身の後のポインタ
		size_t				m_Priority;			//自身のコール優先順位

		std::string			m_Name;				//自身の名前（デバッグ等で使用）
		std::string			m_Check;			//対象の名前（コール時に使用）

		void(Task::*		m_Call)(Task &);	//送信対象のコール関数
		bool				m_Active;			//コールが行われるか

	private:
		static TaskSend*	mp_Begin;			//送信リストの先頭ポインタ

	private:
		//送信側の登録
		void _Register_(const bool pushBack);

		//送信側の消去
		void _Unregister_();

	public:
		TaskSend();
		~TaskSend();

		template<class TARGET = Task, class TASK, class Func = void(Task::*)(Task &), typename PRIORITY = size_t>
		void Register(TASK* p_Task, const Func & callbackFunction = nullptr, const PRIORITY & priority = 0, const bool priorityPushBack = true);

		/// <summary>
		/// <para>──────────────────</para>
		/// <para>タスクコネクト送信側の登録解除を行います。</para>
		/// <para>──────────────────</para>
		/// </summary>
		void Unregister();

		/// <summary>
		/// <para>─────────────────────────────────</para>
		/// <para>TaskConnect::Update 呼び出し時に呼ばれる送信対象のコール関数を消去します。</para>
		/// <para>TaskSend::isCall で false が返されるようになります。</para>
		/// <para>コールを終了する際に使用します。</para>
		/// <para>─────────────────────────────────</para>
		/// </summary>
		void ClearCall();

		/// <summary>
		/// <para>──────────────────────</para>
		/// <para>送信対象のコール関数が設定されているか確認します。</para>
		/// <para>──────────────────────</para>
		/// <para>true  -> コール関数が設定されている</para>
		/// <para>false -> コール関数が設定されていない</para>
		/// <para>──────────────────────</para>
		/// </summary>
		const bool isCall() const;

		/// <summary>
		/// <para>────────────────────────────</para>
		/// <para>TaskConnect::Update 呼び出し時にコールが行われるかを設定します。</para>
		/// <para>一時停止などに使用できます。</para>
		/// <para>────────────────────────────</para>
		/// </summary>
		///
		/// <param name="active">
		/// <para>コールが行われるか</para>
		/// <para>true  -> コール有効</para>
		/// <para>false -> コール無効</para>
		/// </param>
		void SetActive(const bool active);

		/// <summary>
		/// <para>────────────────────────────</para>
		/// <para>TaskConnect::Update 呼び出し時にコールが行われるかを確認します。</para>
		/// <para>────────────────────────────</para>
		/// <para>true  -> コール有効</para>
		/// <para>false -> コール無効</para>
		/// <para>────────────────────────────</para>
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
	};
	
	/// <summary>
	/// <para>──────────────────────────────────────────</para>
	/// <para>タスクコネクト送信側の登録を行います。</para>
	/// <para>テンプレート引数を使用します。&lt;受信側のタスク名&gt;</para>
	/// <para>──────────────────────────────────────────</para>
	/// <para>登録を行うことで TaskConnect::Update 呼び出し時に 対象のタスクの関数が呼び出されます。</para>
	/// <para>呼び出されるものは対象のタスク側で TaskReceive を宣言し、登録を行ったタスクに限られます。</para>
	/// <para>──────────────────────────────────────────</para>
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
	/// <para>TaskConnect::Update 呼び出し時に呼ばれる送信対象のコール関数</para>
	/// <para>型変換の省略とデフォルト引数の設定のため、テンプレートになっています。</para>
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
	template<class TARGET, class TASK, class Func, typename PRIORITY>
	inline void TaskSend::Register(TASK * p_Task, const Func & callbackFunction, const PRIORITY & priority, const bool priorityPushBack)
	{
		//コールが登録されていたら登録解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = p_Task;
		m_Priority = static_cast<size_t>(priority);
		m_Name = typeid(TASK).name();
		m_Check = typeid(TARGET).name();
		m_Call = (void(Task::*)(Task &))callbackFunction;
		m_Active = true;

		//登録
		this->_Register_(priorityPushBack);
	}

	/// <summary>
	/// <para>──────────────────────────</para>
	/// <para>TaskConnect::Update 呼び出し時のコール優先度を変更します。</para>
	/// <para>──────────────────────────</para>
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
	inline void TaskSend::SetPriority(const PRIORITY & priority, const bool pushBack)
	{
		//リストから登録を解除する
		this->_Unregister_();

		//順番を変更
		m_Priority = static_cast<size_t>(priority);

		//リストに再登録する
		this->_Register_(pushBack);
	}

	/// <summary>
	/// <para>─────────</para>
	/// <para>優先度を取得します。</para>
	/// <para>─────────</para>
	/// </summary>
	template<typename PRIORITY>
	inline const PRIORITY TaskSend::priority() const
	{
		return static_cast<PRIORITY>(m_Priority);
	}
}
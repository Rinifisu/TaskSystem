﻿/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "rnfs_Task.h"

namespace rnfs
{
	/// <summary>
	/// <para>───────────────</para>
	/// <para>タスクキープ</para>
	/// <para>タスクを持つための専用ポインタです。</para>
	/// <para>───────────────</para>
	/// </summary>
	template<class TYPE = Task>
	class TaskKeep final
	{
	private:
		TYPE*	mp_Task;	//タスクポインタ
		bool	m_Safety;	//キープカウント保障

	private:
		TaskKeep(const TaskKeep<TYPE> & taskKeep) = delete;
		TaskKeep(TaskKeep<TYPE> && taskKeep) = delete;

		void _Reset_(TYPE* p_Task);

	public:
		TaskKeep();
		~TaskKeep();

		void operator = (TYPE* p_Task);
		void operator = (const TaskKeep<TYPE> & taskKeep);

		void Clear();
		void Free();

		void Safety(const bool safety);

		TYPE* operator -> () const;
		TYPE& operator () () const;

		TYPE& getTask() const;
		TYPE* getTaskPointer() const;

		operator bool() const;
	};

	//タスクの消去と上書き
	template<class TYPE>
	inline void TaskKeep<TYPE>::_Reset_(TYPE* p_Task)
	{
		//Task::Destroy が呼び出されたタスクは次の更新で消えてしまうので追加できない
		//タスククラスのコンストラクタ内で Task::Destroy を呼び出す事によるエラーが多い
		if (p_Task && p_Task->m_Destroy)
		{
			//停止
			assert(!"TaskKeep -> Task::Destroy を呼び出した後のタスクはキープできません。");

			//安全のため、消去を無効にする
			p_Task->m_Destroy = false;
		}

		//タスクが既にキープされている場合は、消去する
		if (mp_Task && m_Safety)
		{
			//キープカウントを減らす
			--mp_Task->m_Link;

			//キープがない場合はタスクを消去
			if (mp_Task->m_Link == 0) mp_Task->m_Destroy = true;
		}

		//タスクの代入
		mp_Task = p_Task;

		//タスクが存在する場合は、ここ以外からの消去を出来ないようにキープカウントを追加する
		if (mp_Task && m_Safety) ++mp_Task->m_Link;
	}

	/// <summary>
	/// <para>─────────────────────</para>
	/// <para>初期化します。</para>
	/// <para>コピーコンストラクタは TaskKeep の仕様上できません。</para>
	/// <para>─────────────────────</para>
	/// </summary>
	template<class TYPE>
	inline TaskKeep<TYPE>::TaskKeep()
		: mp_Task(nullptr), m_Safety(true)
	{

	}

	//タスクの消去
	template<class TYPE>
	inline TaskKeep<TYPE>::~TaskKeep()
	{
		_Reset_(nullptr);
	}

	/// <summary>
	/// <para>──────────────────────</para>
	/// <para>タスクをキープします。</para>
	/// <para>既にキープ中であるタスクは消去、または解放されます。</para>
	/// <para>──────────────────────</para>
	/// </summary>
	///
	/// <param name="p_Task">
	/// <para>キープ対象のタスク</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeep<TYPE>::operator = (TYPE* p_Task)
	{
		_Reset_(p_Task);
	}

	/// <summary>
	/// <para>──────────────────────</para>
	/// <para>タスクをキープします。</para>
	/// <para>既にキープ中であるタスクは消去、または解放されます。</para>
	/// <para>──────────────────────</para>
	/// </summary>
	///
	/// <param name="taskKeep">
	/// <para>キープ対象のタスクをキープしている TaskKeep</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeep<TYPE>::operator = (const TaskKeep<TYPE> & taskKeep)
	{
		_Reset_(taskKeep.getTaskPointer());
	}

	/// <summary>
	/// <para>─────────────</para>
	/// <para>タスクを消去、または解放します。</para>
	/// <para>─────────────</para>
	/// </summary>
	template<class TYPE>
	inline void TaskKeep<TYPE>::Clear()
	{
		_Reset_(nullptr);
	}

	/// <summary>
	/// <para>──────────────────────────────────</para>
	/// <para>タスクを消去せずに解放します。</para>
	/// <para>タスクは消去されないので、自身で Task::Destroy を呼び出して消去する必要があります。</para>
	/// <para>──────────────────────────────────</para>
	/// </summary>
	template<class TYPE>
	inline void TaskKeep<TYPE>::Free()
	{
		//タスクが存在しない場合は、実行しない
		if (mp_Task == nullptr) return;

		//ロックを解除する
		if (m_Safety) --mp_Task->m_Link;

		//タスク消去をしないまま、キープを切断する
		mp_Task = nullptr;
	}

	/// <summary>
	/// <para>──────────────────────────────────────────────</para>
	/// <para>タスクの消去防止機能であるキープカウントの有効無効を切り替えます。</para>
	/// <para>初期状態では自動で有効になっています。</para>
	/// <para>無効状態で Task::Destroy を行うとキープしているにも関わらず、消去が可能になります。</para>
	/// <para>また、無効にした TaskKeep では、タスクの消去ができなくなるため、注意が必要になります。</para>
	/// <para>Task::Destroy の説明にある「TaskKeep のタスクは消去できません。」を無効にする場合は、ここで false を設定します。</para>
	/// <para>──────────────────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="safety">
	/// <para>安全保障機能の有効無効切り替え</para>
	/// <para>true  -> 有効</para>
	/// <para>false -> 無効</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeep<TYPE>::Safety(const bool safety)
	{
		if (m_Safety && !safety)
		{
			//無効化
			m_Safety = false;

			//キープカウントを減らす
			if (mp_Task) --mp_Task->m_Link;
		}
		else if (!m_Safety && safety)
		{
			//有効化
			m_Safety = true;

			//キープカウントを増やす
			if (mp_Task) ++mp_Task->m_Link;
		}
	}

	/// <summary>
	/// <para>────────</para>
	/// <para>タスクを取得します。</para>
	/// <para>────────</para>
	/// </summary>
	template<class TYPE>
	inline TYPE* TaskKeep<TYPE>::operator -> () const
	{
		return mp_Task;
	}

	/// <summary>
	/// <para>────────</para>
	/// <para>タスクを取得します。</para>
	/// <para>────────</para>
	/// </summary>
	template<class TYPE>
	inline TYPE & TaskKeep<TYPE>::operator () () const
	{
		return *mp_Task;
	}

	/// <summary>
	/// <para>────────</para>
	/// <para>タスクを取得します。</para>
	/// <para>────────</para>
	/// </summary>
	template<class TYPE>
	inline TYPE & TaskKeep<TYPE>::getTask() const
	{
		return *mp_Task;
	}

	/// <summary>
	/// <para>───────────</para>
	/// <para>タスクポインタを取得します。</para>
	/// <para>───────────</para>
	/// </summary>
	template<class TYPE>
	inline TYPE* TaskKeep<TYPE>::getTaskPointer() const
	{
		return mp_Task;
	}

	/// <summary>
	/// <para>────────────</para>
	/// <para>タスクが存在するかを返します。</para>
	/// <para>────────────</para>
	/// </summary>
	template<class TYPE>
	inline TaskKeep<TYPE>::operator bool() const
	{
		return mp_Task != nullptr;
	}
}
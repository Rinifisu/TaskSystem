/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <cassert>			//assert
#include <unordered_map>	//std::unordered_map
#include <deque>			//std::deque

#include "rnfs_TaskKeep.h"

namespace rnfs
{
	/// <summary>
	/// <para>─────────────────────</para>
	/// <para>タスクキープ配列</para>
	/// <para>タスクを配列で持つための専用ポインタ配列です。</para>
	/// <para>ソート機能が無い代わりに、識別番号を使用します。</para>
	/// <para>─────────────────────</para>
	/// </summary>
	template<class TYPE = Task>
	class TaskKeepArray final
	{
	private:
		size_t										m_NextID;		//次に代入する unordered_map の識別番号

		std::unordered_map<size_t, TaskKeep<TYPE>>	m_Data;			//タスクキープ配列
		std::deque<size_t>							m_RegistID;		//登録済みの番号一覧
		std::deque<size_t>							m_DeleteID;		//消去済みの番号一覧

	public:
		TaskKeepArray();
		~TaskKeepArray() = default;

		TaskKeepArray(const TaskKeepArray<TYPE> & taskKeepArray) = default;
		TaskKeepArray(TaskKeepArray<TYPE> && taskKeepArray) = delete;

		typename std::unordered_map<size_t, TaskKeep<TYPE>>::iterator begin();
		typename std::unordered_map<size_t, TaskKeep<TYPE>>::iterator end();

		typename std::unordered_map<size_t, TaskKeep<TYPE>>::const_iterator begin() const;
		typename std::unordered_map<size_t, TaskKeep<TYPE>>::const_iterator end() const;

		void operator =(const TaskKeepArray<TYPE> & taskKeepArray) = delete;
		void operator =(TaskKeepArray<TYPE> && taskKeepArray) = delete;

		TYPE & operator () (const size_t number);
		TYPE & operator [] (const size_t number);

		TYPE & task_ID(const size_t number);
		TYPE & task(const size_t number);

		TYPE* taskPointer_ID(const size_t number);
		TYPE* taskPointer(const size_t number);

		void Add_Back(TYPE* p_Task);
		void Add_Front(TYPE* p_Task);
		void Insert(const size_t number, TYPE* p_Task);

		void Clear_ID(const size_t number);
		void Clear(const size_t number);

		void Clear_Back();
		void Clear_Front();
		void Clear_All();

		void Free_ID(const size_t number);
		void Free(const size_t number);

		void Free_Back();
		void Free_Front();
		void Free_All();

		void Safety_ID(const size_t number, const bool safety);
		void Safety(const size_t number, const bool safety);

		const size_t toID(const size_t number) const;
		const size_t toArrayNumber(const size_t number) const;

		const size_t nextID() const;

		const bool isID(const size_t number) const;
		const bool isEmpty() const;
		const size_t size() const;

		operator bool() const;
	};

	/// <summary>
	/// <para>─────</para>
	/// <para>コンストラクタ</para>
	/// <para>─────</para>
	/// </summary>
	template<class TYPE>
	inline TaskKeepArray<TYPE>::TaskKeepArray()
		: m_NextID(0)
	{

	}

	/// <summary>
	/// <para>────────────</para>
	/// <para>先頭のイテレータを返します。</para>
	/// <para>first -> 識別番号</para>
	/// <para>second -> タスクキープ</para>
	/// <para>────────────</para>
	/// </summary>
	template<class TYPE>
	inline typename std::unordered_map<size_t, TaskKeep<TYPE>>::iterator TaskKeepArray<TYPE>::begin()
	{
		return m_Data.begin();
	}

	/// <summary>
	/// <para>────────────</para>
	/// <para>末尾のイテレータを返します。</para>
	/// <para>first -> 識別番号</para>
	/// <para>second -> タスクキープ</para>
	/// <para>────────────</para>
	/// </summary>
	template<class TYPE>
	inline typename std::unordered_map<size_t, TaskKeep<TYPE>>::iterator TaskKeepArray<TYPE>::end()
	{
		return m_Data.end();
	}

	/// <summary>
	/// <para>────────────</para>
	/// <para>先頭のイテレータを返します。</para>
	/// <para>first -> 識別番号</para>
	/// <para>second -> タスクキープ</para>
	/// <para>────────────</para>
	/// </summary>
	template<class TYPE>
	inline typename std::unordered_map<size_t, TaskKeep<TYPE>>::const_iterator TaskKeepArray<TYPE>::begin() const
	{
		return m_Data.begin();
	}

	/// <summary>
	/// <para>────────────</para>
	/// <para>末尾のイテレータを返します。</para>
	/// <para>first -> 識別番号</para>
	/// <para>second -> タスクキープ</para>
	/// <para>────────────</para>
	/// </summary>
	template<class TYPE>
	inline typename std::unordered_map<size_t, TaskKeep<TYPE>>::const_iterator TaskKeepArray<TYPE>::end() const
	{
		return m_Data.end();
	}

	/// <summary>
	/// <para>─────────────────</para>
	/// <para>識別番号を使用して、タスクを取得します。</para>
	/// <para>─────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TYPE>
	inline TYPE & TaskKeepArray<TYPE>::operator () (const size_t number)
	{
		//実体が生成されないように参照する
		return m_Data[number].task();
	}

	/// <summary>
	/// <para>────────</para>
	/// <para>タスクを取得します。</para>
	/// <para>────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列番号</para>
	/// </param>
	template<class TYPE>
	inline TYPE & TaskKeepArray<TYPE>::operator [] (const size_t number)
	{
		//実体が生成されないように参照する
		return m_Data[m_RegistID[number]].task();
	}

	/// <summary>
	/// <para>─────────────────</para>
	/// <para>識別番号を使用して、タスクを取得します。</para>
	/// <para>─────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TYPE>
	inline TYPE & TaskKeepArray<TYPE>::task_ID(const size_t number)
	{
		//実体が生成されないように参照する
		return m_Data[number].task();
	}

	/// <summary>
	/// <para>────────</para>
	/// <para>タスクを取得します。</para>
	/// <para>────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列番号</para>
	/// </param>
	template<class TYPE>
	inline TYPE & TaskKeepArray<TYPE>::task(const size_t number)
	{
		//実体が生成されないように参照する
		return m_Data[m_RegistID[number]].task();
	}

	/// <summary>
	/// <para>────────────────────</para>
	/// <para>識別番号を使用して、タスクポインタを取得します。</para>
	/// <para>────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TYPE>
	inline TYPE* TaskKeepArray<TYPE>::taskPointer_ID(const size_t number)
	{
		return m_Data[number].taskPointer();
	}

	/// <summary>
	/// <para>───────────</para>
	/// <para>タスクポインタを取得します。</para>
	/// <para>───────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列番号</para>
	/// </param>
	template<class TYPE>
	inline TYPE* TaskKeepArray<TYPE>::taskPointer(const size_t number)
	{
		return m_Data[m_RegistID[number]].taskPointer();
	}

	/// <summary>
	/// <para>─────────────────</para>
	/// <para>タスクをキープし、配列の末尾に追加します。</para>
	/// <para>─────────────────</para>
	/// </summary>
	///
	/// <param name="p_Task">
	/// <para>キープ対象のタスク</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Add_Back(TYPE* p_Task)
	{
		//消去済み配列が空であれば
		if (m_DeleteID.empty())
		{
			//新規識別番号の位置にタスクをキープ
			m_Data[m_NextID] = p_Task;

			//新規識別番号を登録済み配列に追加
			m_RegistID.emplace_back(m_NextID);

			//識別番号を次に移動する
			++m_NextID;
		}
		//そうでなければ
		else
		{
			//消去済み識別番号の位置にタスクをキープ
			m_Data[m_DeleteID.front()] = p_Task;

			//消去済み識別番号を登録済み配列に追加
			m_RegistID.emplace_back(m_DeleteID.front());

			//消去済みではなくなったので、先頭を削除
			m_DeleteID.pop_front();
		}
	}

	/// <summary>
	/// <para>──────────────────</para>
	/// <para>タスクをキープし、配列の先頭に追加します。</para>
	/// <para>──────────────────</para>
	/// </summary>
	///
	/// <param name="p_Task">
	/// <para>キープ対象のタスク</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Add_Front(TYPE* p_Task)
	{
		if (m_DeleteID.empty())
		{
			//新規識別番号の位置にタスクをキープ
			m_Data[m_NextID] = p_Task;

			//新規識別番号を登録済み配列に追加
			m_RegistID.emplace_front(m_NextID);

			//識別番号を次に移動する
			++m_NextID;
		}
		//そうでなければ
		else
		{
			//消去済み識別番号の位置にタスクをキープ
			m_Data[m_DeleteID.front()] = p_Task;

			//消去済み識別番号を登録済み配列に追加
			m_RegistID.emplace_front(m_DeleteID.front());

			//消去済みではなくなったので、先頭を削除
			m_DeleteID.pop_front();
		}
	}

	/// <summary>
	/// <para>─────────────────</para>
	/// <para>タスクをキープし、配列の間に追加します。</para>
	/// <para>─────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>追加する配列番号位置</para>
	/// </param>
	///
	/// <param name="p_Task">
	/// <para>キープ対象のタスク</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Insert(const size_t number, TYPE * p_Task)
	{
		//消去済み配列が空であれば
		if (m_DeleteID.empty())
		{
			//新規識別番号の位置にタスクをキープ
			m_Data[m_NextID] = p_Task;

			//新規識別番号を登録済み配列に追加
			m_RegistID.emplace(m_RegistID.begin() + number, 1, m_NextID);

			//識別番号を次に移動する
			++m_NextID;
		}
		//そうでなければ
		else
		{
			//消去済み識別番号の位置にタスクをキープ
			m_Data[m_DeleteID.front()] = p_Task;

			//消去済み識別番号を登録済み配列に追加
			m_RegistID.insert(m_RegistID.begin() + number, 1, m_DeleteID.front());

			//消去済みではなくなったので、先頭を削除
			m_DeleteID.pop_front();
		}
	}

	/// <summary>
	/// <para>───────────────────</para>
	/// <para>識別番号を使用して、タスクキープを消去します。</para>
	/// <para>───────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Clear_ID(const size_t number)
	{
		//存在しない場合は終了
		if (m_Data.count(number) <= 0) return;

		//タスクキープの消去
		m_Data.erase(number);

		//消去済み配列に追加
		m_DeleteID.emplace_back(number);

		//登録済み配列から消去
		m_RegistID.erase(m_RegistID.begin() + toArrayNumber(number));
	}

	/// <summary>
	/// <para>───────────</para>
	/// <para>タスクキープを消去します。</para>
	/// <para>───────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列番号</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Clear(const size_t number)
	{
		//存在しない場合は終了
		if (m_Data.count(m_RegistID[number]) <= 0) return;

		//タスクキープの消去
		m_Data.erase(m_RegistID[number]);

		//消去済み配列に追加
		m_DeleteID.emplace_back(m_RegistID[number]);

		//登録済み配列から消去
		m_RegistID.erase(m_RegistID.begin() + number);
	}

	/// <summary>
	/// <para>──────────────────</para>
	/// <para>配列の末尾にあるタスクキープを消去します。</para>
	/// <para>──────────────────</para>
	/// </summary>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Clear_Back()
	{
		//存在しない場合は終了
		if (m_Data.empty()) return;

		//タスクキープの消去
		m_Data.erase(m_RegistID.back());

		//消去済み配列に追加
		m_DeleteID.emplace_back(m_RegistID.back());

		//登録済み配列から消去
		m_RegistID.erase(m_RegistID.end());
	}

	/// <summary>
	/// <para>──────────────────</para>
	/// <para>配列の先頭にあるタスクキープを消去します。</para>
	/// <para>──────────────────</para>
	/// </summary>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Clear_Front()
	{
		//存在しない場合は終了
		if (m_Data.empty()) return;

		//タスクキープの消去
		m_Data.erase(m_RegistID.front());

		//消去済み配列に追加
		m_DeleteID.emplace_back(m_RegistID.front());

		//登録済み配列から消去
		m_RegistID.erase(m_RegistID.begin());
	}

	/// <summary>
	/// <para>────────────</para>
	/// <para>タスクキープを全消去します。</para>
	/// <para>────────────</para>
	/// </summary>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Clear_All()
	{
		//タスク配列の全初期化
		m_Data.clear();

		//登録済み配列の全初期化
		m_RegistID.clear();

		//消去済み配列の全初期化
		m_DeleteID.clear();

		//識別番号を先頭の位置へ移動
		m_NextID = 0;
	}

	/// <summary>
	/// <para>───────────────────────────────────</para>
	/// <para>識別番号を使用して、タスクキープ配列からタスクを解放します。</para>
	/// <para>タスクは消去されないので、自身で Task::Destroy を呼び出して消去する必要があります。</para>
	/// <para>───────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Free_ID(const size_t number)
	{
		//存在しない場合は終了
		if (m_Data.count(number) <= 0) return;

		//タスクの解放
		m_Data[number].Free();

		//空のタスクキープの消去
		m_Data.erase(number);

		//消去済み配列に追加
		m_DeleteID.emplace_back(number);

		//登録済み配列から消去
		m_RegistID.erase(m_RegistID.begin() + toArrayNumber(number));
	}

	/// <summary>
	/// <para>───────────────────────────────────</para>
	/// <para>タスクキープ配列から解放します。</para>
	/// <para>タスクは消去されないので、自身で Task::Destroy を呼び出して消去する必要があります。</para>
	/// <para>───────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列番号</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Free(const size_t number)
	{
		//存在しない場合は終了
		if (m_Data.count(m_RegistID[number]) <= 0) return;

		//タスクの解放
		m_Data[m_RegistID[number]].Free();

		//空のタスクキープの消去
		m_Data.erase(m_RegistID[number]);

		//消去済み配列に追加
		m_DeleteID.emplace_back(m_RegistID[number]);

		//登録済み配列から消去
		m_RegistID.erase(m_RegistID.begin() + number);
	}

	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Free_Back()
	{
		//存在しない場合は終了
		if (m_Data.empty()) return;

		//タスクの解放
		m_Data[m_RegistID.back()].Free();

		//空のタスクキープの消去
		m_Data.erase(m_RegistID.back());

		//消去済み配列に追加
		m_DeleteID.emplace_back(m_RegistID.back());

		//登録済み配列から消去
		m_RegistID.erase(m_RegistID.end());
	}

	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Free_Front()
	{
		//存在しない場合は終了
		if (m_Data.empty()) return;

		//タスクの解放
		m_Data[m_RegistID.front()].Free();

		//空のタスクキープの消去
		m_Data.erase(m_RegistID.front());

		//消去済み配列に追加
		m_DeleteID.emplace_back(m_RegistID.front());

		//登録済み配列から消去
		m_RegistID.erase(m_RegistID.begin());
	}

	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Free_All()
	{
		//タスクの全解放
		for (auto & i : m_Data) i.second.Free();

		//タスク配列の全初期化
		m_Data.clear();

		//登録済み配列の全初期化
		m_RegistID.clear();

		//消去済み配列の全初期化
		m_DeleteID.clear();

		//識別番号を先頭の位置へ移動
		m_NextID = 0;
	}

	/// <summary>
	/// <para>───────────────────────────────</para>
	/// <para>識別番号を使用して、タスクキープの安全保障機能の有効無効を設定します。</para>
	/// <para>───────────────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	///
	/// <param name="safety">
	/// <para>安全保障機能の有効無効切り替え</para>
	/// <para>true  -> 有効</para>
	/// <para>false -> 無効</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Safety_ID(const size_t number, const bool safety)
	{
		//安全保障機能を設定
		m_Data[number].Safety(safety);
	}

	/// <summary>
	/// <para>──────────────────────</para>
	/// <para>タスクキープの安全保障機能の有効無効を設定します。</para>
	/// <para>──────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列番号</para>
	/// </param>
	///
	/// <param name="safety">
	/// <para>安全保障機能の有効無効切り替え</para>
	/// <para>true  -> 有効</para>
	/// <para>false -> 無効</para>
	/// </param>
	template<class TYPE>
	inline void TaskKeepArray<TYPE>::Safety(const size_t number, const bool safety)
	{
		//安全保障機能を設定
		m_Data[m_RegistID[number]].Safety(safety);
	}

	/// <summary>
	/// <para>──────────</para>
	/// <para>識別番号を取得します。</para>
	/// <para>──────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列番号</para>
	/// </param>
	template<class TYPE>
	inline const size_t TaskKeepArray<TYPE>::toID(const size_t number) const
	{
		return m_RegistID[number];
	}

	/// <summary>
	/// <para>───────────────────</para>
	/// <para>識別番号を使用して、配列番号を取得します。</para>
	/// <para>───────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TYPE>
	inline const size_t TaskKeepArray<TYPE>::toArrayNumber(const size_t number) const
	{
		//配列番号を調べる
		size_t count = 0;

		//登録済み配列の位置が、識別番号の順にはなっていないので探索する
		for (auto & i : m_RegistID)
		{
			//識別番号が一致した場合は現時点の配列番号を返す
			if (number == i) return count;
			//カウントアップ
			else ++count;
		}

		//識別番号が一致しない
		assert(!"TaskKeepArray -> 配列番号を取得できません。");

		//安全のため、先頭の要素を返す
		return 0;
	}

	/// <summary>
	/// <para>───────────────────────</para>
	/// <para>タスク追加で次に割り当てられる識別番号を取得します。</para>
	/// <para>───────────────────────</para>
	/// </summary>
	template<class TYPE>
	inline const size_t TaskKeepArray<TYPE>::nextID() const
	{
		return m_DeleteID.empty() ? m_NextID : m_DeleteID.front();
	}

	/// <summary>
	/// <para>────────────────────</para>
	/// <para>指定した識別番号のタスクの有無を確認します。</para>
	/// <para>────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TYPE>
	inline const bool TaskKeepArray<TYPE>::isID(const size_t number) const
	{
		return m_Data.count(number) != 0;
	}

	/// <summary>
	/// <para>─────────────</para>
	/// <para>空の状態であるかを取得します。</para>
	/// <para>─────────────</para>
	/// </summary>
	template<class TYPE>
	inline const bool TaskKeepArray<TYPE>::isEmpty() const
	{
		return m_RegistID.empty();
	}

	/// <summary>
	/// <para>─────────</para>
	/// <para>要素数を取得します。</para>
	/// <para>─────────</para>
	/// </summary>
	template<class TYPE>
	inline const size_t TaskKeepArray<TYPE>::size() const
	{
		return m_RegistID.size();
	}

	/// <summary>
	/// <para>────────────</para>
	/// <para>タスクが存在するかを返します。</para>
	/// <para>────────────</para>
	/// </summary>
	template<class TYPE>
	inline TaskKeepArray<TYPE>::operator bool() const
	{
		return !m_RegistID.empty();
	}
}
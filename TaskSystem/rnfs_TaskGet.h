/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <functional>

#include "rnfs_Task.h"

namespace rnfs
{
	/// <summary>
	/// <para>─────────────</para>
	/// <para>タスクゲット</para>
	/// <para>外部からタスクを呼び出せます。</para>
	/// <para>─────────────</para>
	/// </summary>
	class TaskGet
	{
	private:
		Task*			mp_Task;	//タスクのポインタ

		std::string		m_Name;		//識別用の名前
		size_t			m_ID;		//消去用の管理番号

	private:
		static std::unordered_map<std::string, TaskKeepArray<Task>> m_Data; //登録タスクのキープ

	private:
		//ゲットリストへの登録
		void _Register_();

		//ゲットリストから消去
		void _Unregister_();

	public:
		TaskGet();
		~TaskGet();

		template<class TASK>
		void Register(TASK* p_Task);

		/// <summary>
		/// <para>─────────────</para>
		/// <para>タスクゲットの解除を行います。</para>
		/// <para>─────────────</para>
		/// </summary>
		void Unregister();

		/// <summary>
		/// <para>─────────────────────────</para>
		/// <para>識別番号を取得します。</para>
		/// <para>配列の変化に対応できるタスク取得が識別番号で行えます。</para>
		/// <para>─────────────────────────</para>
		/// </summary>
		const size_t id();

	public:
		template<class TASK>
		static TASK & get(const size_t arrayNumber = 0);

		template<class TASK>
		static TASK & get_ID(const size_t idNumber = 0);

		template<class TASK>
		static const size_t toID(const size_t number);

		template<class TASK>
		static const size_t toArrayNumber(const size_t number);

		template<class TASK>
		static const bool isID(const size_t number);

		template<class TASK>
		static const size_t size();
	};

	/// <summary>
	/// <para>───────────────────────────────</para>
	/// <para>タスクゲットの登録を行います。</para>
	/// <para>テンプレート引数を使用することで、継承元クラスに登録可能になります。</para>
	/// <para>───────────────────────────────</para>
	/// <para>登録を行うことで TaskGet::get で指定したタスクが配列形式で呼び出せます。</para>
	/// <para>───────────────────────────────</para>
	/// </summary>
	///
	/// <param name="p_Task">
	/// <para>自身のポインタ</para>
	/// <para>必ず this を入力してください。</para>
	/// <para>タスク識別用の名前を取得するため、テンプレートになっています。</para>
	/// </param>
	template<class TASK>
	inline void TaskGet::Register(TASK* p_Task)
	{
		//登録されていたら登録解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = p_Task;
		m_Name = typeid(TASK).name();

		//登録
		this->_Register_();
	}

	/// <summary>
	/// <para>───────────────────────</para>
	/// <para>TaskGet::Register で登録済みのタスクを取得します。</para>
	/// <para>テンプレート引数を使用します。＜タスク名＞</para>
	/// <para>───────────────────────</para>
	/// </summary>
	///
	/// <param name="arrayNumber">
	/// <para>配列番号</para>
	/// </param>
	template<class TASK>
	inline TASK & TaskGet::get(const size_t arrayNumber)
	{
		return *dynamic_cast<TASK*>(m_Data.at(typeid(TASK).name()).getTaskPointer(arrayNumber));
	}

	/// <summary>
	/// <para>───────────────────────────────</para>
	/// <para>TaskGet::Register で登録済みのタスクを、識別番号を使用して取得します。</para>
	/// <para>テンプレート引数を使用します。＜タスク名＞</para>
	/// <para>───────────────────────────────</para>
	/// </summary>
	///
	/// <param name="idNumber">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TASK>
	inline TASK & TaskGet::get_ID(const size_t idNumber)
	{
		return *dynamic_cast<TASK*>(m_Data.at(typeid(TASK).name()).getTaskPointer_ID(idNumber));
	}

	/// <summary>
	/// <para>────────────────────────────────────</para>
	/// <para>TaskGet::Register で登録済みのタスクの識別番号を、配列番号を使用して取得します。</para>
	/// <para>テンプレート引数を使用します。＜タスク名＞</para>
	/// <para>────────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列番号</para>
	/// </param>
	template<class TASK>
	inline const size_t TaskGet::toID(const size_t number)
	{
		return m_Data.at(typeid(TASK).name()).toID(number);
	}

	/// <summary>
	/// <para>────────────────────────────────────</para>
	/// <para>TaskGet::Register で登録済みのタスクの配列番号を、識別番号を使用して取得します。</para>
	/// <para>テンプレート引数を使用します。＜タスク名＞</para>
	/// <para>────────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TASK>
	inline const size_t TaskGet::toArrayNumber(const size_t number)
	{
		return m_Data.at(typeid(TASK).name()).toArrayNumber(number);
	}

	/// <summary>
	/// <para>─────────────────────────────────</para>
	/// <para>指定した識別番号の TaskGet::Register で登録済みタスクの有無を確認します。</para>
	/// <para>テンプレート引数を使用します。＜タスク名＞</para>
	/// <para>─────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="number">
	/// <para>配列の識別番号</para>
	/// </param>
	template<class TASK>
	inline const bool TaskGet::isID(const size_t number)
	{
		//登録済みのタスクが１つも無い場合は存在しない
		if (m_Data.count(typeid(TASK).name()) <= 0) return false;
		//要素数の取得
		return m_Data.at(typeid(TASK).name()).isID(number);
	}

	/// <summary>
	/// <para>────────────────────────</para>
	/// <para>TaskGet::Register で登録済みのクラス数を取得します。</para>
	/// <para>テンプレート引数を使用します。＜タスク名＞</para>
	/// <para>────────────────────────</para>
	/// </summary>
	template<class TASK>
	inline const size_t TaskGet::size()
	{
		//登録済みのタスクが１つも無い場合は０を返す
		if (m_Data.count(typeid(TASK).name()) <= 0) return 0;
		//要素数の取得
		else return m_Data.at(typeid(TASK).name()).size();
	}
}
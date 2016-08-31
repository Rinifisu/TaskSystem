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
	template<class TYPE = std::unordered_map<std::type_index, TaskKeepArray<Task>>>
	class _TaskGet_
	{
	protected:
		static TYPE	m_Data; //登録タスクのキープ
	};

	template<class TYPE>
	TYPE _TaskGet_<TYPE>::m_Data;

	///<summary>
	///<para>─────────────</para>
	///<para>タスクゲット</para>
	///<para>外部からタスクを呼び出せます。</para>
	///<para>─────────────</para>
	///</summary>
	class TaskGet final : public _TaskGet_<>
	{
	private:
		Task*				mp_Task;	//タスクのポインタ

		std::type_index		m_Type;		//識別用の情報
		TaskID				m_ID;		//消去用の管理番号

	private:
		//ゲットリストへの登録
		void _Register_()
		{
			//追加される位置を取得する
			m_ID = m_Data[m_Type].nextID();

			//追加
			m_Data[m_Type].Keep_Back(mp_Task);

			//カウントを無効にする
			m_Data[m_Type].Safety_ID(m_ID, false);
		}

		//ゲットリストから消去
		void _Unregister_()
		{
			//タスクの解放
			m_Data[m_Type].Free_ID(m_ID);

			//リストが空になったら、消去する
			if (m_Data[m_Type].isEmpty()) m_Data.erase(m_Type);
		}

	public:
		///<summary>
		///<para>────────</para>
		///<para>初期化を行います。</para>
		///<para>────────</para>
		///</summary>
		TaskGet()
			: mp_Task(nullptr), m_Type(typeid(nullptr)), m_ID(0)
		{

		}
		
		template<class TASK>
		TaskGet(TASK* p_Task);

		///<summary>
		///<para>──────────────</para>
		///<para>タスクゲットの登録解除を行います。</para>
		///<para>──────────────</para>
		///</summary>
		~TaskGet()
		{
			if (mp_Task) this->_Unregister_();
		}

		TaskGet(const TaskGet & taskGet) = delete;
		TaskGet(TaskGet && taskGet) = delete;
		void operator =(const TaskGet & taskGet) = delete;
		void operator =(TaskGet && taskGet) = delete;

		template<class TASK>
		void Register(TASK* p_Task);

		///<summary>
		///<para>──────────────────</para>
		///<para>タスクゲットの登録が行われるかを確認します。</para>
		///<para>──────────────────</para>
		///</summary>
		const bool isRegister() const
		{
			return mp_Task != nullptr;
		}

		///<summary>
		///<para>──────────────</para>
		///<para>タスクゲットの登録解除を行います。</para>
		///<para>──────────────</para>
		///</summary>
		void Unregister()
		{
			//リストから登録を解除する
			if (mp_Task) this->_Unregister_();

			//初期化
			mp_Task = nullptr;
			m_Type = typeid(nullptr);
			m_ID = 0;
		}

		///<summary>
		///<para>────────────────────────</para>
		///<para>識別番号を取得します。</para>
		///<para>配列の変化に対応できるタスク取得が識別番号で行えます。</para>
		///<para>────────────────────────</para>
		///</summary>
		const TaskID id() const
		{
			return m_ID;
		}

		///<summary>
		///<para>────────────</para>
		///<para>全体の処理を行う空間です。</para>
		///<para>────────────</para>
		///</summary>
		class All
		{
		public:
			template<class TASK>
			static TASK & task_ID(const TaskID id);
			template<class TASK>
			static TASK & task(const size_t arrayNumber);

			template<class TASK>
			static TASK & back();
			template<class TASK>
			static TASK & front();

			template<class TASK>
			static const size_t toArrayNumber(const TaskID id);
			template<class TASK>
			static const TaskID toID(const size_t arrayNumber);

			template<class TASK>
			static const bool isID(const TaskID id);
			template<class TASK>
			static const bool isEmpty();
			template<class TASK>
			static const size_t size();

			template<class TASK>
			static const TaskID backID();
			template<class TASK>
			static const TaskID frontID();

			template<class TASK>
			static const bool clear(const size_t arrayNumber);
			template<class TASK>
			static const bool clear_ID(const TaskID id);

			template<class TASK>
			static const bool clear_Back();
			template<class TASK>
			static const bool clear_Front();
			template<class TASK>
			static const size_t clear_All();
		};
	};

	///<summary>
	///<para>───────────────────────────────</para>
	///<para>タスクゲットの登録を行います。</para>
	///<para>───────────────────────────────</para>
	///<para>登録を行うことで TaskGet::task で指定したタスクが配列形式で呼び出せます。</para>
	///<para>───────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///<para>タスク識別用の名前を取得するため、テンプレートになっています。</para>
	///</param>
	template<class TASK>
	inline TaskGet::TaskGet(TASK* p_Task)
		: mp_Task(p_Task), m_Type(typeid(TASK)), m_ID(0)
	{
		this->_Register_();
	}

	///<summary>
	///<para>───────────────────────────────</para>
	///<para>タスクゲットの登録を行います。</para>
	///<para>テンプレート引数を使用することで、継承元クラスに登録可能になります。</para>
	///<para>───────────────────────────────</para>
	///<para>登録を行うことで TaskGet::task で指定したタスクが配列形式で呼び出せます。</para>
	///<para>───────────────────────────────</para>
	///</summary>
	///
	///<param name="p_Task">
	///<para>自身のポインタ</para>
	///<para>必ず this を入力してください。</para>
	///<para>タスク識別用の名前を取得するため、テンプレートになっています。</para>
	///</param>
	template<class TASK>
	inline void TaskGet::Register(TASK* p_Task)
	{
		//登録されていたら登録解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = p_Task;
		m_Type = typeid(TASK);

		//登録
		this->_Register_();
	}

	///<summary>
	///<para>──────────────────────────────</para>
	///<para>TaskGet::Register で登録済みのタスクを、識別番号を使用して取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>──────────────────────────────</para>
	///</summary>
	///
	///<param name="id">
	///<para>配列の識別番号</para>
	///</param>
	template<class TASK>
	inline TASK & TaskGet::All::task_ID(const TaskID id)
	{
		return *dynamic_cast<TASK*>(m_Data.at(typeid(TASK)).taskPointer_ID(id));
	}

	///<summary>
	///<para>─────────────────────</para>
	///<para>TaskGet::Register で登録済みのタスクを取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>─────────────────────</para>
	///</summary>
	///
	///<param name="arrayNumber">
	///<para>配列番号</para>
	///</param>
	template<class TASK>
	inline TASK & TaskGet::All::task(const size_t arrayNumber)
	{
		return *dynamic_cast<TASK*>(m_Data.at(typeid(TASK)).taskPointer(arrayNumber));
	}

	///<summary>
	///<para>───────────────────────</para>
	///<para>TaskGet::Register で登録済みの末尾タスクを取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>───────────────────────</para>
	///</summary>
	template<class TASK>
	inline TASK & TaskGet::All::back()
	{
		return *dynamic_cast<TASK*>(m_Data.at(typeid(TASK)).backPointer());
	}

	///<summary>
	///<para>───────────────────────</para>
	///<para>TaskGet::Register で登録済みの先頭タスクを取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>───────────────────────</para>
	///</summary>
	template<class TASK>
	inline TASK & TaskGet::All::front()
	{
		return *dynamic_cast<TASK*>(m_Data.at(typeid(TASK)).frontPointer());
	}

	///<summary>
	///<para>───────────────────────────────────</para>
	///<para>TaskGet::Register で登録済みのタスクの配列番号を、識別番号を使用して取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>───────────────────────────────────</para>
	///</summary>
	///
	///<param name="id">
	///<para>配列の識別番号</para>
	///</param>
	template<class TASK>
	inline const size_t TaskGet::All::toArrayNumber(const TaskID id)
	{
		return m_Data.at(typeid(TASK)).toArrayNumber(id);
	}

	///<summary>
	///<para>───────────────────────────────────</para>
	///<para>TaskGet::Register で登録済みのタスクの識別番号を、配列番号を使用して取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>───────────────────────────────────</para>
	///</summary>
	///
	///<param name="arrayNumber">
	///<para>配列番号</para>
	///</param>
	template<class TASK>
	inline const TaskID TaskGet::All::toID(const size_t arrayNumber)
	{
		return m_Data.at(typeid(TASK)).toID(arrayNumber);
	}

	///<summary>
	///<para>─────────────────────────────────</para>
	///<para>指定した識別番号の TaskGet::Register で登録済みのタスクの有無を確認します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>─────────────────────────────────</para>
	///</summary>
	///
	///<param name="id">
	///<para>配列の識別番号</para>
	///</param>
	template<class TASK>
	inline const bool TaskGet::All::isID(const TaskID id)
	{
		//登録済みのタスクが１つも無い場合は存在しない
		if (m_Data.count(typeid(TASK)) <= 0) return false;
		//要素数の取得
		return m_Data.at(typeid(TASK)).isID(id);
	}

	///<summary>
	///<para>──────────────────────────────</para>
	///<para>TaskGet::Register で登録が全く行われていない状態であるかを取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>──────────────────────────────</para>
	///</summary>
	template<class TASK>
	inline const bool TaskGet::All::isEmpty()
	{
		return m_Data.count(typeid(TASK)) <= 0;
	}

	///<summary>
	///<para>──────────────────────</para>
	///<para>TaskGet::Register で登録済みのタスク数を取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>──────────────────────</para>
	///</summary>
	template<class TASK>
	inline const size_t TaskGet::All::size()
	{
		//登録済みのタスクが１つも無い場合は０を返す
		if (m_Data.count(typeid(TASK)) <= 0) return 0;
		//要素数の取得
		else return m_Data.at(typeid(TASK)).size();
	}

	///<summary>
	///<para>────────────────────────────</para>
	///<para>TaskGet::Register で登録済みの末尾タスクの識別番号を取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>────────────────────────────</para>
	///</summary>
	template<class TASK>
	inline const TaskID TaskGet::All::backID()
	{
		return m_Data.at(typeid(TASK)).backID();
	}

	///<summary>
	///<para>────────────────────────────</para>
	///<para>TaskGet::Register で登録済みの先頭タスクの識別番号を取得します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>────────────────────────────</para>
	///</summary>
	template<class TASK>
	inline const TaskID TaskGet::All::frontID()
	{
		return m_Data.at(typeid(TASK)).frontID();
	}

	///<summary>
	///<para>─────────────────────</para>
	///<para>TaskGet::Register で登録済みのタスクを消去します。</para>
	///<para>キープ中のタスクは消去できません。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>─────────────────────</para>
	///</summary>
	///
	///<param name="arrayNumber">
	///<para>配列番号</para>
	///</param>
	template<class TASK>
	inline const bool TaskGet::All::clear(const size_t arrayNumber)
	{
		//登録済みのタスクが１つも無い場合は何もしない
		if (m_Data.count(typeid(TASK)) <= 0) return false;

		//一時的に参照
		TaskKeepArray<Task> & taskKeepArray = m_Data.at(typeid(TASK));
		//キープしているタスクがある場合は何もせずに終了
		if (0 < taskKeepArray.task(arrayNumber).link()) return false;

		//カウントを有効にして消去する
		taskKeepArray.Safety(arrayNumber, true);
		taskKeepArray.Clear(arrayNumber);

		return true;
	}

	///<summary>
	///<para>──────────────────────────────</para>
	///<para>指定した識別番号の TaskGet::Register で登録済みのタスクを消去します。</para>
	///<para>キープ中のタスクは消去できません。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>──────────────────────────────</para>
	///</summary>
	///
	///<param name="id">
	///<para>配列の識別番号</para>
	///</param>
	template<class TASK>
	inline const bool TaskGet::All::clear_ID(const TaskID id)
	{
		//登録済みのタスクが１つも無い場合は何もしない
		if (m_Data.count(typeid(TASK)) <= 0) return false;

		//一時的に参照
		TaskKeepArray<Task> & taskKeepArray = m_Data.at(typeid(TASK));
		//キープしているタスクがある場合は何もせずに終了
		if (0 < taskKeepArray.task_ID(id).link()) return false;

		//カウントを有効にして消去する
		taskKeepArray.Safety_ID(id, true);
		taskKeepArray.Clear_ID(id);

		return true;
	}

	///<summary>
	///<para>────────────────────────────────</para>
	///<para>指定した識別番号の TaskGet::Register で登録済みの末尾タスクを消去します。</para>
	///<para>キープ中のタスクは消去できません。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>────────────────────────────────</para>
	///</summary>
	template<class TASK>
	inline const bool TaskGet::All::clear_Back()
	{
		//登録済みのタスクが１つも無い場合は何もしない
		if (m_Data.count(typeid(TASK)) <= 0) return false;

		//一時的に参照
		TaskKeepArray<Task> & taskKeepArray = m_Data.at(typeid(TASK));
		//キープしているタスクがある場合は何もせずに終了
		if (0 < taskKeepArray.back().link()) return false;

		//カウントを有効にして消去する
		taskKeepArray.Safety_ID(taskKeepArray.backID(), true);
		taskKeepArray.Clear_Back();

		return true;
	}

	///<summary>
	///<para>────────────────────────────────</para>
	///<para>指定した識別番号の TaskGet::Register で登録済みの先頭タスクを消去します。</para>
	///<para>キープ中のタスクは消去できません。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>────────────────────────────────</para>
	///</summary>
	template<class TASK>
	inline const bool TaskGet::All::clear_Front()
	{
		//登録済みのタスクが１つも無い場合は何もしない
		if (m_Data.count(typeid(TASK)) <= 0) return false;

		//一時的に参照
		TaskKeepArray<Task> & taskKeepArray = m_Data.at(typeid(TASK));
		//キープしているタスクがある場合は何もせずに終了
		if (0 < taskKeepArray.front().link()) return false;

		//カウントを有効にして消去する
		taskKeepArray.Safety_ID(taskKeepArray.frontID(), true);
		taskKeepArray.Clear_Front();

		return true;
	}

	///<summary>
	///<para>───────────────────────</para>
	///<para>TaskGet::Register で登録済みのタスクを全て消去します。</para>
	///<para>キープ中のタスクは消去できません。</para>
	///<para>消去に成功したタスク数が返されます。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>───────────────────────</para>
	///</summary>
	template<class TASK>
	inline const size_t TaskGet::All::clear_All()
	{
		//登録済みのタスクが１つも無い場合は何もしない
		if (m_Data.count(typeid(TASK)) <= 0) return 0;

		//消去したタスクの数
		size_t deleteTask = 0;
		//一時的に参照
		TaskKeepArray<Task> & taskKeepArray = m_Data.at(typeid(TASK));

		for (size_t i = 0; i < taskKeepArray.size();)
		{
			//キープしているタスクがある場合は何もせずに次へ
			if (0 < taskKeepArray.task(i).link()) ++i;
			else
			{
				//カウントを有効にして消去する
				taskKeepArray.Safety(i, true);
				taskKeepArray.Clear(i);
				++deleteTask;
			}
		}

		return deleteTask;
	}
}
/*
The MIT License (MIT)
Copyright © 2015-2017 Rinifisu
https://twitter.com/Rinifisu
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <cassert>		//assert
#include <Windows.h>	//LARGE_INTEGER

namespace rnfs
{
	//時間を計測するクラス
	class _TaskTime_ final
	{
	private:
		LARGE_INTEGER			m_Buf;	//開始時のカウント時間
		mutable LARGE_INTEGER	m_Pos;	//現在のカウント時間
		LARGE_INTEGER			m_Freq;	//周波数

	public:
		_TaskTime_()
		{
			//システムの1秒間の周波数を取得
			QueryPerformanceFrequency(&m_Freq);

			//カウント数を取得
			QueryPerformanceCounter(&m_Buf);
		}

		float get() const
		{
			//カウント数を取得
			QueryPerformanceCounter(&m_Pos);

			//差分カウント数を、1秒間のカウント数で割って、現在の時間に変換する
			return static_cast<float>(m_Pos.QuadPart - m_Buf.QuadPart) / static_cast<float>(m_Freq.QuadPart);
		}
	};

	//ソースファイル生成回避宣言
	template<class TYPE = Task*>
	class _Task_
	{
	protected:
		static TYPE mp_Begin;	//先頭ポインタ
		static TYPE mp_End;		//末尾ポインタ
	};

	template<class TYPE>
	TYPE _Task_<TYPE>::mp_Begin = nullptr;
	template<class TYPE>
	TYPE _Task_<TYPE>::mp_End = nullptr;

	enum class TaskDestroyMode
	{
		None,		//何もせず、Task::Destroy を呼び出して消去する必要があります。
		Destroy,	//Task::All::Update が呼び出された時に消去されます。
		Count,		//経過したフレーム数(Task::Update が呼び出された回数)で消去されます。
		Time,		//経過した時間で消去されます。
	};

	///<summary>
	///<para>─────────────────</para>
	///<para>タスク</para>
	///<para>オブジェクトを自動管理することができます。</para>
	///<para>─────────────────</para>
	///</summary>
	class Task : public _Task_<>
	{
		template<class TYPE>
		friend class	TaskKeep;	//保管や消去で必要

	private:
		Task*			mp_Prev;	//自身の前ポインタ
		Task*			mp_Next;	//自身の後ポインタ

		TaskDestroyMode	m_Mode;		//消去手段
		size_t			m_Count;	//Task::Update が呼び出された回数
		_TaskTime_		m_Time;		//経過した時間
		float			m_Target;	//目標数

		size_t			m_Link;		//TaskKeep の接続確認

	private:
		//システムへの登録
		static inline void _Register_(Task* p_Task)
		{
			//先頭が空の場合は新規タスクを設定
			if (mp_Begin == nullptr) mp_Begin = p_Task;
			else
			{
				//新規タスクの前に末尾タスクを代入
				p_Task->mp_Prev = mp_End;

				//末尾タスクの次に新規タスクを代入
				mp_End->mp_Next = p_Task;
			}

			//末尾タスクが新規タスクになる
			mp_End = p_Task;
		}

		//システムから消去　次のポインタが返される
		static inline Task* _Unregister_(Task* p_Task)
		{
			Task* next = p_Task->mp_Next; //自身の次
			Task* prev = p_Task->mp_Prev; //自身の前

			//次位置に前位置を代入してつなぎ合わせる
			if (next != nullptr) next->mp_Prev = prev;
			//null の場合は末尾タスクがなくなったので、前のタスクを末尾にする
			else mp_End = prev;

			//前に次を代入してつなぎ合わせる
			if (prev != nullptr) prev->mp_Next = next;
			//null の場合は先頭タスクがなくなったので、次のタスクを先頭にする
			else mp_Begin = next;

			//タスクの消去
			delete p_Task;

			//次のタスクを返す
			return next;
		}

	protected:
		///<summary>
		///<para>───────────</para>
		///<para>システムに登録を行います。</para>
		///<para>───────────</para>
		///</summary>
		Task() : mp_Prev(nullptr), mp_Next(nullptr), m_Mode(TaskDestroyMode::None), m_Count(0), m_Target(0.0f), m_Link(0)
		{
			Task::_Register_(this);
		}

		template<typename TYPE = float>
		Task(const TaskDestroyMode & mode, const TYPE & destroyValue = 0.0f);

		//継承クラスのデストラクタが呼ばれる
		virtual ~Task() = default;

	public:
		Task(const Task & task) = delete;
		Task(Task && task) = delete;
		void operator =(const Task & task) = delete;
		void operator =(Task && task) = delete;

		///<summary>
		///<para>───────────────</para>
		///<para>消去フラグを立てます。</para>
		///<para>保管中のタスクは消去できません。</para>
		///<para>───────────────</para>
		///<para>次の Task::All::Update 呼び出し時に</para>
		///<para>タスクの消去が行われます。</para>
		///<para>───────────────</para>
		///</summary>
		virtual void Destroy() final
		{
			//保管しているタスクは消去できない
			if (0 < m_Link) return;

			//次の更新で消去するように設定
			m_Mode = TaskDestroyMode::Destroy;
		}

		template<typename TYPE = float>
		void SetAutoDestroy(const TaskDestroyMode & mode, const TYPE & destroyValue = 0.0f);

		///<summary>
		///<para>────────────────</para>
		///<para>消去フラグが立ててあるかを確認します。</para>
		///<para>────────────────</para>
		///</summary>
		virtual bool isDestroy() const final
		{
			return m_Mode == TaskDestroyMode::Destroy;
		}

		///<summary>
		///<para>─────────────────────────────────────</para>
		///<para>タスクが生成されてから経過したフレーム数(Task::Update が呼び出された回数)を取得します。</para>
		///<para>─────────────────────────────────────</para>
		///</summary>
		virtual size_t count() const final
		{
			return m_Count;
		}

		///<summary>
		///<para>────────────────────────────────────</para>
		///<para>タスクが消去されるまでの残りフレーム数(Task::Update が呼び出された回数)を取得します。</para>
		///<para>TaskDestroyMode::Count で自動消去設定を行う必要があります。</para>
		///<para>設定が不適切であれば 0 が返されます。</para>
		///<para>────────────────────────────────────</para>
		///</summary>
		virtual size_t remainingCount() const final
		{
			return m_Mode == TaskDestroyMode::Count ? static_cast<size_t>(m_Target) - m_Count : 0;
		}

		///<summary>
		///<para>─────────────────────</para>
		///<para>タスクが生成されてから経過した時間を取得します。</para>
		///<para>─────────────────────</para>
		///</summary>
		virtual float time() const final
		{
			return m_Time.get();
		}

		///<summary>
		///<para>───────────────────────────</para>
		///<para>タスクが消去されるまでの残り時間を取得します。</para>
		///<para>TaskDestroyMode::Time で自動消去設定を行う必要があります。</para>
		///<para>設定が不適切であれば 0.0f が返されます。</para>
		///<para>───────────────────────────</para>
		///</summary>
		virtual float remainingTime() const final
		{
			return m_Mode == TaskDestroyMode::Time ? m_Target - m_Time.get() : 0.0f;
		}

		///<summary>
		///<para>────────────────────</para>
		///<para>TaskKeep の保管数を取得します。</para>
		///<para>保管されている場合は、1 以上の値が返されます。</para>
		///<para>────────────────────</para>
		///</summary>
		virtual size_t link() const final
		{
			return m_Link;
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
			///<para>────────────────</para>
			///<para>登録されているタスクを全て消去します。</para>
			///<para>保管中のタスクは消去されません。</para>
			///<para>────────────────</para>
			///</summary>
			static inline void Clear()
			{
				Task* p_Task = mp_Begin; //現在のタスク

				//末尾までループする
				while (p_Task != nullptr)
				{
					//タスクを消去し、次のタスクへ移動
					//保管しているタスクは消去できない
					if (p_Task->m_Link <= 0) p_Task = Task::_Unregister_(p_Task);
					else p_Task = p_Task->mp_Next;
				}
			}

			///<summary>
			///<para>─────────────────</para>
			///<para>消去フラグ有効のタスクを全て消去します。</para>
			///<para>─────────────────</para>
			///</summary>
			static inline void Update()
			{
				Task* p_Task = mp_Begin; //現在のタスク

				//末尾までループする
				while (p_Task != nullptr)
				{
					//呼び出された回数を加算
					++p_Task->m_Count;

					//次のタスクを事前取得
					Task* p_next = p_Task->mp_Next;

					//タスクの消去
					switch (p_Task->m_Mode)
					{
					case TaskDestroyMode::None:
						//無し
						break;

					case TaskDestroyMode::Destroy:
						//Task::All::Update が呼び出された時に消去
						Task::_Unregister_(p_Task);
						break;

					case TaskDestroyMode::Count:
						//経過したフレーム数(Task::Update が呼び出された回数)で消去
						if (static_cast<size_t>(p_Task->m_Target) <= p_Task->m_Count) Task::_Unregister_(p_Task);
						break;

					case TaskDestroyMode::Time:
						//経過した時間で消去
						if (p_Task->m_Target <= p_Task->m_Time.get()) Task::_Unregister_(p_Task);
						break;

					default:
						assert(!"Task -> TaskDestroyMode の設定が不正です。");
						break;
					}

					//次のタスクへ移動
					p_Task = p_next;
				}
			}
		};
	};

	///<summary>
	///<para>──────────────────</para>
	///<para>システムに登録を行います。</para>
	///<para>タスクの消去方法を設定することができます。</para>
	///<para>──────────────────</para>
	///</summary>
	///
	///<param name="mode">
	///<para>タスクの消去方法</para>
	///</param>
	///
	///<param name="destroyValue">
	///<para>─────────────────────────────</para>
	///<para>手前の引数(mode)で設定した消去方法により、変化します。</para>
	///<para>─────────────────────────────</para>
	///<para>TaskDestroyMode::Count</para>
	///<para>消去フレーム数(消去呼び出し回数)</para>
	///<para>─────────────────────────────</para>
	///<para>TaskDestroyMode::Time</para>
	///<para>消去時間(秒)</para>
	///<para>─────────────────────────────</para>
	///<para>型変換の省略とデフォルト引数の設定のため、テンプレートになっています。</para>
	///<para>─────────────────────────────</para>
	///</param>
	template<typename TYPE>
	inline Task::Task(const TaskDestroyMode & mode, const TYPE & destroyValue)
		: mp_Prev(nullptr), mp_Next(nullptr), m_Mode(mode), m_Count(0), m_Target(static_cast<float>(destroyValue)), m_Link(0)
	{
		Task::_Register_(this);
	}

	///<summary>
	///<para>─────────────────────────</para>
	///<para>タスクの消去方法を設定します。</para>
	///<para>タスクが生成されてからの経過設定になる為、差分が必要です。</para>
	///<para>─────────────────────────</para>
	///</summary>
	///
	///<param name="mode">
	///<para>タスクの消去方法</para>
	///</param>
	///
	///<param name="destroyValue">
	///<para>─────────────────────────────</para>
	///<para>手前の引数(mode)で設定した消去方法により、変化します。</para>
	///<para>─────────────────────────────</para>
	///<para>TaskDestroyMode::Count</para>
	///<para>消去フレーム数(消去呼び出し回数)</para>
	///<para>─────────────────────────────</para>
	///<para>TaskDestroyMode::Time</para>
	///<para>消去時間(秒)</para>
	///<para>─────────────────────────────</para>
	///<para>型変換の省略とデフォルト引数の設定のため、テンプレートになっています。</para>
	///<para>─────────────────────────────</para>
	///</param>
	template<typename TYPE>
	inline void Task::SetAutoDestroy(const TaskDestroyMode & mode, const TYPE & destroyValue)
	{
		m_Mode = mode;
		m_Target = destroyValue;
	}

	///<summary>
	///<para>───────────────────────────────────────────────────────</para>
	///<para>タスクを生成します。</para>
	///<para>new タスク名; のタスク生成も可能ですが、delete 記述を行わない事による違和感やルール違反を避ける為、この関数の使用を推奨します。</para>
	///<para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	///<para>───────────────────────────────────────────────────────</para>
	///<para>　推奨：Create&lt;タスク名&gt;(引数);</para>
	///<para>非推奨：new タスク名(引数);</para>
	///<para>───────────────────────────────────────────────────────</para>
	///</summary>
	///
	///<param name="args">
	///<para>コンストラクタの引数</para>
	///</param>
	template<class TYPE, typename ... ARGS>
	static inline TYPE* Create(ARGS && ... args)
	{
		return new TYPE(std::forward<ARGS>(args) ...);
	}
}
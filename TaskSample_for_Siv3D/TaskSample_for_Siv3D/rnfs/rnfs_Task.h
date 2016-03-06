//Copyright © 2015-2016 Rinifisu
//http://rinifisu.blog.jp/

#pragma once

#include "rnfs_TaskKeepArray.h"

namespace rnfs
{
	/// <summary>
	/// <para>─────────────────</para>
	/// <para>タスク</para>
	/// <para>オブジェクトを自動管理することができます。</para>
	/// <para>─────────────────</para>
	/// </summary>
	class Task
	{
		template<class TYPE>
		friend class	TaskKeep;		//キープや消去で必要

	private:
		Task*			mp_Prev;		//自身の前ポインタ
		Task*			mp_Next;		//自身の後ポインタ

		bool			m_Destroy;		//消去フラグ

		size_t			m_Link;			//TaskKeep の接続確認

	private:
		static Task*	mp_Begin;		//先頭ポインタ
		static Task*	mp_End;			//末尾ポインタ

	private:
		//システムから消去　次のポインタが返される
		static Task* _Unregister_(Task* p_Task);

	protected:
		/// <summary>
		/// <para>───────────</para>
		/// <para>システムに登録を行います。</para>
		/// <para>───────────</para>
		/// </summary>
		Task();

		//継承クラスのデストラクタが呼ばれる
		virtual ~Task() = default;

		/// <summary>
		/// <para>─────────────────</para>
		/// <para>消去フラグを立てます。</para>
		/// <para>キープ中のタスクは消去できません。</para>
		/// <para>─────────────────</para>
		/// <para>次の Task::System::Update 呼び出し時に</para>
		/// <para>タスクの消去が行われます。</para>
		/// <para>─────────────────</para>
		/// </summary>
		virtual void Destroy() final;

		/// <summary>
		/// <para>────────────────────</para>
		/// <para>キープ数を取得します。</para>
		/// <para>キープされている場合は、1 以上の値が返されます。</para>
		/// <para>────────────────────</para>
		/// </summary>
		virtual size_t link() const final;

	public:
		/// <summary>
		/// <para>──────────────</para>
		/// <para>タスク全体の処理を行う空間です。</para>
		/// <para>──────────────</para>
		/// </summary>
		class System
		{
		public:
			/// <summary>
			/// <para>────────────────</para>
			/// <para>登録されているタスクを全て消去します。</para>
			/// <para>キープ中のタスクは消去されません。</para>
			/// <para>────────────────</para>
			/// </summary>
			static void Clear();

			/// <summary>
			/// <para>─────────────────</para>
			/// <para>消去フラグ有効のタスクを全て消去します。</para>
			/// <para>─────────────────</para>
			/// </summary>
			static void Update();
		};
	};
}
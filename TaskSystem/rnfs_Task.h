/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

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
		friend class	TaskKeep;	//キープや消去で必要

	private:
		Task*			mp_Prev;	//自身の前ポインタ
		Task*			mp_Next;	//自身の後ポインタ

		size_t			m_LifeSpan;	//寿命（0:無効　1:消去）
		
		size_t			m_Link;		//TaskKeep の接続確認

	private:
		static Task*	mp_Begin;	//先頭ポインタ
		static Task*	mp_End;		//末尾ポインタ

	private:
		//システムへの登録
		static void _Register_(Task* p_Task);
		//システムから消去　次のポインタが返される
		static Task* _Unregister_(Task* p_Task);

	protected:
		/// <summary>
		/// <para>───────────</para>
		/// <para>システムに登録を行います。</para>
		/// <para>───────────</para>
		/// </summary>
		Task();

		/// <summary>
		/// <para>──────────────────</para>
		/// <para>システムに登録を行います。</para>
		/// <para>指定したフレーム数経過で自動消去します。</para>
		/// <para>──────────────────</para>
		/// </summary>
		///
		/// <param name="lifeSpan">
		/// <para>自動消去する残りフレーム数</para>
		/// </param>
		Task(const size_t lifeSpan);

		//継承クラスのデストラクタが呼ばれる
		virtual ~Task() = default;

		/// <summary>
		/// <para>───────────────</para>
		/// <para>消去フラグを立てます。</para>
		/// <para>キープ中のタスクは消去できません。</para>
		/// <para>タスク内からの呼び出し限定です。</para>
		/// <para>───────────────</para>
		/// <para>次の Task::All::Update 呼び出し時に</para>
		/// <para>タスクの消去が行われます。</para>
		/// <para>───────────────</para>
		/// </summary>
		virtual void Destroy() final;

	public:
		Task(const Task & task) = delete;
		Task(Task && task) = delete;
		void operator =(const Task & task) = delete;
		void operator =(Task && task) = delete;

		/// <summary>
		/// <para>────────────────</para>
		/// <para>消去フラグが立ててあるかを確認します。</para>
		/// <para>────────────────</para>
		/// </summary>
		virtual bool isDestroy() const final;

		/// <summary>
		/// <para>────────────────────</para>
		/// <para>キープ数を取得します。</para>
		/// <para>キープされている場合は、1 以上の値が返されます。</para>
		/// <para>────────────────────</para>
		/// </summary>
		virtual size_t link() const final;

		/// <summary>
		/// <para>────────────</para>
		/// <para>全体の処理を行う空間です。</para>
		/// <para>────────────</para>
		/// </summary>
		class All
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

	/// <summary>
	/// <para>───────────────────────────────────────────────────────</para>
	/// <para>タスクを生成します。</para>
	/// <para>new タスク名(); のタスク生成も可能ですが、delete 記述を行わない事による違和感やルール違反を避ける為、この関数の使用を推奨します。</para>
	/// <para>テンプレート引数を使用します。&lt;タスク名&gt;</para>
	/// <para>───────────────────────────────────────────────────────</para>
	/// <para>　推奨：Create&lt;タスク名&gt;(引数);</para>
	/// <para>非推奨：new タスク名(引数);</para>
	/// <para>───────────────────────────────────────────────────────</para>
	/// </summary>
	///
	/// <param name="args">
	/// <para>コンストラクタの引数</para>
	/// </param>
	template <class TYPE, typename ... ARGS>
	static TYPE* Create(ARGS && ... args)
	{
		return new TYPE(std::forward<ARGS>(args) ...);
	}
}
/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "rnfs_Task.h"

namespace rnfs
{
	Task* Task::mp_Begin = nullptr;		//先頭ポインタ
	Task* Task::mp_End = nullptr;		//末尾ポインタ

	void Task::_Register_(Task * p_Task)
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

	Task* Task::_Unregister_(Task* p_Task)
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

	Task::Task()
		: mp_Prev(nullptr), mp_Next(nullptr), m_LifeSpan(0), m_Link(0)
	{
		Task::_Register_(this);
	}

	Task::Task(const size_t lifeSpan)
		: mp_Prev(nullptr), mp_Next(nullptr), m_LifeSpan(lifeSpan + 1), m_Link(0)
	{
		Task::_Register_(this);
	}

	void Task::Destroy()
	{
		//キープしているタスクは消去できない
		if (0 < m_Link) return;

		//生存時間を 2 にする（次のフレームで消去）
		m_LifeSpan = 2;
	}

	bool Task::isDestroy() const
	{
		return m_LifeSpan == 2;
	}

	size_t Task::link() const
	{
		return m_Link;
	}

	void Task::All::Clear()
	{
		Task* p_Task = mp_Begin; //現在のタスク

		//末尾までループする
		while (p_Task != nullptr)
		{
			//タスクを消去し、次のタスクへ移動
			//キープしているタスクは消去できない
			if (p_Task->m_Link <= 0) p_Task = Task::_Unregister_(p_Task);
			else p_Task = p_Task->mp_Next;
		}
	}

	void Task::All::Update()
	{
		Task* p_Task = mp_Begin; //現在のタスク

		//末尾までループする
		while (p_Task != nullptr)
		{
			//寿命が有効なら減らす
			if (1 < p_Task->m_LifeSpan) --p_Task->m_LifeSpan;

			//タスクを消去し、次のタスクへ移動
			//キープしているタスクは消去できない
			if (p_Task->m_LifeSpan == 1 && p_Task->m_Link <= 0) p_Task = Task::_Unregister_(p_Task);
			else p_Task = p_Task->mp_Next;
		}
	}
}
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

	Task* Task::mp_Destroy = nullptr;	//消去対象の先頭ポインタ

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
		: mp_Prev(nullptr), mp_Next(nullptr), mp_Target(nullptr), m_Link(0)
	{
		//先頭が空の場合は新規タスクを設定
		if (mp_Begin == nullptr) mp_Begin = this;
		else
		{
			//新規タスクの前に末尾タスクを代入
			mp_Prev = mp_End;

			//末尾タスクの次に新規タスクを代入
			mp_End->mp_Next = this;
		}

		//末尾タスクが新規タスクになる
		mp_End = this;
	}

	void Task::Destroy()
	{
		//キープしているタスクは消去できない
		if (m_Link != 0) return;

		//新規タスクの次に先頭タスクを代入
		mp_Target = mp_Destroy;

		//先頭に新規タスクを代入
		mp_Destroy = this;
	}

	bool Task::isDestroy() const
	{
		return mp_Target != nullptr || mp_Destroy == this;
	}

	size_t Task::link() const
	{
		return m_Link;
	}

	void Task::All::Clear()
	{
		//現在のタスク
		Task* p_Task = mp_Begin;

		//末尾までループする
		while (p_Task != nullptr)
		{
			//タスクを消去し、次のタスクへ移動
			//キープしているタスクは消去できない
			if (p_Task->m_Link == 0) p_Task = Task::_Unregister_(p_Task);
			else p_Task = p_Task->mp_Next;
		}

		//先頭を空にする
		mp_Destroy = nullptr;
	}

	void Task::All::Update()
	{
		//末尾までループする
		while (mp_Destroy != nullptr)
		{
			//消去の前に次のタスクを持つ
			Task* p_Task = mp_Destroy->mp_Target;
			//先頭のタスクを消去
			Task::_Unregister_(mp_Destroy);
			//先頭を次のタスクにする
			mp_Destroy = p_Task;
		}
	}
}
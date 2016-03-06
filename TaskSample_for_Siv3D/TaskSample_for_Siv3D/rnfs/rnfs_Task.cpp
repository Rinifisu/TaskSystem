﻿//Copyright © 2015-2016 Rinifisu
//http://rinifisu.blog.jp/

#include "rnfs_Task.h"

namespace rnfs
{
	Task* Task::mp_Begin = nullptr;	//先頭ポインタ
	Task* Task::mp_End = nullptr;	//末尾ポインタ

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
		//null の場合は末尾タスクがなくなったので、次のタスクを先頭にする
		else mp_Begin = next;

		//タスクの消去
		delete p_Task;

		//次のタスクを返す
		return next;
	}

	Task::Task()
		: mp_Prev(nullptr), mp_Next(nullptr), m_Destroy(false), m_Link(0)
	{
		//先頭が空の場合は新規タスクを設定
		if (mp_Begin == nullptr) mp_Begin = this;
		else
		{
			//新規タスクの前に末尾タスクを代入
			this->mp_Prev = mp_End;

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

		//消去フラグを有効にする
		m_Destroy = true;
	}

	size_t Task::link() const
	{
		return m_Link;
	}

	void Task::System::Clear()
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
	}

	void Task::System::Update()
	{
		//現在のタスク
		Task* p_Task = mp_Begin;

		//末尾までループする
		while (p_Task != nullptr)
		{
			//消去フラグが立っている場合はタスクの消去し、次のタスクへ移動
			if (p_Task->m_Destroy) p_Task = Task::_Unregister_(p_Task);
			//更新処理
			else p_Task = p_Task->mp_Next;
		}
	}
}
/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "rnfs_TaskReceive.h"

#include "rnfs_TaskSend.h"

namespace rnfs
{
	std::unordered_map<std::string, TaskKeepArray<Task>> TaskReceive::m_Receive; //受信リストのポインタ

	TaskReceive::TaskReceive()
		: mp_Task(nullptr), m_Name(""), m_ID(0)
	{

	}

	TaskReceive::~TaskReceive()
	{
		this->Unregister();
	}

	void TaskReceive::Unregister()
	{
		//リストから登録を解除する
		if (mp_Task)
		{
			//タスクの解放
			m_Receive[m_Name].Free_ID(m_ID);

			//リストが空になったら、消去する
			if (m_Receive[m_Name].isEmpty()) m_Receive.erase(m_Name);
		}

		//初期化
		mp_Task = nullptr;
		m_Name = "";
		m_ID = 0;
	}

	void TaskReceive::All::Update()
	{
		TaskSend* p_Transmit = TaskSend::mp_Begin; //現在のリストポインタ

		//末尾までループする
		while (p_Transmit != nullptr)
		{
			//コールが有効であり、コールが設定されていたら
			if (p_Transmit->m_Active && p_Transmit->m_Call)
			{
				//関数の実行
				for (auto & i : m_Receive[p_Transmit->m_Check])
				{
					(i.second.task().*p_Transmit->m_Call)(*p_Transmit->mp_Task);
				}
			}

			//次のリストへ移動
			p_Transmit = p_Transmit->mp_Next;
		}
	}
}
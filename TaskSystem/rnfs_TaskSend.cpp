/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "rnfs_TaskSend.h"

namespace rnfs
{
	std::unordered_map<std::string, TaskKeepArray<Task>> TaskSend::m_Send; //送信リストのポインタ

	TaskSend::TaskSend()
		: mp_Task(nullptr), m_Name(""), m_ID(0)
	{

	}

	TaskSend::~TaskSend()
	{
		this->Unregister();
	}

	void TaskSend::Unregister()
	{
		//リストから登録を解除する
		if (mp_Task)
		{
			//タスクの解放
			m_Send[m_Name].Free_ID(m_ID);

			//リストが空になったら、消去する
			if (m_Send[m_Name].isEmpty()) m_Send.erase(m_Name);
		}

		//初期化
		mp_Task = nullptr;
		m_Name = "";
		m_ID = 0;
	}

	void TaskSend::All::Update()
	{
		TaskReceive* p_Receive = TaskReceive::mp_Begin; //現在のリストポインタ

		//末尾までループする
		while (p_Receive != nullptr)
		{
			//コールが有効であり、コールが設定されていたら
			if (p_Receive->m_Active && p_Receive->m_Call)
			{
				//関数の実行
				for (auto & i : m_Send[p_Receive->m_Check])
				{
					(*p_Receive->mp_Task.*p_Receive->m_Call)(i.second.task());
				}
			}

			//次のリストへ移動
			p_Receive = p_Receive->mp_Next;
		}
	}
}
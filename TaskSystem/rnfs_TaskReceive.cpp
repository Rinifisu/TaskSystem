//Copyright © 2015-2016 Rinifisu
//http://rinifisu.blog.jp/

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

	void TaskReceive::System::Update()
	{
		TaskSend* p_Transmit = TaskSend::mp_Begin; //現在のリストポインタ

		//末尾までループする
		while (p_Transmit != nullptr)
		{
			//コールが有効であり、コールが設定されていたら
			if (p_Transmit->m_Active && p_Transmit->m_Call)
			{
				//関数の実行
				for (auto& i : m_Receive[p_Transmit->m_Check])
				{
					(i.second.getTask().*p_Transmit->m_Call)(*p_Transmit->mp_Task);
				}
			}

			//次のリストへ移動
			p_Transmit = p_Transmit->mp_Next;
		}
	}
}
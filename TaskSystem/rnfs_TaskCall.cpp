/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "rnfs_TaskCall.h"

namespace rnfs
{
	std::unordered_map<unsigned char, TaskCall*> TaskCall::m_Begin; //コールリストの先頭ポインタ

	void TaskCall::_Register_(const bool pushBack)
	{
		//先頭が空の場合は新規リストを設定
		if (m_Begin[m_Group] == nullptr) m_Begin[m_Group] = this;
		//登録処理
		else
		{
			TaskCall* task = m_Begin[m_Group];	//現在のリストポインタ
			TaskCall* prev = nullptr;			//１つ前のリストポインタ

			//末尾までループする
			while (task != nullptr)
			{
				//優先度の関係でこれ以上進めない場合
				if ((pushBack && m_Priority < task->m_Priority)
					|| (!pushBack && m_Priority <= task->m_Priority))
				{
					//入れ替え対象のリストが先頭だった場合は自身で上書き
					if (task == m_Begin[m_Group]) m_Begin[m_Group] = this;

					//次が無いポインタにリストを代入
					mp_Next = task;

					//前が無いポインタに新規リストを代入
					task->mp_Prev = this;

					break;
				}

				//前のリストに現在のリストを代入
				prev = task;

				//次のリストへ移動
				task = task->mp_Next;

				//優先順位が決まらなかった場合は nullptr を代入
				//ここに到達するリストは優先度がリストの中で１番最大である
				if (task == nullptr) mp_Next = nullptr;
			}

			//先頭を変更した場合は前が存在しないので nullptr を保持
			if (prev != nullptr)
			{
				//次が無いポインタに新規リストを代入
				prev->mp_Next = this;

				//前が無いポインタにリストを代入
				mp_Prev = prev;
			}
		}
	}

	void TaskCall::_Unregister_()
	{
		TaskCall* updateNext = mp_Next; //リストの次
		TaskCall* updatePrev = mp_Prev; //リストの前

		//次に前を代入してつなぎ合わせる
		if (updateNext != nullptr) updateNext->mp_Prev = updatePrev;

		//前に次を代入してつなぎ合わせる
		if (updatePrev != nullptr) updatePrev->mp_Next = updateNext;
		//null の場合は一番先頭にあったリストがなくなったので、次のリストを先頭にする
		else m_Begin[m_Group] = updateNext;

		//次のリストも null の場合はリストが空になったので、消去する
		if (m_Begin[m_Group] == nullptr) m_Begin.erase(m_Group);
	}

	TaskCall::TaskCall()
		: mp_Task(nullptr), mp_Prev(nullptr), mp_Next(nullptr)
		, m_Priority(0), m_Name(""), m_Group(0)
		, m_Active(false)
	{

	}

	TaskCall::~TaskCall()
	{
		//コールが登録されていたら登録解除する
		if (mp_Task) this->_Unregister_();
	}

	void TaskCall::Unregister()
	{
		//リストから登録を解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = nullptr;
		mp_Prev = nullptr;
		mp_Next = nullptr;
		m_Priority = 0;
		m_Name = "";
		m_Group = 0;
		m_Call = nullptr;
		m_Active = false;
	}

	void TaskCall::ClearCall()
	{
		m_Call = nullptr;
	}

	const bool TaskCall::isCall() const
	{
		//空でないか確認
		return m_Call;
	}

	void TaskCall::SetActive(const bool active)
	{
		m_Active = active;
	}

	const bool TaskCall::isActive() const
	{
		return m_Active;
	}

	void TaskCall::SetName(const std::string & name)
	{
		m_Name = name;
	}

	const std::string & TaskCall::name() const
	{
		return m_Name;
	}

	void TaskCall::SetGroup(const unsigned char group)
	{
		m_Group = group;
	}

	const unsigned char TaskCall::group() const
	{
		return m_Group;
	}

	void TaskCall::System::Update(const unsigned char group)
	{
		TaskCall* p_TaskCall = m_Begin[group]; //現在のリストポインタ

		//末尾までループする
		while (p_TaskCall != nullptr)
		{
			//コールが有効であり、コールが設定されていたら
			if (p_TaskCall->m_Active && p_TaskCall->m_Call)
			{
				//関数の実行
				(p_TaskCall->mp_Task->*p_TaskCall->m_Call)();
			}

			//次のリストへ移動
			p_TaskCall = p_TaskCall->mp_Next;
		}
	}
}
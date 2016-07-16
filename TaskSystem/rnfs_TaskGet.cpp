/*
The MIT License (MIT)
Copyright © 2015-2016 Rinifisu
http://rinifisu.blog.jp/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "rnfs_TaskGet.h"

namespace rnfs
{
	std::unordered_map<std::string, TaskKeepArray<Task>> TaskGet::m_Data; //システム

	TaskGet::TaskGet()
		: mp_Task(nullptr), m_Name(""), m_ID(0)
	{

	}

	TaskGet::~TaskGet()
	{
		if (mp_Task) this->_Unregister_();
	}

	const bool TaskGet::isRegister() const
	{
		return mp_Task != nullptr;
	}

	void TaskGet::Unregister()
	{
		//リストから登録を解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = nullptr;
		m_Name = "";
		m_ID = 0;
	}

	const TaskID TaskGet::id() const
	{
		return m_ID;
	}
	
	void TaskGet::_Register_()
	{
		//追加される位置を取得する
		m_ID = m_Data[m_Name].nextID();

		//追加
		m_Data[m_Name].Keep_Back(mp_Task);

		//カウントを無効にする
		m_Data[m_Name].Safety_ID(m_ID, false);
	}

	void TaskGet::_Unregister_()
	{
		//タスクの解放
		m_Data[m_Name].Free_ID(m_ID);

		//リストが空になったら、消去する
		if (m_Data[m_Name].isEmpty()) m_Data.erase(m_Name);
	}
}
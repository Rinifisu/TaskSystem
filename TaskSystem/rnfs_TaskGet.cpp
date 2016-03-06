//Copyright © 2015-2016 Rinifisu
//http://rinifisu.blog.jp/

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

	void TaskGet::Unregister()
	{
		//リストから登録を解除する
		if (mp_Task) this->_Unregister_();

		//初期化
		mp_Task = nullptr;
		m_Name = "";
		m_ID = 0;
	}

	const size_t TaskGet::id()
	{
		return m_ID;
	}
	
	void TaskGet::_Register_()
	{
		//追加される位置を取得する
		m_ID = m_Data[m_Name].nextID();

		//追加
		m_Data[m_Name].Add_Back(mp_Task);

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
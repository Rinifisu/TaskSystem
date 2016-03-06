//二重インクルード防止
#pragma once

//Siv3D
#include <Siv3D.hpp>

//rnfsフレームワーク
#include "rnfs.h"

//コールグループ
enum CallGroup
{
	CALL_UPDATE,
	CALL_DRAW,
};

//名前空間省略
using rnfs::Task;
using rnfs::TaskCall;
using rnfs::TaskGet;
using rnfs::TaskKeep;
using rnfs::TaskKeepArray;
using rnfs::TaskSend;
using rnfs::TaskReceive;
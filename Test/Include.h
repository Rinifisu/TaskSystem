//��d�C���N���[�h�h�~
#pragma once

//Siv3D
#include <Siv3D.hpp>

//rnfs�t���[�����[�N
#include "rnfs.h"

//�R�[���O���[�v
enum CallGroup
{
	CALL_UPDATE,
	CALL_DRAW,
};

//���O��ԏȗ�
using rnfs::Task;
using rnfs::TaskCall;
using rnfs::TaskGet;
using rnfs::TaskKeep;
using rnfs::TaskKeepArray;
using rnfs::TaskSend;
using rnfs::TaskReceive;
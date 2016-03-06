//��d�C���N���[�h�h�~
#pragma once

//���s�ɕK���K�v
#include "Include.h"

//�����O
#include "Ring.h"
//�v���g�^�C�v�錾
class Ring;

//�d��
#define GRAVITY	9.8 / 60.0

//�{�[�����΂��N���X
class Ball : public Task
{
private:
	Vec2			m_Position;	//���W

	double			m_Jump;		//�W�����v��
	double			m_Move;		//���E�̈ړ���

	Color			m_Color;	//�F

private:
	TaskCall		m_Update;	//�X�V�R�[��
	TaskCall		m_Draw;		//�`��R�[��

	TaskReceive		m_Receive;	//�^�X�N��M

public:
	//�R���X�g���N�^
	Ball(const Vec2 & position);

	//�f�X�g���N�^
	~Ball() = default;

private:
	//�X�V
	void Update();

	//�`��
	void Draw();

public:
	//Ring �Ɠ����蔻����s��
	void HitCheck(Ring & ring);
};
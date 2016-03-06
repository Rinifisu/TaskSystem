//��d�C���N���[�h�h�~
#pragma once

//���s�ɕK���K�v
#include "Include.h"

//�{�[��
#include "Ball.h"
//�v���g�^�C�v�錾
class Ball;

//�����O���o��������N���X
class Ring : public Task
{
private:
	Vec2		m_Position;	//���W

	int			m_Radius;	//���a
	int			m_Alpha;	//�s�����x

private:
	TaskCall	m_Update;	//�X�V�R�[��
	TaskCall	m_Draw;		//�`��R�[��

	TaskSend	m_Transmit;	//�^�X�N���M

public:
	//�R���X�g���N�^
	Ring();

	//�f�X�g���N�^
	~Ring() = default;

private:
	//�X�V
	void Update();

	//�`��
	void Draw();

public:
	//�`�悷�郊���O�Ɠ����~���擾����
	Circle getCircle() const;
};
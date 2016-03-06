#include "Ring.h"

#include "Priority.h"

Ring::Ring()
	: Task()
	, m_Position(Mouse::Pos())
	, m_Radius(0), m_Alpha(255)
{
	//�R�[���̓o�^
	m_Update.Register(this, &Ring::Update, CALL_UPDATE, Priority::Ring);
	m_Draw.Register(this, &Ring::Draw, CALL_DRAW, Priority::Ring);

	//�^�X�N���M�̓o�^
	m_Transmit.Register<Ball>(this, &Ball::HitCheck);
}

void Ring::Update()
{
	//���a���g�傷��
	++m_Radius;

	//�s�����x��������
	m_Alpha -= 5;
	//���S�ɓ����ɂȂ�����
	if (m_Alpha <= 0)
	{
		//�{�[���̐���
		new Ball(m_Position);

		//���g����������
		this->Destroy();
	}
}

void Ring::Draw()
{
	//�~�g�̂ݕ`��
	Circle(m_Position, m_Radius).drawFrame(1.0, 1.0, Color(Palette::Red, m_Alpha));
}

Circle Ring::getCircle() const
{
	//�~��Ԃ�
	return Circle(m_Position, m_Radius);
}
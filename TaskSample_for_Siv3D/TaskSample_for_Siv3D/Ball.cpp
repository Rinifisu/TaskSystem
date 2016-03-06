#include "Ball.h"

#include "Priority.h"

Ball::Ball(const Vec2 & position)
	: Task()
	, m_Position(position)
	, m_Jump(-7.0), m_Move(Random(-3.0, 3.0))
	, m_Color(Random(0, 255), Random(0, 255), Random(0, 255), 255)
{
	//�R�[���̓o�^
	m_Update.Register(this, &Ball::Update, CALL_UPDATE, Priority::Ball);
	m_Draw.Register(this, &Ball::Draw, CALL_DRAW, Priority::Ball);

	//�^�X�N��M�̓o�^
	m_Receive.Register(this);
}

void Ball::Update()
{
	//���E�̈ړ�
	m_Position.x += m_Move;

	//�d�͂̕ω�
	m_Jump += GRAVITY;
	//�㉺�̈ړ�
	m_Position.y += m_Jump;
	//��ʊO�܂ŗ���������A���g����������
	if (480.0 < m_Position.y) this->Destroy();
}

void Ball::Draw()
{
	//�~��`��
	Circle(m_Position, 10.0).draw(m_Color);
}

void Ball::HitCheck(Ring & ring)
{
	//�{�[���ƃ����O���ڐG���Ă�����
	if (Circle(m_Position, 10.0).intersects(ring.getCircle()))
	{
		//�d�͂��㏸��Ԃɂ���
		m_Jump = -7.0;
	}
}
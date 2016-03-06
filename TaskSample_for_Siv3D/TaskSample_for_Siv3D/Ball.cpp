#include "Ball.h"

#include "Priority.h"

Ball::Ball(const Vec2 & position)
	: Task()
	, m_Position(position)
	, m_Jump(-7.0), m_Move(Random(-3.0, 3.0))
	, m_Color(Random(0, 255), Random(0, 255), Random(0, 255), 255)
{
	//コールの登録
	m_Update.Register(this, &Ball::Update, CALL_UPDATE, Priority::Ball);
	m_Draw.Register(this, &Ball::Draw, CALL_DRAW, Priority::Ball);

	//タスク受信の登録
	m_Receive.Register(this);
}

void Ball::Update()
{
	//左右の移動
	m_Position.x += m_Move;

	//重力の変化
	m_Jump += GRAVITY;
	//上下の移動
	m_Position.y += m_Jump;
	//画面外まで落下したら、自身を消去する
	if (480.0 < m_Position.y) this->Destroy();
}

void Ball::Draw()
{
	//円を描画
	Circle(m_Position, 10.0).draw(m_Color);
}

void Ball::HitCheck(Ring & ring)
{
	//ボールとリングが接触していたら
	if (Circle(m_Position, 10.0).intersects(ring.getCircle()))
	{
		//重力を上昇状態にする
		m_Jump = -7.0;
	}
}
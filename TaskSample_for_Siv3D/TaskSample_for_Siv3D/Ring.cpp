#include "Ring.h"

#include "Priority.h"

Ring::Ring()
	: Task()
	, m_Position(Mouse::Pos())
	, m_Radius(0), m_Alpha(255)
{
	//コールの登録
	m_Update.Register(this, &Ring::Update, CALL_UPDATE, Priority::Ring);
	m_Draw.Register(this, &Ring::Draw, CALL_DRAW, Priority::Ring);

	//タスク送信の登録
	m_Transmit.Register<Ball>(this, &Ball::HitCheck);
}

void Ring::Update()
{
	//半径を拡大する
	++m_Radius;

	//不透明度を下げる
	m_Alpha -= 5;
	//完全に透明になったら
	if (m_Alpha <= 0)
	{
		//ボールの生成
		new Ball(m_Position);

		//自身を消去する
		this->Destroy();
	}
}

void Ring::Draw()
{
	//円枠のみ描画
	Circle(m_Position, m_Radius).drawFrame(1.0, 1.0, Color(Palette::Red, m_Alpha));
}

Circle Ring::getCircle() const
{
	//円を返す
	return Circle(m_Position, m_Radius);
}
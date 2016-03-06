#include "Ring.h"

#include "Priority.h"

Ring::Ring()
	: Task()
	, m_Position(Mouse::Pos())
	, m_Radius(0), m_Alpha(255)
{
	//ƒR[ƒ‹‚Ì“o˜^
	m_Update.Register(this, &Ring::Update, CALL_UPDATE, Priority::Ring);
	m_Draw.Register(this, &Ring::Draw, CALL_DRAW, Priority::Ring);

	//ƒ^ƒXƒN‘—M‚Ì“o˜^
	m_Transmit.Register<Ball>(this, &Ball::HitCheck);
}

void Ring::Update()
{
	//”¼Œa‚ğŠg‘å‚·‚é
	++m_Radius;

	//•s“§–¾“x‚ğ‰º‚°‚é
	m_Alpha -= 5;
	//Š®‘S‚É“§–¾‚É‚È‚Á‚½‚ç
	if (m_Alpha <= 0)
	{
		//ƒ{[ƒ‹‚Ì¶¬
		new Ball(m_Position);

		//©g‚ğÁ‹‚·‚é
		this->Destroy();
	}
}

void Ring::Draw()
{
	//‰~˜g‚Ì‚İ•`‰æ
	Circle(m_Position, m_Radius).drawFrame(1.0, 1.0, Color(Palette::Red, m_Alpha));
}

Circle Ring::getCircle() const
{
	//‰~‚ğ•Ô‚·
	return Circle(m_Position, m_Radius);
}
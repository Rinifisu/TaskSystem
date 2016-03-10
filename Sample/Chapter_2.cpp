#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_2：更新関数を切り替える

TaskCall::SetCall を使い、更新関数を切り替えます。
左クリックで風船が生成され、
1．膨らむ
2．破裂
の2ステップで消去します。
*/

//-------------------------------------------- 区切り --------------------------------------------//

class Balloon : public Task
{
private:
	Vec2		m_Pos;		//座標
	double		m_Radius;	//描画半径
	int			m_Alpha;	//不透明度

	TaskCall	m_Update;	//更新設定

public:
	Balloon() : Task()
		, m_Pos(Mouse::Pos())
		, m_Radius(0.0), m_Alpha(126)
		, m_Update(this, &Balloon::Swell)
	{ }

private:
	void Swell()
	{
		//半径を拡大
		m_Radius += 0.5;
		//半径が50以上で更新関数切り替え
		if (50.0 <= m_Radius) m_Update.SetCall(&Balloon::Rupture);

		//描画
		Circle(m_Pos, m_Radius).draw(Alpha(m_Alpha));
	}

	void Rupture()
	{
		//不透明度を下げる
		m_Alpha -= 18;
		//完全透明で消去
		if (m_Alpha <= 0) this->Destroy();

		//描画
		else Circle(m_Pos, m_Radius).drawFrame(2.0, 2.0, Alpha(m_Alpha));
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

void Main()
{
	while (System::Update())
	{
		//左クリックで風船を生成
		if (Input::MouseL.clicked) Create<Balloon>();

		//TaskCall で設定した関数を呼び出す
		TaskCall::System::Update();
		//Destroy を呼び出したタスクを消去
		Task::System::Update();
	}

	//全タスクの消去
	Task::System::Clear();
}
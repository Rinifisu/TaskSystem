#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_2：更新関数を切り替える

TaskCall::SetCall を使い、更新関数を切り替えます。
右、下、左、上の順番に移動した後、消滅します。
切り替わりが分かりやすいように、必要な処理を1つの関数にまとめています。
*/

//移動継続時間
#define MOVE_TIME 60

class Frame : public Task
{
private:
	int			m_Time;		//移動時間
	Vec2		m_Pos;		//座標

	TaskCall	m_Update;	//更新設定

public:
	Frame() : Task()
		, m_Time(MOVE_TIME), m_Pos(Mouse::Pos())
		, m_Update(this, &Frame::Right) //「右に移動」から開始
	{ }

private:
	//-------------------- 右に移動 --------------------//
	void Right()
	{
		//移動時間が0以下で更新関数切り替え
		if (--m_Time <= 0)
		{
			m_Time = MOVE_TIME;
			m_Update.SetCall(&Frame::Down);
		}

		//移動
		++m_Pos.x;
		//描画
		Circle(m_Pos, 8.0).draw(Palette::White);
	}

	//-------------------- 下に移動 --------------------//
	void Down()
	{
		//移動時間が0以下で更新関数切り替え
		if (--m_Time <= 0)
		{
			m_Time = MOVE_TIME;
			m_Update.SetCall(&Frame::Left);
		}

		//移動
		++m_Pos.y;
		//描画
		Circle(m_Pos, 6.0).draw(Palette::Red);
	}

	//-------------------- 左に移動 --------------------//
	void Left()
	{
		//移動時間が0以下で更新関数切り替え
		if (--m_Time <= 0)
		{
			m_Time = MOVE_TIME;
			m_Update.SetCall(&Frame::Up);
		}

		//移動
		--m_Pos.x;
		//描画
		Circle(m_Pos, 4.0).draw(Palette::Yellow);
	}

	//-------------------- 上に移動 --------------------//
	void Up()
	{
		//移動時間が0以下で消去
		if (--m_Time <= 0) this->Destroy();

		//移動
		--m_Pos.y;
		//描画
		Circle(m_Pos, 2.0).draw(Palette::Green);
	}
};

void Main()
{
	while (System::Update())
	{
		//フレームを生成
		Create<Frame>();

		//TaskCall で設定した関数を呼び出す
		TaskCall::System::Update();
		//Destroy を呼び出したタスクを消去
		Task::System::Update();
	}

	//全タスクの消去
	Task::System::Clear();
}
#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_5：タスクキープ＆呼び出し関数分別

Ball を生成し、左クリックでジャンプを行います。
描画関数を用意し、更新関数にあった描画処理を移動しました。
TaskCall を増やす事により、呼び出し方を個別に変更できます。

TaskKeep でポインタのようにタスクを持つことができ、外部操作を行えます。
Main 関数でジャンプを行う関数を TaskKeep 経由で呼び出します。
Update と Draw は private 関数にできる為、安全です。
*/

//-------------------------------------------- 区切り --------------------------------------------//

class Ball : public Task
{
private:
	Vec2		m_Pos;		//座標
	double		m_Gravity;	//重力

	TaskCall	m_Update;	//更新設定
	TaskCall	m_Draw;		//描画設定

public:
	Ball() : Task()
		, m_Pos(320.0, 460.0), m_Gravity(0.0)

		, m_Update(this)				//最初は関数を呼び出さずに、後から設定する
		, m_Draw(this, &Ball::Draw)		//描画を常に行う
	{ }

private:
	void Update()
	{
		//重力加速度÷60フレーム
		m_Gravity -= 9.8 / 60.0;
		//重力加算
		m_Pos.y -= m_Gravity;

		//地面に着地したら
		if (460.0 < m_Pos.y)
		{
			//座標を地面に設定
			m_Pos.y = 460.0;

			//自動呼び出し関数を削除して、呼ばれないようにする
			m_Update.ClearCall();
		}
	}

	void Draw()
	{
		//描画
		Circle(m_Pos, 10.0).draw();
	}

public:
	void Jump()
	{
		//ジャンプ中であれば、何もしない
		//isCall で自動呼び出し関数を、設定しているか確認できる
		if (m_Update.isCall()) return;

		//重力を上昇状態にする
		m_Gravity = 10.0;

		//自動呼び出し関数を設定する
		m_Update.SetCall(&Ball::Update);
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

void Main()
{
	//ボールを生成し、TaskKeep でタスクを持つ
	TaskKeep<Ball> ball = Create<Ball>();

	while (System::Update())
	{
		//左クリックでボールのジャンプ
		if (Input::MouseL.clicked) ball->Jump();

		TaskCall::System::Update();
		Task::System::Update();
	}

	Task::System::Clear();
}
#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_9：タスク内で当たり判定処理 ＆ 優先順位

TaskSend と TaskReceive を使い、当たり判定を行います。
従来のタスクシステムであれば、当たり判定専用のタスクシステムを用意する必要がありましたが、こちらでは不要です。

送信側である TaskSend は送信を許可する Register で完了します。
受信側である TaskReceive は Register で受信対象や受信関数を設定することで完了します。

TaskCall の第3引数に優先順位を設定し、後から追加される Enemy に重なることを防止できます。
*/

//-------------------------------------------- 区切り --------------------------------------------//

enum CallGroup
{
	CallGroup_Update,
	CallGroup_Draw,
};

enum CallPriority
{
	CallPriority_Enemy,		//最初に描画が行われる
	CallPriority_Player,	//その後に描画が行われるので、プレイヤーが隠れない
};

//-------------------------------------------- 区切り --------------------------------------------//

class Enemy : public Task
{
private:
	Vec2		m_Pos;	//座標

	TaskCall	m_Draw;	//描画設定

	TaskSend	m_Send;	//送信設定

public:
	Enemy() : Task()
		, m_Pos(Random(20.0, 620.0), Random(20.0, 460.0))
		, m_Draw(this, &Enemy::Draw, CallGroup_Draw, CallPriority_Enemy) //描画をプレイヤーより先に行う
	{
		//送信側登録
		m_Send.Register(this);
	}

private:
	void Draw()
	{
		//描画
		Circle(m_Pos, 10.0).draw(Palette::Red);
	}

public:
	void SetDestroy()
	{
		//消去
		this->Destroy();
	}

	Circle getCircle() const
	{
		//描画する円を返す
		return Circle(m_Pos, 10.0);
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

class Player : public Task
{
private:
	Vec2		m_Pos;		//座標
	Vec2		m_Target;	//移動先

	TaskCall	m_Update;	//更新設定
	TaskCall	m_Draw;		//描画設定

	TaskReceive	m_Receive;	//受信設定

public:
	Player() : Task()
		, m_Pos(320.0, 240.0), m_Target(320.0, 240.0)
		, m_Update(this, &Player::Update, CallGroup_Update)
		, m_Draw(this, &Player::Draw, CallGroup_Draw, CallPriority_Player) //描画をエネミーの後に行う
	{
		//受信側設定
		m_Receive.Register<Enemy>(this, &Player::HitCheck);
	}

private:
	void Update()
	{
		//左クリックでマウスの位置へターゲット変更
		if (Input::MouseL.clicked) m_Target = Mouse::Pos();

		//ターゲットに向けて滑らか移動
		m_Pos += (m_Target - m_Pos) * 0.1;
	}

	void Draw()
	{
		//描画
		Circle(m_Pos, 20.0).draw(Palette::Green);
	}

public:
	void HitCheck(Enemy & enemy)
	{
		//プレイヤーと衝突していたら消去
		if (Circle(m_Pos, 20.0).intersects(enemy.getCircle())) enemy.SetDestroy();
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

void Main()
{
	//プレイヤーの生成
	create<Player>();

	while (System::Update())
	{
		//右クリックでエネミーを生成
		if (Input::MouseR.pressed) create<Enemy>();

		//TaskSend と TaskReceive で設定した関数を呼び出す
		TaskSend::All::Update();

		TaskCall::All::Update(CallGroup_Update);
		TaskCall::All::Update(CallGroup_Draw);

		Task::All::Update();
	}

	Task::All::Clear();
}
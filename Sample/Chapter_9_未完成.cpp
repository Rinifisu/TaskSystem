#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_9：タスク内で当たり判定処理 ＆ 優先順位

TaskReceive と TaskSend を使い、当たり判定を行います。
従来のタスクシステムであれば、当たり判定専用のタスクシステムを用意する必要がありましたが、こちらでは不要です。

受信側である TaskReceive は受信を許可する Register で完了します。
送信側である TaskSend は Register で送信対象や送信関数を設定することで完了します。

デメリットとして、お互いを include し、プロトタイプ宣言を行う必要があります。
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
	Vec2		m_Pos;		//座標

	TaskCall	m_Draw;		//描画設定

	TaskReceive	m_Receive;	//受信設定

public:
	Enemy() : Task()
		, m_Pos(Random(20.0, 620.0), Random(20.0, 460.0))
		, m_Draw(this, &Enemy::Draw, CallGroup_Draw, CallPriority_Enemy) //描画をプレイヤーより先に行う
	{
		//受信側登録
		m_Receive.Register(this);
	}

private:
	void Draw()
	{
		//描画
		Circle(m_Pos, 10.0).draw(Palette::Red);
	}

public:
	void HitCheck(Player & player)
	{
		//プレイヤーと衝突していたら消去
		if (Circle(m_Pos, 10.0).intersects(player.getCircle())) this->Destroy();
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

	TaskSend	m_Send;		//送信設定

public:
	Player() : Task()
		, m_Pos(320.0, 240.0), m_Target(320.0, 240.0)
		, m_Update(this, &Player::Update, CallGroup_Update)
		, m_Draw(this, &Player::Draw, CallGroup_Draw, CallPriority_Player) //描画をエネミーの後に行う
	{
		//送信側設定
		m_Send.Register<Enemy>(this, &Enemy::HitCheck);
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
		Circle(m_Pos, 10.0).draw(Palette::Green);
	}

public:
	Circle getCircle() const
	{
		//描画する円を返す
		return Circle(m_Pos, 10.0);
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

void Main()
{
	//プレイヤーの生成
	Create<Player>();

	while (System::Update())
	{
		//右クリックでエネミーを生成
		if (Input::MouseR.pressed) Create<Enemy>();

		//TaskReceive と TaskSend で設定した関数を呼び出す
		TaskReceive::All::Update();

		TaskCall::All::Update(CallGroup_Update);
		TaskCall::All::Update(CallGroup_Draw);

		Task::All::Update();
	}

	Task::All::Clear();
}
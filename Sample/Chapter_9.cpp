#include <Siv3D.hpp>
#include <rnfs.h>

/*
Chapter_9：タスク内で当たり判定処理 ＆ 優先順位

TaskLink を使い、当たり判定を行います。
従来のタスクシステムであれば、当たり判定専用のタスクシステムを用意する必要がありましたが、こちらでは不要です。

送信側は Register で登録を行います。
受信側は Call で受信したいクラスと関数を指定して呼び出す事ができます。

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

	TaskLink	m_Send;	//送信用

public:
	Enemy() : Task()
		, m_Pos(Random(20.0, 620.0), Random(20.0, 460.0))
		, m_Draw(this, &Enemy::Draw, CallGroup_Draw, CallPriority_Enemy)	//描画をプレイヤーより先に行う
		, m_Send(this)														//this のみで受信側登録完了
	{ }

private:
	void Draw()
	{
		//描画
		Circle(m_Pos, 10.0).draw(Palette::Red);
	}

public:
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

public:
	Player() : Task()
		, m_Pos(320.0, 240.0), m_Target(320.0, 240.0)
		, m_Update(this, &Player::Update, CallGroup_Update)
		, m_Draw(this, &Player::Draw, CallGroup_Draw, CallPriority_Player) //描画をエネミーの後に行う
	{ }

private:
	void Update()
	{
		//左クリックでマウスの位置へターゲット変更
		if (Input::MouseL.clicked) m_Target = Mouse::Pos();

		//ターゲットに向けて滑らか移動
		m_Pos += (m_Target - m_Pos) * 0.1;

		//タスクを受信して、当たり判定を実行
		TaskLink::All::Call<Enemy>(this, &Player::HitCheck);
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
		if (Circle(m_Pos, 20.0).intersects(enemy.getCircle())) enemy.Destroy();
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

		TaskCall::All::Update(CallGroup_Update);
		TaskCall::All::Update(CallGroup_Draw);

		Task::All::Update();
	}

	Task::All::Clear();
}
#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_3：タスク内からタスクを生成

Fireworks を生成し、消去の際に Spark を30個生成し、花火を作ります。
include と Create の2ステップで生成できます。
乱数を様々な場所に使用している為、コンストラクタが複雑になっています。
*/

//-------------------------------------------- 区切り --------------------------------------------//

class Spark : public Task
{
private:
	Vec2		m_Pos;		//座標
	Vec2		m_Vec;		//移動法線
	double		m_Speed;	//移動速度
	Color		m_Color;	//描画色

	TaskCall	m_Update;	//更新設定

public:
	Spark(const Vec2 & vec, const Color & color) : Task()
		, m_Pos(vec), m_Vec(RandomVec2())
		, m_Speed(Random(5.0, 20.0)), m_Color(color)
		, m_Update(this, &Spark::Update)
	{ }

private:
	void Update()
	{
		//移動速度が0以下で消去
		if (--m_Speed <= 0.0) this->Destroy();
		//移動
		m_Pos += m_Vec * m_Speed;
		//描画
		Circle(m_Pos, 2.0).draw(m_Color);
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

class Fireworks : public Task
{
private:
	int			m_Time;		//生存時間
	Vec2		m_Pos;		//座標
	Color		m_Color;	//描画色

	TaskCall	m_Update;	//更新設定

public:
	Fireworks() : Task()
		, m_Time(Random(30, 90)), m_Pos(Mouse::Pos().x, 480.0)
		, m_Color(RandomColor())
		, m_Update(this, &Fireworks::Update)
	{ }

private:
	void Update()
	{
		//生存時間が0以下
		if (--m_Time <= 0)
		{
			//火花を30個生成（引数で座標と色を引き継ぐ）
			for (int i = 0; i < 30; ++i) Create<Spark>(m_Pos, m_Color);
			//消去
			this->Destroy();
		}

		//移動
		m_Pos.y -= 5;
		//描画
		Circle(m_Pos, 5.0).draw(m_Color);
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

void Main()
{
	//打ち上げ時間
	int nextTime = 0;

	while (System::Update())
	{
		//時間を更新
		nextTime = (nextTime + 1) % 15;
		//花火を15フレームごとに生成
		if (nextTime == 0) Create<Fireworks>();

		TaskCall::System::Update();
		Task::System::Update();
	}

	Task::System::Clear();
}
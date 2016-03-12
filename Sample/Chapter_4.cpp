#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_4：再帰的タスク生成

画面の中央に UFO を生成し、消去の際にまた UFO を生成します。
コンストラクタで情報を引き継ぎ、継続できない場合はリセットします。
1回の生成で、半永久的に繰り返されます。
*/

//-------------------------------------------- 区切り --------------------------------------------//

class UFO : public Task
{
private:
	Vec2		m_Pos;		//座標
	Vec2		m_Vec;		//移動法線
	double		m_Speed;	//移動速度
	double		m_Radius;	//描画半径

	TaskCall	m_Update;	//更新設定

public:
	UFO(const Vec2 & pos, const double & radius) : Task()
		, m_Pos(pos), m_Vec(RandomVec2())
		, m_Speed(15.0), m_Radius(radius)
		, m_Update(this, &UFO::Update)
	{ }

private:
	void Update()
	{
		//移動速度が0以下
		if (--m_Speed <= 0.0)
		{
			//縮小
			m_Radius -= 2.0;

			//半径が0.0以下の場合は、新規生成
			if (m_Radius <= 0.0) Create<UFO>(Vec2(320.0, 240.0), 8.0);
			//そうでない場合は、座標と半径を引き継いで生成
			else Create<UFO>(m_Pos, m_Radius);

			//消去
			this->Destroy();
		}
		//移動
		m_Pos += m_Vec * m_Speed;

		//描画
		Circle(m_Pos, m_Radius).draw();
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

void Main()
{
	//画面の中央にユーフォーを生成
	Create<UFO>(Vec2(320.0, 240.0), 8.0);

	while (System::Update())
	{
		TaskCall::All::Update();
		Task::All::Update();
	}

	Task::All::Clear();
}
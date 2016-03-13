#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_7：識別番号でタスク内から消去

Ring を生成します。
識別番号は、連想配列と同じ仕組みで扱えます。
追加直前のタスクのコンストラクタで、s_Ring.nextID() を呼ぶことで s_Ring で割り当てられる識別番号を取得できます。
この識別番号を使い、タスク内から配列を脱退、消去を行っています。
*/

//-------------------------------------------- 区切り --------------------------------------------//

//プロトタイプ宣言
class Ring;
//リングの TaskKeepArray
static TaskKeepArray<Ring> s_Ring;

enum CallGroup
{
	CallGroup_Update,
	CallGroup_Draw,
};

class Ring : public Task
{
private:
	Vec2		m_Pos;		//座標
	double		m_Radius;	//描画半径
	int			m_Alpha;	//不透明度

	size_t		m_ID;		//識別番号

	TaskCall	m_Update;	//更新設定
	TaskCall	m_Draw;		//描画設定

public:
	Ring() : Task()
		, m_Pos(Mouse::Pos()), m_Radius(0.0), m_Alpha(255), m_ID(s_Ring.nextID()) //識別番号を nextID で取得できる
		, m_Update(this, &Ring::Update, CallGroup_Update)
		, m_Draw(this, &Ring::Draw, CallGroup_Draw)
	{ }

private:
	void Update()
	{
		//半径を拡大
		++m_Radius;

		//不透明度を下げる
		m_Alpha -= 5;
		//完全透明で消去する（識別番号を使用）
		if (m_Alpha <= 0) s_Ring.Clear_ID(m_ID);
	}

	void Draw()
	{
		//描画
		Circle(m_Pos, m_Radius).drawFrame(1.0, 1.0, Alpha(m_Alpha));
	}

public:
	void Up()
	{
		//上昇する
		m_Pos.y -= 5.0;
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

void Main()
{
	while (System::Update())
	{
		//リングの生成
		s_Ring.Add_Back(Create<Ring>());

		//左クリックで上昇
		if (Input::MouseL.pressed)
		{
			for (auto & i : s_Ring) i.second->Up();
		}

		TaskCall::All::Update(CallGroup_Update);
		TaskCall::All::Update(CallGroup_Draw);

		Task::All::Update();
	}

	s_Ring.Clear_All();
	Task::All::Clear();
}
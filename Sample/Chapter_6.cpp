#include <Siv3D.hpp>
#include <rnfs.h>

/*
Chapter_6：タスクキープ配列 ＆ TaskCall のグループ分け

左クリックで Ink を生成し、右クリックで不規則移動を行います。
TaskCall の初期設定にグループを指定し、更新を全て行った後、描画する構造にします。

TaskKeepArray で TaskKeep を複数持つことができ、外部操作を行えます。
Main 関数で、不規則移動を行う関数を TaskKeepArray 経由で呼び出します。
*/

//-------------------------------------------- 区切り --------------------------------------------//

enum CallGroup
{
	CallGroup_Update,	//更新グループ
	CallGroup_Draw,		//描画グループ
};

class Ink : public Task
{
private:
	Vec2		m_Pos;		//座標
	double		m_Hue;		//色相

	TaskCall	m_Update;	//更新設定
	TaskCall	m_Draw;		//描画設定

public:
	Ink() : Task()
		, m_Pos(Mouse::Pos()), m_Hue(0.0)

		, m_Update(this, &Ink::Update, CallGroup_Update)	//更新グループに設定
		, m_Draw(this, &Ink::Draw, CallGroup_Draw)			//描画グループに設定
	{ }

private:
	void Update()
	{
		//色相を加算する
		if (360.0 < ++m_Hue) m_Hue = 0.0;
	}

	void Draw()
	{
		//描画
		Circle(m_Pos, 10.0).draw(HueToColor(m_Hue));
	}

public:
	void Move()
	{
		//乱数ベクトルで移動
		m_Pos += RandomVec2();
	}
};

//-------------------------------------------- 区切り --------------------------------------------//

void Main()
{
	//インクのタスクキープ配列
	TaskKeepArray<Ink> ink;

	while (System::Update())
	{
		//左クリックでインクの生成
		if (Input::MouseL.pressed) ink.Create_Back();

		//右クリックでインクを不規則に動かす
		if (Input::MouseR.pressed)
		{
			// first -> 識別番号（Chapter_7 で説明）
			//second -> タスク
			for (auto & i : ink) i.second->Move();
		}

		//更新グループの TaskCall が全て呼ばれる
		TaskCall::All::Update(CallGroup_Update);
		//描画グループの TaskCall が全て呼ばれる
		TaskCall::All::Update(CallGroup_Draw);

		Task::All::Update();
	}

	//インクを全て消去する
	ink.Clear_All();

	Task::All::Clear();
}
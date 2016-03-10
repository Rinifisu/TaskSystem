#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_1：タスクの生成と消去

タスク実装に最低限必要な機能を作り、生成から消去までを行います。
マウスカーソルの位置から円を生成し、60フレーム後に消えるようにします。
TaskCall を使用する事で、指定した関数を自動で呼び出せます。
*/

class Locus : public Task
{
private:
	int			m_Time;		//生存時間
	Vec2		m_Pos;		//座標
	TaskCall	m_Update;	//更新設定

public:
	Locus() : Task()
		, m_Time(60), m_Pos(Mouse::Pos())
		, m_Update(this, &Locus::Update) //第2引数に指定した関数が自動で呼び出される
	{ }

private:
	void Update()
	{
		//生存時間が0以下で消去
		if (--m_Time <= 0) this->Destroy();
		//描画
		Circle(m_Pos, 5.0).draw();
	}
};

void Main()
{
	while (System::Update())
	{
		//軌跡を生成
		Create<Locus>();

		//TaskCall で設定した関数を呼び出す
		TaskCall::System::Update();
		//Destroy を呼び出したタスクを消去
		Task::System::Update();
	}

	//全タスクの消去
	Task::System::Clear();
}
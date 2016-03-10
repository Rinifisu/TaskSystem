#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

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
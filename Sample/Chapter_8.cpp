#include <Siv3D.hpp>
#include "TaskSystem/rnfs.h"

/*
Chapter_8：TaskKeepArray 要らずの TaskGet

Chapter_7 とほとんど同じ構造ですが、構造を少し複雑にしていた TaskKeepArray がありません。
TaskGet に登録する事で、内部に事前用意されている TaskKeepArray に登録され、どこからでも呼び出せるようになります。
登録解除はタスク消滅時に TaskGet のデストラクタで自動解除する為、this->Destroy(); で簡単に行えます。
その為、TaskKeepArray ではなく TaskGet を推奨しますが、可視性が失われる事やイテレータなどの一部機能が無いデメリットもあります。
*/

//-------------------------------------------- 区切り --------------------------------------------//

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

	TaskCall	m_Update;	//更新設定
	TaskCall	m_Draw;		//描画設定

	TaskGet		m_Get;		//登録設定

public:
	Ring() : Task()
		, m_Pos(Mouse::Pos()), m_Radius(0.0), m_Alpha(255)
		, m_Update(this, &Ring::Update, CallGroup_Update)
		, m_Draw(this, &Ring::Draw, CallGroup_Draw), m_Get(this) //this のみで登録完了（Add_Back と同じ扱いになる）
	{ }

private:
	void Update()
	{
		//半径を拡大
		++m_Radius;

		//不透明度を下げる
		m_Alpha -= 5;
		//完全透明で消去する
		if (m_Alpha <= 0) this->Destroy();
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
		create<Ring>();

		//左クリックで上昇
		if (Input::MouseL.pressed)
		{
			//イテレータが使えず、文法も長くなってしまうが、場所を問わずに取得が可能になる
			for (size_t i = 0; i < TaskGet::All::size<Ring>(); ++i) TaskGet::All::task<Ring>(i).Up();
		}

		TaskCall::All::Update(CallGroup_Update);
		TaskCall::All::Update(CallGroup_Draw);

		Task::All::Update();
	}

	Task::All::Clear();
}
//二重インクルード防止
#pragma once

//実行に必ず必要
#include "Include.h"

//リング
#include "Ring.h"
//プロトタイプ宣言
class Ring;

//重力
#define GRAVITY	9.8 / 60.0

//ボールを飛ばすクラス
class Ball : public Task
{
private:
	Vec2			m_Position;	//座標

	double			m_Jump;		//ジャンプ力
	double			m_Move;		//左右の移動量

	Color			m_Color;	//色

private:
	TaskCall		m_Update;	//更新コール
	TaskCall		m_Draw;		//描画コール

	TaskReceive		m_Receive;	//タスク受信

public:
	//コンストラクタ
	Ball(const Vec2 & position);

	//デストラクタ
	~Ball() = default;

private:
	//更新
	void Update();

	//描画
	void Draw();

public:
	//Ring と当たり判定を行う
	void HitCheck(Ring & ring);
};
//二重インクルード防止
#pragma once

//実行に必ず必要
#include "Include.h"

//ボール
#include "Ball.h"
//プロトタイプ宣言
class Ball;

//リングを出現させるクラス
class Ring : public Task
{
private:
	Vec2		m_Position;	//座標

	int			m_Radius;	//半径
	int			m_Alpha;	//不透明度

private:
	TaskCall	m_Update;	//更新コール
	TaskCall	m_Draw;		//描画コール

	TaskSend	m_Transmit;	//タスク送信

public:
	//コンストラクタ
	Ring();

	//デストラクタ
	~Ring() = default;

private:
	//更新
	void Update();

	//描画
	void Draw();

public:
	//描画するリングと同じ円を取得する
	Circle getCircle() const;
};
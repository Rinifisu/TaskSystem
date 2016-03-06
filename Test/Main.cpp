//リング
#include "Ring.h"

//タスクとシステムの更新
bool Update()
{
	//消去信号を出しているタスクを消去する
	Task::System::Update();
	//更新を全て行う
	TaskCall::System::Update(CALL_UPDATE);
	//タスク送信と受信のコールを全て実行
	TaskReceive::System::Update();
	//描画を全て行う
	TaskCall::System::Update(CALL_DRAW);

	//システムの更新
	if (System::Update()) return true;
	
	//タスクを全て消去する
	Task::System::Clear();

	//アプリケーションの終了
	return false;
}

//メイン
void Main()
{
	//終了まで無限ループする
	while (Update())
	{
		//リングの生成
		new Ring;
	}
}
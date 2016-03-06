#TaskSystem
##説明
Siv3D 用のタスクシステムライブラリです。

現在は初期段階の為、別のゲームライブラリとの互換性もありますが、Siv3D 用に最適化予定です。

従来のタスクシステムとは異なるシステムが用意されており、


タスク専用のポインタ「TaskKeep」

「TaskKeep」を配列のように扱うことができ、更にタスククラス内で配列から解放操作が行える「TaskKeepArray」

単体の「Update」「Draw」といった固定関数ではなく、自身で呼び出す関数を簡単に設定ができる「TaskCall」

別のタスクとの当たり判定や、一括チェックなどが配列などを使用せずに行える「TaskSend」「TaskReceive」

「TaskKeep」等でポインタ持ちをせずに、どこからでも指定したタスクを呼び出すことができる「TaskGet」


このような機能があります。

##ライセンス
Copyright (c) 2016 Rinifisu
Released under the MIT license

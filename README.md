#TaskSystem
##説明
タスクシステムライブラリです。

サンプルはSiv3D(http://play-siv3d.hateblo.jp/)を使用していますが、別のゲームライブラリとの互換性もあります。

従来のタスクシステムとは異なるシステムがあり、


タスク専用のポインタ「TaskKeep」

「TaskKeep」を配列のように扱うことができ、更にタスククラス内で配列から解放操作が行える「TaskKeepArray」

単体の「Update」「Draw」といった固定関数ではなく、呼び出す関数を自由に設定できる「TaskCall」

別のタスクとの当たり判定や、一括チェックなどを簡単に行える「TaskSend」「TaskReceive」

「TaskKeep」等でポインタ持ちをせずに、どこからでも指定したタスクを呼び出すことができる「TaskGet」


このような機能を使う事によって、制作効率が向上します。

##ライセンス
Copyright (c) 2015-2016 Rinifisu

http://rinifisu.blog.jp/

Released under the MIT license

#pragma once

//ヘッダファイルを読み込み
#include "game.h"

//キーボードのヘッダファイル

//マクロ定義
#define KEY_KIND_MAX 256	//取得するキーの種類
#define MARGIN 50			//画面端の余白

//キーボード構造体
struct KEYBOARD
{
	char TempKeyState[KEY_KIND_MAX];	//入力状態を管理
	int AllKeyState[KEY_KIND_MAX];		//すべてのキーの入力時間を管理
	int OldKeyState[KEY_KIND_MAX];		//以前のすべてのキーの入力時間を管理
};

//外部のグローバル変数
extern KEYBOARD keyboard;

//プロトタイプ宣言
extern VOID AllKeyUpdate(VOID);		//すべてのキーの状態を取得する
extern BOOL KeyDown(int KEY_INPUT_);	//特定のキーを押しているか
extern BOOL KeyUp(int KEY_INPUT_);		//特定のキーを上げているか
extern BOOL KeyClick(int KEY_INPUT_);	//特定のキーをクリックしたか
extern BOOL KeyDownKeep(int KEY_INPUT_, int MilliTime);	//特定のキーを指定のミリ秒分押し続けているか
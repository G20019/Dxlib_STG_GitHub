// ヘッダファイル読み込み
#include "game.h"		// ゲーム全体のヘッダファイル
#include "keyboard.h"	// キーボードの処理
#include "FPS.h"		// FPSの処理

#include <math.h>		// 数学

// マクロ定義
#define TAMA_DIV_MAX	4	// 弾の画像の最大数
#define TAMA_MAX		100	// 弾の総数
#define TEKI_KIND		8	// 敵の種類
#define TEKI_MAX		10	// 敵の数

// 構造体の定義
// 画像の構造体
struct IMAGE
{
	int handle = -1;	// 画像のハンドル（管理番号）
	char path[255];		// 画像の場所（パス）

	int x = 0;			// X位置
	int y = 0;			// Y位置
	int width = 0;		// 幅
	int height = 0;		// 高さ

	BOOL IsDraw = FALSE;// 画像が描画できる？
};

// キャラクタの構造体
struct CHARACTOR
{
	IMAGE img;			// 画像構造体
	int speed = 10;		// 移動速度
	RECT coll;			// 当たり判定の領域（四角）
};

// 動画の構造体
struct MOVIE
{
	int handle = -1;	// 動画のハンドル
	char path[255];		// 動画のパス

	int x = 0;			// X位置
	int y = 0;			// Y位置
	int width = 0;		// 幅
	int height = 0;		// 高さ

	int volume = 255;	// ボリューム(MIN 0 ～ 255 MAX)
};

// 音楽の構造体
struct AUDIO
{
	int handle = -1;	// 音楽のハンドル
	char path[255];		// 音楽のパス

	int volume = -1;	// ボリューム(MIN 0 ～ 255 MAX)
	int playType = -1;	// BGM or SE
};

// 弾の構造体
struct TAMA
{
	int handle[TAMA_DIV_MAX];	// 弾のハンドル
	char path[255];		// 画像のパス

	int DivTate;				// 分割数（縦）
	int DivYoko;				// 分割数（横）
	int DivMax;					// 分割総数

	int AnimeCnt = 0;			// アニメーションカウンタ
	int AnimeCntMax = 0;		// アニメーションカウンタMAX

	int NowIndex = 0;			// 現在の画像の要素数

	int startX;					// X最初位置
	int startY;					// Y最初位置

	float radius;				// 半径
	float degree;				// 角度

	int x = 0;					// X位置
	int y = 0;					// Y位置
	int width = 0;				// 幅
	int height = 0;				// 高さ

	int speed = 0;				// 速度

	RECT coll;					// 当たり判定（矩形）

	BOOL isDraw = FALSE;		// 描画できるか
};

// グローバル変数
// シーンを管理する変数
GAME_SCENE GameScene;		// 現在のゲームのシーン
GAME_SCENE OldGameScene;	// 前回のゲームのシーン
GAME_SCENE NextGameScene;	// 次のゲームのシーン

// 画面の切り替え
BOOL IsFadeOut = FALSE;	// フェードアウト
BOOL IsFadeIn = FALSE;	// フェードイン

int fadeTimeMill = 2000;					// 切り替えミリ秒
int fadeTimeMax = fadeTimeMill / 1000 * 60;	// ミリ秒をフレーム秒に変換

// フェードアウト
int fadeOutCntInit = 0;				// 初期値
int fadeOutCnt = fadeOutCntInit;	// フェードアウトのカウンタ
int fadeOutCntMax = fadeTimeMax;	// フェードアウトのカウンタMAX

// フェードイン
int fadeInCntInit = fadeTimeMax;	// 初期値
int fadeInCnt = fadeInCntInit;		// フェードインのカウンタ
int fadeInCntMax = fadeTimeMax;		// フェードインのカウンタMAX

// 弾の構造体変数
struct TAMA tama_moto;		// 元
struct TAMA tama[TAMA_MAX];	// 実際に使う

// 弾の発射カウンタ
int tamaShotCnt = 0;
int tamaShotCntMax = 1;

// プレイヤー
CHARACTOR player;

// 背景画像
IMAGE back[2];	// 背景は2つの画像

// 敵データ(元）
CHARACTOR teki_moto[TEKI_KIND];

// 実際の敵データ
CHARACTOR teki[TEKI_MAX];

// 敵データのパス
char tekiPath[TEKI_KIND][255] =
{
	{".\\Image\\teki_blue.png"},
	{".\\Image\\teki_gray.png"},
	{".\\Image\\teki_green.png"},
	{".\\Image\\teki_mizu.png"},
	{".\\Image\\teki_purple.png"},
	{".\\Image\\teki_red.png"},
	{".\\Image\\teki_red_big.png"},
	{".\\Image\\teki_yellow.png"},
};

// 敵が出てくるカウント
int tekiAddCnt = 0;
int tekiAddCntMax = 60;	// 60FPSで1回・・・1秒に一回

// ゲームのスコア
int score = 0;

// プロトタイプ宣言
VOID Title(VOID);		// タイトル画面
VOID TitleProc(VOID);	// タイトル画面（処理）
VOID TitleDraw(VOID);	// タイトル画面（描画）

VOID Play(VOID);		// プレイ画面
VOID PlayProc(VOID);	// プレイ画面（処理）
VOID PlayDraw(VOID);	// プレイ画面（描画）

VOID End(VOID);			// エンド画面
VOID EndProc(VOID);		// エンド画面（処理）
VOID EndDraw(VOID);		// エンド画面（描画）

VOID Change(VOID);		// 切り替え画面
VOID ChangeProc(VOID);	// 切り替え画面（処理）
VOID ChangeDraw(VOID);	// 切り替え画面（描画）

VOID ChangeScene(GAME_SCENE scene);		// シーン切り替え

VOID CollUpdatePlayer(CHARACTOR* chara);	// プレイヤーの当たり判定の領域更新
VOID CollUpdateTeki(CHARACTOR* chara);		// 敵の当たり判定の領域更新
VOID CollUpdate(CHARACTOR* chara);			// 当たり判定の領域更新
VOID CollUpdateTama(TAMA* tama);			// 弾の当たり判定の更新

BOOL OnCollRect(RECT object1, RECT object2);	// 矩形と矩形の当たり判定

BOOL GameLoad(VOID);	// ゲームのデータを読み込み

BOOL LoadImageMem(IMAGE* image, const char* path);								//画像のデータを読み込み
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);		// 音楽のデータを読み込み
BOOL LoadImageDivMem(int* handle, const char* path, int divYoko, int divTate);	// ゲームの画像の分割読み込み

VOID GameInit(VOID);	// ゲームのデータの初期化

VOID DrawTama(TAMA* tama);	// 弾の描画
VOID ShotTama(TAMA* tama, float deg);	// 弾の発射


// プログラムは WinMain から始まる
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);				// Log.txtを出力しない
	ChangeWindowMode(TRUE);								// ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);						// ウィンドウのタイトルの文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	// ウィンドウの解像度を設定
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);				// ウィンドウの大きさを設定
	SetBackgroundColor(255, 255, 255);					// デフォルトの背景の色
	SetWindowIconID(GAME_ICON_ID);						// アイコンファイルを読み込み
	SetWindowStyleMode(GAME_WINDOW_BAR);				// ウィンドウバーの状態
	SetWaitVSyncFlag(TRUE);								// ディスプレイの垂直同期を有効にする
	SetAlwaysRunFlag(TRUE);								// ウィンドウをずっとアクティブにする

	if (DxLib_Init() == -1)	// ＤＸライブラリ初期化処理
	{
		return -1;	// エラーが起きたら直ちに終了
	}

	// ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);

	// 最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	// ゲーム全体の初期化

	// ゲーム読み込み
	if (!GameLoad())
	{
		// ゲームの読み込みに失敗したとき
		DxLib_End();	// DxLib終了
		return -1;		// 異常終了
	}

	// ゲームの初期化
	GameInit();
	
	// 無限ループ
	while (1) 
	{
		if (ProcessMessage() != 0) { break; }	// メッセージを受け取り続ける
		if (ClearDrawScreen() != 0) { break; }	// 画面を消去する

		// キーボード入力の更新
		AllKeyUpdate();

		// FPS値の更新
		FPSUpdate();

		// ESCキーで強制終了
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		// 以前のシーンを取得
		if (GameScene != GAME_SCENE_CHANGE)
		{
			OldGameScene = GameScene;
		}

		// シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:
			Title();			//タイトル画面
			break;
		case GAME_SCENE_PLAY:
			Play();				// プレイ画面
			break;
		case GAME_SCENE_END:
			End();				// エンド画面
			break;
		case GAME_SCENE_CHANGE:
			Change();			// 切り替え画面
			break;
		default:
			break;
		}

		// シーンを切り替える
		if (OldGameScene != GameScene)
		{
			// 現在のシーンが切り替え画面でないとき
			if (GameScene != GAME_SCENE_CHANGE)
			{
				NextGameScene = GameScene;		// 次のシーンを保存
				GameScene = GAME_SCENE_CHANGE;	// 画面切り替えシーンに変える
			}
		}

		// FPS値を描画
		FPSDraw();

		// FPS値を待つ
		FPSWait();

		// ダブルバッファリングした画面を描画
		ScreenFlip();
	}

	// 読み込んだ画像を開放
	for (int i = 0; i < TAMA_DIV_MAX; i++) { DeleteGraph(tama_moto.handle[i]); }

	// プレイヤー解放
	DeleteGraph(player.img.handle);

	// 背景画像開放
	DeleteGraph(back[0].handle);
	DeleteGraph(back[1].handle);

	// 敵の画像を開放
	for (int i = 0; i < TEKI_KIND; i++)
	{
		DeleteGraph(teki_moto[i].img.handle);
	}


	// ＤＸライブラリ使用の終了処理
	DxLib_End();

	return 0;	// ソフトの終了 
}

/// <summary>
/// ゲームデータの読み込み
/// </summary>
/// <returns>読み込めたらTRUE / 読み込めなかったらFALSE</returns>
BOOL GameLoad(VOID) 
{	
	// 弾の分割数を設定
	tama_moto.DivYoko = 4;
	tama_moto.DivTate = 1;

	// 弾のパス
	strcpyDx(tama_moto.path, ".\\Image\\marudai_purple.png");

	// 画像を分割して読み込み
	if (LoadImageDivMem(&tama_moto.handle[0], tama_moto.path, tama_moto.DivYoko, tama_moto.DivTate) == FALSE) { return FALSE; }

	// 幅と高さを取得
	GetGraphSize(tama_moto.handle[0], &tama_moto.width, &tama_moto.height);
	
	// 位置を設定
	tama_moto.x = GAME_WIDTH / 2 - tama_moto.width / 2;	// 中央揃え
	tama_moto.y = GAME_HEIGHT - tama_moto.height;		// 画面下

	// 速度
	tama_moto.speed = 5;

	// アニメを変える速度
	tama_moto.AnimeCntMax = 10;

	// 当たり判定を設定
	CollUpdateTama(&tama_moto);

	// 画像を表示しない
	tama_moto.isDraw = FALSE;

	// 全ての弾に情報をコピー
	for (int i = 0; i < TAMA_MAX; i++) {
		tama[i] = tama_moto;
	}

	// プレイヤーの画像を読み込み
	if (LoadImageMem(&player.img, ".\\Image\\player.png") == FALSE) { return FALSE; }
	player.img.x = GAME_WIDTH / 2 - player.img.width;
	player.img.y = GAME_HEIGHT / 2 - player.img.height;
	CollUpdatePlayer(&player);	// 当たり判定の更新
	player.img.IsDraw = TRUE;	// 描画する

	// 背景の画像を読み込み①
	if (LoadImageMem(&back[0], ".\\Image\\hoshi.png") == FALSE) { return FALSE; }
	back[0].x = 0;
	back[0].y = -back[0].height;	//	画像の高さ文、位置を上にあげる
	back[0].IsDraw = TRUE;	// 描画する

	// 背景の画像を読み込み②
	if (LoadImageMem(&back[1], ".\\Image\\hoshi_rev.png") == FALSE) { return FALSE; }
	back[1].x = 0;
	back[1].y = 0;
	back[1].IsDraw = TRUE;	// 描画する

	// 敵の画像を読み込み
	for (int i = 0; i < TEKI_KIND; i++)
	{
		if (LoadImageMem(&teki_moto[i].img, tekiPath[i]) == FALSE) { return FALSE; }
		teki_moto[i].img.x = GAME_WIDTH / 2 - teki_moto[i].img.width;
		teki_moto[i].img.y = -teki_moto[i].img.height;
		CollUpdatePlayer(&teki_moto[i]);	// 当たり判定の更新
		teki_moto[i].img.IsDraw = FALSE;	// 描画しない
	}

	return TRUE;	//すべて読み込めた
}

/// <summary>
/// 画像を分割してメモリに読み込み
/// </summary>
/// <param name="handle">ハンドル配列の先頭アドレス</param>
/// <param name="path">画像のパス</param>
/// <param name="divYoko">分割するときの横の数</param>
/// <param name="divTate">分割するときの縦の数</param>
/// <returns></returns>
BOOL LoadImageDivMem(int *handle,const char *path,int divYoko, int divTate)
{
	// 弾の読み込み
	int isTamaLoad = -1;	// 画像が読み込めたか

	// 一時的に画像のハンドルを用意する
	int tamaHandle = LoadGraph(path);

	// 読み込みエラー
	if (tamaHandle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	// ウィンドウハンドル
			path,					// 本文
			"画像読み込みエラー",	// タイトル
			MB_OK					// ボタン
		);

		return FALSE;	// 読み込み失敗
	}

	// 画像の幅と高さを取得
	int tamaWidth = -1;		// 幅
	int tamaHeight = -1;	// 高さ
	GetGraphSize(tamaHandle, &tamaWidth, &tamaHeight);

	// 分割して読み込み
	isTamaLoad = LoadDivGraph(
		path,										// 画像のパス
		TAMA_DIV_MAX,								// 分割総数
		divYoko, divTate,							// 横の分割
		tamaWidth / divYoko, tamaHeight / divTate,	// 画像一つ分の幅、高さ
		handle										// 連続で管理する配列の先頭アドレス
	);

	// 分割エラー
	if (isTamaLoad == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	// ウィンドウハンドル
			path,					// 本文
			"画像分割エラー",		// タイトル
			MB_OK					// ボタン
		);

		return FALSE;	// 読み込み失敗
	}

	// 一時的に読み込んだハンドルを解放
	DeleteGraph(tamaHandle);

	return TRUE;
}

/// <summary>
/// 画像をメモリに読み込み
/// </summary>
/// <param name="image">画像構造体のアドレス</param>
/// <param name="path">画像のパス</param>
/// <returns></returns>
BOOL LoadImageMem(IMAGE* image, const char* path)
{
	// 画像の読み込み
	strcpyDx(image->path, path);			// パスのコピー
	image->handle = LoadGraph(image->path);	// 画像の読み込み

	// 画像が読み込めなかったときは、エラー（－1）が入る
	if (image->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	// メインのウィンドウハンドル
			image->path,			// メッセージ本文
			"画像読み込みエラー",	// メッセージタイトル
			MB_OK					// ボタン
		);

		return FALSE;		// 読み込み失敗
	}

	// 画像の幅と高さを取得
	GetGraphSize(image->handle, &image->width, &image->height);

	// 読み込めた
	return TRUE;
}

/// <summary>
/// 音楽をメモリに読み込み
/// </summary>
/// <param name="audio">AUDIO構造体変数のアドレス</param>
/// <param name="path">AUDIO構造体変数のパス</param>
/// <param name="volume">ボリューム</param>
/// <param name="playType">Dx_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	// 音楽の読み込み
	strcpyDx(audio->path, path);				// パスのコピー
	audio->handle = LoadSoundMem(audio->path);	// 音楽の読み込み

	// 音楽が読み込めなかったときは、エラー（－1）が入る
	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	// メインのウィンドウハンドル
			audio->path,			// メッセージ本文
			"音楽読み込みエラー",	// メッセージタイトル
			MB_OK					// ボタン
		);

		return FALSE;		// 読み込み失敗
	}

	// その他の設定
	audio->volume = volume;
	audio->playType = playType;

	// 読み込めた
	return TRUE;
}

/// <summary>
/// ゲームデータを初期化
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{
	// ゲームスコアを初期化
	score = 0;

	// プレイヤーの初期化
	player.img.x = GAME_WIDTH / 2 - player.img.width;
	player.img.y = GAME_HEIGHT / 2 - player.img.height;
	CollUpdatePlayer(&player);	// 当たり判定の更新
	player.img.IsDraw = TRUE;	// 描画する

	// 背景の画像を設定①
	back[0].x = 0;
	back[0].y = -back[0].height;	//	画像の高さ分、位置を上にあげる
	back[0].IsDraw = TRUE;	// 描画する

	// 背景の画像を設定②
	back[1].x = 0;
	back[1].y = 0;
	back[1].IsDraw = TRUE;	// 描画する

	// 敵の初期化
	for (int i = 0; i < TEKI_KIND; i++)
	{
		teki_moto[i].img.x = GAME_WIDTH / 2 - teki_moto[i].img.width;
		teki_moto[i].img.y = -teki_moto[i].img.height;
		CollUpdateTeki(&teki_moto[i]);		// 当たり判定の更新
		teki_moto[i].img.IsDraw = FALSE;	// 描画しない
	}
}

/// <summary>
/// シーンを切り替える関数
/// </summary>
/// <param name="scene">シーン</param>
VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;	// プレイ画面に切り替え
	IsFadeIn = FALSE;	// フェードインしない
	IsFadeOut = TRUE;	// フェードアウトする

	return;
}

/// <summary>
/// タイトル画面
/// </summary>
VOID Title(VOID)
{
	TitleProc();	// 処理
	TitleDraw();	// 描画

	return;
}

/// <summary>
/// タイトル画面の処理
/// </summary>
VOID TitleProc(VOID)
{	
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		// シーン切り替え
		// 次のシーンの初期化をここで行うと楽

		// ゲームの初期化
		GameInit();

		// プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	// タイトル背景の描画
	DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));

	return;
}

/// <summary>
/// 弾の描画
/// </summary>
/// <param name="tama">弾の構造体</param>
VOID DrawTama(TAMA* tama)
{
	// 弾の描画ができるとき
	if (tama->isDraw == TRUE)
	{
		// 弾の描画
		DrawGraph(tama->x, tama->y, tama->handle[tama->NowIndex], TRUE);

		// 画像を変えるタイミング
		if (tama->AnimeCnt < tama->AnimeCntMax)
		{
			tama->AnimeCnt++;
		}
		else
		{
			// 弾の添字が弾の分割数の最大よりも下のとき
			if (tama->NowIndex < TAMA_DIV_MAX - 1)
			{
				tama->NowIndex++;	// 次の画像へ
			}
			else
			{
				tama->NowIndex = 0;	// 最初に戻す
			}
			tama->AnimeCnt = 0;
		}
	}
}

/// <summary>
/// プレイ画面
/// </summary>
VOID Play(VOID)
{
	PlayProc();	// 処理
	PlayDraw();	// 描画

	return;
}

/// <summary>
/// プレイ画面の処理
/// </summary>
VOID PlayProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		// シーン切り替え
		// 次のシーンの初期化をここで行うと楽

		if (KeyClick(KEY_INPUT_RETURN) == TRUE) {
			// エンド画面に切り替え
			ChangeScene(GAME_SCENE_END);
		}

		return;
	}

	// プレイヤーを操作する
	if (KeyDown(KEY_INPUT_A) == TRUE)
	{
		if (player.img.x - player.speed >= 0)
			player.img.x -= player.speed;
	}
	if (KeyDown(KEY_INPUT_D) == TRUE)
	{
		if (player.img.x + player.speed <= GAME_WIDTH - player.img.width)
			player.img.x += player.speed;
	}
	if (KeyDown(KEY_INPUT_W) == TRUE)
	{
		if (player.img.y - player.speed >= 0)
			player.img.y -= player.speed;
	}
	if (KeyDown(KEY_INPUT_S) == TRUE)
	{
		if (player.img.y + player.speed <= GAME_HEIGHT - player.img.height)
			player.img.y += player.speed;
	}

	// プレイヤーの当たり判定を更新
	CollUpdatePlayer(&player);

	// スペースキーを押しているとき
	if (KeyDown(KEY_INPUT_SPACE) == TRUE)
	{
		if (tamaShotCnt == 0)
		{
			// 弾を発射する（描画する）
			for (int i = 0; i < TAMA_MAX; i++) {
				if (tama[i].isDraw == FALSE)
				{
					ShotTama(&tama[i], 270.0f);

					// 弾を一発出したら、ループを抜ける
					break;
				}
			}
			for (int i = 0; i < TAMA_MAX; i++) {
				if (tama[i].isDraw == FALSE)
				{
					ShotTama(&tama[i], 260.0f);

					// 弾を一発出したら、ループを抜ける
					break;
				}
			}
			for (int i = 0; i < TAMA_MAX; i++) {
				if (tama[i].isDraw == FALSE)
				{
					ShotTama(&tama[i], 2800.0f);

					// 弾を一発出したら、ループを抜ける
					break;
				}
			}
		}

		// 弾の発射待ち
		if (tamaShotCnt < tamaShotCntMax)
		{
			tamaShotCnt++;
		}
		else
		{
			tamaShotCnt = 0;
		}
	}

	// 弾を飛ばす
	for (int i = 0; i < TAMA_MAX; i++) {

		// 描画されているとき
		if (tama[i].isDraw == TRUE)
		{
			// 弾の位置を修正
			// 中心距離　＋　飛ばす角度→飛ばす距離を計算　＊　距離
			tama[i].x = tama[i].startX + cos(tama[i].degree * DX_PI / 180.0f) * tama[i].radius;
			tama[i].y = tama[i].startY + sin(tama[i].degree * DX_PI / 180.0f) * tama[i].radius;

			// 半径を足す
			tama[i].radius += tama[i].speed;

			// 弾の当たり判定を更新
			CollUpdateTama(&tama[i]);

			// 画面外に出たら、描画しない
			if (tama[i].y + tama[i].height < 0 ||	// 画面外（上）
				tama[i].y > GAME_HEIGHT ||			// 画面外（下）
				tama[i].x + tama[i].width < 0 ||	// 画面外（左）
				tama[i].x > GAME_WIDTH)				// 画面外（右）
			{
				tama[i].isDraw = FALSE;
			}
		}
	}

	if (tekiAddCnt < tekiAddCntMax)
	{
		tekiAddCnt++;
	}
	else
	{
		// 敵を生成
		for (int i = 0; i < TEKI_MAX; i++)
		{
			// 描画されていない敵を探す
			if (teki[i].img.IsDraw == FALSE)
			{
				int bunkatu = 10;	// 画面の横分割数
				
				if (score < 1000)
				{
					teki[i] = teki_moto[0];
				}
				else if (score < 2000)
				{
					teki[i] = teki_moto[1];
				}
				else
				{
					teki[i] = teki_moto[GetRand(TEKI_KIND - 1)];
				}

				teki[i].img.x = GetRand(bunkatu - 1) * GAME_WIDTH / bunkatu;
				teki[i].img.y = -teki[i].img.height;

				teki[i].img.IsDraw = TRUE;	// 描画する
				break;
			}
		}
	}

	// 敵の処理
	for (int i = 0; i < TEKI_MAX; i++)
	{
		// 描画している敵
		if (teki[i].img.IsDraw == TRUE)
		{
			teki[i].img.y += 1;	// とりあえず下へ移動

			// 敵の当たり判定を更新
			CollUpdateTeki(&teki[i]);

			// 敵が下まで行ったら表示しない
			if (teki[i].img.y > GAME_HEIGHT)
			{
				teki[i].img.IsDraw == FALSE;
			}

			// 敵と自分の弾が当たった時
			for (int cnt = 0; cnt < TAMA_MAX; cnt++)
			{
				// 描画されているとき
				if (tama[cnt].isDraw == TRUE)
				{
					// 当たり判定
					if (OnCollRect(teki[i].coll, tama[cnt].coll) == TRUE)
					{
						tama[cnt].isDraw = FALSE;		// 弾の描画はしない
						teki[i].img.IsDraw = FALSE;	// 敵の描画もしない
						score += 100;					// スコア加算
					}
				}
			}
		}
	}

	return;
}

/// <summary>
/// 弾を飛ばす
/// </summary>
VOID ShotTama(TAMA* tama,float deg)
{
		// 弾を発射する（描画する）
		tama->isDraw = TRUE;

		// 弾の位置を決める
		tama->startX = player.img.x + player.img.width / 2 - tama->width / 2;
		tama->startY = player.img.y;

		// 弾の速度を変える
		tama->speed = 30;

		// 弾の角度
		tama->degree = deg;

		// 弾の半径
		tama->radius = 0.0f;

		// 弾の当たり判定の更新
		CollUpdateTama(tama);
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	// 背景の描画
	for (int i = 0; i < 2; i++)
	{
		// 描画
		DrawGraph(back[i].x, back[i].y, back[i].handle, TRUE);

		// 画像が下まで行ったとき
		if (back[i].y > GAME_HEIGHT)
		{
			back[i].y = -back[i].height + 1;	// 高さ文、上に戻す
		}

		// 画像を下に動かす
		back[i].y++;
	}

	// 敵の描画
	for (int i = 0; i < TEKI_MAX; i++)
	{
		if (teki[i].img.IsDraw == TRUE)
		{
			DrawGraph(teki[i].img.x, teki[i].img.y, teki[i].img.handle, TRUE);
		}

		// 当たり判定の描画
		if (GAME_DEBUG == TRUE)
		{
			DrawBox(
				teki[i].coll.left, teki[i].coll.top, teki[i].coll.right, teki[i].coll.bottom,
				GetColor(0, 0, 255), FALSE);
		}
	}

	// プレイヤーの描画
	if (player.img.IsDraw == TRUE)
	{
		// プレイヤーの描画
		DrawGraph(player.img.x, player.img.y, player.img.handle, TRUE);

		// 当たり判定の描画
		if (GAME_DEBUG == TRUE)
		{
			DrawBox(
				player.coll.left, player.coll.top, player.coll.right, player.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}


	// 弾の描画
	for (int i = 0; i < TAMA_MAX; i++)
	{
		// 描画されているとき
		if (tama[i].isDraw == TRUE)
		{
			DrawTama(&tama[i]);

			// 当たり判定の描画
			if (GAME_DEBUG == TRUE)
			{
				DrawBox(
					tama[i].coll.left, tama[i].coll.top, tama[i].coll.right, tama[i].coll.bottom,
					GetColor(255, 0, 0), FALSE);
			}
		}
	}

	// スコアの描画
	int old = GetFontSize();	// 以前のフォントのサイズを取得
	SetFontSize(40);			// フォントを大きくする
	DrawFormatString(0, 100, GetColor(255, 255, 255), "SCORE:%05d", score);
	SetFontSize(old);			// フォントを元に戻す

	DrawString(0, 0, "プレイ画面", GetColor(0, 0, 0));

	return;
}

/// <summary>
/// エンド画面
/// </summary>
VOID End(VOID)
{
	EndProc();	// 処理
	EndDraw();	// 描画

	return;
}

/// <summary>
/// エンド画面の処理
/// </summary>
VOID EndProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		// シーン切り替え
		// 次のシーンの初期化をここで行うと楽

		// タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);
		
		return;
	}

	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	DrawString(0, 0, "エンド画面", GetColor(0, 0, 0));

	return;
}

/// <summary>
/// 切り替え画面
/// </summary>
VOID Change(VOID)
{
	ChangeProc();	// 処理
	ChangeDraw();	// 描画

	return;
}

/// <summary>
/// 切り替え画面の処理
/// </summary>
VOID ChangeProc(VOID)
{
	// フェードイン
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMax)
		{
			fadeInCnt--;	// カウンタを減らす
		}
		else
		{
			// フェードイン処理が終わった

			fadeInCnt = fadeInCntInit;	// カウンタ初期化
			IsFadeIn = FALSE;			// フェードイン処理終了
		}
	}

	// フェードアウト
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMax)
		{
			fadeOutCnt++;	// カウンタを増やす
		}
		else
		{
			// フェードアウト処理が終わった

			fadeOutCnt = fadeOutCntInit;	// カウンタ初期化
			IsFadeOut = FALSE;			// フェードアウト処理終了
		}
	}

	// 切り替え処理終了
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		// フェードインしていない、フェードアウトもしていないとき
		GameScene = NextGameScene;	// 次のシーンに切り替え
		OldGameScene = GameScene;	// 以前のゲームシーン更新
	}

	return;
}

/// <summary>
/// 切り替え画面の描画
/// </summary>
VOID ChangeDraw(VOID)
{
	// 以前のシーンを描画
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();	// タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();		// プレイ画面の描画
		break;
	case GAME_SCENE_END:
		EndDraw();		// エンド画面の描画
		break;
	default:
		break;
	}

	// フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMax) * 255);
	}
	// フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMax) * 255);
	}

	// 四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0),TRUE);

	// 半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 0, "切り替え画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 当たり判定の領域更新（プレイヤー）
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdatePlayer(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x + 40;
	chara->coll.top = chara->img.y + 40;
	chara->coll.right = chara->img.x + chara->img.width - 40;
	chara->coll.bottom = chara->img.y + chara->img.height - 10;

	return;
}

/// <summary>
/// 当たり判定の領域更新（敵）
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdateTeki(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;
	chara->coll.right = chara->img.x + chara->img.width;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}


/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdate(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;

	chara->coll.right = chara->img.x + chara->img.width;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}

/// <summary>
/// 当たり判定の領域更新（弾）
/// </summary>
/// <param name="chara">弾の構造体</param>
VOID CollUpdateTama(TAMA* tama)
{
	tama->coll.left = tama->x;
	tama->coll.top = tama->y;

	tama->coll.right = tama->x + tama->width;
	tama->coll.bottom = tama->y + tama->height;

	return;
}

/// <summary>
/// 矩形と矩形の当たり判定
/// </summary>
/// <param name="object1">矩形１</param>
/// <param name="object2">矩形２</param>
/// <returns>当たった時true/当たっていないときfalse</returns>
BOOL OnCollRect(RECT object1,RECT object2)
{
	if (
		object1.left < object2.right &&		// 矩形Aの左辺X座標 <　矩形Bの右辺X座標　かつ
		object1.right > object2.left &&		// 矩形Aの右辺X座標 >　矩形Bの左辺X座標　かつ
		object1.top < object2.bottom &&		// 矩形Aの上辺Y座標 <　矩形Bの下辺Y座標　かつ
		object1.bottom > object2.top		// 矩形Aの下辺Y座標 >　矩形Bの上辺Y座標
		)
	{
		// 当たっているとき
		return TRUE;
	}

	// 当たっていないとき
	return FALSE;
}
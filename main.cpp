/*--+----1----+----2----+----3----+----4----+----5-----+----6----+----7----+----8----+----9----+---*/
//無限ループ

//########## ヘッダーファイル読み込み ##########
#include "DxLib.h"

//########## マクロ定義 ##########
#define GAME_WIDTH	1000	//画面の横の大きさ
#define GAME_HEIGHT	700	//画面の縦の大きさ
#define GAME_COLOR	32	//画面のカラービット

#define GAME_WINDOW_BAR	0	//タイトルバーはデフォルトにする
#define GAME_WINDOW_NAME	"GAME TITLE"	//ウィンドウのタイトル

#define GAME_FPS	60  //FPSの数値

//パスの長さ
#define PATH_MAX   255
#define NAME_MAX   255

//エラーメッセージ
#define IMAGE_LOAD_ERR_TITLE    TEXT("画像読み込みエラー")

#define IMAGE_START_IMAGE_PATH  TEXT(".\\IMAGE\\スタート画面.png")  //背景の画像
#define IMAGE_PLAY_IMAGE_PATH   TEXT(".\\IMAGE\\森の中.png")  //背景の画像
#define IMAGE_MENU_IMAGE_PATH   TEXT(".\\IMAGE\\操作説明.png")  //背景の画像
#define IMAGE_MENU_BK_PATH      TEXT(".\\IMAGE\\menu_背景.png")

#define GAME_animal1_CHIP_PATH  TEXT(".\\IMAGE\\animal\\mapchip_1.png")  //チップの画像
#define ANIMAL_MAX				4
#define ANIMAL_CHANGE_MAX		6

#define CHIP_DIV_WIDTH			565   //画像を分割する幅サイズ
#define CHIP_DIV_HEIGHT			660   //画像を分割する高さサイズ
#define GAME_animal1_DIV_TATE   2     //画像を縦に分割する数
#define GAME_animal1_DIV_YOKO   2     //画像を横に分割する数
#define CHIP_DIV_NUM GAME_animal1_DIV_TATE * GAME_animal1_DIV_YOKO  //画像を分割する総数

enum GAME_SCENE {
	GAME_SCENE_START,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
	GAME_SCENE_MENU,
};   //ゲームのシーン

enum SPEED {
	SPEED_LOW = 1,
	SPEED_HIGH = 3
};  //移動スピード

typedef struct STRUCT_I_POINT
{
	int x = -1;
	int y = -1;
}iPOINT;

typedef struct STRUCT_IMAGE
{
	char path[PATH_MAX];	//パス
	int handle;				//ハンドル
	int x;					//X位置
	int y;					//Y位置
	int width;				//幅
	int height;				//高さ
}IMAGE; //画像構造体

typedef struct STRUCT_ANIMAL
{
	char path[PATH_MAX];		//パス
	int handle[CHIP_DIV_NUM];   //分割した動物の画像ハンドルを取得
	int x;						//X位置
	int y;						//Y位置
	int width;					//幅
	int height;					//高さ
	BOOL IsDraw;				//動物を表示できるか
	int nowImageKind;			//動物の現在の画像
	int changeImageCnt;			//画像を変えるためのもの
	int changeImageCntMAX;
	int speed;
}MAPCHIP;

//グローバル変数
int StartTimeFps;				//測定開始時刻
int CountFps;					//カウンタ
float CalcFps;					//計算結果
int SampleNumFps = GAME_FPS;	//平均をとるサンプル数

//キーボードの入力を取得
char AllKeyState[256] = { 0 };
char OldAllKeyState[256] = { 0 };

int GameScene;

MAPCHIP animal[ANIMAL_MAX];
int GHandle[ANIMAL_MAX];

//画像関連
IMAGE ImageSTARTBK;   //ゲームの背景
IMAGE ImagePLAYENDBK;
IMAGE ImageMENU;
IMAGE ImageMENUBK;

//プロトタイプ宣言
VOID MY_FPS_UPDATE(VOID);		//FPS値を計測、更新する関数
VOID MY_FPS_DRAW(VOID);			//FPS値を描画する関数
VOID MY_FPS_WAIT(VOID);			//FPS値を計測し。待つ関数

VOID MY_ALL_KEYDOWN_UPDATE(VOID);  //キーの入力状態を更新する
BOOL MY_KEY_DOWN(int);			   //キーを押しているか、キーコードで判断する
BOOL MY_KEY_UP(int);			   //キーを押し上げたか、キーコードで判断する
BOOL MY_KEYDOWN_KEEP(int, int);    //キーを押し続けているか、キーコードで判断する

VOID MY_START(VOID);		//スタート画面
VOID MY_START_PROC(VOID);   //スタート画面の処理
VOID MY_START_DRAW(VOID);   //スタート画面の描画

VOID MY_PLAY(VOID);		   //プレイ画面
VOID MY_PLAY_PROC(VOID);   //プレイ画面の処理
VOID MY_PLAY_DRAW(VOID);   //プレイ画面の描画

VOID MY_END(VOID);		  //エンド画面
VOID MY_END_PROC(VOID);   //エンド画面の処理
VOID MY_END_DRAW(VOID);   //エンド画面の描画

VOID MY_MENU(VOID);       //操作説明画面
VOID MY_MENU_PROC(VOID);  //操作説明画面の処理
VOID MY_MENU_DRAW(VOID);  //操作説明画面の描画

BOOL MY_LOAD_IMAGE(VOID);    //画像をまとめて読み込む関数
VOID MY_DELETE_IMAGE(VOID);  //画像をまとめて削除する関数

//########## プログラムで最初に実行される関数 ##########
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);						//ウィンドウモードに設定
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//指定の数値でウィンドウを表示する
	SetWindowStyleMode(GAME_WINDOW_BAR);		//タイトルバーはデフォルトにする
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));	//ウィンドウのタイトルの文字
	SetAlwaysRunFlag(TRUE);						//非アクティブでも実行する

	if (DxLib_Init() == -1) { return -1; }	//ＤＸライブラリ初期化処理

	//画像を読み込む
	if (MY_LOAD_IMAGE() == FALSE) { return -1; }

	GameScene = GAME_SCENE_START;   //ゲームシーンはスタート画面から
	
	SetDrawScreen(DX_SCREEN_BACK);	//Draw系関数は裏画面に描画

	//無限ループ
	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }	//メッセージ処理の結果がエラーのとき、強制終了

		if (ClearDrawScreen() != 0) { break; }	//画面を消去できなかったとき、強制終了

		MY_ALL_KEYDOWN_UPDATE();       //押しているキー状態を取得する

		MY_FPS_UPDATE();   //FPSの処理(更新)

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_START:
			MY_START();   //スタート画面
			break;
		case GAME_SCENE_PLAY:
			MY_PLAY();    //プレイ画面
			break;
		case GAME_SCENE_END:
			MY_END();     //エンド画面
			break;
		case GAME_SCENE_MENU:
			MY_MENU();    //操作説明画面
			break;
		}

		//DrawString(DrawX, DrawY, "Hello World", GetColor(255, 255, 255));	//文字を描画

		MY_FPS_DRAW();	   //FPSの処理(描画)

		ScreenFlip();		//モニタのリフレッシュレートの速さで裏画面を再描画

		MY_FPS_WAIT();     //FPSの処理(待つ)
	}
	//▲▲▲▲▲ プログラム追加ここまで ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲

	//▽▽▽▽▽ プログラム削除ここから ▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
	//DrawString(DrawX, DrawY, "Hello World", GetColor(255, 255, 255));	//文字を描画
	//WaitKey();	//キー入力待ち
	//△△△△△ プログラム削除ここまで △△△△△△△△△△△△△△△△△△△△

	//画像ハンドルを破棄
	MY_DELETE_IMAGE();

	DxLib_End();	//ＤＸライブラリ使用の終了処理

	return 0;
}

//FPS値を計測、待つ関数
VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0)  //1フレーム目なら時刻を記憶
	{
		StartTimeFps = GetNowCount();
	}

	if (CountFps == SampleNumFps)   //60フレーム目なら平均を計算
	{
		int now = GetNowCount();
		//現在の時間から、0フレームの時間を引き、FPSの数値で割る = 1FPS辺りの平均時間が計算される
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);
		CountFps = 0;
		StartTimeFps = now;
	}
	CountFps++;
	return;
}

//FPS値を描画する関数
VOID MY_FPS_DRAW(VOID)
{
	//文字列を描画
	DrawFormatString(0, GAME_HEIGHT - 20, GetColor(255, 255, 255), "FPS:%.1f", CalcFps);
	return;
}

//FPS値を計測し、待つ関数
VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;			//かかった時間
	int waitTime = CountFps * 1000 / GAME_FPS - resultTime; //待つべき時間
	
	if (waitTime > 0)
	{
		WaitTimer(waitTime);    //待つ
	}

	return;
}

//キーの入力状態を更新する関数
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	char TempKey[256];  //一時的に、現在のキーの入力状態を格納する

	//直前のキー入力を取っておく
	for (int i = 0; i < 256; i++)
	{
		OldAllKeyState[i] = AllKeyState[i];
	}

	GetHitKeyStateAll(TempKey); // 全てのキーの入力状態を得る

	for (int i = 0; i < 256; i++)
	{
		if (TempKey[i] != 0)	//押されているキーのキーコードを押しているとき
		{
			AllKeyState[i]++;	//押されている
		}
		else
		{
			AllKeyState[i] = 0;	//押されていない
		}
	}
	return;
}

//キーを押しているか、キーコードで判断する
//引　数：int：キーコード：KEY_INPUT_???
BOOL MY_KEY_DOWN(int KEY_INPUT_)
{
	//キーコードのキーを押している時
	if (AllKeyState[KEY_INPUT_] != 0)
	{
		return TRUE;	//キーを押している
	}
	else
	{
		return FALSE;	//キーを押していない
	}
}


//キーを押し上げたか、キーコードで判断する
//引　数：int：キーコード：KEY_INPUT_???
BOOL MY_KEY_UP(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] >= 1	//直前は押していて
		&& AllKeyState[KEY_INPUT_] == 0)	//今は押していないとき
	{
		return TRUE;	//キーを押し上げている
	}
	else
	{
		return FALSE;	//キーを押し上げていない
	}
}

//キーを押し続けているか、キーコードで判断する
//引　数：int：キーコード：KEY_INPUT_???
//引　数：int：キーを押し続ける時間
BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int DownTime)
{
	//押し続ける時間=秒数×FPS値
	//例）60FPSのゲームで、1秒間押し続けるなら、1秒×60FPS
	int UpdateTime = DownTime * GAME_FPS;

	if (AllKeyState[KEY_INPUT_] > UpdateTime)
	{
		return TRUE;	//押し続けている
	}
	else
	{
		return FALSE;	//押し続けていない
	}
}

//スタート画面
VOID MY_START(VOID)
{
	MY_START_PROC();    //スタート画面の処理
	MY_START_DRAW();    //スタート画面の描画

	return;
}

//スタート画面の処理
VOID MY_START_PROC(VOID)
{
	//エンターキーを押したら、プレイシーンへ移動する
	if (MY_KEY_DOWN(KEY_INPUT_RETURN) == TRUE)
	{
		GameScene = GAME_SCENE_PLAY;
	}

	//シフトキー(左シフトキー or 右シフトキー)を押したら、操作説明画面に移動する
	if (MY_KEY_DOWN(KEY_INPUT_LSHIFT) || MY_KEY_DOWN(KEY_INPUT_RSHIFT) == TRUE)
	{
		GameScene = GAME_SCENE_MENU;
	}

	return;
}

//スタート画面の描画
VOID MY_START_DRAW(VOID)
{
	//背景を描画する
	DrawGraph(ImageSTARTBK.x, ImageSTARTBK.y, ImageSTARTBK.handle, TRUE);

	//操作説明画面
	DrawGraph(ImageMENU.x, ImageMENU.y, ImageMENU.handle, TRUE);

	//赤の四角を描画
	//DrawBox(10, 10, GAME_WIDTH - 10, GAME_HEIGHT - 10, GetColor(255, 0, 0), TRUE);
	DrawString(0, 0, "スタート画面(エンターキーを押して下さい)", GetColor(255, 255, 255));

	return;
}

//プレイ画面
VOID MY_PLAY(VOID)
{
	MY_PLAY_PROC();    //プレイ画面の処理
	MY_PLAY_DRAW();    //プレイ画面の描画

	return;
}

//プレイ画面の処理
VOID MY_PLAY_PROC(VOID)
{
	//スペースキーを押したら、エンドシーンに移動する
	if (MY_KEY_DOWN(KEY_INPUT_SPACE) == TRUE)
	{
		GameScene = GAME_SCENE_END;
	}

	for (int cnt = 0; cnt < ANIMAL_MAX; cnt++)
	{
		if (animal[cnt].IsDraw == FALSE)
		{
			//描画する
			animal[cnt].IsDraw = TRUE;

			animal[cnt].x = GAME_WIDTH / 2 - animal[cnt].width / 2;

			animal[cnt].y = 0;

			break;
		}
	}

	return;
}

//プレイ画面の描画
VOID MY_PLAY_DRAW(VOID)
{
	DrawGraph(ImagePLAYENDBK.x, ImagePLAYENDBK.y, ImagePLAYENDBK.handle, TRUE);

	//動物の情報を生成
	for (int cnt = 0; cnt < ANIMAL_MAX; cnt++)
	{
		//描画できるなら
		if (animal[cnt].IsDraw == TRUE)
		{
			//描画する
			DrawGraph(
				animal[cnt].x,
				animal[cnt].y,
				animal[cnt].handle[animal[cnt].nowImageKind],
				TRUE
			);

			//表示フレームを増やす
			if (animal[cnt].changeImageCnt < animal[cnt].changeImageCntMAX)
			{
				animal[cnt].changeImageCnt++;
			}
			else
			{
				//現在表示している動物がまだいる時
				if (animal[cnt].nowImageKind < CHIP_DIV_NUM)
				{
					if (MY_KEY_DOWN(KEY_INPUT_RETURN) == TRUE)
					{
						animal[cnt].nowImageKind++;
						animal[cnt].IsDraw = FALSE;
						
					}
				}
				else
				{
					animal[cnt].nowImageKind = 0;
				}
				animal[cnt].changeImageCnt = 0;
			}
		}
	}

	//緑の四角を描画
	//DrawBox(10, 10, GAME_WIDTH - 10, GAME_HEIGHT - 10, GetColor(0, 255, 0), TRUE);
	DrawString(0, 0, "プレイ画面(スペースキーを押して下さい)", GetColor(255, 255, 255));

	return;
}

//エンド画面
VOID MY_END(VOID)
{
	MY_END_PROC();    //エンド画面の処理
	MY_END_DRAW();    //エンド画面の描画

	return;
}

//エンド画面の処理
VOID MY_END_PROC(VOID)
{
	//エスケープキーを押したら、スタートシーンへ移動する
	if (MY_KEY_DOWN(KEY_INPUT_ESCAPE) == TRUE)
	{
		GameScene = GAME_SCENE_START;
	}

	return;
}

//エンド画面の描画
VOID MY_END_DRAW(VOID)
{
	DrawGraph(ImagePLAYENDBK.x, ImagePLAYENDBK.y, ImagePLAYENDBK.handle, TRUE);

	//青の四角を描画
	//DrawBox(10, 10, GAME_WIDTH - 10, GAME_HEIGHT - 10, GetColor(0, 0, 255), TRUE);
	DrawString(0, 0, "エンド画面(エスケープキーを押して下さい)", GetColor(255, 255, 255));

	return;
}

//操作説明画面
VOID MY_MENU(VOID)
{
	MY_MENU_PROC();     //操作説明画面の処理
	MY_MENU_DRAW();     //操作説明画面の描画

	return;
}

//操作説明画面の処理
VOID MY_MENU_PROC(VOID)
{
	if (MY_KEY_DOWN(KEY_INPUT_BACK) == TRUE)
	{
		GameScene = GAME_SCENE_START;
	}

	return;
}

//操作説明画面の描画
VOID MY_MENU_DRAW(VOID)
{
	DrawGraph(ImageMENUBK.x, ImageMENUBK.y, ImageMENUBK.handle, TRUE);
	
	//DrawBox(10, 10, GAME_WIDTH - 10, GAME_HEIGHT - 10, GetColor(255, 0, 0), TRUE);
	DrawString(0, 0, "操作説明画面(バックスペースキーを押して下さい)", GetColor(255, 255, 255));

	return;
}

//画像をまとめて読み込む関数
BOOL MY_LOAD_IMAGE(VOID)
{
	//スタート画面の背景画像
	strcpy_s(ImageSTARTBK.path, IMAGE_START_IMAGE_PATH);  //パスの設定
	ImageSTARTBK.handle = LoadGraph(ImageSTARTBK.path);   //読み込み
	if (ImageSTARTBK.handle == -1)
	{
		//エラーメッセージ表示
		MessageBox(GetMainWindowHandle(), IMAGE_START_IMAGE_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}
	GetGraphSize(ImageSTARTBK.handle, &ImageSTARTBK.width, &ImageSTARTBK.height);
	ImageSTARTBK.x = GAME_WIDTH / 2 - ImageSTARTBK.width / 2;
	ImageSTARTBK.y = GAME_HEIGHT / 2 - ImageSTARTBK.height / 2;

	//プレイ画面とエンド画面の背景画像
	strcpy_s(ImagePLAYENDBK.path, IMAGE_PLAY_IMAGE_PATH);  //パスの設定
	ImagePLAYENDBK.handle = LoadGraph(ImagePLAYENDBK.path);   //読み込み
	if (ImagePLAYENDBK.handle == -1)
	{
		//エラーメッセージ表示
		MessageBox(GetMainWindowHandle(), IMAGE_PLAY_IMAGE_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}
	GetGraphSize(ImagePLAYENDBK.handle, &ImagePLAYENDBK.width, &ImagePLAYENDBK.height);
	ImagePLAYENDBK.x = GAME_WIDTH / 2 - ImagePLAYENDBK.width / 2;
	ImagePLAYENDBK.y = GAME_HEIGHT / 2 - ImagePLAYENDBK.height / 2;

	//操作説明画面へ促すためのボタン
	strcpy_s(ImageMENU.path, IMAGE_MENU_IMAGE_PATH);  //パスの設定
	ImageMENU.handle = LoadGraph(ImageMENU.path);   //読み込み
	if (ImageMENU.handle == -1)
	{
		//エラーメッセージ表示
		MessageBox(GetMainWindowHandle(), IMAGE_MENU_IMAGE_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}
	GetGraphSize(ImageMENU.handle, &ImageMENU.width, &ImageMENU.height);
	ImageMENU.x = GAME_WIDTH - ImageMENU.width - 20;
	ImageMENU.y = GAME_HEIGHT - ImageMENU.height - 20;

	//操作説明画面の背景
	strcpy_s(ImageMENUBK.path, IMAGE_MENU_BK_PATH);  //パスの設定
	ImageMENUBK.handle = LoadGraph(ImageMENUBK.path);   //読み込み
	if (ImageMENUBK.handle == -1)
	{
		//エラーメッセージ表示
		MessageBox(GetMainWindowHandle(), IMAGE_MENU_BK_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}
	GetGraphSize(ImageMENUBK.handle, &ImageMENUBK.width, &ImageMENUBK.height);
	ImageMENUBK.x = GAME_WIDTH / 2 - ImageMENUBK.width / 2;
	ImageMENUBK.y = GAME_HEIGHT / 2 - ImageMENUBK.height / 2;

	int animalRes = LoadDivGraph(
		GAME_animal1_CHIP_PATH,										//動物チップのパス
		CHIP_DIV_NUM, GAME_animal1_DIV_TATE, GAME_animal1_DIV_YOKO, //分割する数
		CHIP_DIV_WIDTH, CHIP_DIV_HEIGHT,							//画像を分割する幅と高さ
		&animal[0].handle[0]
	);

	if (animalRes == -1)
	{
		MessageBox(GetMainWindowHandle(), GAME_animal1_CHIP_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}

	GetGraphSize(animal[0].handle[0], &animal[0].width, &animal[0].height);

	for (int cnt = 0; cnt < ANIMAL_MAX; cnt++)
	{
		strcpyDx(animal[cnt].path, TEXT(GAME_animal1_CHIP_PATH));
		
		for (int i_num = 0; i_num < CHIP_DIV_NUM; i_num++)
		{
			animal[cnt].handle[i_num] = animal[0].handle[i_num];
		}

		animal[cnt].width = animal[0].width;

		animal[cnt].height = animal[0].height;

		animal[cnt].x = 0;

		animal[cnt].y = 0;

		animal[cnt].IsDraw = FALSE;

		animal[cnt].changeImageCnt = 0;

		animal[cnt].changeImageCntMAX = ANIMAL_CHANGE_MAX;

		animal[cnt].speed = SPEED_HIGH;

		animal[cnt].nowImageKind = 0;
	}

	return TRUE;
}

//画像をまとめて削除する関数
VOID MY_DELETE_IMAGE(VOID)
{
	DeleteGraph(ImageSTARTBK.handle);
	DeleteGraph(ImagePLAYENDBK.handle);
	DeleteGraph(ImageMENU.handle);
	DeleteGraph(ImageMENUBK.handle);

	for (int i_num = 0; i_num < CHIP_DIV_NUM; i_num++)
	{
		DeleteGraph(animal[0].handle[i_num]);
	}

	return;
}
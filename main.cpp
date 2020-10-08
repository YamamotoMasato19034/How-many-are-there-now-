/*--+----1----+----2----+----3----+----4----+----5-----+----6----+----7----+----8----+----9----+---*/
//�������[�v

//########## �w�b�_�[�t�@�C���ǂݍ��� ##########
#include "DxLib.h"

//########## �}�N����` ##########
#define GAME_WIDTH	1000	//��ʂ̉��̑傫��
#define GAME_HEIGHT	700	//��ʂ̏c�̑傫��
#define GAME_COLOR	32	//��ʂ̃J���[�r�b�g

#define GAME_WINDOW_BAR	0	//�^�C�g���o�[�̓f�t�H���g�ɂ���
#define GAME_WINDOW_NAME	"GAME TITLE"	//�E�B���h�E�̃^�C�g��

#define GAME_FPS	60  //FPS�̐��l

//�p�X�̒���
#define PATH_MAX   255
#define NAME_MAX   255

//�G���[���b�Z�[�W
#define IMAGE_LOAD_ERR_TITLE    TEXT("�摜�ǂݍ��݃G���[")

//�摜�֘A
#define IMAGE_START_IMAGE_PATH  TEXT(".\\IMAGE\\�X�^�[�g���.png")  //�w�i�̉摜
#define IMAGE_PLAY_IMAGE_PATH   TEXT(".\\IMAGE\\�X�̒�.png")  //�w�i�̉摜
#define IMAGE_MENU_IMAGE_PATH   TEXT(".\\IMAGE\\�������.png")  //�w�i�̉摜
#define IMAGE_MENU_BK_PATH      TEXT(".\\IMAGE\\menu_�w�i.png")

//�����`�b�v�֘A
#define GAME_animal1_CHIP_PATH  TEXT(".\\IMAGE\\animal\\mapchip_1.png")  //�`�b�v�̉摜
#define ANIMAL_MAX				4
#define ANIMAL_CHANGE_MAX		6

//�摜�����֘A
#define CHIP_DIV_WIDTH			565   //�摜�𕪊����镝�T�C�Y
#define CHIP_DIV_HEIGHT			660   //�摜�𕪊����鍂���T�C�Y
#define GAME_animal1_DIV_TATE   2     //�摜���c�ɕ������鐔
#define GAME_animal1_DIV_YOKO   2     //�摜�����ɕ������鐔
#define CHIP_DIV_NUM GAME_animal1_DIV_TATE * GAME_animal1_DIV_YOKO  //�摜�𕪊����鑍��

#define FIRST_MASK				20

//�t�H���g�̃p�X�̒���
#define FONT_PATH_MAX			255

//�t�H���g
#define FONT_TANUKI_PATH		TEXT(".\\FONT\\TanukiMagic.ttf")
#define FONT_TANUKI_NAME		TEXT("���ʂ������}�W�b�N")

//�t�H���g�̃G���[���b�Z�[�W
#define FONT_INSTALL_ERR_TITLE	TEXT("�t�H���g�C���X�g�[���G���[")
#define FONT_CREATE_ERR_TITLE	TEXT("�t�H���g�쐬�G���[")

enum GAME_SCENE {
	GAME_SCENE_START,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
	GAME_SCENE_MENU,
};   //�Q�[���̃V�[��

typedef struct STRUCT_I_POINT
{
	int x = -1;
	int y = -1;
}iPOINT;

typedef struct STRUCT_IMAGE
{
	char path[PATH_MAX];	//�p�X
	int handle;				//�n���h��
	int x;					//X�ʒu
	int y;					//Y�ʒu
	int width;				//��
	int height;				//����
}IMAGE; //�摜�\����

typedef struct STRUCT_ANIMAL
{
	char path[PATH_MAX];		//�p�X
	int handle[CHIP_DIV_NUM];   //�������������̉摜�n���h�����擾
	int x;						//X�ʒu
	int y;						//Y�ʒu
	int width;					//��
	int height;					//����
	BOOL IsDraw;				//������\���ł��邩
	int nowImageKind;			//�����̌��݂̉摜
}MAPCHIP;

typedef struct STRUCT_FONT
{
	char path[FONT_PATH_MAX];   //�p�X
	char name[FONT_PATH_MAX];   //�t�H���g��
	int handle;                 //�n���h��
	int size;					//�傫��
	int bold;					//����
	int type;					//�^�C�v
}FONT;

//�O���[�o���ϐ�
int StartTimeFps;				//����J�n����
int CountFps;					//�J�E���^
float CalcFps;					//�v�Z����
int SampleNumFps = GAME_FPS;	//���ς��Ƃ�T���v����

//�L�[�{�[�h�̓��͂��擾
char AllKeyState[256] = { 0 };
char OldAllKeyState[256] = { 0 };

//�Q�[���V�[��
int GameScene;

//�}�b�v�`�b�v�֘A
MAPCHIP animal[ANIMAL_MAX];
int GHandle[ANIMAL_MAX];

//�}�X�N�֘A
int order = 0;
int dammy = 0;
int Mask_num = 0;
int Mask_sum = 0;

int stage = 0;

//�摜�֘A
IMAGE ImageSTARTBK;   //�Q�[���̔w�i
IMAGE ImagePLAYENDBK;
IMAGE ImageMENU;
IMAGE ImageMENUBK;

//�t�H���g�֘A
FONT TANUKI;

//�v���g�^�C�v�錾
VOID MY_FPS_UPDATE(VOID);		//FPS�l���v���A�X�V����֐�
VOID MY_FPS_DRAW(VOID);			//FPS�l��`�悷��֐�
VOID MY_FPS_WAIT(VOID);			//FPS�l���v�����B�҂֐�

VOID MY_ALL_KEYDOWN_UPDATE(VOID);  //�L�[�̓��͏�Ԃ��X�V����
BOOL MY_KEY_DOWN(int);			   //�L�[�������Ă��邩�A�L�[�R�[�h�Ŕ��f����
BOOL MY_KEY_UP(int);			   //�L�[�������グ�����A�L�[�R�[�h�Ŕ��f����
BOOL MY_KEYDOWN_KEEP(int, int);    //�L�[�����������Ă��邩�A�L�[�R�[�h�Ŕ��f����
BOOL MY_KEYDOWN_1second(int);

VOID MY_START(VOID);		//�X�^�[�g���
VOID MY_START_PROC(VOID);   //�X�^�[�g��ʂ̏���
VOID MY_START_DRAW(VOID);   //�X�^�[�g��ʂ̕`��

VOID MY_PLAY(VOID);		   //�v���C���
VOID MY_PLAY_PROC(VOID);   //�v���C��ʂ̏���
VOID MY_PLAY_DRAW(VOID);   //�v���C��ʂ̕`��

VOID MY_END(VOID);		  //�G���h���
VOID MY_END_PROC(VOID);   //�G���h��ʂ̏���
VOID MY_END_DRAW(VOID);   //�G���h��ʂ̕`��

VOID MY_MENU(VOID);       //����������
VOID MY_MENU_PROC(VOID);  //���������ʂ̏���
VOID MY_MENU_DRAW(VOID);  //���������ʂ̕`��

BOOL MY_LOAD_IMAGE(VOID);    //�摜���܂Ƃ߂ēǂݍ��ފ֐�
VOID MY_DELETE_IMAGE(VOID);  //�摜���܂Ƃ߂č폜����֐�

BOOL MY_FONT_INSTALL_ONCE(VOID);    //�t�H���g���ꎞ�I�ɃC���X�g�[��
VOID MY_FONT_UNINSTALL_ONCE(VOID);  //�t�H���g���ꎞ�I�ɃA���C���X�g�[��
BOOL MY_FONT_CREATE(VOID);			//�t�H���g���쐬����
VOID MY_FONT_DELETE(VOID);			//�t�H���g���폜����

//########## �v���O�����ōŏ��Ɏ��s�����֐� ##########
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);						//�E�B���h�E���[�h�ɐݒ�
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//�w��̐��l�ŃE�B���h�E��\������
	SetWindowStyleMode(GAME_WINDOW_BAR);		//�^�C�g���o�[�̓f�t�H���g�ɂ���
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));	//�E�B���h�E�̃^�C�g���̕���
	SetAlwaysRunFlag(TRUE);						//��A�N�e�B�u�ł����s����

	if (DxLib_Init() == -1) { return -1; }	//�c�w���C�u��������������

	//�t�H���g���ꎞ�I�ɃC���X�g�[��
	if (MY_FONT_INSTALL_ONCE() == FALSE) { return -1; }

	//�t�H���g�n���h�����쐬
	if (MY_FONT_CREATE() == FALSE) { return -1; }

	//�摜��ǂݍ���
	if (MY_LOAD_IMAGE() == FALSE) { return -1; }

	GameScene = GAME_SCENE_START;   //�Q�[���V�[���̓X�^�[�g��ʂ���
	
	SetDrawScreen(DX_SCREEN_BACK);	//Draw�n�֐��͗���ʂɕ`��

	//�������[�v
	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }	//���b�Z�[�W�����̌��ʂ��G���[�̂Ƃ��A�����I��

		if (ClearDrawScreen() != 0) { break; }	//��ʂ������ł��Ȃ������Ƃ��A�����I��

		MY_ALL_KEYDOWN_UPDATE();       //�����Ă���L�[��Ԃ��擾����

		MY_FPS_UPDATE();   //FPS�̏���(�X�V)

		//�V�[�����Ƃɏ������s��
		switch (GameScene)
		{
		case GAME_SCENE_START:
			MY_START();   //�X�^�[�g���
			break;
		case GAME_SCENE_PLAY:
			MY_PLAY();    //�v���C���
			break;
		case GAME_SCENE_END:
			MY_END();     //�G���h���
			break;
		case GAME_SCENE_MENU:
			MY_MENU();    //����������
			break;
		}

		//DrawString(DrawX, DrawY, "Hello World", GetColor(255, 255, 255));	//������`��

		MY_FPS_DRAW();	   //FPS�̏���(�`��)

		ScreenFlip();		//���j�^�̃��t���b�V�����[�g�̑����ŗ���ʂ��ĕ`��

		MY_FPS_WAIT();     //FPS�̏���(�҂�)
	}
	
	//DrawString(DrawX, DrawY, "Hello World", GetColor(255, 255, 255));	//������`��
	//WaitKey();	//�L�[���͑҂�

	//�摜�n���h����j��
	MY_DELETE_IMAGE();

	//�t�H���g�n���h����j��
	MY_FONT_DELETE();

	//�ꎞ�I�ɃC���X�g�[�������t�H���g���A���C���X�g�[��
	MY_FONT_UNINSTALL_ONCE();

	DxLib_End();	//�c�w���C�u�����g�p�̏I������

	return 0;
}

//FPS�l���v���A�҂֐�
VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0)  //1�t���[���ڂȂ玞�����L��
	{
		StartTimeFps = GetNowCount();
	}

	if (CountFps == SampleNumFps)   //60�t���[���ڂȂ畽�ς��v�Z
	{
		int now = GetNowCount();
		//���݂̎��Ԃ���A0�t���[���̎��Ԃ������AFPS�̐��l�Ŋ��� = 1FPS�ӂ�̕��ώ��Ԃ��v�Z�����
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);
		CountFps = 0;
		StartTimeFps = now;
	}
	CountFps++;
	return;
}

//FPS�l��`�悷��֐�
VOID MY_FPS_DRAW(VOID)
{
	//�������`��
	DrawFormatString(0, GAME_HEIGHT - 20, GetColor(255, 255, 255), "FPS:%.1f", CalcFps);
	return;
}

//FPS�l���v�����A�҂֐�
VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;			//������������
	int waitTime = CountFps * 1000 / GAME_FPS - resultTime; //�҂ׂ�����
	
	if (waitTime > 0)
	{
		WaitTimer(waitTime);    //�҂�
	}

	return;
}

//�L�[�̓��͏�Ԃ��X�V����֐�
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	char TempKey[256];  //�ꎞ�I�ɁA���݂̃L�[�̓��͏�Ԃ��i�[����

	//���O�̃L�[���͂�����Ă���
	for (int i = 0; i < 256; i++)
	{
		OldAllKeyState[i] = AllKeyState[i];
	}

	GetHitKeyStateAll(TempKey); // �S�ẴL�[�̓��͏�Ԃ𓾂�

	for (int i = 0; i < 256; i++)
	{
		if (TempKey[i] != 0)	//������Ă���L�[�̃L�[�R�[�h�������Ă���Ƃ�
		{
			AllKeyState[i]++;	//������Ă���
		}
		else
		{
			AllKeyState[i] = 0;	//������Ă��Ȃ�
		}
	}
	return;
}

//�L�[�������Ă��邩�A�L�[�R�[�h�Ŕ��f����
//���@���Fint�F�L�[�R�[�h�FKEY_INPUT_???
BOOL MY_KEY_DOWN(int KEY_INPUT_)
{
	//�L�[�R�[�h�̃L�[�������Ă��鎞
	if (AllKeyState[KEY_INPUT_] != 0)
	{
		return TRUE;	//�L�[�������Ă���
	}
	else
	{
		return FALSE;	//�L�[�������Ă��Ȃ�
	}
}


//�L�[�������グ�����A�L�[�R�[�h�Ŕ��f����
//���@���Fint�F�L�[�R�[�h�FKEY_INPUT_???
BOOL MY_KEY_UP(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] >= 1	//���O�͉����Ă���
		&& AllKeyState[KEY_INPUT_] == 0)	//���͉����Ă��Ȃ��Ƃ�
	{
		return TRUE;	//�L�[�������グ�Ă���
	}
	else
	{
		return FALSE;	//�L�[�������グ�Ă��Ȃ�
	}
}

//�L�[�����������Ă��邩�A�L�[�R�[�h�Ŕ��f����
//���@���Fint�F�L�[�R�[�h�FKEY_INPUT_???
//���@���Fint�F�L�[�����������鎞��
BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int DownTime)
{
	//���������鎞��=�b���~FPS�l
	//��j60FPS�̃Q�[���ŁA1�b�ԉ���������Ȃ�A1�b�~60FPS
	int UpdateTime = DownTime * GAME_FPS;

	if (AllKeyState[KEY_INPUT_] > UpdateTime)
	{
		return TRUE;	//���������Ă���
	}
	else
	{
		return FALSE;	//���������Ă��Ȃ�
	}
}

//�����̉摜�ύX�p
BOOL MY_KEYDOWN_1second(int KEY_INPUT_)
{
	//�L�[�R�[�h�̃L�[�������Ă��鎞
	if (AllKeyState[KEY_INPUT_] == 1)
	{
		return TRUE;	//�L�[�������Ă���
	}
	else
	{
		return FALSE;	//�L�[�������Ă��Ȃ�
	}
}

//�X�^�[�g���
VOID MY_START(VOID)
{
	MY_START_PROC();    //�X�^�[�g��ʂ̏���
	MY_START_DRAW();    //�X�^�[�g��ʂ̕`��

	return;
}

//�X�^�[�g��ʂ̏���
VOID MY_START_PROC(VOID)
{
	//�G���^�[�L�[����������A�v���C�V�[���ֈړ�����
	if (MY_KEY_DOWN(KEY_INPUT_RETURN) == TRUE)
	{
		stage = FIRST_MASK;
		GameScene = GAME_SCENE_PLAY;
	}

	//�V�t�g�L�[(���V�t�g�L�[ or �E�V�t�g�L�[)����������A���������ʂɈړ�����
	if (MY_KEY_DOWN(KEY_INPUT_LSHIFT) || MY_KEY_DOWN(KEY_INPUT_RSHIFT) == TRUE)
	{
		GameScene = GAME_SCENE_MENU;
	}

	return;
}

//�X�^�[�g��ʂ̕`��
VOID MY_START_DRAW(VOID)
{
	//�w�i��`�悷��
	DrawGraph(ImageSTARTBK.x, ImageSTARTBK.y, ImageSTARTBK.handle, TRUE);

	//����������
	DrawGraph(ImageMENU.x, ImageMENU.y, ImageMENU.handle, TRUE);

	return;
}

//����������
VOID MY_MENU(VOID)
{
	MY_MENU_PROC();     //���������ʂ̏���
	MY_MENU_DRAW();     //���������ʂ̕`��

	return;
}

//���������ʂ̏���
VOID MY_MENU_PROC(VOID)
{
	if (MY_KEY_DOWN(KEY_INPUT_BACK) == TRUE)
	{
		GameScene = GAME_SCENE_START;
	}

	return;
}

//���������ʂ̕`��
VOID MY_MENU_DRAW(VOID)
{
	DrawGraph(ImageMENUBK.x, ImageMENUBK.y, ImageMENUBK.handle, TRUE);
	return;
}

//�v���C���
VOID MY_PLAY(VOID)
{
	MY_PLAY_PROC();    //�v���C��ʂ̏���
	MY_PLAY_DRAW();    //�v���C��ʂ̕`��

	return;
}

//�v���C��ʂ̏���
VOID MY_PLAY_PROC(VOID)
{
	/*for (int cnt = 0; cnt < ANIMAL_MAX; cnt++)
	{
		animal[cnt].IsDraw = FALSE;
	}*/
	//���ʂ𒴂�����I��
	if (Mask_sum > stage)
	{
		GameScene = GAME_SCENE_END;

		//�I������ۂ͑S�ď���
		for (int cnt = 0; cnt < ANIMAL_MAX; cnt++)
		{
			animal[cnt].IsDraw = FALSE;
		}
		//�ĊJ���Ă��ŏ�����
		order = 0;
		//������
		Mask_num = 0;
		Mask_sum = 0;
	}

	//�G���^�[�L�[����������
	if (MY_KEYDOWN_1second(KEY_INPUT_RETURN) == TRUE)
	{
		//�������擾
		Mask_num = GetRand(10);
		Mask_sum += Mask_num;

		//�P�̂ŕ\������
		if (order == 0)
		{
			//�`�悷��
			animal[ANIMAL_MAX - 1].IsDraw = FALSE;  //��O�̊G������
			animal[order].IsDraw = TRUE;			//�\��
			order++;
		}
		else if (order == 3)
		{
			//�`��
			animal[order - 1].IsDraw = FALSE;		//��O�̊G������
			animal[order].IsDraw = TRUE;			//�\��
			order = 0;
		}
		else
		{
			//�`��
			animal[order - 1].IsDraw = FALSE;		//��O�̊G������
			animal[order].IsDraw = TRUE;			//�\��
			order++;
		}
	}
	return;
}

//�v���C��ʂ̕`��
VOID MY_PLAY_DRAW(VOID)
{
	//�v���C��ʂ̔w�i
	DrawGraph(ImagePLAYENDBK.x, ImagePLAYENDBK.y, ImagePLAYENDBK.handle, TRUE);

	//�g�[�N�V�[���̔w�i
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);  //�����x��25%�グ��
	DrawBox(0, GAME_HEIGHT - 180, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);  //���ɖ߂�

	//�����̏��𐶐�
	for (int cnt = 0; cnt < ANIMAL_MAX; cnt++)
	{
		//�`��ł���Ȃ�
		if (animal[cnt].IsDraw == TRUE)
		{
			//�摜��1.5�{�ɏk���\��
			DrawExtendGraph(
				animal[cnt].x, animal[cnt].y,
				animal[cnt].x + animal[cnt].width / 1.5, animal[cnt].y + (int)animal[cnt].height / 1.5,
				GHandle[cnt], TRUE
			);

			//�~�����}�X�N�̕\��
			DrawFormatStringToHandle(170, GAME_HEIGHT - 170, GetColor(255, 255, 255), TANUKI.handle, "�}�X�N %d�� ���傤�����I�I", Mask_num);
			DrawFormatStringToHandle(0, 0, GetColor(255, 255, 255), TANUKI.handle, "%d��", Mask_sum);

			//�u������H�v�u�����Ȃ��H�v�̒ǉ�
			DrawBox(200, GAME_HEIGHT - 100, 410, GAME_HEIGHT - 50, GetColor(255, 0, 0), TRUE);
			DrawStringToHandle(200, GAME_HEIGHT - 100, "������H", GetColor(255, 255, 255), TANUKI.handle);
			DrawBox(500, GAME_HEIGHT - 100, 770, GAME_HEIGHT - 50, GetColor(0, 0, 255), TRUE);
			DrawStringToHandle(500, GAME_HEIGHT - 100, "�����Ȃ��H", GetColor(255, 255, 255), TANUKI.handle);
		}
	}

	return;
}

//�G���h���
VOID MY_END(VOID)
{
	MY_END_PROC();    //�G���h��ʂ̏���
	MY_END_DRAW();    //�G���h��ʂ̕`��

	return;
}

//�G���h��ʂ̏���
VOID MY_END_PROC(VOID)
{
	//�G�X�P�[�v�L�[����������A�X�^�[�g�V�[���ֈړ�����
	if (MY_KEY_DOWN(KEY_INPUT_ESCAPE) == TRUE)
	{
		GameScene = GAME_SCENE_START;
	}

	return;
}

//�G���h��ʂ̕`��
VOID MY_END_DRAW(VOID)
{
	DrawGraph(ImagePLAYENDBK.x, ImagePLAYENDBK.y, ImagePLAYENDBK.handle, TRUE);

	return;
}

//�摜���܂Ƃ߂ēǂݍ��ފ֐�
BOOL MY_LOAD_IMAGE(VOID)
{
	//�X�^�[�g��ʂ̔w�i�摜
	strcpy_s(ImageSTARTBK.path, IMAGE_START_IMAGE_PATH);  //�p�X�̐ݒ�
	ImageSTARTBK.handle = LoadGraph(ImageSTARTBK.path);   //�ǂݍ���
	if (ImageSTARTBK.handle == -1)
	{
		//�G���[���b�Z�[�W�\��
		MessageBox(GetMainWindowHandle(), IMAGE_START_IMAGE_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}
	GetGraphSize(ImageSTARTBK.handle, &ImageSTARTBK.width, &ImageSTARTBK.height);
	ImageSTARTBK.x = GAME_WIDTH / 2 - ImageSTARTBK.width / 2;
	ImageSTARTBK.y = GAME_HEIGHT / 2 - ImageSTARTBK.height / 2;

	//�v���C��ʂƃG���h��ʂ̔w�i�摜
	strcpy_s(ImagePLAYENDBK.path, IMAGE_PLAY_IMAGE_PATH);  //�p�X�̐ݒ�
	ImagePLAYENDBK.handle = LoadGraph(ImagePLAYENDBK.path);   //�ǂݍ���
	if (ImagePLAYENDBK.handle == -1)
	{
		//�G���[���b�Z�[�W�\��
		MessageBox(GetMainWindowHandle(), IMAGE_PLAY_IMAGE_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}
	GetGraphSize(ImagePLAYENDBK.handle, &ImagePLAYENDBK.width, &ImagePLAYENDBK.height);
	ImagePLAYENDBK.x = GAME_WIDTH / 2 - ImagePLAYENDBK.width / 2;
	ImagePLAYENDBK.y = GAME_HEIGHT / 2 - ImagePLAYENDBK.height / 2;

	//���������ʂ֑������߂̃{�^��
	strcpy_s(ImageMENU.path, IMAGE_MENU_IMAGE_PATH);  //�p�X�̐ݒ�
	ImageMENU.handle = LoadGraph(ImageMENU.path);   //�ǂݍ���
	if (ImageMENU.handle == -1)
	{
		//�G���[���b�Z�[�W�\��
		MessageBox(GetMainWindowHandle(), IMAGE_MENU_IMAGE_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}
	GetGraphSize(ImageMENU.handle, &ImageMENU.width, &ImageMENU.height);
	ImageMENU.x = GAME_WIDTH - ImageMENU.width - 20;
	ImageMENU.y = GAME_HEIGHT - ImageMENU.height - 20;

	//���������ʂ̔w�i
	strcpy_s(ImageMENUBK.path, IMAGE_MENU_BK_PATH);  //�p�X�̐ݒ�
	ImageMENUBK.handle = LoadGraph(ImageMENUBK.path);   //�ǂݍ���
	if (ImageMENUBK.handle == -1)
	{
		//�G���[���b�Z�[�W�\��
		MessageBox(GetMainWindowHandle(), IMAGE_MENU_BK_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}
	GetGraphSize(ImageMENUBK.handle, &ImageMENUBK.width, &ImageMENUBK.height);
	ImageMENUBK.x = GAME_WIDTH / 2 - ImageMENUBK.width / 2;
	ImageMENUBK.y = GAME_HEIGHT / 2 - ImageMENUBK.height / 2;

	int animalRes = LoadDivGraph(
		GAME_animal1_CHIP_PATH,										//�����`�b�v�̃p�X
		CHIP_DIV_NUM, GAME_animal1_DIV_TATE, GAME_animal1_DIV_YOKO, //�������鐔
		CHIP_DIV_WIDTH, CHIP_DIV_HEIGHT,							//�摜�𕪊����镝�ƍ���
		GHandle
	);

	//�����������ł��Ȃ�������
	if (animalRes == -1)
	{
		MessageBox(GetMainWindowHandle(), GAME_animal1_CHIP_PATH, IMAGE_LOAD_ERR_TITLE, MB_OK);
		return FALSE;
	}

	//���ꂼ�ꕝ�ƍ������擾
	for (int cnt = 0; cnt < ANIMAL_MAX; cnt++)
	{
		GetGraphSize(GHandle[cnt], &animal[cnt].width, &animal[cnt].height);
	}

	//����^����
	for (int cnt = 0; cnt < ANIMAL_MAX; cnt++)
	{
		strcpyDx(animal[cnt].path, TEXT(GAME_animal1_CHIP_PATH));						//�p�X
		animal[cnt].width = animal[0].width;											//��
		animal[cnt].height = animal[0].height;											//����
		animal[cnt].x = (GAME_WIDTH / 2 - animal[0].width / 2) + animal[0].width / 10;	//X���W
		animal[cnt].y = animal[0].height / 10;											//Y���W
		animal[cnt].IsDraw = FALSE;														//�`��ł��邩(�����l�� �`��ł��Ȃ�)
	}

	return TRUE;
}

//�摜���܂Ƃ߂č폜����֐�
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

//�t�H���g���ꎞ�I�ɃC���X�g�[��
BOOL MY_FONT_INSTALL_ONCE(VOID)
{
	//�t�H���g���ꎞ�I�ɓǂݍ���
	if (AddFontResourceEx(FONT_TANUKI_PATH, FR_PRIVATE, NULL) == 0)
	{
		//�G���[���b�Z�[�W�\��
		MessageBox(GetMainWindowHandle(), FONT_TANUKI_NAME, FONT_INSTALL_ERR_TITLE, MB_OK);
		return FALSE;
	}

	return TRUE;
}

//�t�H���g���ꎞ�I�ɃA���C���X�g�[��
VOID MY_FONT_UNINSTALL_ONCE(VOID)
{
	//�t�H���g���ꎞ�I�ɃA���C���X�g�[��
	RemoveFontResourceEx(FONT_TANUKI_PATH, FR_PRIVATE, NULL);

	return;
}

//�t�H���g��ǂݍ���
BOOL MY_FONT_CREATE(VOID)
{
	//�t�H���g�f�[�^���쐬
	strcpy_s(TANUKI.path, sizeof(TANUKI.path), FONT_TANUKI_PATH);  //�p�X���R�s�[
	strcpy_s(TANUKI.name, sizeof(TANUKI.name), FONT_TANUKI_NAME);  //�t�H���g�����R�s�[
	TANUKI.handle = -1;							//�n���h����������
	TANUKI.size = 50;								//�T�C�Y: 50
	TANUKI.bold = 4;								//����: 4
	TANUKI.type = DX_FONTTYPE_ANTIALIASING_EDGE;   //�A���`�G�C���A�V���O�t��

	//�t�H���g�n���h���쐬
	TANUKI.handle = CreateFontToHandle(TANUKI.name, TANUKI.size, TANUKI.bold, TANUKI.type);
	//�t�H���g�n���h�����쐬�ł��Ȃ��Ƃ��G���[
	if (TANUKI.handle == -1) { MessageBox(GetMainWindowHandle(), FONT_TANUKI_NAME, FONT_CREATE_ERR_TITLE, MB_OK); return FALSE; }

	return TRUE;
}

//�t�H���g���폜����
VOID MY_FONT_DELETE(VOID)
{
	//�t�H���g�f�[�^���폜
	DeleteFontToHandle(TANUKI.handle);

	return;
}
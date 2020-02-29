#include <math.h>
#include <windows.h>
#define	TIMER_SEC				100
#define TIMER_INFO				101

#define	TIME_REFRESH			10
#define TIME_SHOWINFO			500
#define	GROUND_HEIGHT			40

#define	JUMP_ACCELERATION		20
#define HORIZONTAL_MAXSPEED		10
#define HORIZONTAL_ACCELERATION	0.1
#define GRAVITY_ACCELERATION	0.98

#define USER_WIDTH				64
#define USER_HEIGHT				20

#define SWORD_TYPE				0
#define CAKE_TYPE				1
#define COIN_TYPE				2

#define	MAX_SWORD_NUM			10
#define MAX_CAKE_NUM			10
#define	MAX_COIN_NUM			10

#define MASK_COLOR				0xFF00

#define MAX(A, B)	(((A)>(B))?(A):(B))
#define MIN(A, B)	(((A)<(B))?(A):(B))

int ShowBitmapResource(HDC hDestDC, int xDest, int yDest);

BOOL	Number2String(int iNumber, int iLen, TCHAR *szStr);
VOID	DrawTime(HDC hDC, int px, int py, int time);


class USER {
public:
	int		cxPosition;
	int		cyPosition;
	int		LeftMove;
	int		RightMove;
	double	cxSpeed;
	double	cySpeed;

	int		iMark;
	int		iHP;
	BOOL	bAlive;

	HDC		hUserDC,		hOldUserDC;
	HBITMAP	hUserBitmap,	hOldUserBitmap;
	int	iPixel[4096];

	USER () {}
	USER (	int cxp, int cyp, int cxs, int cys,
			int iwide, int iheight, TCHAR	*FileName)
	{
		this->cxPosition	= cxp;
		this->cyPosition	= cyp;
		this->cxSpeed		= cxs;
		this->cySpeed		= cys;
		this->LeftMove		= 0;
		this->RightMove		= 0;
		this->iHP			= 100;
		this->iMark			= 0;
		this->bAlive		= TRUE;


		this->hUserBitmap	= (HBITMAP)LoadImage(NULL,
			FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		GetBitmapBits(hUserBitmap,USER_HEIGHT*USER_WIDTH,this->iPixel) ;
	}
	void	Jump(int cyClient)
	{
		if (cyClient - GROUND_HEIGHT == this->cyPosition + USER_HEIGHT)
		{
			PlaySound(TEXT("Jump.wav"), NULL, SND_FILENAME | SND_ASYNC);
			this->cySpeed	-= JUMP_ACCELERATION;
		}
		return;
	}
	void	Move(int cxClient, int cyClient)
	{
		if (cyClient - GROUND_HEIGHT == this->cyPosition + USER_HEIGHT &&
			this->cySpeed != -JUMP_ACCELERATION)
		{
			this->cySpeed	= 0.0;
		}
		else
		{
			this->cySpeed += GRAVITY_ACCELERATION;
		}
		this->cxPosition	= this->cxPosition +
			HORIZONTAL_MAXSPEED * (double)(this->RightMove - this->LeftMove);
		this->cyPosition	= this->cyPosition + this->cySpeed;
		if (cyClient - GROUND_HEIGHT < this->cyPosition+USER_HEIGHT)
		{
			this->cyPosition	=0;
		}
		if (0 > cxPosition)
		{
			cxPosition	= 0;
		}
		else if (cxPosition > cxClient)
		{
			this->cxPosition = cxClient;
		}
		return;
	}
	void	Draw(HDC	hDC, HDC	hMemDC)
	{
		int i, j, k;
		this->hUserDC		= CreateCompatibleDC(hDC);

		SetBitmapBits(hUserBitmap,USER_HEIGHT*USER_WIDTH*4,this->iPixel) ;
		hOldUserBitmap = (HBITMAP)SelectObject(hUserDC,hUserBitmap) ;
		/*
		BitBlt(hMemDC,
			this->cxPosition-USER_WIDTH/2,
			this->cyPosition-USER_HEIGHT,
			USER_WIDTH,
			USER_HEIGHT,
			hUserDC,0,0,SRCCOPY) ;*/
		for (i=0, k=0 ; k<USER_HEIGHT ; i+=USER_WIDTH, k++)
		{
			for (j=0 ; j<USER_WIDTH ; j++)
			{
				if (MASK_COLOR != iPixel[i+j])
				{
					SetPixel(hMemDC, this->cxPosition-USER_WIDTH/2+j,
						this->cyPosition-USER_HEIGHT+k, iPixel[i+j]);
				}
			}
		}
		SelectObject(hUserDC,hOldUserBitmap) ;
		DeleteDC(hUserDC) ;
		return;
	}
	void	ShowInfo(HDC hMemDC, int cxClient, int cyClient)
	{
		TCHAR	szStr[32];
		SetBkMode(hMemDC, TRANSPARENT);
		SetTextColor(hMemDC, RGB(255, 200, 200));
		sprintf(szStr, "HP ");
		Number2String((int)this->iHP, 6, szStr+3);
		TextOut(hMemDC, cxClient/2, 20, szStr, strlen(szStr));


		SetTextColor(hMemDC, RGB(255, 255, 200));
		sprintf(szStr, "PT ");	//point
		Number2String((int)this->iMark, 6, szStr+3);
		TextOut(hMemDC, cxClient/2 + 100, 20, szStr, strlen(szStr));
		return;
	}
};

class FALLTHING
{
public:
	int	iWide;
	int	iHeight;
	double	dySpeed;
	int	cxPosition;
	int	cyPosition;
	int	Type;
	int	Other;
	bool	bAlive;

	HDC		hFTDC,		hOldFTDC;
	HBITMAP	hFTBitmap,	hOldFTBitmap;
	int	iPixel[4096];

	FALLTHING () {}
	FALLTHING (int iwide, int iheight, int cxClient, int itype, TCHAR	*FileName)
	{
		this->iWide		= iwide;
		this->iHeight	= iheight;
		this->dySpeed	= rand()%5+5;
		this->cxPosition= rand() % cxClient;
		this->cyPosition= 0;
		this->Type		= itype;
		this->Other		= 0.0;
		this->hFTBitmap	= (HBITMAP)LoadImage(NULL,
			FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		switch(this->Type) {
		case COIN_TYPE:
			this->Other	= 100;
			break;
		case CAKE_TYPE:
			this->Other		= rand()%100+10;
			break;
		case SWORD_TYPE:
			this->Other		= rand()%60+50;
			break;
		}
		GetBitmapBits(hFTBitmap,this->iHeight*this->iWide*4,this->iPixel) ;
	}
	BOOL	CrashCheck(int cxUser, int cyUser)
	{
		return ( MAX(cxUser-USER_WIDTH/2,	this->cxPosition-this->iWide/2)
			< MIN(cxUser+USER_WIDTH/2,		this->cxPosition+this->iWide/2) &&
			MAX(cyUser-USER_HEIGHT/2, this->cyPosition-this->iHeight/2)
			< MIN(cyUser+USER_HEIGHT/2, this->cyPosition+this->iHeight/2) );
	}
	BOOL	Move(int cxClient, int cyClient)
	{
		cyPosition	+= dySpeed;
	//	dySpeed		+= GRAVITY_ACCELERATION;

		if (cyPosition > cyClient)
		{
			cyPosition		= 0;
			this->dySpeed	= rand()%5+5;
			this->cxPosition= rand()%cxClient;
		}
		return true;
	}
	BOOL	Draw(HDC	hDC, HDC	hMemDC)
	{
		int i, j, k;
		this->hFTDC		= CreateCompatibleDC(hDC);

		SetBitmapBits(hFTBitmap,this->iHeight*this->iWide*4,this->iPixel) ;
		hOldFTBitmap = (HBITMAP)SelectObject(hFTDC,hFTBitmap) ;

		/*BitBlt(hMemDC,
			this->cxPosition-this->iWide/2,
			this->cyPosition-this->iHeight/2,
			this->iWide,
			this->iHeight,
			hFTDC,0,0,SRCCOPY) ;*/
		for (i=0, k=0 ; k<this->iHeight ; i+=this->iWide, k++)
		{
			for (j=0 ; j<this->iWide ; j++)
			{
				if (MASK_COLOR != iPixel[i+j])
				{
					SetPixel(hMemDC, this->cxPosition-this->iWide/2+j,
						this->cyPosition-this->iHeight/2+k, iPixel[i+j]);
				}
			}
		}
		SelectObject(hFTDC,hOldFTBitmap) ;
		DeleteDC(hFTDC) ;
		return true;
	}
};
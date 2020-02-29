#pragma comment(lib, "winmm")
#pragma comment(lib, "comctl32.lib")
#include <math.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <wtypes.h>
#include <windowsx.h>
#include <winbase.h>
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#include <psapi.h>
#include <time.h>
#include <dos.h>
#include <conio.h>
#include "Function.h"
#include "resource.h"
//using namespace std;


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
//窗口消息处理程序	（窗口句柄，消息类型，消息数据1，消息数据2）


int WINAPI WinMain (HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					PSTR		szCmdLine,
					int			iCmdShow)
{
	static	int	cxClient, cyClient;
	RECT	rectMain;
	POINT	pTemp;



	static		TCHAR szAppName[] = TEXT ("SeeTheSky") ;
	HWND		hwnd ;
	MSG			msg ;
	WNDCLASS	wndclass ;
	wndclass.style			= CS_HREDRAW | CS_VREDRAW ;

	wndclass.lpfnWndProc	= WndProc ;
	wndclass.cbClsExtra		= 0 ;
	wndclass.cbWndExtra		= 0 ;
	wndclass.hInstance		= hInstance ;
	wndclass.hIcon			= LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor		= LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground	= (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName	= NULL ;
	wndclass.lpszClassName	= szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL,
			TEXT ("This program requires Windows NT!"),
			szAppName,
			MB_ICONERROR) ;
		return 0 ;
	}

	hwnd	= CreateWindow(	szAppName,					// window class name
		TEXT ("SeeTheSky"),				// window caption
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_CAPTION,		// window style
		0,				// initial x position
		0,				// initial y position
		CW_USEDEFAULT,				// initial x size
		CW_USEDEFAULT,				// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters
	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	GetClientRect(hwnd, &rectMain);
	cxClient	= rectMain.right - rectMain.left;
	cyClient	= rectMain.bottom- rectMain.top;
	pTemp.x		= cxClient/3;
	pTemp.y		= cyClient/3;
	//	while (GetMessage (&msg, NULL, 0, 0))
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static	HDC		hDC, hMemDC;
	static	int		iTime;
	static	BOOL	bCursorState;
	RECT			rectMain;

	static	HBITMAP	hBitmap, hOldBitmap;


	PAINTSTRUCT		ps ;
	POINT			pTemp;

	static	HINSTANCE	hInstance ;
	static	RECT	rect ;

	static	USER		User;
	static	FALLTHING	Coin[MAX_COIN_NUM];
	static	int		CoinNum;
	static	FALLTHING	Cake[MAX_CAKE_NUM];
	static	int		CakeNum;
	static	FALLTHING	Sword[MAX_SWORD_NUM];
	static	int		SwordNum;

	static	int		cxChar, cyChar;
	static	int		cxClient, cyClient;	//窗口宽度和高度
	static	BOOL	bPause;
	static	BOOL	bGameOver;
	static	BOOL	bShownInfo;
	int	i, j;


	RECT	rectText;
	static	RECT	hrect;

	TCHAR	szStr[128];


	switch (message)
	{
	case WM_CREATE:
		SendMessage(hwnd,
			WM_SETICON,
			(WPARAM)TRUE,
			(LPARAM)LoadIcon(GetModuleHandle(NULL),
			(LPCTSTR)IDI_ICON1));
		MessageBox(hwnd, "SeeTheSky v0.6.0，冰火梦幻个人作品。", "欢迎试用", MB_OK);
		PlaySound("Jump.wav", NULL, SND_FILENAME | SND_ASYNC);
		PlaySound("Explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
		//initialization
		SetTimer(hwnd, TIMER_SEC,	TIME_REFRESH, NULL) ;
		SetTimer(hwnd, TIMER_INFO,	TIME_SHOWINFO, NULL) ;
		GetClientRect (hwnd, &rect) ;	//
		cxChar	= LOWORD(GetDialogBaseUnits());
		cyChar	= HIWORD(GetDialogBaseUnits());
		cxClient= rect.right-rect.left;
		cyClient= rect.bottom-rect.top;


		CoinNum	= 2;
		CakeNum	= 1;
		SwordNum= 3;

		for (i=0 ; i<CakeNum ; i++)
		{
			Cake[i]	= FALLTHING(40, 40, cxClient, CAKE_TYPE, "Cake.bmp");
		}
		for (i=0 ; i<SwordNum ; i++)
		{
			Sword[i]= FALLTHING(20, 100, cxClient, SWORD_TYPE, "Sword.bmp");
		}
		for (i=0 ; i<CoinNum ; i++)
		{
			Coin[i]= FALLTHING(20, 20, cxClient, COIN_TYPE, "Coin.bmp");
		}
		User	= USER(cxClient/2, cyClient - GROUND_HEIGHT - USER_HEIGHT, 0.0, 0.0,
			USER_WIDTH, USER_HEIGHT, "UserBody.bmp");
		iTime	= 0;
		bCursorState	= TRUE;
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case TIMER_SEC:
			GetClientRect(hwnd, &rectMain);

			//获得2个前台DC//////////////////////////////////////
			hDC		= GetDC(hwnd);
			hMemDC	= CreateCompatibleDC(hDC);

			hBitmap	= CreateCompatibleBitmap(hDC, rectMain.right, rectMain.bottom);
			hOldBitmap	= (HBITMAP)SelectObject(hMemDC, hBitmap);


				
			if (FALSE == bPause)
			{
				iTime++;
				//碰撞检测
				for (i=0 ; i<CakeNum ; i++)
				{
					if (Cake[i].bAlive && Cake[i].CrashCheck(User.cxPosition, User.cyPosition))
					{
						Cake[i].cxPosition = rand()%cxClient;
						Cake[i].cyPosition	= 0;
						Cake[i].dySpeed	= rand()%5+5;
						User.iHP		+= Cake[i].Other;
					}
				}
				for (i=0 ; i<SwordNum ; i++)
				{
					if (Sword[i].bAlive && Sword[i].CrashCheck(User.cxPosition, User.cyPosition))
					{
						Sword[i].cxPosition = rand()%cxClient;
						Sword[i].cyPosition	= 0;
						Sword[i].dySpeed	= rand()%5+5;
						User.iHP		-= Sword[i].Other;
						if (User.iHP <= 0)
						{
							PlaySound("Explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
							User.iHP = 0;
							User.bAlive	= false;
							bGameOver= true;
							bPause	= true;
						}
					}
				}
				for (i=0 ; i<CoinNum ; i++)
				{
					if (Coin[i].bAlive && Coin[i].CrashCheck(User.cxPosition, User.cyPosition))
					{
						Coin[i].cxPosition = rand()%cxClient;
						Coin[i].cyPosition	= 0;
						Coin[i].dySpeed	= rand()%5+5;
						User.iMark		+= Coin[i].Other;
					}
				}

				//////Draw start
				for (i=0 ; i<CakeNum ; i++)
				{
					if (Cake[i].bAlive)
					{
						Cake[i].Move(cxClient, cyClient);
						Cake[i].Draw(hDC, hMemDC);
					}
				}
				for (i=0 ; i<SwordNum ; i++)
				{
					if (Sword[i].bAlive)
					{
						Sword[i].Move(cxClient, cyClient);
						Sword[i].Draw(hDC, hMemDC);
					}
				}
				for (i=0 ; i<CoinNum ; i++)
				{
					if (Coin[i].bAlive)
					{
						Coin[i].Move(cxClient, cyClient);
						Coin[i].Draw(hDC, hMemDC);
					}
				}
				User.Move(cxClient, cyClient);
				User.Draw(hDC, hMemDC);
				User.ShowInfo(hMemDC, cxClient, cyClient);
				DrawTime(hMemDC, cxChar*20, 20, iTime);
			}
			else///////////////////////////////////////////
			{
				if (FALSE == bGameOver)
				{

					if (bShownInfo)
					{
						SetBkMode(hMemDC, TRANSPARENT);
						SetTextColor(hMemDC, RGB(255, 200, 200));
						TextOut(hMemDC, cxClient/2-2*cxChar, cyClient/2-cyChar*5, "Pause", 5);
					}
				}
				else
				{
					if (bShownInfo)
					{
						SetBkMode(hMemDC, TRANSPARENT);
						SetTextColor(hMemDC, RGB(255, 220, 200));
						TextOut(hMemDC, cxClient/2-12*cxChar, cyClient/2-cyChar*5, "Press [Enter] to restart", 24);
						sprintf(szStr, "Mark: ");
						Number2String(User.iMark, 6, szStr+6);
						TextOut(hMemDC, cxClient/2-9*cxChar, cyClient/2-cyChar*7, szStr, lstrlen(szStr));
					}
				}
			}
			/////////////////////////////////
			for (i=0 ; i<cxClient ; i++)
			{
				for (j=0 ; j<4 ; j++)
				{
					SetPixel(hMemDC, i, j+cyClient-GROUND_HEIGHT-USER_HEIGHT, RGB(0, 200, 0));
				}
			}

			//////Draw over
			BitBlt(hDC,	0,	0,	cxClient,	cyClient,
				hMemDC,	0, 0, SRCCOPY);

			
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
			ReleaseDC(hwnd, hDC);
			DeleteObject(hBitmap);
			break;
		case	TIMER_INFO:
			bShownInfo	= !bShownInfo;
			break;
		}
		return 0;
	case	WM_KEYDOWN:
		switch (wParam)
		{
		case	VK_LEFT:
			User.LeftMove	= 1;
			break;
		case	VK_RIGHT:
			User.RightMove	= 1;
			break;
		case	VK_UP:
		case	'J':
			User.Jump(cyClient);
			break;
		case	'P':
			bPause	= !bPause;
			break;
		case	VK_ESCAPE:
			PostQuitMessage (0) ;
			break;
		case	VK_RETURN:
			if (bGameOver)
			{
				iTime		= 0;
				bGameOver	= false;
				bPause		= false;
				User.bAlive	= true;
				User.iHP	= 100;
				User.cySpeed= 0;
				User.cxPosition=cxClient/2;
				User.cyPosition=cyClient-GROUND_HEIGHT-USER_HEIGHT; 
				User.iMark	= 0;
				for (i=0 ; i<CoinNum ; i++)
				{
					Coin[i].cxPosition	= rand()%cxClient;
					Coin[i].cyPosition	= 0;
				}
				for (i=0 ; i<CakeNum ; i++)
				{
					Cake[i].cxPosition	= rand()%cxClient;
					Cake[i].cyPosition	= 0;
				}
				for (i=0 ; i<SwordNum ; i++)
				{
					Sword[i].cxPosition	= rand()%cxClient;
					Sword[i].cyPosition	= 0;
				}
			}
		}
		return 0;
	case	WM_KEYUP:
		switch (wParam)
		{
		case	VK_LEFT:
			User.LeftMove	= 0;
			break;
		case	VK_RIGHT:
			User.RightMove	= 0;
			break;
		}
		return 0;
	case   WM_COMMAND :
		return 0;
	case	WM_LBUTTONUP:
		return 0;
	case	WM_RBUTTONUP:
		return 0;
	case	WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		rect.right	= rect.left + cxClient;
		rect.bottom	= rect.top + cyClient;
		return 0 ;
	case   WM_PAINT:
		hDC = BeginPaint (hwnd, &ps) ;
		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_NCMOUSEMOVE:
		{
			if(bCursorState == FALSE)
			{
				bCursorState = TRUE;
				ShowCursor(TRUE);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			if(bCursorState == TRUE)
			{
				bCursorState = FALSE;
				ShowCursor(FALSE);
			}
			// 然后做别的事情
		}
		break;
		// 其他的case项目就不用变了

	case   WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
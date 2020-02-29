#include <windows.h>
#include <string.h>
#include <stdio.h>

int ShowBitmapResource(HDC hDestDC, int xDest, int yDest)
{
	HDC hSrcDC; // source DC - memory device context
	HBITMAP hbitmap; // handle to the bitmap resource
	BITMAP bmp; // structure for bitmap info
	int nHeight, nWidth; // bitmap dimensions

	// first load the bitmap resource
	if ((hbitmap = (HBITMAP)LoadImage(NULL, TEXT("D:\\User.bmp"), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE)) == NULL)
		return(FALSE);

	// create a DC for the bitmap to use
	if ((hSrcDC = CreateCompatibleDC(NULL)) == NULL)
		return(FALSE);

	// select the bitmap into the DC
	if (SelectObject(hSrcDC, hbitmap) == NULL)
		return(FALSE);

	// get image dimensions
	if (GetObject(hbitmap, sizeof(BITMAP), &bmp) == 0)
		return(FALSE);

	nWidth = bmp.bmWidth;
	nHeight = bmp.bmHeight;

	// copy image from one DC to the other
	if (BitBlt(hDestDC, xDest, yDest, nWidth, nHeight, hSrcDC, 0, 0, SRCCOPY) == NULL)
		return(FALSE);

	// kill the memory DC
	DeleteDC(hSrcDC);

	// return success!
	return(TRUE);
}


BOOL	Number2String(int iNumber, int iLen, TCHAR *szStr)
{
	if (0 == iNumber)
	{
		*(szStr+iLen)	= '\0';
		while (iLen--)
		{
			*(szStr+iLen)	= '0';
		}
		return TRUE;
	}
	else
	{
		if (0 > iNumber)
		{
			sprintf(szStr, TEXT("-"));
			iNumber	= -iNumber;
			szStr++;
		}
		szStr+=iLen;
		*szStr = '\0';
		for (iLen--,szStr-- ; iLen >=0 ; iLen--, iNumber /= 10, szStr--)
		{
			*szStr = iNumber%10+'0';
		}
		return (-1 == iLen);
	}
}


VOID	DrawTime(HDC hDC, int px, int py, int time)
{
	static	int i, j, hour, minute, second, mcsecond;
	static	TCHAR	Time[16];
	mcsecond= time % 100;
	time	/= 100;
	second	= time % 60;
	time	/=60;
	minute	= time % 60;
	time	/=60;
	hour	= time;

	Time[0]	= '0' + hour/10;
	Time[1]	= '0' + hour%10;
	Time[2]	= 'h';
	Time[3]	= '0' + minute/10;
	Time[4]	= '0' + minute%10;
	Time[5]	= '\'';
	Time[6]	= '0' + second/10;
	Time[7]	= '0' + second%10;
	Time[8]	= '\"';
	Time[9]	= '0' + mcsecond/10;
	Time[10]	= '0' + mcsecond%10;
	Time[11]	= '\0';
	SetTextColor(hDC, RGB(255, 200, 200));
	SetBkMode(hDC, TRANSPARENT);
	TextOut(hDC, px, py, Time, 11);
	return;
}
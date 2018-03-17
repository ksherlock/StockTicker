// StockTicker.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <stdio.h>
#include "StockTicker.h"
#include "resource.h"

#include "String.h"
#include "SocketIO.h"
#include "XmlParser.h"
#include "ColorButton.h"
#include "List.h"
#include "Benaphore.h"

/*
 *
 *
 */




bool do_xml(XmlParser &, BString &);
bool do_xml(XmlParser &);


HANDLE	fThread;	// background thread
HWND	fWindow;
bool	fQuit;
HFONT	fFont;
HMENU	fPopup;

Benaphore *fBenaphore;
HANDLE	fEvent;
long	fSignal;

HDC		fHdc;
HBITMAP	fBitmap;

/*--- preferences stuff ---*/
int			fPeriod;	// seconds between updates
COLORREF	fColors[3];	// winners, losers, inactive colors
BList		fStocks;



//BList sort callback
int listSort(const void *item1, const void *item2)
{
	return BString::Compare((BString *)item1, (BString *)item2);
}


DWORD WINAPI backgroundThread(void *data)
{
	while (true)
	{

		query_yahoo();


		InvalidateRect((HWND)data, NULL, false);

		if (WaitForSingleObject(fEvent, fPeriod * 100) == WAIT_TIMEOUT) 
			continue;
		if (InterlockedExchange(&fSignal, SIG_NULL) == SIG_DIE) 
			ExitThread(0);
	}

	return 0;
}

void draw(HWND hwnd, HDC hdc, RECT &updateRect)
{
RECT bounds;
#if 0
HBRUSH brushBlack;
HBRUSH brushGrey;

TRIVERTEX        vert[3] ;
GRADIENT_RECT    gRect;
#endif

TEXTMETRIC	tm;
HFONT oldFont;


	oldFont = fFont ? (HFONT)SelectObject(hdc, fFont) : NULL;

	GetClientRect(hwnd, &bounds);
	OffsetRect(&bounds, -bounds.left, -bounds.top);

	if (fHdc)
	{
		BitBlt(hdc, 0, 0, bounds.right, bounds.bottom, fHdc, 0, 0, SRCCOPY);
		
	}

#if 0
	brushBlack = CreateSolidBrush(0x00000000);
	brushGrey = CreateSolidBrush(0x00373737);



gRect.UpperLeft  = 0;
gRect.LowerRight = 1;

vert [0] .x      = 0;
vert [0] .y      = 0;
vert [0] .Red    = 0x1800;
vert [0] .Green  = 0x1800;
vert [0] .Blue   = 0x2900;
vert [0] .Alpha  = 0x0000;

vert [1] .x      = bounds.right / 2;
vert [1] .y      = bounds.bottom; 
vert [1] .Red    = 0x3800;
vert [1] .Green  = 0x3800;
vert [1] .Blue   = 0x5900;
vert [1] .Alpha  = 0x0000;

vert [2] .x      = bounds.right;
vert [2] .y      = bounds.bottom;
vert [2] .Red    = 0x1800;
vert [2] .Green  = 0x1800;
vert [2] .Blue   = 0x2900;
vert [2] .Alpha  = 0x0000;

	GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
	vert[1].y = 0;
	GradientFill(hdc,&vert[1],2,&gRect,1,GRADIENT_FILL_RECT_H);

	// black box
	InflateRect(&bounds, -20, -20);
	OffsetRect(&bounds, 2, 2);
	FillRect(hdc, &bounds, brushGrey);
	OffsetRect(&bounds, -2, -2);
	FillRect(hdc, &bounds, brushBlack);
#endif
	InflateRect(&bounds, -20, -20);
	SetBkColor(hdc, 0x00000000);

	RECT r;
	BString *strings;
	int i;


	GetTextMetrics(hdc, &tm);
	r.left = bounds.left;
	r.top = bounds.top;
	r.bottom = r.top + tm.tmHeight;
	
	for (i = 0; i < fStocks.CountItems(); i++)
	{
		strings = (BString *)fStocks.ItemAt(i);
		if (!strings || !strings[0].Length()) continue;

		SetTextColor(hdc, fColors[0]);  // default - assume ok
		// if no last price, assume inactive
		if (!strings[1].Length()) SetTextColor(hdc, fColors[2]);
		// or if the change is -, is a loser
		else if (strings[2].ByteAt(0) == '-') SetTextColor(hdc, fColors[1]);
		
		//reset the left position
		r.left = bounds.left;
		r.right = r.left + tm.tmMaxCharWidth * 4;

		// symbol
		DrawText(hdc, strings[0].String(), strings[0].Length(), 
			&r, DT_LEFT | DT_TOP);


		// last price
		r.left += 7 * tm.tmMaxCharWidth;
		r.right = r.left + tm.tmMaxCharWidth * 5;
		DrawText(hdc, strings[1].String(), strings[1].Length(), &r, DT_RIGHT | DT_TOP);
		

		// delta price
		r.left += 6 * tm.tmMaxCharWidth;
		r.right = r.left + tm.tmMaxCharWidth * 6;
		DrawText(hdc, strings[2].String(), strings[2].Length(), &r, DT_RIGHT | DT_TOP);


		// delta %
		r.left += 6 * tm.tmMaxCharWidth;
		r.right = r.left + tm.tmMaxCharWidth * 6;
		DrawText(hdc, strings[3].String(), strings[3].Length(), &r, DT_RIGHT | DT_TOP);

		// volume
		r.left += 6 * tm.tmMaxCharWidth;
		r.right = r.left + tm.tmMaxCharWidth * 8;
		DrawText(hdc, strings[4].String(), strings[4].Length(), &r, DT_RIGHT | DT_TOP);




		// next line
		r.bottom += tm.tmHeight ; //* 1.5;
		r.top += tm.tmHeight ; //* 1.5;
	}
#if 0
	DeleteObject(brushBlack);
	DeleteObject(brushGrey);
#endif
	if (oldFont) SelectObject(hdc, oldFont);
}


void Resize(HWND window, int count)
{
	TEXTMETRIC tm;
	HFONT oldFont;
	RECT frame, client;
	HDC hdc;
	int height, width;

	HBRUSH brushBlack;
	HBRUSH brushGrey;

	TRIVERTEX        vert[3] ;
	GRADIENT_RECT    gRect;



	hdc = GetDC(window);

	oldFont = fFont ? (HFONT)SelectObject(hdc, fFont) : NULL;

	GetTextMetrics(hdc, &tm);

	GetClientRect(window, &client);
	GetWindowRect(window, &frame);

	height = (frame.bottom - frame.top) - (client.bottom - client.top);
	height += count * tm.tmHeight;
	height += 40;

	width = (frame.right - frame.left) - (client.right - client.left);
	width += 35 * tm.tmMaxCharWidth;
	width += 40;

	MoveWindow(window, frame.left, frame.top, 
		width, height, true);

	GetClientRect(window, &client);
	OffsetRect(&client, -client.left, -client.top);
	
	if (oldFont) SelectObject(hdc, oldFont);


	if (!fHdc) fHdc = CreateCompatibleDC(hdc);

	BITMAPINFOHEADER bmi;

	memset(&bmi, 0, sizeof(BITMAPINFOHEADER));
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = client.right;
	bmi.biHeight = client.bottom;
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = (client.right * 32 * client.bottom) / 8;
	bmi.biClrImportant = 0;

	if (fBitmap) DeleteObject(fBitmap);

	fBitmap = CreateDIBSection(fHdc, (BITMAPINFO *)&bmi, DIB_RGB_COLORS, NULL, NULL, 0); 
	SelectObject(fHdc, fBitmap);


	brushBlack = CreateSolidBrush(0x00000000);
	brushGrey = CreateSolidBrush(0x00373737);


	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	vert[0] .x      = 0;
	vert[0] .y      = 0;
	vert[0] .Red    = 0x1800;
	vert[0] .Green  = 0x1800;
	vert[0] .Blue   = 0x2900;
	vert[0] .Alpha  = 0x0000;

	vert[1] .x      = client.right / 2;
	vert[1] .y      = client.bottom; 
	vert[1] .Red    = 0x3800;
	vert[1] .Green  = 0x3800;
	vert[1] .Blue   = 0x5900;
	vert[1] .Alpha  = 0x0000;

	vert[2] .x      = client.right;
	vert[2] .y      = client.bottom;
	vert[2] .Red    = 0x1800;
	vert[2] .Green  = 0x1800;
	vert[2] .Blue   = 0x2900;
	vert[2] .Alpha  = 0x0000;

	GradientFill(fHdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
	vert[1].y = 0;
	GradientFill(fHdc,&vert[1],2,&gRect,1,GRADIENT_FILL_RECT_H);

	// black box
	InflateRect(&client, -20, -20);
	OffsetRect(&client, 2, 2);
	FillRect(fHdc, &client, brushGrey);
	OffsetRect(&client, -2, -2);
	FillRect(fHdc, &client, brushBlack);

	DeleteObject(brushBlack);
	DeleteObject(brushGrey);
	//
	ReleaseDC(window, hdc);
}

LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case ST_RESIZE:
		Resize(hWnd, fStocks.CountItems());
		return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CREATE:
		// create my font
		{
		HDC hdc;
		hdc = GetDC(hWnd);

		fFont = CreateFont(
			-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72),
			0, 0, 0, FW_DONTCARE, 
			false, false, false, 
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
			FIXED_PITCH | FF_MODERN, "Courier New");
		if (fFont)
		{
			SendMessage(hWnd, WM_SETFONT, (WPARAM)fFont, (LPARAM)false);
			Resize(hWnd, fStocks.CountItems());
		}
		ReleaseDC(hWnd, hdc);
		}
		return 0;
		break;

	case WM_RBUTTONDOWN:
		{
			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			ClientToScreen(hWnd, &point);
			TrackPopupMenu(fPopup, 
				TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
				point.x, point.y, 0, hWnd, NULL);
		}
		return 0;
		break;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case ID_ABOUT:
				break;
			case ID_PREFERENCES:
				DialogBox(
					(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
					MAKEINTRESOURCE(DLG_SETTINGS),
					hWnd,
					(DLGPROC)preferencesProc);
				break;

			case ID_QUICKADD:
					DialogBoxParam(
						(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
						MAKEINTRESOURCE(DLG_ADD),
						hWnd,
						(DLGPROC)addProc, -1);
					break;

			case ID_QUICKDELETE:
					DialogBoxParam(
						(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
						MAKEINTRESOURCE(DLG_ADD),
						hWnd,
						(DLGPROC)addProc, -2);
					break;

			case ID_REFRESH:
				InterlockedExchange(&fSignal, SIG_REFRESH);
				SetEvent(fEvent);
				break;

			}
		}
		return 0;
		break;

	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			RECT updateRect;
			if (wParam)	hdc = (HDC)wParam;
			else hdc = BeginPaint(hWnd, &ps);
			GetUpdateRect(hWnd, &updateRect, 0);
			fBenaphore->Lock();
			draw(hWnd, hdc, updateRect);
			fBenaphore->Unlock();
			if (wParam == 0) EndPaint(hWnd, &ps);
		}
		return 0;
	default: return DefWindowProc(hWnd, message, wParam, lParam); 
	}
	return 0;
}
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
DWORD fThreadId;
MSG msg;
WNDCLASSEX wcex;
WSADATA wsaData;
MENUITEMINFO menuiteminfo;

BString *strings;

	Register_ColorButton(hInstance);
	WSAStartup(MAKEWORD(2, 0), &wsaData);


	fBenaphore = new Benaphore(2);

	fFont = NULL;
	fBitmap = NULL;
	fHdc = NULL;

	fPopup = CreatePopupMenu();
	memset(&menuiteminfo, 0, sizeof(MENUITEMINFO));
	menuiteminfo.cbSize = sizeof(MENUITEMINFO);
	menuiteminfo.fMask = MIIM_TYPE | MIIM_ID;
	menuiteminfo.fType = MFT_STRING;
	menuiteminfo.fState = MFS_DEFAULT;

	menuiteminfo.wID = ID_PREFERENCES;
	menuiteminfo.dwTypeData = "Preferences...";
	InsertMenuItem(fPopup, ID_PREFERENCES, false, &menuiteminfo);

	menuiteminfo.wID = ID_ABOUT;
	menuiteminfo.dwTypeData = "About...";
	InsertMenuItem(fPopup, ID_ABOUT, false, &menuiteminfo);


	menuiteminfo.fType = MFT_SEPARATOR;
	InsertMenuItem(fPopup, 0, false, &menuiteminfo);

	menuiteminfo.fType = MFT_STRING;

	menuiteminfo.wID = ID_QUICKADD;
	menuiteminfo.dwTypeData = "Quick Add";
	InsertMenuItem(fPopup, ID_QUICKADD, false, &menuiteminfo);

	menuiteminfo.wID = ID_QUICKDELETE;
	menuiteminfo.dwTypeData = "Quick Delete";
	InsertMenuItem(fPopup, ID_QUICKDELETE, false, &menuiteminfo);


	menuiteminfo.wID = ID_REFRESH;
	menuiteminfo.dwTypeData = "Refresh";
	InsertMenuItem(fPopup, ID_REFRESH, false, &menuiteminfo);


	fEvent = CreateEvent(NULL, false, 0, NULL);
	fSignal = SIG_NULL;

	// initialize the settings.
	fPeriod = 3 * 60;	// 3 minutes
	fColors[0] = 0x0000ff00;	// birhgt green
	fColors[1] = 0x000000ff;	// bright red
	fColors[2] = 0x00007f00;	// dull green
	

	strings = new BString[6];
	strings[0] = "MSFT";
	fStocks.AddItem(strings);

	strings = new BString[6];
	strings[0] = "INTC";
	fStocks.AddItem(strings);

	strings = new BString[6];
	strings[0] = "LNUX";

	fStocks.AddItem(strings);
	fStocks.SortItems(listSort);


	memset(&wcex, 0, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
	wcex.lpfnWndProc	= (WNDPROC)windowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)STOCKTICKER_ICON);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "kelvin~stock ticker";
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);

	fWindow = CreateWindow("kelvin~stock ticker", "Stock Ticker", 
		WS_OVERLAPPEDWINDOW & (~WS_SIZEBOX) & (~WS_MAXIMIZEBOX),
		CW_USEDEFAULT, 0, 320, 200, NULL, NULL, hInstance, NULL);

	fThread = CreateThread(NULL, 0, backgroundThread, fWindow, 
		CREATE_SUSPENDED, &fThreadId);
	

	ShowWindow(fWindow, nCmdShow);
	// kiss of life
	ResumeThread(fThread);
	// main loop....
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		DispatchMessage(&msg);
	}

	UnRegister_ColorButton();
	DestroyMenu(fPopup);

	// exit - signal the other thread
	InterlockedExchange(&fSignal, SIG_DIE);
	SetEvent(fEvent);

	WaitForSingleObject(fThread, 0);
	DeleteObject(fEvent);

	if (fFont) DeleteObject(fFont);


	delete fBenaphore;

	// free up the memory
	for (int i = fStocks.CountItems() ; i ; i--)
	{
		strings = (BString *)fStocks.RemoveItem(i - 1);
		if (strings) delete [] strings;
	}
	if (fBitmap) DeleteObject(fBitmap);
	if (fHdc) DeleteDC(fHdc);
	return 0;
}




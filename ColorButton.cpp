/*
 *
 * implements a custom color button control for dialog boxes...
 *
 */

#include <windows.h>

#include "ColorButton.h"


static COLORREF cust_colors[16] =
{
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
	0x00ffffff,
};


enum
{
	NO_TRACK = 0,
	IN_TRACK,
	OUT_TRACK
};

static LRESULT
ColorButton_Create(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
LONG color;
	color = (LONG)0;
	SetWindowLong(hWnd, 0, color);
	SetWindowLong(hWnd, 4, NO_TRACK); // not tracking
	return 0;
}


static void ColorButton_Refresh(HWND hWnd, HDC hdc)
{
RECT rect;
HBRUSH hbr;
LONG color;
HPEN pen_dgrey;
HPEN pen_lgrey;
HPEN pen_white;
HPEN pen_black;
HPEN pen_orig;
bool depress;

	depress = (GetWindowLong(hWnd, 4) == IN_TRACK);
 
	pen_black = CreatePen(PS_SOLID, 1, 0x00000000);
	pen_white = CreatePen(PS_SOLID, 1, 0x00ffffff);
	pen_dgrey = CreatePen(PS_SOLID, 1, 0x00808080);
	pen_lgrey = CreatePen(PS_SOLID, 1,	GetSysColor(COLOR_BTNFACE));
	GetClientRect(hWnd, &rect);
	hbr = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));


	pen_orig = (HPEN)SelectObject(hdc, depress ? pen_black : pen_white);
	MoveToEx(hdc, rect.left, rect.bottom - 1, NULL);
	LineTo(hdc, rect.left, rect.top); 
	LineTo(hdc, rect.right - 1, rect.top); 
	SelectObject(hdc, depress ? pen_white : pen_black);
	LineTo(hdc, rect.right - 1, rect.bottom - 1);
	LineTo(hdc, rect.left - 1, rect.bottom - 1); // left = 1 since it doesn't draw the final

	InflateRect(&rect, -1, -1);

	SelectObject(hdc, depress ? pen_dgrey : pen_lgrey);
	MoveToEx(hdc, rect.left, rect.bottom - 1, NULL);
	LineTo(hdc, rect.left, rect.top); 
	LineTo(hdc, rect.right - 1, rect.top); 
	SelectObject(hdc, depress ? pen_lgrey : pen_dgrey);
	MoveToEx(hdc, rect.right -1, rect.top, NULL);
	LineTo(hdc, rect.right - 1, rect.bottom - 1);
	LineTo(hdc, rect.left - 1, rect.bottom - 1);

	InflateRect(&rect, -1, -1);
	FillRect(hdc, &rect, hbr);
	DeleteObject(hbr);

	// now draw a border around the color chunk 

	//hbr = CreateSolidBrush((COLORREF)0);  // black

	//if (depress) OffsetRect(&rect, 1, 1);
	InflateRect(&rect, -2, -2);
	//FrameRect(hdc, &rect, hbr);
	//DeleteObject(hbr);

	//press = !depress;
	//depress = 1;
	if (!depress)
	{
		SelectObject(hdc, pen_black);
		MoveToEx(hdc, rect.left, rect.bottom - 1, NULL);
		LineTo(hdc, rect.left, rect.top); 
		LineTo(hdc, rect.right - 1, rect.top); 
		SelectObject(hdc, pen_white);
		LineTo(hdc, rect.right - 1, rect.bottom - 1);
		LineTo(hdc, rect.left - 1, rect.bottom - 1);	
	}
#if 0
	InflateRect(&rect, -1, -1);

	SelectObject(hdc, depress ? pen_dgrey : pen_lgrey);
	MoveToEx(hdc, rect.left, rect.bottom - 1, NULL);
	LineTo(hdc, rect.left, rect.top); 
	LineTo(hdc, rect.right - 1, rect.top); 
	SelectObject(hdc, depress ? pen_lgrey : pen_dgrey);
	MoveToEx(hdc, rect.right -1, rect.top, NULL);
	LineTo(hdc, rect.right - 1, rect.bottom - 1);
	LineTo(hdc, rect.left - 1, rect.bottom - 1);
#endif


	// and draw the color itself.
	color = GetWindowLong(hWnd, 0);
	hbr = CreateSolidBrush((COLORREF)color);
	InflateRect(&rect, -1, -1);

	FillRect(hdc, &rect, hbr);
	DeleteObject(hbr);

	SelectObject(hdc, pen_orig);
	DeleteObject(pen_white);
	DeleteObject(pen_black);
	DeleteObject(pen_dgrey);
	DeleteObject(pen_lgrey);
}

static LRESULT
ColorButton_Paint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
HDC hdc;
PAINTSTRUCT ps;

	hdc = (wParam == 0) ? BeginPaint(hWnd, &ps) : (HDC)wParam;
	ColorButton_Refresh(hWnd, hdc);
	if (wParam == 0) EndPaint(hWnd, &ps);
	return 0;
}



static LRESULT
ColorButton_LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
CHOOSECOLOR cc;
LONG color;
long old_track;

	ReleaseCapture();	// no longer care

	old_track = GetWindowLong(hWnd, 4);
	SetWindowLong(hWnd, 4, NO_TRACK); // no longer tracking

	if (old_track != IN_TRACK) return 0; 

	InvalidateRect(hWnd, NULL, true); // redraw as non-depressed
	color = GetWindowLong(hWnd, 0); 

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hWnd;
	cc.hInstance = NULL;
	cc.rgbResult = color;
	cc.lpCustColors = cust_colors;
	cc.Flags = CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	if (ChooseColor(&cc))
	{
		SetWindowLong(hWnd, 0, (LONG)cc.rgbResult);
		InvalidateRect(hWnd, NULL, true);
	}
	return 0;

}

static LRESULT
ColorButton_LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SetCapture(hWnd);			// start tracking the mouse 
	SetWindowLong(hWnd, 4, IN_TRACK);	// tracking;
	InvalidateRect(hWnd, NULL, true);
	return 0;
}

static LRESULT
ColorButton_MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
POINT point;
RECT rect;
long old_track;
int x;

	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);
	old_track = GetWindowLong(hWnd, 4);
	if (old_track == NO_TRACK) return 0;
	GetClientRect(hWnd, &rect);

	x = 0;
	if (old_track == IN_TRACK) x++;
	if (PtInRect(&rect, point)) x++;
	if (x == 1)
	{
		SetWindowLong(hWnd, 4, old_track == IN_TRACK ? OUT_TRACK : IN_TRACK);
		InvalidateRect(hWnd, NULL, true);
	}
	return 0;
}

static LRESULT
ColorButton_Size(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, NULL, true);
	return 0;
}

static LRESULT
ColorButton_SetColor(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SetWindowLong(hWnd, 0, (LONG)wParam);
	InvalidateRect(hWnd, NULL, true);
	return 0;
}

static LRESULT
ColorButton_GetColor(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)GetWindowLong(hWnd, 0);
}

static LRESULT
ColorButton_NCHitTest(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
POINT point;
RECT rect;

	GetClientRect(hWnd, &rect);
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);

	ScreenToClient(hWnd, &point);

	if (PtInRect(&rect, point))
		return HTCLIENT;

	return HTTRANSPARENT; // didn't hit our button
}

static LRESULT WINAPI
ColorButton_WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
		return true;

	case WM_NCHITTEST:
		return ColorButton_NCHitTest(hWnd, wParam, lParam);

	case COLOR_GETCOLOR:
		return ColorButton_GetColor(hWnd, wParam, lParam);

	case COLOR_SETCOLOR:
		return ColorButton_SetColor(hWnd, wParam, lParam);

	case WM_CREATE:
		return ColorButton_Create(hWnd, wParam, lParam);

	case WM_KILLFOCUS:
		return 0; //ColorButton_KillFocus(hWnd, wParam, lParam);

	case WM_DESTROY:
		return 0; //ColorButton_Destroy(hWnd, wParam, lParam);

	case WM_LBUTTONDOWN:
      return ColorButton_LButtonDown (hWnd, wParam, lParam);
	 
	case WM_LBUTTONUP:
		return ColorButton_LButtonUp(hWnd, wParam, lParam);

	case WM_MOUSEMOVE:
		return ColorButton_MouseMove(hWnd, wParam, lParam);

    case WM_PAINT:
      return ColorButton_Paint (hWnd, wParam, lParam);

    case WM_SETFOCUS:
      return 0; //ColorButton_SetFocus (hwnd, wParam, lParam);

    case WM_SIZE:
      return ColorButton_Size (hWnd, wParam, lParam);

	}
	return 0;
}


void Register_ColorButton(HINSTANCE hInst)
{
WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.hInstance  = hInst;
	wc.lpfnWndProc = ColorButton_WindowProc;
	wc.cbWndExtra = 8; // extra bits for the color ref + flags
	wc.lpszClassName = "WinS_ColorButton";
	wc.style = CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

}
void UnRegister_ColorButton(void)
{
	UnregisterClass("WinS_ColorButton", NULL);
}

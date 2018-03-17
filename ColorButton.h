#ifndef __COLORBUTTON_H__
#define __COLORBUTTON_H__

#include <windows.h>
#include <winuser.h>

enum
{
	COLOR_GETCOLOR = WM_USER,
	COLOR_SETCOLOR
};

inline void SetColor(HWND hWnd, COLORREF cr)
{
	PostMessage(hWnd, COLOR_SETCOLOR, (WPARAM)cr, 0);
}

inline COLORREF GetColor(HWND hWnd)
{
	return (COLORREF)SendMessage(hWnd, COLOR_GETCOLOR, 0, 0);
}


extern void Register_ColorButton(HINSTANCE);
extern void UnRegister_ColorButton(void);

#endif
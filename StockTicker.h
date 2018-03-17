#ifndef __STOCKTICKER_H__
#define __STOCKTICKER_H__

#include <windows.h>
#include "List.h"

class Benaphore;

extern BList fStocks;
extern HWND fWindow;
extern Benaphore *fBenaphore;
extern int fPeriod;
extern COLORREF fColors[];
extern HANDLE fEvent;
extern long fSignal;

extern int listSort(const void *item1, const void *item2);

enum
{
	ST_RESIZE	= WM_USER,
};

// ipc signals
enum
{
	SIG_NULL = 0,
	SIG_REFRESH,
	SIG_DIE = -1
};


LRESULT CALLBACK preferencesProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK addProc(HWND, UINT, WPARAM, LPARAM);

bool query_yahoo(void);
bool query_nsdaq(void);

#endif
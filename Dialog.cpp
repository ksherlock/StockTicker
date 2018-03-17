#include <windows.h>
#include "resource.h"
#include "StockTicker.h"
#include "String.h"
#include "Benaphore.h"
#include "ColorButton.h"


void addText(BString& text, HWND ctl)
{
int start, end;
BString str;

	start = 0;
	end = 0;
	
	text.ReplaceAll('\t', ' ');
	text.ReplaceAll(',', ' ');

	while (true)
	{
		// skip over any leading spaces or tabs
		for (start = end; start < text.Length(); start++)
		{
			if (text[start] == ' ') continue;
			break;
		}

		if (start == text.Length()) break;
	
		end = text.FindFirst(' ', start);
		if (end == -1) end = text.Length();

		str.Truncate(0);
		
		text.CopyInto(str, start, end - start);	// s/b -1?

		str.ToUpper();

		if (SendMessage(ctl, LB_FINDSTRINGEXACT, -1, (LPARAM)str.String()) == LB_ERR)
		{
			SendMessage(ctl, LB_ADDSTRING, 0, (LPARAM)str.String());
		}
	}

}


/* add symbols to a BList */
bool addText(BString& text)
{
int start, end;
BString str;
bool ret;

	ret = false;

	start = 0;
	end = 0;
	
	text.ReplaceAll('\t', ' ');
	text.ReplaceAll(',', ' ');

	fBenaphore->Lock();

	while (true)
	{
		// skip over any leading spaces or tabs
		for (start = end; start < text.Length(); start++)
		{
			if (text[start] == ' ') continue;
			break;
		}

		if (start == text.Length()) break;
	
		end = text.FindFirst(' ', start);
		if (end == -1) end = text.Length();

		str.Truncate(0);
		
		text.CopyInto(str, start, end - start);	// s/b -1?

		str.ToUpper();

		int32 i;
		for(i = 0; i < fStocks.CountItems(); i++)
		{
			BString *tmp = (BString *)fStocks.ItemAt(i);
			if (!tmp) continue;
			if (tmp->ICompare(str) == 0) break;
		}
		if (i == fStocks.CountItems())
		{
			BString *tmp;
			tmp = new BString[6];
			tmp[0].Adopt(str);
			fStocks.AddItem(tmp);
			ret = true;
		}
	}
	if (ret)
	{
		fStocks.SortItems(listSort);
		InterlockedExchange(&fSignal, SIG_REFRESH);
		SetEvent(fEvent);
	}
	fBenaphore->Unlock();
	return ret;
}

/* remove symbols to a BList */
bool removeText(BString& text)
{
int start, end;
BString str;
bool ret;

	ret = false;

	start = 0;
	end = 0;
	
	text.ReplaceAll('\t', ' ');
	text.ReplaceAll(',', ' ');


	while (true)
	{
		// skip over any leading spaces or tabs
		for (start = end; start < text.Length(); start++)
		{
			if (text[start] == ' ') continue;
			break;
		}

		if (start == text.Length()) break;
	
		end = text.FindFirst(' ', start);
		if (end == -1) end = text.Length();

		str.Truncate(0);
		
		text.CopyInto(str, start, end - start);	// s/b -1?

		str.ToUpper();

		int32 i;
		for(i = 0; i < fStocks.CountItems(); i++)
		{
			BString *tmp = (BString *)fStocks.ItemAt(i);
			if (!tmp) continue;
			if (tmp->ICompare(str) == 0)
			{
				fBenaphore->Lock();
				fStocks.RemoveItem(i);
				delete []tmp;
				fBenaphore->Unlock();
				ret = true;
				break;
			}
		}
	}
	if (ret)
	{
		InterlockedExchange(&fSignal, SIG_REFRESH);
		SetEvent(fEvent);
	}
	return ret;
}



// if created with a paramter, the parameter is a list-box control.
//Otherwise, it should add directly to fStocks.
LRESULT CALLBACK addProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HWND ctl;
			ctl = GetDlgItem(hWnd, CTL_EDIT);
			SetFocus(ctl);
			SetWindowLong(hWnd, GWL_USERDATA, (long)lParam);
			if (lParam == -2)
			{
				SetDlgItemText(hWnd, CTL_STATIC, "Remove Symbols");
			}
		}
		return 0;
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;

		case IDOK:
			{
				HWND ctl;
				char *cp;
				int len;

				ctl = GetDlgItem(hWnd, CTL_EDIT);
				len = GetWindowTextLength(ctl);
				if (len)
				{
					long l;
					cp = new char[len + 1];
					GetWindowText(ctl, cp, len + 1);
					l = GetWindowLong(hWnd, GWL_USERDATA);

					switch(l)
					{
					case 0: break;
					case -1: 
						if (addText(BString(cp)))
							SendMessage(fWindow, ST_RESIZE, 0, 0);
						break;
					case -2:
						if (removeText(BString(cp)))
							SendMessage(fWindow, ST_RESIZE, 0, 0);
						break;
					default:
						addText(BString(cp), (HWND)l);
					}

					delete []cp;
				}
			}
			EndDialog(hWnd, 1);
			break;
		}
		return 0;
	}
	return 0;
}

LRESULT CALLBACK preferencesProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		{
			HWND ctl;

			SetDlgItemInt(hWnd, CTL_REFRESH, fPeriod, false);

			ctl = GetDlgItem(hWnd, CTL_COLOR1);
			if (ctl) SetColor(ctl, fColors[0]);

			ctl = GetDlgItem(hWnd, CTL_COLOR2);
			if (ctl) SetColor(ctl, fColors[1]);

			ctl = GetDlgItem(hWnd, CTL_COLOR3);
			if (ctl) SetColor(ctl, fColors[2]);

			// set the list of stocks
			ctl = GetDlgItem(hWnd, CTL_LIST);
			if (ctl)
			{
				BString *strings;
				int i;
				for (i = fStocks.CountItems(); i; i--)
				{
					strings = (BString *)fStocks.ItemAt(i - 1);
					if (!strings) continue;
					if (!strings[0].Length()) continue;
					SendMessage(ctl, LB_ADDSTRING, 0, (LPARAM)strings[0].String());
				}

			}
			// disable for now
			ctl = GetDlgItem(hWnd, CTL_REMOVE);
			if (ctl) EnableWindow(ctl, false);
		}
		return 0;
		break;


	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return true;
			break;

		case IDOK:
			{
				HWND ctl;

				// reset the period
				fPeriod = GetDlgItemInt(hWnd, CTL_REFRESH, NULL, false);
				if (fPeriod < 5) fPeriod = 5;

				// reset the colors
				ctl = GetDlgItem(hWnd, CTL_COLOR1);
				if (ctl) fColors[0] = GetColor(ctl);

				ctl = GetDlgItem(hWnd, CTL_COLOR2);
				if (ctl) fColors[1] = GetColor(ctl);

				ctl = GetDlgItem(hWnd, CTL_COLOR3);
				if (ctl) fColors[2] = GetColor(ctl);

				// reset the stock list
				fBenaphore->Lock();
				// step 1 - go through the old list looking for stuff that was deleted
				// step 2 go through the new list looking for stuff that was added.

				ctl = GetDlgItem(hWnd, CTL_LIST);
				if (ctl)
				{
					int i, j;
					char *cp;
					int len;
					int fCount;
					bool fChanged;
					fChanged = false;
					fCount = fStocks.CountItems();

					BString *strings;
					i = SendMessage(ctl, LB_GETCOUNT, 0, 0);
					if (i > 0) while (i)
					{
						len = SendMessage(ctl, LB_GETTEXTLEN, (WPARAM)i - 1, 0);
						if (len > 0)
						{
							cp = new char[len+1];
							SendMessage(ctl, LB_GETTEXT, (WPARAM)i - 1, (LPARAM)cp);
							for (j = 0; j < fStocks.CountItems(); j++)
							{
								strings = (BString *)fStocks.ItemAt(j);
								if (!strings || !strings[0].Length()) continue;
								if (!strings[0].ICompare(cp)) break;
							}
							// if we're at the end, just add it.
							if (j == fStocks.CountItems())
							{
								strings = new BString[6];
								strings[0] = cp;
								strings[0].ToUpper();
								fStocks.AddItem(strings);
								fChanged = true;
							}
							delete []cp;
						}
						i--;
					}
					i = 0;
					while (i < fStocks.CountItems())
					{
						strings = (BString *)fStocks.ItemAt(i);
						// the following 2 cases should never happen...
						if (!strings)
						{
							fStocks.RemoveItem(i);
							fChanged = true;
							continue;
						}
						if (!strings[0].Length())
						{
							fStocks.RemoveItem(i);
							delete []strings;
							fChanged = true;
							continue;
						}
						j = SendMessage(ctl, LB_FINDSTRINGEXACT, -1, 
							(LPARAM)strings[0].String());
						if (j == LB_ERR)	// not found!
						{
							fStocks.RemoveItem(i);
							delete []strings;
							fChanged = true;
							continue;
						}
						// ok
						i++;
					}
					
					if (fCount != fStocks.CountItems())
					{
						fStocks.SortItems(listSort);
						SendMessage(fWindow, ST_RESIZE, 0, 0);
					}
					else if (fChanged)
					{
						fStocks.SortItems(listSort);
						InvalidateRect(fWindow, NULL, false);
					}
				}
				fBenaphore->Unlock();

				EndDialog(hWnd, true);
			}
			return true;
			break;

		case CTL_REMOVE:
			{
				int index;
				HWND ctl;
				ctl = GetDlgItem(hWnd, CTL_LIST);
				index = SendMessage(ctl, LB_GETCURSEL, 0, 0);
				SendMessage(ctl, LB_DELETESTRING, (WPARAM)index, 0);
				index = SendMessage(ctl, LB_GETCURSEL, 0, 0);
				if (index == LB_ERR)
				{
					ctl = GetDlgItem(hWnd, CTL_REMOVE);
					EnableWindow(ctl, false);
				}
			}
			return true;
			break;
		case CTL_ADD:
			{
				DialogBoxParam(
					(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
					MAKEINTRESOURCE(DLG_ADD),
					hWnd, (DLGPROC)addProc, (long)GetDlgItem(hWnd, CTL_LIST));
			}
			return 0;
			break;


		case CTL_LIST:
			{
				HWND ctl;
				int index;
				ctl = GetDlgItem(hWnd, CTL_LIST);
				index = SendMessage(ctl, LB_GETCURSEL, 0, 0);
				ctl = GetDlgItem(hWnd, CTL_REMOVE);
				EnableWindow(ctl, index != LB_ERR);
			}
			return 0;
			break;
		}
		break;
	}
	return 0;
}

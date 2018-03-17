/*
 *
 * pull stock quotes from finance.nasdaq.com
 *
 *
 */

#include "StockTicker.h"
#include "DataIO.h"
#include "SocketIO.h"
#include "String.h"
#include <stdio.h>
#include "Benaphore.h"

bool query_yahoo(void)
{
	SocketIO s;
	if (s.SetTo("finance.yahoo.com", 80) == B_OK)
	{
		BString request;
		int32 err;
		char buffer[1024];
		int i,j;

		// n = symbol, l1 = last price, c1 = change, p2 = % change, v = volume
		request += "GET /d?f=sl1c1p2v";

		fBenaphore->Lock();
		
		for (i = 0; i < fStocks.CountItems(); i++)
		{
			BString * str = (BString *)fStocks.ItemAt(i);
			if (!str || !str->Length()) continue;
			request += "&s=";
			request += *str;
		}

		fBenaphore->Unlock();

		request += " HTTP/1.1\r\n";
		request += "Host: finance.yahoo.com\r\n";
		request += "Connection: close\r\n\r\n";
		
		err = s.Write(request.String(), request.Length());
		if (err < 0) return false;

		request.Truncate(0);


		// placed here so since it's free (we're waiting on tcp io)

		BString array[5];

		// wait for data...
		while ((err = s.Read(buffer, 1024)) == 0) ;
		if (err < 0) return false;
		while (true)
		{
			if (err > 0) request.Append(buffer, err);
			err = s.Read(buffer, 1024);
			if (err < 1) break;
		}
		s.Unset();

		if (sscanf(request.String(), "HTTP/1.1 %d", &err) != 1) return false;
		if (err != 200) return false;
		
		i = request.FindFirst("\r\n\r\n");
		if (i < 0) return false;
		i += 4;
		// first line is # of bytes
		j = request.FindFirst("\r\n", i);
		if (j < 0) return false;
		i = j + 2;

		// now is comma-separated values, possibly quoted.
		
		while (true)
		{
			int cnt, end;

			for (j = 0; j < 5; j++)
				array[j].Truncate(0);

			// xxx,xxx,xxx,xxx,xxx
			for (cnt = 0; cnt < 4; cnt++)
			{
				end = request.FindFirst(',', i);
				if (end > 0)
				{
					if (request[i] == '+') i++;
					request.CopyInto(array[cnt], i, end - i);
					i = end + 1;
					array[cnt].RemoveSet("\"");
				}
				else return false;
			}
			// final one, delimited by end-of string or \r or \n
			for (end = i; end < request.Length(); end++)
			{
				if (request[end] == '\r' || request[end] == '\n')
					break;
			}

			// s/b end - i - 1 ???
			request.CopyInto(array[cnt], i, end - i);
			array[cnt].RemoveSet("\"");

			fBenaphore->Lock();

			for (j = 0; j <fStocks.CountItems(); j++)
			{
				BString * str;
				str = (BString *)fStocks.ItemAt(j);
				if (!str || !str->Length()) continue;
				// could optimizse since we should know the order,
				// and should be alphabetized....
				if (BString::ICompare(str, array)) continue;
				for (int x = 0; x < 5; x++)
				{
					str[x].Adopt(array[x]);
				}
				break;
			}
			fBenaphore->Unlock();

			// skip to next line, if appropriate.
			i = end;
			while (i < request.Length() && (request[i] == '\r' || request[i] == '\n')) 
				i++;
				
			if (i == request.Length()) break;
		}

	}
	else return false;
}
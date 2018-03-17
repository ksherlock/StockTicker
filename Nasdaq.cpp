#include "StockTicker.h"
#include "Benaphore.h"
#include "SocketIO.h"
#include "List.h"
#include "String.h"
#include "XmlParser.h"
#include <stdio.h>

bool do_xml(XmlParser &, BString &);
bool do_xml(XmlParser &);
int regmatch(char *pattern, char *text);

bool query_nasdaq(void)
{
SocketIO s;
char buffer[1024];
BString request;
int32 err;
BMallocIO mio;
BString *strings;

	if (s.SetTo("quotes.nasdaq.com", 80) == B_OK)
	{
		mio.SetSize(0);
		request.Truncate(0);
		request += "GET /quote.dll?page=xml&mode=stock";
		
		fBenaphore->Lock();
		for (int i = fStocks.CountItems(); i; i--)
		{
			strings = (BString *)fStocks.ItemAt(i - 1);
			if (!strings || !strings[0].Length()) continue;

			request += "&symbol=";
			request += strings[0];
		}
		fBenaphore->Unlock();

		request += " HTTP/1.1\r\n";
		request += "Host: quotes.nasdaq.com\r\n";
		request += "Connection: close\r\n\r\n";

		err = s.Write(request.String(), request.Length());
		while ((err = s.Read(buffer, 1024)) == 0) ;
		if (err < 0) return false;
		while (true)
		{
			if (err > 0) mio.Write(buffer, err);
			err = s.Read(buffer, 1024);
			if (err < 1) return false;
		}
		s.Unset();

		if (mio.BufferLength())
		{
			const char *cp;
			int i;
			int status;

			mio.Seek(0, SEEK_SET);
			// HTTP/#.# ###
			cp = (const char *)mio.Buffer();

			// may have an op[tional text respsonce after the number
			if (sscanf(cp, "HTTP/1.1 %d", &status) != 1) return false;
			if (status != 200) return false;
				// find the start of the http data
			for (i = 0; i < mio.BufferLength(); i++)
			{
				if (cp[i] == '\r' && !strncmp(cp + i, "\r\n\r\n", 4)) break;
			}
			mio.Seek(i + 4, SEEK_SET);
			XmlParser xml(mio);
			do_xml(xml);
			return true;
		}
		else return false;
	}
	
	return false;
}
#include "CsvParser.h"

#include <ctype.h>

CsvParser::CsvParser(BDataIO& file):
	io(file)
{
	pointer = 0;
	length = 0;
	fPushBack = 0;
	lastError = B_OK;
}

void CsvParser::PushBack(int c)
{
	fPushBack = fPushBack << 8 | (c & 0xff);
}

// get the next char, reading from the file if necessary.
int CsvParser::NextChar(void)
{
	if (fPushBack)
	{
		int c = fPushBack & 0xff;
		fPushBack = fPushBack >> 8;
		return c;
	}

	if (lastError) return lastError;

	if (pointer < length)
		return buffer[pointer++];

	pointer = 0;
	do
	{
		length = io.Read(buffer, 256);
	}
	while (length == 0);

	if (length < 0)
	{
		lastError = length;
		return length;	// error
	}

	pointer = 1;
	return buffer[0];
}

BString CsvParser::NextToken(csv_token &token)
{
	int q;
	int c;

	q = 0;

	BString result;

	//scan until we get an unquoted comma
	while (true)
	{
		c = NextChar();
		switch (c)
		{
		case B_ERROR:
		case B_EOF:
			if (result.Length())
				token = csv_data;
			else token = csv_eof;
			return result;
			break;
		case '\r':
		case '\n':
			if (result.Length())
			{
			}
			else
			{
				while 
			}
		default:
		}
		

	}


}

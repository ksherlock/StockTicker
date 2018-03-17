#ifndef __CSVPARSER_H__
#define __CSVPARSER_H__

#include "String.h"
#include "DataIO.h"
#include "SupportDefs.h"

enum csv_token
{
	csv_err = -1,
	csv_eof = 0,
	csv_data,
	csv_newline
};

class CsvParser
{
public:
	CsvParser(BDateIO &);
	BString NextToken(csv_token &token);

private:

	int NextChar(void);
	void PushBack(char c);
	void PushToken(csv_token);

	BDataIO &io;
	int32 fPushBack;
	int length;
	int pointer;
	int lastError;
	char buffer[256];
};

#endif
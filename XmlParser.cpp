/*
 * Copyright 2002 Kelvin W Sherlock
 *
 *
 * $Id: //depot/WinShisen/XmlParser.cpp#4 $
 */


#include "XmlParser.h"
#include <ctype.h>
#include "String.h"

XmlParser::XmlParser(BDataIO& file):
	io(file)
{
	pointer = 0;
	length = 0;
	fPushBack = 0;
}

//unget a character
// supports up to 4 characters (2 if unicode)
void XmlParser::PushBack(int c)
{
	fPushBack = fPushBack << 8 | (c & 0xff);
}

// get the next char, reading from the file if necessary.
int XmlParser::NextChar(void)
{
	if (fPushBack)
	{
		int c = fPushBack & 0xff;
		fPushBack = fPushBack >> 8;
		return c;
	}

	if (pointer < length)
		return buffer[pointer++];

	pointer = 0;
	do
	{
		length = io.Read(buffer, 1024);
	}
	while (length == 0);

	if (length < 0) return length;	// error

	pointer = 1;
	return buffer[0];
}



BString XmlParser::NextToken(xml_token &token)
{
int c;
BString result;

	// 1 remove any leading space
	do
	{
		c = NextChar();
		if (c < 0)
		{
			token = xml_eof;
			return "";
		}
	}
	while (isspace(c));

	result.SetTo(c, 1);

	if (c != '<')
	{
		token = Data(result);
		return result;
	}

	// check for <! or <?
	c = NextChar();

	if (c < 0)
	{
		token = xml_err;
		return result;
	}
	result.Append(c, 1);

	if (c == '?')
	{
		token = Directive(result);
	}

	else if (c == '!')
	{
		token = Bang(result);
	}

	else token = Tag(result);

	return result;

}

/*
 * data.  Parse until we get a '<' or eof
 *
 */
xml_token XmlParser::Data(BString& result)
{
int c;
int l;

	while (true)
	{
		c = NextChar();
		if (c < 0)
		{
			break;
		}
		if (c == '<')
		{
			PushBack(c);	// save for next time.
			break;
		}
		result.Append(c, 1);
	}

	// remove any trailing  whitespace???
	l = result.Length();
	while (isspace(result[l-1])) l--;
	if (l != result.Length())
	{
		result.Truncate(l);
	}
	return xml_data;
}

/*
 * <[^>] >
 *
 */
xml_token XmlParser::Tag(BString& result)
{
int c;
int quote;

	quote = 0;

	while (true)
	{
		c = NextChar();
		if (c < 0) return xml_err;

		if (c == '"' && quote != 1)		// set to 2/0
		{
			quote ^= 0x02;
		}
		
		if (c == '\'' && quote != 2)	// set to 1/0
		{
			quote ^= 0x01;
		}

		result.Append(c, 1);
		if (c == '>' && quote == 0) break;
	}

	return xml_tag;
}

/*
 * <? [^>] ?>
 *
 */
xml_token XmlParser::Directive(BString& result)
{
int c;
int c_old;
int quote;

	c_old = 0;
	quote = 0;

	while (true)
	{
		c = NextChar();
		if (c < 0)
			return xml_err;	// not eof;
		
		if (c == '"' && quote != 1)		// set to 2/0
		{
			quote ^= 0x02;
		}

		if (c == '\'' && quote != 2)	// set to 1/0
		{
			quote ^= 0x01;
		}

		result.Append(c, 1);

		if (quote == 0 && c == '>' && c_old == '?') break;
		c_old = c;
	}	

	return xml_directive;
}

// may be a comment, CDATA, or something else
xml_token XmlParser::Bang(BString& result)
{
int count;
int c;
int quote;

	count = 2;	// already has <!
	quote = 0;

	while (true)
	{
		c = NextChar();
		if (c < 0) return xml_err;

		result.Append(c, 1);
		count++;
		if (count == 4 && result == "<!--") return Comment(result);
		if (count == 9 && result == "<![CDATA[") return CData(result);

		if (c == '"' && quote != 1)		// set to 2/0
		{
			quote ^= 0x02;
		}

		if (c == '\'' && quote != 2)	// set to 1/0
		{
			quote ^= 0x01;
		}	
		
		if (quote == 0 && c == '>') break;
	}
	return xml_declaration;	//???
}

/*
 * <![CDATA[ * ]]>
 */
xml_token XmlParser::CData(BString& result)
{
int c;
int ccnt;

	ccnt = 0;

	while (true)
	{
		c = NextChar();
		if (c < 0) return xml_err;

		result.Append(c, 1);

		if (c == '>' && ccnt > 1) break;

		if (c == ']') ccnt++;
		else ccnt = 0;
	}
	return xml_cdata;
}

/*
 * <!-- * -->
 *
 * per xml 1.0 spec, ---> does not end the tag.
 */
xml_token XmlParser::Comment(BString& result)
{
int c;
int ccnt;

	ccnt = 0;

	while (true)
	{
		c = NextChar();
		if (c < 0) return xml_err;

		result.Append(c, 1);

		if (c == '>' && ccnt == 2) break;
		if (c == '-') ccnt++;
		else ccnt = 0;
	}
	return xml_comment;
}
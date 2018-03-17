#ifndef __XMLPARSER_H__
#define __XMLPARSER_H__


#include "String.h"
#include "DataIO.h"
#include "SupportDefs.h"

enum xml_token
{
	xml_err = -1,
	xml_eof = 0,
	xml_data,		// text between tags
	xml_tag,		// <tag>
	xml_directive,	// '<?' * '?>'
	xml_declaration,	// <! * >
	xml_cdata,		// '<![CDATA[' * ']]>'
	xml_comment,	// '<!--' * '-->'
};

class XmlParser
{
public:
	XmlParser(BDataIO& file);


	BString NextToken(xml_token &token);

private:
	xml_token	Data(BString&);
	xml_token	CData(BString&);
	xml_token	Directive(BString&);
	xml_token	Comment(BString&);
	xml_token	Tag(BString&);
	xml_token	Bang(BString&);

	int		NextChar(void);
	void	PushBack(int c);

	
	
	char buffer[1024];
	int length;
	int pointer;
	BDataIO &io;
	int32 fPushBack;
};


#endif
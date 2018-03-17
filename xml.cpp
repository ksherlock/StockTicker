#include "XmlParser.h"
#include "String.h"
#include "List.h"
#include "Benaphore.h"


extern Benaphore *fBenaphore;
extern BList fStocks;

static char *nasdaqTags[] =
{
	"last-sale-price",
	"net-change-price",
	"net-change-pct",
	"share-volume-qty",
//	"previous-close-price",
};
static BString variables[6];

bool do_xml(XmlParser &, BString &);
bool do_xml(XmlParser& xml)
{
BString data;
xml_token token;

	// get the first start token
	while (true)
	{
		data = xml.NextToken(token);
		if (token == xml_eof) return true;
		if (token == xml_err) return false;
		if (token == xml_tag)
		{
			if (!do_xml(xml, data)) return false;
		}
	}

}

bool do_xml(XmlParser& xml, BString& start)
{
xml_token token;
BString string;
BString data;


	if (start[start.Length() - 2] == '/') return true; // self-contained

/*--- initialize variables for the start block ---*/
	if (!start.Compare("<equity-quote ", 14))
	{
		int i;
		for (i = 0; i < sizeof(variables) / sizeof(BString) ; i++)
		{
			variables[i].Truncate(0);
		}
		i = start.FindFirst("symbol=\"");
		if (i > 0)
		{
			char c;
			for (i = i + 8; i < start.Length() ; i++)
			{
				c = start.ByteAt(i);
				if (c == '"') break;
				variables[0].Append(c, 1);
			}
			variables[0].ToUpper();
		}
	}
/*---	---*/
	
	while (true)
	{
		string = xml.NextToken(token);
		if (token == xml_err) 
			return false;
		if (token == xml_eof) 
			return false;
		if (token == xml_tag)
		{

			// check if it matches the start or is a new tag
			if (string[1] == '/')
			{	
				char c, d;
				int i;
				for (i = 1; ; i++)
				{
					c = start.ByteAt(i);
					d = string.ByteAt(i+1);

					if (c == 0) return false; // shouldn't happen
					if (c == ' ') c = '>';
					if (d == ' ') d = '>';
					if (c == '>' && d == '>')
					{
						// check for </equity-quote
						if (!string.Compare("</equity-quote", 14))
						{
							BString *strings;

							fBenaphore->Lock();

							for (i = fStocks.CountItems(); i ; i--)
							{
								strings = (BString *)fStocks.ItemAt(i - 1);
								if (!strings || !strings[0].Length()) continue;
								if (strings[0] != variables[0]) continue;

								for (i = 1; i < 6; i++)
									strings[i].Adopt(variables[i]);
								break;
							}

							fBenaphore->Unlock();
							return true;
						}
						int j;
						for (j = 0; j < sizeof(nasdaqTags) / sizeof(char *); j++)
						{
							if (i != strlen(nasdaqTags[j]) + 1) continue;
							if (strncmp(start.String() + 1, nasdaqTags[j], i - 1)) continue;

							variables[j+1].Adopt(data);
							break;
						}
						return true;
					}
					if (c != d) 
						return false;
				}
			}
			else
			{
				if (!do_xml(xml, string)) return false;
			}
		}
		if (token == xml_data)
		{
			data.Adopt(string);
		}
	}
}
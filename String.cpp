/*
 * partial BString implementation
 *
 * Kelvin W Sherlock
 *
 * The WinShisen project
 *
 * $Id: //depot/WinShisen/String.cpp#11 $
 */

#include "String.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "SupportDefs.h"

const uint32 alloc_size = 256;

template <class T>
inline T _min_(T d1, T d2)
{
	return (d1 < d2) ? d1 : d2;
}
template <class T>
inline T _max_(T d1, T d2)
{
	return (d1 > d2) ? d1 : d2;
}

BString::BString(void)
{
	alloc = alloc_size;
	data = new char[alloc];
	len = 0;
	data[len] = 0;
}

BString::BString(const BString &string)
{
	data = NULL;
	len = 0;
	alloc = 0;

	SetTo(string);
}
BString::BString(const char *string)
{
	data = NULL;
	alloc = 0;
	SetTo(string);
}
BString::BString(const char *string, int32 maxlen)
{
	data = NULL;
	alloc = 0;
	SetTo(string, maxlen);
}
BString::~BString()
{
	if (data) delete[] data;
}

BString& BString::Append(char c, int32 count)
{
	if (count > 0 && c != 0)
	{
		resize (len + count + 1, true);
		for (register int32 i = 0; i < count; i++)
		{
			data[len + i] = c;
		}
		len += count;
		data[len] = 0;
	}
	return *this;
}

BString& BString::Append(const BString& string)
{
	if (string.len > 0)
	{
		resize(len + string.len + 1, true);
		memcpy(data + len, string.data, string.len + 1);
		len += string.len;
	}
	return *this;
}



BString& BString::Append(const char *string, int32 length)
{
	if (length > 0 && string && *string)
	{
		length = _min_(length, (int32)strlen(string));
		resize(len + length + 1, true);
		memcpy(data + len, string, length);
		len += length;
		data[len] = 0;
	}
	return *this;
}

BString& BString::Append(const char *string)
{
int32 length;
	if (string && *string)
	{
		length = strlen(string);
		resize(len + length + 1, true);
		memcpy(data + len, string, length + 1);
		len += length;
	}

	return *this;
}

BString& BString::Append(const BString& string, int32 length)
{
	if (length > string.len) length = string.len;
	if (length > 0)
	{
		resize(len + length + 1, true);
		memcpy(data + len, string.data, length);
		len += length;
		data[len] = 0;
	}
	return *this;
}

//
// Prepend
//

BString& BString::Prepend(const BString& source)
{
	if (source.len > 0)
	{
		BString temp(source);
		temp.Append(*this);
		Adopt(temp);
	}
	return *this;
}

BString& BString::Prepend(const BString& source, int32 charCount)
{
	if (source.len > 0 && charCount > 0)
	{
		BString temp;
		temp.SetTo(source, charCount);
		temp.Append(*this);
		Adopt(temp);
	}
	return *this;
}

BString& BString::Prepend(const char * source)
{
	if (source && *source)
	{
		BString temp(source);
		temp.Append(*this);
		Adopt(temp);
	}
	return *this;
}

BString& BString::Prepend(const char * source, int32 charCount)
{
	if (source && *source && charCount > 0)
	{
		BString temp(source, charCount);
		temp.Append(*this);
		Adopt(temp);
	}
	return *this;
}


BString& BString::Prepend(char c, int32 charCount)
{
	if (charCount > 0)
	{
		BString temp;
		temp.SetTo(c, charCount);
		temp.Append(*this);
		Adopt(temp);
	}
	return *this;
}



BString& BString::operator << (const BString& string)
{
	return Append(string);
}
BString& BString::operator << (const char *string)
{
	return Append(string);
}
BString& BString::operator << (const char c)
{
	return Append(c, 1);
}

BString& BString::operator << (const int32 val)
{
char buffer[12]; // 0xffffffff = 4294967295
	ltoa(val, buffer, 10);
	return this->operator += (buffer);
}

BString& BString::operator << (const uint32 val)
{
char buffer[12];
	ultoa(val, buffer, 10);
	return this->operator += (buffer);
}

#if 0
BString& BString::operator << ( const float val)
{
uint32 bits;
int32 size;
bool	fSign;
int32	fExp;
int32	fFraction;
	
	/*
	 * IEEE floating point format:
	 * bit 0:  sign flag (0 = positive, 1 = negative
	 * bit 1-8: exponent bits
	 * bit 9-31: fraction bits
	 */

	//	  S EEEEEEEE FFF FFFF FFFF FFFF FFFF FFFF
	//    0 1      8 9                         31

	bits = *((uint32 *)&val));
	
	fFraction = bits & 0x007fffff;
	fExp = (bits >> 23) & 0xff;
	fSign = (bits & 0x80000000) == 0 : false : true;


	// special cases...
	
	if (fExp == 255)
	{
		if (fFraction) return Append("NaN", 3);
		if (fSign) return Append("-Infinity");
		return Append("Infinity");
	}

	if (fExp == 0)
	{
		if (fFraction == 0)
		{
			if (fSign) return Append("-0.00");
			return Append("0.00");
		}
	}


	// value = -1 ** fSign * 2 ** (fExp - 127) * (1.fFraction)
	// we only care about 2 significant digits (3 to handle rounding), though.



}
#endif

void BString::resize(int32 newsize, bool copy)
{
char *temp;

	if (alloc < newsize)
	{
		alloc = (newsize + alloc_size) & ~(alloc_size - 1);
		temp = new char[alloc];
		if (copy && data) memcpy(temp, data, len + 1);
		if (data) delete[] data;
		data = temp;
	}
	else if (alloc > newsize) // trim data size
	{
		int32 a = (newsize + alloc_size) & ~(alloc_size - 1);
		if (a != alloc)
		{
			alloc = a;
			temp = new char[alloc];
			if (copy && data) memcpy(temp, data, len + 1);
			if (data) delete[] data;
			data = temp;
		}
	}
}

BString& BString::SetTo(char c, int32 charCount)
{
	if (c == 0) charCount = 0;
	resize(charCount + 1, false);
	len = charCount;
	for (register int32 i = 0; i < charCount; i++)
		data[i] = c;
	data[charCount] = 0;
	return *this;
}

BString& BString::SetTo(const char *source)
{
	len = source ? strlen(source) : 0;
 	resize(len + 1, false);
	if (source && len) memcpy(data, source, len);
	data[len] = 0;

	return *this;
}

BString& BString::SetTo(const char *source, int32 charCount)
{
register int32 i;

	if (!source || !*source) return SetTo(NULL);

	for (i = 0; i < charCount; i++)
	{
		if (source[i] == 0) break;
	}

	charCount = _min_(i, charCount);

	resize(charCount + 1, false);
	for (i = 0; i < charCount; i++)
		data[i] = source[i];
	data[charCount] = 0;
	len = charCount;

	return *this;
}

BString& BString::SetTo(const BString& source)
{
	resize(source.len + 1, false);
	memcpy(data, source.data, source.len + 1);
	len = source.len;

	return *this;
}

BString& BString::SetTo(const BString& source, int32 charCount)
{
	if (charCount > source.len) return SetTo(source);
	
	resize(charCount + 1, false);
	for (register int32 i = 0; i < charCount; i++)
		data[i] = source.data[i];
	data[charCount] = 0;
	len = charCount;

	return *this;
}



/*
 * truncates the string to newLength
 * if not lazy, frees up excess memory
 */
BString& BString::Truncate(int32 newLength, bool lazy)
{
	if (newLength >= 0 && newLength < len)
	{
		len = newLength;
		data[len] = 0;

		if (!lazy) // free up unused memory
		{
			resize(len + 1, true);
		}
	}
	return *this;
}


/*
 * adopt
 *
 * steals from the from string's data
 */
BString& BString::Adopt(BString& string)
{
	if (data) delete[] data;
	data = string.data;
	len = string.len;
	alloc = string.alloc;

	string.data = NULL;
	string.len = 0;
	string.alloc = 0;

	return *this;
}

BString& BString::Adopt(BString& string, int32 count)
{
	if (count < 0) count = 0;
	if (count > string.len) count = string.len;

	if (data) delete[] data;

	data = string.data;
	data[count] = 0;	// won't overrun buffer
	len = count;
	alloc = string.alloc;

	string.data = NULL;
	string.len = 0;
	string.alloc = 0;

	return *this;
}

BString& BString::ToLower(void)
{
	for (register int32 i = 0; i < len; i++)
	{
		if (isupper(data[i])) data[i] = tolower(data[i]);
	}
	return *this;
}

BString& BString::ToUpper(void)
{
	for (register int32 i = 0; i < len; i++)
	{
		if (islower(data[i])) data[i] = toupper(data[i]);
	}
	return *this;
}

BString& BString::Capitalize(void)
{
	if (len && islower(data[0])) data[0] = toupper(data[0]);
	for (register int32 i = 1; i < len; i++)
	{
		if (isupper(data[i])) data[i] = tolower(data[i]);
	}
	return *this;
}

/*
 * capitalizes each word
 * a word = group of 1+ alphabetic characters delimited by 
 * 1+ non-alphabetic characters 
 */
BString& BString::CapitalizeEachWord(void)
{
	bool cap;
	char c;

	cap = true;	// first one will be a word
	for (int32 i = 0; i < len; i++)
	{
		c = data[i];
		if (isalpha(c))
		{
			if (cap && islower(c))
			{
				data[i] = toupper(c);
			}
			else if (!cap && isupper(c))
			{
				data[i] = tolower(c);
			}
			cap = false;
		}
		else cap = true;	// non-alpha - capitalize the next alphabetic char
	}
	return *this;
}

BString& BString::CharacterEscape(const char *set, char escapeWith)
{
int32 elen;
BString string;
char c;

	elen = set ? strlen(set) : 0;

	if (!elen || !escapeWith) return *this;

	for (int32 i = 0; i < len; i++)
	{
		c = data[i];
		if (c == escapeWith)
		{
			string.Append(escapeWith, 2);
			continue;
		}
		for (int32 j = 0; j < elen; j++)
		{
			if (c == set[j])
			{
				string.Append(escapeWith, 1);
				break;
			}
		}
		string.Append(c, 1);
	}

	Adopt(string);

	return *this;
}

BString& BString::CharacterEscape(const char *original, const char *set, char escapeWith)
{
char c;
int32 elen;
int32 olen;

	olen = original ? strlen(original) : 0;
	elen = set ? strlen(set) : 0;

	if (!elen || !olen || !escapeWith)
	{
		SetTo(original);
		return *this;
	}

	SetTo(NULL);

	for (int32 i = 0; i < elen; i++)
	{
		c = original[i];
		if (c == escapeWith)
		{
			Append(escapeWith, 2);
			continue;
		}
		for (register int32 j = 0; j < elen; j++)
		{
			if (c == set[j])
			{
				Append(escapeWith, 1);
				break;
			}
		}
		Append(c, 1);
	}

	return *this;
}


BString& BString::CharacterDeescape(char escapeChar)
{
BString string;
char c;
bool esc;

	esc = false; // for escaping the escape character
	for (int32 i = 0; i < len; i++)
	{
		c = data[i];
		if (c == escapeChar)
		{
			if (esc) string.Append(c,1);
			esc = (!esc);
		}
		else string.Append(c,1);
	}
	Adopt(string);

	return *this;
}

BString& BString::CharacterDeescape(const char *original, char escapeChar)
{
int olen;
char c;
bool esc;

	SetTo(NULL);
	olen = original ? strlen(original) : 0;

	esc = false;
	for (int32 i = 0; i < olen; i++)
	{
		c = original[i];
		if (c == escapeChar)
		{
			if (esc) Append(c, 1);
			esc = (!esc);
		}
		else Append(c, 1);
	}

	return *this;
}

/*
 * escape/deescape for xml
 * % <-> %25
 * " <-> %22
 * ' <-> %2c
 */

const char  hextable[] = "0123456789abcdef";

BString& BString::XmlEscape(void)
{
bool change;
BString temp;
register int32 i;
char c;
bool escape;

	change = false;
	escape = false;
	for (i = 0; i < len; i++)
	{
		c = data[i];
		if (c & 0x80) escape = true;
		if (iscntrl(c)) escape = true;
		if (!isprint(c)) escape = true;
		switch (c)
		{
		case '%':
		case '"':
		case '\'':
			escape = true;
		}
		if (escape)
		{
			escape = false;
			change = true;
			temp.Append('%', 1);
			temp.Append(hextable[(c >> 4) & 0x0f],1);
			temp.Append(hextable[c & 0x0f], 1);
		}
		else
			temp.Append(c, 1);

	}

	if (change) Adopt(temp);
	return *this;
}

BString& BString::XmlDeescape(void)
{
bool change;
BString temp;
register int32 i;
char c;
	i = 0;
	while (i < len)
	{
		c = data[i++];
		if (c == '%')
		{
			// format is %[0-9a-fA-F]{2}
			if (i + 1 < len && isxdigit(data[i]) && isxdigit(data[i+1]))
			{
				char x[3];
				x[0] = data[i++];
				x[1] = data[i++];
				x[2] = 0;
				c = (char)strtol(x, NULL, 16);
				change = true;
			}
		}
		temp.Append(c, 1);
	}
	if (change) Adopt(temp);
	return *this;
}


// Find First routines
// return B_ERROR if can't be found

int32 BString::FindFirst(const char *string) const
{
char *cp;

	if (!string) return B_ERROR;

	cp = strstr(data, string);
	if (cp == NULL) return B_ERROR;
	return (int32)(cp - data);

}

int32 BString::FindFirst(const char *string, int32 offset) const
{
char *cp;

	if (offset > len) return B_ERROR;
	if (!string) return B_ERROR;

	cp = strstr(data + offset, string);
	if (cp == NULL) return B_ERROR;

	return (int32)(cp - data);
}

int32 BString::FindFirst(char c) const
{
	for (register int32 i = 0; i < len; i++)
	{
		if (data[i] == c) return i;
	}
	return B_ERROR;
}

int32 BString::FindFirst(char c, int32 offset) const
{
	for (register int32 i = offset; i < len; i++)
	{
		if (data[i] == c) return i;
	}
	return B_ERROR;
}


int32 BString::IFindFirst(char c) const
{
	c = toupper(c);
	for (register int32 i = 0; i < len; i++)
	{
		if (toupper(data[i]) == c) return i;
	}
	return B_ERROR;
}

int32 BString::IFindFirst(char c, int32 offset) const
{
	c = toupper(c);
	for (register int32 i = offset; i < len; i++)
	{
		if (toupper(data[i]) == c) return i;
	}
	return B_ERROR;
}

// offset in FindLast = only look before that point.
int32 BString::FindLast(char c) const
{
	for (register int32 i = len; i; i--)
	{
		if (data[i - 1] == c) return i;
	}
	return B_ERROR;
}

int32 BString::FindLast(char c, int32 offset) const
{
	for (register int32 i = offset; i; i--)
	{
		if (data[i - 1] == c) return i;
	}
	return B_ERROR;
}

int32 BString::IFindLast(char c) const
{
	c = toupper(c);
	for (register int32 i = len; i; i--)
	{
		if (toupper(data[i - 1]) == c) return i;
	}
	return B_ERROR;
}

int32 BString::IFindLast(char c, int32 offset) const
{
	c = toupper(c);
	for (register int32 i = offset; i; i--)
	{
		if (toupper(data[i - 1]) == c) return i;
	}
	return B_ERROR;
}

int BString::Compare(const BString& string) const
{
	if (this == &string) return 0;
	return strcmp(data, string.data);
}

int BString::Compare(const BString& string, int32 range) const
{
	if (this == &string) return 0;
	return strncmp(data, string.data, range);
}

int BString::Compare(const char *string) const
{
	return strcmp(data, string);
}

int BString::Compare(const char *string, int32 range) const
{
	return strncmp(data, string, range);
}
//global functions
int BString::Compare(const BString& astring, const BString& bstring)
{
	if (&astring == &bstring) return 0;
	return strcmp(astring.data, bstring.data);
}

int BString::Compare(const BString *astring, const BString *bstring)
{
	if (astring == bstring) return 0;
	return strcmp(astring->data, bstring->data);
}


int BString::ICompare(const BString& string) const
{
	if (this == &string) return 0;
	return stricmp(data, string.data);
}

int BString::ICompare(const BString& string, int32 range) const
{
	if (this == &string) return 0;
	return strnicmp(data, string.data, range);
}

int BString::ICompare(const char *string) const
{
	return stricmp(data, string);
}

int BString::ICompare(const char *string, int32 range) const
{
	return strnicmp(data, string, range);
}
// global funcions
int BString::ICompare(const BString& astring, const BString& bstring)
{
	if (&astring == &bstring) return 0;
	return stricmp(astring.data, bstring.data);
}

int BString::ICompare(const BString *astring, const BString *bstring)
{
	if (astring == bstring) return 0;
	return stricmp(astring->data, bstring->data);
}

// copy a substring into destination
BString& BString::CopyInto(BString& destination, int32 sourceOffset, int32 charCount) const
{
	if (sourceOffset > len)
	{
		destination.SetTo("");
		return destination;
	}
	charCount = _min_(len - sourceOffset, charCount);
	destination.SetTo(data + sourceOffset, charCount);

	return destination;
}

void BString::CopyInto(char *destination, int32 sourceOffset, int32 charCount) const
{
	if (!destination) return;
	if (sourceOffset > len)
	{
		*destination = 0;
		return;
	}
	charCount = _min_(len - sourceOffset, charCount);
	register int i;
	for (i = 0; i < charCount; i++)
	{
		destination[i] = data[i]; 
	}
	destination[i] = 0;
}

/*
 * replace all
 */
BString& BString::ReplaceAll(char oldChar, char newChar, int32 offset)
{
register int32 i;

	for (i = offset; i < len; i++)
	{
		if (data[i] == oldChar) data[i] = newChar;
	}
	return *this;
}

BString& BString::ReplaceAll(const char *oldString, const char *newString, int32 offset)
{
register int32 i;

int32 oldLen;
int32 newLen;
bool changed;

BString temp;

	if (!oldString || !*oldString) return *this;

	// make sure 1st character exists.
	i = FindFirst(*oldString, offset);
	if (i < 0) return *this;

	oldLen = strlen(oldString);
	newLen = newString ? strlen(newString) : 0;
	changed = false;

	if (i) temp.Append(*this, i);
	
	while (i <len)
	{
		if (!strncmp(oldString, &data[i], oldLen))
		{
			temp.Append(newString);
			i += oldLen;
			changed = true;
		}
		else
			temp.Append(data[i++], 1);
	}
	if (changed) Adopt(temp);
	return *this;
}


BString& BString::IReplaceAll(char oldChar, char newChar, int32 offset)
{
register int32 i;

	oldChar = toupper(oldChar);

	for (i = offset; i < len; i++)
	{
		if (toupper(data[i]) == oldChar) data[i] = newChar;
	}
	return *this;
}

BString& BString::IReplaceAll(const char *oldString, const char *newString, int32 offset)
{
register int32 i;

int32 oldLen;
int32 newLen;
bool changed;

BString temp;

	if (!oldString || !*oldString) return *this;

	// make sure 1st character exists.
	i = IFindFirst(*oldString, offset);
	if (i < 0) return *this;

	oldLen = strlen(oldString);
	newLen = newString ? strlen(newString) : 0;
	changed = false;

	if (i) temp.Append(*this, i);
	
	while (i <len)
	{
		if (!strnicmp(oldString, &data[i], oldLen))
		{
			temp.Append(newString);
			i += oldLen;
			changed = true;
		}
		else
			temp.Append(data[i++], 1);
	}
	if (changed) Adopt(temp);
	return *this;
}

BString& BString::RemoveSet(const char *set)
{
register int32 i;
register int32 j;
char c;
BString temp;
bool changed;


	if (!set || !*set) return *this;

	changed = false;

	for (i = 0; i < len; i++)
	{
		c = data[i];

		for (j = 0; set[j]; j++)
		{
			if (c == set[j])
			{
				changed = true;
				c = 0;	// not unicode happy
				break;
			}
		}
		if (c) temp.Append(c, 1);
	}

	if (changed) Adopt(temp);
	return *this;
}
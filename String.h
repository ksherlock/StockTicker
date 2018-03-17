#ifndef __BSTRING_H__
#define __BSTRING_H__

/*
 * BString implementation
 * (c) 2002 Kelvin W Sherloc
 * All Rights Reserved
 * $Id: //depot/WinShisen/String.h#7 $
 */


#include <string.h>
#include "SupportDefs.h"

class BString
{
public:
	BString(void);
	BString(const char *string);
	BString(const char *string, int32 maxLen); 
	BString(const BString &string);
~BString();

BString& SetTo(const char *source);
BString& SetTo(const char *source, int32 charCount);
BString& SetTo(const BString& source);
BString& SetTo(const BString& source, int32 charCount);
BString& SetTo(char c, int32 charCount);

BString& Adopt(BString& string);
BString& Adopt(BString& string, int32 count);

BString &CopyInto(BString &destination, int32 sourceOffset, int32 charCount) const;
void CopyInto(char *destination, int32 sourceOffset, int32 charCount) const;

BString &MoveInto(BString &destination, int32 sourceOffset, int32 charCount);
void MoveInto(char *destination, int32 sourceOffset, int32 charCount);



BString& Truncate(int32 newLength, bool lazy =true);

int32 FindFirst(const BString &string) const;
int32 FindFirst(const BString &string, int32 offset) const;
int32 FindFirst(const char *string) const;
int32 FindFirst(const char *string, int32 offset) const;
int32 FindFirst(char c) const;
int32 FindFirst(char c, int32 offset) const; 

int32 FindLast(const BString &string) const;
int32 FindLast(const BString &string, int32 offset) const;
int32 FindLast(const char *string) const;
int32 FindLast(const char *string, int32 offset) const;
int32 FindLast(char c) const;
int32 FindLast(char c, int32 offset) const; 

int32 IFindFirst(const BString &string) const;
int32 IFindFirst(const BString &string, int32 offset) const;
int32 IFindFirst(const char *string) const;
int32 IFindFirst(const char *string, int32 offset) const; 
int32 IFindFirst(char c) const;
int32 IFindFirst(char c, int32 offset) const;

int32 IFindLast(const BString &string) const;
int32 IFindLast(const BString &string, int32 offset) const; 
int32 IFindLast(const char *string) const;
int32 IFindLast(const char *string, int32 offset) const;  
int32 IFindLast(char c) const;
int32 IFindLast(char c, int32 offset) const;

/*
 * string comparison.  See also operator <, >, ==
 */
int Compare(const BString &string) const;
int Compare(const BString &string, int32 range) const;
int Compare(const char *string) const;
int Compare(const char *string, int32 range) const;
static int Compare(const BString &astring, const BString &bstring); 
static int Compare(const BString *astring, const BString *bstring);

int ICompare(const BString &string) const;
int ICompare(const BString &string, int32 range) const;
int ICompare(const char *string) const;
int ICompare(const char *string, int32 range) const;
static int ICompare(const BString &astring, const BString &bstring);
static int ICompare(const BString *astring, const BString *bstring);

/*
 * remove characters/strings
 */
BString &Remove(int32 startingAt, int32 charCount);
BString &RemoveFirst(const BString &string);
BString &RemoveFirst(const char *string);
BString &RemoveLast(const BString &string);
BString &RemoveLast(const char *string);
BString &RemoveAll(const BString &string);
BString &RemoveAll(const char *string);
BString &RemoveSet(const char *charSet);
BString &RemoveSet(const BString& set);
/*
 * replace characters/strings
 */
BString &Replace(const char *oldString, const char *newString, int32 count, int32 offset = 0);
BString &Replace(char oldChar, char newChar, int32 count, int32 offset = 0);
BString &ReplaceFirst(const char *oldString, const char *newString);
BString &ReplaceFirst(char oldChar, char newChar);
BString &ReplaceLast(const char *oldString, const char *newString);
BString &ReplaceLast(char oldChar, char newChar);
BString &ReplaceAll(const char *oldString, const char *newString, int32 offset = 0);
BString &ReplaceAll(char oldChar, char newChar, int32 offset = 0);
BString &ReplaceSet(const char *oldSet, const char *newSet);
BString &ReplaceSet(const char *oldSet, char newSet);
BString &IReplace(const char *oldString, const char *newString, int32 count, int32 offset = 0);
BString &IReplace(char oldChar, char newChar, int32 count, int32 offset = 0);
BString &IReplaceFirst(const char *oldString, const char *newString);
BString &IReplaceFirst(char oldChar, char newChar);
BString &IReplaceLast(const char *oldString, const char *newString);
BString &IReplaceLast(char oldChar, char newChar);
BString &IReplaceAll(const char *oldString, const char *newString, int32 offset = 0);
BString &IReplaceAll(char oldChar, char newChar, int32 offset = 0);



BString& ToLower(void);
BString& ToUpper(void);
BString& Capitalize(void);
BString& CapitalizeEachWord(void);

BString& CharacterEscape(const char *original, const char * set, char escapeWith);
BString& CharacterEscape(const char * set, char escapeWith);
BString& CharacterDeescape(const char *original, char escapeWith);
BString& CharacterDeescape(char escapeWith);

BString& XmlEscape(void);
BString& XmlDeescape(void);


inline BString& operator = (const BString &string);
inline BString& operator = (const char *string);
inline BString& operator = (const char c);

BString& operator += (const BString &string);
BString& operator += (const char *string);
BString& operator += (const char c);

BString& Append(const BString &string);
BString& Append(const char *string);
BString& Append(const BString &string, int32 length);
BString& Append(const char *string, int32 length);
BString& Append(char c, int32 count);

// Prepend
BString &Prepend(const BString &source);
BString &Prepend(const BString &source, int32 charCount);
BString &Prepend(const char *source);
BString &Prepend(const char *source, int32 charCount);
BString &Prepend(char c, int32 charCount);


BString& operator << (const BString &string);
BString& operator << (const char *string);
BString& operator << (const char c);

BString& operator << (const int32 val);
BString& operator << (const uint32 val);
BString& operator << (const float val);

inline char operator[](int32 index) const;
inline char ByteAt(int32 index) const;
inline friend bool operator == (const BString& string1, const BString& string2);
inline friend bool operator == (const char * string1, const BString& string2);
inline friend bool operator == (const BString& string1, const char * string2);

inline friend bool operator != (const BString& string1, const BString& string2);
inline friend bool operator != (const char * string1, const BString& string2);
inline friend bool operator != (const BString& string1, const char * string2);

inline friend bool operator < (const BString& string1, const BString& string2);
inline friend bool operator < (const char * string1, const BString& string2);
inline friend bool operator < (const BString& string1, const char * string2);

inline friend bool operator > (const BString& string1, const BString& string2);
inline friend bool operator > (const char * string1, const BString& string2);
inline friend bool operator > (const BString& string1, const char * string2);

inline int32 Length(void) const;
inline const char * String(void) const;

private:
void	resize(int32 newsize, bool copy);
char *data;
int32 len;
int32 alloc;
};

inline const char * BString::String(void) const
{
	return data ? data : "";
}
inline int32 BString::Length(void) const
{
	return len;
}

inline char BString::operator [](int32 index) const
{
	return data[index];
}

inline char BString::ByteAt(int32 index) const
{
	if (!data || index <0 || index > len) return 0;
	return data[index];
}


// operator +=
// front end for append 
inline BString& BString::operator += (const BString &string)
{
	return Append(string);
}
inline BString& BString::operator += (const char *string)
{
	return Append(string);
}
inline BString& BString::operator += (const char c)
{
	return Append(c, 1);
}


// operator = 

inline BString& BString::operator = (const BString &string)
{
	return SetTo(string);
}
inline BString& BString::operator = (const char *string)
{
	return SetTo(string);
}
inline BString& BString::operator = (const char c)
{
	return SetTo(c, 1);
}


// ==
inline bool operator == (const BString & string1, const BString &string2)
{
	if (string1.data == string2.data) return true; // self == self
	return (strcmp(string1.data, string2.data) == 0);
}
inline bool operator == (const char *string1, const BString& string2)
{
	return (strcmp(string1, string2.data) == 0);
}
inline bool operator == (const BString& string1, const char * string2)
{
	return (strcmp(string1.data, string2) == 0);
}

// !=
inline bool operator != (const BString & string1, const BString &string2)
{
	if (string1.data == string2.data) return false; // self == self
	return (strcmp(string1.data, string2.data) != 0);
}
inline bool operator != (const char *string1, const BString& string2)
{
	return (strcmp(string1, string2.data) != 0);
}
inline bool operator != (const BString& string1, const char * string2)
{
	return (strcmp(string1.data, string2) != 0);
}


// >
inline bool operator > (const BString & string1, const BString &string2)
{
	if (string1.data == string2.data) return false; // self == self
	return (strcmp(string1.data, string2.data) > 0);
}
inline bool operator > (const char *string1, const BString& string2)
{
	return (strcmp(string1, string2.data) > 0);
}
inline bool operator > (const BString& string1, const char * string2)
{
	return (strcmp(string1.data, string2) > 0);
}

// <
inline bool operator < (const BString & string1, const BString &string2)
{
	if (string1.data == string2.data) return false; // self == self
	return (strcmp(string1.data, string2.data) < 0);
}
inline bool operator < (const char *string1, const BString& string2)
{
	return (strcmp(string1, string2.data) < 0);
}
inline bool operator < (const BString& string1, const char * string2)
{
	return (strcmp(string1.data, string2) < 0);
}
#endif
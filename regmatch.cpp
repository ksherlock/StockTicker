
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


int regmatch(char *pattern, char *text);

#if 0
int main(int argc, char **argv)
{

//	return !regmatch("(com|org)a", "orga");

	if (argc != 3) return 1;
	
	return !regmatch(argv[1], argv[2]);
}
#endif

/*
 * - match anything (0+ occurrences)
 ? - match 1 character of anything
 [...] match any character in the set
 [^...] match any character *not* in the set
 {str<,str>} match any comma-separated-value
*/

#define CHECK_ESC(x)			\
	if (esc)					\
	{							\
		if (*t != x) return 0;	\
		t++;					\
		cp++;					\
		esc = false;			\
	}							\

int regmatch(char *pattern, char *text)
{
char *cp;
char *t;
int ret = false;
int esc = false;



	cp = pattern;
	t = text;
	while(*cp)
	{
		switch (*cp)
		{
		// RECURSIVE
		case '*':	// match 0+ chars of anything
			CHECK_ESC('*')
			else
			{
				char *tmp;
				if (!*(cp+1)) return true;  // must match
				tmp = t;
				cp++;
				while (*tmp)
				{
					if (regmatch(cp, tmp)) return 1;  // match
					tmp++;
				}
				return 0;  // no match
			}
			break;
		case '?':	// match exactly 1 occurrence of any char
			CHECK_ESC('?')
			else
			{
				if (!*t) return false;
				cp++;
				t++;
			}
			break;

		case '[':	// sets
			CHECK_ESC('[')
			else
			{
				// rules:
				// - indicates a range UNLESS it is the first/final character
				// ] indicates end of set UNLESS it is the first character
				// ^ indicates exclude IF it is the first character
				//[^]-] = don't match "]" or "-"

				int exc = 0;
				int done = 0;
				int match = 0;
				//int prev = 0;
				
				if (!*t) return 0;  // can't match

				cp++;  //move past [
				if (*cp == '^')
				{
					exc = 1;
					cp++;
				}
				// 
				while (!done)
				{
					if ((cp[1] == '-') && cp[2] != ']') // range
					{
						if (cp[0] <= cp[2])
						{
							if ((*t >= cp[0]) && (*t <= cp[2]))
							{
								match = 1;
								done = 1;
							}
						}
						cp += 2;  //move to end-of range
					}
					#if 0
					if ((*cp == '-') && (cp[1] != ']') && prev)
					{
						cp++; // now points to end of range
						if ((prev < *cp) 	// valid range
							&& (*t > prev)	// greater than start (= checked before)
							&& (*t <= *cp))	// <= end
						{
							match = 1;
							done = 1;
						}
					}
					#endif
					else
					{
						//prev = *cp;  // current char
						if (*cp == *t) //(prev == *t)
						{
							match = 1;
							done = 1;
							break;
						}
					}
					cp++;
					if (*cp == ']') done = 1;
				}
				while (*cp && (*cp != ']')) cp++; // find end of set
				cp++; // move past ']'
				t++;
				//if (match && exc) return 0;
				//if (!match && !exc) return 0;
				if (match + exc != 1) return 0;
			}
			break;
		
		case ']':	// posix std - only valid if escaped...
			CHECK_ESC(']')
			else cp++; // just skip
			break;

		// RECURSIVE
		// this has to be recursive to catch cases like
		// {lo,lon,long} which would match "long", but only if all
		// possibilities are checked
		case '(':	// string sets {abc,efg}
			CHECK_ESC('(')
			else
			{
				//int exc = 0;
				int done = 0;
				int match = 0;
				char *tmp;
				char *end;
				// {,a} should match an optional 'a'
				//if (!*t) return 0;  // can't match
				tmp = t;
				// get ptr to next pattern to match
				end = cp;
				while (*end && (*end != ')')) end++;
				if (*end) end++;
				cp++;  //move past {

				while(!done)
				{
					if (*cp == '|' || *cp == ')' ) // matched substring.  Now recurse
					{
						if (regmatch(end, tmp)) return 1;
						//if (match == 1 && exc == 0) return 1;
						//if (match == 1 && exc == 1) return 0; //

						tmp = t;	// reset to starting point for next token
					}
					if (*cp == *tmp)
					{
						tmp++;
						cp++;
						continue;
					}
					if (*cp != *tmp) // not a match, get next token
					{
						while (*cp && (*cp != '|') && (*cp != ')')) cp++;
						tmp = t;	// reset
					}
	
					
					if (*cp == ')') done = 1;
					else cp++;
				}
				return match;
				//if (match + exc != 1) return 0;
				//return 1;
				
				//while (*cp, *cp != '}') cp++; // find end of set
				//cp++; // move past '}'
			}
			break;
		case ')':	//
			CHECK_ESC(')')
			else cp++;	// just skip
			break;

		case '\\':	// check if an escaped character....
			CHECK_ESC('\\')
			else
			{
				cp++;
				esc = true;
			}
			break;
		
		default:
			// do an exact character match
			esc = false;
			if (*cp != *t) return 0;
			else
			{
				cp++;
				t++;
			}
		}
	}
	if (*cp || * t) return 0;  // leftovers!
	return 1;
};
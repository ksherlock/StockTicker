#ifndef __SUPPORT_DEFS_H__
#define __SUPPORT_DEFS_H__

#ifdef __GCC__
typedef unsigned long long uint64;
typedef long long int64;
#elif _WINDOWS
typedef unsigned __int64 uint64;
typedef __int64 int64;
#endif
typedef unsigned long uint32;
typedef long int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;

typedef long off_t;
typedef long status_t;
typedef long ssize_t;

#include "Errors.h"
#if 0
enum
{
	B_OK = 1,
	B_ERROR = -1,
	B_EOF = -2,
	B_NOT_ALLOWED = -3,
	B_BAD_FILE = -4,
	B_NO_INIT = -6,
	B_NO_MEMORY = -7,
	B_NAME_NOT_FOUND = -8,
	B_BAD_VALUE = -9,
	B_BAD_TYPE = -10,
	B_BAD_INDEX = -11,
};
#endif
#endif
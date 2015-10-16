#ifndef _COMMON_TYPES_H
#define _COMMON_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL            ((void *) 0)
#endif
// -------------------------           /


#ifndef  BOOL
#define BOOL            int
#endif

#ifndef  TRUE
#define TRUE            1
#endif

#ifndef  FALSE
#define FALSE           0
#endif
// -------------------------           /


#undef  LLONG
#define LLONG           long long
// -------------------------           /


#undef  LONG_DOUBLE
#define LONG_DOUBLE   	long double
// -------------------------           /

#ifndef INT64
	#if defined(WIN32)
		typedef unsigned __int64 UINT64;
		typedef __int64 INT64;
	#else
		typedef unsigned long long UINT64;
		typedef long long INT64;
	#endif
#endif
// -------------------------           /

#ifdef _USE_SQL
#include <sqltypes.h>
#else
typedef unsigned int   DWORD;
#endif
// -------------------------           /

#ifndef BYTE
typedef unsigned char   BYTE;
#endif
// -------------------------           /

#ifndef WORD
typedef unsigned short  WORD;
#endif
// -------------------------           /

#ifndef UINT
typedef unsigned int    UINT;
#endif
// -------------------------           /

#ifndef byte
typedef unsigned char   byte;
#endif
// -------------------------           /

#ifndef SOCKET
typedef int             SOCKET;
#endif
// -------------------------           /

#ifndef LONG
typedef long            LONG;
#endif
// -------------------------           /

#ifndef LOWORD
#define LOWORD(l)           ((WORD)((DWORD)(l) & 0xffff))
#endif

#ifndef HIWORD
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))
#endif

#ifndef LOBYTE
#define LOBYTE(w)           ((BYTE)((DWORD)(w) & 0xff))
#endif

#ifndef HIBYTE
#define HIBYTE(w)           ((BYTE)((DWORD)(w) >> 8))
#endif

#ifndef FLOATZERO
#define FLOATZERO        0.0001f
#endif

#ifndef FLOATINVALID   
#define FLOATINVALID     -1.0f
#endif

#ifndef TIME_T32
typedef unsigned int            TIME_T32;
#endif
// -------------------------           /

#define MAX_BLOCK_SIZE          8192
#define MAX_LINE_SIZE           1024
#define MAX_PATH_LEN            256
// -------------------------           /


#ifdef __cplusplus
}
#endif

#endif  /* _COMMON_TYPES_H */

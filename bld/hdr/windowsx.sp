::
:: this mapping is used by WIN32 platform for 16-bit Windows source code compatibility
:: in windowsx.h header file
:: it removes appropriate entry in malloc.h and string.h if windowsx.h header is used
::
:segment WINDOWSX_MALLOC | WINDOWSX_STRING
:segment DOS
#ifndef _INC_WINDOWSX
:endsegment
:endsegment
:segment WINDOWSX_DEFINES
#define _ffree      free
#define _fmalloc    malloc
#define _fmsize     _msize
#define _frealloc   realloc
#define _nstrdup    _strdup
#define hmemcpy     MoveMemory
:elsesegment WINDOWSX_MALLOC
:: these functions don't exist in OW C run-time library for WIN32 platform
:: therefore they are mapped to existing run-time functions
:endsegment
:segment WINDOWSX_DEFINES
#define _ncalloc    calloc
#define _nexpand    _expand
#define _nfree      free
#define _nmalloc    malloc
#define _nrealloc   realloc
#define _nmsize     _msize
:elsesegment WINDOWSX_MALLOC
_WCRTLINK extern void _WCNEAR *_ncalloc( __w_size_t __n, __w_size_t __size );
_WCRTLINK extern void _WCNEAR *_nexpand( void _WCNEAR *__ptr, __w_size_t __size );
_WCRTLINK extern void _nfree( void _WCNEAR *__ptr );
_WCRTLINK extern void _WCNEAR *_nmalloc( __w_size_t __size );
_WCRTLINK extern void _WCNEAR *_nrealloc( void _WCNEAR *__ptr, __w_size_t __size );
_WCRTLINK extern __w_size_t _nmsize( void _WCNEAR *__ptr );
:endsegment
:segment WINDOWSX_DEFINES
#define _fmemccpy   _memccpy
#define _fmemchr    memchr
#define _fmemcmp    memcmp
#define _fmemcpy    memcpy
#define _fmemicmp   _memicmp
#define _fmemmove   memmove
#define _fmemset    memset
#define _fstrcat    strcat
#define _fstrchr    strchr
#define _fstrcmp    strcmp
:elsesegment WINDOWSX_STRING
_WCRTLINK extern void _WCFAR *_fmemccpy( void _WCFAR *__s1, const void _WCFAR *__s2, int __c, __w_size_t __n );
_WCIRTLINK extern void _WCFAR *_fmemchr( const void _WCFAR *__s, int __c, __w_size_t __n );
_WCIRTLINK extern int _fmemcmp( const void _WCFAR *__s1, const void _WCFAR *__s2, __w_size_t __n );
_WCIRTLINK extern void _WCFAR *_fmemcpy( void _WCFAR *__s1, const void _WCFAR *__s2, __w_size_t __n );
_WCRTLINK extern int _fmemicmp( const void _WCFAR *__s1, const void _WCFAR *__s2, __w_size_t __n );
_WCRTLINK extern void _WCFAR *_fmemmove( void _WCFAR *__s1, const void _WCFAR *__s2, __w_size_t __n );
_WCIRTLINK extern void _WCFAR *_fmemset( void _WCFAR *__s, int __c, __w_size_t __n );
_WCIRTLINK extern char _WCFAR *_fstrcat( char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCRTLINK extern char _WCFAR *_fstrchr( const char _WCFAR *__s, int __c );
_WCIRTLINK extern int _fstrcmp( const char _WCFAR *__s1, const char _WCFAR *__s2 );
:endsegment
:segment WINDOWSX_DEFINES
#define _fstrcpy    strcpy
#define _fstrcspn   strcspn
#define _fstrdup    _strdup
#define _fstricmp   _stricmp
#define _fstrlen    strlen
#define _fstrlwr    _strlwr
#define _fstrncat   strncat
#define _fstrncmp   strncmp
#define _fstrncpy   strncpy
#define _fstrnicmp  _strnicmp
:elsesegment WINDOWSX_STRING
_WCIRTLINK extern char _WCFAR *_fstrcpy( char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCRTLINK extern __w_size_t _fstrcspn( const char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCRTLINK extern char _WCFAR *_fstrdup( const char _WCFAR *__string );
_WCRTLINK extern int _fstricmp( const char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCIRTLINK extern __w_size_t _fstrlen( const char _WCFAR *__s );
_WCRTLINK extern char _WCFAR *_fstrlwr( char _WCFAR *__string );
_WCRTLINK extern char _WCFAR *_fstrncat( char _WCFAR *__s1, const char _WCFAR *__s2, __w_size_t __n );
_WCRTLINK extern int _fstrncmp( const char _WCFAR *__s1, const char _WCFAR *__s2, __w_size_t __n );
_WCRTLINK extern char _WCFAR *_fstrncpy( char _WCFAR *__s1, const char _WCFAR *__s2, __w_size_t __n );
_WCRTLINK extern int _fstrnicmp( const char _WCFAR *__s1, const char _WCFAR *__s2, __w_size_t __n );
:endsegment
:segment WINDOWSX_DEFINES
#define _fstrnset   _strnset
#define _fstrpbrk   strpbrk
#define _fstrrchr   strrchr
#define _fstrrev    _strrev
#define _fstrset    _strset
#define _fstrspn    strspn
#define _fstrstr    strstr
#define _fstrtok    strtok
#define _fstrupr    _strupr
:elsesegment WINDOWSX_STRING
_WCRTLINK extern char _WCFAR *_fstrnset( char _WCFAR *__string, int __c, __w_size_t __len );
_WCRTLINK extern char _WCFAR *_fstrpbrk( const char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCRTLINK extern char _WCFAR *_fstrrchr( const char _WCFAR *__s, int __c );
_WCRTLINK extern char _WCFAR *_fstrrev( char _WCFAR *__string );
_WCRTLINK extern char _WCFAR *_fstrset( char _WCFAR *__string, int __c );
_WCRTLINK extern __w_size_t _fstrspn( const char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCRTLINK extern char _WCFAR *_fstrstr( const char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCRTLINK extern char _WCFAR *_fstrtok( char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCRTLINK extern char _WCFAR *_fstrupr( char _WCFAR *__string );
:endsegment
:: following far functions are Watcom extension
:segment WINDOWSX_DEFINES
#define _fstrspnp   strspnp
#define _fstrtok_r  strtok_r
:elsesegment WINDOWSX_STRING
_WCRTLINK extern char _WCFAR *_fstrspnp( const char _WCFAR *__s1, const char _WCFAR *__s2 );
_WCRTLINK extern char _WCFAR *_fstrtok_r( char _WCFAR *__str, const char _WCFAR *__charset, char _WCFAR **__ptr );
:endsegment
:segment WINDOWSX_MALLOC | WINDOWSX_STRING
:segment DOS
#endif
:endsegment
:endsegment

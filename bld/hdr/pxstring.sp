:: POSIX single-byte and wide character string extension functions
::
::                          <string.h> (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *wcsdup( const wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: POSIX
_WCRTLINK extern char       *strdup( const char *__string );
:endsegment
::
::                          <string.h> (<memory.h>,<string.h>)
:segment STRING_MH
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: POSIX
_WCRTLINK extern void       *memccpy( void *__s1, const void *__s2, int __c, __w_size_t __n );
:endsegment
:endsegment

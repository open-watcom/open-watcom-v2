:: POSIX strdup function
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

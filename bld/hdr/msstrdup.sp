:: MS strdup function
::
::                          <string.h> (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wcsdup( const wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_strdup( const char *__string );
:elsesegment
:: POSIX
:endsegment
::

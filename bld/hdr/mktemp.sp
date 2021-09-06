:: MS _mktemp function
::
::                          ??? (<io.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wmktemp( wchar_t *__template );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_mktemp( char *__template );
:elsesegment
:: MS
:endsegment

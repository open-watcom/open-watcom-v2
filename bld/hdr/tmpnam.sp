:: ISO C tmpnam function
::
::                          <stdio.h> (<stdio.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wtmpnam( wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: ISO C
_WCRTLINK extern char       *tmpnam( char *__s );
:endsegment

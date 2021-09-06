:: POSIX/ISO tmpnam function
::
::                          <stdio.h> (<stdio.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern wchar_t    *_wtmpnam( wchar_t * );
:elsesegment ANSINAME
:: MS extension
:elsesegment
:: POSIX/ISO
_WCRTLINK extern char       *tmpnam( char *__s );
:endsegment

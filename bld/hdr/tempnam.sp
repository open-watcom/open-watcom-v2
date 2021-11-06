:: POSIX tempnam function
::
::                          <stdio.h> (<stdio.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wtempnam( const wchar_t *__dir, const wchar_t *__prefix );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_tempnam( const char *__dir, const char *__prefix );
:elsesegment
:: POSIX
_WCRTLINK extern char       *tempnam( const char *__dir, const char *__prefix );
:endsegment

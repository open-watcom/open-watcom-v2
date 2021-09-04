:: MS extensio _mktemp function
::
::                          ??? (<io.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern wchar_t    *_wmktemp( wchar_t *__template );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern char       *_mktemp( char *__template );
:elsesegment
:: POSIX
:endsegment

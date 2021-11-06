:: MS fullpath function
::
::                          ??? (<stdlib.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wfullpath( wchar_t *, const wchar_t *, __w_size_t );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_fullpath( char *__buf, const char *__path, __w_size_t __size );
:elsesegment
:: MS
:endsegment
::

:: MS sopen function
::
::                          ??? (<io.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wsopen( const wchar_t *, int, int, ... );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _sopen( const char *__path, int __oflag, int __share, ... );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        sopen( const char *__path, int __oflag, int __share, ... );
:endsegment

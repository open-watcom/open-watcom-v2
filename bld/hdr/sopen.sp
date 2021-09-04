:: MS specific sopen function
::
::                          ??? (<io.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wsopen( const wchar_t *, int, int, ... );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _sopen( const char *__path, int __oflag, int __share, ... );
:elsesegment
:: MS deprecated alias
_WCRTLINK extern int        sopen( const char *__path, int __oflag, int __share, ... );
:endsegment

:: ISO C system function
::
::                          <stdlib.h> (<process.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wsystem( const wchar_t *__cmd );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: ISO C
_WCRTLINK extern int        system( const char *__cmd );
:endsegment

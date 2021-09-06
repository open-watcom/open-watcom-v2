:: ISO system function
::
::                          <stdlib.h> (<process.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wsystem( const wchar_t *__cmd );
:elsesegment ANSINAME
:: MS extension
:elsesegment
:: ISO C
_WCRTLINK extern int        system( const char *__cmd );
:endsegment

:: ISO C remove function
::
::                          <stdio.h> (<stdio.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wremove( const wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: ISO C
_WCRTLINK extern int        remove( const char *__filename );
:endsegment
::

:: ISO C remove function
::
::                          <stdio.h> (<stdio.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wremove( const wchar_t * );
:elsesegment MSEXT
:: MS extension
:elsesegment
:: ISO C
_WCRTLINK extern int        remove( const char *__filename );
:endsegment
::

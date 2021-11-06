:: ISO C rename function
::
::                          <stdio.h> (<io.h>,<stdio.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wrename( const wchar_t *, const wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: ISO C
_WCRTLINK extern int        rename( const char *__old, const char *__new );
:endsegment
::

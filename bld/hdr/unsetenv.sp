:: POSIX unsetenv function
::
::                          <stdlib.h> (<stdlib.h>,<process.h>,<env.h>,<wchar.h>)
:segment WIDE
:: OW wide extension
_WCRTLINK extern int        _wunsetenv( const wchar_t *__name );
:elsesegment ANSINAME
:: OW extension
:elsesegment
:: POSIX
_WCRTLINK extern int        unsetenv( const char *__name );
:endsegment

:: POSIX putenv function
::
::                          <stdlib.h> (<stdlib.h>,<process.h>,<env.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wputenv( const wchar_t *__env_string );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _putenv( const char *__string );
:elsesegment
:: POSIX
_WCRTLINK extern int        putenv( const char *__string );
:endsegment

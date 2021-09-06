:: POSIX setenv function
::
::                          <stdlib.h> (<stdlib.h>,<process.h>,<env.h>,<wchar.h>)
:segment WIDE
:: OW wide extension
_WCRTLINK extern int        _wsetenv( const wchar_t *__name, const wchar_t *__newvalue, int __overwrite );
:elsesegment ANSINAME
:: OW extension
:elsesegment
:: POSIX
_WCRTLINK extern int        setenv( const char *__name, const char *__newvalue, int __overwrite );
:endsegment

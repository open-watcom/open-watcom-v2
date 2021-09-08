:: ISO C getenv function
::
::                          <stdlib.h> (<stdlib.h>,<process.h>,<env.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wgetenv( const wchar_t *__name );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: ISO C
_WCRTLINK extern char       *getenv( const char *__name );
:endsegment

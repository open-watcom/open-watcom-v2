:: ISO getenv function
::
::                          <stdlib.h> (<stdlib.h>,<process.h>,<env.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern wchar_t    *_wgetenv( const wchar_t *__name );
:elsesegment ANSINAME
:: MS extension
:elsesegment
:: ISO C
_WCRTLINK extern char       *getenv( const char *__name );
:endsegment

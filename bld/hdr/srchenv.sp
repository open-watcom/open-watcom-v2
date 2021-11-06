:: MS searchenv function
::
::                          ??? (<stdlib.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern void       _wsearchenv( const wchar_t *__name, const wchar_t *__env_var, wchar_t *__buf );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern void       _searchenv( const char *__name, const char *__env_var, char *__buf );
:elsesegment
:: MS
:endsegment
::

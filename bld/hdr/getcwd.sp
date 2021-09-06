:: POSIX getcwd function
::
::                          <unistd.h> (<direct.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wgetcwd( wchar_t *__buf, __w_size_t __size );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_getcwd( char *__buf, __w_size_t __size );
:elsesegment
:: POSIX
_WCRTLINK extern char       *getcwd( char *__buf, __w_size_t __size );
:endsegment
::

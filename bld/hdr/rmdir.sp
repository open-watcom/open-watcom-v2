:: POSIX rmdir function
::
::                          <unistd.h> (<direct.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wrmdir( const wchar_t *__path );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _rmdir( const char *__path );
:elsesegment
:: POSIX
_WCRTLINK extern int        rmdir( const char *__path );
:endsegment
::

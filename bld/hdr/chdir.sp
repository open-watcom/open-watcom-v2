:: POSIX chdir function
::
::                          <unistd.h> (<direct.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wchdir( const wchar_t *__path );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _chdir( const char *__path );
:elsesegment
:: POSIX
_WCRTLINK extern int        chdir( const char *__path );
:endsegment

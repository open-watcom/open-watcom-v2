:: POSIX chdir function
::
::                          <unistd.h> (<direct.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wchdir( const wchar_t *__path );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _chdir( const char *__path );
:elsesegment
:: POSIX
_WCRTLINK extern int        chdir( const char *__path );
:endsegment

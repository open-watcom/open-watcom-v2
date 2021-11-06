:: POSIX mkdir function
::
::                          <sys/stat.h> (<direct.h>,<wchar.h>)
:segment LINUX | QNX
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: POSIX
_WCRTLINK extern int        mkdir( const char *__path, mode_t mode );
:endsegment
:elsesegment
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wmkdir( const wchar_t *__path );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _mkdir( const char *__path );
:elsesegment
:: POSIX
_WCRTLINK extern int        mkdir( const char *__path );
:endsegment
:endsegment
::

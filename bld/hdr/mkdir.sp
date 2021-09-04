:: POSIX mkdir function
::
::                          <sys/stat.h> (<direct.h>,<wchar.h>)
:segment LINUX | QNX
:segment WIDE
:: MS wide extension
:elsesegment MSEXT
:: MS extension
:elsesegment
:: POSIX
_WCRTLINK extern int        mkdir( const char *__path, mode_t mode );
:endsegment
:elsesegment
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wmkdir( const wchar_t *__path );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _mkdir( const char *__path );
:elsesegment
:: POSIX
_WCRTLINK extern int        mkdir( const char *__path );
:endsegment
:endsegment
::

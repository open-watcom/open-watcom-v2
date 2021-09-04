:: POSIX chmod function
::
::                          <sys/stat.h> (<io.h>,<wchar.h>)
:segment WIDE
_WCRTLINK extern int        _wchmod( const wchar_t *__path, mode_t __pmode );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _chmod( const char *__path, mode_t __pmode );
:elsesegment
:: POSIX
_WCRTLINK extern int        chmod( const char *__path, mode_t __pmode );
:endsegment

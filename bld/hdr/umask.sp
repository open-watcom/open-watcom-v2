:: POSIX umask function
::
::                          <sys/stat.h>,<sys/stat.h> (<io.h>)
:segment WIDE
:: MS wide extension
:elsesegment MSEXT
:: MS version
_WCRTLINK extern mode_t     _umask( mode_t __cmask );
:elsesegment
:: POSIX
_WCRTLINK extern mode_t     umask( mode_t __cmask );
:endsegment

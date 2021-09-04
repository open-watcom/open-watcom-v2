:: POSIX getpid
::
::                          <unistd.h> (<process.h>)
:segment WIDE
:: MS wide extension
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern pid_t      _getpid( void );
:elsesegment
:: POSIX
_WCRTLINK extern pid_t      getpid( void );
:endsegment

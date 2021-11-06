:: POSIX getpid
::
::                          <unistd.h> (<process.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern pid_t      _getpid( void );
:elsesegment
:: POSIX
_WCRTLINK extern pid_t      getpid( void );
:endsegment

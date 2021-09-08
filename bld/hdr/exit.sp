:: ISO C exit function
::
::                          <stdlib.h> (<stdlib.h>,<process.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: ISO C
_WCRTLINK _WCNORETURN extern void exit( int __status );
:endsegment

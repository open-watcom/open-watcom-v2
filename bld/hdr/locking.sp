:: MS locking function
::
::                          ??? (<io.h>,<sys/locking.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _locking( int __handle, int __mode, unsigned long __nbyte );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        locking( int __handle, int __mode, unsigned long __nbyte );
:endsegment
::

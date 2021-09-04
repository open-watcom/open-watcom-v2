:: MS extension locking function
::
::                          ??? (<io.h>,<sys/locking.h>)
:segment WIDE
:: MS wide extension
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _locking(int __handle,int __mode,unsigned long __nbyte);
:elsesegment
:: MS deprecated
_WCRTLINK extern int        locking(int __handle,int __mode,unsigned long __nbyte);
:endsegment
::

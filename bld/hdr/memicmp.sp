:: MS single-byte and wide character string extension functions
::
::                          ??? (<memory.h>,<string.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _memicmp( const void *__s1, const void *__s2, __w_size_t __n );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        memicmp( const void *__s1, const void *__s2, __w_size_t __n );
:endsegment
::

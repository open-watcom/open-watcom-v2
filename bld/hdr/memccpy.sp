:: POSIX memccpy function
::
::                          <string.h> (<memory.h>,<string.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern void       *_memccpy( void *__s1, const void *__s2, int __c, __w_size_t __n );
:elsesegment
:: POSIX
_WCRTLINK extern void       *memccpy( void *__s1, const void *__s2, int __c, __w_size_t __n );
:endsegment
::

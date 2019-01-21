:segment QNX
#ifdef __386__
:endsegment
:segment LINUX | QNX
_WCRTLINK extern int          brk( void _WCNEAR *__endds );
:endsegment
:segment QNX
#endif
:endsegment
_WCRTLINK extern void _WCNEAR *__brk( unsigned __new_brk_value );
_WCRTLINK extern void _WCNEAR *sbrk( int __increment );

:segment QNX
#if defined(__386__)
:endsegment
:segment QNX | LINUX
_WCRTLINK extern int          brk( void *endds );
:endsegment
:segment QNX
#endif
:endsegment
_WCRTLINK extern void _WCNEAR *sbrk( int __increment );

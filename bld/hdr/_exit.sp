#ifdef __NETWARE__
 _WCRTLINK extern void  _exit( int __status );
#else
 _WCRTLINK _NORETURN extern void  _exit( int __status );
#endif

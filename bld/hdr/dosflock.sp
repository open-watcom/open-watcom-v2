:: DOS like file locking
_WCRTLINK extern int        lock( int __handle, unsigned long __offset, unsigned long __nbytes );
_WCRTLINK extern int        unlock( int __handle, unsigned long __offset, unsigned long __nbytes );

:: Common I/O functions
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _read( int fildes, void *buf, unsigned nbyte );
:elsesegment
:: POSIX
_WCRTLINK extern ssize_t    read( int fildes, void *buf, size_t nbyte );
:endsegment
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _write( int fildes, const void *buf, unsigned nbyte );
:elsesegment
:: POSIX
_WCRTLINK extern ssize_t    write( int fildes, const void *buf, size_t nbyte );
:endsegment
::
:segment WIDE
_WCRTLINK extern int        _waccess( const wchar_t *__path, int __mode );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _access( const char *__path, int __mode );
:elsesegment
:: POSIX
_WCRTLINK extern int        access( const char *__path, int __mode );
:endsegment
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _dup( int __handle );
:elsesegment
:: POSIX
_WCRTLINK extern int        dup( int __handle );
:endsegment
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _dup2( int __handle1, int __handle2 );
:elsesegment
:: POSIX
_WCRTLINK extern int        dup2( int __handle1, int __handle2 );
:endsegment
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _close( int __handle );
:elsesegment
:: POSIX
_WCRTLINK extern int        close( int __fildes );
:endsegment
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern off_t      _lseek( int __handle, off_t __offset, int __origin );
_WCRTLINK extern long long  _lseeki64( int __fildes, long long __offset, int __whence );
:elsesegment
:: POSIX
_WCRTLINK extern off_t      lseek( int __handle, off_t __offset, int __origin );
:endsegment
::
:segment WIDE
_WCRTLINK extern int        _wunlink( const wchar_t *__path );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _unlink( const char *__path );
:elsesegment
:: POSIX
_WCRTLINK extern int        unlink( const char *__path );
:endsegment
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _isatty( int __handle );
:elsesegment
:: POSIX
_WCRTLINK extern int        isatty( int __handle );
:endsegment
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
:elsesegment
:: POSIX
_WCRTLINK extern int        fsync( int __handle );
:endsegment
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
:segment DOS
_WCRTLINK extern int        _commit( int __handle );
_WCRTLINK extern int        _pipe( int *__phandles, unsigned __psize, int __textmode );
:endsegment
:elsesegment
:: POSIX
:segment !DOS
_WCRTLINK extern int        pipe( int __fildes[2] );
:endsegment
:endsegment
::

::
:: OW OSF handle handling functions
::
:segment WIDE
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern long       _get_osfhandle( int __posixhandle );
_WCRTLINK extern int        _hdopen( int __handle,int __mode );
_WCRTLINK extern int        _open_osfhandle( long __osfhandle, int __flags );
_WCRTLINK extern int        _os_handle( int __handle );
:elsesegment
:: POSIX
:endsegment
::

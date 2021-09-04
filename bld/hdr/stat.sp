:: POSIX stat function family
::
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wstat( const wchar_t *__path, struct stat *__buf );
_WCRTLINK extern int        _wstati64( const wchar_t *__path, struct _stati64 *__buf );
:elsesegment MSEXT
:: extension
_WCRTLINK extern int        _stat( const char *__path, struct stat *__buf );
:segment DOS
_WCRTLINK extern int        _stati64( const char *__path, struct _stati64 *__buf );
:endsegment
:segment LINUX
_WCRTLINK extern int        stat64( const char *__path, struct stat64 *__buf );
:endsegment
:elsesegment
:: POSIX
_WCRTLINK extern int        stat( const char *__path, struct stat *__buf );
:endsegment
::
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wfstat( int __fildes, struct stat *__buf );
_WCRTLINK extern int        _wfstati64( int __fildes, struct _stati64 *__buf );
:elsesegment MSEXT
:: extension
_WCRTLINK extern int        _fstat( int __fildes, struct stat *__buf );
:segment DOS
_WCRTLINK extern int        _fstati64( int __fildes, struct _stati64 *__buf );
:endsegment
:segment LINUX
_WCRTLINK extern int        fstat64( int __fildes, struct stat64 *__buf );
:endsegment
:elsesegment
:: POSIX
_WCRTLINK extern int        fstat( int __fildes, struct stat *__buf );
:endsegment
::
:segment LINUX
:segment WIDE
:: MS wide extension
:elsesegment MSEXT
:: extension
_WCRTLINK extern int        lstat64( const char *__path, struct stat64 *__buf );
:elsesegment
:: POSIX
_WCRTLINK extern int        lstat( const char *__path, struct stat *__buf );
:endsegment
:endsegment
::

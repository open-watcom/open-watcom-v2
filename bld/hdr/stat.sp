:: POSIX stat function family
::
::                          <fcntl.h>,<sys/stat.h> (<sys/stat.h>,<wchar.h>)
:segment WIDE
:: Wide character version
:segment DOS
_WCRTLINK extern int        _wstat( const wchar_t *__path, struct stat *__buf );
_WCRTLINK extern int        _wstati64( const wchar_t *__path, struct _stati64 *__buf );
:endsegment
:elsesegment ANSINAME
:: extension
_WCRTLINK extern int        _stat( const char *__path, struct stat *__buf );
:segment DOS
_WCRTLINK extern int        _stati64( const char *__path, struct _stati64 *__buf );
:endsegment
:elsesegment
:: POSIX
_WCRTLINK extern int        stat( const char *__path, struct stat *__buf );
:segment LINUX
_WCRTLINK extern int        stat64( const char *__path, struct stat64 *__buf );
:endsegment
:endsegment
::
::                          <fcntl.h>,<sys/stat.h> (<sys/stat.h>,<wchar.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: extension
_WCRTLINK extern int        _fstat( int __fildes, struct stat *__buf );
:segment DOS
_WCRTLINK extern int        _fstati64( int __fildes, struct _stati64 *__buf );
:endsegment
:elsesegment
:: POSIX
_WCRTLINK extern int        fstat( int __fildes, struct stat *__buf );
:segment LINUX
_WCRTLINK extern int        fstat64( int __fildes, struct stat64 *__buf );
:endsegment
:endsegment
::
::                          <fcntl.h>,<sys/stat.h> (<sys/stat.h>,<wchar.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: extension
:elsesegment
:: POSIX
_WCRTLINK extern int        lstat( const char *__path, struct stat *__buf );
:segment LINUX
_WCRTLINK extern int        lstat64( const char *__path, struct stat64 *__buf );
:endsegment
:endsegment
::

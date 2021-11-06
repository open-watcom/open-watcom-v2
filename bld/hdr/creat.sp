:: POSIX creat function
::
::                          <fcntl.h>,<sys/stat.h> (<io.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wcreat( const wchar_t *, mode_t __mode );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _creat( const char *__path, mode_t __mode );
:elsesegment
:: POSIX
_WCRTLINK extern int        creat( const char *__path, mode_t __mode );
:endsegment

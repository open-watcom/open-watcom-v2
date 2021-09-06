:: POSIX open function
::
::                          <fcntl.h>,<sys/stat.h> (<io.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wopen( const wchar_t *__path, int __oflag, ... );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _open( const char *__path, int __oflag, ... );
:elsesegment
:: POSIX
_WCRTLINK extern int        open( const char *__path, int __oflag, ... );
:endsegment

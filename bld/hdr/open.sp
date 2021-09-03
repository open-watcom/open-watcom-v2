:: POSIX open function
::
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wopen( const wchar_t *__path, int __oflag, ... );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _open( const char *__path, int __oflag, ... );
:elsesegment
:: POSIX
_WCRTLINK extern int        open( const char *__path, int __oflag, ... );
:endsegment

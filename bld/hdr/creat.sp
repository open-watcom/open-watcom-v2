:: POSIX creat function
::
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wcreat( const wchar_t *, mode_t __mode );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _creat( const char *__path, mode_t __mode );
:elsesegment
:: POSIX
_WCRTLINK extern int        creat( const char *__path, mode_t __mode );
:endsegment

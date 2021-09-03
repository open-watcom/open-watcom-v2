:: POSIX rmdir function
::
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wrmdir( const wchar_t *__path );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _rmdir( const char *__path );
:elsesegment
:: POSIX
_WCRTLINK extern int        rmdir( const char *__path );
:endsegment
::

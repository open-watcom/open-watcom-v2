:: POSIX tempnam function
::
:segment WIDE
:: MS wide extension
_WCRTLINK extern wchar_t    *_wtempnam( const wchar_t *__dir, const wchar_t *__prefix );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern char       *_tempnam( const char *__dir, const char *__prefix );
:elsesegment
:: POSIX
_WCRTLINK extern char       *tempnam( const char *__dir, const char *__prefix );
:endsegment

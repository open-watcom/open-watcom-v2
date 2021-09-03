:: POSIX/ISO tmpnam function
::
:segment WIDE
:: MS wide extension
_WCRTLINK extern wchar_t    *_wtmpnam( wchar_t * );
:elsesegment MSEXT
:: MS extension
:elsesegment
:: POSIX/ISO
_WCRTLINK extern char       *tmpnam( char *__s );
:endsegment

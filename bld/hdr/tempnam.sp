:segment WIDE
_WCRTLINK extern wchar_t    *_wtempnam( const wchar_t *__dir, const wchar_t *__prefix );
:elsesegment
_WCRTLINK extern char       *_tempnam( const char *__dir, const char *__prefix );
:endsegment

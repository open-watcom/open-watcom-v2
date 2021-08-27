:segment WIDE
_WCRTLINK extern wchar_t    *_wmktemp( wchar_t *__template );
:elsesegment
_WCRTLINK extern char       *_mktemp( char *__template );
:endsegment

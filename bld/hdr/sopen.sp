:: MS specific sopen
::
:segment WIDE
_WCRTLINK extern int _wsopen( const wchar_t *, int, int, ... );
:elsesegment
_WCRTLINK extern int _sopen( const char *__path, int __oflag, int __share, ... );
:: MS deprecated alias
_WCRTLINK extern int sopen( const char *__path, int __oflag, int __share, ... );
:endsegment

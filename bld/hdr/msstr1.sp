:: MS single-byte and wide character string extension functions
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: MS deprecated
_WCRTLINK extern int        strcmpi( const char *__s1, const char *__s2 );
:endsegment
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wcsicoll( const wchar_t *__s1, const wchar_t *__s2 );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _stricoll( const char *__s1, const char *__s2 );
:elsesegment
:: MS
:endsegment
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wcsncoll( const wchar_t *__s1, const wchar_t *__s2, __w_size_t __n );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _strncoll( const char *__s1, const char *__s2, __w_size_t __n );
:elsesegment
:: MS
:endsegment
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wcsnicoll( const wchar_t *__s1, const wchar_t *__s2, __w_size_t __n );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _strnicoll( const char *__s1, const char *__s2, __w_size_t __n );
:elsesegment
:: MS
:endsegment
::

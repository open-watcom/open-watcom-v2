:: MS single-byte and wide character string extension functions
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wcsnset( wchar_t *, int, __w_size_t );
:: MS deprecated - not implemented
:: _WCRTLINK extern wchar_t *wcsnset( wchar_t *, int, __w_size_t );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_strnset( char *__string, int __c, __w_size_t __len );
:elsesegment
:: MS deprecated
_WCRTLINK extern char       *strnset( char *__string, int __c, __w_size_t __len );
:endsegment
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wcsnicmp( const wchar_t *, const wchar_t *, __w_size_t );
:: MS deprecated - not implemented
:: _WCRTLINK extern int     wcsnicmp( const wchar_t *, const wchar_t *, __w_size_t );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _strnicmp( const char *__s1, const char *__s2, __w_size_t __n );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        strnicmp( const char *__s1, const char *__s2, __w_size_t __n );
:endsegment
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wcslwr( wchar_t * );
:: MS deprecated - not implemented
:: _WCRTLINK extern wchar_t *wcslwr( wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_strlwr( char *__string );
:elsesegment
:: MS deprecated
_WCRTLINK extern char       *strlwr( char *__string );
:endsegment
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wcsupr( wchar_t * );
:: MS deprecated - not implemented
:: _WCRTLINK extern wchar_t *wcsupr( wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_strupr( char *__string );
:elsesegment
:: MS deprecated
_WCRTLINK extern char       *strupr( char *__string );
:endsegment
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wcsicmp( const wchar_t *, const wchar_t * );
:: MS deprecated - not implemented
:: _WCRTLINK extern int     wcsicmp( const wchar_t *, const wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _stricmp( const char *__s1, const char *__s2 );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        stricmp( const char *__s1, const char *__s2 );
:endsegment
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
_WCRTLINK extern wchar_t    *_wcsrev( wchar_t * );
:: MS deprecated - not implemented
:: _WCRTLINK extern wchar_t *wcsrev( wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_strrev( char *__string );
:elsesegment
:: MS deprecated
_WCRTLINK extern char       *strrev( char *__string );
:endsegment
::
::                          ??? (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wcsset( wchar_t *, wchar_t );
:: MS deprecated - not implemented
:: _WCRTLINK extern wchar_t *wcsset( wchar_t *, wchar_t );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_strset( char *__string, int __c );
:elsesegment
:: MS deprecated
_WCRTLINK extern char       *strset( char *__string, int __c );
:endsegment
::
::                          <string.h> (<string.h>,<wchar.h>)
:segment STRING_MH
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wcserror( int );
_WCRTLINK extern wchar_t    *__wcserror( const wchar_t * );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_strerror( const char *__s );
:elsesegment
:: MS
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
:endsegment

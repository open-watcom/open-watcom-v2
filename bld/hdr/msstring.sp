:: MS single-byte and wide character string extension functions
::
:segment WIDE
_WCRTLINK extern wchar_t    *_wcsnset( wchar_t *, int, __w_size_t );
:: MS deprecated alias - not implemented
:: _WCRTLINK extern wchar_t *wcsnset( wchar_t *, int, __w_size_t );
:elsesegment
_WCRTLINK extern char       *_strnset( char *__string, int __c, __w_size_t __len );
:: MS deprecated alias
_WCRTLINK extern char       *strnset( char *__string, int __c, __w_size_t __len );
:endsegment
::
:segment WIDE
_WCRTLINK extern int        _wcsnicmp( const wchar_t *, const wchar_t *, __w_size_t );
:: MS deprecated alias - not implemented
:: _WCRTLINK extern int     wcsnicmp( const wchar_t *, const wchar_t *, __w_size_t );
:elsesegment
_WCRTLINK extern int        _strnicmp( const char *__s1, const char *__s2, __w_size_t __n );
:: MS deprecated alias
_WCRTLINK extern int        strnicmp( const char *__s1, const char *__s2, __w_size_t __n );
:endsegment
::
:segment WIDE
_WCRTLINK extern wchar_t    *_wcsdup( const wchar_t * );
:elsesegment
:: MS prototype
_WCRTLINK extern char       *_strdup( const char *__string );
:endsegment
::
:segment WIDE
_WCRTLINK extern wchar_t    *_wcslwr( wchar_t * );
:: MS deprecated alias - not implemented
:: _WCRTLINK extern wchar_t *wcslwr( wchar_t * );
:elsesegment
_WCRTLINK extern char       *_strlwr( char *__string );
:: MS deprecated alias
_WCRTLINK extern char       *strlwr( char *__string );
:endsegment
::
:segment WIDE
_WCRTLINK extern wchar_t    *_wcsupr( wchar_t * );
:: MS deprecated alias - not implemented
:: _WCRTLINK extern wchar_t *wcsupr( wchar_t * );
:elsesegment
_WCRTLINK extern char       *_strupr( char *__string );
:: MS deprecated alias
_WCRTLINK extern char       *strupr( char *__string );
:endsegment
::
:segment WIDE
_WCRTLINK extern int        _wcsicmp( const wchar_t *, const wchar_t * );
:: MS deprecated alias - not implemented
:: _WCRTLINK extern int     wcsicmp( const wchar_t *, const wchar_t * );
:elsesegment
_WCRTLINK extern int        _stricmp( const char *__s1, const char *__s2 );
:: MS deprecated alias
_WCRTLINK extern int        stricmp( const char *__s1, const char *__s2 );
:endsegment
::
:segment WIDE
:elsesegment
:: MS deprecated alias
_WCRTLINK extern int        strcmpi( const char *__s1, const char *__s2 );
:endsegment
::
:segment WIDE
_WCRTLINK extern wchar_t    *_wcsrev( wchar_t * );
:: MS deprecated alias - not implemented
:: _WCRTLINK extern wchar_t *wcsrev( wchar_t * );
:elsesegment
_WCRTLINK extern char       *_strrev( char *__string );
:: MS deprecated alias
_WCRTLINK extern char       *strrev( char *__string );
:endsegment
::
:segment WIDE
_WCRTLINK extern wchar_t    *_wcsset( wchar_t *, wchar_t );
:: MS deprecated alias - not implemented
:: _WCRTLINK extern wchar_t *wcsset( wchar_t *, wchar_t );
:elsesegment
_WCRTLINK extern char       *_strset( char *__string, int __c );
:: MS deprecated alias
_WCRTLINK extern char       *strset( char *__string, int __c );
:endsegment
::
:segment STRING_MH
:segment WIDE
_WCRTLINK extern wchar_t    *_wcserror( int );
_WCRTLINK extern wchar_t    *__wcserror( const wchar_t * );
:elsesegment
_WCRTLINK extern char       *_strerror( const char *__s );
:endsegment
::
:segment WIDE
_WCRTLINK extern int        _wcsicoll( const wchar_t *__s1, const wchar_t *__s2 );
:elsesegment
_WCRTLINK extern int        _stricoll( const char *__s1, const char *__s2 );
:endsegment
::
:segment WIDE
_WCRTLINK extern int        _wcsncoll( const wchar_t *__s1, const wchar_t *__s2, __w_size_t __n );
:elsesegment
_WCRTLINK extern int        _strncoll( const char *__s1, const char *__s2, __w_size_t __n );
:endsegment
::
:segment WIDE
_WCRTLINK extern int        _wcsnicoll( const wchar_t *__s1, const wchar_t *__s2, __w_size_t __n );
:elsesegment
_WCRTLINK extern int        _strnicoll( const char *__s1, const char *__s2, __w_size_t __n );
:endsegment
::
:segment WIDE
:elsesegment
_WCRTLINK extern int        _memicmp( const void *__s1, const void *__s2, __w_size_t __n );
:: MS deprecated alias
_WCRTLINK extern int        memicmp( const void *__s1, const void *__s2, __w_size_t __n );
:endsegment
::
:segment WIDE
:elsesegment
_WCRTLINK extern void       *_memccpy( void *__s1, const void *__s2, int __c, __w_size_t __n );
:endsegment
:endsegment

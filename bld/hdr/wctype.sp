:include nsstd.sp
_WCRTLINK extern int        iswalnum( wint_t __wc );
_WCRTLINK extern int        iswalpha( wint_t __wc );
_WCRTLINK extern int        iswcntrl( wint_t __wc );
_WCRTLINK extern int        iswdigit( wint_t __wc );
_WCRTLINK extern int        iswgraph( wint_t __wc );
_WCRTLINK extern int        iswlower( wint_t __wc );
_WCRTLINK extern int        iswprint( wint_t __wc );
_WCRTLINK extern int        iswpunct( wint_t __wc );
_WCRTLINK extern int        iswspace( wint_t __wc );
_WCRTLINK extern int        iswupper( wint_t __wc );
_WCRTLINK extern int        iswxdigit( wint_t __wc );
_WCRTLINK extern int        iswctype( wint_t __wc, wctype_t __desc );
:: C99 functions in wctype.h that are currently considered extensions.
:include extc99.sp
_WCRTLINK extern int        iswblank( wint_t __wc );
:include extepi.sp
_WCRTLINK extern wint_t     towlower( wint_t __wc );
_WCRTLINK extern wint_t     towupper( wint_t __wc );
_WCRTLINK extern wctype_t   wctype( const char *__property );
:segment !WCHAR_MH

_WCRTLINK extern wint_t     towctrans( wint_t __wc, wctrans_t __desc );
_WCRTLINK extern wctrans_t  wctrans( const char *__property );
:endsegment
:include nsstdepi.sp

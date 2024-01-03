/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Platform independent worker routines for scanf().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#ifdef SAFE_SCANF
    #include "saferlib.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#include <stdarg.h>
#ifdef __QNX__
    #include "slibqnx.h"
#endif
#include "scanf.h"
#include "prtscncf.h"
#include "fixpoint.h"
#if defined( __WIDECHAR__ ) || defined( CLIB_USE_MBCS_TRANSLATION )
    #include <mbstring.h>
#endif
#include "setbits.h"
#include "fltsupp.h"


#define TRUE    1
#define FALSE   0

#ifdef __WIDECHAR__
    #define TO_WIDE(c)  (c)
#else
    #define TO_WIDE(c)  ((unsigned char)c)
#endif
#define PTR_ASCII(p)    ((FAR_ASCII_STRING)(p))
#define PTR_WIDE(p)     ((FAR_WIDE_STRING)(p))

#define EFG_SCANF (*__EFG_scanf)

/* internal file/string get, unget routines */
#ifdef __WINDOWS_386__
    #ifdef __SW_3S
        #pragma aux cget __modify [__eax __edx __ecx __fs __gs]
        #pragma aux uncget __modify [__eax __edx __ecx __fs __gs]
    #else
        #pragma aux cget __modify [__fs __gs]
        #pragma aux uncget __modify [__fs __gs]
    #endif
#endif

/* Macros to reduce the already large number of ifdefs in the code */
#ifdef __STDC_WANT_LIB_EXT1__

    #define GET_MAXELEM(p,x)    x = va_arg( *p, size_t )
    #define DEFINE_VARS(x,y)    size_t x, y = 0
    #define CHECK_ELEMS(x,y,z)  if( x < ++y ) return( z )
#else

    #define GET_MAXELEM(p,x)
    #define DEFINE_VARS(x,y)
    #define CHECK_ELEMS(x,y,z)

#endif

static INTCHAR_TYPE cget( PTR_SCNF_SPECS specs )
{
    return( (*((specs)->cget_rtn))( specs ) );
}


static void uncget( INTCHAR_TYPE c, PTR_SCNF_SPECS specs )
{
    ((*((specs)->uncget_rtn))( c, specs ));
}


/*
 * get_opt -- get option string for current conversion directive
 *            and fills in the SCNF_SPECS structure.
 *            returns advanced format string pointer.
 */
static const CHAR_TYPE *get_opt( const CHAR_TYPE *opt_str, PTR_SCNF_SPECS specs )
{
    CHAR_TYPE   c;
    int         width;

    specs->assign           = TRUE;
    specs->far_ptr          = 0;
    specs->near_ptr         = 0;
    specs->char_var         = 0;
    specs->short_var        = 0;
    specs->long_var         = 0;
    specs->long_long_var    = 0;
    specs->long_double_var  = 0;
    specs->p_format         = 0;                    /* 21-nov-89 */
    specs->width            = -1;
    if( *opt_str == STRING( '*' ) ) {
        specs->assign = FALSE;
        ++opt_str;
    }
    c = *opt_str;
    if( __F_NAME(isdigit,iswdigit)( (UCHAR_TYPE)c ) ) {
        width = 0;
        do {
            width = width * 10 + ( c - STRING( '0' ) );
            c = *++opt_str;
        } while( __F_NAME(isdigit,iswdigit)( (UCHAR_TYPE)c ) );
        specs->width = width;
    }
    switch( *opt_str ) {
    case STRING( 'N' ):
        specs->near_ptr = 1;
        ++opt_str;
        break;
#if defined( __FAR_SUPPORT__ )
    case STRING( 'F' ):   /* conflicts with ISO-defined 'F' conversion */
        /* fall through */
#endif
    case STRING( 'W' ):
        specs->far_ptr = 1;
        ++opt_str;
        break;
    }
    switch( *opt_str ) {
    case STRING( 'h' ):
        if( opt_str[1] == STRING( 'h' ) ) {
            specs->char_var = 1;
            opt_str += 2;
            break;
        }
        specs->short_var = 1;
        ++opt_str;
        break;
#ifdef JSPEC_IS_LLONG
    JSPEC_CASE_LLONG
        specs->long_long_var = 1;
        ++opt_str;
        break;
#endif
    case STRING( 'l' ):
        if( opt_str[1] == STRING( 'l' ) ) {
            specs->long_long_var = 1;
            opt_str += 2;
            break;
        }
        /* fall through */
    ZSPEC_CASE_LONG
    TSPEC_CASE_LONG
    case STRING( 'w' ):
        specs->long_var = 1;
        ++opt_str;
        break;
    case STRING( 'L' ):
        specs->long_double_var = 1;
        ++opt_str;
        break;
    case STRING( 'I' ):
        if( opt_str[1] == STRING( '6' ) && opt_str[2] == STRING( '4' ) ) {
            specs->long_long_var = 1;
            opt_str += 3;
        }
        break;
#if defined( TSPEC_IS_INT ) || defined( ZSPEC_IS_INT )
    TSPEC_CASE_INT      /* If either 't' or 'z' spec corresponds to 'int',  */
    ZSPEC_CASE_INT      /* we need to parse and ignore the spec.            */
        ++opt_str;
        break;
#endif
    }
    return( opt_str );
}


/*
 * scan_white -- scan white space from input stream
 */
static int scan_white( PTR_SCNF_SPECS specs )
{
    INTCHAR_TYPE    c;
    int             len;

    len = 0;
    for( ;; ) {
        c = cget( specs );
        if( !__F_NAME(isspace,iswspace)( c ) )
            break;
        ++len;
    }
    if( !specs->eoinp )
        uncget( c, specs );
    return( len );
}

/*
 * scan_char -- handles %c and %C
 */
static int scan_char( PTR_SCNF_SPECS specs, va_list *pargs )
{
    int             len;
    int             width;
    FAR_STRING      str;
    INTCHAR_TYPE    c;
    DEFINE_VARS( maxelem, nelem );

    if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
        if( specs->far_ptr ) {
            str = va_arg( *pargs, CHAR_TYPE _WCFAR * );
        } else if( specs->near_ptr ) {
            str = va_arg( *pargs, CHAR_TYPE _WCNEAR * );
        } else {
            str = va_arg( *pargs, CHAR_TYPE * );
        }
#else
        str = va_arg( *pargs, CHAR_TYPE * );
#endif
        GET_MAXELEM( pargs, maxelem );
    }
    len = 0;
    if( (width = specs->width) == -1 )
        width = 1;
    while( width > 0 ) {
        c = cget( specs );
        if( specs->eoinp )
            break;
        ++len;
        --width;
        if( specs->assign ) {
            CHECK_ELEMS( maxelem, nelem, -1 );
#if defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
            if( specs->short_var ) {
                // target is MBCS and source is WIDE
                // MBCS <- WIDE
                char        mbBuf[MB_CUR_MAX];

                if( wctomb( mbBuf, c ) != -1 ) {
                    *PTR_ASCII( str ) = mbBuf[0];
                    str = (FAR_STRING)( PTR_ASCII( str ) + 1 );
                    if( _ismbblead( (unsigned char)mbBuf[0] ) )  {
                        CHECK_ELEMS( maxelem, nelem, -1 );
                        *PTR_ASCII( str ) = mbBuf[1];
                        str = (FAR_STRING)( PTR_ASCII( str ) + 1 );
                    }
                } else {
                    return( 0 );
                }
            } else {
                // target and source are WIDE
                // WIDE <- WIDE
                *str++ = c;
            }
#elif defined( CLIB_USE_MBCS_TRANSLATION )
            if( specs->long_var ) {
                // target is WIDE and source is MBCS
                // WIDE <- MBCS
                wchar_t     wc;
                char        mbBuf[MB_CUR_MAX];

                mbBuf[0] = c;
                if( _ismbblead( (unsigned char)mbBuf[0] ) )
                    mbBuf[1] = cget( specs );
                if( mbtowc( &wc, mbBuf, MB_CUR_MAX ) != -1 ) {
                    *PTR_WIDE( str ) = wc;
                    str = (FAR_STRING)( PTR_WIDE( str ) + 1 );
                } else {
                    return( 0 );
                }
            } else {
                // target and source are SBCS
                // SBCS <- SBCS
                *str++ = c;
            }
#else
            // target and source are same
            // SBCS <- SBCS or WIDE <- WIDE
            *str++ = c;
#endif
        }
    }
    return( len );
}


/*
 * cgetw -- cget which keeps track of field width.
 *          returns INTCHAR_EOF on end of field or end of file.
 */
static INTCHAR_TYPE cgetw( PTR_SCNF_SPECS specs )
{
    INTCHAR_TYPE    c;

    if( specs->width-- == 0 )
        return( INTCHAR_EOF );
    c = cget( specs );
    return( !( specs->eoinp ) ? c : INTCHAR_EOF );
}


/*
 * scan_string -- handles %s and %S
 */
static int scan_string( PTR_SCNF_SPECS specs, va_list *pargs )
{
    INTCHAR_TYPE            c;
    int                     len;
    FAR_ASCII_STRING        str;
    char                    chsize;
    DEFINE_VARS( maxelem, nelem );

    if( specs->long_var ) {         /* %ls or %ws */
        chsize = sizeof( wchar_t );
    } else if( specs->short_var ) { /* %hs */
        chsize = 1;
    } else {                        /* %s */
        chsize = CHARSIZE;
    }
    if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
        if( specs->far_ptr ) {
            str = va_arg( *pargs, char _WCFAR * );
        } else if( specs->near_ptr ) {
            str = va_arg( *pargs, char _WCNEAR * );
        } else {
            str = va_arg( *pargs, char * );
        }
#else
        str = va_arg( *pargs, char * );
#endif
        GET_MAXELEM( pargs, maxelem );
    }
    len = 0;
    for( ;; ) {
        c = cget( specs );
        if( !__F_NAME(isspace,iswspace)( c ) )
            break;
        ++len;
    }
    if( specs->eoinp ) {
        len = 0;            /* since this is eof, no input done */
        goto done;
    }
    if( specs->width-- == 0 )
        goto ugdone;
    do {
        ++len;
        if( specs->assign ) {
            CHECK_ELEMS( maxelem, nelem, -1 );
            if( chsize == 1 ) {
#if defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
                // target is MBCS and source is WIDE
                // WIDE <- MBCS
                char        mbBuf[MB_CUR_MAX];

                if( wctomb( mbBuf, c ) != -1 ) {
                    *str++ = mbBuf[0];
                    if( _ismbblead( (unsigned char)mbBuf[0] ) ) {
                        CHECK_ELEMS( maxelem, nelem, -1 );
                        *str++ = mbBuf[1];
                    }
                } else {
                    return( 0 );
                }
#else
                // target is SBCS and source is WIDE or SBCS
                // SBCS <- SBCS or SBCS <- WIDE
                *str++ = TO_ASCII( c );
#endif
            } else {
#if !defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
                // target is WIDE and source is MBCS
                // WIDE <- MBCS
                wchar_t     wc;
                char        mbBuf[MB_CUR_MAX];

                mbBuf[0] = c;
                if( _ismbblead( (unsigned char)mbBuf[0] ) )
                    mbBuf[1] = cget( specs );
                if( mbtowc( &wc, mbBuf, MB_CUR_MAX ) != -1 ) {
                    *PTR_WIDE( str ) = wc;
                    str = (FAR_ASCII_STRING)( PTR_WIDE( str ) + 1 );
                } else {
                    return( 0 );
                }
#else
                // target is WIDE and source is WIDE or SBCS
                // WIDE <- WIDE or WIDE <- SBCS
                *PTR_WIDE( str ) = TO_WIDE( c );
                str = (FAR_ASCII_STRING)( PTR_WIDE( str ) + 1 );
#endif
            }
        }
        if( (c = cgetw( specs )) == INTCHAR_EOF ) {
            goto done;
        }
    } while( !__F_NAME(isspace,iswspace)( c ) );
ugdone:
    uncget( c, specs );
done:
    if( specs->assign && len > 0 ) {
        CHECK_ELEMS( maxelem, nelem, -1 );
        if( chsize == 1 ) {
            *str = '\0';
        } else {
            *PTR_WIDE( str ) = L'\0';
        }
    }
    return( len );
}


/*
 * report_scan -- handles %n
 */
static void report_scan( PTR_SCNF_SPECS specs, va_list *pargs, int match )
{
    FAR_INT         iptr;

    if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
        if( specs->far_ptr ) {
            iptr = va_arg( *pargs, int _WCFAR * );
        } else if( specs->near_ptr ) {
            iptr = va_arg( *pargs, int _WCNEAR * );
        } else {
            iptr = va_arg( *pargs, int * );
        }
#else
        iptr = va_arg( *pargs, int * );
#endif
        if( specs->char_var ) {
            *((FAR_CHAR)iptr) = match;
        } else if( specs->short_var ) {
            *((FAR_SHORT)iptr) = match;
        } else if( specs->long_var ) {
            *((FAR_LONG)iptr) = match;
        } else if( specs->long_long_var ) {
            *((FAR_INT64)iptr) = match;
        } else {
            *iptr = match;
        }
    }
}

#if !defined( __WIDECHAR__ )

/*
 * makelist -- create scanset for %[ directive.
 *             scanset is stored as 256 bit flags in a 32 byte array.
 */
static const char *makelist( const char *format, unsigned char *scanset )
{
    char    lst_chr;

    memset( scanset, 0, CHARVECTOR_SIZE );
    while( (lst_chr = *format) != '\0' ) {
        ++format;
        if( lst_chr == ']' ) {
            break;
        }
        SETCHARBIT( scanset, lst_chr );
    }
    return( format );
}

#endif


/*
 * scan_arb -- handles %[
 */
static int scan_arb( PTR_SCNF_SPECS specs, va_list *pargs, const CHAR_TYPE **format )
{
    unsigned            width;
    FAR_STRING          str;
    int                 len, not_flag;
    INTCHAR_TYPE        c;
#if defined( __WIDECHAR__ )
    const CHAR_TYPE     *list;
    CHAR_TYPE           ch;
    char                in_list;
#else
    unsigned char       scanset[ CHARVECTOR_SIZE ];
#endif
    DEFINE_VARS( maxelem, nelem );

    if( not_flag = (**format == STRING( '^' )) ) {
        ++(*format);
    }
#if !defined( __WIDECHAR__ )
    *format = makelist( *format, scanset );
#endif
    if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
        if( specs->far_ptr ) {
            str = va_arg( *pargs, CHAR_TYPE _WCFAR * );
        } else if( specs->near_ptr ) {
            str = va_arg( *pargs, CHAR_TYPE _WCNEAR * );
        } else {
            str = va_arg( *pargs, CHAR_TYPE * );
        }
#else
        str = va_arg( *pargs, CHAR_TYPE * );
#endif
        GET_MAXELEM( pargs, maxelem );
    }
    len = 0;
    for( width = specs->width; width > 0; --width ) {
        c = cget( specs );
        if( specs->eoinp )
            break;
#if defined( __WIDECHAR__ )
        list = *format;
        ch = *list;
        in_list = TRUE;
        while( (CHAR_TYPE)c != ch ) {
            list++;
            ch = *list;
            if( ch == STRING( ']' ) ) {
                in_list = FALSE;
                break;
            }
        }
        if( in_list == not_flag ) {
            uncget( c, specs );
            break;
        }
#else
        if( (GETCHARBIT( scanset, c ) == 0) != not_flag ) {
            uncget( c, specs );
            break;
        }
#endif
        if( specs->assign ) {
            CHECK_ELEMS( maxelem, nelem, -1 );
            *str++ = (CHAR_TYPE)c;
        }
        ++len;
    }
    if( specs->assign && len > 0 ) {
        CHECK_ELEMS( maxelem, nelem, -1 );
        *str = NULLCHAR;
    }
#if defined( __WIDECHAR__ )
    while( *(*format)++ != STRING( ']' ) )  /* skip past format specifier */
        ;
#endif
    return( len );
}


/*
 * scan_float -- handles floating point numerical conversion
 *               *** should implement buffer overflow protection ***
 */
static int scan_float( PTR_SCNF_SPECS specs, va_list *pargs )
{
    double          value;
    char            *num_str, buf[80];
    int             len;
    int             pref_len;
    INTCHAR_TYPE    c;
    int             digit_found;
    FAR_FLOAT       fptr;
    char            *p;
    T32             at;
    T32             ft;

    // check and convert float number string (WIDE, MBCS and SBCS) to SBCS string
    num_str = buf;
    pref_len = len = 0;
    for( ;; ) {
        c = cget( specs );
        if( !__F_NAME(isspace,iswspace)( c ) )
            break;
        ++pref_len;
    }
    if( specs->eoinp )
        goto done;
    if( specs->width-- == 0 )
        goto ugdone;
    if( c == STRING( '+' ) || c == STRING( '-' ) ) {
        *num_str++ = TO_ASCII( c );
        ++pref_len;
        if( (c = cgetw( specs )) == INTCHAR_EOF ) {
            goto done;
        }
    }
    if( !__F_NAME(isdigit,iswdigit)( c ) && c != STRING( '.' ) )
        goto ugdone;
    at.uWhole = 0;
    digit_found = FALSE;
    if( __F_NAME(isdigit,iswdigit)( c ) ) {
        digit_found = TRUE;
        do {
            *num_str++ = TO_ASCII( c );
            if( specs->short_var )
                at.wd.hi = at.wd.hi * 10 + c - STRING( '0' );
            ++len;
            if( (c = cgetw( specs )) == INTCHAR_EOF ) {
                goto done;
            }
        } while( __F_NAME(isdigit,iswdigit)( c ) );
    }
    if( c == STRING( '.' ) ) {
        *num_str++ = '.';
        ++len;              /* account for the '.' */
        if( (c = cgetw( specs )) == INTCHAR_EOF )
            goto done;
        if( !digit_found && !__F_NAME(isdigit,iswdigit)( c ) )
            goto ugdone;
        while( __F_NAME(isdigit,iswdigit)( c ) ) {
            *num_str++ = TO_ASCII( c );
            ++len;
            if( (c = cgetw( specs )) == INTCHAR_EOF ) {
                break;
            }
        }
        if( specs->short_var ) {    /* %hf fixed-point format 05-feb-92 */
            ft.uWhole = 0;
            p = num_str;
            for( ;; ) {
                --p;
                if( *p == '.' )
                    break;
                ft.bite.b3 = *p - '0';
                ft.uWhole = ft.uWhole / 10;
            }
            at.wd.lo = ft.wd.lo;
        }
        if( c == INTCHAR_EOF ) {
            goto done;
        }
    }
    if( specs->short_var == 0  &&  (c == STRING( 'e' ) || c == STRING( 'E' )) ) {
        *num_str++ = TO_ASCII( c );
        ++len;
        if( (c = cgetw( specs )) == INTCHAR_EOF )
            goto done;
        if( c == STRING( '+' ) || c == STRING( '-' ) ) {
            *num_str++ = TO_ASCII( c );
            ++len;
            if( (c = cgetw( specs )) == INTCHAR_EOF ) {
                goto done;
            }
        }
        if( !__F_NAME(isdigit,iswdigit)( c ) ) {
            len = 0;                /* fast way to flag error */
        } else {
            do {
                *num_str++ = TO_ASCII( c );
                ++len;
                if( (c = cgetw( specs )) == INTCHAR_EOF ) {
                    goto done;
                }
            } while( __F_NAME(isdigit,iswdigit)( c ) );
        }
    }
ugdone:
    uncget( c, specs );
done:
    // convert SBCS string with float number to value
    if( len > 0 ) {
        len += pref_len;
        if( specs->assign ) {
            *num_str = '\0';
            if( specs->short_var ) {
                if( buf[0] == '-' ) {
                    at.sWhole = - at.sWhole;
                }
            } else {
                EFG_SCANF( buf, (void *)&value );
            }
#if defined( __FAR_SUPPORT__ )
            if( specs->far_ptr ) {
                fptr = va_arg( *pargs, float _WCFAR * );
            } else if( specs->near_ptr ) {
                fptr = va_arg( *pargs, float _WCNEAR * );
            } else {
                fptr = va_arg( *pargs, float * );
            }
#else
            fptr = va_arg( *pargs, float * );
#endif
            if( specs->short_var ) {
                *((FAR_LONG) fptr) = at.uWhole;
            } else if( specs->long_var || specs->long_double_var ) {
                *((FAR_DOUBLE) fptr) = value;
            } else {
                *fptr = value;
            }
        }
    }
    return( len );
}


static int radix_value( INTCHAR_TYPE c )
{
    if( c >= STRING( '0' ) && c <= STRING( '9' ) )
        return( c - STRING( '0' ) );
    c = __F_NAME(tolower,towlower)( c );
    if( c >= STRING( 'a' ) && c <= STRING( 'f' ) )
        return( c - STRING( 'a' ) + 10 );
    return( 16 );
}


/*
 * scan_int -- handles integer numeric conversion
 */
static int scan_int( PTR_SCNF_SPECS specs, va_list *pargs, int base, int sign_flag )
{
    long                value;
    int                 len;
    int                 pref_len;
    INTCHAR_TYPE        c;
    int                 minus;
    int                 digit;
    FAR_INT             iptr;
    unsigned long long  long_value;
    FAR_INT64           llptr;

    long_value = 0;
    value = 0;
    pref_len = len = 0;
    for( ;; ) {
        c = cget( specs );
        if( !__F_NAME(isspace,iswspace)( c ) )
            break;
        ++pref_len;
    }
    if( specs->eoinp )
        goto done;
    if( specs->width-- == 0 )
        goto ugdone;
    minus = FALSE;
    if( sign_flag ) {
        switch( c ) {
        case STRING( '-' ):
            minus = TRUE;
            // fall down
        case STRING( '+' ):
            ++pref_len;
            if( (c = cgetw( specs )) == INTCHAR_EOF ) {
                goto done;
            }
            break;
        }
    }
    if( base == 0 ) {
        if( c == STRING( '0' ) ) {
            len = 1;
            if( (c = cgetw( specs )) == INTCHAR_EOF )
                goto done;
            if( c == STRING( 'x' ) || c == STRING( 'X' ) ) {
                len = 0;
                ++pref_len;         /* for the '0' */
                ++pref_len;         /* for the 'x' */
                if( (c = cgetw( specs )) == INTCHAR_EOF )
                    goto done;
                base = 16;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if( base == 16 ) {
        if( c == STRING( '0' ) ) {
            len = 1;
            if( (c = cgetw( specs )) == INTCHAR_EOF )
                goto done;
            if( c == STRING( 'x' ) || c == STRING( 'X' ) ) {
                len = 0;
                ++pref_len;         /* for the '0' */
                ++pref_len;         /* for the 'x' */
                if( (c = cgetw( specs )) == INTCHAR_EOF ) {
                    goto done;
                }
            }
        }
    }
    if( specs->long_long_var ) {
        for( ;; ) {
            digit = radix_value( c );
            if( digit >= base )
                break;
            long_value = long_value * base + digit;
            ++len;
            if( (c = cgetw( specs )) == INTCHAR_EOF ) {
                goto done;
            }
        }
        if( c == STRING( ':' ) && specs->p_format ) {
            for( ;; ) {
                ++len;
                if( (c = cgetw( specs )) == INTCHAR_EOF )
                    goto done;
                digit = radix_value( c );
                if( digit >= base )
                    break;
                long_value = long_value * base + digit;
            }
        }
    } else {
        for( ;; ) {
            digit = radix_value( c );
            if( digit >= base )
                break;
            value = value * base + digit;
            ++len;
            if( (c = cgetw( specs )) == INTCHAR_EOF ) {
                goto done;
            }
        }
        if( c == STRING( ':' ) && specs->p_format ) {
            for( ;; ) {
                ++len;
                if( (c = cgetw( specs )) == INTCHAR_EOF )
                    goto done;
                digit = radix_value( c );
                if( digit >= base )
                    break;
                value = value * base + digit;
            }
        }
    }
ugdone:
    uncget( c, specs );
done:
    if( specs->long_long_var ) {
        if( minus ) {
            long_value =- long_value;
        }
        if( len > 0 ) {
            len += pref_len;
            if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
                if( specs->far_ptr ) {
                    llptr = va_arg( *pargs, unsigned long long _WCFAR * );
                } else if( specs->near_ptr ) {
                    llptr = va_arg( *pargs, unsigned long long _WCNEAR * );
                } else {
                    llptr = va_arg( *pargs, unsigned long long * );
                }
#else
                llptr = va_arg( *pargs, unsigned long long * );
#endif
                *llptr = long_value;
            }
        }
    } else {
        if( minus ) {
            value = -value;
        }
        if( len > 0 ) {
            len += pref_len;
            if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
                if( specs->far_ptr ) {
                    iptr = va_arg( *pargs, int _WCFAR * );
                } else if( specs->near_ptr ) {
                    iptr = va_arg( *pargs, int _WCNEAR * );
                } else {
                    iptr = va_arg( *pargs, int * );
                }
#else
                iptr = va_arg( *pargs, int * );
#endif
                if( specs->char_var ) {
                    *((FAR_CHAR)iptr) = value;
                } else if( specs->short_var ) {
                    *((FAR_SHORT)iptr) = value;
                } else if( specs->long_var ) {
                    *((FAR_LONG)iptr) = value;
                } else {
                    *iptr = value;
                }
            }
        }
    }
    return( len );
}


#ifdef __STDC_WANT_LIB_EXT1__

/*
 * null_arg -- check for a null pointer passed in arguments
 */
static int null_arg( PTR_SCNF_SPECS specs, va_list *pargs )
{
    FAR_STRING      str;
    va_list         args_copy;

    /* unused parameters */ (void)specs;

    str = NULL;
    va_copy( args_copy, *pargs );
#if defined( __FAR_SUPPORT__ )
    if( specs->far_ptr ) {
        str = va_arg( args_copy, CHAR_TYPE _WCFAR * );
    } else if( specs->near_ptr ) {
        CHAR_TYPE _WCNEAR   *ptr;

        ptr = va_arg( args_copy, CHAR_TYPE _WCNEAR * );
        /* The following should work:
         *
         * str = (ptr == NULL) ? (void _WCFAR *)NULL : ptr;
         *
         * but doesn't due to a bug in C compiler introduced in
         * 11.0 and fixe in OW 1.4. Ternary operator may be used
         * when building with OW 1.5. See also similar code in prtf.c.
         */
        if( ptr == NULL ) {
            str = NULL;
        } else {
            str = ptr;
        }
    } else {
        CHAR_TYPE   *ptr;

        ptr = va_arg( args_copy, CHAR_TYPE * );
        if( ptr == NULL ) {
            str = NULL;
        } else {
            str = ptr;
        }
    }
#else
    str = va_arg( args_copy, CHAR_TYPE * );
#endif
    va_end( args_copy );
    return( str == NULL ? 1 : 0 );
}

#endif


#ifdef __STDC_WANT_LIB_EXT1__
int __F_NAME(__scnf_s,__wscnf_s)( PTR_SCNF_SPECS specs, const CHAR_TYPE *format, const char **msg, va_list args )
#else
int __F_NAME(__scnf,__wscnf)( PTR_SCNF_SPECS specs, const CHAR_TYPE *format, va_list args )
#endif
{
    int             char_match;
    int             items_converted;
    int             items_assigned;
    int             match_len;
    INTCHAR_TYPE    c;
    CHAR_TYPE       fmt_chr;

    char_match = items_assigned = items_converted = 0;
    specs->eoinp = FALSE;

    for( fmt_chr = *format++; fmt_chr != NULLCHAR; fmt_chr = *format++ ) {
        if( __F_NAME(isspace,iswspace)( (UCHAR_TYPE)fmt_chr ) ) {
            char_match += scan_white( specs );
        } else if( fmt_chr != STRING( '%' ) ) {
            c = cget( specs );
            if( (CHAR_TYPE)c != fmt_chr ) {
                if( !specs->eoinp )
                    uncget( c, specs );
                break;
            }
            ++char_match;                           /* 27-oct-88 */
        } else {            /* fmt_chr == '%' */
            format = get_opt( format, specs );
            fmt_chr = *format;
            if( fmt_chr != NULLCHAR )
                ++format;
#ifdef __STDC_WANT_LIB_EXT1__
            if( fmt_chr != STRING( '%' ) ) {
                /* The '%' specifier is the only one not expecting pointer arg */
                if( specs->assign && null_arg( specs, &args ) ) {
                    *msg = "%ptr -> NULL";
                    return( EOF );
                }
            }
#endif
            switch( fmt_chr ) {
            case STRING( 'd' ):
                match_len = scan_int( specs, &args, 10, TRUE );
                goto check_match;
            case STRING( 'i' ):
                match_len = scan_int( specs, &args, 0, TRUE );
                goto check_match;
            case STRING( 'o' ):
                match_len = scan_int( specs, &args, 8, TRUE );
                goto check_match;
            case STRING( 'u' ):
                match_len = scan_int( specs, &args, 10, TRUE );
                goto check_match;
            case STRING( 'p' ):
#if defined( __BIG_DATA__ )
                specs->long_var = 1;    /* indicate far pointer */
                specs->p_format = 1;    /* ... */
#endif
                // fall through
            case STRING( 'x' ):
            case STRING( 'X' ):
                match_len = scan_int( specs, &args, 16, TRUE );
                goto check_match;
            case STRING( 'e' ):
            case STRING( 'E' ):
            case STRING( 'f' ):
            case STRING( 'F' ):
            case STRING( 'g' ):
            case STRING( 'G' ):
                match_len = scan_float( specs, &args );
                goto check_match;
#if !defined(__WIDECHAR__) && !defined(__NETWARE__)
            case STRING( 'S' ):
                specs->long_var = 1;
                /* fall through to %s handler */
#endif
            case STRING( 's' ):
                match_len = scan_string( specs, &args );
                goto check_match;
            case STRING( '[' ):
                match_len = scan_arb( specs, &args, &format );
                goto check_match;
#if !defined(__WIDECHAR__) && !defined(__NETWARE__)
            case STRING( 'C' ):
                specs->long_var = 1;
                /* fall through to %c handler */
#endif
            case STRING( 'c' ):
                match_len = scan_char( specs, &args );
check_match:
                if( match_len > 0 ) {
                    char_match += match_len;
                    ++items_converted;
                    if( specs->assign ) {
                        ++items_assigned;
                    }
                } else {
#ifdef __STDC_WANT_LIB_EXT1__
                    if( match_len < 0 ) {
                        /* Matching failure caused by insufficient space in output
                         * is not input failure, hence we won't return EOF regardless
                         * of specs->eoinp state.
                         */
                        ++items_converted;
                    }
#endif
                    goto fail;
                }
                break;
            case STRING( 'n' ):
                report_scan( specs, &args, char_match );
                break;
            case STRING( '%' ):
                c = cget( specs );
                if( (CHAR_TYPE)c != STRING( '%' ) ) {
                    if( !specs->eoinp )
                        uncget( c, specs );
                    goto fail;
                } else {
                    char_match += 1;
                }
                break;
            }
        }
        if( specs->eoinp ) {
            while( *format == STRING( '%' ) ) {
                ++format;
                format = get_opt( format, specs );
                if( *format == STRING( 'n' ) ) {
                    ++format;
#ifdef __STDC_WANT_LIB_EXT1__
                    if( specs->assign && null_arg( specs, &args ) ) {
                        *msg = "%ptr -> NULL";
                        return( EOF );
                    }
#endif
                    report_scan( specs, &args, char_match );
                } else {
                    break;
                }
            }
            break;
        }
    }

fail:
    if( items_converted == 0 && specs->eoinp )
        return( EOF );
    return( items_assigned );
}

/****************************************************************************
*
*                            Open Watcom Project
*
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


#define __LONG_LONG_SUPPORT__

#if !defined( __NETWARE__ ) && !defined( __UNIX__ ) && !defined( __RDOS__ ) && !defined( __RDOSDEV__ )
    #define USE_MBCS_TRANSLATION
#endif

#include "variety.h"
#ifdef SAFE_SCANF
    #include "saferlib.h"
#endif
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#include <stdarg.h>
#include "scanf.h"
#include "prtscncf.h"
#include "fixpoint.h"
#include "ftos.h"
#include "farsupp.h"
#include "myvalist.h"
#if defined( __WIDECHAR__ ) || defined( USE_MBCS_TRANSLATION )
    #include <mbstring.h>
#endif
#include "setbits.h"

#define TRUE    1
#define FALSE   0

#define STOP_CHR 0xFFFFFFFF

#define EFG_SCANF (*__EFG_scanf)

/* internal file/string get, unget routines */
#ifdef __WINDOWS_386__
    #ifdef __SW_3S
        #pragma aux cget modify [eax edx ecx fs gs]
        #pragma aux uncget modify [eax edx ecx fs gs]
    #else
        #pragma aux cget modify [fs gs]
        #pragma aux uncget modify [fs gs]
    #endif
#endif

#if defined(__HUGE__)
    #define SCNF_FAR    _WCFAR
#else
    #define SCNF_FAR
#endif


/* Macros to reduce the already large number of ifdefs in the code */
#ifdef SAFE_SCANF

    #define GET_MAXELEM(x)      x = va_arg( arg->v, size_t )
    #define DEFINE_VARS(x,y)    size_t x, y = 0
    #define CHECK_ELEMS(x,y,z)  if( x < ++y ) return( z )
#else

    #define GET_MAXELEM(x)
    #define DEFINE_VARS(x,y)
    #define CHECK_ELEMS(x,y,z)

#endif

static int cget( PTR_SCNF_SPECS specs )
{
    return( (*((specs)->cget_rtn))( specs ) );
}


static void uncget( int c, PTR_SCNF_SPECS specs )
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
    int     c, width;

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
    if( *opt_str == '*' ) {
        specs->assign = FALSE;
        ++opt_str;
    }
    c = *opt_str;
    if( __F_NAME(isdigit,iswdigit)( c ) ) {
        width = 0;
        do {
            width *= 10;
            width += ( c - '0' );
            c = *++opt_str;
        } while( __F_NAME(isdigit,iswdigit)( c ) );
        specs->width = width;
    }
    switch( *opt_str ) {
    case 'N':
        specs->near_ptr = 1;
        ++opt_str;
        break;
#if defined( __FAR_SUPPORT__ )
    case 'F':   /* conflicts with ISO-defined 'F' conversion */
        /* fall through */
#endif
    case 'W':
        specs->far_ptr = 1;
        ++opt_str;
        break;
    }
    switch( *opt_str ) {
    case 'h':
        if( opt_str[1] == 'h' ) {
            specs->char_var = 1;
            opt_str += 2;
            break;
        }
        specs->short_var = 1;
        ++opt_str;
        break;
    case 'l':
#if defined( __LONG_LONG_SUPPORT__ )
        if( opt_str[1] == 'l' ) {
            specs->long_long_var = 1;
            opt_str += 2;
            break;
        }
#endif
        /* fall through */
    ZSPEC_CASE_LONG
    TSPEC_CASE_LONG
    case 'w':
        specs->long_var = 1;
        ++opt_str;
        break;
#if defined( __LONG_LONG_SUPPORT__ )
    JSPEC_CASE_LLONG
        /* fall through */
#endif
    case 'L':
        specs->long_double_var = 1;
        specs->long_long_var = 1;
        ++opt_str;
        break;
#if defined( __LONG_LONG_SUPPORT__ )
    case 'I':
        if( opt_str[1] == '6' && opt_str[2] == '4' ) {
            specs->long_long_var = 1;
            opt_str += 3;
        }
        break;
#endif
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
    int     c, len;

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
static int scan_char( PTR_SCNF_SPECS specs, my_va_list *arg )
{
    int             len;
    int             width;
    FAR_STRING      str;
    int             c;
    DEFINE_VARS( maxelem, nelem );

    if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
        if( specs->far_ptr ) {
            str = va_arg( arg->v, CHAR_TYPE _WCFAR * );
        } else if( specs->near_ptr ) {
            str = va_arg( arg->v, CHAR_TYPE _WCNEAR * );
        } else {
            str = va_arg( arg->v, CHAR_TYPE * );
        }
#else
        str = va_arg( arg->v, CHAR_TYPE * );
#endif
        GET_MAXELEM( maxelem );
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
#if defined( __WIDECHAR__ ) && defined( USE_MBCS_TRANSLATION )
            if( specs->short_var ) {
                char        mbBuf[MB_CUR_MAX];

                if( wctomb( mbBuf, c ) != -1 ) {
                    *(FAR_ASCII_STRING)str = mbBuf[0];
                    str = (FAR_STRING) ( (FAR_ASCII_STRING)str + 1 );
                    if( _ismbblead( mbBuf[0] ) )  {
                        CHECK_ELEMS( maxelem, nelem, -1 );
                        *(FAR_ASCII_STRING)str = mbBuf[1];
                        str = (FAR_STRING) ( (FAR_ASCII_STRING)str + 1 );
                    }
                } else {
                    return( 0 );
                }
            } else {
                *str++ = c;
            }
#elif defined( USE_MBCS_TRANSLATION )
            if( specs->long_var ) {
                wchar_t     wc;
                char        mbBuf[MB_CUR_MAX];

                mbBuf[0] = c;
                if( _ismbblead( mbBuf[0] ) )
                    mbBuf[1] = cget( specs );
                if( mbtowc( &wc, mbBuf, MB_CUR_MAX ) != -1 ) {
                    *(FAR_UNI_STRING)str = wc;
                    str = (FAR_STRING) ( (FAR_UNI_STRING)str + 1 );
                } else {
                    return( 0 );
                }
            } else {
                *str++ = c;
            }
#else
            *str++ = c;
#endif
        }
    }
    return( len );
}


/*
 * cgetw -- cget which keeps track of field width.
 *          returns STOP_CHR on end of field or end of file.
 */
static long cgetw( PTR_SCNF_SPECS specs )
{
    int     c;

    if( specs->width-- == 0 )
        return( STOP_CHR );
    c = cget( specs );
    return( !( specs->eoinp ) ? c : STOP_CHR );
}


/*
 * scan_string -- handles %s and %S
 */
static int scan_string( PTR_SCNF_SPECS specs, my_va_list *arg )
{
    int                     c;
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
            str = va_arg( arg->v, char _WCFAR * );
        } else if( specs->near_ptr ) {
            str = va_arg( arg->v, char _WCNEAR * );
        } else {
            str = va_arg( arg->v, char * );
        }
#else
        str = va_arg( arg->v, char * );
#endif
        GET_MAXELEM( maxelem );
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
#if defined( __WIDECHAR__ ) && defined( USE_MBCS_TRANSLATION )
                char        mbBuf[MB_CUR_MAX];

                if( wctomb( mbBuf, c ) != -1 ) {
                    *(FAR_ASCII_STRING)str = mbBuf[0];
                    if( _ismbblead( mbBuf[0] ) ) {
                        CHECK_ELEMS( maxelem, nelem, -1 );
                        str++;
                        *(FAR_ASCII_STRING)str = mbBuf[1];
                    }
                } else {
                    return( 0 );
                }
#else
                *str = c;
#endif
            } else {
#if !defined( __WIDECHAR__ ) && defined( USE_MBCS_TRANSLATION )
                wchar_t     wc;
                char        mbBuf[MB_CUR_MAX];

                mbBuf[0] = c;
                if( _ismbblead( mbBuf[0] ) )
                    mbBuf[1] = cget( specs );
                if( mbtowc( &wc, mbBuf, MB_CUR_MAX ) != -1 ) {
                    *(FAR_UNI_STRING)str = wc;
                } else {
                    return( 0 );
                }
#else
                *(FAR_UNI_STRING)str = c;
#endif
            }
            str += chsize;
        }
        if( (c = cgetw( specs )) == STOP_CHR ) {
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
            *(FAR_UNI_STRING)str = 0;
        }
    }
    return( len );
}


/*
 * report_scan -- handles %n
 */
static void report_scan( PTR_SCNF_SPECS specs, my_va_list *arg, int match )
{
    FAR_INT         iptr;

    if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
        if( specs->far_ptr ) {
            iptr = va_arg( arg->v, int _WCFAR * );
        } else if( specs->near_ptr ) {
            iptr = va_arg( arg->v, int _WCNEAR * );
        } else {
            iptr = va_arg( arg->v, int * );
        }
#else
        iptr = va_arg( arg->v, int * );
#endif
        if( specs->char_var ) {
            *((FAR_CHAR)iptr) = match;
        } else if( specs->short_var ) {
            *((FAR_SHORT)iptr) = match;
        } else if( specs->long_var ) {
            *((FAR_LONG)iptr) = match;
#if defined( __LONG_LONG_SUPPORT__ )
        } else if( specs->long_long_var ) {
            *((FAR_INT64)iptr) = match;
#endif
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
    if( (lst_chr = *format++) == '\0' )
        return( format );
    do {
        SETCHARBIT( scanset, lst_chr );
        if( (lst_chr = *format) == '\0' )
            break;
        ++format;
    } while( lst_chr != ']' );
    return( format );
}
#endif


/*
 * scan_arb -- handles %[
 */
static int scan_arb( PTR_SCNF_SPECS specs, my_va_list *arg, const CHAR_TYPE **format )
{
    unsigned            width;
    FAR_STRING          str;
    int                 len, c, not_flag;
#if defined( __WIDECHAR__ )
    const CHAR_TYPE     *list;
    CHAR_TYPE           ch;
    char                in_list;
#else
    unsigned char       scanset[ CHARVECTOR_SIZE ];
#endif
    DEFINE_VARS( maxelem, nelem );

    if( not_flag = (**format == '^') ) {
        ++(*format);
    }
#if !defined( __WIDECHAR__ )
    *format = makelist( *format, scanset );
#endif
    if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
        if( specs->far_ptr ) {
            str = va_arg( arg->v, CHAR_TYPE _WCFAR * );
        } else if( specs->near_ptr ) {
            str = va_arg( arg->v, CHAR_TYPE _WCNEAR * );
        } else {
            str = va_arg( arg->v, CHAR_TYPE * );
        }
#else
        str = va_arg( arg->v, CHAR_TYPE * );
#endif
        GET_MAXELEM( maxelem );
    }
    len = 0;
    width = specs->width;
    while( width > 0 ) {
        c = cget( specs );
        if( specs->eoinp )
            break;
#if defined( __WIDECHAR__ )
        list = *format;
        ch = *list;
        in_list = TRUE;
        while( c != ch ) {
            list++;
            ch = *list;
            if( ch == ']' ) {
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
        ++len;
        --width;
        if( specs->assign ) {
            CHECK_ELEMS( maxelem, nelem, -1 );
            *str++ = c;
        }
    }
    if( specs->assign && len > 0 ) {
        CHECK_ELEMS( maxelem, nelem, -1 );
        *str = '\0';
    }
#if defined( __WIDECHAR__ )
    while( *(*format)++ != ']' )  /* skip past format specifier */
        ;
#endif
    return( len );
}


/*
 * scan_float -- handles floating point numerical conversion
 *               *** should implement buffer overflow protection ***
 */
static int scan_float( PTR_SCNF_SPECS specs, my_va_list *arg )
{
    double      value;
    char        *num_str, buf[80];
    int         len;
    int         pref_len;
    long        c;
    int         digit_found;
    FAR_FLOAT   fptr;
    char        *p;
    T32         at;
    T32         ft;

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
    if( c == '+' || c == '-' ) {
        *num_str++ = c;
        ++pref_len;
        if( (c = cgetw( specs )) == STOP_CHR ) {
            goto done;
        }
    }
    if( !__F_NAME(isdigit,iswdigit)( c ) && c != '.' )
        goto ugdone;
    at.uWhole = 0;
    digit_found = FALSE;
    if( __F_NAME(isdigit,iswdigit)( c ) ) {
        digit_found = TRUE;
        do {
            *num_str++ = c;
            if( specs->short_var )
                at.wd.hi = at.wd.hi * 10 + c - '0';
            ++len;
            if( (c = cgetw( specs )) == STOP_CHR ) {
                goto done;
            }
        } while( __F_NAME(isdigit,iswdigit)( c ) );
    }
    if( c == '.' ) {
        *num_str++ = c;
        ++len;              /* account for the '.' */
        if( (c = cgetw( specs )) == STOP_CHR )
            goto done;
        if( !digit_found && !__F_NAME(isdigit,iswdigit)(c) )
            goto ugdone;
        while( __F_NAME(isdigit,iswdigit)( c ) ) {
            *num_str++ = c;
            ++len;
            if( (c = cgetw( specs )) == STOP_CHR ) {
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
        if( c == STOP_CHR ) {
            goto done;
        }
    }
    if( specs->short_var == 0  &&  (c == 'e' || c == 'E') ) {
        *num_str++ = c;
        ++len;
        if( (c = cgetw( specs )) == STOP_CHR )
            goto done;
        if( c == '+' || c == '-' ) {
            *num_str++ = c;
            ++len;
            if( (c = cgetw( specs )) == STOP_CHR ) {
                goto done;
            }
        }
        if( !__F_NAME(isdigit,iswdigit)( c ) ) {
            len = 0;                /* fast way to flag error */
        } else {
            do {
                *num_str++ = c;
                ++len;
                if( (c = cgetw( specs )) == STOP_CHR ) {
                    goto done;
                }
            } while( __F_NAME(isdigit,iswdigit)( c ) );
        }
    }
ugdone:
    uncget( (int)c, specs );
done:
    if( len > 0 ) {
        len += pref_len;
        if( specs->assign ) {
            *num_str = NULLCHAR;
            if( specs->short_var ) {
                if( buf[0] == '-' ) {
                    at.sWhole = - at.sWhole;
                }
            } else {
                EFG_SCANF( buf, (void *)&value );      /* 27-mar-90 */
            }
#if defined( __FAR_SUPPORT__ )
            if( specs->far_ptr ) {
                fptr = va_arg( arg->v, float _WCFAR * );
            } else if( specs->near_ptr ) {
                fptr = va_arg( arg->v, float _WCNEAR * );
            } else {
                fptr = va_arg( arg->v, float * );
            }
#else
            fptr = va_arg( arg->v, float * );
#endif
            if( specs->short_var ) {                        /* 05-feb-92 */
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


static int radix_value( int c )
{
    if( c >= '0' && c <= '9' )
        return( c - '0' );
    c = __F_NAME(tolower,towlower)( c );
    if( c >= 'a' && c <= 'f' )
        return( c - 'a' + 10 );
    return( 16 );
}


/*
 * scan_int -- handles integer numeric conversion
 */
static int scan_int( PTR_SCNF_SPECS specs, my_va_list *arg, int base, int sign_flag )
{
    long                value;
    int                 len;
    int                 pref_len;
    int                 c;
    int                 sign;
    int                 digit;
    FAR_INT             iptr;
#if defined( __LONG_LONG_SUPPORT__ )
    unsigned long long  long_value;
    FAR_INT64           llptr;

    long_value = 0;
#endif

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
    sign = '+';
    if( sign_flag && (c == '+' || c == '-') ) {
        sign = c;
        ++pref_len;
        if( (c = cgetw( specs )) == STOP_CHR ) {
            goto done;
        }
    }
    if( base == 0 ) {
        if( c == '0' ) {
            len = 1;
            if( (c = cgetw( specs )) == STOP_CHR )
                goto done;
            if( c == 'x' || c == 'X' ) {
                len = 0;
                ++pref_len;         /* for the '0' */
                ++pref_len;         /* for the 'x' */
                if( (c = cgetw( specs )) == STOP_CHR )
                    goto done;
                base = 16;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if( base == 16 ) {
        if( c == '0' ) {
            len = 1;
            if( (c = cgetw( specs )) == STOP_CHR )
                goto done;
            if( c == 'x'  ||  c == 'X' ) {
                len = 0;
                ++pref_len;         /* for the '0' */
                ++pref_len;         /* for the 'x' */
                if( (c = cgetw( specs )) == STOP_CHR ) {
                    goto done;
                }
            }
        }
    }
#if defined( __LONG_LONG_SUPPORT__ )
    if( specs->long_long_var ) {
        for( ;; ) {
            digit = radix_value( c );
            if( digit >= base )
                break;
            long_value = long_value * base + digit;
            ++len;
            if( (c = cgetw( specs )) == STOP_CHR ) {
                goto done;
            }
        }
        if( c == ':'  &&  specs->p_format ) {
            for( ;; ) {
                ++len;
                if( (c = cgetw( specs )) == STOP_CHR )
                    goto done;
                digit = radix_value( c );
                if( digit >= base )
                    break;
                long_value = long_value * base + digit;
            }
        }
    } else
#endif
    {
        for( ;; ) {
            digit = radix_value( c );
            if( digit >= base )
                break;
            value = value * base + digit;
            ++len;
            if( (c = cgetw( specs )) == STOP_CHR ) {
                goto done;
            }
        }
        if( c == ':'  &&  specs->p_format ) {
            for( ;; ) {
                ++len;
                if( (c = cgetw( specs )) == STOP_CHR )
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
#if defined( __LONG_LONG_SUPPORT__ )
    if( specs->long_long_var ) {
        if( sign == '-' ) {
            long_value =- long_value;
        }
        if( len > 0 ) {
            len += pref_len;
            if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
                if( specs->far_ptr ) {
                    llptr = va_arg( arg->v, unsigned long long _WCFAR * );
                } else if( specs->near_ptr ) {
                    llptr = va_arg( arg->v, unsigned long long _WCNEAR * );
                } else {
                    llptr = va_arg( arg->v, unsigned long long * );
                }
#else
                llptr = va_arg( arg->v, unsigned long long * );
#endif
                *llptr = long_value;
            }
        }
    } else
#endif
    {
        if( sign == '-' ) {
            value = -value;
        }
        if( len > 0 ) {
            len += pref_len;
            if( specs->assign ) {
#if defined( __FAR_SUPPORT__ )
                if( specs->far_ptr ) {
                    iptr = va_arg( arg->v, int _WCFAR * );
                } else if( specs->near_ptr ) {
                    iptr = va_arg( arg->v, int _WCNEAR * );
                } else {
                    iptr = va_arg( arg->v, int * );
                }
#else
                iptr = va_arg( arg->v, int * );
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


#ifdef SAFE_SCANF

/*
 * null_arg -- check for a null pointer passed in arguments
 */
static int null_arg( PTR_SCNF_SPECS specs, my_va_list *arg )
{
    FAR_STRING      str = NULL;
    va_list         args_copy;

    va_copy( args_copy, arg->v );
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


#ifdef SAFE_SCANF
int __F_NAME(__scnf_s,__wscnf_s)( PTR_SCNF_SPECS specs, const CHAR_TYPE *format, const char **msg, va_list args )
#else
int __F_NAME(__scnf,__wscnf)( PTR_SCNF_SPECS specs, const CHAR_TYPE *format, va_list args )
#endif
{
    int         char_match;
    int         items_converted;
    int         items_assigned;
    int         match_len;
    int         c;
    int         fmt_chr;
    my_va_list  margs;

    margs = MY_VA_LIST( args );

    char_match = items_assigned = items_converted = 0;
    specs->eoinp = 0;

    while( (fmt_chr = *format++) != NULLCHAR ) {
        if( __F_NAME(isspace,iswspace)( fmt_chr ) ) {
            char_match += scan_white( specs );
        } else if( fmt_chr != '%' ) {
            if( (c = cget( specs )) != fmt_chr ) {
                if( !specs->eoinp )
                    uncget( c, specs );
                break;
            }
            ++char_match;                           /* 27-oct-88 */
        } else {            /* fmt_chr == '%' */
            format = get_opt( format, specs );
            if( (fmt_chr = *format) != NULLCHAR )
                ++format;
#ifdef SAFE_SCANF
            if( fmt_chr != '%' ) {
                /* The '%' specifier is the only one not expecting pointer arg */
                if( specs->assign && null_arg( specs, &margs ) ) {
                    *msg = "%ptr -> NULL";
                    return( EOF );
                }
            }
#endif
            switch( fmt_chr ) {
            case 'd':
                match_len = scan_int( specs, &margs, 10, TRUE );
                goto check_match;
            case 'i':
                match_len = scan_int( specs, &margs, 0, TRUE );
                goto check_match;
            case 'o':
                match_len = scan_int( specs, &margs, 8, TRUE );
                goto check_match;
            case 'u':
                match_len = scan_int( specs, &margs, 10, TRUE );
                goto check_match;
            case 'p':
#if defined( __BIG_DATA__ )
                specs->long_var = 1;    /* indicate far pointer */
                specs->p_format = 1;    /* ... */
#endif
                // fall through
            case 'x':
            case 'X':
                match_len = scan_int( specs, &margs, 16, TRUE );
                goto check_match;
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
                match_len = scan_float( specs, &margs );
                goto check_match;
#if !defined(__WIDECHAR__) && !defined(__NETWARE__)
            case 'S':
                specs->long_var = 1;
                /* fall through to %s handler */
#endif
            case 's':
                match_len = scan_string( specs, &margs );
                goto check_match;
            case '[':
                match_len = scan_arb( specs, &margs, &format );
                goto check_match;
#if !defined(__WIDECHAR__) && !defined(__NETWARE__)
            case 'C':
                specs->long_var = 1;
                /* fall through to %c handler */
#endif
            case 'c':
                match_len = scan_char( specs, &margs );
check_match:
                if( match_len > 0 ) {
                    char_match += match_len;
                    ++items_converted;
                    if( specs->assign ) {
                        ++items_assigned;
                    }
                } else {
#ifdef SAFE_SCANF
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
            case 'n':
                report_scan( specs, &margs, char_match );
                break;
            case '%':
                if( (c = cget( specs )) != '%' ) {
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
            while( *format == '%' ) {
                ++format;
                format = get_opt( format, specs );
                if( *format == 'n' ) {
                    ++format;
#ifdef SAFE_SCANF
                    if( specs->assign && null_arg( specs, &margs ) ) {
                        *msg = "%ptr -> NULL";
                        return( EOF );
                    }
#endif
                    report_scan( specs, &margs, char_match );
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

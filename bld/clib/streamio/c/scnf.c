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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#define __LONG_LONG_SUPPORT__

#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "scanf.h"
#include "fixpoint.h"
#include "ftos.h"
#if defined(__LONG_LONG_SUPPORT__)
    #include "clibi64.h"
#endif
#include "farsupp.h"
#include "myvalist.h"
#ifndef __NETWARE__
    #include <mbstring.h>
#endif

#define TRUE    1
#define FALSE   0

#if !defined(__NETWARE__) && !defined(__QNX__)
    #define USE_MBCS_TRANSLATION
#endif

#define STOP_CHR 0xFFFFFFFF

#if defined(__PENPOINT__)
  #define EFG_SCANF __cnvs2d
#else
  #define EFG_SCANF (*__EFG_scanf)
#endif

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
static int cget(PTR_SCNF_SPECS specs)
{
        return ((*((specs)->cget_rtn))(specs));
} /* cget() */


static void uncget(int c, PTR_SCNF_SPECS specs)
{
        ((*((specs)->uncget_rtn))(c, specs));
} /* uncget() */


#if defined(__HUGE__)
    #define SCNF_FAR    _WCFAR
#else
    #define SCNF_FAR
#endif

/* non-int static routines */
static const CHAR_TYPE *get_opt();
static void report_scan();
static long cgetw( PTR_SCNF_SPECS );


int __F_NAME(__scnf,__wscnf)( specs, format, args )
        PTR_SCNF_SPECS specs;
        const CHAR_TYPE *format;
        va_list args;
{
        int char_match;
        int item_match;
        int match_len;
        int c;
        int fmt_chr;
        my_va_list margs;

        margs = MY_VA_LIST( args );

        char_match = item_match = 0;
        specs->eoinp = 0;

        while( (fmt_chr = *format++) != NULLCHAR ) {
            if( __F_NAME(isspace,iswspace)( fmt_chr ) ) {
                char_match += scan_white( specs );
            } else if( fmt_chr != '%' ) {
                if( (c = cget( specs )) != fmt_chr ) {
                    if( !specs->eoinp )  uncget( c, specs );
                    break;
                }
                ++char_match;                           /* 27-oct-88 */
            } else {            /* fmt_chr == '%' */
                format = get_opt( format, specs );
                if( (fmt_chr = *format) != NULLCHAR )  ++format;
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
                    #if defined(__BIG_DATA__)
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
                        /* only increment count if value assigned 17-jun-88 */
                        if( specs->assign ) ++item_match;
                    } else
                        goto fail;
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
                if( *format == '%' ) {                          /* 26-mar-91 */
                    ++format;
                    format = get_opt( format, specs );
                    if( *format == 'n' ) {
                        report_scan( specs, &margs, char_match );
                    }
                }
                break;
            }
        }

   fail:
        if( item_match == 0 && specs->eoinp )   return( EOF );
        return( item_match );
    }

/*
 * get_opt -- get option string for current conversion directive
 *            and fills in the SCNF_SPECS structure.
 *            returns advanced format string pointer.
 */
static const CHAR_TYPE *get_opt( opt_str, specs )
        const CHAR_TYPE *opt_str;
        PTR_SCNF_SPECS specs;
    {
        register int c, width;

        specs->assign = TRUE;
        specs->far_ptr = 0;
        specs->near_ptr = 0;
        specs->short_var = 0;
        specs->long_var = 0;
        specs->long_double_var = 0;
        specs->p_format = 0;                    /* 21-nov-89 */
        specs->width  = -1;
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
        #if defined( __FAR_SUPPORT__ )
            if( *opt_str == 'N' ) {
                specs->near_ptr = 1;
                ++opt_str;
            } else if( *opt_str == 'F' ) {
                specs->far_ptr = 1;
                ++opt_str;
            }
        #endif
        switch( *opt_str ) {
        case 'h':
            specs->short_var = 1;
            ++opt_str;
            break;
        case 'l':
        case 'w':
            specs->long_var = 1;
            ++opt_str;
            break;
        case 'L':
            specs->long_double_var = 1;
            ++opt_str;
            break;
        #if defined(__LONG_LONG_SUPPORT__)
        case 'I':
            if( opt_str[1] == '6' && opt_str[2] == '4' ) {
                specs->long_double_var = 1;
                opt_str += 3;
            }
            break;
        #endif
        }
        return( opt_str );
    }

/*
 * scan_white -- scan white space from input stream
 */
static int scan_white( specs )
        PTR_SCNF_SPECS specs;
    {
        register int c, len;

        len = 0;
        for( ; ; ) {
            c = cget( specs );
            if( !__F_NAME(isspace,iswspace)( c ) ) break;
            ++len;
        }
        if( !specs->eoinp ) uncget( c, specs );
        return( len );
    }

/*
 * scan_char -- handles %c and %C
 */
static int scan_char( specs, arg )
        PTR_SCNF_SPECS specs;
        my_va_list *arg;
    {
        int             len;
        int             width;
        FAR_STRING      str;
        int             c;

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
        }
        len = 0;
        if( (width = specs->width) == -1 )  width = 1;
        while( width > 0 ) {
            c = cget( specs );
            if( specs->eoinp ) break;
            ++len;
            --width;
            if( specs->assign ) {
                #if defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
                    if( specs->short_var ) {
                        char        mbBuf[MB_CUR_MAX];

                        if( wctomb( mbBuf, c ) != -1 ) {
                            *(FAR_ASCII_STRING)str = mbBuf[0];
                            str = (FAR_STRING) ( (FAR_ASCII_STRING)str + 1 );
                            if( _ismbblead( mbBuf[0] ) )  {
                                *(FAR_ASCII_STRING)str = mbBuf[1];
                                str = (FAR_STRING) ( (FAR_ASCII_STRING)str + 1 );
                            }
                        } else {
                            return( 0 );
                        }
                    } else
                #elif defined(USE_MBCS_TRANSLATION)
                    if( specs->long_var ) {
                        wchar_t     wc;
                        char        mbBuf[MB_CUR_MAX];

                        mbBuf[0] = c;
                        if( _ismbblead( mbBuf[0] ) )  mbBuf[1] = cget( specs );
                        if( mbtowc( &wc, mbBuf, MB_CUR_MAX ) != -1 ) {
                            *(FAR_UNI_STRING)str = wc;
                            str = (FAR_STRING) ( (FAR_UNI_STRING)str + 1 );
                        } else {
                            return( 0 );
                        }
                    } else
                #endif
                {
                    *str++ = c;
                }
            }
        }
        return( len );
    }


/*
 * scan_string -- handles %s and %S
 */
static int scan_string( specs, arg )
        PTR_SCNF_SPECS specs;
        my_va_list *arg;
    {
        int                     c;
        int                     len;
        FAR_ASCII_STRING        str;
        char                    chsize;

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
        }
        len = 0;
        for( ;; ) {
            c = cget( specs );
            if( !__F_NAME(isspace,iswspace)( c ) ) break;
            ++len;
        }
        if( specs->eoinp ) {
            len = 0;            /* since this is eof, no input done */
            goto done;
        }
        if( specs->width-- == 0 ) goto ugdone;
        do {
            ++len;
            if( specs->assign ) {
                if( chsize == 1 ) {
                    #if defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
                        char        mbBuf[MB_CUR_MAX];

                        if( wctomb( mbBuf, c ) != -1 ) {
                            *(FAR_ASCII_STRING)str = mbBuf[0];
                            if( _ismbblead( mbBuf[0] ) ) {
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
                    #if !defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
                        wchar_t     wc;
                        char        mbBuf[MB_CUR_MAX];

                        mbBuf[0] = c;
                        if( _ismbblead( mbBuf[0] ) )  mbBuf[1] = cget( specs );
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
            if( (c = cgetw( specs )) == STOP_CHR ) goto done;
        } while( !__F_NAME(isspace,iswspace)( c ) );
    ugdone:
        uncget( c, specs );
    done:
        if( specs->assign && len > 0 ) {
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
static void report_scan( specs, arg, match )
        PTR_SCNF_SPECS specs;
        my_va_list *arg;
        int match;
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
            if( specs->short_var ) {
                *((FAR_SHORT) iptr) = match;
            } else if( specs->long_var ) {
                *((FAR_LONG) iptr) = match;
            } else {
                *iptr = match;
            }
        }
    }

#if !defined(__WIDECHAR__)
#define SCANSET_LENGTH  (256 / 8)

static const char lst_mask[8] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

/*
 * makelist -- create scanset for %[ directive.
 *             scanset is stored as 256 bit flags in a 32 byte array.
 */
static const char *makelist( format, scanset )
        const char *format;
        char *scanset;
    {
        register int lst_chr;

        memset( scanset, 0, SCANSET_LENGTH );
        if( (lst_chr = *format++) == '\0' )  return( format );
        do {
            scanset[lst_chr >> 3] |= lst_mask[lst_chr & 0x07];
            if( (lst_chr = *format) == '\0' ) break;
            ++format;
        } while( lst_chr != ']' );
        return( format );
    }
#endif

/*
 * scan_arb -- handles %[
 */
static int scan_arb( specs, arg, format )
        PTR_SCNF_SPECS specs;
        my_va_list *arg;
        const CHAR_TYPE **format;
    {
        unsigned width;
        FAR_STRING str;
        int len, c, not_flag;
        #if defined(__WIDECHAR__)
            const CHAR_TYPE *list;
            CHAR_TYPE   ch;
            char        in_list;
        #else
            char scanset[SCANSET_LENGTH];
        #endif

        if( not_flag = (**format == '^') ) {
            ++(*format);
        }
        #if !defined(__WIDECHAR__)
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
        }
        len = 0;
        width = specs->width;
        while( width > 0 ) {
            c = cget( specs );
            if( specs->eoinp ) break;
            #if defined(__WIDECHAR__)
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
                if( ((scanset[c >> 3] & lst_mask[c & 0x07]) == 0) != not_flag ) {
                    uncget( c, specs );
                    break;
                }
            #endif
            ++len;
            --width;
            if( specs->assign ) *str++ = c;
        }
        if( specs->assign && len > 0 ) *str = '\0';
        return( len );
    }

/*
 * scan_float -- handles floating point numerical conversion
 *               *** should implement buffer overflow protection ***
 */
static int scan_float( specs, arg )
        PTR_SCNF_SPECS specs;
        my_va_list *arg;
    {
        auto double value;
        char *num_str, buf[80];
        int len;
        int pref_len;
        long c;
        int digit_found;
        FAR_FLOAT fptr;
        char    *p;
        T32     at;
        T32     ft;

        num_str = buf;
        pref_len = len = 0;
        for( ; ; ) {
            c = cget( specs );
            if( !__F_NAME(isspace,iswspace)( c ) ) break;
            ++pref_len;
        }
        if( specs->eoinp ) goto done;
        if( specs->width-- == 0 ) goto ugdone;
        if( c == '+' || c == '-' ) {
            *num_str++ = c;
            ++pref_len;
            if( (c = cgetw( specs )) == STOP_CHR ) goto done;
        }
        if( !__F_NAME(isdigit,iswdigit)( c ) && c != '.' ) goto ugdone;
        at.uWhole = 0;
        digit_found = FALSE;
        if( __F_NAME(isdigit,iswdigit)( c ) ) {
            digit_found = TRUE;
            do {
                *num_str++ = c;
                if( specs->short_var ) at.wd.hi = at.wd.hi * 10 + c - '0';
                ++len;
                if( (c = cgetw( specs )) == STOP_CHR ) goto done;
            } while( __F_NAME(isdigit,iswdigit)( c ) );
        }
        if( c == '.' ) {
            *num_str++ = c;
            ++len;              /* account for the '.' */
            if( (c = cgetw( specs )) == STOP_CHR ) goto done;
            if( !digit_found && !__F_NAME(isdigit,iswdigit)(c) ) goto ugdone;
            while( __F_NAME(isdigit,iswdigit)( c ) ) {
                *num_str++ = c;
                ++len;
                if( (c = cgetw( specs )) == STOP_CHR ) break;
            }
            if( specs->short_var ) {    /* %hf fixed-point format 05-feb-92 */
                ft.uWhole = 0;
                p = num_str;
                for(;;) {
                    --p;
                    if( *p == '.' ) break;
                    ft.bite.b3 = *p - '0';
                    ft.uWhole = ft.uWhole / 10;
                }
                at.wd.lo = ft.wd.lo;
            }
            if( c == STOP_CHR ) goto done;
        }
        if( specs->short_var == 0  &&  (c == 'e' || c == 'E') ) {
            *num_str++ = c;
            ++len;
            if( (c = cgetw( specs )) == STOP_CHR ) goto done;
            if( c == '+' || c == '-' ) {
                *num_str++ = c;
                ++len;
                if( (c = cgetw( specs )) == STOP_CHR ) goto done;
            }
            if( !__F_NAME(isdigit,iswdigit)( c ) ) {
                len = 0;                /* fast way to flag error */
            } else {
                do {
                    *num_str++ = c;
                    ++len;
                    if( (c = cgetw( specs )) == STOP_CHR ) goto done;
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
/*              value = strtod( buf, NULL );  */

                /* use type cast to avoid getting "ftos" brought in */

/*              __cnvs2d( buf, (void *)&value ); */
                if( specs->short_var ) {
                    if( buf[0] == '-' )  at.sWhole = - at.sWhole;
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
                } else if( specs->long_var      ||  specs->long_double_var ) {
                    *((FAR_DOUBLE) fptr) = value;
                } else {
                    *fptr = value;
                }
            }
        }
        return( len );
    }

/*
 * scan_int -- handles integer numeric conversion
 */
static int scan_int( specs, arg, base, sign_flag )
        PTR_SCNF_SPECS specs;
        my_va_list *arg;
        int base, sign_flag;
   {
        long int value;
        int len;
        int pref_len;
        int c;
        int sign;
        int digit;
        FAR_INT iptr;
        #if defined(__LONG_LONG_SUPPORT__)
            UINT64_TYPE long_value;
            FAR_INT64 llptr;
            _clib_U32ToU64( 0, long_value );
        #endif

        value = 0;
        pref_len = len = 0;
        for( ; ; ) {
            c = cget( specs );
            if( !__F_NAME(isspace,iswspace)( c ) ) break;
            ++pref_len;
        }
        if( specs->eoinp ) goto done;
        if( specs->width-- == 0 ) goto ugdone;
        sign = '+';
        if( sign_flag && (c == '+' || c == '-') ) {
            sign = c;
            ++pref_len;
            if( (c = cgetw( specs )) == STOP_CHR ) goto done;
        }
        if( base == 0 ) {
            if( c == '0' ) {
                len = 1;
                if( (c = cgetw( specs )) == STOP_CHR ) goto done;
                if( c == 'x' || c == 'X' ) {
                    len = 0;
                    ++pref_len;         /* for the '0' */
                    ++pref_len;         /* for the 'x' */
                    if( (c = cgetw( specs )) == STOP_CHR ) goto done;
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
                if( (c = cgetw( specs )) == STOP_CHR ) goto done;
                if( c == 'x'  ||  c == 'X' ) {
                    len = 0;
                    ++pref_len;         /* for the '0' */
                    ++pref_len;         /* for the 'x' */
                    if( (c = cgetw( specs )) == STOP_CHR ) goto done;
                }
            }
        }
        #if defined(__LONG_LONG_SUPPORT__)
            if( specs->long_double_var ) {
                UINT64_TYPE long_base;
                UINT64_TYPE long_digit;

                _clib_U32ToU64( base, long_base );
                for( ; ; ) {
                    digit = radix_value( c );
                    if( digit >= base ) break;
                    _clib_U32ToU64( digit, long_digit );
                    _clib_U64Mul( long_value, long_base, long_value );
                    _clib_U64Add( long_value, long_digit, long_value );
                    ++len;
                    if( (c = cgetw( specs )) == STOP_CHR ) goto done;
                }
                if( c == ':'  &&  specs->p_format ) {
                    for( ; ; ) {
                        ++len;
                        if( (c = cgetw( specs )) == STOP_CHR ) goto done;
                        digit = radix_value( c );
                        if( digit >= base ) break;
                        _clib_U32ToU64( digit, long_digit );
                        _clib_U64Mul( long_value, long_base, long_value );
                        _clib_U64Add( long_value, long_digit, long_value );
                    }
                }
            } else
        #endif
        {
            for( ; ; ) {
                digit = radix_value( c );
                if( digit >= base ) break;
                value = value * base + digit;
                ++len;
                if( (c = cgetw( specs )) == STOP_CHR ) goto done;
            }
            if( c == ':'  &&  specs->p_format ) {
                for( ; ; ) {
                    ++len;
                    if( (c = cgetw( specs )) == STOP_CHR ) goto done;
                    digit = radix_value( c );
                    if( digit >= base ) break;
                    value = value * base + digit;
                }
            }
        }
    ugdone:
        uncget( c, specs );
    done:
        #if defined(__LONG_LONG_SUPPORT__)
            if( specs->long_double_var ) {
                if( sign == '-' ) {
                    _clib_U64Neg( long_value, long_value );
                }
                if( len > 0 ) {
                    len += pref_len;
                    if( specs->assign ) {
                        #if defined( __FAR_SUPPORT__ )
                            if( specs->far_ptr ) {
                                llptr = va_arg( arg->v, UINT64_TYPE _WCFAR * );
                            } else if( specs->near_ptr ) {
                                llptr = va_arg( arg->v, UINT64_TYPE _WCNEAR * );
                            } else {
                                llptr = va_arg( arg->v, UINT64_TYPE * );
                            }
                        #else
                            llptr = va_arg( arg->v, UINT64_TYPE * );
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
                    if( specs->short_var ) {
                        *((FAR_SHORT) iptr) = value;
                    } else if( specs->long_var ) {
                        *((FAR_LONG) iptr) = value;
                    } else {
                        *iptr = value;
                    }
                }
            }
        }
        return( len );
    }


static int radix_value( c )
        register int c;
    {
        if( c >= '0' && c <= '9' ) return( c - '0' );
        c = __F_NAME(tolower,towlower)( c );
        /*
        c = isupper( c ) ? tolower( c ) : c;
        */
        if( c >= 'a' && c <= 'f' ) return( c - 'a' + 10 );
        return( 16 );
    }


/*
 * cgetw -- cget which keeps track of field width.
 *          returns STOP_CHR on end of field or end of file.
 */
static long cgetw( specs )
        PTR_SCNF_SPECS specs;
    {
        register int c;

        if( specs->width-- == 0 ) return ( STOP_CHR );
        c = cget( specs );
        return( !specs->eoinp ? (long)c : STOP_CHR );
    }


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
* Description:  __prtf() - low level string formatter.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#ifdef SAFE_PRINTF
    #include "saferlib.h"
#endif
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined( __WIDECHAR__ ) || defined( CLIB_USE_MBCS_TRANSLATION )
    #include <mbstring.h>
#endif
#ifdef __QNX__
    #include "slibqnx.h"
#endif
#include "prtscncf.h"
#include "fixpoint.h"
#include "fltsupp.h"
#include "printf.h"


#define BUF_SIZE    72  /* 64-bit ints formatted as binary can get big */
#define TRUE        1
#define FALSE       0

#define PASCAL_STRING           STRING( 'S' )       /* for Novell */
#define WIDE_CHAR_STRING        STRING( 'S' )

#if defined( __QNX_386__ )
    /* for use in QNX 32-bit shared library */
    #pragma aux __prtf "_sl_*" __far;
#endif


#if defined( __QNX__ )
    #define EFG_PRINTF __EFG_Format
#else
    #define EFG_PRINTF (*__EFG_printf)
#endif


#if defined( __WIDECHAR__ )
    #define _OTHER_FAR_STRING           FAR_ASCII_STRING
    #define _OTHER_NULLCHAR             '\0'
    #define _OTHER_TO_WIDE(c)           ((unsigned char)c)
#else
    #define _OTHER_FAR_STRING           FAR_WIDE_STRING
    #define _OTHER_NULLCHAR             L'\0'
    #define _OTHER_TO_WIDE(c)           (c)
#endif


#if defined( __QNX__ ) && !defined( __STDC_WANT_LIB_EXT1__ ) && !defined( __WIDECHAR__ ) && defined( _M_I86 ) && !defined( IN_SLIB )

/* 16-bit QNX no code, only in SLIB */

#else

static const CHAR_TYPE *evalflags( const CHAR_TYPE *ctl, PTR_PRTF_SPECS specs )
{
    specs->_flags = 0;
    for( ; ; ctl++ ) {
        if( *ctl == STRING( '-' ) ) {
            specs->_flags |= SPF_LEFT_ADJUST;
        } else if( *ctl == STRING( '#' ) ) {
            specs->_flags |= SPF_ALT;
        } else if( *ctl == STRING( '+' ) ) {
            specs->_flags |= SPF_FORCE_SIGN;
            specs->_flags &= ~SPF_BLANK;
        } else if( *ctl == STRING( ' ' ) ) {
            if( ( specs->_flags & SPF_FORCE_SIGN ) == 0 ) {
                specs->_flags |= SPF_BLANK;
            }
        } else if( *ctl == STRING( '0' ) ) {
            specs->_pad_char = STRING( '0' );
#ifdef __QNX__
            specs->_flags |= SPF_ZERO_PAD;
#endif
        } else {
            break;
        }
    }
    return( ctl );
}

static const CHAR_TYPE *getprintspecs( const CHAR_TYPE *ctl, va_list *pargs, PTR_PRTF_SPECS specs )
{
    specs->_pad_char = STRING( ' ' );
    ctl = evalflags( ctl, specs );
    specs->_fld_width = 0;
    if( *ctl == STRING( '*' ) ) {
        specs->_fld_width = va_arg( *pargs, int );
        if( specs->_fld_width < 0 ) {
            specs->_fld_width = - specs->_fld_width;
            specs->_flags |= SPF_LEFT_ADJUST;
        }
        ctl++;
    } else {
        while(( *ctl >= STRING( '0' ) ) && ( *ctl <= STRING( '9' ) )) {
            specs->_fld_width = specs->_fld_width * 10 + ( *ctl++ - STRING( '0' ) );
        }
    }
    specs->_prec = -1;
    if( *ctl == STRING( '.' ) ) {
        specs->_prec = 0;
        ctl++;
        if( *ctl == STRING( '*' ) ) {
            specs->_prec = va_arg( *pargs, int );
            if( specs->_prec < 0 )
                specs->_prec = -1;
            ctl++;
        } else {
            while(( *ctl >= STRING( '0' ) ) && ( *ctl <= STRING( '9' ) )) {
                specs->_prec = specs->_prec * 10 + ( *ctl++ - STRING( '0' ) );
            }
        }
        /*
        "For b, d, i, o, u, x, X, e, E, f, g and G conversions, leading
        zeros (following any indication of sign or base) are used to
        pad the field width; no space padding is performed. If the 0
        or - flags both appear, the 0 flag is ignored.  For b, d, i, o,
        u, x or X conversions, if a precision is specified, the 0 flag
        is ignored. For other conversions, the behaviour is undefined."
        */
//      if( specs->_prec != -1 )  specs->_pad_char = STRING( ' ' ); /* 30-jul-95 *//*removed by JBS*/
    }
    switch( *ctl ) {
#ifdef JSPEC_IS_LLONG
    JSPEC_CASE_LLONG
        specs->_flags |= SPF_LONG_LONG;
        ctl++;
        break;
#endif
    case STRING( 'l' ):
        if( ctl[1] == STRING( 'l' ) ) {
            specs->_flags |= SPF_LONG_LONG;
            ctl += 2;
            break;
        }
        /* fall through */
    ZSPEC_CASE_LONG
    TSPEC_CASE_LONG
    case STRING( 'w' ):
        specs->_flags |= SPF_LONG;
        ctl++;
        break;
    case STRING( 'h' ):
        if( ctl[1] == STRING( 'h' ) ) {
            specs->_flags |= SPF_CHAR;
            ctl += 2;
            break;
        }
        specs->_flags |= SPF_SHORT;
        ctl++;
        break;
    case STRING( 'I' ):
        if(( ctl[1] == STRING( '6' ) ) && ( ctl[2] == STRING( '4' ) )) {
            specs->_flags |= SPF_LONG_LONG;
            ctl += 3;
        }
        break;
    case STRING( 'L' ):
        specs->_flags |= SPF_LONG_DOUBLE;
        ctl++;
        break;
#if defined( __FAR_SUPPORT__ )
    case STRING( 'F' ):         /* conflicts with ISO-defined 'F' conversion */
        /* fall through */
#endif
    case STRING( 'W' ):                   /* 8086 specific flag for FAR pointer */
        specs->_flags |= SPF_FAR;
        ctl++;
        break;
    case STRING( 'N' ):                   /* 8086 specific flag for NEAR pointer */
        specs->_flags |= SPF_NEAR;
        ctl++;
        break;
#if defined( TSPEC_IS_INT ) || defined( ZSPEC_IS_INT )
    TSPEC_CASE_INT      /* If either STRING( 't' ) or STRING( 'z' ) spec corresponds to 'int',  */
    ZSPEC_CASE_INT      /* we need to parse and ignore the spec.            */
        ctl++;
        break;
#endif
    }
    return( ctl );
}


static int far_strlen( FAR_STRING s, int precision )
{
    int     len;

    len = 0;
    while(( len != precision ) && ( *s++ != NULLCHAR ))
        ++len;

    return( len );
}

/*
 * far_other_strlen - calculates the length of an ascii string
 *                    for the unicode version
 *                  - calculates the length of a unicode string
 *                    for the ascii version
 */

static int far_other_strlen( FAR_STRING s, int precision )
{
#if defined( __RDOS__ ) || defined( __RDOSDEV__ )

    /* unused parameters */ (void)s; (void)precision;

    return( 0 );  // RDOS doesn't support unicode
#else
    int                 len = 0;
    _OTHER_FAR_STRING   ptr = (_OTHER_FAR_STRING)s;
  #if !defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
    char                mbBuf[MB_CUR_MAX];
    int                 chBytes;

    if( precision == -1 ) {
        while( *ptr != _OTHER_NULLCHAR ) {
            chBytes = wctomb( mbBuf, *ptr++ );
            if( chBytes != -1 ) {
                len += chBytes;
            }
        }
        return( len );
    }
    while( *ptr != _OTHER_NULLCHAR && ( len <= precision )) {
        chBytes = wctomb( mbBuf, *ptr++ );
        if( chBytes != -1 ) {
            len += chBytes;
        }
    }
    return(( len <= precision ) ? len : precision );
  #else
    while( *ptr++ != _OTHER_NULLCHAR && ( len != precision ))
        ++len;

    return( len );
  #endif
#endif
}

static void fmt4hex( unsigned value, CHAR_TYPE *buf, int maxlen )
{
    int         i, len;
    CHAR_TYPE   buffer[10];

    __F_NAME(itoa,_itow)( value, buffer, 16 );
    len = __F_NAME(strlen,wcslen)( buffer );
    /* first check len, if it is not OK don't update i
     * because i value is used next for adding leading zeros
     */
    for( i = maxlen; len-- > 0 && i-- > 0; ) {
        buf[i] = buffer[len];
    }
    while( i-- > 0 ) {
        buf[i] = STRING( '0' );
    }
    buf[maxlen] = NULLCHAR;
}


static void FixedPoint_Format( CHAR_TYPE *buf, long value, PTR_PRTF_SPECS specs )
{
    T32         at;
    int         i;
    CHAR_TYPE   *bufp;

    at.sWhole = value;
    if( at.sWhole < 0 ) {
        at.sWhole = - at.sWhole;
        *buf++ = STRING( '-' );
    }
    if( specs->_prec == -1 )
        specs->_prec = 4;

    __F_NAME(itoa,_itow)( at.wd.hi, buf, 10 );
    bufp = buf;         /* remember start address of buffer */
    while( *buf != NULLCHAR )
        ++buf;

    if( specs->_prec != 0 ) {
        *buf++ = STRING( '.' );
        for( i = 0; i < specs->_prec; i++ ) {
            at.wd.hi = 0;
            at.uWhole *= 10;
            *buf++ = at.bite.b3 + STRING( '0' );
        }
        *buf = NULLCHAR;
    }
    if( at.wd.lo & 0x8000 ) {   /* fraction >= .5, need to round */
        for(;;) {                               /* 22-dec-91 */
            if( buf == bufp ) {
                *buf++ = STRING( '1' );
                while( *buf == STRING( '0' ) )
                    ++buf;

                if( *buf == STRING( '.' ) ) {
                    *buf++ = STRING( '0' );
                    *buf++ = STRING( '.' );
                    while( *buf == STRING( '0' ) ) {
                        ++buf;
                    }
                }
                *buf++ = STRING( '0' );
                *buf = NULLCHAR;
                break;
            }
            --buf;
            if( *buf == STRING( '.' ) )
                --buf;

            if( *buf != STRING( '9' ) ) {
                ++*buf;
                break;
            }
            *buf = STRING( '0' );
        }
    }
}

static void float_format( CHAR_TYPE *buffer, va_list *pargs, PTR_PRTF_SPECS specs )
{
#ifdef __WIDECHAR__
    unsigned char       mbBuffer[BUF_SIZE*MB_CUR_MAX];
    _mbcs_SPECS         mbSpecs;
    int                 count;
    size_t              rc;
    unsigned char       *p;
#endif // __WIDECHAR__

#ifdef __WIDECHAR__
    /*
     * EFG_PRINTF can only handle MBCS buffers and the MBCS version of the
     * SPECS structure.  So, make a _mbcs_SPECS structure equivalent to the
     * _wide_SPECS one, and use that instead.  Note that we can't use
     * memcpy() because some field sizes are different.
     */
    mbSpecs._dest = NULL;               /* this field isn't actually used */
    mbSpecs._flags = specs->_flags;
    mbSpecs._version = specs->_version;
    mbSpecs._fld_width = specs->_fld_width;
    mbSpecs._prec = specs->_prec;
    mbSpecs._output_count = specs->_output_count;
    mbSpecs._n0 = specs->_n0;
    mbSpecs._nz0 = specs->_nz0;
    mbSpecs._n1 = specs->_n1;
    mbSpecs._nz1 = specs->_nz1;
    mbSpecs._n2 = specs->_n2;
    mbSpecs._nz2 = specs->_nz2;
    mbSpecs._character = TO_ASCII( specs->_character );
    mbSpecs._pad_char = TO_ASCII( specs->_pad_char );
#endif

#ifdef __WIDECHAR__
    EFG_PRINTF( (char *)mbBuffer, pargs, &mbSpecs );
#else
    EFG_PRINTF( buffer, pargs, specs );
#endif

#ifdef __WIDECHAR__
    /*
     * Now convert the returned information back into our _wide_SPECS
     * structure.  We can't just use mbstowcs because it's an array of
     * characters, not a string.
     */
    p = mbBuffer;
    for( count = 0; count < BUF_SIZE; count++ ) {
        rc = mbtowc( &(buffer[count]), (char *)p, MB_CUR_MAX );
        if( rc == -1 ) {
            buffer[count] = STRING( '?' );
        }
        p = _mbsinc( p );
    }
    specs->_flags = mbSpecs._flags;
    specs->_version = mbSpecs._version;
    specs->_fld_width = mbSpecs._fld_width;
    specs->_prec = mbSpecs._prec;
    specs->_output_count = mbSpecs._output_count;
    specs->_n0 = mbSpecs._n0;
    specs->_nz0 = mbSpecs._nz0;
    specs->_n1 = mbSpecs._n1;
    specs->_nz1 = mbSpecs._nz1;
    specs->_n2 = mbSpecs._n2;
    specs->_nz2 = mbSpecs._nz2;
    specs->_character = _OTHER_TO_WIDE( mbSpecs._character );
    specs->_pad_char = _OTHER_TO_WIDE( mbSpecs._pad_char );
#endif
}

static void floathex_format( CHAR_TYPE *buffer, va_list *pargs, PTR_PRTF_SPECS specs )
{
    uint64_t fltraw = va_arg( *pargs, unsigned long long );
    CHAR_TYPE *arg = &buffer[ specs->_n0 ];
    CHAR_TYPE *wp = arg;

#define EXP (int)(((fltraw >> 52ull) & 0x7FFul) - 1023)

    if (fltraw & 0x8000000000000000ull) {
	    fltraw &= 0x7FFFFFFFFFFFFFFFull;
	    *wp++ = '-';
    }

    *wp++ = '0';
    *wp++ = 'x';
    if (EXP == -1023) *wp++ = '0';/*subnormal*/
    else *wp++ = '1';

    {
        uint64_t m = fltraw & 0xFFFFFFFFFFFFFull; /* low 52 bits */
        if (m != 0ull) {
            *wp++ = '.';
            while ((m & 0xFull) == 0) m >>= 4ull;
            __F_NAME(ulltoa,_ulltow)( m, wp, 16 );
            wp += far_strlen( wp, -1 );
        }

        *wp++ = 'p';
        if (EXP != -1023) {
            if (EXP >= 0) *wp++ = '+';
            __F_NAME(ltoa,_ltow)( EXP, wp, 10 );
        }
        else {
            *wp++ = '+';
            __F_NAME(ltoa,_ltow)( 0, wp, 10 );
        }
        wp += far_strlen( wp, -1 );
    }

    if( specs->_character == STRING( 'A' ) ) {
        __F_NAME(_strupr,_wcsupr)( buffer );
    }

#undef EXP

    specs->_n1 = (size_t)(wp - arg);
}

static void SetZeroPad( PTR_PRTF_SPECS specs )
{
    int         n;

    if( !(specs->_flags & SPF_LEFT_ADJUST) ) {
        if( specs->_pad_char == STRING( '0' ) ) {
            n = specs->_fld_width - specs->_n0 - specs->_nz0 -
                    specs->_n1 - specs->_nz1 - specs->_n2 - specs->_nz2;
            if( n > 0 ) {
                specs->_nz0 += n;
            }
        }
    }
}


#if !defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
static void write_wide_string( FAR_WIDE_STRING str, PTR_PRTF_SPECS specs, prtf_callback_t *out_putc )
{
    int     bytes;
    char    mbBuf[MB_CUR_MAX];
    char    *mbBufPtr;

    while( specs->_n1 > 0 ) {
        bytes = wctomb( mbBuf, *str++ );
        if( bytes != -1 ) {
            if( bytes <= specs->_n1 ) {
                mbBufPtr = mbBuf;
                while( bytes-- ) {
                    (out_putc)( specs, *mbBufPtr++ );
                    --specs->_n1;
                }
            } else {
                specs->_n1 = 0;
            }
        }
    }
}
#endif


#if defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
static void write_skinny_string( FAR_ASCII_STRING str, PTR_PRTF_SPECS specs, prtf_callback_t *out_putc )
{
    int                 bytes;
    wchar_t             wc;
    FAR_ASCII_STRING    mbPtr = str;
    char                mbBuf[MB_CUR_MAX];

    while( specs->_n1 > 0 ) {
        mbBuf[0] = *mbPtr++;
        if( _ismbblead( (unsigned char)mbBuf[0] ) )
            mbBuf[1] = *mbPtr++;

        bytes = mbtowc( &wc, mbBuf, MB_CUR_MAX );
        if( bytes <= specs->_n1 ) {
            if( bytes != -1 ) {
                (out_putc)( specs, wc );
                specs->_n1 -= bytes;
            }
        } else {
            specs->_n1 = 0;
        }
    }
}
#endif


static FAR_STRING formstring( CHAR_TYPE *buffer, va_list *pargs, PTR_PRTF_SPECS specs, CHAR_TYPE *null_string )
{
    FAR_STRING              arg;
    int                     length;
    int                     radix;
    unsigned long long      long_long_value;
    unsigned long           long_value;
    unsigned int            int_value;
#if defined( __FAR_SUPPORT__ )
    unsigned int            seg_value;
#endif
#if !defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
    int                     bytes;
#endif

    /* unused parameters */ (void)null_string;

    arg = buffer;

    specs->_n0 = specs->_nz0 =
    specs->_n1 = specs->_nz1 =
    specs->_n2 = specs->_nz2 = 0;

    switch( specs->_character ) {
    case STRING( 'b' ):
    case STRING( 'o' ):
    case STRING( 'u' ):
    case STRING( 'x' ):
    case STRING( 'X' ):
        if( specs->_flags & SPF_LONG_LONG ) {
            long_long_value = va_arg( *pargs, unsigned long long );
        } else if( specs->_flags & SPF_LONG ) {
            long_value = va_arg( *pargs, unsigned long );
        } else {
            long_value = va_arg( *pargs, unsigned );
            if( specs->_flags & SPF_SHORT ) {
                long_value = (unsigned short)long_value;
            } else if( specs->_flags & SPF_CHAR ) {
                long_value = (unsigned char)long_value;
            }
        }
        break;
    case STRING( 'd' ):
    case STRING( 'i' ):
        if( specs->_flags & SPF_LONG_LONG ) {
            long_long_value = va_arg( *pargs, long long );
        } else if( specs->_flags & SPF_LONG ) {
            long_value = va_arg( *pargs, long );
        } else {
            long_value = va_arg( *pargs, int );
            if( specs->_flags & SPF_SHORT ) {
                long_value = (short)long_value;
            } else if( specs->_flags & SPF_CHAR ) {
                long_value = (signed char)long_value;
            }
        }
        {
            int negative = FALSE;

            if( specs->_flags & SPF_LONG_LONG ) {
                if( (long long)long_long_value < 0 ) {
                    negative = TRUE;
                }
            } else if( (long)long_value < 0 ) {
                negative = TRUE;
            }
            if( negative ) {
                buffer[specs->_n0++] = STRING( '-' );

                if( specs->_flags & SPF_LONG_LONG ) {
                    long_long_value = -long_long_value;
                } else {
                    long_value = -long_value;
                }
            } else if( specs->_flags & SPF_FORCE_SIGN ) {
                buffer[specs->_n0++] = STRING( '+' );
            } else if( specs->_flags & SPF_BLANK ) {
                buffer[specs->_n0++] = STRING( ' ' );
            }
        }
        break;
    }

    radix  = 10;                        /* base 10 for 'd', 'i' and 'u' */
    switch( specs->_character ) {
    case STRING( 'f' ):
    case STRING( 'F' ):
        if( specs->_flags & SPF_SHORT ) {       /* "%hf"  13-jun-91 */
            long_value = va_arg( *pargs, long );
            FixedPoint_Format( buffer, long_value, specs );
            specs->_n1 = far_strlen( buffer, -1 );
            break;
        }
        /* types f & F fall through */

    case STRING( 'g' ):
    case STRING( 'G' ):
    case STRING( 'e' ):
    case STRING( 'E' ):
        float_format( buffer, pargs, specs );
        arg++; // = &buffer[1];
        break;

    case STRING( 'a' ):
    case STRING( 'A' ):
        floathex_format( buffer, pargs, specs );
        break;

    case STRING( 's' ):
#ifndef __NETWARE__
    case WIDE_CHAR_STRING:
#else
    case PASCAL_STRING:
#endif
        // arg has been initialized to point to buffer
        // set buffer[0] to a null character assuming pointer will be NULL
        // If pointer is not null, then arg will be assigned the pointer
        buffer[0] = NULLCHAR;       // assume null pointer
#if defined( __FAR_SUPPORT__ )
        if( specs->_flags & SPF_FAR ) {
            CHAR_TYPE _WCFAR *temp = va_arg( *pargs, CHAR_TYPE _WCFAR * );

            if( temp ) {
                arg = temp;
            }
        } else if( specs->_flags & SPF_NEAR ) {
            CHAR_TYPE _WCNEAR *temp = va_arg( *pargs, CHAR_TYPE _WCNEAR * );

            if( temp ) {
                arg = (void *)temp;
            }
        } else {
            CHAR_TYPE *temp = va_arg( *pargs, CHAR_TYPE * );

            if( temp ) {
                arg = temp;
            }
        }
#else
        {
            CHAR_TYPE *temp = va_arg( *pargs, CHAR_TYPE * );

            if( temp ) {
                arg = temp;
            }
        }
#endif

#ifdef __NETWARE__
        if( specs->_character == PASCAL_STRING ) {
#ifdef __WIDECHAR__
            if( specs->_flags & SPF_SHORT )
#else
            if( specs->_flags & SPF_LONG )
#endif
            {
                length = *( (_OTHER_FAR_STRING)arg );
                arg = (FAR_STRING)( (_OTHER_FAR_STRING)arg + 1 );
            } else {
                length = *arg++;
            }
        } else
#elif !defined( __NETWARE__ ) && !defined( __WIDECHAR__ )
        if( specs->_character == WIDE_CHAR_STRING ) {
            if( specs->_flags & SPF_SHORT ) {
                length = far_strlen( arg, specs->_prec );
            } else {
                length = far_other_strlen( arg, specs->_prec );
            }
        } else
#endif
#ifdef __WIDECHAR__
        if( specs->_flags & SPF_SHORT ) {
#else
        if( specs->_flags & SPF_LONG ) {
#endif
            length = far_other_strlen( arg, specs->_prec );
        } else {
            length = far_strlen( arg, specs->_prec );
        }

        specs->_n1 = length;
        if(( specs->_prec >= 0 ) && ( specs->_prec < length )) {
            specs->_n1 = specs->_prec;
        }
        break;

    case STRING( 'x' ):
    case STRING( 'X' ):
        if( specs->_flags & SPF_ALT ) {
            if( specs->_flags & SPF_LONG_LONG ) {
                if( long_long_value != 0 ) {
                    buffer[specs->_n0++] = STRING( '0' );
                    buffer[specs->_n0++] = specs->_character;
                }
            } else if( long_value != 0 ) {
                buffer[specs->_n0++] = STRING( '0' );
                buffer[specs->_n0++] = specs->_character;
            }
        }
        radix = 16;                 /* base 16 */
        goto processNumericTypes;

    case STRING( 'b' ):           /* CDH 2003 Apr 23 *//* Add binary mode */
        radix = 2;                  /* base 2 */
        goto processNumericTypes;

    case STRING( 'o' ):
        radix = 8;                  /* base 8 */
        /* 'o' conversion falls through */

    case STRING( 'd' ):
    case STRING( 'i' ):
    case STRING( 'u' ):
//       'x' and 'X' jumps here

processNumericTypes:
        if( specs->_prec != -1 )
            specs->_pad_char = STRING( ' ' );     /* 30-jul-95, 11-may-99 */

        /* radix contains the base; 8 for 'o', 10 for 'd' and 'i' and 'u',
           16 for 'x' and 'X', and 2 for 'b' */

        arg = &buffer[ specs->_n0 ];

        if( specs->_flags & SPF_LONG_LONG ) {
            if(( specs->_prec == 0 ) && ( long_long_value == 0 )) {
                *arg = NULLCHAR;
                length = 0;
            } else {
                __F_NAME(ulltoa,_ulltow)( long_long_value, &buffer[specs->_n0], radix );
                if( specs->_character == STRING( 'X' ) ) {
                    __F_NAME(_strupr,_wcsupr)( buffer );
                }
                length = far_strlen( arg, -1 );
            }
        } else if(( specs->_prec == 0 ) && ( long_value == 0 )) {
            *arg = NULLCHAR;
            length = 0;
        } else {
            __F_NAME(ultoa,_ultow)( long_value, &buffer[specs->_n0], radix );
            if( specs->_character == STRING( 'X' ) ) {
                __F_NAME(_strupr,_wcsupr)( buffer );
            }
            length = far_strlen( arg, -1 );
        }
        specs->_n1 = length;
        if( specs->_n1 < specs->_prec ) {
            specs->_nz0 = specs->_prec - specs->_n1;
        } else if( specs->_flags & SPF_ALT && radix < 10
         && (!length || (arg[0] != STRING( '0' ))) ) {
            /* For 'b' and 'o' conversions, alternate format forces the number to
             * start with a zero (effectively increases precision by one), but
             * only if it doesn't start with a zero already.
             */
            ++specs->_nz0;
        }
        break;

    case STRING( 'p' ):
    case STRING( 'P' ):
#if defined( __FAR_SUPPORT__ )
    #if defined( __BIG_DATA__ )
        if( (specs->_flags & (SPF_NEAR | SPF_FAR)) == 0 ) {
            specs->_flags |= SPF_FAR;
        }
        if( specs->_fld_width == 0 ) {
            if( specs->_flags & SPF_NEAR ) {
                specs->_fld_width = sizeof( unsigned ) * 2;
            } else {
                specs->_fld_width = sizeof( CHAR_TYPE _WCFAR * ) * 2 + 1;
            }
        }
    #else
        if( specs->_fld_width == 0 ) {
            if( specs->_flags & SPF_FAR ) {
                specs->_fld_width = sizeof( CHAR_TYPE _WCFAR * ) * 2 + 1;
            } else {
                specs->_fld_width = sizeof( unsigned ) * 2;
            }
        }
    #endif
#else
        if( specs->_fld_width == 0 ) {
            specs->_fld_width = sizeof( unsigned ) * 2;
        }
#endif
        specs->_flags &= ~(SPF_BLANK | SPF_FORCE_SIGN);
        int_value = va_arg( *pargs, unsigned );               /* offset */
#if defined( __FAR_SUPPORT__ )
        if( specs->_flags & SPF_FAR ) {
            seg_value = va_arg( *pargs, unsigned ) & 0xFFFF; /* segment */
            /* use "unsigned short" for 386 instead of "unsigned" */
            fmt4hex( seg_value, buffer, 4 );
            buffer[4] = STRING( ':' );
            fmt4hex( int_value, buffer + 4 + 1, sizeof( unsigned ) * 2 );
        } else {
            fmt4hex( int_value, buffer, sizeof( unsigned ) * 2 );
        }
#else
        fmt4hex( int_value, buffer, sizeof( unsigned ) * 2 );
#endif
        if( specs->_character == STRING( 'P' ) ) {
            __F_NAME(_strupr,_wcsupr)( buffer );
        }
        specs->_n0 = far_strlen( arg, -1 );
        break;

    case STRING( 'c' ):
#if defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
        if( specs->_flags & SPF_SHORT ) {
            char *      mbPtr;
            char        mbBuf[MB_CUR_MAX];
            wchar_t     wc;

            mbPtr = va_arg( *pargs, char* );
            mbBuf[0] = mbPtr[0];
            if( _ismbblead( (unsigned char)mbBuf[0] ) )
                mbBuf[1] = mbPtr[1];

            if( mbtowc( &wc, mbBuf, MB_CUR_MAX ) != -1 ) {
                buffer[0] = wc;
            }
        } else {
            buffer[0] = va_arg( *pargs, int );
        }
        specs->_n1 = 1;
#elif !defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
        specs->_n1 = 1;
        if( specs->_flags & SPF_LONG ) {
            char        mbBuf[MB_CUR_MAX];
            wchar_t     wc;

            wc = va_arg( *pargs, int );
            if( wctomb( mbBuf, wc ) != -1 ) {
                buffer[0] = mbBuf[0];
                if( _ismbblead( (unsigned char)mbBuf[0] ) ) {
                    buffer[1] = mbBuf[1];
                    specs->_n0++;
                }
            }
        } else {
            buffer[0] = va_arg( *pargs, int );
        }
#else
        specs->_n1 = 1;
        buffer[0] = va_arg( *pargs, int );
#endif
        break;

#if !defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
    case STRING( 'C' ):
        bytes = wctomb( buffer, va_arg( *pargs, int ) );
//      if( bytes != -1  &&  bytes <= specs->_prec ) {
        if( bytes != -1 ) { /* Normative Addendum 4.5.3.3.1: no precision */
            specs->_n1 = bytes;
        } else {
            specs->_n1 = 0;
        }
        break;
#endif

    default:
        specs->_fld_width = 0;
        buffer[0] = specs->_character;
        specs->_n0 = 1;
        break;
    }
    SetZeroPad( specs );
    return( arg );
}


#ifdef __STDC_WANT_LIB_EXT1__
int __F_NAME(__prtf_s,__wprtf_s)( void PTR_PRTF_FAR dest, const CHAR_TYPE *format, va_list args, const char **msg, prtf_callback_t *out_putc )
#else
int __F_NAME(__prtf,__wprtf)( void PTR_PRTF_FAR dest, const CHAR_TYPE *format, va_list args, prtf_callback_t *out_putc )
#endif
/* dest         parm for use by out_putc    */
/* format       pointer to format string    */
/* args         pointer to pointer to args  */
/* msg          rt-constraint message       */
/* out_putc     char output routine         */
{
    CHAR_TYPE           buffer[ BUF_SIZE ];
    CHAR_TYPE           null_char = NULLCHAR;
    CHAR_TYPE           *a;
    FAR_STRING          arg;
    const CHAR_TYPE     *ctl;
    PRTF_SPECS          specs;

    specs._dest = dest;
    specs._flags = 0;
    specs._version = SPECS_VERSION;
    specs._output_count = 0;
    ctl = format;
    while( *ctl != NULLCHAR ) {
        if( *ctl != STRING( '%' ) ) {
            (out_putc)( &specs, *ctl++ );
        } else {
            ctl++;
            ctl = getprintspecs( ctl, &args, &specs );
            specs._character = *ctl++;
            if( specs._character == NULLCHAR )
                break;

            if( specs._character == STRING( 'n' ) ) {
#ifdef __STDC_WANT_LIB_EXT1__
                /* The %n specifier is not allowed - too dangerous. */
                *msg = "%n";
                break;
#else
                FAR_INT         iptr;

                /*
                   For the shared library, all pointers are FAR unless
                   explicitly set NEAR (the shared library use big_data model).
                */
#if defined( __FAR_SUPPORT__ )
                if( specs._flags & SPF_FAR ) {
                    iptr = va_arg( args, int _WCFAR * );
                } else if( specs._flags & SPF_NEAR ) {
                    iptr = va_arg( args, int _WCNEAR * );
                } else {
                    iptr = va_arg( args, int * );
                }
#else
                iptr = va_arg( args, int * );
#endif
                if( specs._flags & SPF_CHAR ) {
                    *((FAR_CHAR)iptr) = specs._output_count;
                } else if( specs._flags & SPF_SHORT ) {
                    *((FAR_SHORT)iptr) = specs._output_count;
                } else if( specs._flags & SPF_LONG ) {
                    *((FAR_LONG)iptr) = specs._output_count;
                } else if( specs._flags & SPF_LONG_LONG ) {
                    *((FAR_INT64)iptr) = specs._output_count;
                } else {
                    *iptr = specs._output_count;
                }
#endif  /* __STDC_WANT_LIB_EXT1__ */
            } else {
#ifdef __STDC_WANT_LIB_EXT1__
                if( specs._character == STRING( 's' ) || specs._character == STRING( 'S' ) ) {
                    FAR_STRING  str;
                    va_list     args_copy;

                    /* Make sure %s argument is not NULL. Note that near pointers
                     * in segmented models need special handling because only
                     * offset will be NULL, not segment.
                     */
                    va_copy( args_copy, args );
#if defined( __FAR_SUPPORT__ )
                    if( specs._flags & SPF_FAR ) {
                        str = va_arg( args_copy, CHAR_TYPE _WCFAR * );
                    } else if( specs._flags & SPF_NEAR ) {
                        CHAR_TYPE _WCNEAR   *ptr;

                        ptr = va_arg( args_copy, CHAR_TYPE _WCNEAR * );
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
                    if( str == NULL ) {
                        *msg = "%s -> NULL";
                        break;  /* bail out */
                    }
                }
#endif  /* __STDC_WANT_LIB_EXT1__ */

                arg = formstring( buffer, &args, &specs, &null_char );
                specs._fld_width -= specs._n0  +
                                    specs._nz0 +
                                    specs._n1  +
                                    specs._nz1 +
                                    specs._n2  +
                                    specs._nz2;
                if( !(specs._flags & SPF_LEFT_ADJUST) ) {
                    if( specs._pad_char == STRING( ' ' ) ) {
                        while( specs._fld_width > 0 ) {
                            (out_putc)( &specs, STRING( ' ' ) );
                            --specs._fld_width;
                        }
                    }
                }
                a = buffer;
                while( specs._n0 > 0 ) {
                    (out_putc)( &specs, *a );
                    ++a;
                    --specs._n0;
                }
                while( specs._nz0 > 0 ) {
                    (out_putc)( &specs, STRING( '0' ) );
                    --specs._nz0;
                }
                if( specs._character == STRING( 's' ) ) {
#if defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
                    if( specs._flags & SPF_SHORT ) {
                        write_skinny_string( (FAR_ASCII_STRING)arg, &specs, out_putc );
                    } else
#elif !defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
                    if( specs._flags & SPF_LONG ) {
                        write_wide_string( (FAR_WIDE_STRING)arg, &specs, out_putc );
                    } else
#endif
                    {
                        while( specs._n1 > 0 ) {
                            (out_putc)( &specs, *arg++ );
                            --specs._n1;
                        }
                    }
                }
#if !defined( __WIDECHAR__ ) && defined( CLIB_USE_MBCS_TRANSLATION )
                else if( specs._character == WIDE_CHAR_STRING ) {
                    write_wide_string( (FAR_WIDE_STRING)arg, &specs, out_putc );
                } else
#elif !defined( __WIDECHAR__ ) && defined( __NETWARE__ )
                else if( specs._character == WIDE_CHAR_STRING ) {
                } else
#endif
                {
                    while( specs._n1 > 0 ) {
                        (out_putc)( &specs, *arg++ );
                        --specs._n1;
                    }
                }
                while( specs._nz1 > 0 ) {
                    (out_putc)( &specs, STRING( '0' ) );
                    --specs._nz1;
                }
                while( specs._n2 > 0 ) {
                    (out_putc)( &specs, *arg );
                    ++arg;
                    --specs._n2;
                }
                while( specs._nz2 > 0 ) {
                    (out_putc)( &specs, STRING( '0' ) );
                    --specs._nz2;
                }
                if( specs._flags & SPF_LEFT_ADJUST ) {
                    while( specs._fld_width > 0 ) {
                        (out_putc)( &specs, STRING( ' ' ) );
                        --specs._fld_width;
                    }
                }
            }
        }
    }
    return( specs._output_count );
}

#if defined( __QNX__ ) && !defined( __STDC_WANT_LIB_EXT1__ ) && !defined( __WIDECHAR__ ) && defined( IN_SLIB )
int __F_NAME(__prtf_slib,__wprtf_slib)( void PTR_PRTF_FAR dest, const CHAR_TYPE *format, va_list *pargs, prtf_callback_t *out_putc, int ptr_size )
{
    /* unused parameters */ (void)ptr_size;

    return( __F_NAME(__prtf,__wprtf)( dest, format, *pargs, out_putc ) );
}
#endif

#endif

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


/*
 * __prtf -- CLIB low level string formatter
 */

#define __LONG_LONG_SUPPORT__

#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mbstring.h>
#include "ftos.h"
#if defined(__LONG_LONG_SUPPORT__)
    #include "clibi64.h"
#endif
#include "farsupp.h"
#include "printf.h"
#include "fixpoint.h"
#include "myvalist.h"

#if defined(__QNX_286__) && !defined(__SHARE__)
    // this check eliminates this file for 16bit non-share QNX
#else

#ifdef IN_SLIB
    #include <sys/magic.h>
#endif

#define BUF_SIZE 40
#define MAX_WIDTH 39
#define TRUE 1
#define FALSE 0

#define PASCAL_STRING           'S'             /* for Novell */
#define WIDE_CHAR_STRING        'S'
#if !defined(__NETWARE__) && !defined(__QNX__)
    #define USE_MBCS_TRANSLATION
#endif

#if defined(__QNX_386__)
/* for use in QNX 32-bit shared library */
#pragma aux __prtf "_sl_*" far;
#endif


typedef int             bool;

#if defined(__PENPOINT__)
  #define EFG_PRINTF _EFG_Format
#elif defined(__QNX__)
  #define EFG_PRINTF __EFG_Format
#else
  #define EFG_PRINTF (*__EFG_printf)
#endif
extern FAR_STRING EFG_PRINTF( char *buffer, my_va_list *args, _mbcs_SPECS __SLIB *specs );


#if defined( __WIDECHAR__ )
  #define _FAR_OTHER_STRING             FAR_ASCII_STRING
#else
  #define _FAR_OTHER_STRING             FAR_UNI_STRING
#endif

#ifdef IN_SLIB
    #include <i86.h>
    #define FUNC            __F_NAME(__prtf_slib,__wprtf_slib)
    #define CVT_NEAR(p)     MK_FP( __MAGIC.dgroup ,(unsigned)(p) )
#else
    #define FUNC            __F_NAME(__prtf,__wprtf)
    #define CVT_NEAR(p)     ((void *)(p))
#endif


#if defined(__WINDOWS_386__)
    #ifdef __SW_3S
        #pragma aux slib_callback_t modify [eax edx ecx fs gs];
    #else
        #pragma aux slib_callback_t modify [fs gs];
    #endif
#endif


/* forward references */
static const CHAR_TYPE *evalflags( const CHAR_TYPE *, SPECS __SLIB * );
static FAR_STRING formstring( CHAR_TYPE *, my_va_list *, SPECS __SLIB *, CHAR_TYPE * );
static const CHAR_TYPE * getprintspecs( const CHAR_TYPE *, my_va_list *, SPECS __SLIB * );
#ifdef USE_MBCS_TRANSLATION
static void write_wide_string( FAR_UNI_STRING str, SPECS *specs,
                               slib_callback_t *out_putc );
static void write_skinny_string( FAR_ASCII_STRING str, SPECS *specs,
                                 slib_callback_t *out_putc );
#endif



int (FUNC)(     void __SLIB *dest,              /* parm for use by out_putc */
                const CHAR_TYPE *format,                /* pointer to format string */
                va_list args,                   /* pointer to pointer to args*/
                slib_callback_t *out_putc       /* char output routine */
#ifdef IN_SLIB
                ,int ptr_size
#endif
              )
{
    auto CHAR_TYPE buffer[ BUF_SIZE ];
    auto CHAR_TYPE null_char = '\0';
    register CHAR_TYPE *a;
    FAR_STRING arg;
    const CHAR_TYPE *ctl;
    SPECS specs;

#ifdef IN_SLIB
    va_list farg;

    if( __MAGIC.dgroup == 0 ) {
        __MAGIC.dgroup = *(unsigned short *)MK_FP(0x78,sizeof(char _WCNEAR *));
    }
    if ( ptr_size == sizeof( void _WCNEAR * ) )
         farg[0] = CVT_NEAR( args[0] );
    else farg[0] = args[0];
    args = farg;
#endif

    specs._o._dest = dest;
    specs._flags = 0;
    specs._version = SPECS_VERSION;
    specs._o._output_count = 0;
    ctl = format;
    while( *ctl != NULLCHAR ) {
        if( *ctl != '%' ) {
            (out_putc)( &specs, *ctl++ );
        } else {
            ++ctl;
            {
                my_va_list pargs;
                pargs = MY_VA_LIST( args );
                ctl = getprintspecs( ctl, &pargs, &specs );
                MY_VA_LIST( args ) = pargs;
            }

#if defined( __FAR_SUPPORT__ )
#ifdef IN_SLIB
            /*
               For the shared library, all pointers are assumed FAR unless
               explicitly set NEAR.
            */
            if ( !(specs._flags & (SPF_FAR | SPF_NEAR)) )
                specs._flags |=
                    (ptr_size == sizeof(void _WCFAR *)) ? SPF_FAR : SPF_NEAR;
#endif
#endif

            specs._o._character = *ctl++;
            if( specs._o._character == NULLCHAR ) break;        /* 05-jan-89 */
            if( specs._o._character == 'n' ) {
                if( specs._flags & SPF_LONG ) {
#if defined( __FAR_SUPPORT__ )
                    if( specs._flags & SPF_FAR ) {
                        *va_arg( args, long int _WCFAR *) = specs._o._output_count;
                    } else if( specs._flags & SPF_NEAR ) {
                        a =  CVT_NEAR( va_arg( args, long int _WCNEAR * ) );
                        *(long int *)a = specs._o._output_count;
                    } else {
                        *va_arg( args, long int *) = specs._o._output_count;
                    }
#else
                    *va_arg( args, long int * ) = specs._o._output_count;
#endif
                } else if( specs._flags & SPF_SHORT ) {     /* JBS 92/02/12 */
#if defined( __FAR_SUPPORT__ )
                    if( specs._flags & SPF_FAR ) {
                        *va_arg( args, short int _WCFAR *) = specs._o._output_count;
                    } else if( specs._flags & SPF_NEAR ) {
                        a =  CVT_NEAR( va_arg( args, short int _WCNEAR * ) );
                        *(short int *)a = specs._o._output_count;
                    } else {
                        *va_arg( args, short int *) = specs._o._output_count;
                    }
#else
                    *va_arg( args, short int * ) = specs._o._output_count;
#endif
                } else {
#if defined( __FAR_SUPPORT__ )
                    if( specs._flags & SPF_FAR ) {
                        *va_arg( args, int _WCFAR *) = specs._o._output_count;
                    } else if( specs._flags & SPF_NEAR ) {
                        a =  CVT_NEAR( va_arg( args, long int _WCNEAR * ) );
                        *(int *)a = specs._o._output_count;
                    } else {
                        *va_arg( args, int *) = specs._o._output_count;
                    }
#else
                    *va_arg( args, int *) = specs._o._output_count;
#endif
                }
            } else {
                {
                    my_va_list pargs;
                    pargs = MY_VA_LIST( args );
                    arg = formstring( buffer, &pargs, &specs, &null_char );
                    MY_VA_LIST( args ) = pargs;
                }
                specs._o._fld_width -= specs._n0  +
                                    specs._nz0 +
                                    specs._n1  +
                                    specs._nz1 +
                                    specs._n2  +
                                    specs._nz2;
                if( !(specs._flags & SPF_LEFT_ADJUST) ) {
                    if( specs._o._pad_char == ' ' ) {
                        while( specs._o._fld_width > 0 ) {
                            (out_putc)( &specs, ' ' );
                            --specs._o._fld_width;
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
                    (out_putc)( &specs, '0' );
                    --specs._nz0;
                }
                if( specs._o._character == 's' ) {
#if defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
                    if( specs._flags & SPF_SHORT ) {
                        write_skinny_string( (FAR_ASCII_STRING)arg, &specs, out_putc );
                    } else
#elif !defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
                    if( specs._flags & SPF_LONG ) {
                        write_wide_string( (FAR_UNI_STRING)arg, &specs, out_putc );
                    } else
#endif
                    {
                        while( specs._n1 > 0 ) {
                            (out_putc)( &specs, *arg++ );
                            --specs._n1;
                        }
                    }
                }
#if !defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
                else if( specs._o._character == WIDE_CHAR_STRING ) {
                    write_wide_string( (FAR_UNI_STRING)arg, &specs, out_putc );
                } else
#elif !defined(__WIDECHAR__) && defined(__NETWARE__)
                else if( specs._o._character == WIDE_CHAR_STRING ) {
                } else
#endif
                {
                    while( specs._n1 > 0 ) {
                        (out_putc)( &specs, *arg++ );
                        --specs._n1;
                    }
                }
                while( specs._nz1 > 0 ) {
                    (out_putc)( &specs, '0' );
                    --specs._nz1;
                }
                while( specs._n2 > 0 ) {
                    (out_putc)( &specs, *arg );
                    ++arg;
                    --specs._n2;
                }
                while( specs._nz2 > 0 ) {
                    (out_putc)( &specs, '0' );
                    --specs._nz2;
                }
                if( specs._flags & SPF_LEFT_ADJUST ) {
                    while( specs._o._fld_width > 0 ) {
                        (out_putc)( &specs, ' ' );
                        --specs._o._fld_width;
                    }
                }
            }
        }
    }
    return( specs._o._output_count );
}

static const CHAR_TYPE * getprintspecs( const CHAR_TYPE *ctl,
                                    my_va_list *pargs,
                                    SPECS __SLIB *specs )
{
    specs->_o._pad_char = ' ';
    ctl = evalflags( ctl, specs );
    specs->_o._fld_width = 0;
    if( *ctl == '*' ) {
        specs->_o._fld_width = va_arg( pargs->v, int );
        if( specs->_o._fld_width < 0 ) {
            specs->_o._fld_width = - specs->_o._fld_width;
            specs->_flags |= SPF_LEFT_ADJUST;
        }
        ctl++;
    } else {
        while( *ctl >= '0'  &&  *ctl <= '9' ) {
            specs->_o._fld_width = specs->_o._fld_width * 10 + ( *ctl++ - '0' );
        }
    }
    specs->_o._prec = -1;
    if( *ctl == '.' ) {
        specs->_o._prec = 0;
        ctl++;
        if( *ctl == '*' ) {
            specs->_o._prec = va_arg( pargs->v, int );
            if( specs->_o._prec < 0 )  specs->_o._prec = -1;    /* 19-jul-90 */
            ctl++;
        } else {
            while( *ctl >= '0'  &&  *ctl <= '9' ) {
                specs->_o._prec = specs->_o._prec * 10 + ( *ctl++ - '0' );
            }
        }
        /*
        "For d, i, o, u, x, X, e, E, f, g and G conversions, leading
        zeros (following any indication of sign or base) are used to
        pad the field width; no space padding is performed. If the 0
        or - flags both appear, the 0 flag is ingnored. For d, i, o,
        u, x or X conversions, if a precision is specified, the 0
        flag is ignored. For other conversions, the behaviour is
        undefined."
        */
//      if( specs->_o._prec != -1 )  specs->_o._pad_char = ' '; /* 30-jul-95 *//*removed by JBS*/
    }
    switch( *ctl ) {
    case 'l':
    case 'w':
        specs->_flags |= SPF_LONG;
        ctl++;
        break;
    case 'h':
        specs->_flags |= SPF_SHORT;
        ctl++;
        break;
#if defined(__LONG_LONG_SUPPORT__)
    case 'I':
        if( ctl[1] == '6' && ctl[2] == '4' ) {
            specs->_flags |= SPF_LONG_DOUBLE;
            ctl += 3;
        }
        break;
#endif
    case 'L':
        specs->_flags |= SPF_LONG_DOUBLE;
        ctl++;
        break;
#if defined( __FAR_SUPPORT__ )
    case 'F':                   /* 8086 specific flag for FAR pointer */
        specs->_flags |= SPF_FAR;
        ctl++;
        break;
    case 'N':                   /* 8086 specific flag for NEAR pointer */
        specs->_flags |= SPF_NEAR;
        ctl++;
        break;
#endif
    }
    return( ctl );
}


static const CHAR_TYPE *evalflags( const CHAR_TYPE *ctl, SPECS __SLIB *specs )
{
    specs->_flags = 0;
    for(;; ctl++ ) {
        if( *ctl == '-' ) {
            specs->_flags |= SPF_LEFT_ADJUST;
        } else if( *ctl == '#' ) {
            specs->_flags |= SPF_ALT;
        } else if( *ctl == '+' ) {
            specs->_flags |= SPF_FORCE_SIGN;
            specs->_flags &= ~SPF_BLANK;
        } else if( *ctl == ' ' ) {
            if( ( specs->_flags & SPF_FORCE_SIGN ) == 0 ) {
                specs->_flags |= SPF_BLANK;
            }
        } else if( *ctl == '0' ) {
            specs->_o._pad_char = '0';
#ifdef __QNX__
            specs->_flags |= SPF_ZERO_PAD;
#endif
        } else {
            break;
        }
    }
    return( ctl );
}


static int far_strlen( FAR_STRING s, int precision )
{
    register int len;

    len = 0;
    while( *s++ != NULLCHAR  &&  len != precision )  ++len;
    return( len );
}

/*
 * far_other_strlen - calculates the length of an ascii string
 *                     for the unicode version
 *                  - calculates the length of a unicode string for
 *                    the standard version
 */

static int far_other_strlen( FAR_STRING s, int precision )
{
    #if !defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
        char                mbBuf[MB_CUR_MAX];
        int                 chBytes;
        int                 totalBytes = 0;
        _FAR_OTHER_STRING   ptr = (_FAR_OTHER_STRING)s;

        if( precision == -1 ) {
            while( *ptr ) {
                chBytes = wctomb( mbBuf, *ptr++ );
                if( chBytes != -1 )  totalBytes += chBytes;
            }
            return( totalBytes );
        } else {
            while( *ptr  &&  totalBytes <= precision ) {
                chBytes = wctomb( mbBuf, *ptr++ );
                if( chBytes != -1 )  totalBytes += chBytes;
            }
            return( totalBytes<=precision ? totalBytes : precision );
        }
    #else
        int                 len;
        _FAR_OTHER_STRING   ptr;

        len = 0;
        ptr = (_FAR_OTHER_STRING) s;
        while( *ptr++ != '\0'  &&  len != precision )  ++len;
        return( len );
    #endif
}

static void fmt4hex( unsigned value, CHAR_TYPE *buf, int maxlen )
{
    register int i, len;

    __F_NAME(itoa,_itow)( value, buf, 16 );
    len = __F_NAME(strlen,wcslen)( buf );
    for( i = maxlen - 1; len; --i ) {
        --len;
        buf[i] = buf[len];
    }
    while( i >= 0 ) {
        buf[i] = '0';
        --i;
    }
    buf[maxlen] = NULLCHAR;
}


static void FixedPoint_Format( CHAR_TYPE *buf, long value, SPECS __SLIB *specs )
{
    T32         at;
    int         i;
    CHAR_TYPE   *bufp;

    at.sWhole = value;
    if( at.sWhole < 0 ) {
        at.sWhole = - at.sWhole;
        *buf++ = '-';
    }
    if( specs->_o._prec == -1 ) specs->_o._prec = 4;
    __F_NAME(itoa,_itow)( at.wd.hi, buf, 10 );
    bufp = buf;         /* remember start address of buffer */
    while( *buf ) ++buf;
    if( specs->_o._prec != 0 ) {
        *buf++ = '.';
        for( i = 0; i < specs->_o._prec; i++ ) {
            at.wd.hi = 0;
            at.uWhole *= 10;
            *buf++ = at.bite.b3 + '0';
        }
        *buf = NULLCHAR;
    }
    if( at.wd.lo & 0x8000 ) {   /* fraction >= .5, need to round */
        for(;;) {                               /* 22-dec-91 */
            if( buf == bufp ) {
                *buf++ = '1';
                while( *buf == '0' ) ++buf;
                if( *buf == '.' ) {
                    *buf++ = '0';
                    *buf++ = '.';
                    while( *buf == '0' ) ++buf;
                }
                *buf++ = '0';
                *buf = NULLCHAR;
                break;
            }
            --buf;
            if( *buf == '.' ) --buf;
            if( *buf != '9' ) {
                ++ *buf;
                break;
            }
            *buf = '0';
        }
    }
}

static void float_format( CHAR_TYPE *buffer, my_va_list *pargs, SPECS __SLIB *specs )
{
#ifdef __QNX__
    FAR_STRING          arg;
#endif
#ifdef __WIDECHAR__
    char                mbBuffer[BUF_SIZE*MB_CUR_MAX];
    _mbcs_SPECS         mbSpecs;
    int                 count;
    size_t              rc;
    char *              p;
#endif // __WIDECHAR__

    char tmp;

    /*
     * We need specs._o._alt_prefix[0] to be !0 so that the efg_format()
     * function can always tell the difference between a 11.0 and pre-11.0
     * specs structure.
     */
    tmp = specs->_o._alt_prefix[0];
    specs->_o._alt_prefix[0] = 0xff;

    /*
     * In case we are going to be using a 10.5 efgfmt() function, we need
     * to set the flag variable in the SPECS105 structure.
     */
    specs->_o._flags = (char) specs->_flags;

#ifdef __WIDECHAR__
    /*
     * EFG_PRINTF can only handle MBCS buffers and the MBCS version of the
     * SPECS structure.  So, make a _mbcs_SPECS structure equivalent to the
     * _wide_SPECS one, and use that instead.  Note that we can't use
     * memcpy() because some field sizes are different.
     */
    mbSpecs._o._dest = NULL;            /* this field isn't actually used */
    mbSpecs._o._fld_width = specs->_o._fld_width;
    mbSpecs._o._prec = specs->_o._prec;
    mbSpecs._o._zero_fill_count = specs->_o._zero_fill_count;
    mbSpecs._o._output_count = specs->_o._output_count;
    mbSpecs._o._flags = (char) specs->_o._flags;
    mbSpecs._o._character = (char) specs->_o._character;
    mbSpecs._o._pad_char = (char) specs->_o._pad_char;
    mbSpecs._o._alt_prefix[0] = (char) specs->_o._alt_prefix[0];
    mbSpecs._o._alt_prefix[1] = (char) specs->_o._alt_prefix[1];
    mbSpecs._o._alt_prefix[2] = (char) specs->_o._alt_prefix[2];
    mbSpecs._unused[0] = specs->_unused[0];
    mbSpecs._unused[1] = specs->_unused[1];
    mbSpecs._version = specs->_version;
    mbSpecs._flags = specs->_flags;
    mbSpecs._n0 = specs->_n0;
    mbSpecs._nz0 = specs->_nz0;
    mbSpecs._n1 = specs->_n1;
    mbSpecs._nz1 = specs->_nz1;
    mbSpecs._n2 = specs->_n2;
    mbSpecs._nz2 = specs->_nz2;
#endif

#ifdef __QNX__
    #ifdef __WIDECHAR__
        arg = EFG_PRINTF( mbBuffer, pargs, &mbSpecs );
    #else
        arg = EFG_PRINTF( buffer, pargs, specs );
    #endif
#else
    #ifdef __WIDECHAR__
        EFG_PRINTF( mbBuffer, pargs, &mbSpecs );
    #else
        EFG_PRINTF( buffer, pargs, specs );
    #endif
#endif

#ifdef __WIDECHAR__
    /*
     * Now convert the returned information back into our _wide_SPECS
     * structure.  We can't just use mbstowcs because it's an array of
     * characters, not a string.
     */
    p = mbBuffer;
    for( count=0; count<BUF_SIZE; count++ ) {
        rc = mbtowc( &(buffer[count]), p, MB_CUR_MAX );
        if( rc == -1 ) {
            buffer[count] = L'?';
        }
        p = _mbsinc( p );
    }
    specs->_o._fld_width = mbSpecs._o._fld_width;
    specs->_o._prec = mbSpecs._o._prec;
    specs->_o._zero_fill_count = mbSpecs._o._zero_fill_count;
    specs->_o._output_count = mbSpecs._o._output_count;
    specs->_o._flags = (wchar_t) mbSpecs._o._flags;
    specs->_o._character = (wchar_t) mbSpecs._o._character;
    specs->_o._pad_char = (wchar_t) mbSpecs._o._pad_char;
    specs->_o._alt_prefix[0] = (wchar_t) mbSpecs._o._alt_prefix[0];
    specs->_o._alt_prefix[1] = (wchar_t) mbSpecs._o._alt_prefix[1];
    specs->_o._alt_prefix[2] = (wchar_t) mbSpecs._o._alt_prefix[2];
    specs->_unused[0] = mbSpecs._unused[0];
    specs->_unused[1] = mbSpecs._unused[1];
    specs->_version = mbSpecs._version;
    specs->_flags = mbSpecs._flags;
    specs->_n0 = mbSpecs._n0;
    specs->_nz0 = mbSpecs._nz0;
    specs->_n1 = mbSpecs._n1;
    specs->_nz1 = mbSpecs._nz1;
    specs->_n2 = mbSpecs._n2;
    specs->_nz2 = mbSpecs._nz2;
#endif

#ifdef __QNX__
    /*
     * If _o._alt_prefix[0] == 0xff then we know just called a pre-11.0
     * efgfmt() (since the 11.0 efgfmt() always resets _o._alt_prefix[0]
     * to 0x00 before returning.
     */
    if (specs->_o._alt_prefix[0] == 0xff)
    {
        int length;

        /*
         * Copy the flags set by the pre-11.0 efgfmt() back into the new
         * 11.0 flags variable.
         */
        specs->_flags = (specs->_flags & SPF_ZERO_PAD) | specs->_o._flags;

        /*
         * Under the pre-11.0 efgfmt() the output string is right flushed
         * in the buffer.  Copy it to the head of the buffer if required.
         */
        if (arg != buffer)
        {
            CHAR_TYPE *tmp = buffer;

            while (*arg)
                *tmp++ = *arg++;

            *tmp = NULLCHAR;
        }

        length = __F_NAME(strlen,wcslen)( buffer );

        if ((*buffer == '-') || (*buffer == '+') || (*buffer == ' '))
            specs->_n0 = 1;
        else
            specs->_n0 = 0;

        if (specs->_flags & SPF_ZERO_PAD)
        {
            specs->_o._pad_char = '0';

            if (!(specs->_flags & SPF_LEFT_ADJUST))
                specs->_nz0 = specs->_o._fld_width - length;
        }

        specs->_n1  = length - specs->_n0;
    }
#endif // _QNX

    specs->_o._alt_prefix[0] = tmp;
}

static void SetZeroPad( SPECS __SLIB *specs )
{
    int         n;

    if( !(specs->_flags & SPF_LEFT_ADJUST) ) {
        if( specs->_o._pad_char == '0' ) {
            n = specs->_o._fld_width - specs->_n0 - specs->_nz0 -
                         specs->_n1 - specs->_nz1 - specs->_n2 - specs->_nz2;
            if( n > 0 ) {
                specs->_nz0 += n;
            }
        }
    }
}


#if !defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
static void write_wide_string( FAR_UNI_STRING str, SPECS *specs,
                               slib_callback_t *out_putc )
{
    int                 bytes;
    char                mbBuf[MB_CUR_MAX];
    char *              mbBufPtr;

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


#if defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
static void write_skinny_string( FAR_ASCII_STRING str, SPECS *specs,
                                 slib_callback_t *out_putc )
{
    int                 bytes;
    wchar_t             wc;
    FAR_ASCII_STRING    mbPtr = str;
    char                mbBuf[MB_CUR_MAX];

    while( specs->_n1 > 0 ) {
        mbBuf[0] = *mbPtr++;
        if( _ismbblead( mbBuf[0] ) )  mbBuf[1] = *mbPtr++;
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


static FAR_STRING formstring( CHAR_TYPE *buffer, my_va_list *pargs,
                              SPECS __SLIB *specs, CHAR_TYPE *null_string )
{
    FAR_STRING          arg;
    int                 length;
    int                 radix;
#if defined(__LONG_LONG_SUPPORT__)
    auto UINT64_TYPE    long_long_value;
#endif
    auto unsigned long  long_value;
    auto unsigned int   int_value;
#if defined( __FAR_SUPPORT__ )
    auto unsigned int   seg_value;
#endif
#if !defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
    int                 bytes;
#endif

    arg = buffer;
    specs->_n0  = 0;
    specs->_nz0 = 0;
    specs->_n1  = 0;
    specs->_nz1 = 0;
    specs->_n2  = 0;
    specs->_nz2 = 0;
    switch( specs->_o._character ) {
    case 'o':
    case 'u':
    case 'x':
    case 'X':
#if defined(__LONG_LONG_SUPPORT__)
        if( specs->_flags & SPF_LONG_DOUBLE ) {
            #ifdef USE_INT64
                long_long_value = va_arg( pargs->v, UINT64_TYPE );
            #else
                long_long_value.u._32[I64LO32] = va_arg( pargs->v, unsigned long );
                long_long_value.u._32[I64HI32] = va_arg( pargs->v, unsigned long );
            #endif
        } else
#endif
        if( specs->_flags & SPF_LONG ) {
            long_value = va_arg( pargs->v, unsigned long );
        } else {
            long_value = va_arg( pargs->v, unsigned );
            if( specs->_flags & SPF_SHORT ) {    /* JBS 92/02/12 */
                long_value = (unsigned short) long_value;
            }
        }
        break;
    case 'd':
    case 'i':
#if defined(__LONG_LONG_SUPPORT__)
        if( specs->_flags & SPF_LONG_DOUBLE ) {
            #ifdef USE_INT64
                long_long_value = va_arg( pargs->v, INT64_TYPE );
            #else
                long_long_value.u._32[I64LO32] = va_arg( pargs->v, unsigned long );
                long_long_value.u._32[I64HI32] = va_arg( pargs->v, unsigned long );
            #endif
        } else
#endif
        if( specs->_flags & SPF_LONG ) {
            long_value = va_arg( pargs->v, long );
        } else {
            long_value = va_arg( pargs->v, int );
            if( specs->_flags & SPF_SHORT ) {    /* JBS 92/02/12 */
                long_value = (short) long_value;
            }
        }
        {
            int negative = 0;
#if defined(__LONG_LONG_SUPPORT__)
            if( specs->_flags & SPF_LONG_DOUBLE ) {
                if( _clib_I64Negative(long_long_value) ) {
                    negative = TRUE;
                }
            } else
#endif
            if( (long)long_value < 0 ) {
                negative = TRUE;
            }
            if( negative ) {
                buffer[specs->_n0++] = '-';
#if defined(__LONG_LONG_SUPPORT__)
                if( specs->_flags & SPF_LONG_DOUBLE ) {
                    _clib_I64Neg( long_long_value, long_long_value );
                } else
#endif
                long_value = - long_value;
            } else if( specs->_flags & SPF_FORCE_SIGN ) {
                buffer[specs->_n0++] = '+';
            } else if( specs->_flags & SPF_BLANK ) {
                buffer[specs->_n0++] = ' ';
            }
        }
        break;
    }

    radix  = 10;                        /* base 10 for 'd', 'i' and 'u' */
    switch( specs->_o._character ) {
    case 'f':
    case 'F':
        if( specs->_flags & SPF_SHORT ) {       /* "%hf"  13-jun-91 */
            long_value = va_arg( pargs->v, long );
            FixedPoint_Format( buffer, long_value, specs );
            specs->_n1 = far_strlen( buffer, -1 );
            break;
        }
    case 'g':
    case 'G':
    case 'e':
    case 'E':
        float_format( buffer, pargs, specs );
        SetZeroPad( specs );
        arg = &buffer[1];
        break;
    case 's':
  #ifndef __NETWARE__
    case WIDE_CHAR_STRING:
  #else
    case PASCAL_STRING:
  #endif
        // arg has been initialized to point to buffer
        // set buffer[0] to a null character assuming pointer will be NULL
        // If pointer is not null, then arg will be assigned the pointer
        buffer[0] = '\0';                       // assume null pointer
#if defined( __FAR_SUPPORT__ )
        if( specs->_flags & SPF_FAR ) {
            CHAR_TYPE _WCFAR *temp = va_arg( pargs->v, CHAR_TYPE _WCFAR * );

            if( temp )  arg = temp;
        } else if( specs->_flags & SPF_NEAR ) {
            CHAR_TYPE _WCNEAR *temp = va_arg( pargs->v, CHAR_TYPE _WCNEAR * );

            if( temp )  arg = CVT_NEAR( temp );
        } else {
            CHAR_TYPE *temp = va_arg( pargs->v, CHAR_TYPE * );

            if( temp )  arg = temp;
        }
#else
        {
            CHAR_TYPE *temp = va_arg( pargs->v, CHAR_TYPE * );

            if( temp )  arg = temp;
        }
#endif

        #ifdef __NETWARE__
            if( specs->_o._character == PASCAL_STRING ) {
              #ifdef __WIDECHAR__
                if( specs->_flags & SPF_SHORT ) {
              #else
                if( specs->_flags & SPF_LONG ) {
              #endif
                    length = *( (_FAR_OTHER_STRING)arg );
                    arg = (FAR_STRING)( (_FAR_OTHER_STRING)arg + 1 );
                } else {
                    length = *arg++;
                }
            } else
        #elif !defined(__NETWARE__) && !defined(__WIDECHAR__)
            if( specs->_o._character == WIDE_CHAR_STRING ) {
                if( specs->_flags & SPF_SHORT ) {
                    length = far_strlen( arg, specs->_o._prec );
                } else {
                    length = far_other_strlen( arg, specs->_o._prec );
                }
            } else
        #endif

            {
              #ifdef __WIDECHAR__
                if( specs->_flags & SPF_SHORT ) {
              #else
                if( specs->_flags & SPF_LONG ) {
              #endif
                    length = far_other_strlen( arg, specs->_o._prec );
                } else {
                    length = far_strlen( arg, specs->_o._prec );
                }
            }
        specs->_n1 = length;
        if( specs->_o._prec >= 0  &&  specs->_o._prec < length ) {
            specs->_n1 = specs->_o._prec;
        }
        break;
    case 'x':
    case 'X':
        if( specs->_flags & SPF_ALT ) {
#if defined(__LONG_LONG_SUPPORT__)
            if( specs->_flags & SPF_LONG_DOUBLE ) {
                if( !_clib_U64Zero(long_long_value) ) {
                    buffer[specs->_n0++] = '0';
                    buffer[specs->_n0++] = specs->_o._character;
                }
            } else
#endif
            if( long_value != 0 ) {
                buffer[specs->_n0++] = '0';
                buffer[specs->_n0++] = specs->_o._character;
            }
        }
        radix = 16;                     /* base 16 */
    case 'o':
        if( specs->_o._character == 'o' ) {
            radix = 8;                  /* base 8 */
            if( specs->_flags & SPF_ALT ) {
                buffer[specs->_n0++] = '0';
            }
        }
    case 'd':
    case 'i':
    case 'u':
        if( specs->_o._prec != -1 )  specs->_o._pad_char = ' '; /* 30-jul-95, 11-may-99 */
        /* radix contains the base; 8 for 'o', and 10 for 'd' and 'i' */
        arg = &buffer[ specs->_n0 ];
#if defined(__LONG_LONG_SUPPORT__)
        if( specs->_flags & SPF_LONG_DOUBLE ) {
            if( specs->_o._prec == 0 && _clib_U64Zero(long_long_value) ) {
                *arg = '\0';
                length = 0;
            } else {
                __F_NAME(__clib_ulltoa,__clib_wulltoa)( &long_long_value, &buffer[specs->_n0], radix );
                if( specs->_o._character == 'X' ) {
                    zupstr( buffer );
                }
                length = far_strlen( arg, -1 );
            }
        } else
#endif
        if( specs->_o._prec == 0  &&  long_value == 0 ) {
            *arg = '\0';
            length = 0;
        } else {
            __F_NAME(ultoa,_ultow)( long_value, &buffer[specs->_n0], radix );
            if( specs->_o._character == 'X' ) {
                zupstr( buffer );
            }
            length = far_strlen( arg, -1 );
        }
        specs->_n1 = length;
        if( specs->_n1 < specs->_o._prec ) {
            specs->_nz0 = specs->_o._prec - specs->_n1;
        }
        if( specs->_o._prec == -1 ) {
            SetZeroPad( specs );
        }
        break;
      case 'p':
      case 'P':
#if defined( __FAR_SUPPORT__ )
  #if defined(__BIG_DATA__)
        if( !( specs->_flags & (SPF_NEAR|SPF_FAR) ) ) {
            specs->_flags |= SPF_FAR;
        }
        if( specs->_o._fld_width == 0 ) {
            if( specs->_flags & SPF_NEAR ) {
                specs->_o._fld_width = sizeof(unsigned)*2;
            } else {
                specs->_o._fld_width = sizeof(CHAR_TYPE _WCFAR *) * 2 + 1;
            }
        }
  #else
        if( specs->_o._fld_width == 0 ) {
            if( specs->_flags & SPF_FAR ) {
                specs->_o._fld_width = sizeof(CHAR_TYPE _WCFAR *) * 2 + 1;
            } else {
                specs->_o._fld_width = sizeof(unsigned)*2;
            }
        }
  #endif
#else
        if( specs->_o._fld_width == 0 ) {
            specs->_o._fld_width = sizeof(unsigned)*2;
        }
#endif
        specs->_flags &= ~( SPF_BLANK | SPF_FORCE_SIGN );
        int_value = va_arg( pargs->v, unsigned );               /* offset */
#if defined( __FAR_SUPPORT__ )
        if( specs->_flags & SPF_FAR ) {
            seg_value = va_arg( pargs->v, unsigned ) & 0xFFFF; /* segment */
            /* use "unsigned short" for 386 instead of "unsigned" 21-jul-89 */
            fmt4hex( seg_value, buffer, sizeof(unsigned short)*2 );
            buffer[sizeof(unsigned short)*2] = ':';
            fmt4hex( int_value, buffer + sizeof(unsigned short)*2 + 1,
                       sizeof(unsigned)*2 );
        } else {
            fmt4hex( int_value, buffer, sizeof(unsigned)*2 );
        }
#else
        fmt4hex( int_value, buffer, sizeof(unsigned)*2 );
#endif
        if( specs->_o._character == 'P' ) {
            zupstr( buffer );
        }
        specs->_n0 = far_strlen( arg, -1 );
        break;
    case 'c':
#if defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
        if( specs->_flags & SPF_SHORT ) {
            char *      mbPtr;
            char        mbBuf[MB_CUR_MAX];
            wchar_t     wc;

            mbPtr = va_arg( pargs->v, char* );
            mbBuf[0] = mbPtr[0];
            if( _ismbblead( mbBuf[0] ) )  mbBuf[1] = mbPtr[1];
            if( mbtowc( &wc, mbBuf, MB_CUR_MAX ) != -1 )  buffer[0] = wc;
        } else {
            buffer[0] = va_arg( pargs->v, int );
        }
        specs->_n0 = 1;
#elif !defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
        specs->_n0 = 1;
        if( specs->_flags & SPF_LONG ) {
            char        mbBuf[MB_CUR_MAX];
            wchar_t     wc;

            wc = va_arg( pargs->v, int );
            if( wctomb( mbBuf, wc ) != -1 ) {
                buffer[0] = mbBuf[0];
                if( _ismbblead( mbBuf[0] ) ) {
                    buffer[1] = mbBuf[1];
                    specs->_n0++;
                }
            }
        } else {
            buffer[0] = va_arg( pargs->v, int );
        }
#else
        specs->_n0 = 1;
        buffer[0] = va_arg( pargs->v, int );
#endif
        break;
  #if !defined(__WIDECHAR__) && defined(USE_MBCS_TRANSLATION)
    case 'C':
        bytes = wctomb( buffer, va_arg( pargs->v, int ) );
//      if( bytes != -1  &&  bytes <= specs->_o._prec ) {
        if( bytes != -1 ) { /* Normative Addendum 4.5.3.3.1: no precision */
            specs->_n0 = bytes;
        } else {
            specs->_n0 = 0;
        }
        break;
  #endif
    default:
        specs->_o._fld_width = 0;
        buffer[ 0 ] = specs->_o._character;
        specs->_n0 = 1;
        break;
    }
    return( arg );
}


static void zupstr( CHAR_TYPE *s )
{
    while( *s ) {
        *s = __F_NAME(toupper,towupper)( *s );
        ++s;
    }
}

#endif

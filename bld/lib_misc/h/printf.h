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
 * printf.h --  definitions needed by callers to internal string
 *              formatter __prtf()
 */
#ifndef _PRINTF_H_INCLUDED
#define _PRINTF_H_INCLUDED

#include "variety.h"
#include "widechar.h"

#if defined(_M_IX86)
  #pragma pack(push,1);
#else
  #pragma pack(push,8);
#endif

#if defined(__QNX__)
    #if defined(__386__)
        #define __SLIB_CALLBACK _WCFAR
        #define __SLIB
        #pragma aux slib_callback_t __far parm [eax] [edx] modify [eax edx];
    #elif defined( __SMALL_DATA__ )
        #define __SLIB_CALLBACK _WCFAR __loadds
        #define __SLIB          _WCFAR
    #else
        #define __SLIB_CALLBACK _WCFAR
        #define __SLIB          _WCFAR
    #endif
#else
    #if defined( __HUGE__ )
        #define __SLIB          _WCFAR
        #define __SLIB_CALLBACK
    #else
        #define __SLIB
        #define __SLIB_CALLBACK
    #endif
#endif

#define SPECS_VERSION           100

/*
 * This is the specs structure for pre-11.0.  It is needed for backwards
 * compatibility.
 *
 * There are both wide and MBCS versions explicitly because part of __wprtf
 * needs to access both kinds of structure.
 */
typedef struct
{
    char        __SLIB *_dest;
    int         _fld_width;     // field width
    int         _prec;          // precision
    int         _zero_fill_count;
    int         _output_count;  // # of characters outputed for %n
    char        _flags;         // flags (see below)
    char        _character;     // format character
    char        _pad_char;
    char        _alt_prefix[3];
} _mbcs_SPECS105;

typedef struct
{
    wchar_t     __SLIB *_dest;
    int         _fld_width;     // field width
    int         _prec;          // precision
    int         _zero_fill_count;
    int         _output_count;  // # of characters outputed for %n
    wchar_t     _flags;         // flags (see below)
    wchar_t     _character;     // format character
    wchar_t     _pad_char;
    wchar_t     _alt_prefix[3];
} _wide_SPECS105;

typedef struct
{
    _mbcs_SPECS105  _o;
    char            _unused[2];
    short           _version;       // structure version # (11.0 --> 1100)
    short           _flags;         // flags (see below)
    int             _n0;            // number of chars to deliver first
    int             _nz0;           // number of zeros to deliver next
    int             _n1;            // number of chars to deliver next
    int             _nz1;           // number of zeros to deliver next
    int             _n2;            // number of chars to deliver next
    int             _nz2;           // number of zeros to deliver next
} _mbcs_SPECS;

typedef struct
{
    _wide_SPECS105  _o;
    char            _unused[2];
    short           _version;       // structure version # (11.0 --> 1100)
    short           _flags;         // flags (see below)
    int             _n0;            // number of chars to deliver first
    int             _nz0;           // number of zeros to deliver next
    int             _n1;            // number of chars to deliver next
    int             _nz1;           // number of zeros to deliver next
    int             _n2;            // number of chars to deliver next
    int             _nz2;           // number of zeros to deliver next
} _wide_SPECS;

#ifdef __WIDECHAR__
    #define SPECS105            _wide_SPECS105
    #define SPECS               _wide_SPECS
#else
    #define SPECS105            _mbcs_SPECS105
    #define SPECS               _mbcs_SPECS
#endif


typedef void (__SLIB_CALLBACK slib_callback_t)( SPECS __SLIB *, int );

/* specification flags... (values for _flags field above) */

#define SPF_ALT         0x0001
#define SPF_BLANK       0x0002
#define SPF_FORCE_SIGN  0x0004
#define SPF_LEFT_ADJUST 0x0008
#define SPF_SHORT       0x0010
#define SPF_LONG        0x0020
#define SPF_NEAR        0x0040
#define SPF_FAR         0x0080
#define SPF_LONG_DOUBLE 0x0100          // also use for __int64
#define SPF_CVT         0x0200          // __cvt function

#ifdef __QNX__
#define SPF_ZERO_PAD    0x8000
#endif // __QNX__

#if !defined(__WIDECHAR__)
int __prtf( void __SLIB *dest,                  /* parm for use by out_putc */
                const char *format,             /* pointer to format string */
                va_list args,                   /* pointer to pointer to args*/
                slib_callback_t *out_putc );    /* character output routine */

#else
int __wprtf( void __SLIB *dest,                 /* parm for use by out_putc */
                const CHAR_TYPE *format,        /* pointer to format string */
                va_list args,                   /* pointer to pointer to args*/
                slib_callback_t *out_putc );    /* character output routine */
#endif

#ifdef __QNX__
int __prtf_slib( void __SLIB *dest,             /* parm for use by out_putc */
                const char * format,            /* pointer to format string */
                char **args,                    /* pointer to pointer to args*/
                slib_callback_t *out_putc,      /* character output routine */
                int ptr_size );

    #if !defined(IN_SLIB) && !defined(__386__)
        extern int ( _WCFAR * ( _WCFAR *__f)) ();
        #define __prtf(a,b,c,d) __prtf_slib(a,b,c,d,sizeof(void *))
        #define __prtf_slib(a,b,c,d,e) ((int(_WCFAR *) (void _WCFAR *,const char _WCFAR *,char * _WCFAR *args,void (_WCFAR *__out)(SPECS _WCFAR *,int),int)) __f[24])(a,b,c,d,e)
    #endif
#endif

#pragma pack(pop);
#endif

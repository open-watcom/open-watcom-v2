/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:   __prtf specs structure declaration
*
****************************************************************************/


#ifndef _SPECS_H_INCLUDED
#define _SPECS_H_INCLUDED


#define SPECS_VERSION       200

#if defined( __QNX__ ) && !defined( __STDC_WANT_LIB_EXT1__ ) && !defined( __WIDECHAR__ )
    #define GET_SPECS_DEST(t,s) (SLIB2CLIB(t,(s)->_dest))
    #define PTR_PRTF_FAR    __SLIB *
#else
    #define GET_SPECS_DEST(t,s) ((t *)(s)->_dest)
  #if defined( __HUGE__ )
    #define PTR_PRTF_FAR    _WCI86FAR *
  #else
    #define PTR_PRTF_FAR    *
  #endif
#endif

/*
 * This is the __prtf specs structure. NB - should be naturally aligned.
 *
 * There are both wide and MBCS versions explicitly because part of __wprtf
 * needs to access both kinds of structure.
 */

typedef struct
{
    char       PTR_PRTF_FAR _dest;
    short                   _flags;         // flags (see below)
    short                   _version;       // structure version # (2.0 --> 200)
    int                     _fld_width;     // field width
    int                     _prec;          // precision
    int                     _output_count;  // # of characters outputted for %n
    int                     _n0;            // number of chars to deliver first
    int                     _nz0;           // number of zeros to deliver next
    int                     _n1;            // number of chars to deliver next
    int                     _nz1;           // number of zeros to deliver next
    int                     _n2;            // number of chars to deliver next
    int                     _nz2;           // number of zeros to deliver next
    char                    _character;     // format character
    char                    _pad_char;
    char                    _padding[2];    // to keep struct aligned
} _mbcs_SPECS;

#define NEAROW_SPECS        _mbcs_SPECS
#define PTR_NEAROW_SPECS    _mbcs_SPECS PTR_PRTF_FAR

typedef struct
{
    wchar_t    PTR_PRTF_FAR _dest;
    short                   _flags;         // flags (see below)
    short                   _version;       // structure version # (2.0 --> 200)
    int                     _fld_width;     // field width
    int                     _prec;          // precision
    int                     _output_count;  // # of characters outputted for %n
    int                     _n0;            // number of chars to deliver first
    int                     _nz0;           // number of zeros to deliver next
    int                     _n1;            // number of chars to deliver next
    int                     _nz1;           // number of zeros to deliver next
    int                     _n2;            // number of chars to deliver next
    int                     _nz2;           // number of zeros to deliver next
    wchar_t                 _character;     // format character
    wchar_t                 _pad_char;
} _wide_SPECS;

#define WIDE_SPECS          _wide_SPECS

/* specification flags... (values for _flags field above) */

#define SPF_ALT         0x0001
#define SPF_BLANK       0x0002
#define SPF_FORCE_SIGN  0x0004
#define SPF_LEFT_ADJUST 0x0008
#define SPF_CHAR        0x0010
#define SPF_SHORT       0x0020
#define SPF_LONG        0x0040
#define SPF_LONG_LONG   0x0080
#define SPF_LONG_DOUBLE 0x0100          // may be also used for __int64
#define SPF_NEAR        0x0200
#define SPF_FAR         0x0400
#define SPF_CVT         0x0800          // __cvt function

#ifdef __QNX__
#define SPF_ZERO_PAD    0x8000
#endif

#ifdef __WIDECHAR__
    #define SPECS       WIDE_SPECS
#else
    #define SPECS       NEAROW_SPECS
#endif

#define PTR_SPECS       SPECS PTR_PRTF_FAR

#endif

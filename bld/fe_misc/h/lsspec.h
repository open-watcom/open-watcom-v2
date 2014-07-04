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


#ifndef _LSSPEC_H
#define _LSSPEC_H

#include "watcom.h"

#define LS_DEFS \
LS_DEF( Errors, 0x0100 ) \
LS_DEF( Usage,  0x0100 ) \
LS_DEF( NULL,   0x0100 )

typedef enum LocaleSpecificCode {
    #define LS_DEF( name, sig ) LS_##name,
    LS_DEFS
    #undef LS_DEF
    LS_MIN = 0,
    LS_MAX = LS_NULL,
} LocaleSpecificCode;

typedef enum LocaleSpecificSig {
    #define LS_DEF( name, sig ) LS_##name##_SIG = sig,
    LS_DEFS
    #undef LS_DEF
} LocaleSpecificSig;

#ifdef __UNIX__
#define LSF_TEXT_HEADER         "Sybase C++ Locale Data 1.0\n\x0c\x04"
#else
#define LSF_TEXT_HEADER         "Sybase C++ Locale Data 1.0\r\n\x1a"
#endif
#define LSF_SIGNATURE           0x041b2c01
#define LSF_MAJOR               0x01
#define LSF_MINOR               0x00


#include "pushpck1.h"

typedef struct LocaleData {
    char        text_header[ ( sizeof( LSF_TEXT_HEADER ) + 3 ) & ~3 ];
    uint_32     signature;
    uint_16     major;
    uint_16     minor;
    uint_32     units;
    uint_32     offset[LS_MAX+1];
} LocaleData;

typedef struct _LocaleItem {
    uint_16     code;           // LocaleSpecificCode
    uint_16     signature;      // LocaleSpecificSig
} _LocaleItem;

// Errors
typedef struct LocaleErrors {
    _LocaleItem header;
    int_32      number;
    char        data[1];        // <byte-strlen> <text> ... <0>
} LocaleErrors;

// Usage
typedef struct LocaleUsage {
    _LocaleItem header;
    char        data[1];        // <text> <0> ... <text> <0> <0>
} LocaleUsage;

#include "poppck.h"

#define LOCALE_DATA_EXT         "int"

#endif

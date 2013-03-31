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


#ifndef RESVER_H_INCLUDED
#define RESVER_H_INCLUDED

#define VER_CALC_SIZE   0xFFFF

#include "pushpck1.h"
typedef struct VerBlockHeader {
    uint_16     Size;           // includes size of nested blocks
    uint_16     ValSize;        // size of the value array that follow header
    uint_16     Type;           // USED FOR NT ONLY 1 == string 0 == binary
    char *      Key;
} _WCUNALIGNED VerBlockHeader;

typedef struct VerValueItem {
    uint_8      IsNum;
    uint_16     strlen;
    union {
        uint_16 Num;
        char *  String;
    } Value;
} _WCUNALIGNED VerValueItem;

typedef struct VerFixedInfo {
    uint_32     Signature;
    uint_32     StructVer;
    uint_32     FileVerHigh;
    uint_32     FileVerLow;
    uint_32     ProdVerHigh;
    uint_32     ProdVerLow;
    uint_32     FileFlagsMask;
    uint_32     FileFlags;
    uint_32     FileOS;
    uint_32     FileType;
    uint_32     FileSubType;
    uint_32     FileDateHigh;
    uint_32     FileDateLow;
} VerFixedInfo;
#include "poppck.h"

/* Why theses two values? Cause that's what Microsoft uses. */
#define VER_FIXED_SIGNATURE     0xfeef04bd
#define VER_FIXED_STRUCT_VER    0x00010000

extern int ResWriteVerBlockHeader( VerBlockHeader * head, uint_8 use_unicode, uint_8 os, WResFileID handle );
extern int ResWriteVerValueItem( VerValueItem * item, uint_8 use_unicode, WResFileID handle );
extern int ResWriteVerFixedInfo( VerFixedInfo *, WResFileID handle );
extern uint_16 ResSizeVerBlockHeader( VerBlockHeader *, uint_8 use_unicode, uint_8 os );
extern uint_16 ResSizeVerValueItem( VerValueItem * item, uint_8 use_unicode );

#endif

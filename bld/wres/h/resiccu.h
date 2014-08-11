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


#ifndef RESICCU_INCLUDED
#define RESICCU_INCLUDED

#include "pushpck1.h"
typedef struct IconCurDirHeader {
    uint_16     Reserved;       /* must be 0 */
    uint_16     Type;           /* 1 for icons, 2 for cursors */
    uint_16     ResCount;
} IconCurDirHeader;

typedef struct IconDirInfo {
    uint_8      Width;
    uint_8      Height;
    uint_8      ColourCount;
    uint_8      Reserved;
    uint_16     Planes;
    uint_16     BitCount;
    uint_32     Length;     /* bytes in resource */
} IconDirInfo;

typedef struct IconDirEntry {
    IconDirInfo Info;
    uint_16     IconID;     /* ID of the resource that this dir */
                            /* entry refers to */
} IconDirEntry;

typedef struct CurDirEntry {
    uint_16     Width;
    uint_16     Height;
    uint_16     Planes;
    uint_16     BitCount;
    uint_32     Length;     /* bytes in resource */
    uint_16     CurID;
} CurDirEntry;

typedef struct CurHotspot {
    int_16      X;          /* these are signed quantities */
    int_16      Y;
} CurHotspot;
#include "poppck.h"

extern bool ResWriteIconCurDirHeader( const IconCurDirHeader *, WResFileID handle );
extern bool ResWriteIconDirEntry( const IconDirEntry *, WResFileID handle );
extern bool ResWriteCurHotspot( const CurHotspot * hotspot, WResFileID handle );
extern bool ResWriteCurDirEntry( const CurDirEntry * entry, WResFileID handle );
extern bool ResReadIconCurDirHeader( IconCurDirHeader *, WResFileID handle );
extern bool ResReadIconDirEntry( IconDirEntry * entry, WResFileID handle );
extern bool ResReadCurDirEntry( CurDirEntry * entry, WResFileID handle );

#endif

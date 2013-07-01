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
* Description:  Interface to semantic actions for single-line resources.
*
****************************************************************************/


#ifndef SEMSINGL_INCLUDED
#define SEMSINGL_INCLUDED

#include "wresall.h"
#include "rctypes.h"
#include "scan.h"

#include "pushpck1.h"
typedef struct IcoFileDirEntry {
    IconDirInfo     Info;
    uint_32         Offset;
} IcoFileDirEntry;
#include "poppck.h"

typedef struct FullIconDirEntry {
    struct FullIconDirEntry *   Next;
    struct FullIconDirEntry *   Prev;
    uint_8                      IsIcoFileEntry;
    union {
        IcoFileDirEntry         Ico;
        IconDirEntry            Res;
    } Entry;
} FullIconDirEntry;

typedef struct FullIconDir {
    FullIconDirEntry *  Head;
    FullIconDirEntry *  Tail;
    IconCurDirHeader    Header;
} FullIconDir;

#include "pushpck1.h"
typedef struct CurFileDirEntry {
    uint_8      Width;
    uint_8      Height;
    uint_8      ColourCount;
    uint_8      Reserved;
    uint_16     XHotspot;
    uint_16     YHotspot;
    uint_32     Length;
    uint_32     Offset;
} CurFileDirEntry;
#include "poppck.h"

typedef struct FullCurDirEntry {
    struct FullCurDirEntry *    Next;
    struct FullCurDirEntry *    Prev;
    uint_8                      IsCurFileEntry;
    union {
        CurFileDirEntry         Cur;        /* cursor file */
        CurDirEntry             Res;        /* resource file (and .EXE) */
    } Entry;
} FullCurDirEntry;

typedef struct FullCurDir {
    FullCurDirEntry *   Head;
    FullCurDirEntry *   Tail;
    IconCurDirHeader    Header;
} FullCurDir;

#include "pushpck1.h"
typedef struct BitmapFileHeader {
    uint_16     Type;       /* must be BITMAP_MAGIC */
    uint_32     Size;
    uint_16     Reserved1;
    uint_16     Reserved2;
    uint_32     Offset;     /* of the bits of the bitmap itself */
} BitmapFileHeader;
#include "poppck.h"

#define BITMAP_MAGIC    0x4d42      /* the letters BM */
#define BMARRAY_MAGIC   0x4142      /* the letters BA */

extern void SemAddSingleLineResource( WResID * name, uint_8 type,
                    FullMemFlags * fullflags, char * filename );
extern void SemWriteFontDir( void );
extern void SemOS2WriteFontDir( void );
extern void SemAddMessageTable( WResID *name, ScanString *filename );
extern void SemOS2AddSingleLineResource( WResID *name, uint_8 type,
                       FullOptFlagsOS2 *fullflags, char *filename );

#endif

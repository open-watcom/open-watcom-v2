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


#ifndef SEMSINGL_INCLUDED
#define SEMSINGL_INCLUDED

#include "watcom.h"
#include "wresall.h"
#include "semantic.h"
#include "types.h"

typedef struct IcoFileDirEntry {
    IconDirInfo     Info;
    uint_32         Offset;
} IcoFileDirEntry;

#pragma pack(1)
typedef struct FullIconDirEntry {
    struct FullIconDirEntry *   Next;
    struct FullIconDirEntry *   Prev;
    uint_8                      IsIcoFileEntry;
    union {
        IcoFileDirEntry         Ico;
        IconDirEntry            Res;
    } Entry;
} FullIconDirEntry;
#pragma pack()

typedef struct FullIconDir {
    FullIconDirEntry *  Head;
    FullIconDirEntry *  Tail;
    IconCurDirHeader    Header;
} FullIconDir;

#pragma pack(1)
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
#pragma pack()

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

#pragma pack(1)
typedef struct BitmapFileHeader {
    uint_16     Type;       /* must be BITMAP_MAGIC */
    uint_32     Size;
    uint_16     Reserved1;
    uint_16     Reserved2;
    uint_32     Offset;     /* of the bits of the bitmap itself */
} BitmapFileHeader;
#pragma pack()

#define BITMAP_MAGIC    0x4d42      /* this is letters BM */

typedef struct FullFontDirEntry {
    struct FullFontDirEntry *   Next;
    struct FullFontDirEntry *   Prev;
    FontDirEntry                Entry;
} FullFontDirEntry;

typedef struct FullFontDir {
    FullFontDirEntry *      Head;
    FullFontDirEntry *      Tail;
    uint_16                 NumOfFonts;
} FullFontDir;


extern void SemAddSingleLineResource( WResID * name, uint_8 type,
                    FullMemFlags * fullflags, char * filename );
extern void SemWriteFontDir( void );
extern void ReportCopyError( RcStatus status, int read_msg, char *filename,
                             int err_code );
extern void SemAddMessageTable( WResID *name, ScanString *filename );
extern RcStatus CopyData( uint_32 offset, uint_32 length, int handle,
                          void *buff, int buffsize, int *err_code );

#endif

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


#ifndef SEMSTR_INCLUDED
#define SEMSTR_INCLUDED

#include "watcom.h"
#include "wresall.h"

typedef struct FullStringTableBlock {
    struct FullStringTableBlock *   Next;
    struct FullStringTableBlock *   Prev;
    uint_16                         BlockNum;
    int                             UseUnicode;
    ResMemFlags                     Flags;
    StringTableBlock                Block;
} FullStringTableBlock;

typedef struct FullStringTable {
    struct FullStringTable          *next;
    FullStringTableBlock            *Head;
    FullStringTableBlock            *Tail;
    WResLangType                    lang;
} FullStringTable;

typedef struct StringItem {
    uint_16         ItemID;
    char *          String;
} StringItem;

extern void SemWriteStringTable( FullStringTable *, WResID * type );
extern void SemMergeErrTable( FullStringTable *, ResMemFlags newflags );
extern void SemMergeStrTable( FullStringTable *, ResMemFlags newflags );
extern void SemMergeStringTables( FullStringTable * currtable,
            FullStringTable * oldtable, ResMemFlags newblockflags );
extern void SemAddStrToStringTable( FullStringTable * currtable,
                            uint_16 stringid, char * string );
extern void SemFreeStringTable( FullStringTable * oldtable );
extern FullStringTable * SemNewStringTable( void );

#endif

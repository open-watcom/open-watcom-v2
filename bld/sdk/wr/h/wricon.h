/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Icon/cursor group structures. Documented on MSDN but not
*               declared in any public header.
*
****************************************************************************/


#ifndef WRICON_INCLUDED
#define WRICON_INCLUDED

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

#include "pushpck1.h"

typedef struct ICONDIRENTRY {
    BYTE        bWidth;
    BYTE        bHeight;
    BYTE        bColorCount;
    BYTE        bReserved;
    WORD        wPlanes;
    WORD        wBitCount;
    DWORD       dwBytesInRes;
    DWORD       dwImageOffset;
} ICONDIRENTRY;

typedef struct ICONHEADER {
    WORD                idReserved;
    WORD                idType;
    WORD                idCount;
    ICONDIRENTRY        idEntries[1];
} ICONHEADER;

typedef struct CURSORDIRENTRY {
    BYTE        bWidth;
    BYTE        bHeight;
    BYTE        bColorCount;
    BYTE        bReserved;
    WORD        wXHotspot;
    WORD        wYHotspot;
    DWORD       dwBytesInRes;
    DWORD       dwImageOffset;
} CURSORDIRENTRY;

typedef struct CURSORHEADER {
    WORD                cdReserved;
    WORD                cdType;
    WORD                cdCount;
    CURSORDIRENTRY      cdEntries[1];
} CURSORHEADER;

typedef struct CURSORHOTSPOT {
    WORD        xHotspot;
    WORD        yHotspot;
} CURSORHOTSPOT;

typedef struct RESICONDIRENTRY {
    BYTE        bWidth;
    BYTE        bHeight;
    BYTE        bColorCount;
    BYTE        bReserved;
    WORD        wPlanes;
    WORD        wBitCount;
    DWORD       lBytesInRes;
    WORD        wNameOrdinal;
//  WORD        pad;
} RESICONDIRENTRY;

typedef struct RESICONHEADER {
    WORD                wReserved;
    WORD                wType;
    WORD                cwCount;
//  WORD                pad;
    RESICONDIRENTRY     idEntries[1];
} RESICONHEADER;

typedef struct RESCURSORDIRENTRY {
    WORD        bWidth;
    WORD        bHeight;
    WORD        wPlanes;
    WORD        wBitCount;
    DWORD       lBytesInRes;
    WORD        wNameOrdinal;
//  WORD        pad;
} RESCURSORDIRENTRY;

typedef struct RESCURSORHEADER {
    WORD                wReserved;
    WORD                wType;
    WORD                cwCount;
//  WORD                pad;
    RESCURSORDIRENTRY   cdEntries[1];
} RESCURSORHEADER;

#include "poppck.h"

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WRDLLENTRY extern uint_32  WRAPI WRSizeOfImage( BITMAPINFOHEADER *bih );
WRDLLENTRY extern WORD     WRAPI WRCountIconImages( char *data, size_t size );
WRDLLENTRY extern bool     WRAPI WRCreateIconHeader( char *data, size_t size, WORD type, ICONHEADER **ih, size_t *ihsize );
WRDLLENTRY extern bool     WRAPI WRCreateCursorResHeader( RESCURSORHEADER **rch, size_t *rchsize, char *data, size_t data_size );
WRDLLENTRY extern bool     WRAPI WRCreateIconResHeader( RESICONHEADER **rih, size_t *rihsize, char *data, size_t data_size );
WRDLLENTRY extern bool     WRAPI WRAddCursorHotspot( char **cursor, size_t *size, CURSORHOTSPOT *hs );
WRDLLENTRY extern bool     WRAPI WRGetAndAddCursorImage( char *data, WResDir dir, CURSORDIRENTRY *cd, uint_16 ord );
WRDLLENTRY extern bool     WRAPI WRGetAndAddIconImage( char *data, WResDir dir, ICONDIRENTRY *id, uint_16 ord );
WRDLLENTRY extern bool     WRAPI WRFindImageId( WRInfo *info, WResTypeNode **otnode, WResResNode **ornode, WResLangNode **lnode, uint_16 type, uint_16 id, WResLangType *lang );

WRDLLENTRY extern bool     WRAPI WRAppendDataToData( char **d1, size_t *d1size, char *d2, size_t d2size );
WRDLLENTRY extern bool     WRAPI WRAddCursorImageToData( WRInfo *info, WResLangNode*lnode, char **data, size_t *size, CURSORHOTSPOT *hotspot );
WRDLLENTRY extern bool     WRAPI WRAddIconImageToData( WRInfo *info, WResLangNode *lnode, char **data, size_t *size );
WRDLLENTRY extern bool     WRAPI WRCreateCursorData( WRInfo *info, WResLangNode *lnode, char **data, size_t *size );
WRDLLENTRY extern bool     WRAPI WRCreateIconData( WRInfo *info, WResLangNode *lnode, char **data, size_t *size );
WRDLLENTRY extern uint_16  WRAPI WRFindUnusedImageId( WRInfo *info, uint_16 start );
WRDLLENTRY extern bool     WRAPI WRCreateCursorEntries( WRInfo *info, WResLangNode *lnode, char *data, size_t size );
WRDLLENTRY extern bool     WRAPI WRCreateIconEntries( WRInfo *info, WResLangNode *lnode, char *data, size_t size );
WRDLLENTRY extern bool     WRAPI WRDeleteGroupImages( WRInfo *info, WResLangNode *lnode, uint_16 type );

#endif

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


#include "precomp.h"
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "watcom.h"
#include "wresall.h"
#include "wreglbl.h"
#include "wreresin.h"
#include "wregcres.h"
#include "wreftype.h"
#include "wrenew.h"
#include "wreimage.h"
#include "wrdll.h"
#include "wrbitmap.h"
#include "wricon.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define CALC_PAD( size, bound ) (((bound) - ((size) % (bound))) % (bound))

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool     WREFindImageId( WRECurrentResInfo *image, uint_16 type, uint_16 id, WResLangType *ltype );
static Bool     WREAddCursorHotspot( BYTE **cursor, uint_32 *size, CURSORHOTSPOT *hs );
static uint_16  WREFindUnusedImageId( WREResInfo *info, uint_16 start );
static Bool     WREGetAndAddCursorImage( BYTE *data, WResDir dir, CURSORDIRENTRY *cd, int ord );
static Bool     WREGetAndAddIconImage( BYTE *data, WResDir dir, ICONDIRENTRY *id, int ord );
static Bool     WRECreateCursorResHeader( RESCURSORHEADER **rch, uint_32 *rchsize, BYTE *data, uint_32 data_size );
static Bool     WRECreateIconResHeader( RESICONHEADER **rih, uint_32 *rihsize, BYTE *data, uint_32 data_size );
//static Bool     WREIsCorrectImageGroup( WRECurrentResInfo *group, uint_16 type, uint_16 id, Bool );
//static Bool     WREStripCursorHotspot( BYTE **cursor, uint_32 *size );
//static Bool     WREStripCursorDirectory( BYTE **cursor, uint_32 *size );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

uint_16 WREFindUnusedImageId( WREResInfo *info, uint_16 start )
{
    WRECurrentResInfo   image;
    Bool                found;
    Bool                rollover;

    found = FALSE;
    rollover = FALSE;
    image.info = info;
    if( start == 0 ) {
        start = 1;
    }
    if( start == 1 ) {
        rollover = TRUE;
    }

    for( ;; ) {
        if( start > 0x7fff ) {
            if( !rollover ) {
                rollover = TRUE;
                start = 1;
            } else {
                break;
            }
        }
        if( !WREFindImageId( &image, (uint_16)RT_ICON, start, NULL ) ) {
            if( !WREFindImageId( &image, (uint_16)RT_CURSOR, start, NULL ) ) {
                found = TRUE;
                break;
            }
        }
        start++;
    }

    if( !found ) {
        start = 0;
    }

    return( start );
}

Bool WREIsCorrectImageGroup( WRECurrentResInfo *group, uint_16 type, uint_16 id )
{
    RESICONHEADER       *ih;
    RESCURSORHEADER     *ch;
    int                 i;
    Bool                ok;

    ok = (group != NULL && group->info != NULL && group->info->info != NULL &&
          group->lang != NULL &&
          (type == (uint_16)RT_GROUP_ICON || type == (uint_16)RT_GROUP_CURSOR));

    if( ok ) {
        if( group->lang->data == NULL ) {
            group->lang->data = WREGetCurrentResData( group );
            ok = (group->lang->data != NULL);
        }
    }

    if( ok ) {
        ok = FALSE;
        if( type == (uint_16)RT_GROUP_ICON ) {
            ih = (RESICONHEADER *)group->lang->data;
            for( i = 0; !ok && i < ih->cwCount; i++ ) {
                ok = (id == (uint_16)ih->idEntries[i].wNameOrdinal);
            }
        } else {
            ch = (RESCURSORHEADER *)group->lang->data;
            for( i = 0; !ok && i < ch->cwCount; i++ ) {
                ok = (id == (uint_16)ch->cdEntries[i].wNameOrdinal);
            }
        }
    }

    return( ok );
}

Bool WREFindImageId( WRECurrentResInfo *image, uint_16 type, uint_16 id,
                     WResLangType *ltype )
{
    Bool                ok;

    ok = (image != NULL && image->info != NULL);

    if( ok ) {
        ok = WRFindImageId( image->info->info, &image->type, &image->res,
                            &image->lang, type, id, ltype );
    }

    if( !ok ) {
        image->type = NULL;
        image->lang = NULL;
        image->lang = NULL;
    }

    return( ok );
}

Bool WREDeleteGroupImages( WRECurrentResInfo *group, uint_16 type )
{
    Bool                ok;

    ok = (group != NULL && group->info != NULL && group->lang != NULL);

    if( ok ) {
        ok = WRDeleteGroupImages( group->info->info, group->lang, type );
    }

    return( ok );
}

Bool WREAppendDataToData( BYTE **d1, uint_32 *d1size, BYTE *d2, uint_32 d2size )
{
    if( d1 == NULL || d1size == NULL || d2 == NULL || d2size == 0 ) {
        return( FALSE );
    }

    *d1 = WRMemRealloc( *d1, *d1size + d2size );
    if( *d1 == NULL ) {
        return( FALSE );
    }

    memcpy( *d1 + *d1size, d2, d2size );
    *d1size += d2size;

    return( TRUE );
}

Bool WREAddCursorImageToData( WRECurrentResInfo *image, BYTE **data,
                              uint_32 *size, CURSORHOTSPOT *hotspot )
{
    int         hs_size; // size of hotspot info
    Bool        ok;

    ok = (image != NULL && image->info != NULL && image->info->info != NULL &&
          image->lang != NULL && data != NULL && size != NULL && hotspot != NULL);

    if( ok ) {
        if( image->lang->data == NULL ) {
            image->lang->data = WREGetCurrentResData( image );
            ok = (image->lang->data != NULL);
        }
    }

    if( ok ) {
        hs_size = sizeof( CURSORHOTSPOT );
        memcpy( hotspot, image->lang->data, hs_size );
        ok = WREAppendDataToData( data, size, (BYTE *)image->lang->data + hs_size,
                                  image->lang->Info.Length - hs_size );
    }

    return( ok );
}

Bool WREAddIconImageToData( WRECurrentResInfo *image, BYTE **data, uint_32 *size )
{
    Bool        ok;

    ok = (image != NULL && image->info != NULL && image->info->info != NULL &&
          image->lang != NULL && data != NULL && size != NULL);

    if( ok ) {
        if( image->lang->data == NULL ) {
            image->lang->data = WREGetCurrentResData( image );
            ok = (image->lang->data != NULL);
        }
    }

    if( ok ) {
        ok = WREAppendDataToData( data, size, image->lang->data,
                                  image->lang->Info.Length );
    }

    return( ok );
}

Bool WRECreateCursorDataFromGroup( WRECurrentResInfo *group, BYTE **data, uint_32 *size )
{
    WRECurrentResInfo   image;
    WResLangType        lt;
    RESCURSORHEADER     *rch;
    CURSORHEADER        *ch;
    CURSORHOTSPOT       hotspot;
    uint_16             ord;
    uint_32             osize;
    int                 i;
    Bool                ok;

    ok = (group != NULL && group->info != NULL && group->info->info != NULL &&
          group->lang != NULL && data != NULL && size != NULL);

    if( ok ) {
        if( group->lang->data == NULL ) {
            group->lang->data = WREGetCurrentResData( group );
            ok = (group->lang->data != NULL);
        }
    }

    if( ok ) {
        image.info = group->info;
        rch = (RESCURSORHEADER *)group->lang->data;
        *size = sizeof( CURSORHEADER );
        *size += sizeof( CURSORDIRENTRY ) * (rch->cwCount - 1);
        *data = (BYTE *)WRMemAlloc( *size );
        ch = (CURSORHEADER *)*data;
        ok = (*data != NULL);
    }

    if( ok ) {
        memcpy( ch, rch, sizeof( WORD ) * 3 );
    }

    if( ok ) {
        for( i = 0; ok && i < rch->cwCount; i++ ) {
            ord = (uint_16)rch->cdEntries[i].wNameOrdinal;
            lt = group->lang->Info.lang;
            ok = WREFindImageId( &image, (uint_16)RT_CURSOR, ord, &lt );
            if( ok ) {
                osize = *size;
                ok = WREAddCursorImageToData( &image, data, size, &hotspot );
                if( ok ) {
                    ch = (CURSORHEADER *)*data;
                    ch->cdEntries[i].bWidth = rch->cdEntries[i].bWidth;
                    ch->cdEntries[i].bHeight = rch->cdEntries[i].bHeight / 2;
                    ch->cdEntries[i].bColorCount = 0;
                    ch->cdEntries[i].bReserved = 0;
                    ch->cdEntries[i].wXHotspot = hotspot.xHotspot;
                    ch->cdEntries[i].wYHotspot = hotspot.yHotspot;
                    ch->cdEntries[i].dwBytesInRes = *size - osize;
                    ch->cdEntries[i].dwImageOffset = osize;
                }
            }
        }
    }

    if( !ok ) {
        if( *data != NULL ) {
            WRMemFree( *data );
            *data = NULL;
        }
        *size = 0;
    }

    return( ok );
}

Bool WRECreateIconDataFromGroup( WRECurrentResInfo *group, BYTE **data, uint_32 *size )
{
    WResLangType        lt;
    WRECurrentResInfo   image;
    RESICONHEADER       *rih;
    ICONHEADER          *ih;
    uint_16             ord;
    uint_32             osize;
    int                 i;
    Bool                ok;

    ok = (group != NULL && group->info != NULL && group->info->info != NULL &&
          group->lang != NULL && data != NULL && size != NULL);

    if( ok ) {
        if( group->lang->data == NULL ) {
            group->lang->data = WREGetCurrentResData( group );
            ok = (group->lang->data != NULL);
        }
    }

    if( ok ) {
        image.info = group->info;
        rih = (RESICONHEADER *)group->lang->data;
        *size = sizeof( ICONHEADER );
        *size += sizeof( ICONDIRENTRY ) * (rih->cwCount - 1);
        *data = (BYTE *)WRMemAlloc( *size );
        ih = (ICONHEADER *)*data;
        ok = (*data != NULL);
    }

    if( ok ) {
        memcpy( ih, rih, sizeof( WORD ) * 3 );
    }

    if( ok ) {
        for( i = 0; ok && i < rih->cwCount; i++ ) {
            ord = (uint_16)rih->idEntries[i].wNameOrdinal;
            lt = group->lang->Info.lang;
            ok = WREFindImageId( &image, (uint_16)RT_ICON, ord, &lt );
            if( ok ) {
                osize = *size;
                ok = WREAddIconImageToData( &image, data, size );
                if( ok ) {
                    ih = (ICONHEADER *)*data;
                    ih->idEntries[i].bWidth = rih->idEntries[i].bWidth;
                    ih->idEntries[i].bHeight = rih->idEntries[i].bHeight;
                    ih->idEntries[i].bColorCount = rih->idEntries[i].bColorCount;
                    //ih->idEntries[i].wPlanes = rih->idEntries[i].wPlanes;
                    //ih->idEntries[i].wBitCount = rih->idEntries[i].wBitCount;
                    ih->idEntries[i].wPlanes = 0;
                    ih->idEntries[i].wBitCount = 0;
                    ih->idEntries[i].bReserved = 0;
                    ih->idEntries[i].dwBytesInRes = *size - osize;
                    ih->idEntries[i].dwImageOffset = osize;
                }
            }
        }
    }

    if( !ok ) {
        if( *data != NULL ) {
            WRMemFree( *data );
            *data = NULL;
        }
        *size = 0;
    }

    return( ok );
}

Bool WREGetAndAddCursorImage( BYTE *data, WResDir dir, CURSORDIRENTRY *cd, int ord )
{
    BYTE                *cursor;
    int                 dup;
    uint_32             size;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    CURSORHOTSPOT       hotspot;
    Bool                ok;

    dup = FALSE;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    tname = NULL;
    rname = NULL;

    ok = (data != NULL && dir != NULL && cd != NULL && cd->dwBytesInRes != 0);

    if ( ok ) {
        cursor = (BYTE *)WRMemAlloc( cd->dwBytesInRes );
        ok = (cursor != NULL);
    }

    if( ok ) {
        memcpy( cursor, data + cd->dwImageOffset, cd->dwBytesInRes );
        hotspot.xHotspot = cd->wXHotspot;
        hotspot.yHotspot = cd->wYHotspot;
        size = cd->dwBytesInRes;
        ok = WREAddCursorHotspot( &cursor, &size, &hotspot );
    }

    if( ok ) {
        tname = WResIDFromNum( (uint_16)RT_CURSOR );
        ok = (tname != NULL);
    }

    if( ok ) {
        rname = WResIDFromNum( ord );
        ok = (rname != NULL);
    }

    if( ok ) {
        ok = !WResAddResource( tname, rname, DEF_MEMFLAGS, 0,
                               size, dir, &lang, &dup );
    }

    if( ok ) {
        ok = WRFindAndSetData( dir, tname, rname, &lang, cursor );
    }

    if( !ok ) {
        if( cursor != NULL ) {
            WRMemFree( cursor );
        }
    }

    if( tname != NULL ) {
        WRMemFree( tname );
    }

    if( rname != NULL ) {
        WRMemFree( rname );
    }

    return( ok );
}

Bool WREGetAndAddIconImage( BYTE *data, WResDir dir, ICONDIRENTRY *id, int ord )
{
    BYTE                *icon;
    int                 dup;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    Bool                ok;

    dup = FALSE;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    tname = NULL;
    rname = NULL;

    ok = (data != NULL && dir != NULL && id != NULL && id->dwBytesInRes != 0);

    if( ok ) {
        icon = (BYTE *)WRMemAlloc( id->dwBytesInRes );
        ok = (icon != NULL);
    }

    if( ok ) {
        memcpy( icon, data + id->dwImageOffset, id->dwBytesInRes );
        tname = WResIDFromNum( (uint_16)RT_ICON );
        ok = (tname != NULL);
    }

    if( ok ) {
        rname = WResIDFromNum( ord );
        ok = (rname != NULL);
    }

    if ( ok ) {
        ok = !WResAddResource( tname, rname, DEF_MEMFLAGS, 0,
                               id->dwBytesInRes, dir, &lang, &dup );
    }

    if( ok ) {
        ok = WRFindAndSetData( dir, tname, rname, &lang, icon );
    }

    if( !ok ) {
        if( icon != NULL ) {
            WRMemFree( icon );
        }
    }

    if( tname != NULL ) {
        WRMemFree( tname );
    }

    if( rname != NULL ) {
        WRMemFree( rname );
    }

    return( ok );
}

Bool WRECreateCursorResHeader( RESCURSORHEADER **rch, uint_32 *rchsize,
                               BYTE *data, uint_32 data_size )
{
    CURSORHEADER        *ch;
    uint_32             chsize;
    ICONHEADER          *ih;
    uint_32             ihsize;
    int                 i;
    Bool                ok;

    ih = NULL;

    ok = (rch != NULL && rchsize != NULL && data != NULL && data_size != 0);

    if( ok ) {
        *rch = NULL;
        *rchsize = 0;
        ch = (CURSORHEADER *)data;
        chsize = sizeof( CURSORHEADER );
        chsize += sizeof( CURSORDIRENTRY ) * (ch->cdCount - 1);
        ok = WRCreateIconHeader( data + chsize, data_size - chsize, 2,
                                 &ih, &ihsize );
    }

    if( ok ) {
        *rchsize = sizeof( RESCURSORHEADER );
        *rchsize += sizeof( RESCURSORDIRENTRY ) * (ih->idCount - 1);
        *rch = (RESCURSORHEADER *)WRMemAlloc( *rchsize );
        ok = (*rch != NULL);
    }

    if( ok ) {
        memcpy( *rch, ch, sizeof( WORD ) * 3 );
        for( i = 0; i < ih->idCount; i++ ) {
            (*rch)->cdEntries[i].bWidth = ih->idEntries[i].bWidth;
            (*rch)->cdEntries[i].bHeight = ih->idEntries[i].bHeight * 2;
            (*rch)->cdEntries[i].wPlanes = ih->idEntries[i].wPlanes;
            (*rch)->cdEntries[i].wBitCount = ih->idEntries[i].wBitCount;
            (*rch)->cdEntries[i].lBytesInRes = ih->idEntries[i].dwBytesInRes;
            (*rch)->cdEntries[i].wNameOrdinal = i + 1;
        }
    }

    if( ih != NULL ) {
        WRMemFree( ih );
    }

    return( ok );
}

Bool WRECreateIconResHeader( RESICONHEADER **rih, uint_32 *rihsize,
                             BYTE *data, uint_32 data_size )
{
    ICONHEADER          *pih;
    uint_32             pihsize;
    ICONHEADER          *ih;
    uint_32             ihsize;
    int                 i;
    Bool                ok;

    ih = NULL;

    ok = (rih != NULL && rihsize != NULL && data != NULL && data_size != 0);

    if( ok ) {
        pih = (ICONHEADER *)data;
        pihsize = sizeof( ICONHEADER );
        pihsize += sizeof( ICONDIRENTRY ) * (pih->idCount - 1);
        ok = WRCreateIconHeader( data + pihsize, data_size - pihsize, 1,
                                 &ih, &ihsize );
    }

    if( ok ) {
        *rihsize = sizeof( RESICONHEADER );
        *rihsize += sizeof( RESICONDIRENTRY ) * (ih->idCount - 1);
        *rih = (RESICONHEADER *)WRMemAlloc( *rihsize );
        ok = (*rih != NULL);
    }

    if( ok ) {
        memcpy( *rih, pih, sizeof( WORD ) * 3 );
        for( i = 0; i < ih->idCount; i++ ) {
            (*rih)->idEntries[i].bWidth = ih->idEntries[i].bWidth;
            (*rih)->idEntries[i].bHeight = ih->idEntries[i].bHeight;
            (*rih)->idEntries[i].bColorCount = ih->idEntries[i].bColorCount;
            (*rih)->idEntries[i].bReserved = 0;
            (*rih)->idEntries[i].wPlanes = ih->idEntries[i].wPlanes;
            (*rih)->idEntries[i].wBitCount = ih->idEntries[i].wBitCount;
            (*rih)->idEntries[i].lBytesInRes = ih->idEntries[i].dwBytesInRes;
            (*rih)->idEntries[i].wNameOrdinal = i + 1;
        }
    }

    if( ih != NULL ) {
        WRMemFree( ih );
    }

    return( ok );
}

// This function assumes that the data represents icon data WITHOUT
// an icon directory
WORD WRECountIconImages( BYTE *data, uint_32 size )
{
    BITMAPINFOHEADER    *bih;
    WORD                count;
    uint_32             pos;

    pos = 0;
    count = 0;
    while( pos < size ) {
        bih = (BITMAPINFOHEADER *)(data + pos);
        count++;
        pos += WRSizeOfImage( bih );
        // if we overrun do not count this block
        if( pos > size ) {
            count--;
        }
    }

    return( count );
}

Bool WRECalcAndAddIconDirectory( BYTE **data, uint_32 *size, WORD type )
{
    ICONHEADER  *ih;
    uint_32     ihsize;

    if( !WRCreateIconHeader( *data, *size, type, &ih, &ihsize ) ) {
        return( FALSE );
    }

    *data = WRMemRealloc( *data, *size + ihsize );
    if( *data == NULL ) {
        return( FALSE );
    }
    memmove( *data + ihsize, *data, *size );
    memcpy( *data, ih, ihsize );
    *size += ihsize;

    WRMemFree( ih );

    return( TRUE );
}

Bool WREStripIconDirectory( BYTE **icon, uint_32 *size )
{
    ICONHEADER  *ih;
    uint_32     ihsize;

    if( icon != NULL && *icon != NULL && size != NULL ) {
        ih = (ICONHEADER *)*icon;
        if( ih->idType == 1 || ih->idType == 2 ) {
            ihsize = sizeof( ICONHEADER );
            ihsize += sizeof( ICONDIRENTRY ) * (ih->idCount - 1);
            memmove( *icon, *icon + ihsize, *size - ihsize );
            *size -= ihsize;
            return( TRUE );
        }
    }

    return( FALSE );
}

Bool WREAddCursorHotspot( BYTE **cursor, uint_32 *size, CURSORHOTSPOT *hs )
{
    int hs_size;

    hs_size = sizeof( CURSORHOTSPOT );

    if( cursor == NULL || size == NULL ) {
        return( FALSE );
    }

    *cursor = WRMemRealloc( *cursor, *size + hs_size );
    if( *cursor == NULL ) {
        return( FALSE );
    }
    memmove( *cursor + hs_size, *cursor, *size );
    memcpy( *cursor, hs, hs_size );
    *size += hs_size;

    return( TRUE );
}

Bool WREStripCursorHotspot( BYTE **cursor, uint_32 *size )
{
    int hs_size;

    hs_size = sizeof( CURSORHOTSPOT );
    if( cursor == NULL && size == NULL && *size > hs_size ) {
        memmove( *cursor, *cursor + hs_size, *size - hs_size );
        *size -= hs_size;
        return( TRUE );
    }

    return( FALSE );
}

Bool WREStripCursorDirectory( BYTE **cursor, uint_32 *size )
{
    CURSORHEADER        *ch;
    uint_32             cd_size;

    if( cursor != NULL && *cursor != NULL && size != NULL ) {
        return( FALSE );
    }

    ch = (CURSORHEADER *)*cursor;
    cd_size = sizeof( CURSORHEADER );
    cd_size += sizeof( CURSORDIRENTRY ) * (ch->cdCount - 1);
    memmove( *cursor, *cursor + cd_size, *size - cd_size );
    *size -= cd_size;

    return( TRUE );
}

Bool WREAddBitmapFileHeader( BYTE **data, uint_32 *size )
{
    return( WRAddBitmapFileHeader( data, size ) );
}

Bool WREStripBitmapFileHeader( BYTE **data, uint_32 *size )
{
    return( WRStripBitmapFileHeader( data, size ) );
}

Bool WRECreateCursorEntries( WRECurrentResInfo *curr, void *data, uint_32 size )
{
    RESCURSORHEADER     *rch;
    CURSORHEADER        *ch;
    uint_16             ord;
    uint_32             rchsize;
    int                 i;
    Bool                ok;

    ok = (curr != NULL && curr->info != NULL && data != NULL && size != 0);

    if( ok ) {
        if( curr->lang->data != NULL ) {
            WRMemFree( curr->lang->data );
            curr->lang->data = NULL;
        }
        curr->lang->Info.Length = 0;
        ok = WRECreateCursorResHeader( &rch, &rchsize, data, size );
    }

    if( ok ) {
        curr->lang->data = (void *)rch;
        curr->lang->Info.Length = rchsize;
        ord = 0;
        ch = (CURSORHEADER *)data;
        for( i = 0; ok && i < rch->cwCount; i++ ) {
            ord = WREFindUnusedImageId( curr->info, ord );
            ok = (ord != 0);
            if( ok ) {
                rch->cdEntries[i].wNameOrdinal = ord;
                ok = WREGetAndAddCursorImage( data, curr->info->info->dir,
                                              &ch->cdEntries[i], ord );
            }
        }
    }

    return( ok );
}

Bool WRECreateIconEntries( WRECurrentResInfo *curr, void *data, uint_32 size )
{
    RESICONHEADER       *rih;
    ICONHEADER          *ih;
    uint_16             ord;
    uint_32             rihsize;
    int                 i;
    Bool                ok;

    ok = (curr != NULL && curr->info != NULL && data != NULL && size != 0);

    if( ok ) {
        if( curr->lang->data != NULL ) {
            WRMemFree( curr->lang->data );
            curr->lang->data = NULL;
        }
        curr->lang->Info.Length = 0;
        ok = WRECreateIconResHeader( &rih, &rihsize, data, size );
    }

    if( ok ) {
        curr->lang->data = (void *)rih;
        curr->lang->Info.Length = rihsize;
        ord = 0;
        ih = (ICONHEADER *)data;
        for( i = 0; ok && i < rih->cwCount; i++ ) {
            ord = WREFindUnusedImageId( curr->info, ord );
            ok = (ord != 0);
            if( ok ) {
                rih->idEntries[i].wNameOrdinal = ord;
                ok = WREGetAndAddIconImage( data, curr->info->info->dir,
                                            &ih->idEntries[i], ord );
            }
        }
    }

    return( ok );
}

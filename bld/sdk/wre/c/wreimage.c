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


#include <windows.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "wresall.h"
#include "wreglbl.h"
#include "wreresin.h"
#include "wregcres.h"
#include "wreftype.h"
#include "wremem.h"
#include "wrenew.h"
#include "wreimage.h"
#include "bitmap.h"
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
static  Bool    WREFindImageId          ( WRECurrentResInfo *image,
                                          uint_16 type, uint_16 id,
                                          WResLangType *ltype );
static  Bool    WREAddCursorHotspot     ( BYTE **cursor, uint_32 *size,
                                          CURSORHOTSPOT *hs );
static  uint_16 WREFindUnusedImageId    ( WREResInfo *info, uint_16 start );
static  Bool    WREGetAndAddCursorImage ( BYTE *data, WResDir dir,
                                          CURSORDIRENTRY *cd, int ord );
static  Bool    WREGetAndAddIconImage   ( BYTE *data, WResDir dir,
                                          ICONDIRENTRY *id, int ord );
static  Bool    WRECreateCursorResHeader( WRESCURSORHEADER **rch,
                                          uint_32 *rchsize, BYTE *data,
                                          uint_32 data_size, Bool is32bit );
static  Bool    WRECreateIconResHeader  ( WRESICONHEADER **rih, uint_32 *rihsize,
                                          BYTE *data, uint_32 data_size,
                                          Bool is32bit );
//static        Bool    WREIsCorrectImageGroup  ( WRECurrentResInfo *group,
//                                        uint_16 type, uint_16 id, Bool );
//static        Bool    WREStripCursorHotspot   ( BYTE **cursor, uint_32 *size );
//static        Bool    WREStripCursorDirectory ( BYTE **cursor, uint_32 *size );

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

    while( TRUE ) {
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

Bool WREIsCorrectImageGroup( WRECurrentResInfo *group, uint_16 type,
                             uint_16 id, Bool is32bit )
{
    WRESICONHEADER      *ih;
    WRESCURSORHEADER    *ch;
    int                 i;
    Bool                ok;

    ok = ( group && group->info && group->info->info && group->lang &&
           ( ( type == (uint_16)RT_GROUP_ICON ) ||
             ( type == (uint_16)RT_GROUP_CURSOR ) ) );


    if( ok ) {
        if( group->lang->data == NULL ) {
            group->lang->data = WREGetCurrentResData( group );
            ok = ( group->lang->data != NULL );
        }
    }

    if( ok ) {
        ok = FALSE;
        if( type == (uint_16)RT_GROUP_ICON ) {
            ih = (WRESICONHEADER *)group->lang->data;
            for( i=0; !ok && i<ih->h16.cwCount; i++ ) {
                if( is32bit ) {
                    ok = ( id == (uint_16)ih->h32.idEntries[i].wNameOrdinal );
                } else {
                    ok = ( id == (uint_16)ih->h16.idEntries[i].wNameOrdinal );
                }
            }
        } else {
            ch = (WRESCURSORHEADER *)group->lang->data;
            for( i=0; !ok && i<ch->h16.cwCount; i++ ) {
                if( is32bit ) {
                    ok = ( id == (uint_16)ch->h32.cdEntries[i].wNameOrdinal );
                } else {
                    ok = ( id == (uint_16)ch->h16.cdEntries[i].wNameOrdinal );
                }
            }
        }
    }

    return( ok );
}

Bool WREFindImageId( WRECurrentResInfo *image, uint_16 type, uint_16 id,
                     WResLangType *ltype )
{
    Bool                ok;

    ok = ( image && image->info );

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

    ok = ( group && group->info && group->lang );

    if( ok ) {
        ok = WRDeleteGroupImages( group->info->info, group->lang, type );
    }

    return( ok );
}

Bool WREAppendDataToData( BYTE **d1, uint_32 *d1size, BYTE *d2, uint_32 d2size )
{
    if( !d1 || !d1size || !d2 || !d2size ) {
        return( FALSE );
    }

    *d1 = WREMemRealloc( *d1, *d1size + d2size );
    if( !*d1 ) {
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

    ok = ( image && image->info && image->info->info && image->lang &&
           data && size && hotspot );

    if( ok ) {
        if( image->lang->data == NULL ) {
            image->lang->data = WREGetCurrentResData( image );
            ok = ( image->lang->data != NULL );
        }
    }

    if( ok ) {
        hs_size = sizeof(CURSORHOTSPOT);
        memcpy( hotspot, image->lang->data, hs_size );
        ok = WREAppendDataToData( data, size,
                                  (BYTE *)image->lang->data + hs_size,
                                  image->lang->Info.Length - hs_size );
    }

    return( ok );
}

Bool WREAddIconImageToData( WRECurrentResInfo *image,
                            BYTE **data, uint_32 *size )
{
    Bool        ok;

    ok = ( image && image->info && image->info->info && image->lang &&
           data && size );

    if( ok ) {
        if( image->lang->data == NULL ) {
            image->lang->data = WREGetCurrentResData( image );
            ok = ( image->lang->data != NULL );
        }
    }

    if( ok ) {
        ok = WREAppendDataToData( data, size, image->lang->data,
                                  image->lang->Info.Length );
    }

    return( ok );
}

Bool WRECreateCursorDataFromGroup( WRECurrentResInfo *group,
                                   BYTE **data, uint_32 *size )
{
    WRECurrentResInfo   image;
    WResLangType        lt;
    WRESCURSORHEADER    *rch;
    CURSORHEADER        *ch;
    CURSORHOTSPOT       hotspot;
    uint_16             ord;
    uint_32             osize;
    int                 i;
    Bool                is32bit;
    Bool                ok;

    ok = ( group && group->info && group->info->info && group->lang &&
           data && size );

    if( ok ) {
        if( group->lang->data == NULL ) {
            group->lang->data = WREGetCurrentResData( group );
            ok = ( group->lang->data != NULL );
        }
    }

    if( ok ) {
        is32bit = group->info->is32bit;
        image.info = group->info;
        rch = (WRESCURSORHEADER *)group->lang->data;
        *size = sizeof( CURSORHEADER );
        *size += sizeof(CURSORDIRENTRY)*(rch->h16.cwCount-1);
        *data = (BYTE *)WREMemAlloc( *size );
        ch = (CURSORHEADER *)*data;
        ok = ( *data != NULL );
    }

    if( ok ) {
        memcpy( ch, rch, sizeof(WORD)*3 );
    }

    if( ok ) {
        for( i=0; ok && i<rch->h16.cwCount; i++ ) {
            if( is32bit ) {
                ord = (uint_16) rch->h32.cdEntries[i].wNameOrdinal;
            } else {
                ord = (uint_16) rch->h16.cdEntries[i].wNameOrdinal;
            }
            lt = group->lang->Info.lang;
            ok = WREFindImageId( &image, (uint_16)RT_CURSOR, ord, &lt );
            if( ok ) {
                osize = *size;
                ok = WREAddCursorImageToData( &image, data, size, &hotspot );
                if( ok ) {
                    ch = (CURSORHEADER *)*data;
                    if( is32bit ) {
                        ch->cdEntries[i].bWidth = rch->h32.cdEntries[i].bWidth;
                        ch->cdEntries[i].bHeight = rch->h32.cdEntries[i].bHeight/2;
                    } else {
                        ch->cdEntries[i].bWidth = rch->h16.cdEntries[i].bWidth;
                        ch->cdEntries[i].bHeight = rch->h16.cdEntries[i].bHeight/2;
                    }
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
            WREMemFree( *data );
            *data = NULL;
        }
        *size = 0;
    }

    return( ok );
}

Bool WRECreateIconDataFromGroup( WRECurrentResInfo *group,
                                 BYTE **data, uint_32 *size )
{
    WResLangType        lt;
    WRECurrentResInfo   image;
    WRESICONHEADER      *rih;
    ICONHEADER          *ih;
    uint_16             ord;
    uint_32             osize;
    int                 i;
    Bool                is32bit;
    Bool                ok;

    ok = ( group && group->info && group->info->info && group->lang &&
           data && size );

    if( ok ) {
        if( group->lang->data == NULL ) {
            group->lang->data = WREGetCurrentResData( group );
            ok = ( group->lang->data != NULL );
        }
    }

    if( ok ) {
        is32bit = group->info->is32bit;
        image.info = group->info;
        rih = (WRESICONHEADER *)group->lang->data;
        *size = sizeof( ICONHEADER );
        *size += sizeof(ICONDIRENTRY)*(rih->h16.cwCount-1);
        *data = (BYTE *)WREMemAlloc( *size );
        ih = (ICONHEADER *)*data;
        ok = ( *data != NULL );
    }

    if( ok ) {
        memcpy( ih, rih, sizeof(WORD)*3 );
    }

    if( ok ) {
        for( i=0; ok && i<rih->h16.cwCount; i++ ) {
            if( is32bit ) {
                ord = (uint_16) rih->h32.idEntries[i].wNameOrdinal;
            } else {
                ord = (uint_16) rih->h16.idEntries[i].wNameOrdinal;
            }
            lt = group->lang->Info.lang;
            ok = WREFindImageId( &image, (uint_16)RT_ICON, ord, &lt );
            if( ok ) {
                osize = *size;
                ok = WREAddIconImageToData( &image, data, size );
                if( ok ) {
                    ih = (ICONHEADER *)*data;
                    if( is32bit ) {
                        ih->idEntries[i].bWidth = rih->h32.idEntries[i].bWidth;
                        ih->idEntries[i].bHeight = rih->h32.idEntries[i].bHeight;
                        ih->idEntries[i].bColorCount = rih->h32.idEntries[i].bColorCount;
                        //ih->idEntries[i].wPlanes = rih->h32.idEntries[i].wPlanes;
                        //ih->idEntries[i].wBitCount = rih->h32.idEntries[i].wBitCount;
                    } else {
                        ih->idEntries[i].bWidth = rih->h16.idEntries[i].bWidth;
                        ih->idEntries[i].bHeight = rih->h16.idEntries[i].bHeight;
                        ih->idEntries[i].bColorCount = rih->h16.idEntries[i].bColorCount;
                        //ih->idEntries[i].wPlanes = rih->h16.idEntries[i].wPlanes;
                        //ih->idEntries[i].wBitCount = rih->h16.idEntries[i].wBitCount;
                    }
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
            WREMemFree( *data );
            *data = NULL;
        }
        *size = 0;
    }

    return( ok );
}

Bool WREGetAndAddCursorImage( BYTE *data, WResDir dir,
                              CURSORDIRENTRY *cd, int ord )
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

    ok = ( data && dir && cd && cd->dwBytesInRes );

    if ( ok ) {
        cursor = (BYTE *)WREMemAlloc( cd->dwBytesInRes );
        ok = ( cursor != NULL );
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
        ok = ( tname != NULL );
    }

    if( ok ) {
        rname = WResIDFromNum( ord );
        ok = ( rname != NULL );
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
            WREMemFree( cursor );
        }
    }

    if( tname != NULL ) {
        WREMemFree( tname );
    }

    if( rname != NULL ) {
        WREMemFree( rname );
    }

    return( ok );
}

Bool WREGetAndAddIconImage( BYTE *data, WResDir dir,
                            ICONDIRENTRY *id, int ord )
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

    ok = ( data && dir && id && id->dwBytesInRes );

    if ( ok ) {
        icon = (BYTE *)WREMemAlloc( id->dwBytesInRes );
        ok = ( icon != NULL );
    }

    if( ok ) {
        memcpy( icon, data + id->dwImageOffset, id->dwBytesInRes );
        tname = WResIDFromNum( (uint_16)RT_ICON );
        ok = ( tname != NULL );
    }

    if( ok ) {
        rname = WResIDFromNum( ord );
        ok = ( rname != NULL );
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
            WREMemFree( icon );
        }
    }

    if( tname != NULL ) {
        WREMemFree( tname );
    }

    if( rname != NULL ) {
        WREMemFree( rname );
    }

    return( ok );
}

Bool WRECreateCursorResHeader( WRESCURSORHEADER **rch, uint_32 *rchsize,
                               BYTE *data, uint_32 data_size, Bool is32bit )
{
    CURSORHEADER        *ch;
    uint_32             chsize;
    ICONHEADER          *ih;
    uint_32             ihsize;
    int                 i;
    Bool                ok;

    ih = NULL;

    ok = ( rch && rchsize && data && data_size );

    if( ok ) {
        *rch = NULL;
        *rchsize = 0;
        ch = (CURSORHEADER *) data;
        chsize = sizeof(CURSORHEADER);
        chsize += sizeof(CURSORDIRENTRY)*(ch->cdCount-1);
        ok = WRCreateIconHeader( data + chsize, data_size - chsize, 2,
                                 &ih, &ihsize );
    }

    if( ok ) {
        if( is32bit ) {
            *rchsize = sizeof(RESCURSORHEADER32);
            *rchsize += sizeof(RESCURSORDIRENTRY32)*(ih->idCount-1);
        } else {
            *rchsize = sizeof(RESCURSORHEADER);
            *rchsize += sizeof(RESCURSORDIRENTRY)*(ih->idCount-1);
        }
        *rch = (WRESCURSORHEADER *) WREMemAlloc( *rchsize );
        ok = ( *rch != NULL );
    }

    if( ok ) {
        memcpy( *rch, ch, sizeof(WORD)*3 );
        for( i=0; i<ih->idCount ; i++ ) {
            if( is32bit ) {
                (*rch)->h32.cdEntries[i].bWidth = ih->idEntries[i].bWidth;
                (*rch)->h32.cdEntries[i].bHeight = ih->idEntries[i].bHeight*2;
                (*rch)->h32.cdEntries[i].wPlanes = ih->idEntries[i].wPlanes;
                (*rch)->h32.cdEntries[i].wBitCount = ih->idEntries[i].wBitCount;
                (*rch)->h32.cdEntries[i].lBytesInRes = ih->idEntries[i].dwBytesInRes;
                (*rch)->h32.cdEntries[i].wNameOrdinal = i + 1;
            } else {
                (*rch)->h16.cdEntries[i].bWidth = ih->idEntries[i].bWidth;
                (*rch)->h16.cdEntries[i].bHeight = ih->idEntries[i].bHeight*2;
                (*rch)->h16.cdEntries[i].wPlanes = ih->idEntries[i].wPlanes;
                (*rch)->h16.cdEntries[i].wBitCount = ih->idEntries[i].wBitCount;
                (*rch)->h16.cdEntries[i].lBytesInRes = ih->idEntries[i].dwBytesInRes;
                (*rch)->h16.cdEntries[i].wNameOrdinal = i + 1;
            }
        }
    }

    if( ih != NULL ) {
        WREMemFree( ih );
    }

    return( ok );
}

Bool WRECreateIconResHeader( WRESICONHEADER **rih, uint_32 *rihsize,
                             BYTE *data, uint_32 data_size, Bool is32bit )
{
    ICONHEADER          *pih;
    uint_32             pihsize;
    ICONHEADER          *ih;
    uint_32             ihsize;
    int                 i;
    Bool                ok;

    ih = NULL;

    ok = ( rih && rihsize && data && data_size );

    if( ok ) {
        pih = (ICONHEADER *) data;
        pihsize = sizeof(ICONHEADER);
        pihsize += sizeof(ICONDIRENTRY)*(pih->idCount-1);
        ok = WRCreateIconHeader( data + pihsize, data_size - pihsize, 1,
                                 &ih, &ihsize );
    }

    if( ok ) {
        if( is32bit ) {
            *rihsize = sizeof(RESICONHEADER32);
            *rihsize += sizeof(RESICONDIRENTRY32)*(ih->idCount-1);
        } else {
            *rihsize = sizeof(RESICONHEADER);
            *rihsize += sizeof(RESICONDIRENTRY)*(ih->idCount-1);
        }
        *rih = (WRESICONHEADER *) WREMemAlloc( *rihsize );
        ok = ( *rih != NULL );
    }

    if( ok ) {
        memcpy( *rih, pih, sizeof(WORD)*3 );
        for( i=0; i<ih->idCount ; i++ ) {
            if( is32bit ) {
                (*rih)->h32.idEntries[i].bWidth = ih->idEntries[i].bWidth;
                (*rih)->h32.idEntries[i].bHeight = ih->idEntries[i].bHeight;
                (*rih)->h32.idEntries[i].bColorCount = ih->idEntries[i].bColorCount;
                (*rih)->h32.idEntries[i].bReserved = 0;
                (*rih)->h32.idEntries[i].wPlanes = ih->idEntries[i].wPlanes;
                (*rih)->h32.idEntries[i].wBitCount = ih->idEntries[i].wBitCount;
                (*rih)->h32.idEntries[i].lBytesInRes = ih->idEntries[i].dwBytesInRes;
                (*rih)->h32.idEntries[i].wNameOrdinal = i + 1;
            } else {
                (*rih)->h16.idEntries[i].bWidth = ih->idEntries[i].bWidth;
                (*rih)->h16.idEntries[i].bHeight = ih->idEntries[i].bHeight;
                (*rih)->h16.idEntries[i].bColorCount = ih->idEntries[i].bColorCount;
                (*rih)->h16.idEntries[i].bReserved = 0;
                (*rih)->h16.idEntries[i].wPlanes = ih->idEntries[i].wPlanes;
                (*rih)->h16.idEntries[i].wBitCount = ih->idEntries[i].wBitCount;
                (*rih)->h16.idEntries[i].lBytesInRes = ih->idEntries[i].dwBytesInRes;
                (*rih)->h16.idEntries[i].wNameOrdinal = i + 1;
            }
        }
    }

    if( ih != NULL ) {
        WREMemFree( ih );
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
        bih = (BITMAPINFOHEADER *)( data + pos );
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

    *data = WREMemRealloc( *data, *size + ihsize );
    if( !*data ) {
        return( FALSE );
    }
    memmove( *data + ihsize, *data, *size );
    memcpy( *data, ih, ihsize );
    *size += ihsize;

    WREMemFree( ih );

    return( TRUE );
}

Bool WREStripIconDirectory( BYTE **icon, uint_32 *size )
{
    ICONHEADER  *ih;
    uint_32     ihsize;

    if( icon && *icon && size ) {
        ih = (ICONHEADER *)*icon;
        if( ( ih->idType == 1 ) || ( ih->idType == 2 ) ) {
            ihsize = sizeof(ICONHEADER);
            ihsize += sizeof(ICONDIRENTRY)*(ih->idCount-1);
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

    hs_size = sizeof(CURSORHOTSPOT);

    if( !cursor || !size ) {
        return( FALSE );
    }

    *cursor = WREMemRealloc( *cursor, *size + hs_size );
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

    hs_size = sizeof(CURSORHOTSPOT);
    if( cursor && size && ( *size > hs_size ) ) {
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

    if( cursor && *cursor && size ) {
        return( FALSE );
    }

    ch = (CURSORHEADER *) *cursor;
    cd_size = sizeof(CURSORHEADER);
    cd_size += sizeof(CURSORDIRENTRY)*(ch->cdCount-1);
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

Bool WRECreateCursorEntries( WRECurrentResInfo *curr,
                             void *data, uint_32 size )
{
    WRESCURSORHEADER    *rch;
    CURSORHEADER        *ch;
    uint_16             ord;
    uint_32             rchsize;
    int                 i;
    Bool                ok;

    ok = ( curr && curr->info && data && size );

    if( ok ) {
        if( curr->lang->data ) {
            WREMemFree( curr->lang->data );
            curr->lang->data = NULL;
        }
        curr->lang->Info.Length = 0;
        ok = WRECreateCursorResHeader( &rch, &rchsize, data, size,
                                       curr->info->is32bit );
    }

    if( ok ) {
        curr->lang->data        = (void *)rch;
        curr->lang->Info.Length = rchsize;
        ord = 0;
        ch = (CURSORHEADER *) data;
        for( i=0; ok && i<rch->h16.cwCount; i++ ) {
            ord = WREFindUnusedImageId( curr->info, ord );
            ok = ( ord != 0 );
            if( ok ) {
                if( curr->info->is32bit ) {
                    rch->h32.cdEntries[i].wNameOrdinal = ord;
                } else {
                    rch->h16.cdEntries[i].wNameOrdinal = ord;
                }
                ok = WREGetAndAddCursorImage( data, curr->info->info->dir,
                                              &ch->cdEntries[i], ord );
            }
        }
    }

    return( ok );
}

Bool WRECreateIconEntries( WRECurrentResInfo *curr,
                           void *data, uint_32 size )
{
    WRESICONHEADER      *rih;
    ICONHEADER          *ih;
    uint_16             ord;
    uint_32             rihsize;
    int                 i;
    Bool                ok;

    ok = ( curr && curr->info && data && size );

    if( ok ) {
        if( curr->lang->data ) {
            WREMemFree( curr->lang->data );
            curr->lang->data = NULL;
        }
        curr->lang->Info.Length = 0;
        ok = WRECreateIconResHeader( &rih, &rihsize, data, size,
                                     curr->info->is32bit );
    }

    if( ok ) {
        curr->lang->data        = (void *)rih;
        curr->lang->Info.Length = rihsize;
        ord = 0;
        ih = (ICONHEADER *) data;
        for( i=0; ok && i<rih->h16.cwCount; i++ ) {
            ord = WREFindUnusedImageId( curr->info, ord );
            ok = ( ord != 0 );
            if( ok ) {
                if( curr->info->is32bit ) {
                    rih->h32.idEntries[i].wNameOrdinal = ord;
                } else {
                    rih->h16.idEntries[i].wNameOrdinal = ord;
                }
                ok = WREGetAndAddIconImage( data, curr->info->info->dir,
                                            &ih->idEntries[i], ord );
            }
        }
    }

    return( ok );
}


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
#include <string.h>
#include <limits.h>

#include "wrdll.h"
#include "wrinfo.h"
#include "wrmem.h"
#include "wrdata.h"
#include "bitmap.h"
#include "wricon.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define DEF_MEMFLAGS ( MEMFLAG_MOVEABLE | MEMFLAG_PURE )

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

uint_32 WR_EXPORT WRSizeOfImage( BITMAPINFOHEADER *bih )
{
    uint_32     size;

    size = (uint_32)(DIB_INFO_SIZE( bih->biBitCount ) + bih->biSizeImage);

    return( size );
}

// This function assumes that the data represents icon data WITHOUT
// an icon directory
WORD WR_EXPORT WRCountIconImages( BYTE *data, uint_32 size )
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

int WR_EXPORT WRCreateIconHeader( BYTE *data, uint_32 size, WORD type,
                                  ICONHEADER **ih, uint_32 *ihsize )
{
    BITMAPINFOHEADER    *bih;
    WORD                count;
    uint_32             pos;
    int                 i;

    if( !data || !size || !ih || !ihsize ) {
        return( FALSE );
    }

    count = WRCountIconImages( data, size );
    if( count == 0 ) {
        return( FALSE );
    }

    *ihsize = sizeof(ICONHEADER) + sizeof(ICONDIRENTRY)*(count-1);
    *ih = WRMemAlloc( *ihsize );
    if( *ih == NULL ) {
        return( FALSE );
    }

    (*ih)->idReserved = 0;
    (*ih)->idType     = type;
    (*ih)->idCount    = count;

    for( i=0,pos=0; i<count; i++ ) {
        bih = (BITMAPINFOHEADER *)( data + pos );
        (*ih)->idEntries[i].bWidth      = bih->biWidth;
        (*ih)->idEntries[i].bHeight     = bih->biHeight/2;
        if( type == 1 ) {
            (*ih)->idEntries[i].bColorCount= (1<<(bih->biBitCount));
        } else {
            (*ih)->idEntries[i].bColorCount= 0;
        }
        (*ih)->idEntries[i].bReserved   = 0;
        (*ih)->idEntries[i].wPlanes     = bih->biPlanes;
        (*ih)->idEntries[i].wBitCount   = bih->biBitCount;
        (*ih)->idEntries[i].dwBytesInRes= WRSizeOfImage( bih );
        if( i == 0 ) {
            (*ih)->idEntries[i].dwImageOffset = *ihsize;
        } else {
            (*ih)->idEntries[i].dwImageOffset =
                (*ih)->idEntries[i-1].dwImageOffset +
                    (*ih)->idEntries[i-1].dwBytesInRes;
        }
        pos += (*ih)->idEntries[i].dwBytesInRes;
    }

    return( TRUE );
}

int WR_EXPORT WRCreateCursorResHeader( WRESCURSORHEADER **rch,
                                       uint_32 *rchsize, BYTE *data,
                                       uint_32 data_size, int is32bit )
{
    CURSORHEADER        *ch;
    uint_32             chsize;
    ICONHEADER          *ih;
    uint_32             ihsize;
    int                 i;
    int                 ok;

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
        *rch = (WRESCURSORHEADER *) WRMemAlloc( *rchsize );
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
        WRMemFree( ih );
    }

    return( ok );
}

int WR_EXPORT WRCreateIconResHeader( WRESICONHEADER **rih, uint_32 *rihsize,
                                     BYTE *data, uint_32 data_size,
                                     int is32bit )
{
    ICONHEADER          *pih;
    uint_32             pihsize;
    ICONHEADER          *ih;
    uint_32             ihsize;
    int                 i;
    int                 ok;

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
        *rih = (WRESICONHEADER *) WRMemAlloc( *rihsize );
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
        WRMemFree( ih );
    }

    return( ok );
}

int WR_EXPORT WRAddCursorHotspot( BYTE **cursor, uint_32 *size,
                                  CURSORHOTSPOT *hs )
{
    int hs_size;

    hs_size = sizeof(CURSORHOTSPOT);

    if( !cursor || !size ) {
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

int WR_EXPORT WRGetAndAddCursorImage( BYTE *data, WResDir dir,
                                      CURSORDIRENTRY *cd, int ord )
{
    BYTE                *cursor;
    int                 dup;
    uint_32             size;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    CURSORHOTSPOT       hotspot;
    int                 ok;

    dup = FALSE;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    tname = NULL;
    rname = NULL;

    ok = ( data && dir && cd && cd->dwBytesInRes );

    if ( ok ) {
        cursor = (BYTE *)WRMemAlloc( cd->dwBytesInRes );
        ok = ( cursor != NULL );
    }

    if( ok ) {
        memcpy( cursor, data + cd->dwImageOffset, cd->dwBytesInRes );
        hotspot.xHotspot = cd->wXHotspot;
        hotspot.yHotspot = cd->wYHotspot;
        size = cd->dwBytesInRes;
        ok = WRAddCursorHotspot( &cursor, &size, &hotspot );
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

int WR_EXPORT WRGetAndAddIconImage( BYTE *data, WResDir dir,
                                    ICONDIRENTRY *id, int ord )
{
    BYTE                *icon;
    int                 dup;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    int                 ok;

    dup = FALSE;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    tname = NULL;
    rname = NULL;

    ok = ( data && dir && id && id->dwBytesInRes );

    if ( ok ) {
        icon = (BYTE *)WRMemAlloc( id->dwBytesInRes );
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

int WR_EXPORT WRFindImageId( WRInfo *info, WResTypeNode **otnode,
                             WResResNode **ornode, WResLangNode **lnode,
                             uint_16 type, uint_16 id,
                             WResLangType *ltype )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangType        lang;
    int                 ok;

    ok = ( info && lnode && ( ( type == (uint_16)RT_ICON ) ||
                              ( type == (uint_16)RT_CURSOR ) ) );

    if( ok ) {
        tnode = WRFindTypeNode( info->dir, type, NULL );
        ok = ( tnode != NULL );
    }

    if( ok ) {
        if( otnode != NULL ) {
            *otnode = tnode;
        }
        rnode = WRFindResNode( tnode, id, NULL );
        ok = ( rnode != NULL );
    }

    if( ok ) {
        if( ornode != NULL ) {
            *ornode = rnode;
        }
        if( ltype != NULL ) {
            lang = *ltype;
        } else {
            lang.lang    = DEF_LANG;
            lang.sublang = DEF_SUBLANG;
        }
        *lnode = WRFindLangNodeFromLangType( rnode, &lang );
        ok = ( *lnode != NULL );
    }

    if( !ok ) {
        *lnode = NULL;
    }

    return( ok );
}

int WR_EXPORT WRAppendDataToData( BYTE **d1, uint_32 *d1size,
                                  BYTE *d2, uint_32 d2size )
{
    if( !d1 || !d1size || !d2 || !d2size ) {
        return( FALSE );
    }

    if( ( *d1size + d2size ) > INT_MAX ) {
        return( FALSE );
    }

    *d1 = WRMemRealloc( *d1, *d1size + d2size );
    if( !*d1 ) {
        return( FALSE );
    }

    memcpy( *d1 + *d1size, d2, d2size );
    *d1size += d2size;

    return( TRUE );
}

int WR_EXPORT WRAddCursorImageToData( WRInfo *info, WResLangNode*lnode,
                                      BYTE **data, uint_32 *size,
                                      CURSORHOTSPOT *hotspot )
{
    BYTE        *ldata;
    int         hs_size; // size of hotspot info
    int         ok;

    ldata = NULL;

    ok = ( info && lnode && data && size && hotspot );

    if( ok ) {
        ldata = WRCopyResData( info, lnode );
        ok = ( ldata != NULL );
    }

    if( ok ) {
        hs_size = sizeof(CURSORHOTSPOT);
        memcpy( hotspot, ldata, hs_size );
        ok = WRAppendDataToData( data, size, ldata + hs_size,
                                 lnode->Info.Length - hs_size );
    }

    if( ldata != NULL ) {
        WRMemFree( ldata );
    }

    return( ok );
}

int WR_EXPORT WRAddIconImageToData( WRInfo *info, WResLangNode *lnode,
                                    BYTE **data, uint_32 *size )
{
    BYTE        *ldata;
    int         ok;

    ldata = NULL;

    ok = ( info && lnode && data && size );

    if( ok ) {
        ldata = WRCopyResData( info, lnode );
        ok = ( ldata != NULL );
    }

    if( ok ) {
        ok = WRAppendDataToData( data, size, ldata, lnode->Info.Length );
    }

    if( ldata != NULL ) {
        WRMemFree( ldata );
    }

    return( ok );
}

int WR_EXPORT WRCreateCursorData( WRInfo *info, WResLangNode *lnode,
                                  BYTE **data, uint_32 *size )
{
    WResLangNode        *ilnode;
    BYTE                *ldata;
    WRESCURSORHEADER    *rch;
    CURSORHEADER        *ch;
    CURSORHOTSPOT       hotspot;
    WResLangType        lt;
    uint_16             ord;
    uint_32             osize;
    int                 i;
    int                 is32bit;
    int                 ok;

    ok = ( info && lnode && data && size );

    if( ok ) {
        ldata = WRCopyResData( info, lnode );
        ok = ( ldata != NULL );
    }

    if( ok ) {
        is32bit = FALSE;
        rch = (WRESCURSORHEADER *)ldata;
        *size = sizeof( CURSORHEADER );
        *size += sizeof(CURSORDIRENTRY)*(rch->h16.cwCount-1);
        *data = (BYTE *)WRMemAlloc( *size );
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
            lt = lnode->Info.lang;
            ok = WRFindImageId( info, NULL, NULL, &ilnode, (uint_16)RT_CURSOR, ord, &lt );
            if( ok ) {
                osize = *size;
                ok = WRAddCursorImageToData( info, ilnode, data, size, &hotspot );
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
            WRMemFree( *data );
            *data = NULL;
        }
        *size = 0;
    }

    return( ok );
}

int WR_EXPORT WRCreateIconData( WRInfo *info, WResLangNode *lnode,
                                BYTE **data, uint_32 *size )
{
    WResLangNode        *ilnode;
    BYTE                *ldata;
    WRESICONHEADER      *rih;
    ICONHEADER          *ih;
    WResLangType        lt;
    uint_16             ord;
    uint_32             osize;
    int                 i;
    int                 is32bit;
    int                 ok;

    ok = ( info && lnode && data && size );

    if( ok ) {
        ldata = WRCopyResData( info, lnode );
        ok = ( ldata != NULL );
    }

    if( ok ) {
        is32bit = FALSE;
        rih = (WRESICONHEADER *)ldata;
        *size = sizeof( ICONHEADER );
        *size += sizeof(ICONDIRENTRY)*(rih->h16.cwCount-1);
        *data = (BYTE *)WRMemAlloc( *size );
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
            lt = lnode->Info.lang;
            ok = WRFindImageId( info, NULL, NULL, &ilnode, (uint_16)RT_ICON, ord, &lt );
            if( ok ) {
                osize = *size;
                ok = WRAddIconImageToData( info, ilnode, data, size );
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
            WRMemFree( *data );
            *data = NULL;
        }
        *size = 0;
    }

    return( ok );
}

uint_16 WR_EXPORT WRFindUnusedImageId( WRInfo *info, uint_16 start )
{
    WResLangNode        *lnode;
    int                 found;
    int                 rollover;

    found = FALSE;
    rollover = FALSE;
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
        if( !WRFindImageId( info, NULL, NULL, &lnode, (uint_16)RT_ICON, start, NULL ) ) {
            if( !WRFindImageId( info, NULL, NULL, &lnode, (uint_16)RT_CURSOR, start, NULL ) ) {
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

int WR_EXPORT WRCreateCursorEntries( WRInfo *info, WResLangNode *lnode,
                                     void *data, uint_32 size )
{
    WRESCURSORHEADER    *rch;
    CURSORHEADER        *ch;
    uint_16             ord;
    uint_32             rchsize;
    int                 is32bit;
    int                 i;
    int                 ok;

    is32bit = FALSE;
    ok = ( info && lnode && data && size );

    if( ok ) {
        if( lnode->data ) {
            WRMemFree( lnode->data );
            lnode->data = NULL;
        }
        lnode->Info.Length = 0;
        ok = WRCreateCursorResHeader( &rch, &rchsize, data, size, is32bit );
    }

    if( ok ) {
        lnode->data        = (void *)rch;
        lnode->Info.Length = rchsize;
        ord = 0;
        ch = (CURSORHEADER *) data;
        for( i=0; ok && i<rch->h16.cwCount; i++ ) {
            ord = WRFindUnusedImageId( info, ord );
            ok = ( ord != 0 );
            if( ok ) {
                if( is32bit ) {
                    rch->h32.cdEntries[i].wNameOrdinal = ord;
                } else {
                    rch->h16.cdEntries[i].wNameOrdinal = ord;
                }
                ok = WRGetAndAddCursorImage( data, info->dir,
                                             &ch->cdEntries[i], ord );
            }
        }
    }

    return( ok );
}

int WR_EXPORT WRCreateIconEntries( WRInfo *info, WResLangNode *lnode,
                                   void *data, uint_32 size )
{
    WRESICONHEADER      *rih;
    ICONHEADER          *ih;
    uint_16             ord;
    uint_32             rihsize;
    int                 is32bit;
    int                 i;
    int                 ok;

    is32bit = FALSE;
    ok = ( info && lnode && data && size );

    if( ok ) {
        if( lnode->data ) {
            WRMemFree( lnode->data );
            lnode->data = NULL;
        }
        lnode->Info.Length = 0;
        ok = WRCreateIconResHeader( &rih, &rihsize, data, size, is32bit );
    }

    if( ok ) {
        lnode->data        = (void *)rih;
        lnode->Info.Length = rihsize;
        ord = 0;
        ih = (ICONHEADER *) data;
        for( i=0; ok && i<rih->h16.cwCount; i++ ) {
            ord = WRFindUnusedImageId( info, ord );
            ok = ( ord != 0 );
            if( ok ) {
                if( is32bit ) {
                    rih->h32.idEntries[i].wNameOrdinal = ord;
                } else {
                    rih->h16.idEntries[i].wNameOrdinal = ord;
                }
                ok = WRGetAndAddIconImage( data, info->dir,
                                           &ih->idEntries[i], ord );
            }
        }
    }

    return( ok );
}

int WR_EXPORT WRDeleteGroupImages( WRInfo *info, WResLangNode *lnode,
                                   uint_16 type )
{
    WResLangType        lt;
    WResTypeNode        *itnode;
    WResResNode         *irnode;
    WResLangNode        *ilnode;
    void                *data;
    WRESICONHEADER      *ih;
    WRESCURSORHEADER    *ch;
    int                 i;
    int                 is32bit;
    uint_16             ord;
    int                 ok;

    ok = ( info && lnode &&
           ( ( type == (uint_16)RT_GROUP_ICON ) ||
             ( type == (uint_16)RT_GROUP_CURSOR ) ) );

    if( ok ) {
        data = WRCopyResData( info, lnode );
        ok = ( data != NULL );
    }

    if( ok ) {
        is32bit = FALSE;
        if( type == (uint_16)RT_GROUP_ICON ) {
            ih = (WRESICONHEADER *)data;
            for( i=0; ok && i<ih->h16.cwCount; i++ ) {
                if( is32bit ) {
                    ord = (uint_16) ih->h32.idEntries[i].wNameOrdinal;
                } else {
                    ord = (uint_16) ih->h16.idEntries[i].wNameOrdinal;
                }
                lt = lnode->Info.lang;
                if( WRFindImageId( info, &itnode, &irnode, &ilnode, (uint_16)RT_ICON, ord, &lt ) ) {
                    if( ilnode->data ) {
                        WRMemFree( ilnode->data );
                        ilnode->data = NULL;
                    }
                    ok = WRRemoveLangNodeFromDir( info->dir, &itnode,
                                                  &irnode, &ilnode );
                }
            }
        } else {
            ch = (WRESCURSORHEADER *)data;
            for( i=0; ok && i<ch->h16.cwCount; i++ ) {
                if( is32bit ) {
                    ord = (uint_16) ch->h32.cdEntries[i].wNameOrdinal;
                } else {
                    ord = (uint_16) ch->h16.cdEntries[i].wNameOrdinal;
                }
                lt = lnode->Info.lang;
                if( WRFindImageId( info, &itnode, &irnode, &ilnode, (uint_16)RT_CURSOR, ord, &lt ) ) {
                    if( ilnode->data ) {
                        WRMemFree( ilnode->data );
                        ilnode->data = NULL;
                    }
                    ok = WRRemoveLangNodeFromDir( info->dir, &itnode,
                                                  &irnode, &ilnode );
                }
            }
        }
    }

    if( data != NULL ) {
        WRMemFree( data );
    }

    return( ok );
}


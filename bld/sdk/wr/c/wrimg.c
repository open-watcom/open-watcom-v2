/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Resource editor image manipulation.
*
****************************************************************************/


#include "wrglbl.h"
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include "wrimg.h"
#include "wrmsg.h"
#include "wrsvres.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "pathgrp2.h"

#include "clibext.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define DEF_MEMFLAGS (MEMFLAG_MOVEABLE | MEMFLAG_PURE)

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

bool WRLoadBitmapFile( WRInfo *info )
{
    bool                ok;
    FILE                *fp;
    long                file_length;
    pgroup2             pg;
    WResID              *type;
    WResID              *name;
    WResLangType        def_lang;

    fp = NULL;
    def_lang.sublang = DEF_LANG;
    def_lang.lang = DEF_SUBLANG;

    ok = ( info != NULL );

    if( ok ) {
        ok = ( (fp = ResOpenFileRO( info->file_name )) != NULL );
    }
    if( ok ) {
        ok = !RESSEEK( fp, 0, SEEK_END );
    }
    if( ok ) {
        file_length = RESTELL( fp );
        ok = ( file_length != 0 && file_length != -1 );
    }

    if( ok ) {
        type = WResIDFromNum( RESOURCE2INT( RT_BITMAP ) );
        ok = (type != NULL);
    }

    if( ok ) {
        _splitpath2( info->file_name, pg.buffer, NULL, NULL, &pg.fname, NULL );
        name = WResIDFromStr( pg.fname );
        ok = ( name != NULL );
    }

    if( ok ) {
        ok = ( (info->dir = WResInitDir()) != NULL );
    }

    if( ok ) {
        ok = !WResAddResource( type, name, 0, sizeof( BITMAPFILEHEADER ),
                               file_length - sizeof( BITMAPFILEHEADER ),
                               info->dir, &def_lang, NULL );
    }

    if( fp != NULL ) {
        ResCloseFile( fp );
    }

    if( name != NULL ) {
        MemFree( name );
    }

    if( type != NULL ) {
        MemFree( type );
    }

    return( ok );
}

bool WRLoadIconFile( WRInfo *info )
{
    BYTE                *data;
    size_t              data_size;
    ICONHEADER          *pih;
    size_t              pihsize;
    RESICONHEADER       *rih;
    size_t              rihsize;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    bool                dup;
    int                 i;
    bool                ok;
    pgroup2             pg;

    data = NULL;
    rih = NULL;
    dup = false;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    tname = NULL;
    rname = NULL;

    ok = ( info != NULL && info->file_name != NULL );

    if( ok ) {
        ok = WRReadEntireFile( info->file_name, &data, &data_size );
    }

    if( ok ) {
        pih = (ICONHEADER *)data;
        pihsize = sizeof( ICONHEADER );
        pihsize += sizeof( ICONDIRENTRY ) * ( pih->idCount - 1 );
        ok = WRCreateIconResHeader( &rih, &rihsize, data, data_size );
    }

    if( ok ) {
        ok = ( (info->dir = WResInitDir()) != NULL );
    }

    if( ok ) {
        tname = WResIDFromNum( RESOURCE2INT( RT_GROUP_ICON ) );
        ok = ( tname != NULL );
    }

    if( ok ) {
        _splitpath2( info->file_name, pg.buffer, NULL, NULL, &pg.fname, NULL );
        rname = WResIDFromStr( pg.fname );
        ok = ( rname != NULL );
    }

    if ( ok ) {
        ok = !WResAddResource( tname, rname, DEF_MEMFLAGS, 0, rihsize, info->dir, &lang, &dup );
    }

    if( ok ) {
        ok = WRFindAndSetData( info->dir, tname, rname, &lang, rih );
    }

    if( ok ) {
        for( i = 0; ok && i < pih->idCount; i++ ) {
            ok = WRGetAndAddIconImage( data, info->dir, &pih->idEntries[i], i + 1 );
        }
    }

    if( !ok ) {
        if( info->dir != NULL ) {
            WRFreeWResDirData( info->dir );
            WResFreeDir( info->dir );
            info->dir = NULL;
        }
    }

    if( data != NULL ) {
        MemFree( data );
    }

    if( tname != NULL ) {
        MemFree( tname );
    }

    if( rname != NULL ) {
        MemFree( rname );
    }

    return( ok );
}

bool WRLoadCursorFile( WRInfo *info )
{
    BYTE                *data;
    size_t              data_size;
    CURSORHEADER        *ch;
    size_t              chsize;
    RESCURSORHEADER     *rch;
    size_t              rchsize;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    bool                dup;
    int                 i;
    bool                ok;
    pgroup2             pg;

    data = NULL;
    rch = NULL;
    dup = false;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    tname = NULL;
    rname = NULL;

    ok = ( info != NULL && info->file_name != NULL );

    if( ok ) {
        ok = WRReadEntireFile( info->file_name, &data, &data_size );
    }

    if( ok ) {
        ch = (CURSORHEADER *)data;
        chsize = sizeof( CURSORHEADER );
        chsize += sizeof( CURSORDIRENTRY ) * ( ch->cdCount - 1 );
        ok = WRCreateCursorResHeader( &rch, &rchsize, data, data_size );
    }

    if( ok ) {
        ok = ( (info->dir = WResInitDir()) != NULL );
    }

    if( ok ) {
        tname = WResIDFromNum( RESOURCE2INT( RT_GROUP_CURSOR ) );
        ok = ( tname != NULL );
    }

    if( ok ) {
        _splitpath2( info->file_name, pg.buffer, NULL, NULL, &pg.fname, NULL );
        rname = WResIDFromStr( pg.fname );
        ok = ( rname != NULL );
    }

    if ( ok ) {
        ok = !WResAddResource( tname, rname, DEF_MEMFLAGS, 0, rchsize, info->dir, &lang, &dup );
    }

    if( ok ) {
        ok = WRFindAndSetData( info->dir, tname, rname, &lang, rch );
    }

    if( ok ) {
        for( i = 0; ok && i < ch->cdCount; i++ ) {
            ok = WRGetAndAddCursorImage( data, info->dir, &ch->cdEntries[i], i + 1 );
        }
    }

    if( !ok ) {
        if( info->dir != NULL ) {
            WRFreeWResDirData( info->dir );
            WResFreeDir( info->dir );
            info->dir = NULL;
        }
    }

    if( data != NULL ) {
        MemFree( data );
    }

    if( tname != NULL ) {
        MemFree( tname );
    }

    if( rname != NULL ) {
        MemFree( rname );
    }

    return( ok );
}

static bool WRSaveImageToFile( WRInfo *info, WResTypeNode *tnode, bool backup )
{
    FILE                *src_fp;
    FILE                *dst_fp;
    bool                ok;
    bool                use_rename;
    WResLangNode        *lnode;

    src_fp = NULL;
    dst_fp = NULL;
    lnode = NULL;

    ok = ( info != NULL && tnode != NULL );

    if( ok ) {
        if( backup && WRFileExists( info->save_name ) ) {
            use_rename = ( info->file_name != NULL && stricmp( info->file_name, info->save_name ) );
            ok = WRBackupFile( info->save_name, use_rename );
        }
    }

    if( ok ) {
        if( info->file_name != NULL ) {
            ok = ( (src_fp = ResOpenFileRO( info->tmp_file )) != NULL );
        }
    }

    if( ok ) {
        ok = ( (dst_fp = ResOpenFileNew( info->save_name )) != NULL );
    }

    if( ok ) {
        if( tnode->Head != NULL && tnode->Head->Head != NULL ) {
            lnode = tnode->Head->Head;
        }
        ok = ( lnode != NULL );
    }

    if( ok ) {
        if( lnode->data != NULL ) {
            ok = WRCopyResFromDataToFile( lnode->data, lnode->Info.Length, dst_fp );
        } else {
            ok = WRCopyResFromFileToFile( src_fp, lnode->Info.Offset, lnode->Info.Length, dst_fp );
        }
    }

    if( src_fp != NULL ) {
        ResCloseFile( src_fp );
    }

    if( dst_fp != NULL ) {
        ResCloseFile( dst_fp );
    }

    if( !ok ) {
        if( dst_fp != NULL ) {
            WRDeleteFile( info->save_name );
        }
    }

    return( ok );
}

bool WRSaveBitmapResource( WRInfo *info, bool backup )
{
    bool                ok;
    WResTypeNode        *tnode;
    WResLangNode        *lnode;
    BYTE                *data;
    size_t              dsize;

    ok = ( info != NULL && info->dir != NULL );

    if( ok ) {
        tnode = WRFindTypeNode( info->dir, RESOURCE2INT( RT_BITMAP ), NULL );
        ok = ( tnode != NULL );
        if( !ok ) {
            WRDisplayErrorMsg( WR_PRJNOBMP );
        }
    }

    if( ok ) {
        ok = ( info->dir->NumTypes == 1 && info->dir->NumResources == 1 );
        if( !ok ) {
            WRDisplayErrorMsg( WR_PRJMORETHANONE );
        }
    }

    if( ok ) {
        lnode = NULL;
        if( tnode->Head != NULL && tnode->Head->Head != NULL ) {
            lnode = tnode->Head->Head;
        }
        ok = ( lnode != NULL );
    }

    if( ok ) {
        if( lnode->data == NULL ) {
            lnode->data = WRLoadResData( info->file_name, lnode->Info.Offset, lnode->Info.Length );
        }
        ok = ( lnode->data != NULL && lnode->Info.Length != 0 );
    }

    if( ok ) {
        data = lnode->data;
        dsize = lnode->Info.Length;
        ok = WRAddBitmapFileHeader( &data, &dsize );
    }

    if( ok ) {
        lnode->data = data;
        lnode->Info.Length = dsize;
        ok = WRSaveImageToFile( info, tnode, backup );
    }

    if( ok ) {
        data = lnode->data;
        dsize = lnode->Info.Length;
        ok = WRStripBitmapFileHeader( &data, &dsize );
    }

    if( ok ) {
        lnode->data = data;
        lnode->Info.Length = dsize;
    }

    return( ok );
}

bool WRSaveCursorResource( WRInfo *info, bool backup )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WResLangType        lang;
    BYTE                *data;
    size_t              size;
    bool                use_rename;
    bool                ok;

    data = NULL;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    ok = ( info != NULL && info->dir != NULL );

    if( ok ) {
        if( backup && WRFileExists( info->save_name ) ) {
            use_rename = ( info->file_name != NULL && stricmp( info->file_name, info->save_name ) );
            ok = WRBackupFile( info->save_name, use_rename );
        }
    }

    if( ok ) {
        tnode = WRFindTypeNode( info->dir, RESOURCE2INT( RT_GROUP_CURSOR ), NULL );
        ok = ( tnode != NULL );
        if( !ok ) {
            WRDisplayErrorMsg( WR_PRJNOCUR );
        }
    }

    if( ok ) {
        rnode = tnode->Head;
        ok = ( rnode != NULL );
    }

    if( ok ) {
        lnode = WRFindLangNodeFromLangType( rnode, &lang );
        ok = ( lnode != NULL );
    }

    if( ok ) {
        ok = WRCreateCursorData( info, lnode, &data, &size );
    }

    if( ok ) {
        ok = WRSaveResDataToFile( info->save_name, data, size );
    }

    if( data != NULL ) {
        MemFree( data );
    }

    return( ok );
}

bool WRSaveIconResource( WRInfo *info, bool backup )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WResLangType        lang;
    BYTE                *data;
    size_t              size;
    bool                use_rename;
    bool                ok;

    data = NULL;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    ok = ( info != NULL && info->dir != NULL );

    if( ok ) {
        if( backup && WRFileExists( info->save_name ) ) {
            use_rename = ( info->file_name != NULL && stricmp( info->file_name, info->save_name ) );
            ok = WRBackupFile( info->save_name, use_rename );
        }
    }

    if( ok ) {
        tnode = WRFindTypeNode( info->dir, RESOURCE2INT( RT_GROUP_ICON ), NULL );
        ok = ( tnode != NULL );
        if( !ok ) {
            WRDisplayErrorMsg( WR_PRJNOICON );
        }
    }

    if( ok ) {
        rnode = tnode->Head;
        ok = ( rnode != NULL );
    }

    if( ok ) {
        lnode = WRFindLangNodeFromLangType( rnode, &lang );
        ok = ( lnode != NULL );
    }

    if( ok ) {
        ok = WRCreateIconData( info, lnode, &data, &size );
    }

    if( ok ) {
        ok = WRSaveResDataToFile( info->save_name, data, size );
    }

    if( data != NULL ) {
        MemFree( data );
    }

    return( ok );
}

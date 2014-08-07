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
* Description:  Resource editor image manipulation.
*
****************************************************************************/


#include <wwindows.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrimg.h"
#include "wrmsg.h"
#include "wrsvres.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define DEF_MEMFLAGS (MEMFLAG_MOVEABLE | MEMFLAG_PURE)

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

int WRLoadBitmapFile( WRInfo *info )
{
    int                 ok;
    WResFileID          file_handle;
    long int            file_length;
    char                fn[_MAX_FNAME];
    WResID              *type;
    WResID              *name;
    WResLangType        def_lang;

    file_handle = -1;
    def_lang.sublang = DEF_LANG;
    def_lang.lang = DEF_SUBLANG;

    ok = (info != NULL);

    if( ok ) {
        ok = ((file_handle = ResOpenFileRO( info->file_name )) != -1);
    }

    if( ok ) {
        file_length = filelength( file_handle );
        ok = (file_length != 0 && file_length != -1);
    }

    if( ok ) {
        type = WResIDFromNum( (long)(pointer_int)RT_BITMAP );
        ok = (type != NULL);
    }

    if( ok ) {
        _splitpath( info->file_name, NULL, NULL, fn, NULL );
        name = WResIDFromStr( fn );
        ok = (name != NULL);
    }

    if( ok ) {
        ok = ((info->dir = WResInitDir()) != NULL);
    }

    if( ok ) {
        ok = !WResAddResource( type, name, 0, sizeof( BITMAPFILEHEADER ),
                               file_length - sizeof( BITMAPFILEHEADER ),
                               info->dir, &def_lang, NULL );
    }

    if( file_handle != -1 ) {
        ResCloseFile( file_handle );
    }

    if( name != NULL ) {
        MemFree( name );
    }

    if( type != NULL ) {
        MemFree( type );
    }

    return( ok );
}

int WRLoadIconFile( WRInfo *info )
{
    BYTE                *data;
    uint_32             data_size;
    ICONHEADER          *pih;
    uint_32             pihsize;
    RESICONHEADER       *rih;
    uint_32             rihsize;
    WResFileID          file;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    char                fn[_MAX_FNAME];
    int                 dup;
    int                 i;
    int                 ok;

    data = NULL;
    rih = NULL;
    dup = FALSE;
    file = -1;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    tname = NULL;
    rname = NULL;

    ok = (info != NULL && info->file_name != NULL);

    if( ok ) {
        ok = ((file = ResOpenFileRO( info->file_name )) != -1);
    }

    if( ok ) {
        ok = WRReadEntireFile( file, &data, &data_size );
    }

    if( ok ) {
        pih = (ICONHEADER *)data;
        pihsize = sizeof( ICONHEADER );
        pihsize += sizeof( ICONDIRENTRY ) * (pih->idCount - 1);
        ok = WRCreateIconResHeader( &rih, &rihsize, data, data_size );
    }

    if( ok ) {
        ok = ((info->dir = WResInitDir()) != NULL);
    }

    if( ok ) {
        tname = WResIDFromNum( (uint_16)(pointer_int)RT_GROUP_ICON );
        ok = (tname != NULL);
    }

    if( ok ) {
        _splitpath( info->file_name, NULL, NULL, fn, NULL );
        rname = WResIDFromStr( fn );
        ok = (rname != NULL);
    }

    if ( ok ) {
        ok = !WResAddResource( tname, rname, DEF_MEMFLAGS, 0, rihsize,
                               info->dir, &lang, &dup );
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

    if( file != -1 ) {
        ResCloseFile( file );
    }

    return( ok );
}

int WRLoadCursorFile( WRInfo *info )
{
    BYTE                *data;
    uint_32             data_size;
    CURSORHEADER        *ch;
    uint_32             chsize;
    RESCURSORHEADER     *rch;
    uint_32             rchsize;
    WResFileID          file;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    char                fn[_MAX_FNAME];
    int                 dup;
    int                 i;
    int                 ok;

    data = NULL;
    rch = NULL;
    dup = FALSE;
    file = -1;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    tname = NULL;
    rname = NULL;

    ok = (info != NULL && info->file_name != NULL);

    if( ok ) {
        ok = ((file = ResOpenFileRO( info->file_name )) != -1);
    }

    if( ok ) {
        ok = WRReadEntireFile( file, &data, &data_size );
    }

    if( ok ) {
        ch = (CURSORHEADER *)data;
        chsize = sizeof( CURSORHEADER );
        chsize += sizeof( CURSORDIRENTRY ) * (ch->cdCount - 1);
        ok = WRCreateCursorResHeader( &rch, &rchsize, data, data_size );
    }

    if( ok ) {
        ok = ((info->dir = WResInitDir()) != NULL);
    }

    if( ok ) {
        tname = WResIDFromNum( (uint_16)(pointer_int)RT_GROUP_CURSOR );
        ok = (tname != NULL);
    }

    if( ok ) {
        _splitpath( info->file_name, NULL, NULL, fn, NULL );
        rname = WResIDFromStr( fn );
        ok = (rname != NULL);
    }

    if ( ok ) {
        ok = !WResAddResource( tname, rname, DEF_MEMFLAGS, 0, rchsize,
                               info->dir, &lang, &dup );
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

    if( tname != NULL ) {
        MemFree( tname );
    }

    if( rname != NULL ) {
        MemFree( rname );
    }

    if( file != -1 ) {
        ResCloseFile( file );
    }

    return( ok );
}

static int WRSaveImageToFile( WRInfo *info, WResTypeNode *tnode, int backup )
{
    WResFileID          src;
    WResFileID          dest;
    int                 ok;
    int                 use_rename;
    WResLangNode        *lnode;

    src = -1;
    dest = -1;
    lnode = NULL;

    ok = (info != NULL && tnode != NULL);

    if( ok ) {
        if( backup && WRFileExists( info->save_name ) ) {
            use_rename = (info->file_name != NULL &&
                          stricmp( info->file_name, info->save_name ));
            ok = WRBackupFile( info->save_name, use_rename );
        }
    }

    if( ok ) {
        if( info->file_name != NULL ) {
            ok = ((src = ResOpenFileRO( info->tmp_file )) != -1);
        }
    }

    if( ok ) {
        dest = open( info->save_name, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY,
                     S_IWRITE | S_IREAD );
        ok = (dest != -1);
    }

    if( ok ) {
        if( tnode->Head != NULL && tnode->Head->Head != NULL ) {
            lnode = tnode->Head->Head;
        }
        ok = (lnode != NULL);
    }

    if( ok ) {
        if( lnode->data != NULL ) {
            ok = WRCopyResFromDataToFile( lnode->data, lnode->Info.Length, dest );
        } else {
            ok = WRCopyResFromFileToFile( src, lnode->Info.Offset,
                                          lnode->Info.Length, dest );
        }
    }

    if( src != -1 ) {
        ResCloseFile( src );
    }

    if( dest != -1 ) {
        ResCloseFile( dest );
    }

    if( !ok ) {
        if( dest != -1 ) {
            WRDeleteFile( info->save_name );
        }
    }

    return( ok );
}

int WRSaveBitmapResource( WRInfo *info, int backup )
{
    int                 ok;
    WResTypeNode        *tnode;
    WResLangNode        *lnode;
    BYTE                *data;
    uint_32             dsize;

    ok = (info != NULL && info->dir != NULL);

    if( ok ) {
        tnode = WRFindTypeNode( info->dir, (uint_16)(pointer_int)RT_BITMAP, NULL );
        ok = (tnode != NULL);
        if( !ok ) {
            WRDisplayErrorMsg( WR_PRJNOBMP );
        }
    }

    if( ok ) {
        ok = (info->dir->NumTypes == 1 && info->dir->NumResources == 1);
        if( !ok ) {
            WRDisplayErrorMsg( WR_PRJMORETHANONE );
        }
    }

    if( ok ) {
        lnode = NULL;
        if( tnode->Head != NULL && tnode->Head->Head != NULL ) {
            lnode = tnode->Head->Head;
        }
        ok = (lnode != NULL);
    }

    if( ok ) {
        if( lnode->data == NULL ) {
            lnode->data = WRLoadResData( info->file_name, lnode->Info.Offset,
                                         lnode->Info.Length );
        }
        ok = (lnode->data != NULL && lnode->Info.Length != 0);
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

int WRSaveCursorResource( WRInfo *info, int backup )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WResLangType        lang;
    BYTE                *data;
    uint_32             size;
    int                 use_rename;
    int                 ok;

    data = NULL;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    ok = (info != NULL && info->dir != NULL);

    data = NULL;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    ok = (info != NULL && info->dir != NULL);

    if( ok ) {
        if( backup && WRFileExists( info->save_name ) ) {
            use_rename = (info->file_name != NULL &&
                          stricmp( info->file_name, info->save_name ));
            ok = WRBackupFile( info->save_name, use_rename );
        }
    }

    if( ok ) {
        tnode = WRFindTypeNode( info->dir, (uint_16)(pointer_int)RT_GROUP_CURSOR, NULL );
        ok = (tnode != NULL);
        if( !ok ) {
            WRDisplayErrorMsg( WR_PRJNOCUR );
        }
    }

    if( ok ) {
        rnode = tnode->Head;
        ok = (rnode != NULL);
    }

    if( ok ) {
        lnode = WRFindLangNodeFromLangType( rnode, &lang );
        ok = (lnode != NULL);
    }

    if( ok ) {
        ok = WRCreateCursorData( info, lnode, &data, &size );
    }

    if( ok ) {
        ok = WRSaveDataToFile( info->save_name, data, size );
    }

    if( data != NULL ) {
        MemFree( data );
    }

    return( ok );
}

int WRSaveIconResource( WRInfo *info, int backup )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WResLangType        lang;
    BYTE                *data;
    uint_32             size;
    int                 use_rename;
    int                 ok;

    data = NULL;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    ok = (info != NULL && info->dir != NULL);

    if( ok ) {
        if( backup && WRFileExists( info->save_name ) ) {
            use_rename = (info->file_name != NULL &&
                          stricmp( info->file_name, info->save_name ));
            ok = WRBackupFile( info->save_name, use_rename );
        }
    }

    if( ok ) {
        tnode = WRFindTypeNode( info->dir, (uint_16)(pointer_int)RT_GROUP_ICON, NULL );
        ok = (tnode != NULL);
        if( !ok ) {
            WRDisplayErrorMsg( WR_PRJNOICON );
        }
    }

    if( ok ) {
        rnode = tnode->Head;
        ok = (rnode != NULL);
    }

    if( ok ) {
        lnode = WRFindLangNodeFromLangType( rnode, &lang );
        ok = (lnode != NULL);
    }

    if( ok ) {
        ok = WRCreateIconData( info, lnode, &data, &size );
    }

    if( ok ) {
        ok = WRSaveDataToFile( info->save_name, data, size );
    }

    if( data != NULL ) {
        MemFree( data );
    }

    return( ok );
}

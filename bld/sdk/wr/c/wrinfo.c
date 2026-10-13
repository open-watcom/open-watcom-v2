/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "wrglbl.h"
#include "wrinfoi.h"
#include "wrmsg.h"
#include "pathgrp2.h"

#include "clibext.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

bool WRCopyFileToTemp( WRInfo *info, const char *filename )
{
    if( filename == NULL ) {
        return( false );
    }

    if( !WRFileExists( filename ) ) {
        return( false );
    }

    if( info->tmp_file == NULL ) {
        info->tmp_file = WRGetTempFileName( NULL );
        if( info->tmp_file == NULL ) {
            return( false );
        }
    }

    if( !WRCopyFile( info->tmp_file, filename ) ) {
        WRPrintErrorMsg( WR_BACKUPFAILED, filename, info->tmp_file,
                         strerror( WRGetLastError() ) );
        return( false );
    }

    return( true );
}

WRInfo * WRAPI WRAllocWRInfo( void )
{
    WRInfo  *info;

    if( (info = MemAlloc( sizeof( WRInfo ) )) != NULL ) {
        memset( info, 0, sizeof( WRInfo ) );
    }

    return( info );
}

void WRAPI WRFreeWRInfo( WRInfo *info )
{
    if( info != NULL ) {
        if( info->file_name != NULL ) {
            MemFree( info->file_name );
        }
        if( info->save_name != NULL ) {
            MemFree( info->save_name );
        }
        if( info->internal_filename != NULL ) {
            MemFree( info->internal_filename );
        }
        if( info->tmp_file != NULL ) {
            if( WRFileExists( info->tmp_file ) ) {
                WRDeleteFile( info->tmp_file );
            }
            MemFree( info->tmp_file );
        }
        if( info->dir != NULL ) {
            WRFreeWResDirData( info->dir );
            WResFreeDir( info->dir );
        }
        MemFree( info );
    }
}

void WRAPI WRFreeWResDirData( WResDir dir )
{
    WResTypeNode  *typenode;
    WResResNode   *resnode;
    WResLangNode  *langnode;

    if( dir == NULL ) {
        return;
    }

    for( typenode = dir->Head; typenode != NULL; typenode = typenode->next ) {
        for( resnode = typenode->Head; resnode != NULL; resnode = resnode->next ) {
            for( langnode = resnode->Head; langnode != NULL; langnode = langnode->next ) {
                if( langnode->data != NULL ) {
                    MemFree( langnode->data );
                    langnode->data = NULL;
                }
            }
        }
    }
}

int WRAPI WRCountZeroLengthResources( WResDir dir )
{
    WResTypeNode        *typenode;
    WResResNode         *resnode;
    WResLangNode        *langnode;
    int                 count;

    if( dir == NULL ) {
        return( 0 );
    }

    count = 0;
    for( typenode = dir->Head; typenode != NULL; typenode = typenode->next ) {
        for( resnode = typenode->Head; resnode != NULL; resnode = resnode->next ) {
            for( langnode = resnode->Head; langnode != NULL; langnode = langnode->next ) {
                if( langnode->Info.Length == 0 ) {
                    count++;
                }
            }
        }
    }

    return( count );
}

static bool WRRelinkDir( WResDir dest, WResDir src )
{
    WResLangType    lang;
    WResTypeNode    *dtypenode;
    WResResNode     *dresnode;
    WResLangNode    *dlangnode;
    WResTypeNode    *stypenode;
    WResResNode     *sresnode;
    WResLangNode    *slangnode;

    if( dest == NULL || src == NULL ) {
        return( false );
    }

    if( dest->NumTypes != src->NumTypes || dest->NumResources != src->NumResources ) {
        return( false );
    }

    dtypenode = dest->Head;
    if( dtypenode != NULL ) {
        stypenode = WRFindTypeNodeFromWResID( src, &dtypenode->Info.TypeName );
    }
    while( dtypenode != NULL && stypenode != NULL ) {
        dresnode = dtypenode->Head;
        if( dresnode != NULL ) {
            sresnode = WRFindResNodeFromWResID( stypenode, &dresnode->Info.ResName );
        }
        while( dresnode != NULL && sresnode != NULL ) {
            dlangnode = dresnode->Head;
            if( dlangnode != NULL ) {
                lang = dlangnode->Info.lang;
                slangnode = WRFindLangNodeFromLangType( sresnode, &lang );
            }
            while( dlangnode != NULL && slangnode != NULL ) {
                if( dlangnode->data == NULL ) {
                    dlangnode->Info.Offset = slangnode->Info.Offset;
                }
                dlangnode = dlangnode->next;
                if( dlangnode != NULL ) {
                    lang = dlangnode->Info.lang;
                    slangnode = WRFindLangNodeFromLangType( sresnode, &lang );
                }
            }
            dresnode = dresnode->next;
            if( dresnode != NULL ) {
                sresnode = WRFindResNodeFromWResID( stypenode, &dresnode->Info.ResName );
            }
        }
        dtypenode = dtypenode->next;
        if( dtypenode != NULL ) {
            stypenode = WRFindTypeNodeFromWResID( src, &dtypenode->Info.TypeName );
        }
    }

    return( true );
}

// We really should not reread the file but, alas, time demands
// something a little less pristine.
bool WRRelinkInfo( WRInfo *info )
{
    char        fn_path[_MAX_PATH];
    WRInfo      *tinfo;
    bool        ok;

    tinfo = NULL;

    ok = (info != NULL);

    if( ok ) {
        if( info->internal_filename != NULL ) {
            WRGetInternalRESName( info->save_name, fn_path );
        } else {
            strcpy( fn_path, info->save_name );
        }
        ok = WRCopyFileToTemp( info, fn_path );
    }

    if( ok ) {
        tinfo = WRLoadResource( fn_path, info->save_type );
        ok = (tinfo != NULL);
    }

    if( ok ) {
        ok = WRRelinkDir( info->dir, tinfo->dir );
    }

    if( tinfo != NULL ) {
        WRFreeWRInfo( tinfo );
    }

    return( ok );
}

bool WRAPI WRGetInternalRESName( const char *filename, char *newname )
{
    pgroup2     pg;

    if( filename != NULL && newname != NULL ) {
        _splitpath2( filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
        _makepath( newname, pg.drive, pg.dir, pg.fname, "res" );
        return( true );
    }

    return( false );
}

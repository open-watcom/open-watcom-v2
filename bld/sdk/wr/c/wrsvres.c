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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>

#include "wrglbl.h"
#include "wrsvres.h"
#include "wrmem.h"
#include "wrmsg.h"
#include "wrcmsg.h"
#include "wridfile.h"
#include "wrtmpfil.h"
#include "wrdatai.h"
#include "wrinfoi.h"
#include "wrselft.h"
#include "wresall.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define CHUNK_SIZE 0x7fff

/****************************************************************************/
/* Here comes the code                                                      */
/****************************************************************************/

static void displayDupMsg( WResID *typeName, WResID *resName )
/************************************************************/
{
    char        *type;
    char        *name;

    type = WResIDToStr( typeName );
    name = WResIDToStr( resName );

    if( type && name ) {
        WRPrintErrorMsg( WR_DUPRESFOUND, type, name );
    }

    if( type ) {
        WRMemFree( type );
    }
    if( name ) {
        WRMemFree( name );
    }

    return;
}

static int WRWriteResourceToWRES( WResTypeNode *tnode, WResResNode *rnode,
                                  WResDir new_dir, WResFileID src,
                                  WResFileID dest, int is32bit )
/**********************************/
{
    WResLangType   lt;
    WResLangNode  *lnode;
    uint_32        offset;
    int            dup;

    if( is32bit ) {
        if( ResPadDWord( dest ) ) {
            return( FALSE );
        }
    }

    offset = tell ( dest );

    lnode = rnode->Head;

    while ( lnode ) {
        lt = lnode->Info.lang;
        if ( WResAddResource( &(tnode->Info.TypeName), &(rnode->Info.ResName),
                              lnode->Info.MemoryFlags, offset,
                              lnode->Info.Length, new_dir,
                              &lt, &dup ) || dup ) {
            if( dup ) {
                displayDupMsg( &(tnode->Info.TypeName), &(rnode->Info.ResName) );
            }
            return ( FALSE );
        }

        if ( lnode->data ) {
            if ( !WRCopyResFromDataToFile ( lnode->data,
                                            lnode->Info.Length, dest ) ) {
                return ( FALSE );
            }
        } else {
            if ( !WRCopyResFromFileToFile ( src, lnode->Info.Offset,
                                            lnode->Info.Length, dest ) ) {
                return ( FALSE );
            }
        }
        if ( lnode == rnode->Tail ) {
            break;
        }
        lnode = lnode->Next;
        offset += lnode->Info.Length;
    }


    return ( TRUE );
}

static ResNameOrOrdinal *WRCreateMRESResName( WResResNode *rnode,
                                              WResLangNode *lnode )
/******************************/
{
    ResNameOrOrdinal *name;
    char             *str;
    int               len;

    if ( !rnode || !lnode ) {
        return ( NULL );
    }

    if ( !lnode->Info.lang.lang && !lnode->Info.lang.sublang ) {
        name = WResIDToNameOrOrd ( &(rnode->Info.ResName) );
    } else {
         if ( rnode->Info.ResName.IsName ) {
             len = rnode->Info.ResName.ID.Name.NumChars;
         } else {
             len = UINT16STRLEN;
         }
         len += ( 2 * UINT16STRLEN + 2 + 1 );
         str = (char *) WRMemAlloc ( len );
         if ( !str ) {
             return ( NULL );
         }
         if ( rnode->Info.ResName.IsName ) {
             memcpy ( str, rnode->Info.ResName.ID.Name.Name,
                      rnode->Info.ResName.ID.Name.NumChars );
             sprintf ( str + rnode->Info.ResName.ID.Name.NumChars, "_%u_%u",
                       lnode->Info.lang.lang, lnode->Info.lang.sublang );
         } else {
             sprintf ( str, "%u_%u_%u", rnode->Info.ResName.ID.Num,
                       lnode->Info.lang.lang, lnode->Info.lang.sublang );
         }
         name = ResStrToNameOrOrd ( str );
         WRMemFree ( str );
    }

    return ( name );
}

static int WRWriteResourceToMRES( WResTypeNode *tnode, WResResNode *rnode,
                                  WResFileID src, WResFileID dest )
/********************************************/
{
    WResLangNode       *lnode;
    MResResourceHeader  mheader;
    int                 ok;

    lnode = rnode->Head;
    ok = TRUE;

    while ( lnode && ok ) {
        mheader.Size        = lnode->Info.Length;
        mheader.MemoryFlags = lnode->Info.MemoryFlags;
        mheader.Type        = WResIDToNameOrOrd ( &(tnode->Info.TypeName) );
        mheader.Name        = WRCreateMRESResName ( rnode, lnode );
        ok = ( mheader.Type && mheader.Name );
        if ( ok ) {
            ok = !MResWriteResourceHeader( &mheader, dest, FALSE );
        }
        if ( ok ) {
            if ( lnode->data ) {
                ok = WRCopyResFromDataToFile ( lnode->data,
                                               lnode->Info.Length, dest );
            } else {
                ok = WRCopyResFromFileToFile ( src, lnode->Info.Offset,
                                               lnode->Info.Length, dest );
            }
        }
        if ( mheader.Type ) {
            WRMemFree ( mheader.Type );
        }
        if ( mheader.Name ) {
            WRMemFree ( mheader.Name );
        }
        if ( lnode == rnode->Tail ) {
            break;
        }
        lnode = lnode->Next;
    }

    return ( ok );
}

static int WRWriteResourcesToMRES( WRInfo *info, WResFileID src,
                                   WResFileID dest )
/**********************/
{
    WResDir             old_dir;
    WResTypeNode       *type_node;
    WResResNode        *res_node;

    old_dir = info->dir;

    if ( old_dir == NULL ) {
        type_node = NULL;
    } else {
        type_node = old_dir->Head;
    }

    while ( type_node ) {
        res_node = type_node->Head;
        while ( res_node ) {
            if ( !WRWriteResourceToMRES (type_node, res_node, src, dest) ) {
                return ( FALSE );
            }
            if ( res_node == type_node->Tail ) {
                break;
            }
            res_node = res_node->Next;
        }
        if ( type_node == old_dir->Tail ) {
            break;
        }
        type_node = type_node->Next;
    }

    return ( TRUE );
}

static int WRWriteResourcesToWRES( WRInfo *info, WResDir new_dir,
                                   WResFileID src, WResFileID dest,
                                   int is32bit )
/******************/
{
    WResDir        old_dir;
    WResTypeNode  *type_node;
    WResResNode   *res_node;

    old_dir = info->dir;

    if ( old_dir == NULL ) {
        type_node = NULL;
    } else {
        type_node = old_dir->Head;
    }

    while( type_node ) {
        res_node = type_node->Head;
        while( res_node ) {
            if( !WRWriteResourceToWRES( type_node, res_node, new_dir,
                                        src, dest, is32bit ) ) {
                return( FALSE );
            }
            if( res_node == type_node->Tail ) {
                break;
            }
            res_node = res_node->Next;
        }
        if ( type_node == old_dir->Tail ) {
            break;
        }
        type_node = type_node->Next;
    }

    return ( TRUE );
}

static int WRSaveResourceToWRES( WRInfo *info, WResFileID src, WResFileID dest )
/******************************************************************************/
{
    WResDir     new_dir;
    WRFileType  save_type;
    int         is32bit;
    int         ok;

    ok = ( ( new_dir = WResInitDir() ) != NULL );

    if( ok ) {
        save_type = info->save_type;
        if( ( save_type != WR_WIN16M_RES ) &&
            ( save_type != WR_WIN16W_RES ) &&
            ( save_type != WR_WINNTM_RES ) &&
            ( save_type != WR_WINNTW_RES ) ) {
            if( info->internal_type != WR_DONT_KNOW ) {
                save_type = info->internal_type;
            }
        }
        is32bit = WRIs32Bit( save_type );
        if( is32bit ) {
            new_dir->TargetOS = WRES_OS_WIN32;
        }
    }

    if ( ok ) {
        ok = WRWriteResourcesToWRES( info, new_dir, src, dest, is32bit );
    }

    if ( ok ) {
        ok = !WResWriteDir( dest, new_dir );
    }

    if ( new_dir ) {
        WResFreeDir ( new_dir );
    }

    return ( ok );
}

static int WRSaveResourceToMRES ( WRInfo *info, WResFileID src, WResFileID dest )
/*******************************************************************************/
{
    return ( WRWriteResourcesToMRES ( info, src, dest ) );
}

static int saveResourceToRES( WRInfo *info, int backup, char *save_name,
                              char *file_name )
/************************/
{
    WResFileID  src;
    WResFileID  dest;
    int         is_wres;
    int         ok;
    int         use_rename;
    WRFileType  save_type;

    src  = -1;
    dest = -1;

    ok = TRUE;

    if( ok ) {
        save_type = info->save_type;
        if( ( save_type != WR_WIN16M_RES ) &&
            ( save_type != WR_WIN16W_RES ) &&
            ( save_type != WR_WINNTM_RES ) &&
            ( save_type != WR_WINNTW_RES ) ) {
            if( info->internal_type != WR_DONT_KNOW ) {
                save_type = info->internal_type;
            }
        }
        #ifndef __NT__
            ok = !WRIs32Bit( save_type );
            if( !ok ) {
                WRDisplayErrorMsg( WR_NOSAVE32IN16 );
            }
        #endif
    }

    if( ok ) {
        if( backup && WRFileExists( save_name ) ) {
            use_rename = ( file_name &&
                           stricmp( file_name, save_name ) );
            ok = WRBackupFile( save_name, use_rename );
        }
    }

    if( ok ) {
        if( file_name ) {
            ok = ( ( src = ResOpenFileRO( info->tmp_file ) ) != -1 );
        }
    }

    is_wres = ( ( save_type == WR_WIN16W_RES ) ||
                ( save_type == WR_WINNTW_RES ) );

    if( ok ) {
        if( is_wres ) {
            ok = ( ( dest = WResOpenNewFile( save_name ) ) != -1 );
        } else {
            ok = ( ( dest = MResOpenNewFile( save_name ) ) != -1 );
        }
    }

    if( ok ) {
        if( is_wres ) {
            ok = WRSaveResourceToWRES( info, src, dest );
        } else {
            ok = WRSaveResourceToMRES( info, src, dest );
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
            WRDeleteFile( save_name );
        }
    }

    return( ok );
}

int WRSaveResourceTo_RC( WRInfo *info, int backup )
{
    char                fn_path[_MAX_PATH];
    int                 ok;

    ok = ( info && info->save_name );

    if( ok ) {
        WRGetInternalRESName( info->save_name, fn_path );
        // if the internal save type has not been set up yet
        // then we must take a crude guess
        if( info->internal_type == WR_DONT_KNOW ) {
            info->internal_type =
                WRSelectFileType( (HWND)NULL, fn_path,
                                  #ifdef __NT__
                                      TRUE,
                                  #else
                                      FALSE,
                                  #endif
                                      FALSE, NULL );
        }
        ok = saveResourceToRES( info, backup, fn_path,
                                ( info->internal_filename )
                                    ? info->internal_filename
                                    : info->file_name );
    }

    return( ok );
}

int WRSaveResourceToRES( WRInfo *info, int backup )
{
    return( saveResourceToRES( info, backup, info->save_name,
                               info->file_name ) );
}

int WRCopyResFromFileToFile( WResFileID src, uint_32 offset,
                             uint_32 length, WResFileID dest )
{
    uint_32     size;
    uint_8      *buf;
    int         ok;

    size = 0;
    buf = NULL;

    ok = ( src != -1 && dest != -1 );

    ok = ( ok && ( ( buf = (uint_8 *)WRMemAlloc( CHUNK_SIZE ) ) != NULL ) );

    ok = ( ok && ( lseek( src, offset, SEEK_SET ) != -1 ) );

    while( ok && ( length - size ) > CHUNK_SIZE ) {
        ok = ok && WRReadResData( src, (BYTE *)buf, CHUNK_SIZE );
        ok = ok && WRWriteResData( dest, (BYTE *)buf, CHUNK_SIZE );
        size += CHUNK_SIZE;
    }
    ok = ok && WRReadResData( src, (BYTE *)buf, ( length - size ) );
    ok = ok && WRWriteResData( dest, (BYTE *)buf, ( length - size ) );

    if( buf ) {
        WRMemFree( buf );
    }

    return( ok );
}

int WRCopyResFromDataToFile( void *ResData, uint_32 len, WResFileID dest )
{
    return( WRWriteResData( dest, (BYTE *)ResData, len ) );
}


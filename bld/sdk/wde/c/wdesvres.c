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
#include <string.h>
#include <stdio.h>
#include <io.h>

#include "wresall.h"
#include "wdeglbl.h"
#include "wderesin.h"
#include "wdeactn.h"
#include "wdemem.h"
#include "wdei2mem.h"
#include "wdecsize.h"
#include "wdefdiag.h"
#include "wdeselft.h"
#include "wdetfile.h"
#include "wdewait.h"
#include "wdesvdlg.h"
#include "wde_wres.h"
#include "wdesvres.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool          WdeInfoToData        ( WdeResInfo * );
static void          WdeFreeInfoData      ( WdeResInfo * );
static WResResNode  *WdeRenameWResResNode ( WResTypeNode *, WResResNode *,
                                            WResID * );
static Bool          WdeAddResToType      ( WResTypeNode *, WResResNode * );
static WResTypeNode *WdeAddTypeToDir      ( WResDir, uint_16 );
static WResResNode  *WdeCreateWResResNode ( uint_16, WResID *, WResLangType *,
                                            uint_16, uint_32, uint_32,
                                            void * );
static WResTypeNode *WdeAllocWResTypeNode ( uint_16 );
static WResResNode  *WdeAllocWResResNode  ( uint_16, WResID * );
static WResLangNode *WdeAllocWResLangNode ( WResLangType *, uint_16, uint_32,
                                            uint_32, void * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WdeCreateDLGName( char *filename, char *newname )
{
    char        fn_drive[_MAX_DRIVE];
    char        fn_dir[_MAX_DIR];
    char        fn_name[_MAX_FNAME];
    char        fn_ext[_MAX_EXT+1];

    if( filename && newname ) {
        _splitpath( filename, fn_drive, fn_dir, fn_name, fn_ext );
        strcpy( fn_ext, ".dlg" );
        _makepath( newname, fn_drive, fn_dir, fn_name, fn_ext );
        return( TRUE );
    }

    return( FALSE );
}

Bool WdeSaveResourceToFile( WdeResInfo *res_info )
{
    Bool        ok;
    char        fn[_MAX_PATH];

    WdeSetWaitCursor( TRUE );

    ok = ( res_info && res_info->info );

    if( ok ) {
        if( ( res_info->info->save_type == WR_WIN_RC_DLG ) ||
            ( res_info->info->save_type == WR_WIN_RC ) ) {
            if( res_info->info->internal_type == WR_DONT_KNOW ) {
                WRGetInternalRESName( res_info->info->save_name, fn );
                res_info->info->internal_type =
                    WdeSelectFileType( fn, res_info->is32bit );
            }
        }
    }

    if( ok ) {
        ok = WdeInfoToData( res_info );
    }

    if( ok ) {
        if( ( res_info->info->save_type == WR_WIN_RC_DLG ) ||
            ( res_info->info->save_type == WR_WIN_RC ) ) {
            ok = WdeSaveRC( res_info->info->save_name, res_info );
        }
    }

    // if we are saving a .RES file then auto create a dlg script
    if( ok ) {
        if( ( res_info->info->save_type == WR_WIN16M_RES ) ||
            ( res_info->info->save_type == WR_WIN16W_RES ) ||
            ( res_info->info->save_type == WR_WINNTM_RES ) ||
            ( res_info->info->save_type == WR_WINNTW_RES ) ) {
            char        dlgName[_MAX_PATH];
            if( WdeCreateDLGName( res_info->info->save_name, dlgName ) ) {
                ok = WdeSaveRC( dlgName, res_info );
            }
        }
    }

    if( ok ) {
        ok = WRSaveResource( res_info->info, TRUE );
    }

    if( ok ) {
        WdeSetResModified ( res_info, FALSE );
    }

    if( res_info && ( res_info->info->save_type != WR_WIN_RC_DLG ) ) {
        WdeFreeInfoData ( res_info );
    }

    WdeSetWaitCursor ( FALSE );

    return ( ok );
}

Bool WdeInfoToData ( WdeResInfo *info )
{
    WResTypeNode        *dnode;
    WResResNode         *rnode;
    LIST                *dlist;
    WdeResDlgItem       *ditem;
    void                *data;
    uint_32             size;
    WResLangType        def_lang;


    if ( !info ) {
        return ( FALSE );
    }

    def_lang.lang    = DEF_SUBLANG;
    def_lang.sublang = DEF_SUBLANG;
    dnode            = info->dlg_entry;
    dlist            = info->dlg_item_list;

    while( dlist ) {
        ditem = (WdeResDlgItem *) ListElement( dlist );

        data = NULL;
        size = 0;

        if( ditem->object || ditem->dialog_info ) {
            if( !WdeGetItemData( ditem, &data, &size ) ) {
                return( FALSE );
            }
        }

        if( ditem->rnode && ditem->lnode ) {
            if( data ) {
                rnode = WdeRenameWResResNode( dnode, ditem->rnode,
                                              ditem->dialog_name );
                if( !rnode ) {
                    WdeMemFree( data );
                    return( FALSE );
                }
                ditem->rnode                   = rnode;
                ditem->lnode->data             = data;
                ditem->lnode->Info.Length      = size;
                ditem->lnode->Info.Offset      = 0;
                ditem->lnode->Info.MemoryFlags =
                    ditem->dialog_info->MemoryFlags;
            }
        } else {
            rnode = WdeCreateWResResNode( 1, ditem->dialog_name, &def_lang,
                                          ditem->dialog_info->MemoryFlags, 0,
                                          size, data );
            if( rnode ) {
                if( !info->info->dir ) {
                    info->info->dir = WResInitDir();
                }
                if( !dnode ) {
                    dnode = WdeAddTypeToDir( info->info->dir,
                                             (uint_16) RT_DIALOG );
                    info->dlg_entry = dnode;
                }
                if( info->info->dir && dnode ) {
                    WdeAddResToType( dnode, rnode );
                    ditem->rnode = rnode;
                    ditem->lnode = rnode->Head;
                } else {
                    WdeMemFree( rnode->Head );
                    WdeMemFree( rnode );
                    WdeMemFree( data );
                    return( FALSE );
                }
            } else {
                WdeMemFree( data );
                return( FALSE );
            }

            info->info->dir->NumResources++;
        }

        dlist = ListNext( dlist );
    }

    return( TRUE );
}

void WdeFreeInfoData( WdeResInfo *info )
{
    if( info && info->info ) {
        WRFreeWResDirData( info->info->dir );
    }
}

WdeDialogBoxInfo *WdeGetItemDBI( WdeResDlgItem *ditem )
{
    WdeDialogBoxInfo    *dbi;
    WResID              *name;

    if( !ditem ) {
        return( FALSE );
    }

    if( ditem->object ) {
        Forward( ditem->object, GET_OBJECT_INFO, NULL, &name );
        if( name ) {
            name = WdeCopyWResID( name );
            if( name ) {
                if( ditem->dialog_name ) {
                    WdeMemFree( ditem->dialog_name );
                }
                ditem->dialog_name = name;
            }
        }
        dbi = WdeDBIFromObject( ditem->object );
        if( dbi ) {
            if( ditem->dialog_info ) {
                WdeFreeDialogBoxInfo( ditem->dialog_info );
            }
            ditem->dialog_info = dbi;
        } else {
            return( FALSE );
        }
    } else {
        dbi = ditem->dialog_info;
    }

    return( dbi );
}

Bool WdeGetItemData ( WdeResDlgItem *ditem, void **data, uint_32 *size )
{
    WdeDialogBoxInfo   *dbi;

    if( !ditem || !data || !size ) {
        return( FALSE );
    }

    dbi = WdeGetItemDBI( ditem );
    if( dbi == NULL ) {
        return( FALSE );
    }

    return( WdeDBI2Mem( dbi, data, size ) );
}

WResResNode *WdeRenameWResResNode ( WResTypeNode *tnode, WResResNode *rnode,
                                    WResID *name )
{
    WResResNode *new_rnode;

    if ( !tnode || !rnode || !name ) {
        return ( NULL );
    }

    // check if the names are already the same
    if( WResIDCmp( &rnode->Info.ResName, name ) ) {
        return( rnode );
    }

    new_rnode = WdeAllocWResResNode ( rnode->Info.NumResources, name );

    if ( new_rnode ) {
        if ( tnode->Head == rnode ) {
            tnode->Head = new_rnode;
        }
        if ( tnode->Tail == rnode ) {
            tnode->Tail = new_rnode;
        }
        new_rnode->Head = rnode->Head;
        new_rnode->Tail = rnode->Tail;
        new_rnode->Next = rnode->Next;
        new_rnode->Prev = rnode->Prev;
        if ( rnode->Prev != NULL ) {
            rnode->Prev->Next = new_rnode;
        }
        if ( rnode->Next != NULL ) {
            rnode->Next->Prev = new_rnode;
        }
        WdeMemFree ( rnode );
    }

    return ( new_rnode );
}

Bool WdeAddResToType ( WResTypeNode *tnode, WResResNode *rnode )
{
    if ( !tnode || !rnode ) {
        return ( FALSE );
    }

    rnode->Next = NULL;
    rnode->Prev = tnode->Tail;
    if ( !tnode->Head ) {
        tnode->Head = rnode;
    }
    if ( tnode->Tail ) {
        tnode->Tail->Next = rnode;
    }
    tnode->Tail = rnode;
    tnode->Info.NumResources++;

    return ( TRUE );
}

WResTypeNode *WdeAddTypeToDir ( WResDir dir, uint_16 type )
{
    WResTypeNode *tnode;

    if ( !dir ) {
        return ( NULL );
    }

    tnode = WdeAllocWResTypeNode ( type );

    if ( tnode ) {
        tnode->Next = NULL;
        tnode->Prev = dir->Tail;
        if ( !dir->Head ) {
            dir->Head = tnode;
        }
        if ( dir->Tail ) {
            dir->Tail->Next = tnode;
        }
        dir->Tail = tnode;
        dir->NumTypes++;
    }

    return ( tnode );
}

WResTypeNode *WdeAllocWResTypeNode ( uint_16 type )
{
    WResTypeNode *tnode;

    tnode = (WResTypeNode *) WdeMemAlloc ( sizeof(WResTypeNode) );

    if ( tnode ) {
        memset ( tnode, 0, sizeof(WResTypeNode) );
        tnode->Info.TypeName.ID.Num = type;
    }

    return ( tnode );
}

WResResNode *WdeCreateWResResNode ( uint_16 num_resources, WResID *name,
                                    WResLangType *lang, uint_16 memflags,
                                    uint_32 offset, uint_32 size,
                                    void *data )
{
    WResResNode  *rnode;
    WResLangNode *lnode;

    rnode = NULL;
    lnode = WdeAllocWResLangNode ( lang, memflags, offset, size, data );
    if ( lnode ) {
        rnode = WdeAllocWResResNode ( num_resources, name );
        if ( rnode ) {
            rnode->Head = lnode;
            rnode->Tail = lnode;
        } else {
            WdeMemFree ( lnode );
        }
    }

    return ( rnode );
}

WResResNode *WdeAllocWResResNode( uint_16 num_resources, WResID *name )
{
    WResResNode *new_rnode;
    int          len;

    if( !name ) {
        return( NULL );
    }

    len = sizeof(WResResNode);
    if( name->IsName ) {
        if( name->ID.Name.NumChars != 0 ) {
            len += name->ID.Name.NumChars - 1;
        }
    }
    new_rnode = (WResResNode *) WdeMemAlloc( len );

    if( new_rnode ) {
        new_rnode->Head              = NULL;
        new_rnode->Tail              = NULL;
        new_rnode->Next              = NULL;
        new_rnode->Prev              = NULL;
        new_rnode->Info.NumResources = num_resources;
        len = sizeof(WResID);
        if( name->IsName ) {
            if( name->ID.Name.NumChars != 0 ) {
                len += name->ID.Name.NumChars - 1;
            }
        }
        memcpy( &new_rnode->Info.ResName, name, len );
    }

    return( new_rnode );
}

WResLangNode *WdeAllocWResLangNode ( WResLangType *lang, uint_16 memflags,
                                     uint_32 offset, uint_32 size,
                                     void *data )
{
    WResLangNode *new_lnode;

    if ( !lang ) {
        return ( NULL );
    }

    new_lnode = (WResLangNode *) WdeMemAlloc ( sizeof(WResLangNode) );

    if ( new_lnode ) {
        new_lnode->Next             = NULL;
        new_lnode->Prev             = NULL;
        new_lnode->data             = data;
        new_lnode->Info.lang        = *lang;
        new_lnode->Info.MemoryFlags = memflags;
        new_lnode->Info.Offset      = offset;
        new_lnode->Info.Length      = size;
    }

    return ( new_lnode );
}


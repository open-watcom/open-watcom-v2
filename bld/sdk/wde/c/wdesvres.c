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


#include "wdeglbl.h"
#include <io.h>
#include "wresall.h"
#include "wderesin.h"
#include "wdeactn.h"
#include "wde2data.h"
#include "wdecsize.h"
#include "wdefdiag.h"
#include "wdeselft.h"
#include "wdewait.h"
#include "wdesvdlg.h"
#include "wde_wres.h"
#include "wdesvres.h"
#include "wresdefn.h"
#include "pathgrp2.h"

#include "clibext.h"


/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool          WdeInfoToData( WdeResInfo * );
static void          WdeFreeInfoData( WdeResInfo * );
static WResResNode  *WdeRenameWResResNode( WResTypeNode *, WResResNode *, WResID * );
static bool          WdeAddResToType( WResTypeNode *, WResResNode * );
static WResTypeNode *WdeAddTypeToDir( WResDir, uint_16 );
static WResResNode  *WdeCreateWResResNode( uint_16, WResID *, WResLangType *, ResMemFlags, uint_32, uint_32, void * );
static WResTypeNode *WdeAllocWResTypeNode( uint_16 );
static WResResNode  *WdeAllocWResResNode( uint_16, WResID * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WdeCreateDLGName( const char *filename, char *newname )
{
    pgroup2     pg;

    if( filename != NULL && newname != NULL ) {
        _splitpath2( filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
        _makepath( newname, pg.drive, pg.dir, pg.fname, "dlg" );
        return( true );
    }

    return( false );
}

bool WdeSaveResourceToFile( WdeResInfo *res_info )
{
    bool        ok;
    char        fn[_MAX_PATH];

    WdeSetWaitCursor( true );

    ok = (res_info != NULL && res_info->info != NULL);

    if( ok ) {
        if( res_info->info->save_type == WR_WIN_RC_DLG ||
            res_info->info->save_type == WR_WIN_RC ) {
            if( res_info->info->internal_type == WR_DONT_KNOW ) {
                WRGetInternalRESName( res_info->info->save_name, fn );
                res_info->info->internal_type = WdeSelectFileType( fn, res_info->is32bit );
            }
        }
    }

    if( ok ) {
        ok = WdeInfoToData( res_info );
    }

    if( ok ) {
        if( res_info->info->save_type == WR_WIN_RC_DLG ||
            res_info->info->save_type == WR_WIN_RC ) {
            ok = WdeSaveRC( res_info->info->save_name, res_info );
        }
    }

    // if we are saving a .RES file then auto create a dlg script
    if( ok ) {
        if( res_info->info->save_type == WR_WIN16M_RES ||
            res_info->info->save_type == WR_WIN16W_RES ||
            res_info->info->save_type == WR_WINNTM_RES ||
            res_info->info->save_type == WR_WINNTW_RES ) {
            char        dlgName[_MAX_PATH];
            if( WdeCreateDLGName( res_info->info->save_name, dlgName ) ) {
                ok = WdeSaveRC( dlgName, res_info );
            }
        }
    }

    if( ok ) {
        ok = WRSaveResource( res_info->info, true );
    }

    if( ok ) {
        WdeSetResModified( res_info, FALSE );
    }

    if( res_info != NULL && res_info->info->save_type != WR_WIN_RC_DLG ) {
        WdeFreeInfoData( res_info );
    }

    WdeSetWaitCursor( false );

    return( ok );
}

bool WdeInfoToData( WdeResInfo *info )
{
    WResTypeNode        *dtypenode;
    WResResNode         *resnode;
    LIST                *dlist;
    WdeResDlgItem       *ditem;
    char                *data;
    size_t              dsize;
    WResLangType        lang;


    if( info == NULL ) {
        return( false );
    }

    lang.lang = DEF_SUBLANG;
    lang.sublang = DEF_SUBLANG;
    dtypenode = info->dlg_entry;
    for( dlist = info->dlg_item_list; dlist != NULL; dlist = ListNext( dlist ) ) {
        ditem = (WdeResDlgItem *)ListElement( dlist );
        data = NULL;
        dsize = 0;

        if( ditem->object != NULL || ditem->dialog_info != NULL ) {
            if( !WdeGetItemData( ditem, &data, &dsize ) ) {
                return( false );
            }
        }

        if( ditem->resnode != NULL && ditem->langnode != NULL ) {
            if( data != NULL ) {
                resnode = WdeRenameWResResNode( dtypenode, ditem->resnode, ditem->dialog_name );
                if( resnode == NULL ) {
                    MemFree( data );
                    return( false );
                }
                ditem->resnode = resnode;
                ditem->langnode->data = data;
                ditem->langnode->Info.Length = dsize;
                ditem->langnode->Info.Offset = 0;
                ditem->langnode->Info.res_flags = ditem->dialog_info->res_flags;
            }
        } else {
            resnode = WdeCreateWResResNode( 1, ditem->dialog_name, &lang,
                                          ditem->dialog_info->res_flags, 0,
                                          dsize, data );
            if( resnode != NULL ) {
                if( info->info->dir == NULL ) {
                    info->info->dir = WResInitDir();
                }
                if( dtypenode == NULL ) {
                    dtypenode = WdeAddTypeToDir( info->info->dir, RESOURCE2INT( RT_DIALOG ) );
                    info->dlg_entry = dtypenode;
                }
                if( info->info->dir != NULL && dtypenode != NULL ) {
                    WdeAddResToType( dtypenode, resnode );
                    ditem->resnode = resnode;
                    ditem->langnode = resnode->Head;
                } else {
                    MemFree( resnode->Head );
                    MemFree( resnode );
                    MemFree( data );
                    return( false );
                }
            } else {
                MemFree( data );
                return( false );
            }

            info->info->dir->NumResources++;
        }
    }

    return( true );
}

void WdeFreeInfoData( WdeResInfo *info )
{
    if( info != NULL && info->info != NULL ) {
        WRFreeWResDirData( info->info->dir );
    }
}

WdeDialogBoxInfo *WdeGetItemDBI( WdeResDlgItem *ditem )
{
    WdeDialogBoxInfo    *dbi;
    WResID              *name;

    if( ditem == NULL ) {
        return( NULL );
    }

    if( ditem->object != NULL ) {
        Forward( ditem->object, GET_OBJECT_INFO, NULL, &name );
        if( name != NULL ) {
            name = WdeCopyWResID( name );
            if( name != NULL ) {
                if( ditem->dialog_name != NULL ) {
                    MemFree( ditem->dialog_name );
                }
                ditem->dialog_name = name;
            }
        }
        dbi = WdeAllocDBIFromObject( (WdeDialogObject *)ditem->object );
        if( dbi != NULL ) {
            if( ditem->dialog_info != NULL ) {
                WdeFreeDialogBoxInfo( ditem->dialog_info );
            }
            ditem->dialog_info = dbi;
        }
    } else {
        dbi = ditem->dialog_info;
    }

    return( dbi );
}

bool WdeGetItemData( WdeResDlgItem *ditem, char **data, size_t *dsize )
{
    WdeDialogBoxInfo   *dbi;

    if( ditem == NULL || data == NULL || dsize == NULL ) {
        return( FALSE );
    }

    dbi = WdeGetItemDBI( ditem );
    if( dbi == NULL ) {
        return( FALSE );
    }

    return( WdeAllocDataFromDBI( dbi, data, dsize ) );
}

WResResNode *WdeRenameWResResNode( WResTypeNode *typenode, WResResNode *resnode, WResID *name )
{
    WResResNode *new_resnode;

    if( typenode == NULL || resnode == NULL || name == NULL ) {
        return( NULL );
    }

    // check if the names are already the same
    if( WResIDCmp( &resnode->Info.ResName, name ) ) {
        return( resnode );
    }

    new_resnode = WdeAllocWResResNode( resnode->Info.NumResources, name );

    if( new_resnode != NULL ) {
        if( typenode->Head == resnode ) {
            typenode->Head = new_resnode;
        }
        if( typenode->Tail == resnode ) {
            typenode->Tail = new_resnode;
        }
        new_resnode->Head = resnode->Head;
        new_resnode->Tail = resnode->Tail;
        new_resnode->next = resnode->next;
        new_resnode->prev = resnode->prev;
        if( resnode->prev != NULL ) {
            resnode->prev->next = new_resnode;
        }
        if( resnode->next != NULL ) {
            resnode->next->prev = new_resnode;
        }
        MemFree( resnode );
    }

    return( new_resnode );
}

bool WdeAddResToType( WResTypeNode *typenode, WResResNode *resnode )
{
    if( typenode == NULL || resnode == NULL ) {
        return( FALSE );
    }
    resnode->next = NULL;
    resnode->prev = typenode->Tail;
    if( typenode->Head == NULL ) {
        typenode->Head = resnode;
    }
    if( typenode->Tail != NULL ) {
        typenode->Tail->next = resnode;
    }
    typenode->Tail = resnode;

    typenode->Info.NumResources++;

    return( TRUE );
}

WResTypeNode *WdeAddTypeToDir( WResDir dir, uint_16 type )
{
    WResTypeNode *typenode;

    if( dir == NULL ) {
        return( NULL );
    }
    typenode = WdeAllocWResTypeNode( type );

    if( typenode != NULL ) {
        typenode->next = NULL;
        typenode->prev = dir->Tail;
        if( dir->Head == NULL ) {
            dir->Head = typenode;
        }
        if( dir->Tail != NULL ) {
            dir->Tail->next = typenode;
        }
        dir->Tail = typenode;

        dir->NumTypes++;
    }

    return( typenode );
}

WResTypeNode *WdeAllocWResTypeNode( uint_16 type )
{
    WResTypeNode *typenode;

    typenode = MemAlloc( sizeof( WResTypeNode ) );

    if( typenode != NULL ) {
        memset( typenode, 0, sizeof( WResTypeNode ) );
        typenode->Info.TypeName.ID.Num = type;
    }

    return( typenode );
}

static WResLangNode *WdeAllocWResLangNode( WResLangType *lang, ResMemFlags res_flags,
                                    uint_32 offset, uint_32 dsize, char *data )
{
    WResLangNode *langnode;

    if( lang == NULL ) {
        return( NULL );
    }

    langnode = MemAlloc( sizeof( WResLangNode ) );

    if( langnode != NULL ) {
        langnode->next = NULL;
        langnode->prev = NULL;
        langnode->data = data;
        langnode->Info.lang = *lang;
        langnode->Info.res_flags = res_flags;
        langnode->Info.Offset = offset;
        langnode->Info.Length = dsize;
    }

    return( langnode );
}

WResResNode *WdeCreateWResResNode( uint_16 num_resources, WResID *name,
                                   WResLangType *lang, ResMemFlags res_flags,
                                   uint_32 offset, uint_32 dsize,
                                   void *data )
{
    WResResNode  *resnode;
    WResLangNode *langnode;

    resnode = NULL;
    langnode = WdeAllocWResLangNode( lang, res_flags, offset, dsize, data );
    if( langnode != NULL ) {
        resnode = WdeAllocWResResNode( num_resources, name );
        if( resnode != NULL ) {
            resnode->Head = langnode;
            resnode->Tail = langnode;
        } else {
            MemFree( langnode );
        }
    }

    return( resnode );
}

WResResNode *WdeAllocWResResNode( uint_16 num_resources, WResID *name )
{
    WResResNode *new_resnode;
    size_t      len;

    if( name == NULL ) {
        return( NULL );
    }

    len = sizeof( WResResNode );
    if( name->IsName ) {
        if( name->ID.Name.NumChars != 0 ) {
            len += name->ID.Name.NumChars - 1;
        }
    }
    new_resnode = MemAlloc( len );
    if( new_resnode != NULL ) {
        new_resnode->Head = NULL;
        new_resnode->Tail = NULL;
        new_resnode->next = NULL;
        new_resnode->prev = NULL;
        new_resnode->Info.NumResources = num_resources;
        len -= sizeof( WResResNode ) - sizeof( WResID );
        memcpy( &new_resnode->Info.ResName, name, len );
    }

    return( new_resnode );
}

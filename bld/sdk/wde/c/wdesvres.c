/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "wdei2mem.h"
#include "wdecsize.h"
#include "wdefdiag.h"
#include "wdeselft.h"
#include "wdetfile.h"
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
static WResResNode  *WdeCreateWResResNode( uint_16, WResID *, WResLangType *, uint_16, uint_32, uint_32, void * );
static WResTypeNode *WdeAllocWResTypeNode( uint_16 );
static WResResNode  *WdeAllocWResResNode( uint_16, WResID * );
static WResLangNode *WdeAllocWResLangNode( WResLangType *, uint_16, uint_32, uint_32, void * );

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
    WResTypeNode        *dnode;
    WResResNode         *rnode;
    LIST                *dlist;
    WdeResDlgItem       *ditem;
    void                *data;
    size_t              size;
    WResLangType        def_lang;


    if( info == NULL ) {
        return( false );
    }

    def_lang.lang = DEF_SUBLANG;
    def_lang.sublang = DEF_SUBLANG;
    dnode = info->dlg_entry;
    for( dlist = info->dlg_item_list; dlist != NULL; dlist = ListNext( dlist ) ) {
        ditem = (WdeResDlgItem *)ListElement( dlist );

        data = NULL;
        size = 0;

        if( ditem->object != NULL || ditem->dialog_info != NULL ) {
            if( !WdeGetItemData( ditem, &data, &size ) ) {
                return( false );
            }
        }

        if( ditem->rnode != NULL && ditem->lnode != NULL ) {
            if( data != NULL ) {
                rnode = WdeRenameWResResNode( dnode, ditem->rnode, ditem->dialog_name );
                if( rnode == NULL ) {
                    WRMemFree( data );
                    return( false );
                }
                ditem->rnode = rnode;
                ditem->lnode->data = data;
                ditem->lnode->Info.Length = size;
                ditem->lnode->Info.Offset = 0;
                ditem->lnode->Info.MemoryFlags = ditem->dialog_info->MemoryFlags;
            }
        } else {
            rnode = WdeCreateWResResNode( 1, ditem->dialog_name, &def_lang,
                                          ditem->dialog_info->MemoryFlags, 0,
                                          size, data );
            if( rnode != NULL ) {
                if( info->info->dir == NULL ) {
                    info->info->dir = WResInitDir();
                }
                if( dnode == NULL ) {
                    dnode = WdeAddTypeToDir( info->info->dir, RESOURCE2INT( RT_DIALOG ) );
                    info->dlg_entry = dnode;
                }
                if( info->info->dir != NULL && dnode != NULL ) {
                    WdeAddResToType( dnode, rnode );
                    ditem->rnode = rnode;
                    ditem->lnode = rnode->Head;
                } else {
                    WRMemFree( rnode->Head );
                    WRMemFree( rnode );
                    WRMemFree( data );
                    return( false );
                }
            } else {
                WRMemFree( data );
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
                    WRMemFree( ditem->dialog_name );
                }
                ditem->dialog_name = name;
            }
        }
        dbi = WdeDBIFromObject( ditem->object );
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

bool WdeGetItemData( WdeResDlgItem *ditem, void *data, size_t *psize )
{
    WdeDialogBoxInfo   *dbi;

    if( ditem == NULL || data == NULL || psize == NULL ) {
        return( FALSE );
    }

    dbi = WdeGetItemDBI( ditem );
    if( dbi == NULL ) {
        return( FALSE );
    }

    return( WdeDBI2Mem( dbi, data, psize ) );
}

WResResNode *WdeRenameWResResNode( WResTypeNode *tnode, WResResNode *rnode, WResID *name )
{
    WResResNode *new_rnode;

    if( tnode == NULL || rnode == NULL || name == NULL ) {
        return( NULL );
    }

    // check if the names are already the same
    if( WResIDCmp( &rnode->Info.ResName, name ) ) {
        return( rnode );
    }

    new_rnode = WdeAllocWResResNode( rnode->Info.NumResources, name );

    if( new_rnode != NULL ) {
        if( tnode->Head == rnode ) {
            tnode->Head = new_rnode;
        }
        if( tnode->Tail == rnode ) {
            tnode->Tail = new_rnode;
        }
        new_rnode->Head = rnode->Head;
        new_rnode->Tail = rnode->Tail;
        new_rnode->Next = rnode->Next;
        new_rnode->Prev = rnode->Prev;
        if( rnode->Prev != NULL ) {
            rnode->Prev->Next = new_rnode;
        }
        if( rnode->Next != NULL ) {
            rnode->Next->Prev = new_rnode;
        }
        WRMemFree( rnode );
    }

    return( new_rnode );
}

bool WdeAddResToType( WResTypeNode *tnode, WResResNode *rnode )
{
    if( tnode == NULL || rnode == NULL ) {
        return( FALSE );
    }

    rnode->Next = NULL;
    rnode->Prev = tnode->Tail;
    if( tnode->Head == NULL ) {
        tnode->Head = rnode;
    }
    if( tnode->Tail != NULL ) {
        tnode->Tail->Next = rnode;
    }
    tnode->Tail = rnode;
    tnode->Info.NumResources++;

    return( TRUE );
}

WResTypeNode *WdeAddTypeToDir( WResDir dir, uint_16 type )
{
    WResTypeNode *tnode;

    if( dir == NULL ) {
        return( NULL );
    }

    tnode = WdeAllocWResTypeNode( type );

    if( tnode != NULL ) {
        tnode->Next = NULL;
        tnode->Prev = dir->Tail;
        if( dir->Head == NULL ) {
            dir->Head = tnode;
        }
        if( dir->Tail != NULL ) {
            dir->Tail->Next = tnode;
        }
        dir->Tail = tnode;
        dir->NumTypes++;
    }

    return( tnode );
}

WResTypeNode *WdeAllocWResTypeNode( uint_16 type )
{
    WResTypeNode *tnode;

    tnode = (WResTypeNode *)WRMemAlloc( sizeof( WResTypeNode ) );

    if( tnode != NULL ) {
        memset( tnode, 0, sizeof( WResTypeNode ) );
        tnode->Info.TypeName.ID.Num = type;
    }

    return( tnode );
}

WResResNode *WdeCreateWResResNode( uint_16 num_resources, WResID *name,
                                   WResLangType *lang, uint_16 memflags,
                                   uint_32 offset, uint_32 size,
                                   void *data )
{
    WResResNode  *rnode;
    WResLangNode *lnode;

    rnode = NULL;
    lnode = WdeAllocWResLangNode( lang, memflags, offset, size, data );
    if( lnode != NULL ) {
        rnode = WdeAllocWResResNode( num_resources, name );
        if( rnode != NULL ) {
            rnode->Head = lnode;
            rnode->Tail = lnode;
        } else {
            WRMemFree( lnode );
        }
    }

    return( rnode );
}

WResResNode *WdeAllocWResResNode( uint_16 num_resources, WResID *name )
{
    WResResNode *new_rnode;
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
    new_rnode = (WResResNode *)WRMemAlloc( len );
    if( new_rnode != NULL ) {
        new_rnode->Head = NULL;
        new_rnode->Tail = NULL;
        new_rnode->Next = NULL;
        new_rnode->Prev = NULL;
        new_rnode->Info.NumResources = num_resources;
        len -= sizeof( WResResNode ) - sizeof( WResID );
        memcpy( &new_rnode->Info.ResName, name, len );
    }

    return( new_rnode );
}

WResLangNode *WdeAllocWResLangNode( WResLangType *lang, uint_16 memflags,
                                    uint_32 offset, uint_32 size,
                                    void *data )
{
    WResLangNode *new_lnode;

    if( lang == NULL ) {
        return( NULL );
    }

    new_lnode = (WResLangNode *)WRMemAlloc( sizeof( WResLangNode ) );

    if( new_lnode != NULL ) {
        new_lnode->Next = NULL;
        new_lnode->Prev = NULL;
        new_lnode->data = data;
        new_lnode->Info.lang = *lang;
        new_lnode->Info.MemoryFlags = memflags;
        new_lnode->Info.Offset = offset;
        new_lnode->Info.Length = size;
    }

    return( new_lnode );
}

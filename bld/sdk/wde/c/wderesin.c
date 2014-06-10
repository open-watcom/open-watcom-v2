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


#include "wdeglbl.h"
#include "wderesin.h"
#include "wdemain.h"
#include "wdetfile.h"
#include "wde_wres.h"
#include "wdeedit.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WdeFreeControlList( LIST ** );
static void WdeDestroyMDIWindow( HWND );
void        WdeFreeResItemList( LIST **item_list );


bool WdeFreeResInfo( WdeResInfo *res_info )
{
    if( res_info != NULL ) {
        InitState( res_info->forms_win );
        if( res_info->res_win != NULL ) {
            WdeSetEditMode( res_info, FALSE );
            CloseFormEdit( res_info->forms_win );
        }
        if( res_info->dlg_item_list != NULL ) {
            WdeFreeResItemList( &res_info->dlg_item_list );
        }
        if( res_info->res_win != (HWND)NULL ) {
            SET_WNDINFO( res_info->res_win, (LONG_PTR)NULL );
            WdeDestroyEditWindows( res_info );
            if( IsWindow( res_info->res_win ) ) {
                WdeDestroyMDIWindow( res_info->res_win );
            }
        }
        if( res_info->info != NULL ) {
            WRFreeWRInfo( res_info->info );
        }
        if( res_info->hash_table != NULL ) {
            WdeFreeHashTable( res_info->hash_table );
        }
        if( res_info->sym_name != NULL ) {
            WRMemFree( res_info->sym_name );
        }
        WRMemFree( res_info );
    } else {
        return( FALSE );
    }

    return( TRUE );
}

bool WdeFreeDialogBoxInfo( WdeDialogBoxInfo *dlg_info )
{
    if( dlg_info != NULL ) {
        WdeFreeControlList( &dlg_info->control_list );
        WdeFreeDialogBoxHeader( &dlg_info->dialog_header );
        WRMemFree( dlg_info );
    } else {
        return( FALSE );
    }

    return( TRUE );
}

void WdeFreeResDlgItem( WdeResDlgItem **ditem, bool destroy_object )
{
    if( ditem != NULL && *ditem != NULL ) {
        if( (*ditem)->dialog_name != NULL ) {
            WResIDFree( (*ditem)->dialog_name );
        }
        if( (*ditem)->dialog_info != NULL ) {
            WdeFreeDialogBoxInfo( (*ditem)->dialog_info );
        }
        if( destroy_object && (*ditem)->object != NULL ) {
            Destroy( (*ditem)->object, FALSE );
        }
        (*ditem)->object = NULL;
        WRMemFree( *ditem );
        *ditem = NULL;
    }
}

void WdeFreeControlList( LIST **control_list )
{
    WdeDialogBoxControl *control;
    LIST                *clist;

    if( control_list != NULL && *control_list != NULL ){
        for( clist = *control_list; clist != NULL; clist = ListNext( clist ) ) {
            control = (WdeDialogBoxControl *)ListElement( clist );
            WdeFreeDialogBoxControl( &control );
        }
        ListFree( *control_list );
        *control_list = NULL;
    }
}

void WdeFreeResItemList( LIST **item_list )
{
    LIST          *ilist;
    WdeResDlgItem *item;

    if( item_list != NULL && *item_list != NULL ) {
        for( ilist = *item_list; ilist != NULL; ilist = ListNext( ilist ) ) {
            item = (WdeResDlgItem *)ListElement( ilist );
            if( item->object != NULL ) {
                Destroy( item->object, FALSE );
            } else {
                WdeFreeResDlgItem( &item, TRUE );
            }
        }
        ListFree( *item_list );
        *item_list = NULL;
    }
}

WdeResDlgItem *WdeAllocResDlgItem( void )
{
    WdeResDlgItem  *item;

    item = (WdeResDlgItem *)WRMemAlloc( sizeof( WdeResDlgItem ) );

    if( item != NULL ) {
        memset( item, 0, sizeof( WdeResDlgItem ) );
    }

    return( item );
}

WdeResInfo *WdeAllocResInfo( void )
{
    WdeResInfo  *res_info;

    res_info = (WdeResInfo *)WRMemAlloc( sizeof( WdeResInfo ) );

    if( res_info != NULL ) {
        memset( res_info, 0, sizeof( WdeResInfo ) );
    }

    return( res_info );
}

bool WdeIsResModified( WdeResInfo *res_info )
{
    LIST          *ilist;
    WdeResDlgItem *item;

    if( res_info != NULL ) {
        if( res_info->modified ) {
            return( TRUE );
        }
        for( ilist = res_info->dlg_item_list; ilist != NULL; ilist = ListNext( ilist ) ) {
            item = (WdeResDlgItem *)ListElement( ilist );
            if( item->modified ) {
                return( TRUE );
            }
        }
    }

    return( FALSE );
}

void WdeSetResModified( WdeResInfo *res_info, bool mod )
{
    LIST          *ilist;
    WdeResDlgItem *item;

    if( res_info != NULL ) {
        for( ilist = res_info->dlg_item_list; ilist != NULL; ilist = ListNext( ilist ) ) {
            item = (WdeResDlgItem *)ListElement( ilist );
            item->modified = mod;
        }
        res_info->modified = mod;
    }
}

void WdeDestroyMDIWindow( HWND win )
{
    SendMessage( WdeGetMDIWindowHandle(), WM_MDIDESTROY, (WPARAM)win, 0 );
}

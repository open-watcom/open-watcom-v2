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


#include "commonui.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wreglbl.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wremain.h"
#include "wrenames.h"
#include "wregcres.h"
#include "wreseted.h"
#include "wreftype.h"
#include "wrectl3d.h"
#include "wrerenam.h"
#include "wre_rc.h"
#include "wrdll.h"
#include "wresall.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL CALLBACK WREResRenameProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void         WRESetWinInfo( HWND, WREResRenameInfo * );
static void         WREGetWinInfo( HWND, WREResRenameInfo * );
static WResResNode *WREAllocResNodeFromWResID( WResID * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WRERenameResource( void )
{
    WRECurrentResInfo  curr;
    WREResRenameInfo   info;
    bool               ok;

    info.old_name = NULL;
    info.new_name = NULL;

    ok = WREGetCurrentResource( &curr );

    if( ok ) {
        if( curr.info->current_type == (uint_16)(pointer_int)RT_STRING ) {
            WREDisplayErrorMsg( WRE_NORENAMESTRINGS );
            ok = false;
        }
    }

    if( ok ) {
        info.old_name = &curr.res->Info.ResName;
        if( WREGetNewName( &info ) && info.new_name != NULL ) {
            ok = WRERenameWResResNode( curr.type, &curr.res, info.new_name );
            curr.info->modified = true;
            if( ok ) {
                WRESetResNamesFromType( curr.info, curr.info->current_type,
                                        TRUE, info.new_name, 0 );
            }
        }
    }

    if( info.new_name != NULL ) {
        WRMemFree( info.new_name );
    }

    return( ok );
}

bool WRERenameWResResNode( WResTypeNode *type_node, WResResNode **res_node, WResID *name )
{
    WResResNode *rn, *r;

    if( type_node == NULL || res_node == NULL || *res_node == NULL || name == NULL ) {
        return( FALSE );
    }

    // check if the names are already the same
    if( WResIDCmp( &(*res_node)->Info.ResName, name ) ) {
        return( TRUE );
    }

    if( (rn = WREAllocResNodeFromWResID( name )) == NULL ) {
        return( FALSE );
    }

    r = WREFindResNodeFromWResID( type_node, &rn->Info.ResName );
    if( r != NULL && r != *res_node ) {
        WREDisplayErrorMsg( WRE_DUPRESNAME );
        WRMemFree( rn );
        return( FALSE );
    }

    if( type_node->Head == *res_node ) {
        type_node->Head = rn;
    }

    if( type_node->Tail == *res_node ) {
        type_node->Tail = rn;
    }

    rn->Head = (*res_node)->Head;
    rn->Tail = (*res_node)->Tail;
    rn->Next = (*res_node)->Next;
    rn->Prev = (*res_node)->Prev;
    rn->Info.NumResources = (*res_node)->Info.NumResources;

    if( (*res_node)->Prev != NULL ) {
        (*res_node)->Prev->Next = rn;
    }
    if( (*res_node)->Next != NULL ) {
        (*res_node)->Next->Prev = rn;
    }

    WRMemFree( *res_node );

    *res_node = rn;

    return( TRUE );
}

WResResNode *WREAllocResNodeFromWResID( WResID *id )
{
    WResResNode *rnode;
    size_t      len, id_len;

    if( id == NULL ) {
        return( NULL );
    }

    len = sizeof( WResResNode );
    id_len = sizeof( WResID );

    if( id->IsName ) {
        id_len += id->ID.Name.NumChars - 1;
        len += id->ID.Name.NumChars - 1;
    }

    rnode = (WResResNode *)WRMemAlloc( len );

    if( rnode != NULL ) {
        memset( rnode, 0, len - id_len );
        memcpy( &rnode->Info.ResName, id, id_len );
    }

    return( rnode );
}

bool WREGetNewName( WREResRenameInfo *info )
{
    HWND        dialog_owner;
    DLGPROC     proc_inst;
    HINSTANCE   app_inst;
    INT_PTR     modified;

    dialog_owner = WREGetMainWindowHandle();
    app_inst = WREGetAppInstance();

    proc_inst = (DLGPROC)MakeProcInstance( (FARPROC)WREResRenameProc, app_inst );

    modified = JDialogBoxParam( app_inst, "WRERenameResource", dialog_owner, proc_inst, (LPARAM)info );

    FreeProcInstance( (FARPROC)proc_inst );

    return( modified != -1 && modified == IDOK );
}

void WRESetWinInfo( HWND hDlg, WREResRenameInfo *info )
{
    if( info != NULL && info->old_name != NULL ) {
        WRESetEditWithWResID( GetDlgItem( hDlg, IDM_RENOLD ), info->old_name );
        WRESetEditWithWResID( GetDlgItem( hDlg, IDM_RENNEW ), info->old_name );
        info->new_name = NULL;
    }
}

void WREGetWinInfo( HWND hDlg, WREResRenameInfo *info )
{
    if( info != NULL ) {
        info->new_name = WREGetWResIDFromEdit( GetDlgItem( hDlg, IDM_RENNEW ), NULL );
    }
}

BOOL CALLBACK WREResRenameProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WREResRenameInfo    *info;
    BOOL                ret;

    ret = FALSE;

    switch( message ) {
    case WM_INITDIALOG:
        info = (WREResRenameInfo *)lParam;
        SET_DLGDATA( hDlg, (LONG_PTR)info );
        WRESetWinInfo( hDlg, info );
        ret = TRUE;
        break;

    case WM_SYSCOLORCHANGE:
        WRECtl3dColorChange();
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDM_HELP:
            WREHelpRoutine();
            break;

        case IDOK:
            info = (WREResRenameInfo *)GET_DLGDATA( hDlg );
            WREGetWinInfo( hDlg, info );
            EndDialog( hDlg, TRUE );
            ret = TRUE;
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            break;
        }
    }

    return( ret );
}

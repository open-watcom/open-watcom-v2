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


#include "precomp.h"
#include <ddeml.h>

#include "watcom.h"
#include "waccel.h"
#include "wmenu.h"
#include "wstring.h"

#include "wreglbl.h"
#include "wremain.h"
#include "wremem.h"
#include "wrenames.h"
#include "wregcres.h"
#include "wreseted.h"
#include "wrectl3d.h"
#include "wredel.h"
#include "wre_rc.h"
#include "wrdll.h"
#include "wresall.h"
#include "wredde.h"
#include "wreaccel.h"
#include "wremenu.h"
#include "wrestr.h"
#include "wredlg.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wreimage.h"
#include "wreimg.h"
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
WINEXPORT BOOL CALLBACK WREResDeleteProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WRESetWinInfo( HWND, char * );
static Bool WREQueryDeleteName( char * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WREDeleteCurrResource( Bool force )
{
    WRECurrentResInfo   curr;
    Bool                ok;

    WREGetCurrentResource( &curr );

    ok = WREDeleteResource( &curr, force );

    return( ok );
}

Bool WREDeleteResource( WRECurrentResInfo *curr, Bool force )
{
    char                *name;
    int                 type;
    Bool                ok;
    int                 index;
    HWND                res_lbox;
    WResLangNode        *lnode;

    name = NULL;
    lnode = NULL;

    if( curr->info->current_type == (uint_16)RT_STRING ) {
        return( WREDeleteStringResources( curr, FALSE ) );
    }

    ok = (curr->info != NULL && curr->res != NULL && curr->lang != NULL);

    if( ok )  {
        if( curr->type->Info.TypeName.IsName ) {
            type = 0;
        } else {
            type = curr->type->Info.TypeName.ID.Num;
        }
        name = WREGetResName( curr->res, type );
        ok = (name != NULL);
    }

    if( ok && !force ) {
        ok = WREQueryDeleteName( name );
    }

    // nuke any edit sessions on this resource
    if( ok ) {
        lnode = curr->lang;
        switch( type ) {
        case RT_MENU:
            WREEndLangMenuSession( lnode );
            break;
        case RT_STRING:
            WREEndResStringSessions( curr->info );
            break;
        case RT_ACCELERATOR:
            WREEndLangAccelSession( lnode );
            break;
        case RT_DIALOG:
            WREEndLangDialogSession( lnode );
            break;
        case RT_GROUP_CURSOR:
        case RT_GROUP_ICON:
            ok = WREDeleteGroupImages( curr, (uint_16)type );
            /* fall through */
        case RT_BITMAP:
            if( ok ) {
                WREEndLangImageSession( lnode );
            }
            break;
        }
    }

    if( ok ) {
        ok = WRRemoveLangNodeFromDir( curr->info->info->dir, &curr->type,
                                      &curr->res, &curr->lang );
        curr->info->modified = TRUE;
    }

    if( ok ) {
        if( !curr->type ) {
            curr->info->current_type = 0;
            ok = WREInitResourceWindow( curr->info, 0 );
        } else {
            res_lbox = GetDlgItem( curr->info->info_win, IDM_RNRES );
            index = (int)SendMessage( res_lbox, LB_FINDSTRING, 0, (LPARAM)name );
            if( index == LB_ERR ) {
                index = 0;
            }
            ok = WRESetResNamesFromType( curr->info, curr->info->current_type,
                                         TRUE, NULL, index );
        }
    }

    if( ok ) {
        WRESetTotalText( curr->info );
    }

    if( name != NULL ) {
        WREMemFree( name );
    }

    return( ok );
}

Bool WREDeleteStringResources( WRECurrentResInfo *curr, Bool removing )
{
    WResTypeNode        *tnode;
    char                *text;
    Bool                ok;

    ok = TRUE;

    if( !removing ) {
        text = WREAllocRCString( WRE_ALLSTRINGS );
        ok = WREQueryDeleteName( text );
        if( text != NULL ) {
            WREFreeRCString( text );
        }
    }

    if( ok ) {
        tnode = curr->type;
        if( tnode == NULL ) {
            tnode = WRFindTypeNode( curr->info->info->dir, (uint_16)RT_STRING, NULL );
        }
        if( tnode != NULL ) {
            curr->info->modified = TRUE;
            ok = WRRemoveTypeNodeFromDir( curr->info->info->dir, tnode );
        }
    }

    // nuke any edit sessions on these string resources
    if( ok ) {
        curr->type = NULL;
        if( !removing ) {
            WREEndResStringSessions( curr->info );
        }
        curr->info->current_type = 0;
        ok = WREInitResourceWindow( curr->info, 0 );
    }

    return( ok );
}

Bool WRERemoveEmptyResource( WRECurrentResInfo *curr )
{
    char                *name;
    int                 type;
    Bool                ok;
    int                 index;
    HWND                res_lbox;

    name = NULL;
    ok = TRUE;

    if( ok )  {
        if( curr->type->Info.TypeName.IsName ) {
            type = 0;
        } else {
            type = curr->type->Info.TypeName.ID.Num;
        }
        name = WREGetResName( curr->res, type );
        ok = (name != NULL);
    }

    if( ok ) {
        ok = WRRemoveLangNodeFromDir( curr->info->info->dir, &curr->type,
                                      &curr->res, &curr->lang );
        curr->info->modified = TRUE;
    }

    if( ok ) {
        if( !curr->type ) {
            curr->info->current_type = 0;
            ok = WREInitResourceWindow( curr->info, 0 );
        } else {
            res_lbox = GetDlgItem( curr->info->info_win, IDM_RNRES );
            index = (int)SendMessage( res_lbox, LB_FINDSTRING, 0, (LPARAM)name );
            if( index == LB_ERR ) {
                index = 0;
            }
            ok = WRESetResNamesFromType( curr->info, curr->info->current_type,
                                         TRUE, NULL, index );
        }
    }

    if( name != NULL ) {
        WREMemFree( name );
    }

    return( ok );
}

Bool WREQueryDeleteName( char *name )
{
    HWND        dialog_owner;
    DLGPROC     proc_inst;
    HINSTANCE   app_inst;
    Bool        modified;

    dialog_owner = WREGetMainWindowHandle();
    app_inst = WREGetAppInstance();

    proc_inst = (DLGPROC)MakeProcInstance( (FARPROC)WREResDeleteProc, app_inst );

    modified = JDialogBoxParam( app_inst, "WREDeleteResource", dialog_owner, proc_inst, (LPARAM)name );

    FreeProcInstance( (FARPROC)proc_inst );

    return( modified != -1 && modified == IDOK );
}

void WRESetWinInfo( HWND hDlg, char *name )
{
    WRESetEditWithStr( GetDlgItem( hDlg, IDM_DELNAME ), name );
}

BOOL CALLBACK WREResDeleteProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    char    *name;
    BOOL    ret;

    ret = FALSE;

    switch( message ) {
    case WM_INITDIALOG:
        name = (char *)lParam;
        WRESetWinInfo( hDlg, name );
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

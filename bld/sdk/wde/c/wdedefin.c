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
#include <stdlib.h>

#include "wdeglbl.h"
#include "wdemem.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wderesin.h"
#include "wdeobjid.h"
#include "wdemain.h"
#include "wdeopts.h"
#include "wdestyle.h"
#include "wdegeted.h"
#include "wdedebug.h"
#include "wdestat.h"
#include "wdefutil.h"
#include "wdecurr.h"
#include "wdectl3d.h"
#include "wde_rc.h"
#include "wdedefin.h"
#include "jdlg.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WINEXPORT WdeGenericDefineProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool WdeGenericSetDefineInfo( WdeDefineObjectInfo *, HWND );
static Bool WdeGenericGetDefineInfo( WdeDefineObjectInfo *, HWND );
static void WdeSetObjectStyle( HWND, DialogStyle );
static void WdeGetObjectStyle( HWND, DialogStyle * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static DialogStyle WdeGenericMask =
    WS_POPUP | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_DISABLED |
    WS_BORDER | WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL | WS_SYSMENU | WS_THICKFRAME |
    WS_MINIMIZE | WS_MAXIMIZE | WS_GROUP | WS_TABSTOP;


Bool WdeDefineCurrentObject( WORD w )
{
    Bool    ret;
    POINT   pnt;
    OBJPTR  obj;
    int     old_option;

    pnt.x = 0;
    pnt.y = 0;

    obj = WdeGetCurrObject();

    if( obj != NULL && obj != GetMainObject() ) {
        if( w != IDM_DEFINEOBJECT ) {
            old_option = WdeSetOption( WdeOptUseDefDlg, FALSE );
        }
        ret = Define( obj, &pnt, NULL );
        if( w != IDM_DEFINEOBJECT ) {
            WdeSetOption( WdeOptUseDefDlg, old_option );
        }
    } else {
        ret = FALSE;
    }

    if( ret ) {
        Notify( obj, PRIMARY_OBJECT, NULL );
    }

    return( ret );
}

Bool WdeGenericDefine( WdeDefineObjectInfo *info )
{
    BOOL                 redraw;
    BOOL                 quick;
    BOOL                 destroy_children;
    DLGPROC              proc_inst;
    HINSTANCE            app_inst;

    if( info->obj == NULL ) {
        WdeWriteTrail( "WdeGenericDefine: NULL obj!" );
        return( FALSE );
    }

    WdeSetStatusText( NULL, "", FALSE );
    WdeSetStatusByID( WDE_GENERICDEFINE, -1 );

    if( info->win == NULL && !Forward( info->obj, GET_WINDOW_HANDLE, &info->win, NULL ) ) {
        WdeWriteTrail( "WdeGenericDefine: GET_WINDOW_HANDLE failed!" );
        return( FALSE );
    }

    redraw = FALSE;

    app_inst = WdeGetAppInstance();

    proc_inst = (DLGPROC)MakeProcInstance ( (FARPROC)WdeGenericDefineProc, app_inst );

    redraw = JDialogBoxParam( app_inst, "WdeDefineGeneric", info->win,
                              proc_inst, (LPARAM)info );

    FreeProcInstance ( (FARPROC)proc_inst );

    if( redraw == -1 ) {
        WdeWriteTrail( "WdeGenericDefine: Dialog not created!" );
        return( FALSE );
    } else if( redraw ) {
        destroy_children = TRUE;
        quick = (info->obj_id == DIALOG_OBJ);
        if( !Forward( info->obj, DESTROY_WINDOW, &quick, &destroy_children ) ) {
            WdeWriteTrail( "WdeGenericDefine: DESTROY_WINDOW failed!" );
            return( FALSE );
        }
        if( !Forward( info->obj, CREATE_WINDOW, NULL, NULL ) ) {
            WdeWriteTrail( "WdeGenericDefine: CREATE_WINDOW failed!" );
            return( FALSE );
        }

        if( info->obj_id != DIALOG_OBJ ) {
            Notify( info->obj, PRIMARY_OBJECT, NULL );
        }
    }

    WdeSetStatusReadyText();

    return( TRUE );
}

Bool WdeGenericSetDefineInfo( WdeDefineObjectInfo *info, HWND hDlg )
{
    DialogStyle style;
    char        *text;

    if ( info->obj_id == 0 && !Forward( info->obj, IDENTIFY, &info->obj_id, NULL ) ) {
        WdeWriteTrail( "WdeGenericSetDefineInfo: IDENTIFY failed!" );
        return( FALSE );
    }

    if( info->obj_id == BASE_OBJ ) {
        WdeWriteTrail( "WdeGenericSetDefineInfo: Attempt to define base object.!" );
        return( FALSE );
    } else if( info->obj_id == DIALOG_OBJ ) {
        if( !info->info.d.header || !info->info.d.name ) {
            if( !Forward( info->obj, GET_OBJECT_INFO,
                          &info->info.d.header, &info->info.d.name ) ) {
                WdeWriteTrail( "WdeGenericDefine: GET_OBJECT_INFO failed!" );
                return( FALSE );
            }
        }
        style = GETHDR_STYLE( info->info.d.header );
        if( GETHDR_CAPTION( info->info.d.header ) != NULL ) {
            WdeSetEditWithStr( GETHDR_CAPTION( info->info.d.header ), hDlg, IDB_TEXT );
        }
        text = WdeAllocRCString( WDE_GENERICNAME );
        WdeSetEditWithStr( text, hDlg, IDB_CAPTEXT );
        if( text != NULL ) {
            WdeFreeRCString( text );
        }
    } else {
        if( !info->info.c.info ) {
            if( !Forward ( info->obj, GET_OBJECT_INFO, &info->info.c.info, NULL ) ) {
                WdeWriteTrail( "WdeGenericDefine: GET_OBJECT_INFO failed!" );
                return( FALSE );
            }
        }
        style = GETCTL_STYLE( info->info.c.info );
        text = WdeAllocRCString( WDE_GENERICTEXT );
        WdeSetEditWithStr( text, hDlg, IDB_CAPTEXT );
        if( text != NULL ) {
            WdeFreeRCString( text );
        }
        if( GETCTL_TEXT( info->info.c.info ) ) {
            WdeSetEditWithResNameOr( GETCTL_TEXT( info->info.c.info ), hDlg, IDB_TEXT );
        }
    }

    WdeSetDefineObjectSymbolInfo( info, hDlg );

    if( style & 0xffff0000 ) {
        WdeSetWinStyles( hDlg, style, WdeGenericMask );
        if( (style & WS_CAPTION) == WS_CAPTION ) {
            CheckDlgButton( hDlg, IDB_WS_CAPTION, 1 );
        }
    } else {
        CheckDlgButton( hDlg, IDB_WS_OVERLAPPED, 1 );
    }

    WdeSetObjectStyle( hDlg, style & 0x0000ffff );

    return( TRUE );
}

Bool WdeGenericGetDefineInfo( WdeDefineObjectInfo *info, HWND hDlg )
{
    DialogStyle style;
    DialogStyle cstyle;
    void        *vp;
    Bool        mod;

    WdeGetWinStyles( hDlg, &style, WdeGenericMask );

    WdeGetObjectStyle( hDlg, &cstyle );

    style = (style & 0xffff0000) | (cstyle & 0x0000ffff);

    if( info->obj_id == BASE_OBJ ) {
        WdeWriteTrail( "WdeGenericSetDefineInfo: Attempt to define base object.!" );
        return( FALSE );
    } else if( info->obj_id == DIALOG_OBJ ) {
        SETHDR_STYLE( info->info.d.header, style );
        vp = (void *)WdeGetStrFromEdit ( hDlg, IDB_TEXT, &mod );
        if( mod && vp != NULL ) {
            if( GETHDR_CAPTION( info->info.d.header ) ) {
                WdeMemFree( GETHDR_CAPTION( info->info.d.header ) );
            }
            SETHDR_CAPTION( info->info.d.header, (char *)vp );
        }
    } else {
        SETCTL_STYLE( info->info.c.info, style );
        vp = (void *)WdeGetResNameOrFromEdit ( hDlg, IDB_TEXT, &mod );
        if ( mod && vp != NULL ) {
            if( GETCTL_TEXT( info->info.c.info ) ) {
                WdeMemFree( GETCTL_TEXT( info->info.c.info ) );
            }
            SETCTL_TEXT( info->info.c.info, (ResNameOrOrdinal *)vp );
        }
    }

    WdeGetDefineObjectSymbolInfo( info, hDlg );
    WdeGetDefineObjectHelpSymbolInfo( info, hDlg );

    return( TRUE );
}

void WdeSetObjectStyle( HWND hDlg, DialogStyle style )
{
    SendDlgItemMessage( hDlg, IDB_STYLES, EM_LIMITTEXT, 10, 0 );

    WdeSetEditWithUINT32( (uint_32)style, 16, hDlg, IDB_STYLES );
}

void WdeGetObjectStyle( HWND hDlg, DialogStyle *style )
{
    *style = (uint_16)WdeGetUINT32FromEdit( hDlg, IDB_STYLES, NULL );
}

BOOL WINEXPORT WdeGenericDefineProc ( HWND hDlg, UINT message,
                                      WPARAM wParam, volatile LPARAM lParam )
{
    static WdeDefineObjectInfo  *info = NULL;
    BOOL                        ret;
    Bool                        use_id;
    uint_16                     id;

    ret = FALSE;

    if( info != NULL ) {
        if( info->hook_func != NULL ) {
            ret = (*info->hook_func)( hDlg, message, wParam, lParam, info->mask );
        }
        if( !ret ) {
            if( info->obj_id == DIALOG_OBJ ) {
                id = info->info.d.id;
                use_id = info->info.d.use_id;
            } else {
                id = GETCTL_ID( info->info.c.info );
                use_id = TRUE;
            }
            ret = WdeProcessSymbolCombo( hDlg, message, wParam, lParam,
                                         info->res_info->hash_table, id, use_id );
        }
        if( ret ) {
            return( ret );
        }
    }

    switch( message ) {
    case WM_SYSCOLORCHANGE:
        WdeCtl3dColorChange ();
        break;

    case WM_INITDIALOG:
        info = (WdeDefineObjectInfo *)lParam;
        if( !WdeGenericSetDefineInfo( info, hDlg ) ) {
            EndDialog( hDlg, FALSE );
        }
        ret = TRUE;
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDB_HELP:
            WdeHelpRoutine();
            break;

        case IDOK:
            if( !WdeGenericGetDefineInfo( info, hDlg ) ) {
                EndDialog( hDlg, FALSE );
            }
            EndDialog( hDlg, TRUE );
            info = NULL;
            ret = TRUE;
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            info = NULL;
            ret = TRUE;
            break;
        }
    }

    return( ret );
}

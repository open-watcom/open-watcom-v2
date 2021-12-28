/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "wdemsgbx.h"
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
#include "wde.rh"
#include "wdedefin.h"
#include "jdlg.h"
#include "wclbproc.h"


/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT INT_PTR CALLBACK WdeGenericDefineDlgProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool WdeGenericSetDefineInfo( WdeDefineObjectInfo *, HWND );
static bool WdeGenericGetDefineInfo( WdeDefineObjectInfo *, HWND );
static void WdeSetObjectStyle( HWND, DialogStyle );
static void WdeGetObjectStyle( HWND, DialogStyle * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static DialogStyle WdeGenericMask =
    WS_POPUP | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_DISABLED |
    WS_BORDER | WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL | WS_SYSMENU | WS_THICKFRAME |
    WS_MINIMIZE | WS_MAXIMIZE | WS_GROUP | WS_TABSTOP;


bool WdeDefineCurrentObject( WORD w )
{
    bool    ret;
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
        ret = Define( obj, &pnt, NULL ) != 0;
        if( w != IDM_DEFINEOBJECT ) {
            WdeSetOption( WdeOptUseDefDlg, old_option );
        }
    } else {
        ret = false;
    }

    if( ret ) {
        Notify( obj, PRIMARY_OBJECT, NULL );
    }

    return( ret );
}

bool WdeGenericDefine( WdeDefineObjectInfo *info )
{
    INT_PTR              redraw;
    bool                 quick;
    bool                 destroy_children;
    DLGPROC              dlgproc;
    HINSTANCE            app_inst;

    if( info->obj == NULL ) {
        WdeWriteTrail( "WdeGenericDefine: NULL obj!" );
        return( false );
    }

    WdeSetStatusText( NULL, "", false );
    WdeSetStatusByID( WDE_GENERICDEFINE, 0 );

    if( info->win == NULL && !Forward( info->obj, GET_WINDOW_HANDLE, &info->win, NULL ) ) {
        WdeWriteTrail( "WdeGenericDefine: GET_WINDOW_HANDLE failed!" );
        return( false );
    }

    redraw = FALSE;

    app_inst = WdeGetAppInstance();

    dlgproc = MakeProcInstance_DLG( WdeGenericDefineDlgProc, app_inst );
    redraw = JDialogBoxParam( app_inst, "WdeDefineGeneric", info->win, dlgproc, (LPARAM)info );
    FreeProcInstance_DLG( dlgproc );

    if( redraw == -1 ) {
        WdeWriteTrail( "WdeGenericDefine: Dialog not created!" );
        return( false );
    } else if( redraw ) {
        destroy_children = true;
        quick = (info->obj_id == DIALOG_OBJ);
        if( !Forward( info->obj, DESTROY_WINDOW, &quick, &destroy_children ) ) {
            WdeWriteTrail( "WdeGenericDefine: DESTROY_WINDOW failed!" );
            return( false );
        }
        if( !Forward( info->obj, CREATE_WINDOW, NULL, NULL ) ) {
            WdeWriteTrail( "WdeGenericDefine: CREATE_WINDOW failed!" );
            return( false );
        }

        if( info->obj_id != DIALOG_OBJ ) {
            Notify( info->obj, PRIMARY_OBJECT, NULL );
        }
    }

    WdeSetStatusReadyText();

    return( true );
}

bool WdeGenericSetDefineInfo( WdeDefineObjectInfo *info, HWND hDlg )
{
    DialogStyle style;
    char        *text;

    if ( info->obj_id == 0 && !Forward( info->obj, IDENTIFY, &info->obj_id, NULL ) ) {
        WdeWriteTrail( "WdeGenericSetDefineInfo: IDENTIFY failed!" );
        return( false );
    }

    if( info->obj_id == BASE_OBJ ) {
        WdeWriteTrail( "WdeGenericSetDefineInfo: Attempt to define base object.!" );
        return( false );
    } else if( info->obj_id == DIALOG_OBJ ) {
        if( !info->info.d.header || !info->info.d.name ) {
            if( !Forward( info->obj, GET_OBJECT_INFO,
                          &info->info.d.header, &info->info.d.name ) ) {
                WdeWriteTrail( "WdeGenericDefine: GET_OBJECT_INFO failed!" );
                return( false );
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
                return( false );
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
            CheckDlgButton( hDlg, IDB_WS_CAPTION, BST_CHECKED );
        }
    } else {
        CheckDlgButton( hDlg, IDB_WS_OVERLAPPED, BST_CHECKED );
    }

    WdeSetObjectStyle( hDlg, style & 0x0000ffff );

    return( true );
}

bool WdeGenericGetDefineInfo( WdeDefineObjectInfo *info, HWND hDlg )
{
    DialogStyle style;
    DialogStyle cstyle;
    void        *vp;
    bool        mod;

    WdeGetWinStyles( hDlg, &style, WdeGenericMask );

    WdeGetObjectStyle( hDlg, &cstyle );

    style = (style & 0xffff0000) | (cstyle & 0x0000ffff);

    if( info->obj_id == BASE_OBJ ) {
        WdeWriteTrail( "WdeGenericSetDefineInfo: Attempt to define base object.!" );
        return( false );
    } else if( info->obj_id == DIALOG_OBJ ) {
        SETHDR_STYLE( info->info.d.header, style );
        vp = (void *)WdeGetStrFromEdit ( hDlg, IDB_TEXT, &mod );
        if( mod && vp != NULL ) {
            if( GETHDR_CAPTION( info->info.d.header ) ) {
                WRMemFree( GETHDR_CAPTION( info->info.d.header ) );
            }
            SETHDR_CAPTION( info->info.d.header, (char *)vp );
        }
    } else {
        SETCTL_STYLE( info->info.c.info, style );
        vp = (void *)WdeGetResNameOrFromEdit ( hDlg, IDB_TEXT, &mod );
        if ( mod && vp != NULL ) {
            if( GETCTL_TEXT( info->info.c.info ) ) {
                WRMemFree( GETCTL_TEXT( info->info.c.info ) );
            }
            SETCTL_TEXT( info->info.c.info, (ResNameOrOrdinal *)vp );
        }
    }

    WdeGetDefineObjectSymbolInfo( info, hDlg );
    WdeGetDefineObjectHelpSymbolInfo( info, hDlg );

    return( true );
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

INT_PTR CALLBACK WdeGenericDefineDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    static WdeDefineObjectInfo  *info = NULL;
    bool                        ret;
    bool                        use_id;
    uint_16                     id;

    ret = false;

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
                use_id = true;
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
        ret = true;
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDB_HELP:
            WdeHelpRoutine();
            break;

        case IDOK:
            EndDialog( hDlg, WdeGenericGetDefineInfo( info, hDlg ) );
            info = NULL;
            ret = true;
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            info = NULL;
            ret = true;
            break;
        }
    }

    return( ret );
}

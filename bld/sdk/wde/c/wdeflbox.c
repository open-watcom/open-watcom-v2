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
#include "wi163264.h"

#include "wdeglbl.h"
#include "wdemem.h"
#include "wderesin.h"
#include "wdeobjid.h"
#include "wdefutil.h"
#include "wde_wres.h"
#include "wdemain.h"
#include "wdedebug.h"
#include "wdedefsz.h"
#include "wdeoinfo.h"
#include "wde_rc.h"
#include "wdecctl.h"
#include "wdeflbox.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeLBoxObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL    WINEXPORT WdeLBoxDispatcher( ACTION, WdeLBoxObject *, void *, void * );
extern LRESULT WINEXPORT WdeLBoxSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeLBox( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeLBCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeLBoxDestroy( WdeLBoxObject *, BOOL *, void * );
static BOOL     WdeLBoxValidateAction( WdeLBoxObject *, ACTION *, void * );
static BOOL     WdeLBoxCopyObject( WdeLBoxObject *, WdeLBoxObject **, WdeLBoxObject * );
static BOOL     WdeLBoxIdentify( WdeLBoxObject *, OBJ_ID *, void * );
static BOOL     WdeLBoxGetWndProc( WdeLBoxObject *, WNDPROC *, void * );
static BOOL     WdeLBoxGetWindowClass( WdeLBoxObject *, char **, void * );
static BOOL     WdeLBoxDefine( WdeLBoxObject *, POINT *, void * );
static void     WdeLBoxSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeLBoxGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeLBoxDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeLBoxDispatch;
static WdeDialogBoxControl      *WdeDefaultLBox = NULL;
static int                      WdeLBoxWndExtra;
static WNDPROC                  WdeOriginalLBoxProc;
//static WNDPROC                WdeLBoxProc;

static DISPATCH_ITEM WdeLBoxActions[] = {
    { DESTROY,          (BOOL (*)( OBJPTR, void *, void * ))WdeLBoxDestroy          },
    { COPY,             (BOOL (*)( OBJPTR, void *, void * ))WdeLBoxCopyObject       },
    { VALIDATE_ACTION,  (BOOL (*)( OBJPTR, void *, void * ))WdeLBoxValidateAction   },
    { IDENTIFY,         (BOOL (*)( OBJPTR, void *, void * ))WdeLBoxIdentify         },
    { GET_WINDOW_CLASS, (BOOL (*)( OBJPTR, void *, void * ))WdeLBoxGetWindowClass   },
    { DEFINE,           (BOOL (*)( OBJPTR, void *, void * ))WdeLBoxDefine           },
    { GET_WND_PROC,     (BOOL (*)( OBJPTR, void *, void * ))WdeLBoxGetWndProc       }
};

#define MAX_ACTIONS      (sizeof( WdeLBoxActions ) / sizeof( DISPATCH_ITEM ))

OBJPTR WINEXPORT WdeLBoxCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeLBox( parent, obj_rect, handle,
                             LBS_STANDARD, "", LISTBOX_OBJ ) );
    } else {
        return( WdeLBCreate( parent, obj_rect, NULL,
                             LISTBOX_OBJ, (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeLBox( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_VISIBLE | WS_TABSTOP | WS_CHILD;
    SETCTL_STYLE( WdeDefaultLBox, style );
    SETCTL_TEXT( WdeDefaultLBox, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultLBox, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeLBCreate( parent, obj_rect, handle, id, WdeDefaultLBox );

    WdeMemFree( GETCTL_TEXT( WdeDefaultLBox ) );
    SETCTL_TEXT( WdeDefaultLBox, NULL );

    return( new );
}

OBJPTR WdeLBCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeLBoxObject *new;

    WdeDebugCreate( "LBox", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeLBoxCreate: LBox has no parent!" );
        return( NULL );
    }

    new = (WdeLBoxObject *)WdeMemAlloc( sizeof( WdeLBoxObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeLBoxCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeLBoxDispatch;

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeLBoxCreate: CONTROL_OBJ not created!" );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeLBoxCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeLBoxCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    return( new );
}

BOOL WINEXPORT WdeLBoxDispatcher( ACTION act, WdeLBoxObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "LBox", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeLBoxActions[i].id == act ) {
            return( WdeLBoxActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeLBoxInit( Bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, "LISTBOX", &wc );
    WdeOriginalLBoxProc = wc.lpfnWndProc;
    WdeLBoxWndExtra = wc.cbWndExtra;

    if( first ) {
#if 0
        if( wc.style & CS_GLOBALCLASS ) {
            wc.style ^= CS_GLOBALCLASS;
        }
        if( wc.style & CS_PARENTDC ) {
            wc.style ^= CS_PARENTDC;
        }
        wc.style |= CS_HREDRAW | CS_VREDRAW;
        wc.hInstance = WdeApplicationInstance;
        wc.lpszClassName = "wdelistbox";
        wc.cbWndExtra += sizeof( OBJPTR );
        //wc.lpfnWndProc = WdeLBoxSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeLBoxInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultLBox = WdeAllocDialogBoxControl();
    if( WdeDefaultLBox == NULL ) {
        WdeWriteTrail( "WdeLBoxInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultLBox, 0 );
    SETCTL_ID( WdeDefaultLBox, 0 );
    SETCTL_EXTRABYTES( WdeDefaultLBox, 0 );
    SETCTL_SIZEX( WdeDefaultLBox, 0 );
    SETCTL_SIZEY( WdeDefaultLBox, 0 );
    SETCTL_SIZEW( WdeDefaultLBox, 0 );
    SETCTL_SIZEH( WdeDefaultLBox, 0 );
    SETCTL_TEXT( WdeDefaultLBox, NULL );
    SETCTL_CLASSID( WdeDefaultLBox, ResNumToControlClass( CLASS_LISTBOX ) );

    WdeLBoxDispatch = MakeProcInstance( (FARPROC)WdeLBoxDispatcher, WdeGetAppInstance() );

    return( TRUE );
}

void WdeLBoxFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultLBox );
    FreeProcInstance( WdeLBoxDispatch );
}

BOOL WdeLBoxDestroy( WdeLBoxObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeLBoxDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WdeMemFree( obj );

    return( TRUE );
}

BOOL WdeLBoxValidateAction( WdeLBoxObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeLBoxActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeLBoxCopyObject( WdeLBoxObject *obj, WdeLBoxObject **new, WdeLBoxObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeLBoxCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeLBoxObject *)WdeMemAlloc( sizeof( WdeLBoxObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeLBoxCopyObject: Object malloc failed" );
        return( FALSE );
    }

    (*new)->dispatcher = obj->dispatcher;
    (*new)->object_id = obj->object_id;

    if( handle == NULL ) {
        (*new)->object_handle = *new;
    } else {
        (*new)->object_handle = handle;
    }

    if( !CopyObject( obj->control, &(*new)->control, (*new)->object_handle ) ) {
        WdeWriteTrail( "WdeLBoxCopyObject: Control not created!" );
        WdeMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeLBoxIdentify( WdeLBoxObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeLBoxGetWndProc( WdeLBoxObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeLBoxSuperClassProc;

    return( TRUE );
}

BOOL WdeLBoxGetWindowClass( WdeLBoxObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = "listbox";

    return( TRUE );
}

BOOL WdeLBoxDefine( WdeLBoxObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP |
                  WS_VSCROLL | WS_HSCROLL  | WS_BORDER;
    o_info.set_func = (WdeSetProc)WdeLBoxSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeLBoxGetDefineInfo;
    o_info.hook_func = WdeLBoxDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeLBoxSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000ffff;

    if( mask & LBS_NOTIFY ) {
        CheckDlgButton( hDlg, IDB_LBS_NOTIFY, 1 );
    }

    if( mask & LBS_SORT ) {
        CheckDlgButton( hDlg, IDB_LBS_SORT, 1 );
    }

    if( mask & LBS_NOREDRAW ) {
        CheckDlgButton( hDlg, IDB_LBS_NOREDRAW, 1 );
    }

    if( mask & LBS_MULTIPLESEL ) {
        CheckDlgButton( hDlg, IDB_LBS_MULTIPLESEL, 1 );
    }

    if( mask & LBS_USETABSTOPS ) {
        CheckDlgButton( hDlg, IDB_LBS_USETABSTOPS, 1 );
    }

    if( mask & LBS_NOINTEGRALHEIGHT ) {
        CheckDlgButton( hDlg, IDB_LBS_NOINTEGRALHEIGHT, 1 );
    }

    if( mask & LBS_MULTICOLUMN ) {
        CheckDlgButton( hDlg, IDB_LBS_MULTICOLUMN, 1 );
    }

    if( mask & LBS_WANTKEYBOARDINPUT ) {
        CheckDlgButton( hDlg, IDB_LBS_WANTKEYBOARDINPUT, 1 );
    }

    if( mask & LBS_EXTENDEDSEL ) {
        CheckDlgButton( hDlg, IDB_LBS_EXTENDEDSEL, 1 );
    }

    if( mask & LBS_DISABLENOSCROLL ) {
        CheckDlgButton( hDlg, IDB_LBS_DISABLENOSCROLL, 1 );
    }

    if( mask & LBS_STANDARD ) {
        CheckDlgButton( hDlg, IDB_LBS_STANDARD, 1 );
    }

    CheckDlgButton( hDlg, IDB_LBS_HASSTRINGS, 1 );
    EnableWindow( GetDlgItem( hDlg, IDB_LBS_HASSTRINGS ), FALSE );

    if( mask & LBS_OWNERDRAWFIXED ) {
        CheckDlgButton( hDlg, IDB_LBS_OWNERDRAWFIXED, 1 );
        EnableWindow( GetDlgItem( hDlg, IDB_LBS_HASSTRINGS ), TRUE );
        if( !(mask & LBS_HASSTRINGS) ) {
            CheckDlgButton( hDlg, IDB_LBS_HASSTRINGS, 0 );
        }
    }

    if( mask & LBS_OWNERDRAWVARIABLE ) {
        CheckDlgButton( hDlg, IDB_LBS_OWNERDRAWVARIABLE, 1 );
        EnableWindow ( GetDlgItem(hDlg, IDB_LBS_HASSTRINGS), TRUE );
        if ( !(mask & LBS_HASSTRINGS) ) {
            CheckDlgButton ( hDlg, IDB_LBS_HASSTRINGS, 0 );
        }
    }

#if __NT__XX
    EnableWindow( GetDlgItem( hDlg, IDB_LBS_NOSEL ), TRUE );
    if( mask & LBS_NOSEL ) {
        CheckDlgButton( hDlg, IDB_LBS_NOSEL, 1 );
    }
#else
    EnableWindow( GetDlgItem( hDlg, IDB_LBS_NOSEL ), FALSE );
#endif

#if __NT__XX
    // do the extended style stuff
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_STATICEDGE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_NOPARENTNOTIFY ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LEFT ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RIGHT ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LTRREADING ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RTLREADING ), TRUE );

    mask = GETCTL_EXSTYLE( o_info->info.c.info );
    if( mask & WS_EX_STATICEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_STATICEDGE, 1 );
    }
    if( mask & WS_EX_NOPARENTNOTIFY ) {
        CheckDlgButton( hDlg, IDB_WS_EX_NOPARENTNOTIFY, 1 );
    }
    if( mask & WS_EX_RIGHT ) {
        CheckDlgButton( hDlg, IDB_WS_EX_RIGHT, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_LEFT, 1 );
    }
    if( mask & WS_EX_RTLREADING ) {
        CheckDlgButton( hDlg, IDB_WS_EX_RTLREADING, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_LTRREADING, 1 );
    }
#else
    // disable the extended styles
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_STATICEDGE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_NOPARENTNOTIFY ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LEFT ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RIGHT ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LTRREADING ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RTLREADING ), FALSE );
#endif
}

void WdeLBoxGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = 0;

    if( IsDlgButtonChecked( hDlg, IDB_LBS_NOTIFY ) ) {
        mask |= LBS_NOTIFY;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_SORT ) ) {
        mask |= LBS_SORT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_NOREDRAW ) ) {
        mask |= LBS_NOREDRAW;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_MULTIPLESEL ) ) {
        mask |= LBS_MULTIPLESEL;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_HASSTRINGS ) ) {
        mask |= LBS_HASSTRINGS;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_USETABSTOPS ) ) {
        mask |= LBS_USETABSTOPS;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_NOINTEGRALHEIGHT ) ) {
        mask |= LBS_NOINTEGRALHEIGHT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_MULTICOLUMN ) ) {
        mask |= LBS_MULTICOLUMN;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_WANTKEYBOARDINPUT ) ) {
        mask |= LBS_WANTKEYBOARDINPUT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_EXTENDEDSEL ) ) {
        mask |= LBS_EXTENDEDSEL;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_DISABLENOSCROLL ) ) {
        mask |= LBS_DISABLENOSCROLL;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_OWNERDRAWFIXED ) ) {
        mask |= LBS_OWNERDRAWFIXED;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LBS_OWNERDRAWVARIABLE ) ) {
        mask |= LBS_OWNERDRAWVARIABLE;
    }

#if __NT__XX
    if( IsDlgButtonChecked( hDlg, IDB_LBS_NOSEL ) ) {
        mask |= LBS_NOSEL;
    }
#endif

    SETCTL_STYLE( o_info->info.c.info,
                  (GETCTL_STYLE( o_info->info.c.info ) & 0xffff0000) | mask );

#if __NT__XX
    // set the extended mask
    mask = 0;
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_STATICEDGE ) ) {
        mask |= WS_EX_STATICEDGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_NOPARENTNOTIFY ) ) {
        mask |= WS_EX_NOPARENTNOTIFY;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_RIGHT ) ) {
        mask |= WS_EX_RIGHT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_RTLREADING ) ) {
        mask |= WS_EX_RTLREADING;
    }
    SETCTL_EXSTYLE( o_info->info.c.info, mask );
#endif
}

BOOL WdeLBoxDefineHook ( HWND hDlg, UINT message,
                         WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    BOOL processed;

    /* touch unused vars to get rid of warning */
    _wde_touch( mask );
    _wde_touch( lParam );

    processed = FALSE;

    if( message == WM_COMMAND && GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
        switch( LOWORD( wParam ) ) {
        case IDB_LBS_NOTIFY:
        case IDB_LBS_SORT:
        case IDB_WS_VSCROLL:
        case IDB_WS_BORDER:
            if( IsDlgButtonChecked( hDlg, IDB_LBS_NOTIFY ) &&
                IsDlgButtonChecked( hDlg, IDB_LBS_SORT ) &&
                IsDlgButtonChecked( hDlg, IDB_WS_VSCROLL ) &&
                IsDlgButtonChecked( hDlg, IDB_WS_BORDER ) ) {
                CheckDlgButton( hDlg, IDB_LBS_STANDARD, 1 );
            } else {
                CheckDlgButton( hDlg, IDB_LBS_STANDARD, 0 );
            }
            processed = TRUE;
            break;

        case IDB_LBS_STANDARD:
            if( IsDlgButtonChecked( hDlg, IDB_LBS_STANDARD ) ) {
                CheckDlgButton( hDlg, IDB_LBS_NOTIFY, 1 );
                CheckDlgButton( hDlg, IDB_LBS_SORT, 1 );
                CheckDlgButton( hDlg, IDB_WS_VSCROLL, 1 );
                CheckDlgButton( hDlg, IDB_WS_BORDER, 1 );
            } else {
                CheckDlgButton( hDlg, IDB_LBS_NOTIFY, 0 );
                CheckDlgButton( hDlg, IDB_LBS_SORT, 0 );
                CheckDlgButton( hDlg, IDB_WS_VSCROLL, 0 );
                CheckDlgButton( hDlg, IDB_WS_BORDER, 0 );
            }
            processed = TRUE;
            break;

        case IDB_LBS_OWNERDRAWFIXED:
        case IDB_LBS_OWNERDRAWVARIABLE:
            if( IsDlgButtonChecked( hDlg, IDB_LBS_OWNERDRAWFIXED ) ||
                IsDlgButtonChecked( hDlg, IDB_LBS_OWNERDRAWVARIABLE ) ) {
                EnableWindow( GetDlgItem( hDlg, IDB_LBS_HASSTRINGS ), TRUE );
                CheckDlgButton( hDlg, IDB_LBS_OWNERDRAWFIXED, 0 );
                CheckDlgButton( hDlg, IDB_LBS_OWNERDRAWVARIABLE, 0 );
                CheckDlgButton( hDlg, LOWORD( wParam ), 1 );
            } else {
                CheckDlgButton( hDlg, IDB_LBS_HASSTRINGS, 1 );
                EnableWindow( GetDlgItem( hDlg, IDB_LBS_HASSTRINGS ), FALSE );
            }
            processed = TRUE;
            break;
        }
    }

    return( processed );
}

LRESULT WINEXPORT WdeLBoxSuperClassProc( HWND hWnd, UINT message, WPARAM wParam,
                                         volatile LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalLBoxProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

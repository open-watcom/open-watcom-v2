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
#include "wdeobjid.h"
#include "wdefutil.h"
#include "wde_wres.h"
#include "wdemain.h"
#include "wdeoinfo.h"
#include "wdedefsz.h"
#include "wdedebug.h"
#include "wde_rc.h"
#include "wdesdup.h"
#include "wdecctl.h"
#include "wdeftabc.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeTabCObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL    CALLBACK WdeTabCDispatcher( ACTION, WdeTabCObject *, void *, void * );
WINEXPORT LRESULT CALLBACK WdeTabCSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeTabC( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeTCCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeTabCDestroy( WdeTabCObject *, BOOL *, void * );
static BOOL     WdeTabCValidateAction( WdeTabCObject *, ACTION *, void * );
static BOOL     WdeTabCCopyObject( WdeTabCObject *, WdeTabCObject **, WdeTabCObject * );
static BOOL     WdeTabCIdentify( WdeTabCObject *, OBJ_ID *, void * );
static BOOL     WdeTabCGetWndProc( WdeTabCObject *, WNDPROC *, void * );
static BOOL     WdeTabCGetWindowClass( WdeTabCObject *, char **, void * );
static BOOL     WdeTabCDefine( WdeTabCObject *, POINT *, void * );
static void     WdeTabCSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeTabCGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeTabCDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeTabCDispatch;
static WdeDialogBoxControl      *WdeDefaultTabC = NULL;
static int                      WdeTabCWndExtra;
static WNDPROC                  WdeOriginalTabCProc;
//static WNDPROC                WdeTabCProc;

#define WWC_TABCONTROL   WC_TABCONTROL

static DISPATCH_ITEM WdeTabCActions[] = {
    { DESTROY,          (DISPATCH_RTN *)WdeTabCDestroy          },
    { COPY,             (DISPATCH_RTN *)WdeTabCCopyObject       },
    { VALIDATE_ACTION,  (DISPATCH_RTN *)WdeTabCValidateAction   },
    { IDENTIFY,         (DISPATCH_RTN *)WdeTabCIdentify         },
    { GET_WINDOW_CLASS, (DISPATCH_RTN *)WdeTabCGetWindowClass   },
    { DEFINE,           (DISPATCH_RTN *)WdeTabCDefine           },
    { GET_WND_PROC,     (DISPATCH_RTN *)WdeTabCGetWndProc       }
};

#define MAX_ACTIONS     (sizeof( WdeTabCActions ) / sizeof( DISPATCH_ITEM ))

WINEXPORT OBJPTR CALLBACK WdeTabCCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeTabC( parent, obj_rect, handle, 0, "", TABCNTL_OBJ ) );
    } else {
        return( WdeTCCreate( parent, obj_rect, NULL, TABCNTL_OBJ,
                             (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeTabC( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultTabC, style );
    SETCTL_TEXT( WdeDefaultTabC, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultTabC, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeTCCreate( parent, obj_rect, handle, id, WdeDefaultTabC );

    WRMemFree( GETCTL_TEXT( WdeDefaultTabC ) );
    SETCTL_TEXT( WdeDefaultTabC, NULL );

    return( new );
}

OBJPTR WdeTCCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeTabCObject *new;

    WdeDebugCreate( "TabC", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeTabCCreate: TabC has no parent!" );
        return( NULL );
    }

    new = (WdeTabCObject *)WRMemAlloc( sizeof( WdeTabCObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeTabCCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeTabCDispatch;
    new->object_id = id;
    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeTabCCreate: CONTROL_OBJ not created!" );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeTabCCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeTabCCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    return( new );
}

WINEXPORT BOOL CALLBACK WdeTabCDispatcher( ACTION act, WdeTabCObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "TabC", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeTabCActions[i].id == act ) {
            return( WdeTabCActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

bool WdeTabCInit( bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, WWC_TABCONTROL, &wc );
    WdeOriginalTabCProc = wc.lpfnWndProc;
    WdeTabCWndExtra = wc.cbWndExtra;

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
        wc.lpszClassName = "wdeedit";
        wc.cbWndExtra += sizeof( OBJPTR );
        //wc.lpfnWndProc = WdeTabCSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeTabCInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultTabC = WdeAllocDialogBoxControl();
    if( WdeDefaultTabC == NULL ) {
        WdeWriteTrail( "WdeTabCInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultTabC, WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
    SETCTL_ID( WdeDefaultTabC, 0 );
    SETCTL_EXTRABYTES( WdeDefaultTabC, 0 );
    SETCTL_SIZEX( WdeDefaultTabC, 0 );
    SETCTL_SIZEY( WdeDefaultTabC, 0 );
    SETCTL_SIZEW( WdeDefaultTabC, 0 );
    SETCTL_SIZEH( WdeDefaultTabC, 0 );
    SETCTL_TEXT( WdeDefaultTabC, NULL );
    SETCTL_CLASSID( WdeDefaultTabC, WdeStrToControlClass( WWC_TABCONTROL ) );

    WdeTabCDispatch = MakeProcInstance( (FARPROC)WdeTabCDispatcher, WdeGetAppInstance() );
    return( TRUE );
}

void WdeTabCFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultTabC );
    FreeProcInstance( WdeTabCDispatch );
}

BOOL WdeTabCDestroy( WdeTabCObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeTabCDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WRMemFree( obj );

    return( TRUE );
}

BOOL WdeTabCValidateAction( WdeTabCObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeTabCActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeTabCCopyObject( WdeTabCObject *obj, WdeTabCObject **new, WdeTabCObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeTabCCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeTabCObject *)WRMemAlloc( sizeof( WdeTabCObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeTabCCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeTabCCopyObject: Control not created!" );
        WRMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeTabCIdentify( WdeTabCObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeTabCGetWndProc( WdeTabCObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeTabCSuperClassProc;

    return( TRUE );
}

BOOL WdeTabCGetWindowClass( WdeTabCObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = WWC_TABCONTROL;

    return( TRUE );
}

BOOL WdeTabCDefine( WdeTabCObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP | WS_BORDER;
    o_info.set_func = (WdeSetProc)WdeTabCSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeTabCGetDefineInfo;
    o_info.hook_func = WdeTabCDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeTabCSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask;

    // set the tab control options
    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000ffff;
    if( mask & TCS_FORCEICONLEFT ) {
        CheckDlgButton( hDlg, IDB_TCS_FORCEICONLEFT, BST_CHECKED );
    }
    if( mask & TCS_FORCELABELLEFT ) {
        CheckDlgButton( hDlg, IDB_TCS_FORCELABELLEFT, BST_CHECKED );
        // insist on FORCEICONLEFT and FIXEDWIDTH and disable FORCEICONLEFT
        CheckDlgButton( hDlg, IDB_TCS_FORCEICONLEFT, BST_CHECKED );
        CheckDlgButton( hDlg, IDB_TCS_FIXEDWIDTH, BST_CHECKED );
        EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCEICONLEFT ), FALSE );
    }
    if( mask & TCS_BUTTONS ) {
        CheckDlgButton( hDlg, IDB_TCS_BUTTONS, BST_CHECKED );
    } else {
        CheckDlgButton( hDlg, IDB_TCS_TABS, BST_CHECKED );
    }
    if( mask & TCS_RAGGEDRIGHT ) {
        CheckDlgButton( hDlg, IDB_TCS_RAGGEDRIGHT, BST_CHECKED );
    } else {
        CheckDlgButton( hDlg, IDB_TCS_RIGHTJUSTIFY, BST_CHECKED );
    }
    if( mask & TCS_MULTILINE ) {
        CheckDlgButton( hDlg, IDB_TCS_MULTILINE, BST_CHECKED );
    } else {
        CheckDlgButton( hDlg, IDB_TCS_SINGLELINE, BST_CHECKED );
        // force raggedright on and disable both it and right justify
        CheckDlgButton( hDlg, IDB_TCS_RAGGEDRIGHT, BST_CHECKED );
        CheckDlgButton( hDlg, IDB_TCS_RIGHTJUSTIFY, BST_UNCHECKED );
        EnableWindow( GetDlgItem( hDlg, IDB_TCS_RAGGEDRIGHT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_TCS_RIGHTJUSTIFY ), FALSE );
    }
    if( mask & TCS_FIXEDWIDTH ) {
        CheckDlgButton( hDlg, IDB_TCS_FIXEDWIDTH, BST_CHECKED );
        // force raggedright on and disable both it and right justify
        CheckDlgButton( hDlg, IDB_TCS_RAGGEDRIGHT, BST_CHECKED );
        CheckDlgButton( hDlg, IDB_TCS_RIGHTJUSTIFY, BST_UNCHECKED );
        EnableWindow( GetDlgItem( hDlg, IDB_TCS_RAGGEDRIGHT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_TCS_RIGHTJUSTIFY ), FALSE );
    } else {
        // turn off FORCE ... LEFT controls and disable them
        CheckDlgButton( hDlg, IDB_TCS_FORCEICONLEFT, BST_UNCHECKED );
        CheckDlgButton( hDlg, IDB_TCS_FORCELABELLEFT, BST_UNCHECKED );
        EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCEICONLEFT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCELABELLEFT ), FALSE );
    }
    if( mask & TCS_FOCUSONBUTTONDOWN ) {
        CheckDlgButton( hDlg, IDB_TCS_FOCUSONBUTTONDOWN, BST_CHECKED );
    }
    if( mask & TCS_OWNERDRAWFIXED ) {
        CheckDlgButton( hDlg, IDB_TCS_OWNERDRAWFIXED, BST_CHECKED );
    }
    if( mask & TCS_TOOLTIPS ) {
        CheckDlgButton( hDlg, IDB_TCS_TOOLTIPS, BST_CHECKED );
    }
    if( mask & TCS_FOCUSNEVER ) {
        CheckDlgButton( hDlg, IDB_TCS_FOCUSNEVER, BST_CHECKED );
    }

    // set the extended style controls only
    WdeEXSetDefineInfo( o_info, hDlg );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}

void WdeTabCGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask = 0;

    // get the tab control settings
    if( IsDlgButtonChecked( hDlg, IDB_TCS_FORCEICONLEFT ) ) {
        mask |= TCS_FORCEICONLEFT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_FORCELABELLEFT ) ) {
        mask |= TCS_FORCELABELLEFT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_BUTTONS ) ) {
        mask |= TCS_BUTTONS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_MULTILINE ) ) {
        mask |= TCS_MULTILINE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_RAGGEDRIGHT ) ) {
        mask |= TCS_RAGGEDRIGHT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_FIXEDWIDTH ) ) {
        mask |= TCS_FIXEDWIDTH;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_FOCUSONBUTTONDOWN ) ) {
        mask |= TCS_FOCUSONBUTTONDOWN;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_OWNERDRAWFIXED ) ) {
        mask |= TCS_OWNERDRAWFIXED;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_TOOLTIPS ) ) {
        mask |= TCS_TOOLTIPS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TCS_FOCUSNEVER ) ) {
        mask |= TCS_FOCUSNEVER;
    }

    SETCTL_STYLE( o_info->info.c.info,
                  (GETCTL_STYLE( o_info->info.c.info ) & 0xffff0000) | mask );

    // get the extended control settings
    WdeEXGetDefineInfo( o_info, hDlg );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}

BOOL WdeTabCDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
#ifdef __NT__XX
    BOOL processed;
    WORD wp;

    /* touch unused vars to get rid of warning */
    _wde_touch( mask );
    _wde_touch( lParam );

    processed = FALSE;

    if( message == WM_COMMAND && GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
        wp = LOWORD( wParam );
        switch( wp ) {
        case IDB_TCS_FIXEDWIDTH:
            if( IsDlgButtonChecked( hDlg, IDB_TCS_FIXEDWIDTH ) ) {
                // force raggedright on and disable both controls
                CheckDlgButton( hDlg, IDB_TCS_RAGGEDRIGHT, BST_CHECKED );
                CheckDlgButton( hDlg, IDB_TCS_RIGHTJUSTIFY, BST_UNCHECKED );
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_RAGGEDRIGHT ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_RIGHTJUSTIFY ), FALSE );
                // enable the FORCE...LEFT controls
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCEICONLEFT ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCELABELLEFT ), TRUE );
            } else {
                // enable RAGGEDRIGHT and RIGHTJUSTIFY controls
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_RAGGEDRIGHT ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_RIGHTJUSTIFY ), TRUE );
                // disable the FORCE...LEFT controls
                CheckDlgButton( hDlg, IDB_TCS_FORCEICONLEFT, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_TCS_FORCELABELLEFT, BST_UNCHECKED );
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCEICONLEFT ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCELABELLEFT ), FALSE );
            }
            processed = TRUE;
            break;

       case IDB_TCS_FORCELABELLEFT:
            if( IsDlgButtonChecked( hDlg, IDB_TCS_FORCELABELLEFT ) ) {
                // turn on FORCEICONLEFT as well, and disable it
                CheckDlgButton( hDlg, IDB_TCS_FORCEICONLEFT, BST_CHECKED );
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCEICONLEFT ), FALSE );
            } else {
                // enable the FORCEICONLEFT control
                EnableWindow( GetDlgItem( hDlg, IDB_TCS_FORCEICONLEFT ), TRUE );
            }
            processed = TRUE;
            break;

       case IDB_TCS_MULTILINE:
       case IDB_TCS_SINGLELINE:
            if( !IsDlgButtonChecked( hDlg, IDB_TCS_FIXEDWIDTH ) ) {
                if( IsDlgButtonChecked( hDlg, IDB_TCS_MULTILINE ) ) {
                    // enable the RAGGEDRIGHT and RIGHTJUSTIFY control
                    EnableWindow( GetDlgItem( hDlg, IDB_TCS_RAGGEDRIGHT ), TRUE );
                    EnableWindow( GetDlgItem( hDlg, IDB_TCS_RIGHTJUSTIFY ), TRUE );
                } else {
                    // force RAGGEDRIGHT and disable it and RIGHTJUSTIFY
                    CheckDlgButton( hDlg, IDB_TCS_RAGGEDRIGHT, BST_CHECKED );
                    CheckDlgButton( hDlg, IDB_TCS_RIGHTJUSTIFY, BST_UNCHECKED );
                    EnableWindow( GetDlgItem( hDlg, IDB_TCS_RAGGEDRIGHT ), FALSE );
                    EnableWindow( GetDlgItem( hDlg, IDB_TCS_RIGHTJUSTIFY ), FALSE );
                }
            }
            processed = TRUE;
            break;
        }
    }

    return( processed );
#else
    _wde_touch( hDlg );
    _wde_touch( message );
    _wde_touch( wParam );
    _wde_touch( lParam );
    _wde_touch( mask );
    return( FALSE );
#endif
}

WINEXPORT LRESULT CALLBACK WdeTabCSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalTabCProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

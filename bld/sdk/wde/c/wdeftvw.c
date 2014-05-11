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
#include "wdeftvw.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeTViewObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL    CALLBACK WdeTViewDispatcher( ACTION, WdeTViewObject *, void *, void * );
WINEXPORT LRESULT CALLBACK WdeTViewSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeTView( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeTVCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeTViewDestroy( WdeTViewObject *, BOOL *, void * );
static BOOL     WdeTViewValidateAction( WdeTViewObject *, ACTION *, void * );
static BOOL     WdeTViewCopyObject( WdeTViewObject *, WdeTViewObject **, WdeTViewObject * );
static BOOL     WdeTViewIdentify( WdeTViewObject *, OBJ_ID *, void * );
static BOOL     WdeTViewGetWndProc( WdeTViewObject *, WNDPROC *, void * );
static BOOL     WdeTViewGetWindowClass( WdeTViewObject *, char **, void * );
static BOOL     WdeTViewDefine( WdeTViewObject *, POINT *, void * );
static void     WdeTViewSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeTViewGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeTViewDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeTViewDispatch;
static WdeDialogBoxControl      *WdeDefaultTView = NULL;
static int                      WdeTViewWndExtra;
static WNDPROC                  WdeOriginalTViewProc;
//static WNDPROC                WdeTViewProc;

#define WWC_TREEVIEW     WC_TREEVIEW

static DISPATCH_ITEM WdeTViewActions[] = {
    { DESTROY,          (DISPATCH_RTN *)WdeTViewDestroy         },
    { COPY,             (DISPATCH_RTN *)WdeTViewCopyObject      },
    { VALIDATE_ACTION,  (DISPATCH_RTN *)WdeTViewValidateAction  },
    { IDENTIFY,         (DISPATCH_RTN *)WdeTViewIdentify        },
    { GET_WINDOW_CLASS, (DISPATCH_RTN *)WdeTViewGetWindowClass  },
    { DEFINE,           (DISPATCH_RTN *)WdeTViewDefine          },
    { GET_WND_PROC,     (DISPATCH_RTN *)WdeTViewGetWndProc      }
};

#define MAX_ACTIONS     (sizeof( WdeTViewActions ) / sizeof( DISPATCH_ITEM ))

WINEXPORT OBJPTR CALLBACK WdeTViewCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeTView( parent, obj_rect, handle, 0, "", TVIEW_OBJ ) );
    } else {
        return( WdeTVCreate( parent, obj_rect, NULL, TVIEW_OBJ,
                             (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeTView( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                     DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultTView, style );
    SETCTL_TEXT( WdeDefaultTView, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultTView, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeTVCreate( parent, obj_rect, handle, id, WdeDefaultTView );

    WRMemFree( GETCTL_TEXT( WdeDefaultTView ) );
    SETCTL_TEXT( WdeDefaultTView, NULL );

    return( new );
}

OBJPTR WdeTVCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeTViewObject *new;

    WdeDebugCreate( "TView", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeTViewCreate: TView has no parent!" );
        return( NULL );
    }

    new = (WdeTViewObject *)WRMemAlloc( sizeof( WdeTViewObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeTViewCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeTViewDispatch;
    new->object_id = id;
    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeTViewCreate: CONTROL_OBJ not created!" );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeTViewCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeTViewCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    return( new );
}

WINEXPORT BOOL CALLBACK WdeTViewDispatcher( ACTION act, WdeTViewObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "TView", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeTViewActions[i].id == act ) {
            return( WdeTViewActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeTViewInit( Bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, WWC_TREEVIEW, &wc );
    WdeOriginalTViewProc = wc.lpfnWndProc;
    WdeTViewWndExtra = wc.cbWndExtra;

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
        //wc.lpfnWndProc = WdeTViewSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeTViewInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultTView = WdeAllocDialogBoxControl();
    if( WdeDefaultTView == NULL ) {
        WdeWriteTrail( "WdeTViewInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultTView, WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
    SETCTL_ID( WdeDefaultTView, 0 );
    SETCTL_EXTRABYTES( WdeDefaultTView, 0 );
    SETCTL_SIZEX( WdeDefaultTView, 0 );
    SETCTL_SIZEY( WdeDefaultTView, 0 );
    SETCTL_SIZEW( WdeDefaultTView, 0 );
    SETCTL_SIZEH( WdeDefaultTView, 0 );
    SETCTL_TEXT( WdeDefaultTView, NULL );
    SETCTL_CLASSID( WdeDefaultTView, WdeStrToControlClass( WWC_TREEVIEW ) );

    WdeTViewDispatch = MakeProcInstance( (FARPROC)WdeTViewDispatcher,
                                         WdeGetAppInstance() );
    return( TRUE );
}

void WdeTViewFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultTView );
    FreeProcInstance( WdeTViewDispatch );
}

BOOL WdeTViewDestroy( WdeTViewObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeTViewDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WRMemFree( obj );

    return( TRUE );
}

BOOL WdeTViewValidateAction( WdeTViewObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeTViewActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeTViewCopyObject( WdeTViewObject *obj, WdeTViewObject **new, WdeTViewObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeTViewCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeTViewObject *)WRMemAlloc( sizeof( WdeTViewObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeTViewCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeTViewCopyObject: Control not created!" );
        WRMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeTViewIdentify( WdeTViewObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeTViewGetWndProc( WdeTViewObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeTViewSuperClassProc;

    return( TRUE );
}

BOOL WdeTViewGetWindowClass( WdeTViewObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = WWC_TREEVIEW;

    return( TRUE );
}

BOOL WdeTViewDefine( WdeTViewObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP | WS_BORDER;
    o_info.set_func = (WdeSetProc)WdeTViewSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeTViewGetDefineInfo;
    o_info.hook_func = WdeTViewDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeTViewSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask;

    // set the tree view options
    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x000000ff;
    if( mask & TVS_HASBUTTONS ) {
        CheckDlgButton( hDlg, IDB_TVS_HASBUTTONS, 1 );
    }
    if( mask & TVS_HASLINES ) {
        CheckDlgButton( hDlg, IDB_TVS_HASLINES, 1 );
    }
    if( mask & TVS_LINESATROOT ) {
        CheckDlgButton( hDlg, IDB_TVS_LINESATROOT, 1 );
    }
    if( mask & TVS_EDITLABELS ) {
        CheckDlgButton( hDlg, IDB_TVS_EDITLABELS, 1 );
    }
    if( mask & TVS_DISABLEDRAGDROP ) {
        CheckDlgButton( hDlg, IDB_TVS_DISABLEDRAGDROP, 1 );
    }
    if( mask & TVS_SHOWSELALWAYS ) {
        CheckDlgButton( hDlg, IDB_TVS_SHOWSELALWAYS, 1 );
    }

    // set the extended style controls only
    WdeEXSetDefineInfo( o_info, hDlg );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}

void WdeTViewGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask = 0;

    // get the tree view control settings
    if( IsDlgButtonChecked( hDlg, IDB_TVS_HASBUTTONS ) ) {
        mask |= TVS_HASBUTTONS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TVS_HASLINES ) ) {
        mask |= TVS_HASLINES;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TVS_LINESATROOT ) ) {
        mask |= TVS_LINESATROOT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TVS_EDITLABELS ) ) {
        mask |= TVS_EDITLABELS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TVS_DISABLEDRAGDROP ) ) {
        mask |= TVS_DISABLEDRAGDROP;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TVS_SHOWSELALWAYS ) ) {
        mask |= TVS_SHOWSELALWAYS;
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

BOOL WdeTViewDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    BOOL processed;

    /* touch unused vars to get rid of warning */
    _wde_touch( hDlg );
    _wde_touch( message );
    _wde_touch( wParam );
    _wde_touch( lParam );
    _wde_touch( mask );

    processed = FALSE;

    return( processed );
}

WINEXPORT LRESULT CALLBACK WdeTViewSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalTViewProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

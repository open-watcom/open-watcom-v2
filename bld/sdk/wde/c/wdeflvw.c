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
#include "wdemem.h"
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
#include "wdeflvw.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeLViewObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL    CALLBACK WdeLViewDispatcher( ACTION, WdeLViewObject *, void *, void * );
WINEXPORT LRESULT CALLBACK WdeLViewSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeLView( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeLVCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeLViewDestroy( WdeLViewObject *, BOOL *, void * );
static BOOL     WdeLViewValidateAction( WdeLViewObject *, ACTION *, void * );
static BOOL     WdeLViewCopyObject( WdeLViewObject *, WdeLViewObject **, WdeLViewObject * );
static BOOL     WdeLViewIdentify( WdeLViewObject *, OBJ_ID *, void * );
static BOOL     WdeLViewGetWndProc( WdeLViewObject *, WNDPROC *, void * );
static BOOL     WdeLViewGetWindowClass( WdeLViewObject *, char **, void * );
static BOOL     WdeLViewDefine( WdeLViewObject *, POINT *, void * );
static void     WdeLViewSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeLViewGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeLViewDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeLViewDispatch;
static WdeDialogBoxControl      *WdeDefaultLView = NULL;
static int                      WdeLViewWndExtra;
static WNDPROC                  WdeOriginalLViewProc;
//static WNDPROC                WdeLViewProc;

#define WWC_LISTVIEW     WC_LISTVIEW

static DISPATCH_ITEM WdeLViewActions[] = {
    { DESTROY,          (DISPATCH_RTN *)WdeLViewDestroy         },
    { COPY,             (DISPATCH_RTN *)WdeLViewCopyObject      },
    { VALIDATE_ACTION,  (DISPATCH_RTN *)WdeLViewValidateAction  },
    { IDENTIFY,         (DISPATCH_RTN *)WdeLViewIdentify        },
    { GET_WINDOW_CLASS, (DISPATCH_RTN *)WdeLViewGetWindowClass  },
    { DEFINE,           (DISPATCH_RTN *)WdeLViewDefine          },
    { GET_WND_PROC,     (DISPATCH_RTN *)WdeLViewGetWndProc      }
};

#define MAX_ACTIONS      (sizeof( WdeLViewActions ) / sizeof( DISPATCH_ITEM ))

WINEXPORT OBJPTR CALLBACK WdeLViewCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeLView( parent, obj_rect, handle, 0, "", LVIEW_OBJ ) );
    } else {
        return( WdeLVCreate( parent, obj_rect, NULL, LVIEW_OBJ,
                             (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeLView( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                     DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultLView, style );
    SETCTL_TEXT( WdeDefaultLView, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultLView, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeLVCreate( parent, obj_rect, handle, id, WdeDefaultLView );

    WdeMemFree( GETCTL_TEXT( WdeDefaultLView ) );
    SETCTL_TEXT( WdeDefaultLView, NULL );

    return( new );
}

OBJPTR WdeLVCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeLViewObject *new;

    WdeDebugCreate( "LView", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeLViewCreate: LView has no parent!" );
        return( NULL );
    }

    new = (WdeLViewObject *)WdeMemAlloc( sizeof( WdeLViewObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeLViewCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeLViewDispatch;

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeLViewCreate: CONTROL_OBJ not created!" );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeLViewCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeLViewCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    return( new );
}

WINEXPORT BOOL CALLBACK WdeLViewDispatcher( ACTION act, WdeLViewObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "LView", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeLViewActions[i].id == act ) {
            return( WdeLViewActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeLViewInit( Bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, WWC_LISTVIEW, &wc );
    WdeOriginalLViewProc = wc.lpfnWndProc;
    WdeLViewWndExtra = wc.cbWndExtra;

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
        //wc.lpfnWndProc = WdeLViewSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeLViewInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultLView = WdeAllocDialogBoxControl();
    if( WdeDefaultLView == NULL ) {
        WdeWriteTrail( "WdeLViewInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultLView, WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
    SETCTL_ID( WdeDefaultLView, 0 );
    SETCTL_EXTRABYTES( WdeDefaultLView, 0 );
    SETCTL_SIZEX( WdeDefaultLView, 0 );
    SETCTL_SIZEY( WdeDefaultLView, 0 );
    SETCTL_SIZEW( WdeDefaultLView, 0 );
    SETCTL_SIZEH( WdeDefaultLView, 0 );
    SETCTL_TEXT( WdeDefaultLView, NULL );
    SETCTL_CLASSID( WdeDefaultLView, WdeStrToControlClass( WWC_LISTVIEW ) );

    WdeLViewDispatch = MakeProcInstance( (FARPROC)WdeLViewDispatcher, WdeGetAppInstance() );
    return( TRUE );
}

void WdeLViewFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultLView );
    FreeProcInstance( WdeLViewDispatch );
}

BOOL WdeLViewDestroy( WdeLViewObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeLViewDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WdeMemFree( obj );

    return( TRUE );
}

BOOL WdeLViewValidateAction( WdeLViewObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeLViewActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeLViewCopyObject( WdeLViewObject *obj, WdeLViewObject **new, WdeLViewObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeLViewCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeLViewObject *)WdeMemAlloc( sizeof( WdeLViewObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeLViewCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeLViewCopyObject: Control not created!" );
        WdeMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeLViewIdentify( WdeLViewObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeLViewGetWndProc( WdeLViewObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeLViewSuperClassProc;

    return( TRUE );
}

BOOL WdeLViewGetWindowClass( WdeLViewObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = WWC_LISTVIEW;

    return( TRUE );
}

BOOL WdeLViewDefine( WdeLViewObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP | WS_BORDER;
    o_info.set_func = (WdeSetProc)WdeLViewSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeLViewGetDefineInfo;
    o_info.hook_func = WdeLViewDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeLViewSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask;

    // set the list view options
    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x00000003;
    if( mask == LVS_REPORT ) {
        CheckDlgButton( hDlg, IDB_LVS_REPORT, 1 );
    } else if( mask == LVS_SMALLICON ) {
        CheckDlgButton( hDlg, IDB_LVS_SMALLICON, 1 );
    } else if( mask == LVS_LIST ) {
        CheckDlgButton( hDlg, IDB_LVS_LIST, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_LVS_ICON, 1 );
    }

    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000ffff;

    if( mask & LVS_SINGLESEL ) {
        CheckDlgButton( hDlg, IDB_LVS_SINGLESEL, 1 );
    }
    if( mask & LVS_SHOWSELALWAYS ) {
        CheckDlgButton( hDlg, IDB_LVS_SHOWSELALWAYS, 1 );
    }
    if( mask & LVS_SHAREIMAGELISTS ) {
        CheckDlgButton( hDlg, IDB_LVS_SHAREIMAGELISTS, 1 );
    }
    if( mask & LVS_NOLABELWRAP ) {
        CheckDlgButton( hDlg, IDB_LVS_NOLABELWRAP, 1 );
    }
    if( mask & LVS_AUTOARRANGE ) {
        CheckDlgButton( hDlg, IDB_LVS_AUTOARRANGE, 1 );
    }
    if( mask & LVS_EDITLABELS ) {
        CheckDlgButton( hDlg, IDB_LVS_EDITLABELS, 1 );
    }
    if( mask & LVS_ALIGNLEFT ) {
        CheckDlgButton( hDlg, IDB_LVS_ALIGNLEFT, 1 );
    }
    if( mask & LVS_OWNERDRAWFIXED ) {
        CheckDlgButton( hDlg, IDB_LVS_OWNERDRAWFIXED, 1 );
    }
    if( mask & LVS_NOSCROLL ) {
        CheckDlgButton( hDlg, IDB_LVS_NOSCROLL, 1 );
    }
    if( mask & LVS_NOCOLUMNHEADER ) {
        CheckDlgButton( hDlg, IDB_LVS_NOCOLUMNHEADER, 1 );
    }
    if( mask & LVS_NOSORTHEADER ) {
        CheckDlgButton( hDlg, IDB_LVS_NOSORTHEADER, 1 );
    }

    if( mask & LVS_SORTDESCENDING ) {
        CheckDlgButton( hDlg, IDB_LVS_SORTDESCENDING, 1 );
    } else if( mask & LVS_SORTASCENDING ) {
        CheckDlgButton( hDlg, IDB_LVS_SORTASCENDING, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_LVS_NOSORTING, 1 );
    }

    // set the extended style controls only
    WdeEXSetDefineInfo( o_info, hDlg );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}

void WdeLViewGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask;

    // get the list view settings
    if( IsDlgButtonChecked( hDlg, IDB_LVS_ICON ) ) {
        mask = LVS_ICON;
    } else if( IsDlgButtonChecked( hDlg, IDB_LVS_REPORT ) ) {
        mask = LVS_REPORT;
    } else if( IsDlgButtonChecked( hDlg, IDB_LVS_SMALLICON ) ) {
        mask = LVS_SMALLICON;
    } else {
        mask = LVS_LIST;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LVS_SINGLESEL ) ) {
        mask |= LVS_SINGLESEL;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_SHOWSELALWAYS ) ) {
        mask |= LVS_SHOWSELALWAYS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_SORTASCENDING ) ) {
        mask |= LVS_SORTASCENDING;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_SORTDESCENDING ) ) {
        mask |= LVS_SORTDESCENDING;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_SHAREIMAGELISTS ) ) {
        mask |= LVS_SHAREIMAGELISTS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_NOLABELWRAP ) ) {
        mask |= LVS_NOLABELWRAP;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_AUTOARRANGE ) ) {
        mask |= LVS_AUTOARRANGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_EDITLABELS ) ) {
        mask |= LVS_EDITLABELS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_NOSCROLL ) ) {
        mask |= LVS_NOSCROLL;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_ALIGNTOP ) ) {
        mask |= LVS_ALIGNTOP;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_ALIGNLEFT ) ) {
        mask |= LVS_ALIGNLEFT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_OWNERDRAWFIXED ) ) {
        mask |= LVS_OWNERDRAWFIXED;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_NOCOLUMNHEADER ) ) {
        mask |= LVS_NOCOLUMNHEADER;
    }
    if( IsDlgButtonChecked( hDlg, IDB_LVS_NOSORTHEADER ) ) {
        mask |= LVS_NOSORTHEADER;
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

BOOL WdeLViewDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
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

WINEXPORT LRESULT CALLBACK WdeLViewSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalLViewProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

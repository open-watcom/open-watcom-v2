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
#include "wderes.h"
#include "wde_rc.h"
#include "wdesdup.h"
#include "wdecctl.h"
#include "wdefcntl.h"
#include "wdefsbar.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeSBarObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL    CALLBACK WdeSBarDispatcher( ACTION, WdeSBarObject *, void *, void * );
WINEXPORT LRESULT CALLBACK WdeSBarSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeSBar( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeSBCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeSBarDestroy( WdeSBarObject *, BOOL *, void * );
static BOOL     WdeSBarValidateAction( WdeSBarObject *, ACTION *, void * );
static BOOL     WdeSBarCopyObject( WdeSBarObject *, WdeSBarObject **, WdeSBarObject * );
static BOOL     WdeSBarIdentify( WdeSBarObject *, OBJ_ID *, void * );
static BOOL     WdeSBarGetWndProc( WdeSBarObject *, WNDPROC *, void * );
static BOOL     WdeSBarGetWindowClass( WdeSBarObject *, char **, void * );
static BOOL     WdeSBarDefine( WdeSBarObject *, POINT *, void * );
static void     WdeSBarSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeSBarGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeSBarDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeSBarDispatch;
static WdeDialogBoxControl      *WdeDefaultSBar = NULL;
static int                      WdeSBarWndExtra;
static WNDPROC                  WdeOriginalSBarProc;
//static WNDPROC                WdeSBarProc;

#define WSTATUSCLASSNAME        STATUSCLASSNAME

static DISPATCH_ITEM WdeSBarActions[] = {
    { DESTROY,          (DISPATCH_RTN *)WdeSBarDestroy          },
    { COPY,             (DISPATCH_RTN *)WdeSBarCopyObject       },
    { VALIDATE_ACTION,  (DISPATCH_RTN *)WdeSBarValidateAction   },
    { IDENTIFY,         (DISPATCH_RTN *)WdeSBarIdentify         },
    { GET_WINDOW_CLASS, (DISPATCH_RTN *)WdeSBarGetWindowClass   },
    { DEFINE,           (DISPATCH_RTN *)WdeSBarDefine           },
    { GET_WND_PROC,     (DISPATCH_RTN *)WdeSBarGetWndProc       }
};

#define MAX_ACTIONS     (sizeof( WdeSBarActions ) / sizeof( DISPATCH_ITEM ))

Bool WdeSBNoodleSize( OBJPTR obj, Bool recreate )
{
    HWND                hWnd;
    WdeSBarObject       *sb_obj;

    if( obj == NULL ) {
        return( FALSE );
    }

    sb_obj = (WdeSBarObject *)obj;

    if( recreate ) {
        Forward( obj, DESTROY_WINDOW, FALSE, NULL );
        Forward( obj, CREATE_WINDOW, FALSE, NULL );
    }

    if( Forward( (OBJPTR)sb_obj->object_handle, GET_WINDOW_HANDLE, &hWnd, NULL ) ) {
        WdeResInfo      *rinfo;
        rinfo = WdeGetCurrentRes();
        if( rinfo != NULL ) {
            RECT                rect;
            GetWindowRect( hWnd, &rect );
            MapWindowPoints( (HWND)NULL, rinfo->forms_win, (POINT *)&rect, 2 );
            HideSelectBoxes();
            Resize( sb_obj->control, &rect, FALSE );
            WdeUpdateCDialogUnits( sb_obj->control, &rect, NULL );
            ShowSelectBoxes();
        }
    }

    return( TRUE );
}

WINEXPORT OBJPTR CALLBACK WdeSBarCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeSBar( parent, obj_rect, handle, 0, "", SBAR_OBJ ) );
    } else {
        return( WdeSBCreate( parent, obj_rect, NULL, SBAR_OBJ,
                             (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeSBar( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultSBar, style );
    SETCTL_TEXT( WdeDefaultSBar, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultSBar, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeSBCreate( parent, obj_rect, handle, id, WdeDefaultSBar );

    WRMemFree( GETCTL_TEXT( WdeDefaultSBar ) );
    SETCTL_TEXT( WdeDefaultSBar, NULL );

    return( new );
}

OBJPTR WdeSBCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeSBarObject       *new;

    WdeDebugCreate( "SBar", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeSBarCreate: SBar has no parent!" );
        return( NULL );
    }

    new = (WdeSBarObject *)WRMemAlloc( sizeof( WdeSBarObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeSBarCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeSBarDispatch;

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeSBarCreate: CONTROL_OBJ not created!" );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeSBarCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeSBarCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    WdeSBNoodleSize( new, FALSE );

    return( new );
}

BOOL CALLBACK WdeSBarDispatcher( ACTION act, WdeSBarObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "SBar", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeSBarActions[i].id == act ) {
            return( WdeSBarActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeSBarInit( Bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, WSTATUSCLASSNAME, &wc );
    WdeOriginalSBarProc = wc.lpfnWndProc;
    WdeSBarWndExtra = wc.cbWndExtra;

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
        //wc.lpfnWndProc = WdeSBarSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeSBarInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultSBar = WdeAllocDialogBoxControl();
    if( WdeDefaultSBar == NULL ) {
        WdeWriteTrail( "WdeSBarInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultSBar, WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
    SETCTL_ID( WdeDefaultSBar, 0 );
    SETCTL_EXTRABYTES( WdeDefaultSBar, 0 );
    SETCTL_SIZEX( WdeDefaultSBar, 0 );
    SETCTL_SIZEY( WdeDefaultSBar, 0 );
    SETCTL_SIZEW( WdeDefaultSBar, 0 );
    SETCTL_SIZEH( WdeDefaultSBar, 0 );
    SETCTL_TEXT( WdeDefaultSBar, NULL );
    SETCTL_CLASSID( WdeDefaultSBar, WdeStrToControlClass( WSTATUSCLASSNAME ) );

    WdeSBarDispatch = MakeProcInstance( (FARPROC)WdeSBarDispatcher, WdeGetAppInstance() );
    return( TRUE );
}

void WdeSBarFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultSBar );
    FreeProcInstance( WdeSBarDispatch );
}

BOOL WdeSBarDestroy( WdeSBarObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeSBarDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WRMemFree( obj );

    return( TRUE );
}

BOOL WdeSBarValidateAction( WdeSBarObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( *act == MOVE || *act == RESIZE ) {
        OBJPTR  parent;
        OBJ_ID  id;
        GetObjectParent( (OBJPTR)obj, &parent );
        Forward( (OBJPTR)obj, IDENTIFY, &id, NULL );
        if( id == DIALOG_OBJ ) {
            return( FALSE );
        }
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeSBarActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR) obj->control, *act, p2 ) );
}

BOOL WdeSBarCopyObject( WdeSBarObject *obj, WdeSBarObject **new, WdeSBarObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeSBarCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeSBarObject *)WRMemAlloc( sizeof( WdeSBarObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeSBarCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeSBarCopyObject: Control not created!" );
        WRMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeSBarIdentify( WdeSBarObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeSBarGetWndProc( WdeSBarObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeSBarSuperClassProc;

    return( TRUE );
}

BOOL WdeSBarGetWindowClass( WdeSBarObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = WSTATUSCLASSNAME;

    return( TRUE );
}

BOOL WdeSBarDefine( WdeSBarObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP | WS_BORDER;
    o_info.set_func = (WdeSetProc)WdeSBarSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeSBarGetDefineInfo;
    o_info.hook_func = WdeSBarDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeSBarSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask;

    // set the status bar options
    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000ff00;
    if( mask & SBT_OWNERDRAW ) {
        CheckDlgButton( hDlg, IDB_SBT_OWNERDRAW, 1 );
    }
    if( mask & SBT_NOBORDERS ) {
        CheckDlgButton( hDlg, IDB_SBT_NOBORDERS, 1 );
    }
    if( mask & SBT_POPOUT ) {
        CheckDlgButton( hDlg, IDB_SBT_POPOUT, 1 );
    }

    // set the extended style controls only
    WdeEXSetDefineInfo( o_info, hDlg );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}

void WdeSBarGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask = 0;

    // get the status bar settings
    if( IsDlgButtonChecked( hDlg, IDB_SBT_OWNERDRAW ) ) {
        mask |= SBT_OWNERDRAW;
    }
    if( IsDlgButtonChecked( hDlg, IDB_SBT_NOBORDERS ) ) {
        mask |= SBT_NOBORDERS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_SBT_POPOUT ) ) {
        mask |= SBT_POPOUT;
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

BOOL WdeSBarDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
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

LRESULT CALLBACK WdeSBarSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalSBarProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

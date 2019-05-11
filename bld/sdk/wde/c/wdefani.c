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
#include "wde.rh"
#include "wdesdup.h"
#include "wdecctl.h"
#include "wdefani.h"
#include "wdedispa.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

#define pick_ACTS(o) \
    pick_ACTION_DESTROY(o,pick) \
    pick_ACTION_COPY(o,pick) \
    pick_ACTION_VALIDATE_ACTION(o,pick) \
    pick_ACTION_IDENTIFY(o,pick) \
    pick_ACTION_GET_WINDOW_CLASS(o,pick) \
    pick_ACTION_DEFINE(o,pick) \
    pick_ACTION_GET_WND_PROC(o,pick)

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    DISPATCH_FN *dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeAniCObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT bool    CALLBACK WdeAniCDispatcher( ACTION_ID, OBJPTR, void *, void * );
WINEXPORT LRESULT CALLBACK WdeAniCSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeAniC( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeAniCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static void     WdeAniCSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeAniCGetDefineInfo( WdeDefineObjectInfo *, HWND );
static bool     WdeAniCDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

#define pick(e,n,c)     static bool WdeAniC ## n ## c;
    pick_ACTS( WdeAniCObject )
#undef pick

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static DISPATCH_FN              *WdeAniCDispatch;
static WdeDialogBoxControl      *WdeDefaultAniC = NULL;
static int                      WdeAniCWndExtra;
static WNDPROC                  WdeOriginalAniCProc;

#define WANIMATE_CLASS   ANIMATE_CLASS

static DISPATCH_ITEM WdeAniCActions[] = {
#define pick(e,n,c)     {e, (DISPATCH_RTN *)WdeAniC ## n},
    pick_ACTS( WdeAniCObject )
#undef pick
};

#define MAX_ACTIONS      (sizeof( WdeAniCActions ) / sizeof( DISPATCH_ITEM ))

OBJPTR CALLBACK WdeAniCCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeAniC( parent, obj_rect, handle, 0, "", ANIMATE_OBJ ) );
    } else {
        return( WdeAniCreate( parent, obj_rect, NULL, ANIMATE_OBJ, (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeAniC( OBJPTR parent, RECT *obj_rect, OBJPTR handle, DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultAniC, style );
    SETCTL_TEXT( WdeDefaultAniC, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultAniC, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeAniCreate( parent, obj_rect, handle, id, WdeDefaultAniC );

    WRMemFree( GETCTL_TEXT( WdeDefaultAniC ) );
    SETCTL_TEXT( WdeDefaultAniC, NULL );

    return( new );
}

OBJPTR WdeAniCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle, OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeAniCObject *new;

    WdeDebugCreate( "AniC", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeAniCCreate: AniC has no parent!" );
        return( NULL );
    }

    new = (WdeAniCObject *)WRMemAlloc( sizeof( WdeAniCObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeAniCCreate: Object malloc failed" );
        return( NULL );
    }

    OBJ_DISPATCHER_SET( new, WdeAniCDispatch );

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = (OBJPTR)new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeAniCCreate: CONTROL_OBJ not created!" );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeAniCCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, false );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeAniCCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, false );
        WRMemFree( new );
        return( NULL );
    }

    return( (OBJPTR)new );
}

bool CALLBACK WdeAniCDispatcher( ACTION_ID act, OBJPTR obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "AniC", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeAniCActions[i].id == act ) {
            return( WdeAniCActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( ((WdeAniCObject *)obj)->control, act, p1, p2 ) );
}

bool WdeAniCInit( bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, WANIMATE_CLASS, &wc );
    WdeOriginalAniCProc = wc.lpfnWndProc;
    WdeAniCWndExtra = wc.cbWndExtra;

    if( first ) {
#if 0
        if( wc.style & CS_GLOBALCLASS ) {
            wc.style ^= CS_GLOBALCLASS;
        }
        if( wc.style & CS_PARENTDC ) {
            wc.style ^= CS_PARENTDC;
        }
        wc.style |= (CS_HREDRAW | CS_VREDRAW);
        wc.hInstance = WdeApplicationInstance;
        wc.lpszClassName = "wdeedit";
        wc.cbWndExtra += sizeof( OBJPTR );
        //wc.lpfnWndProc = WdeAniCSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeAniCInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultAniC = WdeAllocDialogBoxControl();
    if( WdeDefaultAniC == NULL ) {
        WdeWriteTrail( "WdeAniCInit: Alloc of control failed!" );
        return( false );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultAniC, WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
    SETCTL_ID( WdeDefaultAniC, 0 );
    SETCTL_EXTRABYTES( WdeDefaultAniC, 0 );
    SETCTL_SIZEX( WdeDefaultAniC, 0 );
    SETCTL_SIZEY( WdeDefaultAniC, 0 );
    SETCTL_SIZEW( WdeDefaultAniC, 0 );
    SETCTL_SIZEH( WdeDefaultAniC, 0 );
    SETCTL_TEXT( WdeDefaultAniC, NULL );
    SETCTL_CLASSID( WdeDefaultAniC, WdeStrToControlClass( WANIMATE_CLASS ) );

    WdeAniCDispatch = MakeProcInstance_DISPATCHER( WdeAniCDispatcher, WdeGetAppInstance() );
    return( true );
}

void WdeAniCFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultAniC );
    FreeProcInstance_DISPATCHER( WdeAniCDispatch );
}

bool WdeAniCDestroy( WdeAniCObject *obj, bool *flag, bool *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeAniCDestroy: Control DESTROY failed" );
        return( false );
    }

    WRMemFree( obj );

    return( true );
}

bool WdeAniCValidateAction( WdeAniCObject *obj, ACTION_ID *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeAniCActions[i].id == *act ) {
            return( true );
        }
    }

    return( ValidateAction( obj->control, *act, p2 ) );
}

bool WdeAniCCopyObject( WdeAniCObject *obj, WdeAniCObject **new, OBJPTR handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeAniCCopyObject: Invalid new object!" );
        return( false );
    }

    *new = (WdeAniCObject *)WRMemAlloc( sizeof( WdeAniCObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeAniCCopyObject: Object malloc failed" );
        return( false );
    }

    OBJ_DISPATCHER_COPY( *new, obj );
    (*new)->object_id = obj->object_id;

    if( handle == NULL ) {
        (*new)->object_handle = (OBJPTR)*new;
    } else {
        (*new)->object_handle = handle;
    }

    if( !CopyObject( obj->control, &(*new)->control, (*new)->object_handle ) ) {
        WdeWriteTrail( "WdeAniCCopyObject: Control not created!" );
        WRMemFree( *new );
        return( false );
    }

    return( true );
}

bool WdeAniCIdentify( WdeAniCObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( true );
}

bool WdeAniCGetWndProc( WdeAniCObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeAniCSuperClassProc;

    return( true );
}

bool WdeAniCGetWindowClass( WdeAniCObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = WANIMATE_CLASS;

    return( true );
}

bool WdeAniCDefine( WdeAniCObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP | WS_BORDER;
    o_info.set_func = (WdeSetProc)WdeAniCSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeAniCGetDefineInfo;
    o_info.hook_func = WdeAniCDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeAniCSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask;

    // set the animate control options
    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000000f;
    if( mask & ACS_CENTER ) {
        CheckDlgButton( hDlg, IDB_ACS_CENTER, BST_CHECKED );
    }
    if( mask & ACS_TRANSPARENT ) {
        CheckDlgButton( hDlg, IDB_ACS_TRANSPARENT, BST_CHECKED );
    }
    if( mask & ACS_AUTOPLAY ) {
        CheckDlgButton( hDlg, IDB_ACS_AUTOPLAY, BST_CHECKED );
    }

    // set the extended style controls only
    WdeEXSetDefineInfo( o_info, hDlg );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}

void WdeAniCGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask = 0;

    // get the animate control styles
    if( IsDlgButtonChecked( hDlg, IDB_ACS_CENTER ) ) {
        mask |= ACS_CENTER;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ACS_TRANSPARENT ) ) {
        mask |= ACS_TRANSPARENT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ACS_AUTOPLAY ) ) {
        mask |= ACS_AUTOPLAY;
    }

    SETCTL_STYLE( o_info->info.c.info, (GETCTL_STYLE( o_info->info.c.info ) & 0xffff0000) | mask );

    // get the extended control settings
    WdeEXGetDefineInfo( o_info, hDlg );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}

bool WdeAniCDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( hDlg );
    _wde_touch( message );
    _wde_touch( wParam );
    _wde_touch( lParam );
    _wde_touch( mask );

    return( false );
}

LRESULT CALLBACK WdeAniCSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalAniCProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

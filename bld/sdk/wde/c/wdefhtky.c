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
#include "wdefhtky.h"
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
} WdeHtKyObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT bool    CALLBACK WdeHtKyDispatcher( ACTION_ID, OBJPTR, void *, void * );
WINEXPORT LRESULT CALLBACK WdeHtKySuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeHtKy( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeHKCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static void     WdeHtKySetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeHtKyGetDefineInfo( WdeDefineObjectInfo *, HWND );
static bool     WdeHtKyDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

#define pick(e,n,c) static bool WdeHtKy ## n ## c;
    pick_ACTS( WdeHtKyObject )
#undef pick

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static DISPATCH_FN              *WdeHtKyDispatch;
static WdeDialogBoxControl      *WdeDefaultHtKy = NULL;
static int                      WdeHtKyWndExtra;
static WNDPROC                  WdeOriginalHtKyProc;
//static WNDPROC                WdeHtKyProc;

#define WHOTKEY_CLASS    HOTKEY_CLASS

static DISPATCH_ITEM WdeHtKyActions[] = {
    #define pick(e,n,c) {e, (DISPATCH_RTN *)WdeHtKy ## n},
    pick_ACTS( WdeHtKyObject )
    #undef pick
};

#define MAX_ACTIONS      (sizeof( WdeHtKyActions ) / sizeof( DISPATCH_ITEM ))

OBJPTR CALLBACK WdeHtKyCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeHtKy( parent, obj_rect, handle, 0, "", HOTKEY_OBJ ) );
    } else {
        return( WdeHKCreate( parent, obj_rect, NULL, HOTKEY_OBJ, (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeHtKy( OBJPTR parent, RECT *obj_rect, OBJPTR handle, DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultHtKy, style );
    SETCTL_TEXT( WdeDefaultHtKy, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultHtKy, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeHKCreate( parent, obj_rect, handle, id, WdeDefaultHtKy );

    WRMemFree( GETCTL_TEXT( WdeDefaultHtKy ) );
    SETCTL_TEXT( WdeDefaultHtKy, NULL );

    return( new );
}

OBJPTR WdeHKCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeHtKyObject *new;

    WdeDebugCreate( "HtKy", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeHtKyCreate: HtKy has no parent!" );
        return( NULL );
    }

    new = (WdeHtKyObject *)WRMemAlloc( sizeof( WdeHtKyObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeHtKyCreate: Object malloc failed" );
        return( NULL );
    }

    OBJ_DISPATCHER_SET( new, WdeHtKyDispatch );

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = (OBJPTR)new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeHtKyCreate: CONTROL_OBJ not created!" );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeHtKyCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, false );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeHtKyCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, false );
        WRMemFree( new );
        return( NULL );
    }

    return( (OBJPTR)new );
}

bool CALLBACK WdeHtKyDispatcher( ACTION_ID act, OBJPTR obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "HtKy", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeHtKyActions[i].id == act ) {
            return( WdeHtKyActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( ((WdeHtKyObject *)obj)->control, act, p1, p2 ) );
}

bool WdeHtKyInit( bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, WHOTKEY_CLASS, &wc );
    WdeOriginalHtKyProc = wc.lpfnWndProc;
    WdeHtKyWndExtra = wc.cbWndExtra;

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
        //wc.lpfnWndProc = WdeHtKySuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeHtKyInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultHtKy = WdeAllocDialogBoxControl();
    if( WdeDefaultHtKy == NULL ) {
        WdeWriteTrail( "WdeHtKyInit: Alloc of control failed!" );
        return( false );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultHtKy, WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
    SETCTL_ID( WdeDefaultHtKy, 0 );
    SETCTL_EXTRABYTES( WdeDefaultHtKy, 0 );
    SETCTL_SIZEX( WdeDefaultHtKy, 0 );
    SETCTL_SIZEY( WdeDefaultHtKy, 0 );
    SETCTL_SIZEW( WdeDefaultHtKy, 0 );
    SETCTL_SIZEH( WdeDefaultHtKy, 0 );
    SETCTL_TEXT( WdeDefaultHtKy, NULL );
    SETCTL_CLASSID( WdeDefaultHtKy, WdeStrToControlClass( WHOTKEY_CLASS ) );

    WdeHtKyDispatch = MakeProcInstance_DISPATCHER( WdeHtKyDispatcher, WdeGetAppInstance() );
    return( true );
}

void WdeHtKyFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultHtKy );
    FreeProcInstance_DISPATCHER( WdeHtKyDispatch );
}

bool WdeHtKyDestroy( WdeHtKyObject *obj, bool *flag, bool *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeHtKyDestroy: Control DESTROY failed" );
        return( false );
    }

    WRMemFree( obj );

    return( true );
}

bool WdeHtKyValidateAction( WdeHtKyObject *obj, ACTION_ID *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeHtKyActions[i].id == *act ) {
            return( true );
        }
    }

    return( ValidateAction( obj->control, *act, p2 ) );
}

bool WdeHtKyCopyObject( WdeHtKyObject *obj, WdeHtKyObject **new, OBJPTR handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeHtKyCopyObject: Invalid new object!" );
        return( false );
    }

    *new = (WdeHtKyObject *)WRMemAlloc( sizeof( WdeHtKyObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeHtKyCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeHtKyCopyObject: Control not created!" );
        WRMemFree( *new );
        return( false );
    }

    return( true );
}

bool WdeHtKyIdentify( WdeHtKyObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( true );
}

bool WdeHtKyGetWndProc( WdeHtKyObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeHtKySuperClassProc;

    return( true );
}

bool WdeHtKyGetWindowClass( WdeHtKyObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = WHOTKEY_CLASS;

    return( true );
}

bool WdeHtKyDefine( WdeHtKyObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP | WS_BORDER;
    o_info.set_func = (WdeSetProc)WdeHtKySetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeHtKyGetDefineInfo;
    o_info.hook_func = WdeHtKyDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeHtKySetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    // there are no styles for the hotkey control
    // set the extended style controls only
    WdeEXSetDefineInfo( o_info, hDlg );
}

void WdeHtKyGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    // clear the style flags, since there are none for a progress bar
    SETCTL_STYLE( o_info->info.c.info, GETCTL_STYLE( o_info->info.c.info ) & 0xffff0000 );

    // get the extended control settings
    WdeEXGetDefineInfo( o_info, hDlg );
}

bool WdeHtKyDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( hDlg );
    _wde_touch( message );
    _wde_touch( wParam );
    _wde_touch( lParam );
    _wde_touch( mask );

    return( false );
}

LRESULT CALLBACK WdeHtKySuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalHtKyProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

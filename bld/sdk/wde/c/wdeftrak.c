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
#include "win1632.h"

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
#include "wdeftrak.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeTrakObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL    WINEXPORT WdeTrakDispatcher( ACTION, WdeTrakObject *, void *, void * );
extern LRESULT WINEXPORT WdeTrakSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeTrak( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeTrackCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeTrakDestroy( WdeTrakObject *, BOOL *, void * );
static BOOL     WdeTrakValidateAction( WdeTrakObject *, ACTION *, void * );
static BOOL     WdeTrakCopyObject( WdeTrakObject *, WdeTrakObject **, WdeTrakObject * );
static BOOL     WdeTrakIdentify( WdeTrakObject *, OBJ_ID *, void * );
static BOOL     WdeTrakGetWndProc( WdeTrakObject *, WNDPROC *, void * );
static BOOL     WdeTrakGetWindowClass( WdeTrakObject *, char **, void * );
static BOOL     WdeTrakDefine( WdeTrakObject *, POINT *, void * );
static void     WdeTrakSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeTrakGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeTrakDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeTrakDispatch;
static WdeDialogBoxControl      *WdeDefaultTrak = NULL;
static int                      WdeTrakWndExtra;
static WNDPROC                  WdeOriginalTrakProc;
//static WNDPROC                WdeTrakProc;

#define WTRACKBAR_CLASS  TRACKBAR_CLASS

static DISPATCH_ITEM WdeTrakActions[] = {
    { DESTROY,          (BOOL (*)( OBJPTR, void *, void * ))WdeTrakDestroy          },
    { COPY,             (BOOL (*)( OBJPTR, void *, void * ))WdeTrakCopyObject       },
    { VALIDATE_ACTION,  (BOOL (*)( OBJPTR, void *, void * ))WdeTrakValidateAction   },
    { IDENTIFY,         (BOOL (*)( OBJPTR, void *, void * ))WdeTrakIdentify         },
    { GET_WINDOW_CLASS, (BOOL (*)( OBJPTR, void *, void * ))WdeTrakGetWindowClass   },
    { DEFINE,           (BOOL (*)( OBJPTR, void *, void * ))WdeTrakDefine           },
    { GET_WND_PROC,     (BOOL (*)( OBJPTR, void *, void * ))WdeTrakGetWndProc       }
};

#define MAX_ACTIONS     (sizeof( WdeTrakActions ) / sizeof( DISPATCH_ITEM ))

OBJPTR WINEXPORT WdeTrakCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeTrak( parent, obj_rect, handle, 0, "", TRACKBAR_OBJ ) );
    } else {
        return( WdeTrackCreate( parent, obj_rect, NULL, TRACKBAR_OBJ,
                                (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeTrak( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultTrak, style );
    SETCTL_TEXT( WdeDefaultTrak, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultTrak, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeTrackCreate( parent, obj_rect, handle, id, WdeDefaultTrak );

    WdeMemFree( GETCTL_TEXT( WdeDefaultTrak ) );
    SETCTL_TEXT( WdeDefaultTrak, NULL );

    return( new );
}

OBJPTR WdeTrackCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                       OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeTrakObject *new;

    WdeDebugCreate( "Trak", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeTrakCreate: Trak has no parent!" );
        return( NULL );
    }

    new = (WdeTrakObject *)WdeMemAlloc( sizeof( WdeTrakObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeTrakCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeTrakDispatch;
    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeTrakCreate: CONTROL_OBJ not created!" );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeTrakCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeTrakCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    return( new );
}

BOOL WINEXPORT WdeTrakDispatcher( ACTION act, WdeTrakObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Trak", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeTrakActions[i].id == act ) {
            return( WdeTrakActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeTrakInit( Bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, WTRACKBAR_CLASS, &wc );
    WdeOriginalTrakProc = wc.lpfnWndProc;
    WdeTrakWndExtra = wc.cbWndExtra;

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
        //wc.lpfnWndProc = WdeTrakSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeTrakInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultTrak = WdeAllocDialogBoxControl();
    if( WdeDefaultTrak == NULL ) {
        WdeWriteTrail( "WdeTrakInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultTrak, WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
    SETCTL_ID( WdeDefaultTrak, 0 );
    SETCTL_EXTRABYTES( WdeDefaultTrak, 0 );
    SETCTL_SIZEX( WdeDefaultTrak, 0 );
    SETCTL_SIZEY( WdeDefaultTrak, 0 );
    SETCTL_SIZEW( WdeDefaultTrak, 0 );
    SETCTL_SIZEH( WdeDefaultTrak, 0 );
    SETCTL_TEXT( WdeDefaultTrak, NULL );
    SETCTL_CLASSID( WdeDefaultTrak, WdeStrToControlClass( WTRACKBAR_CLASS ) );

    WdeTrakDispatch = MakeProcInstance( (FARPROC)WdeTrakDispatcher,
                                        WdeGetAppInstance() );
    return( TRUE );
}

void WdeTrakFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultTrak );
    FreeProcInstance( WdeTrakDispatch );
}

BOOL WdeTrakDestroy( WdeTrakObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeTrakDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WdeMemFree( obj );

    return( TRUE );
}

BOOL WdeTrakValidateAction( WdeTrakObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeTrakActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeTrakCopyObject( WdeTrakObject *obj, WdeTrakObject **new, WdeTrakObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeTrakCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeTrakObject *)WdeMemAlloc( sizeof( WdeTrakObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeTrakCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeTrakCopyObject: Control not created!" );
        WdeMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeTrakIdentify( WdeTrakObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeTrakGetWndProc( WdeTrakObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeTrakSuperClassProc;

    return( TRUE );
}

BOOL WdeTrakGetWindowClass( WdeTrakObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = WTRACKBAR_CLASS;

    return( TRUE );
}

BOOL WdeTrakDefine( WdeTrakObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP | WS_BORDER;
    o_info.set_func = (WdeSetProc)WdeTrakSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeTrakGetDefineInfo;
    o_info.hook_func = WdeTrakDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeTrakSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask;

    // set the style controls
    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x000000ff;
    if( mask & TBS_AUTOTICKS ) {
        CheckDlgButton( hDlg, IDB_TBS_AUTOTICKS, 1 );
    }
    if( mask & TBS_BOTH ) {
        CheckDlgButton( hDlg, IDB_TBS_BOTH, 1 );
    } else if( mask & TBS_NOTICKS ) {
        CheckDlgButton( hDlg, IDB_TBS_NOTICKS, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_TBS_ONESIDE, 1 );
    }

    if( mask & TBS_VERT ) {
        CheckDlgButton( hDlg, IDB_TBS_VERT, 1 );
        EnableWindow( GetDlgItem( hDlg, IDB_TBS_RIGHT ), TRUE);
        EnableWindow( GetDlgItem( hDlg, IDB_TBS_LEFT ), TRUE);
        EnableWindow( GetDlgItem( hDlg, IDB_TBS_TOP ), FALSE);
        EnableWindow( GetDlgItem( hDlg, IDB_TBS_BOTTOM ), FALSE);
    } else {
        CheckDlgButton( hDlg, IDB_TBS_HORZ, 1 );
        EnableWindow( GetDlgItem( hDlg, IDB_TBS_RIGHT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_TBS_LEFT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_TBS_TOP ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_TBS_BOTTOM ), TRUE );
    }

    if( mask & TBS_TOP ) {
        CheckDlgButton( hDlg, IDB_TBS_TOP, 1 );
        CheckDlgButton( hDlg, IDB_TBS_LEFT, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_TBS_BOTTOM, 1 );
        CheckDlgButton( hDlg, IDB_TBS_RIGHT, 1 );
    }

    if( mask & TBS_ENABLESELRANGE ) {
        CheckDlgButton( hDlg, IDB_TBS_ENABLESELRANGE, 1 );
    }
    if( mask & TBS_FIXEDLENGTH ) {
        CheckDlgButton( hDlg, IDB_TBS_FIXEDLENGTH, 1 );
    }
    if( mask & TBS_NOTHUMB ) {
        CheckDlgButton( hDlg, IDB_TBS_NOTHUMB, 1 );
    }

    // set the extended style controls only
    WdeEXSetDefineInfo( o_info, hDlg );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}

void WdeTrakGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#ifdef __NT__XX
    DialogStyle mask = 0;
    BOOL        vert = FALSE;

    // get the track bar settings
    if( IsDlgButtonChecked( hDlg, IDB_TBS_AUTOTICKS ) ) {
        mask |= TBS_AUTOTICKS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TBS_BOTH ) ) {
        mask |= TBS_BOTH;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TBS_NOTICKS ) ) {
        mask |= TBS_NOTICKS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TBS_VERT ) ) {
        mask |= TBS_VERT;
        vert = TRUE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TBS_TOP ) ) {
        if( !vert ) {
            mask |= TBS_TOP;
        }
    }
    if( IsDlgButtonChecked( hDlg, IDB_TBS_LEFT ) ) {
        if( vert ) {
            mask |= TBS_LEFT;
        }
    }
    if( IsDlgButtonChecked( hDlg, IDB_TBS_ENABLESELRANGE ) ) {
        mask |= TBS_ENABLESELRANGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TBS_FIXEDLENGTH ) ) {
        mask |= TBS_FIXEDLENGTH;
    }
    if( IsDlgButtonChecked( hDlg, IDB_TBS_NOTHUMB ) ) {
        mask |= TBS_NOTHUMB;
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

BOOL WdeTrakDefineHook( HWND hDlg, UINT message,
                        WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
#ifdef __NT__XX
    BOOL    processed;
    BOOL    flag;
    WORD    wp;

    /* touch unused vars to get rid of warning */
    _wde_touch( mask );
    _wde_touch( lParam );

    processed = FALSE;

    if( message == WM_COMMAND && GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
        wp = LOWORD( wParam );
        switch( wp ) {
        case IDB_TBS_ONESIDE:
            if( IsDlgButtonChecked( hDlg, IDB_TBS_VERT ) ) {
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_RIGHT ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_LEFT ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_TOP ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_BOTTOM ), FALSE );
            } else {
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_RIGHT ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_LEFT ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_TOP ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_BOTTOM ), TRUE );
            }
            processed = TRUE;
            break;

        case IDB_TBS_BOTH:
        case IDB_TBS_NOTICKS:
            flag = TRUE;
            if( IsDlgButtonChecked( hDlg, IDB_TBS_BOTH ) ||
                IsDlgButtonChecked( hDlg, IDB_TBS_NOTICKS ) ) {
                flag = FALSE;
            }
            EnableWindow( GetDlgItem( hDlg, IDB_TBS_RIGHT ), flag );
            EnableWindow( GetDlgItem( hDlg, IDB_TBS_LEFT ), flag );
            EnableWindow( GetDlgItem( hDlg, IDB_TBS_TOP ), flag );
            EnableWindow( GetDlgItem( hDlg, IDB_TBS_BOTTOM ), flag );
            processed = TRUE;
            break;

        case IDB_TBS_VERT:
            if( IsDlgButtonChecked( hDlg, IDB_TBS_ONESIDE ) ) {
                flag = FALSE;
                if( IsDlgButtonChecked( hDlg, IDB_TBS_VERT ) ) {
                    flag = TRUE;
                }
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_RIGHT ), flag );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_LEFT ), flag );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_TOP ), !flag );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_BOTTOM ), !flag );
            }
            processed = TRUE;
            break;

       case IDB_TBS_HORZ:
            if( IsDlgButtonChecked( hDlg, IDB_TBS_ONESIDE ) ) {
                flag = TRUE;
                if( IsDlgButtonChecked( hDlg, IDB_TBS_HORZ ) ) {
                    flag = FALSE;
                }
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_RIGHT ), flag );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_LEFT ), flag );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_TOP ), !flag );
                EnableWindow( GetDlgItem( hDlg, IDB_TBS_BOTTOM ), !flag );
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

LRESULT WINEXPORT WdeTrakSuperClassProc( HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalTrakProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

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
#include "wdecctl.h"
#include "wdefedit.h"
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
} WdeEditObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT bool    CALLBACK WdeEditDispatcher( ACTION_ID, OBJPTR, void *, void * );
WINEXPORT LRESULT CALLBACK WdeEditSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeEdit( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeEdCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static void     WdeEditSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeEditGetDefineInfo( WdeDefineObjectInfo *, HWND );
static bool     WdeEditDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

#define pick(e,n,c) static bool WdeEdit ## n ## c;
    pick_ACTS( WdeEditObject )
#undef pick

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static DISPATCH_FN              *WdeEditDispatch;
static WdeDialogBoxControl      *WdeDefaultEdit = NULL;
static int                      WdeEditWndExtra;
static WNDPROC                  WdeOriginalEditProc;
//static WNDPROC                WdeEditProc;

static DISPATCH_ITEM WdeEditActions[] = {
    #define pick(e,n,c) {e, (DISPATCH_RTN *)WdeEdit ## n},
    pick_ACTS( WdeEditObject )
    #undef pick
};

#define MAX_ACTIONS      (sizeof( WdeEditActions ) / sizeof( DISPATCH_ITEM ))

OBJPTR CALLBACK WdeEditCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeEdit( parent, obj_rect, handle, ES_LEFT | ES_AUTOHSCROLL, "", EDIT_OBJ ) );
    } else {
        return( WdeEdCreate( parent, obj_rect, NULL, EDIT_OBJ, (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeEdit( OBJPTR parent, RECT *obj_rect, OBJPTR handle, DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultEdit, style );
    SETCTL_TEXT( WdeDefaultEdit, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultEdit, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeEdCreate( parent, obj_rect, handle, id, WdeDefaultEdit );

    WRMemFree( GETCTL_TEXT( WdeDefaultEdit ) );
    SETCTL_TEXT( WdeDefaultEdit, NULL );

    return( new );
}

OBJPTR WdeEdCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeEditObject *new;

    WdeDebugCreate( "Edit", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeEditCreate: Edit has no parent!" );
        return( NULL );
    }

    new = (WdeEditObject *)WRMemAlloc( sizeof( WdeEditObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeEditCreate: Object malloc failed" );
        return( NULL );
    }

    OBJ_DISPATCHER_SET( new, WdeEditDispatch );

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = (OBJPTR)new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeEditCreate: CONTROL_OBJ not created!" );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeEditCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, false );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeEditCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, false );
        WRMemFree( new );
        return( NULL );
    }

    return( (OBJPTR)new );
}

bool CALLBACK WdeEditDispatcher( ACTION_ID act, OBJPTR obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Edit", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeEditActions[i].id == act ) {
            return( WdeEditActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( ((WdeEditObject *)obj)->control, act, p1, p2 ) );
}

bool WdeEditInit( bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, "Edit", &wc );
    WdeOriginalEditProc = wc.lpfnWndProc;
    WdeEditWndExtra = wc.cbWndExtra;

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
        //wc.lpfnWndProc = WdeEditSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeEditInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultEdit = WdeAllocDialogBoxControl();
    if( WdeDefaultEdit == NULL ) {
        WdeWriteTrail( "WdeEditInit: Alloc of control failed!" );
        return( false );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultEdit, WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
    SETCTL_ID( WdeDefaultEdit, 0 );
    SETCTL_EXTRABYTES( WdeDefaultEdit, 0 );
    SETCTL_SIZEX( WdeDefaultEdit, 0 );
    SETCTL_SIZEY( WdeDefaultEdit, 0 );
    SETCTL_SIZEW( WdeDefaultEdit, 0 );
    SETCTL_SIZEH( WdeDefaultEdit, 0 );
    SETCTL_TEXT( WdeDefaultEdit, NULL );
    SETCTL_CLASSID( WdeDefaultEdit, ResNumToControlClass( CLASS_EDIT ) );

    WdeEditDispatch = MakeProcInstance_DISPATCHER( WdeEditDispatcher, WdeGetAppInstance() );

    return( true );
}

void WdeEditFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultEdit );
    FreeProcInstance_DISPATCHER( WdeEditDispatch );
}

bool WdeEditDestroy( WdeEditObject *obj, bool *flag, bool *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeEditDestroy: Control DESTROY failed" );
        return( false );
    }

    WRMemFree( obj );

    return( true );
}

bool WdeEditValidateAction( WdeEditObject *obj, ACTION_ID *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeEditActions[i].id == *act ) {
            return( true );
        }
    }

    return( ValidateAction( obj->control, *act, p2 ) );
}

bool WdeEditCopyObject( WdeEditObject *obj, WdeEditObject **new, OBJPTR handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeEditCopyObject: Invalid new object!" );
        return( false );
    }

    *new = (WdeEditObject *)WRMemAlloc( sizeof( WdeEditObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeEditCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeEditCopyObject: Control not created!" );
        WRMemFree( *new );
        return( false );
    }

    return( true );
}

bool WdeEditIdentify( WdeEditObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( true );
}

bool WdeEditGetWndProc( WdeEditObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeEditSuperClassProc;

    return( true );
}

bool WdeEditGetWindowClass( WdeEditObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = "Edit";

    return( true );
}

bool WdeEditDefine( WdeEditObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_VSCROLL | WS_HSCROLL |
                  WS_TABSTOP | WS_GROUP | WS_BORDER | WS_THICKFRAME;
    o_info.set_func = (WdeSetProc)WdeEditSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeEditGetDefineInfo;
    o_info.hook_func = WdeEditDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeEditSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000ffff;

    /* set the type of combo box */
    if( !(mask & 0x00000003) ) {
        CheckDlgButton( hDlg, IDB_ES_LEFT, BST_CHECKED );
    } else if( mask & ES_CENTER ) {
        CheckDlgButton( hDlg, IDB_ES_CENTER, BST_CHECKED );
    } else if( mask & ES_RIGHT ) {
        CheckDlgButton( hDlg, IDB_ES_RIGHT, BST_CHECKED );
    } else {
        WdeWriteTrail( "WdeEditSetDefineInfo: Bad Edit mask!" );
    }

    if( mask & ES_UPPERCASE ) {
        CheckDlgButton( hDlg, IDB_ES_UPPERCASE, BST_CHECKED );
    }
    if( mask & ES_LOWERCASE ) {
        CheckDlgButton( hDlg, IDB_ES_LOWERCASE, BST_CHECKED );
    }
    if( mask & ES_PASSWORD ) {
        CheckDlgButton( hDlg, IDB_ES_PASSWORD, BST_CHECKED );
    }
    if( mask & ES_AUTOVSCROLL ) {
        CheckDlgButton( hDlg, IDB_ES_AUTOVSCROLL, BST_CHECKED );
    }
    if( mask & ES_AUTOHSCROLL ) {
        CheckDlgButton( hDlg, IDB_ES_AUTOHSCROLL, BST_CHECKED );
    }
    if( mask & ES_NOHIDESEL ) {
        CheckDlgButton( hDlg, IDB_ES_NOHIDESEL, BST_CHECKED );
    }
    if( mask & ES_OEMCONVERT ) {
        CheckDlgButton( hDlg, IDB_ES_OEMCONVERT, BST_CHECKED );
    }
    if( mask & ES_READONLY ) {
        CheckDlgButton( hDlg, IDB_ES_READONLY, BST_CHECKED );
    }
    if( mask & ES_WANTRETURN ) {
        CheckDlgButton( hDlg, IDB_ES_WANTRETURN, BST_CHECKED );
    }
    if( mask & ES_MULTILINE ) {
        CheckDlgButton( hDlg, IDB_ES_MULTILINE, BST_CHECKED );
    } else {
        EnableWindow( GetDlgItem( hDlg, IDB_ES_AUTOVSCROLL ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_WS_HSCROLL ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_WS_VSCROLL ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_ES_CENTER ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_ES_RIGHT ), FALSE );
    }

#if __NT__XX
    EnableWindow( GetDlgItem( hDlg, IDB_ES_NUMBER ), TRUE );
    if( mask & ES_NUMBER ) {
        CheckDlgButton( hDlg, IDB_ES_NUMBER, BST_CHECKED );
    }
#else
    EnableWindow( GetDlgItem( hDlg, IDB_ES_NUMBER ), FALSE );
#endif

#if __NT__XX
    // do the extended style stuff
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_STATICEDGE ), TRUE);
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_NOPARENTNOTIFY ), TRUE);
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LEFT ), TRUE);
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RIGHT ), TRUE);
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LTRREADING ), TRUE);
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RTLREADING ), TRUE);

    mask = GETCTL_EXSTYLE( o_info->info.c.info );
    if( mask & WS_EX_STATICEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_STATICEDGE, BST_CHECKED );
    }
    if( mask & WS_EX_NOPARENTNOTIFY ) {
        CheckDlgButton( hDlg, IDB_WS_EX_NOPARENTNOTIFY, BST_CHECKED );
    }
    if( mask & WS_EX_RIGHT ) {
        CheckDlgButton( hDlg, IDB_WS_EX_RIGHT, BST_CHECKED );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_LEFT, BST_CHECKED );
    }
    if( mask & WS_EX_RTLREADING ) {
        CheckDlgButton( hDlg, IDB_WS_EX_RTLREADING, BST_CHECKED );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_LTRREADING, BST_CHECKED );
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

void WdeEditGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = 0;

    if( IsDlgButtonChecked( hDlg, IDB_ES_LEFT ) ) {
        mask |= ES_LEFT;
    } else if( IsDlgButtonChecked( hDlg, IDB_ES_CENTER ) ) {
        mask |= ES_CENTER;
    } else if( IsDlgButtonChecked( hDlg, IDB_ES_RIGHT ) ) {
        mask |= ES_RIGHT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_ES_MULTILINE ) ) {
        mask |= ES_MULTILINE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_UPPERCASE ) ) {
        mask |= ES_UPPERCASE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_LOWERCASE ) ) {
        mask |= ES_LOWERCASE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_PASSWORD ) ) {
        mask |= ES_PASSWORD;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_AUTOVSCROLL ) ) {
        mask |= ES_AUTOVSCROLL;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_AUTOHSCROLL ) ) {
        mask |= ES_AUTOHSCROLL;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_NOHIDESEL ) ) {
        mask |= ES_NOHIDESEL;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_OEMCONVERT ) ) {
        mask |= ES_OEMCONVERT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_READONLY ) ) {
        mask |= ES_READONLY;
    }
    if( IsDlgButtonChecked( hDlg, IDB_ES_WANTRETURN ) ) {
        mask |= ES_WANTRETURN;
    }

#if __NT__XX
    if( IsDlgButtonChecked( hDlg, IDB_ES_NUMBER ) ) {
        mask |= ES_NUMBER;
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

bool WdeEditDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    bool processed;

    /* touch unused vars to get rid of warning */
    _wde_touch( mask );
    _wde_touch( lParam );

    processed = false;

    if( message == WM_COMMAND && GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
        switch( LOWORD( wParam ) ) {
        case IDB_ES_LEFT:
            if( IsDlgButtonChecked( hDlg, IDB_ES_LEFT ) ) {
                CheckDlgButton( hDlg, IDB_ES_RIGHT, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_ES_CENTER, BST_UNCHECKED );
            }
            processed = true;
            break;

        case IDB_ES_CENTER:
            if( IsDlgButtonChecked( hDlg, IDB_ES_CENTER ) ) {
                CheckDlgButton( hDlg, IDB_ES_LEFT, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_ES_RIGHT, BST_UNCHECKED );
            }
            processed = true;
            break;

        case IDB_ES_UPPERCASE:
            if( IsDlgButtonChecked( hDlg, IDB_ES_UPPERCASE ) ) {
                CheckDlgButton( hDlg, IDB_ES_LOWERCASE, BST_UNCHECKED );
            }
            processed = true;
            break;

        case IDB_ES_LOWERCASE:
            if( IsDlgButtonChecked( hDlg, IDB_ES_LOWERCASE ) ) {
                CheckDlgButton( hDlg, IDB_ES_UPPERCASE, BST_UNCHECKED );
            }
            processed = true;
            break;

        case IDB_ES_RIGHT:
            if( IsDlgButtonChecked( hDlg, IDB_ES_RIGHT ) ) {
                CheckDlgButton( hDlg, IDB_ES_LEFT, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_ES_CENTER, BST_UNCHECKED );
            }
            processed = true;
            break;

        case IDB_ES_MULTILINE:
            if( IsDlgButtonChecked( hDlg, IDB_ES_MULTILINE ) ) {
                EnableWindow( GetDlgItem( hDlg, IDB_ES_CENTER ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_RIGHT ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_AUTOVSCROLL ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_WS_HSCROLL ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_WS_VSCROLL ), TRUE );
            } else {
                CheckDlgButton( hDlg, IDB_ES_AUTOVSCROLL, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_WS_VSCROLL, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_WS_HSCROLL, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_ES_CENTER, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_ES_RIGHT, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_ES_LEFT, BST_CHECKED );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_AUTOVSCROLL ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_WS_HSCROLL ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_WS_VSCROLL ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_CENTER ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_RIGHT ), FALSE );
            }
            processed = true;
            break;
        }
    }

    return( processed );
}

LRESULT CALLBACK WdeEditSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalEditProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

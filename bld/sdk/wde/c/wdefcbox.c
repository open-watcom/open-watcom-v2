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
#include "wderes.h"
#include "wdeobjid.h"
#include "wdefutil.h"
#include "wde_wres.h"
#include "wdeedit.h"
#include "wdemain.h"
#include "wdedefin.h"
#include "wdedebug.h"
#include "wdedefsz.h"
#include "wdeoinfo.h"
#include "wde_rc.h"
#include "wdecctl.h"
#include "wdefcbox.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeCBoxObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL    CALLBACK WdeCBoxDispatcher( ACTION, WdeCBoxObject *, void *, void * );
WINEXPORT LRESULT CALLBACK WdeCBoxSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeCBox( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeCBCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeCBoxDestroy( WdeCBoxObject *, BOOL *, void * );
static BOOL     WdeCBoxValidateAction( WdeCBoxObject *, ACTION *, void * );
static BOOL     WdeCBoxCopyObject( WdeCBoxObject *, WdeCBoxObject **, WdeCBoxObject * );
static BOOL     WdeCBoxIdentify( WdeCBoxObject *, OBJ_ID *, void * );
static BOOL     WdeCBoxGetWndProc( WdeCBoxObject *, WNDPROC *, void * );
static BOOL     WdeCBoxGetWindowClass( WdeCBoxObject *, char **, void * );
static BOOL     WdeCBoxDefine( WdeCBoxObject *, POINT *, void * );
static void     WdeCBoxSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeCBoxGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeCBoxDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeCBoxDispatch;
static WdeDialogBoxControl      *WdeDefaultCBox = NULL;
static int                      WdeCBoxWndExtra;
static WNDPROC                  WdeOriginalCBoxProc;
//static WNDPROC                WdeCBoxProc;

static DISPATCH_ITEM WdeCBoxActions[] = {
    { DESTROY,          (DISPATCH_RTN *)WdeCBoxDestroy              },
    { COPY,             (DISPATCH_RTN *)WdeCBoxCopyObject           },
    { VALIDATE_ACTION,  (DISPATCH_RTN *)WdeCBoxValidateAction       },
    { IDENTIFY,         (DISPATCH_RTN *)WdeCBoxIdentify             },
    { GET_WINDOW_CLASS, (DISPATCH_RTN *)WdeCBoxGetWindowClass       },
    { DEFINE,           (DISPATCH_RTN *)WdeCBoxDefine               },
    { GET_WND_PROC,     (DISPATCH_RTN *)WdeCBoxGetWndProc           }
};

#define MAX_ACTIONS      (sizeof( WdeCBoxActions ) / sizeof( DISPATCH_ITEM ))

WINEXPORT OBJPTR CALLBACK WdeCBoxCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeCBox( parent, obj_rect, handle,
                             CBS_DROPDOWN, "", COMBOBOX_OBJ ) );
    } else {
        return( WdeCBCreate( parent, obj_rect, NULL, COMBOBOX_OBJ,
                             (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeCBox( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_VISIBLE | WS_TABSTOP | WS_CHILD;
    SETCTL_STYLE( WdeDefaultCBox, style );
    SETCTL_TEXT( WdeDefaultCBox, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultCBox, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeCBCreate( parent, obj_rect, handle, id, WdeDefaultCBox );

    WRMemFree( GETCTL_TEXT( WdeDefaultCBox ) );
    SETCTL_TEXT( WdeDefaultCBox, NULL );

    return( new );
}

OBJPTR WdeCBCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeCBoxObject *new;

    WdeDebugCreate( "CBox", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeCBoxCreate: CBox has no parent!" );
        return( NULL );
    }

    new = (WdeCBoxObject *)WRMemAlloc( sizeof( WdeCBoxObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeCBoxCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeCBoxDispatch;

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeCBoxCreate: CONTROL_OBJ not created!" );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeCBoxCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeCBoxCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    return( new );
}

WINEXPORT BOOL CALLBACK WdeCBoxDispatcher( ACTION act, WdeCBoxObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "CBox", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeCBoxActions[i].id == act ) {
            return( WdeCBoxActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeCBoxInit( Bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, "COMBOBOX", &wc );
    WdeOriginalCBoxProc = wc.lpfnWndProc;
    WdeCBoxWndExtra = wc.cbWndExtra;

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
        wc.lpszClassName = "wdecombobox";
        wc.cbWndExtra += sizeof( OBJPTR );
        //wc.lpfnWndProc = WdeCBoxSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeCBoxInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultCBox = WdeAllocDialogBoxControl();
    if( !WdeDefaultCBox ) {
        WdeWriteTrail( "WdeCBoxInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultCBox, 0 );
    SETCTL_ID( WdeDefaultCBox, 0 );
    SETCTL_EXTRABYTES( WdeDefaultCBox, 0 );
    SETCTL_SIZEX( WdeDefaultCBox, 0 );
    SETCTL_SIZEY( WdeDefaultCBox, 0 );
    SETCTL_SIZEW( WdeDefaultCBox, 0 );
    SETCTL_SIZEH( WdeDefaultCBox, 0 );
    SETCTL_TEXT( WdeDefaultCBox, NULL );
    SETCTL_CLASSID( WdeDefaultCBox, ResNumToControlClass( CLASS_COMBOBOX ) );

    WdeCBoxDispatch = MakeProcInstance( (FARPROC)WdeCBoxDispatcher, WdeGetAppInstance());
    return( TRUE );
}

void WdeCBoxFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultCBox );
    FreeProcInstance( WdeCBoxDispatch );
}

BOOL WdeCBoxDestroy( WdeCBoxObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeCBoxDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WRMemFree( obj );

    return( TRUE );
}

BOOL WdeCBoxValidateAction( WdeCBoxObject *obj, ACTION *act, void *p2 )
{
    int     i;

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeCBoxActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeCBoxCopyObject( WdeCBoxObject *obj, WdeCBoxObject **new, WdeCBoxObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeCBoxCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeCBoxObject *)WRMemAlloc( sizeof( WdeCBoxObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeCBoxCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeCBoxCopyObject: Control not created!" );
        WRMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeCBoxIdentify( WdeCBoxObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeCBoxGetWndProc( WdeCBoxObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeCBoxSuperClassProc;

    return( TRUE );
}

BOOL WdeCBoxGetWindowClass( WdeCBoxObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = "combobox";

    return( TRUE );
}

BOOL WdeCBoxDefine( WdeCBoxObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP |
                  WS_VSCROLL | WS_HSCROLL;
    o_info.set_func = (WdeSetProc)WdeCBoxSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeCBoxGetDefineInfo;
    o_info.hook_func = WdeCBoxDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeCBoxSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x00000003;

    /* set the type of combo box */
    if( mask == CBS_SIMPLE ) {
        CheckDlgButton( hDlg, IDB_CBS_SIMPLE, 1 );
    } else if( mask == CBS_DROPDOWN ) {
        CheckDlgButton( hDlg, IDB_CBS_DROPDOWN, 1 );
    } else if( mask == CBS_DROPDOWNLIST ) {
        CheckDlgButton( hDlg, IDB_CBS_DROPDOWNLIST, 1 );
    } else {
        WdeWriteTrail( "WdeCBoxSetDefineInfo: Bad Combo mask!" );
    }

    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000fff0;

    /* set the combo box options */
    if( mask & CBS_AUTOHSCROLL ) {
        CheckDlgButton( hDlg, IDB_CBS_AUTOHSCROLL, 1 );
    }

    if( mask & CBS_OEMCONVERT ) {
        CheckDlgButton( hDlg, IDB_CBS_OEMCONVERT, 1 );
    }

    if( mask & CBS_SORT ) {
        CheckDlgButton( hDlg, IDB_CBS_SORT, 1 );
    }

    if( mask & CBS_NOINTEGRALHEIGHT ) {
        CheckDlgButton( hDlg, IDB_CBS_NOINTEGRALHEIGHT, 1 );
    }

    if( mask & CBS_DISABLENOSCROLL ) {
        CheckDlgButton( hDlg, IDB_CBS_DISABLENOSCROLL, 1 );
    }

    CheckDlgButton( hDlg, IDB_CBS_HASSTRINGS, 1 );
    EnableWindow( GetDlgItem( hDlg, IDB_CBS_HASSTRINGS ), FALSE );

    if( mask & CBS_OWNERDRAWFIXED ) {
        CheckDlgButton( hDlg, IDB_CBS_OWNERDRAWFIXED, 1 );
        EnableWindow( GetDlgItem( hDlg, IDB_CBS_HASSTRINGS ), TRUE );
        if( !(mask & CBS_HASSTRINGS) ) {
            CheckDlgButton( hDlg, IDB_CBS_HASSTRINGS, 0 );
        }
    }

    if( mask & CBS_OWNERDRAWVARIABLE ) {
        CheckDlgButton( hDlg, IDB_CBS_OWNERDRAWVARIABLE, 1 );
        EnableWindow( GetDlgItem( hDlg, IDB_CBS_HASSTRINGS ), TRUE );
        if( !(mask & CBS_HASSTRINGS) ) {
            CheckDlgButton( hDlg, IDB_CBS_HASSTRINGS, 0 );
        }
    }

#if __NT__XX
    EnableWindow( GetDlgItem( hDlg, IDB_CBS_LOWERCASE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_CBS_UPPERCASE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_CBS_BOTHCASE ), TRUE );
    if( GETCTL_STYLE( o_info->info.c.info ) & CBS_LOWERCASE ) {
        CheckDlgButton( hDlg, IDB_CBS_LOWERCASE, 1 );
    } else if( GETCTL_STYLE( o_info->info.c.info ) & CBS_UPPERCASE ) {
        CheckDlgButton( hDlg, IDB_CBS_UPPERCASE, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_CBS_BOTHCASE, 1 );
    }
#else
    EnableWindow( GetDlgItem( hDlg, IDB_CBS_LOWERCASE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_CBS_UPPERCASE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_CBS_BOTHCASE ), FALSE );
#endif

#if __NT__XX
    // Do the extended style stuff.
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
    // Disable the extended styles.
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_STATICEDGE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_NOPARENTNOTIFY ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LEFT ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RIGHT ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LTRREADING ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RTLREADING ), FALSE );
#endif
}

void WdeCBoxGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = 0;

    if( IsDlgButtonChecked( hDlg, IDB_CBS_SIMPLE ) ) {
        mask |= CBS_SIMPLE;
    } else if( IsDlgButtonChecked( hDlg, IDB_CBS_DROPDOWN ) ) {
        mask |= CBS_DROPDOWN;
    } else if( IsDlgButtonChecked( hDlg, IDB_CBS_DROPDOWNLIST ) ) {
        mask |= CBS_DROPDOWNLIST;
    } else {
        WdeWriteTrail( "WdeCBoxGetDefineInfo: Bad Combo style!" );
    }

    if( IsDlgButtonChecked( hDlg, IDB_CBS_OWNERDRAWFIXED ) ) {
        mask |= CBS_OWNERDRAWFIXED;
    }

    if( IsDlgButtonChecked( hDlg, IDB_CBS_OWNERDRAWVARIABLE ) ) {
        mask |= CBS_OWNERDRAWVARIABLE;
    }

    if( IsDlgButtonChecked( hDlg, IDB_CBS_AUTOHSCROLL ) ) {
        mask |= CBS_AUTOHSCROLL;
    }

    if( IsDlgButtonChecked( hDlg, IDB_CBS_OEMCONVERT ) ) {
        mask |= CBS_OEMCONVERT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_CBS_SORT ) ) {
        mask |= CBS_SORT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_CBS_HASSTRINGS ) ) {
        mask |= CBS_HASSTRINGS;
    }

    if( IsDlgButtonChecked( hDlg, IDB_CBS_NOINTEGRALHEIGHT ) ) {
        mask |= CBS_NOINTEGRALHEIGHT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_CBS_DISABLENOSCROLL ) ) {
        mask |= CBS_DISABLENOSCROLL;
    }

#if __NT__XX
    if( IsDlgButtonChecked( hDlg, IDB_CBS_LOWERCASE ) ) {
        mask |= CBS_LOWERCASE;
    } else if( IsDlgButtonChecked( hDlg, IDB_CBS_UPPERCASE ) ) {
        mask |= CBS_UPPERCASE;
    }
#endif

    SETCTL_STYLE( o_info->info.c.info,
                  (GETCTL_STYLE( o_info->info.c.info ) & 0xffff0000) | mask );

#if __NT__XX
    // Set the extended mask.
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

BOOL WdeCBoxDefineHook ( HWND hDlg, UINT message,
                         WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    BOOL processed;
    WORD wp;

    /* touch unused vars to get rid of warning */
    _wde_touch( mask );
    _wde_touch( lParam );

    processed = FALSE;

    if( message == WM_COMMAND && GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
        wp = LOWORD( wParam );
        switch( wp ) {
        case IDB_CBS_OWNERDRAWFIXED:
        case IDB_CBS_OWNERDRAWVARIABLE:
            if( IsDlgButtonChecked( hDlg, IDB_CBS_OWNERDRAWFIXED ) ||
                IsDlgButtonChecked( hDlg, IDB_CBS_OWNERDRAWVARIABLE ) ) {
                EnableWindow( GetDlgItem( hDlg, IDB_CBS_HASSTRINGS ), TRUE );
                CheckDlgButton( hDlg, IDB_CBS_OWNERDRAWFIXED, 0 );
                CheckDlgButton( hDlg, IDB_CBS_OWNERDRAWVARIABLE, 0 );
                CheckDlgButton( hDlg, wp, 1 );
            } else {
                CheckDlgButton( hDlg, IDB_CBS_HASSTRINGS, 1 );
                EnableWindow( GetDlgItem( hDlg, IDB_CBS_HASSTRINGS ), FALSE );
            }
            processed = TRUE;
            break;
        }
    }

    return ( processed );
}


WINEXPORT LRESULT CALLBACK WdeCBoxSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT            ret;

    if( WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        ret = FALSE;
    } else {
        ret = CallWindowProc( WdeOriginalCBoxProc, hWnd, message, wParam, lParam );
    }

    return( ret );
}

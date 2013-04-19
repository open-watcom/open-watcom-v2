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
#include "wdecctl.h"
#include "wdefedit.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeEditObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL    CALLBACK WdeEditDispatcher( ACTION, WdeEditObject *, void *, void * );
WINEXPORT LRESULT CALLBACK WdeEditSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeEdit( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeEdCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeEditDestroy( WdeEditObject *, BOOL *, void * );
static BOOL     WdeEditValidateAction( WdeEditObject *, ACTION *, void * );
static BOOL     WdeEditCopyObject( WdeEditObject *, WdeEditObject **, WdeEditObject * );
static BOOL     WdeEditIdentify( WdeEditObject *, OBJ_ID *, void * );
static BOOL     WdeEditGetWndProc( WdeEditObject *, WNDPROC *, void * );
static BOOL     WdeEditGetWindowClass( WdeEditObject *, char **, void * );
static BOOL     WdeEditDefine( WdeEditObject *, POINT *, void * );
static void     WdeEditSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeEditGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeEditDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeEditDispatch;
static WdeDialogBoxControl      *WdeDefaultEdit = NULL;
static int                      WdeEditWndExtra;
static WNDPROC                  WdeOriginalEditProc;
//static WNDPROC                WdeEditProc;

static DISPATCH_ITEM WdeEditActions[] = {
    { DESTROY,          (DISPATCH_RTN *)WdeEditDestroy              },
    { COPY,             (DISPATCH_RTN *)WdeEditCopyObject           },
    { VALIDATE_ACTION,  (DISPATCH_RTN *)WdeEditValidateAction       },
    { IDENTIFY,         (DISPATCH_RTN *)WdeEditIdentify             },
    { GET_WINDOW_CLASS, (DISPATCH_RTN *)WdeEditGetWindowClass       },
    { DEFINE,           (DISPATCH_RTN *)WdeEditDefine               },
    { GET_WND_PROC,     (DISPATCH_RTN *)WdeEditGetWndProc           }
};

#define MAX_ACTIONS      (sizeof( WdeEditActions ) / sizeof( DISPATCH_ITEM ))

WINEXPORT OBJPTR CALLBACK WdeEditCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeEdit( parent, obj_rect, handle,
                             ES_LEFT | ES_AUTOHSCROLL, "", EDIT_OBJ ) );
    } else {
        return( WdeEdCreate( parent, obj_rect, NULL, EDIT_OBJ,
                             (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeEdit( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultEdit, style );
    SETCTL_TEXT( WdeDefaultEdit, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultEdit, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeEdCreate( parent, obj_rect, handle, id, WdeDefaultEdit );

    WdeMemFree( GETCTL_TEXT( WdeDefaultEdit ) );
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

    new = (WdeEditObject *)WdeMemAlloc( sizeof( WdeEditObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeEditCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeEditDispatch;

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeEditCreate: CONTROL_OBJ not created!" );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeEditCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeEditCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    return( new );
}

WINEXPORT BOOL CALLBACK WdeEditDispatcher( ACTION act, WdeEditObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Edit", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeEditActions[i].id == act ) {
            return( WdeEditActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeEditInit( Bool first )
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
        return( FALSE );
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

    WdeEditDispatch = MakeProcInstance( (FARPROC)WdeEditDispatcher, WdeGetAppInstance() );

    return( TRUE );
}

void WdeEditFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultEdit );
    FreeProcInstance( WdeEditDispatch );
}

BOOL WdeEditDestroy( WdeEditObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeEditDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WdeMemFree( obj );

    return( TRUE );
}

BOOL WdeEditValidateAction( WdeEditObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeEditActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeEditCopyObject( WdeEditObject *obj, WdeEditObject **new,
                        WdeEditObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeEditCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeEditObject *)WdeMemAlloc( sizeof( WdeEditObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeEditCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeEditCopyObject: Control not created!" );
        WdeMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeEditIdentify( WdeEditObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeEditGetWndProc( WdeEditObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeEditSuperClassProc;

    return( TRUE );
}

BOOL WdeEditGetWindowClass( WdeEditObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = "Edit";

    return( TRUE );
}

BOOL WdeEditDefine( WdeEditObject *obj, POINT *pnt, void *p2 )
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
        CheckDlgButton( hDlg, IDB_ES_LEFT, 1 );
    } else if( mask & ES_CENTER ) {
        CheckDlgButton( hDlg, IDB_ES_CENTER, 1 );
    } else if( mask & ES_RIGHT ) {
        CheckDlgButton( hDlg, IDB_ES_RIGHT, 1 );
    } else {
        WdeWriteTrail( "WdeEditSetDefineInfo: Bad Edit mask!" );
    }

    if( mask & ES_UPPERCASE ) {
        CheckDlgButton( hDlg, IDB_ES_UPPERCASE, 1 );
    }
    if( mask & ES_LOWERCASE ) {
        CheckDlgButton( hDlg, IDB_ES_LOWERCASE, 1 );
    }
    if( mask & ES_PASSWORD ) {
        CheckDlgButton( hDlg, IDB_ES_PASSWORD, 1 );
    }
    if( mask & ES_AUTOVSCROLL ) {
        CheckDlgButton( hDlg, IDB_ES_AUTOVSCROLL, 1 );
    }
    if( mask & ES_AUTOHSCROLL ) {
        CheckDlgButton( hDlg, IDB_ES_AUTOHSCROLL, 1 );
    }
    if( mask & ES_NOHIDESEL ) {
        CheckDlgButton( hDlg, IDB_ES_NOHIDESEL, 1 );
    }
    if( mask & ES_OEMCONVERT ) {
        CheckDlgButton( hDlg, IDB_ES_OEMCONVERT, 1 );
    }
    if( mask & ES_READONLY ) {
        CheckDlgButton( hDlg, IDB_ES_READONLY, 1 );
    }
    if( mask & ES_WANTRETURN ) {
        CheckDlgButton( hDlg, IDB_ES_WANTRETURN, 1 );
    }
    if( mask & ES_MULTILINE ) {
        CheckDlgButton( hDlg, IDB_ES_MULTILINE, 1 );
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
        CheckDlgButton( hDlg, IDB_ES_NUMBER, 1 );
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

BOOL WdeEditDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    BOOL processed;

    /* touch unused vars to get rid of warning */
    _wde_touch( mask );
    _wde_touch( lParam );

    processed = FALSE;

    if( message == WM_COMMAND && GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
        switch( LOWORD( wParam ) ) {
        case IDB_ES_LEFT:
            if( IsDlgButtonChecked( hDlg, IDB_ES_LEFT ) ) {
                CheckDlgButton( hDlg, IDB_ES_RIGHT, 0 );
                CheckDlgButton( hDlg, IDB_ES_CENTER, 0 );
            }
            processed = TRUE;
            break;

        case IDB_ES_CENTER:
            if( IsDlgButtonChecked( hDlg, IDB_ES_CENTER ) ) {
                CheckDlgButton( hDlg, IDB_ES_LEFT, 0 );
                CheckDlgButton( hDlg, IDB_ES_RIGHT, 0 );
            }
            processed = TRUE;
            break;

        case IDB_ES_UPPERCASE:
            if( IsDlgButtonChecked( hDlg, IDB_ES_UPPERCASE ) ) {
                CheckDlgButton( hDlg, IDB_ES_LOWERCASE, 0 );
            }
            processed = TRUE;
            break;

        case IDB_ES_LOWERCASE:
            if( IsDlgButtonChecked( hDlg, IDB_ES_LOWERCASE ) ) {
                CheckDlgButton( hDlg, IDB_ES_UPPERCASE, 0 );
            }
            processed = TRUE;
            break;

        case IDB_ES_RIGHT:
            if( IsDlgButtonChecked( hDlg, IDB_ES_RIGHT ) ) {
                CheckDlgButton( hDlg, IDB_ES_LEFT, 0 );
                CheckDlgButton( hDlg, IDB_ES_CENTER, 0 );
            }
            processed = TRUE;
            break;

        case IDB_ES_MULTILINE:
            if( IsDlgButtonChecked( hDlg, IDB_ES_MULTILINE ) ) {
                EnableWindow( GetDlgItem( hDlg, IDB_ES_CENTER ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_RIGHT ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_AUTOVSCROLL ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_WS_HSCROLL ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDB_WS_VSCROLL ), TRUE );
            } else {
                CheckDlgButton( hDlg, IDB_ES_AUTOVSCROLL, 0 );
                CheckDlgButton( hDlg, IDB_WS_VSCROLL, 0 );
                CheckDlgButton( hDlg, IDB_WS_HSCROLL, 0 );
                CheckDlgButton( hDlg, IDB_ES_CENTER, 0 );
                CheckDlgButton( hDlg, IDB_ES_RIGHT, 0 );
                CheckDlgButton( hDlg, IDB_ES_LEFT, 1 );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_AUTOVSCROLL ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_WS_HSCROLL ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_WS_VSCROLL ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_CENTER ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDB_ES_RIGHT ), FALSE );
            }
            processed = TRUE;
            break;
        }
    }

    return( processed );
}

WINEXPORT LRESULT CALLBACK WdeEditSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalEditProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

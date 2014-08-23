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
#include "wdedebug.h"
#include "wdedefsz.h"
#include "wdeoinfo.h"
#include "wde_rc.h"
#include "wdecctl.h"
#include "wdefscrl.h"


/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeScrollObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL    CALLBACK WdeScrollDispatcher( ACTION, WdeScrollObject *, void *, void * );
WINEXPORT LRESULT CALLBACK WdeScrollSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeScroll( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeScrollCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeScrollDestroy( WdeScrollObject *, BOOL *, void * );
static BOOL     WdeScrollResize( WdeScrollObject *, RECT *, BOOL * );
static BOOL     WdeScrollMove( WdeScrollObject *, POINT *, BOOL * );
static BOOL     WdeScrollValidateAction( WdeScrollObject *, ACTION *, void * );
static BOOL     WdeScrollCopyObject( WdeScrollObject *, WdeScrollObject **, WdeScrollObject * );
static BOOL     WdeScrollIdentify( WdeScrollObject *, OBJ_ID *, void * );
static BOOL     WdeScrollGetWndProc( WdeScrollObject *, WNDPROC *, void * );
static BOOL     WdeScrollGetWindowClass( WdeScrollObject *, char **, void * );
static BOOL     WdeScrollDefine( WdeScrollObject *, POINT *, void * );
static void     WdeScrollSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeScrollGetDefineInfo( WdeDefineObjectInfo *, HWND );
static BOOL     WdeScrollDefineHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeScrollDispatch;
static WdeDialogBoxControl      *WdeDefaultScroll = NULL;
static int                      WdeScrollWndExtra;
static WNDPROC                  WdeOriginalScrollProc;
//static WNDPROC                WdeScrollProc;

static DISPATCH_ITEM WdeScrollActions[] = {
    { DESTROY,          (DISPATCH_RTN *)WdeScrollDestroy        },
    { MOVE,             (DISPATCH_RTN *)WdeScrollMove           },
    { RESIZE,           (DISPATCH_RTN *)WdeScrollResize         },
    { COPY,             (DISPATCH_RTN *)WdeScrollCopyObject     },
    { VALIDATE_ACTION,  (DISPATCH_RTN *)WdeScrollValidateAction },
    { IDENTIFY,         (DISPATCH_RTN *)WdeScrollIdentify       },
    { GET_WINDOW_CLASS, (DISPATCH_RTN *)WdeScrollGetWindowClass },
    { DEFINE,           (DISPATCH_RTN *)WdeScrollDefine         },
    { GET_WND_PROC,     (DISPATCH_RTN *)WdeScrollGetWndProc     }
};

#define MAX_ACTIONS     (sizeof( WdeScrollActions ) / sizeof( DISPATCH_ITEM ))

WINEXPORT OBJPTR CALLBACK WdeHScrollCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeScroll( parent, obj_rect, handle, SBS_HORZ, "", HSCROLL_OBJ ) );
    } else {
        return( WdeScrollCreate( parent, obj_rect, NULL, HSCROLL_OBJ,
                                 (WdeDialogBoxControl *)handle ) );
    }
}

WINEXPORT OBJPTR CALLBACK WdeVScrollCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeScroll( parent, obj_rect, handle, SBS_VERT, "", VSCROLL_OBJ ) );
    } else {
        return( WdeScrollCreate( parent, obj_rect, NULL, VSCROLL_OBJ,
                                 (WdeDialogBoxControl *)handle ) );
    }
}

WINEXPORT OBJPTR CALLBACK WdeSizeBoxCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeScroll( parent, obj_rect, handle, SBS_SIZEBOX, "", SIZEBOX_OBJ ) );
    } else {
        return( WdeScrollCreate( parent, obj_rect, NULL, SIZEBOX_OBJ,
                                 (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeScroll( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                      DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_VISIBLE | WS_CHILD;
    SETCTL_STYLE( WdeDefaultScroll, style );
    SETCTL_TEXT( WdeDefaultScroll, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultScroll, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeScrollCreate( parent, obj_rect, handle, id, WdeDefaultScroll );

    WRMemFree( GETCTL_TEXT( WdeDefaultScroll ) );
    SETCTL_TEXT( WdeDefaultScroll, NULL );

    return( new );
}

OBJPTR WdeScrollCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                        OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeScrollObject *new;

    WdeDebugCreate( "Scroll", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeScrollCreate: Scroll has no parent!" );
        return( NULL );
    }

    new = (WdeScrollObject *)WRMemAlloc( sizeof( WdeScrollObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeScrollCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeScrollDispatch;
    new->object_id = id;
    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeScrollCreate: CONTROL_OBJ not created!" );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeScrollCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeScrollCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WRMemFree( new );
        return( NULL );
    }

    return( new );
}

WINEXPORT BOOL CALLBACK WdeScrollDispatcher( ACTION act, WdeScrollObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Scroll", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeScrollActions[i].id == act ) {
            return( WdeScrollActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

bool WdeScrollInit( bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, "SCROLLBAR", &wc );
    WdeOriginalScrollProc = wc.lpfnWndProc;
    WdeScrollWndExtra = wc.cbWndExtra;

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
        wc.lpszClassName = "wdescrollbar";
        wc.cbWndExtra += sizeof( OBJPTR );
        //wc.lpfnWndProc = WdeScrollSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeScrollInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultScroll = WdeAllocDialogBoxControl();
    if( WdeDefaultScroll == NULL ) {
        WdeWriteTrail( "WdeScrollInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultScroll, 0 );
    SETCTL_ID( WdeDefaultScroll, 0 );
    SETCTL_EXTRABYTES( WdeDefaultScroll, 0 );
    SETCTL_SIZEX( WdeDefaultScroll, 0 );
    SETCTL_SIZEY( WdeDefaultScroll, 0 );
    SETCTL_SIZEW( WdeDefaultScroll, 0 );
    SETCTL_SIZEH( WdeDefaultScroll, 0 );
    SETCTL_TEXT( WdeDefaultScroll, NULL );
    SETCTL_CLASSID( WdeDefaultScroll, ResNumToControlClass( CLASS_SCROLLBAR ) );

    WdeScrollDispatch = MakeProcInstance( (FARPROC)WdeScrollDispatcher, WdeGetAppInstance() );
    return( TRUE );
}

void WdeScrollFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultScroll );
    FreeProcInstance( WdeScrollDispatch );
}

BOOL WdeScrollDestroy( WdeScrollObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeScrollDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WRMemFree( obj );

    return( TRUE );
}

BOOL WdeScrollResize( WdeScrollObject *obj, RECT *new_pos, BOOL *flag )
{
    WdeOrderMode        mode;

    if( Forward( (OBJPTR)obj, GET_ORDER_MODE, &mode, NULL ) && mode != WdeSelect ) {
        return( FALSE );
    }

    if( !Forward( obj->control, RESIZE, new_pos, flag ) ) {
        WdeWriteTrail( "WdeScrollResize: control RESIZE failed!" );
        return( FALSE );
    }

    if( !Forward( obj->object_handle, DESTROY_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeScrollResize: DESTROY_WINDOW failed!" );
        return( FALSE );
    }

    if( !Forward( obj->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeScrollResize: CREATE_WINDOW failed!" );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeScrollMove( WdeScrollObject *obj, POINT *offset, BOOL *forms_called )
{
    if( !Forward( obj->control, MOVE, offset, forms_called ) ) {
        WdeWriteTrail( "WdeScrollResize: control MOVE failed!" );
        return( FALSE );
    }

    if( !Forward( obj->object_handle, DESTROY_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeScrollResize: DESTROY_WINDOW failed!" );
        return( FALSE );
    }

    if( !Forward( obj->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeScrollResize: CREATE_WINDOW failed!" );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeScrollValidateAction( WdeScrollObject *obj, ACTION *act, void *p2 )
{
    int                 i;
    WdeOrderMode        mode;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( *act == MOVE || *act == RESIZE ) {
        if( Forward( (OBJPTR)obj, GET_ORDER_MODE, &mode, NULL ) && mode != WdeSelect ) {
            return( FALSE );
        }
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeScrollActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR) obj->control, *act, p2 ) );
}

BOOL WdeScrollCopyObject( WdeScrollObject *obj, WdeScrollObject **new,
                          WdeScrollObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeScrollCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeScrollObject *)WRMemAlloc( sizeof( WdeScrollObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeScrollCopyObject: Object malloc failed" );
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
        WdeWriteTrail( "WdeScrollCopyObject: Control not created!" );
        WRMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeScrollIdentify( WdeScrollObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeScrollGetWndProc( WdeScrollObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeScrollSuperClassProc;

    return( TRUE );
}

BOOL WdeScrollGetWindowClass( WdeScrollObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = "scrollbar";

    return( TRUE );
}

BOOL WdeScrollDefine( WdeScrollObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP;
    o_info.set_func = (WdeSetProc)WdeScrollSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeScrollGetDefineInfo;
    o_info.hook_func = WdeScrollDefineHook;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeScrollSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000ffff;

    if( id  == HSCROLL_OBJ ) {
        if( mask & SBS_VERT ) {
            WdeWriteTrail( "WdeScrollSetDefineInfo: Bad HScroll mask!" );
        }

        if( mask & SBS_TOPALIGN ) {
            CheckDlgButton( hDlg, IDB_SBS_TOPALIGN, BST_CHECKED );
        } else if( mask & SBS_BOTTOMALIGN ) {
            CheckDlgButton( hDlg, IDB_SBS_BOTTOMALIGN, BST_CHECKED );
        }
    } else if( id == VSCROLL_OBJ ) {
        if ( !(mask & SBS_VERT) ) {
            WdeWriteTrail( "WdeScrollSetDefineInfo: Bad VScroll mask!" );
        }

        if( mask & SBS_LEFTALIGN ) {
            CheckDlgButton( hDlg, IDB_SBS_LEFTALIGN, BST_CHECKED );
        } else if( mask & SBS_RIGHTALIGN ) {
            CheckDlgButton( hDlg, IDB_SBS_RIGHTALIGN, BST_CHECKED );
        }
    } else if ( id == SIZEBOX_OBJ ) {
        if( !(mask & SBS_SIZEBOX) ) {
            WdeWriteTrail( "WdeScrollSetDefineInfo: Bad SizeBox mask!" );
        }

        if( mask & SBS_SIZEBOXTOPLEFTALIGN ) {
            CheckDlgButton( hDlg, IDB_SBS_SIZEBOXTOPLEFTALIGN, BST_CHECKED );
        } else if( mask & SBS_SIZEBOXBOTTOMRIGHTALIGN ) {
            CheckDlgButton( hDlg, IDB_SBS_SIZEBOXBOTTOMRIGHTALIGN, BST_CHECKED );
        }

#if __NT__XX
        EnableWindow( GetDlgItem( hDlg, IDB_SBS_SIZEGRIP ), TRUE );
        if( mask & SBS_SIZEGRIP ) {
            CheckDlgButton( hDlg, IDB_SBS_SIZEGRIP, BST_CHECKED );
        }
#else
        EnableWindow( GetDlgItem( hDlg, IDB_SBS_SIZEGRIP ), FALSE );
#endif
    } else {
        WdeWriteTrail( "WdeScrollSetDefineInfo: Bad OBJ_ID!" );
    }

#if __NT__XX
    // do the extended style stuff - its the same for HScroll,
    // VScroll and SizeBox
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_CLIENTEDGE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_STATICEDGE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_NOPARENTNOTIFY ), TRUE );

    mask = GETCTL_EXSTYLE( o_info->info.c.info );
    if( mask & WS_EX_CLIENTEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_CLIENTEDGE, BST_CHECKED );
    }
    if( mask & WS_EX_STATICEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_STATICEDGE, BST_CHECKED );
    }
    if( mask & WS_EX_NOPARENTNOTIFY ) {
        CheckDlgButton( hDlg, IDB_WS_EX_NOPARENTNOTIFY, BST_CHECKED );
    }
#else
    // disable the extended styles
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_CLIENTEDGE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_STATICEDGE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_NOPARENTNOTIFY ), FALSE );
#endif
}

void WdeScrollGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = 0;

    if( id == HSCROLL_OBJ ) {
        mask = SBS_HORZ;

        if( IsDlgButtonChecked( hDlg, IDB_SBS_TOPALIGN ) ) {
            mask |= SBS_TOPALIGN;
        } else if( IsDlgButtonChecked( hDlg, IDB_SBS_BOTTOMALIGN ) ) {
            mask |= SBS_BOTTOMALIGN;
        }
    } else if( id == VSCROLL_OBJ ) {
        mask = SBS_VERT;

        if( IsDlgButtonChecked( hDlg, IDB_SBS_LEFTALIGN ) ) {
            mask |= SBS_LEFTALIGN;
        } else if( IsDlgButtonChecked( hDlg, IDB_SBS_RIGHTALIGN ) ) {
            mask |= SBS_RIGHTALIGN;
        }
    } else if( id == SIZEBOX_OBJ ) {
        mask = SBS_SIZEBOX;

        if( IsDlgButtonChecked( hDlg, IDB_SBS_SIZEBOXTOPLEFTALIGN ) ) {
            mask |= SBS_SIZEBOXTOPLEFTALIGN;
        } else if( IsDlgButtonChecked( hDlg, IDB_SBS_SIZEBOXBOTTOMRIGHTALIGN ) ) {
            mask |= SBS_SIZEBOXBOTTOMRIGHTALIGN;
        }

#if __NT__XX
        if( IsDlgButtonChecked( hDlg, IDB_SBS_SIZEGRIP ) ) {
            mask |= SBS_SIZEGRIP;
        }
#endif
    } else {
        WdeWriteTrail( "WdeButtonGetDefineInfo: Bad OBJ_ID!" );
        return;
    }

    SETCTL_STYLE( o_info->info.c.info,
                  (GETCTL_STYLE( o_info->info.c.info ) & 0xffff0000) | mask );

#if __NT__XX
    // set the extended mask - same for HScroll,
    // VScroll and SizeBox
    mask = 0;
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_CLIENTEDGE ) ) {
        mask |= WS_EX_CLIENTEDGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_STATICEDGE ) ) {
        mask |= WS_EX_STATICEDGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_NOPARENTNOTIFY ) ) {
        mask |= WS_EX_NOPARENTNOTIFY;
    }
    SETCTL_EXSTYLE( o_info->info.c.info, mask );
#endif
}

BOOL WdeScrollDefineHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    BOOL    processed;
    WORD    wp;

    /* touch unused vars to get rid of warning */
    _wde_touch( mask );
    _wde_touch( lParam );

    processed = FALSE;

    if( message == WM_COMMAND && GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
        wp = LOWORD( wParam );
        switch( wp ) {
        case IDB_SBS_TOPALIGN:
            if( IsDlgButtonChecked( hDlg, wp ) ) {
                CheckDlgButton( hDlg, IDB_SBS_BOTTOMALIGN, BST_UNCHECKED );
            }
            processed = TRUE;
            break;

        case IDB_SBS_LEFTALIGN:
            if( IsDlgButtonChecked( hDlg, wp ) ) {
                CheckDlgButton( hDlg, IDB_SBS_RIGHTALIGN, BST_UNCHECKED );
            }
            processed = TRUE;
            break;

        case IDB_SBS_BOTTOMALIGN:
            if( IsDlgButtonChecked( hDlg, wp ) ) {
                CheckDlgButton( hDlg, IDB_SBS_TOPALIGN, BST_UNCHECKED );
            }
            processed = TRUE;
            break;

        case IDB_SBS_SIZEBOXBOTTOMRIGHTALIGN:
            if( IsDlgButtonChecked( hDlg, wp ) ) {
                CheckDlgButton( hDlg, IDB_SBS_SIZEBOXTOPLEFTALIGN, BST_UNCHECKED );
            }
            processed = TRUE;
            break;

        case IDB_SBS_SIZEBOXTOPLEFTALIGN:
            if( IsDlgButtonChecked( hDlg, wp ) ) {
                CheckDlgButton( hDlg, IDB_SBS_SIZEBOXBOTTOMRIGHTALIGN, BST_UNCHECKED );
            }
            processed = TRUE;
            break;

        case IDB_SBS_RIGHTALIGN:
            if( IsDlgButtonChecked( hDlg, wp ) ) {
                CheckDlgButton( hDlg, IDB_SBS_LEFTALIGN, BST_UNCHECKED );
            }
            processed = TRUE;
            break;
        }
    }

    return( processed );
}

WINEXPORT LRESULT CALLBACK WdeScrollSuperClassProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalScrollProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

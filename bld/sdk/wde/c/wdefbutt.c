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

#include "wdeglbl.h"
#include "wdemem.h"
#include "wderesiz.h"
#include "wderesin.h"
#include "wdeobjid.h"
#include "wdemain.h"
#include "wdedefin.h"
#include "wdedebug.h"
#include "wdeoinfo.h"
#include "wdefutil.h"
#include "wdedefsz.h"
#include "wdetxtsz.h"
#include "wde_rc.h"
#include "wde_wres.h"
#include "wdecctl.h"
#include "wdefbutt.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_GBOX_BORDER_SIZE WDE_BORDER_SIZE

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeButtonObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL    WINEXPORT WdeButtonDispatcher( ACTION, WdeButtonObject *, void *, void * );
extern LRESULT WINEXPORT WdeButtonSuperClassProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeButton( OBJPTR, RECT *, OBJPTR, DialogStyle, char *, OBJ_ID );
static OBJPTR   WdeButtonCreate( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl * );
static BOOL     WdeButtonDestroy( WdeButtonObject *, BOOL *, void * );
static BOOL     WdeButtonValidateAction( WdeButtonObject *, ACTION *, void * );
static BOOL     WdeButtonCopyObject( WdeButtonObject *, WdeButtonObject **, WdeButtonObject * );
static BOOL     WdeButtonIdentify( WdeButtonObject *, OBJ_ID *, void * );
static BOOL     WdeButtonGetWndProc( WdeButtonObject *, WNDPROC *, void * );
static BOOL     WdeButtonGetWindowClass( WdeButtonObject *, char **, void * );
static BOOL     WdeButtonDefine( WdeButtonObject *, POINT *, void * );
static void     WdeButtonSetDefineInfo( WdeDefineObjectInfo *, HWND );
static void     WdeButtonGetDefineInfo( WdeDefineObjectInfo *, HWND );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeButtonDispatch;
static WdeDialogBoxControl      *WdeDefaultButton = NULL;
static int                      WdeButtonWndExtra;
static WNDPROC                  WdeOriginalButtonProc;
//static WNDPROC                WdeButtonProc;

static DISPATCH_ITEM WdeButtonActions[] = {
    { DESTROY,          (BOOL (*)( OBJPTR, void *, void * ))WdeButtonDestroy            },
    { COPY,             (BOOL (*)( OBJPTR, void *, void * ))WdeButtonCopyObject         },
    { VALIDATE_ACTION,  (BOOL (*)( OBJPTR, void *, void * ))WdeButtonValidateAction     },
    { IDENTIFY,         (BOOL (*)( OBJPTR, void *, void * ))WdeButtonIdentify           },
    { GET_WINDOW_CLASS, (BOOL (*)( OBJPTR, void *, void * ))WdeButtonGetWindowClass     },
    { DEFINE,           (BOOL (*)( OBJPTR, void *, void * ))WdeButtonDefine             },
    { GET_WND_PROC,     (BOOL (*)( OBJPTR, void *, void * ))WdeButtonGetWndProc         }
};

#define MAX_ACTIONS      (sizeof( WdeButtonActions ) / sizeof( DISPATCH_ITEM ))

OBJPTR WINEXPORT WdePButtonCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeButton( parent, obj_rect, handle,
                               BS_PUSHBUTTON, "Push", PBUTTON_OBJ ) );
    } else {
        return( WdeButtonCreate( parent, obj_rect, NULL, PBUTTON_OBJ,
                                 (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WINEXPORT WdeCButtonCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeButton( parent, obj_rect, handle,
                               BS_AUTOCHECKBOX, "Check", CBUTTON_OBJ ) );
    } else {
        return( WdeButtonCreate( parent, obj_rect, NULL, CBUTTON_OBJ,
                                 (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WINEXPORT WdeRButtonCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeButton( parent, obj_rect, handle,
                               BS_AUTORADIOBUTTON, "Radio", RBUTTON_OBJ ) );
    } else {
        return( WdeButtonCreate( parent, obj_rect, NULL, RBUTTON_OBJ,
                                 (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WINEXPORT WdeGButtonCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    if( handle == NULL ) {
        return( WdeMakeButton( parent, obj_rect, handle,
                               BS_GROUPBOX, "Group", GBUTTON_OBJ ) );
    } else {
        return( WdeButtonCreate( parent, obj_rect, NULL, GBUTTON_OBJ,
                                 (WdeDialogBoxControl *)handle ) );
    }
}

OBJPTR WdeMakeButton( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                      DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_VISIBLE | WS_CHILD;
    if( id == PBUTTON_OBJ || id == CBUTTON_OBJ ) {
        style |= WS_TABSTOP;
    }
    SETCTL_STYLE( WdeDefaultButton, style );
    SETCTL_TEXT( WdeDefaultButton, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultButton, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect( parent, id, obj_rect );

    new = WdeButtonCreate( parent, obj_rect, handle, id, WdeDefaultButton );

    WdeMemFree( GETCTL_TEXT( WdeDefaultButton ) );
    SETCTL_TEXT( WdeDefaultButton, NULL );

    return( new );
}

OBJPTR WdeButtonCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                        OBJ_ID id, WdeDialogBoxControl *info )
{
    WdeButtonObject *new;
    Bool            b;

    WdeDebugCreate( "Button", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeButtonCreate: Button has no parent!" );
        return( NULL );
    }

    new = (WdeButtonObject *)WdeMemAlloc( sizeof( WdeButtonObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeButtonCreate: Object malloc failed" );
        return( NULL );
    }

    new->dispatcher = WdeButtonDispatch;

    new->object_id = id;

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeButtonCreate: CONTROL_OBJ not created!" );
        WdeMemFree( new );
        return( NULL );
    }

    if( id == GBUTTON_OBJ ) {
        b = TRUE;
        if( !Forward( new->object_handle, SET_CLEAR_INT, &b, NULL ) ) {
            WdeWriteTrail( "WdeButtonCreate: SET_CLEAR_INT failed!" );
            Destroy( new->control, FALSE );
            WdeMemFree( new );
            return( NULL );
        }
    }

    if( !Forward( new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeButtonCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeButtonCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    return( new );
}

BOOL WINEXPORT WdeButtonDispatcher( ACTION act, WdeButtonObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Button", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeButtonActions[i].id == act ) {
            return( WdeButtonActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeButtonInit( Bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, "BUTTON", &wc );
    WdeOriginalButtonProc = wc.lpfnWndProc;
    WdeButtonWndExtra = wc.cbWndExtra;

    if( first ) {
#if 0
        if ( wc.style & CS_GLOBALCLASS ) {
            wc.style ^= CS_GLOBALCLASS;
        }
        if ( wc.style & CS_PARENTDC ) {
            wc.style ^= CS_PARENTDC;
        }
        wc.style |= CS_HREDRAW | CS_VREDRAW;
        wc.hInstance = WdeApplicationInstance;
        wc.lpszClassName = "wdebutton";
        wc.cbWndExtra += sizeof( OBJPTR );
        //wc.lpfnWndProc = WdeButtonSuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail( "WdeButtonInit: RegisterClass failed." );
        }
#endif
    }

    WdeDefaultButton = WdeAllocDialogBoxControl();
    if( WdeDefaultButton == NULL ) {
        WdeWriteTrail( "WdeButtonInit: Alloc of control failed!" );
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultButton, 0 );
    SETCTL_ID( WdeDefaultButton, 0 );
    SETCTL_EXTRABYTES( WdeDefaultButton, 0 );
    SETCTL_SIZEX( WdeDefaultButton, 0 );
    SETCTL_SIZEY( WdeDefaultButton, 0 );
    SETCTL_SIZEW( WdeDefaultButton, 0 );
    SETCTL_SIZEH( WdeDefaultButton, 0 );
    SETCTL_TEXT( WdeDefaultButton, NULL );
    SETCTL_CLASSID( WdeDefaultButton, ResNumToControlClass( CLASS_BUTTON ) );

    WdeButtonDispatch = MakeProcInstance( (FARPROC)WdeButtonDispatcher,
                                          WdeGetAppInstance() );
    return( TRUE );
}

void WdeButtonFini( void )
{
    WdeFreeDialogBoxControl( &WdeDefaultButton );
    FreeProcInstance( WdeButtonDispatch );
}

BOOL WdeButtonDestroy( WdeButtonObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeButtonDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    WdeMemFree( obj );

    return( TRUE );
}

static BOOL WdeValidateGroupBoxMove( WdeButtonObject *obj, POINT *pnt )
{
    RECT                obj_rect;
    RECT                text_rect;
    SIZE                size;
    OBJPTR              parent;
    WdeResizeRatio      resizer;
    WdeDialogBoxControl *info;
    WdeOrderMode        mode;

    if( Forward( (OBJPTR)obj, GET_ORDER_MODE, &mode, NULL ) && mode != WdeSelect ) {
        return( FALSE );
    }

    Location( (OBJPTR)obj, &obj_rect );
    GetObjectParent( (OBJPTR)obj, &parent );

    if( Forward( obj->control, GET_OBJECT_INFO, &info, NULL ) && parent != NULL &&
        Forward( parent, GET_RESIZER, &resizer, NULL ) ) {
        if( WdeGetNameOrOrdSize( parent, GETCTL_TEXT( info ), &size ) ) {
            text_rect.left = obj_rect.left + 8;
            text_rect.top = obj_rect.top + (resizer.xmap * 3) / 4;
            text_rect.right = text_rect.left + size.cx;
            text_rect.bottom = text_rect.top + size.cy;
        } else {
            SetRectEmpty( &text_rect );
        }
    } else {
        SetRectEmpty( &text_rect );
    }

    if( PtInRect( &text_rect, *pnt ) ) {
        return( TRUE );
    }

    obj_rect.left += WDE_GBOX_BORDER_SIZE;
    obj_rect.right -= WDE_GBOX_BORDER_SIZE;
    obj_rect.top += WDE_GBOX_BORDER_SIZE;
    obj_rect.bottom -= WDE_GBOX_BORDER_SIZE;

    if( PtInRect( &obj_rect, *pnt ) ) {
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeButtonValidateAction( WdeButtonObject *obj, ACTION *act, void *p2 )
{
    int     i;

    if( *act == MOVE && obj->object_id == GBUTTON_OBJ ) {
        return( WdeValidateGroupBoxMove( obj, p2 ) );
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeButtonActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeButtonCopyObject( WdeButtonObject *obj, WdeButtonObject **new,
                          WdeButtonObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeButtonCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeButtonObject *)WdeMemAlloc( sizeof( WdeButtonObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeButtonCopyObject: Object malloc failed" );
        return( FALSE );
    }

    (*new)->dispatcher = obj->dispatcher;
    (*new)->object_id = obj->object_id;

    if( handle == NULL ) {
        (*new)->object_handle = *new;
    } else {
        (*new)->object_handle = handle;
    }

    if( !CopyObject( obj->control, &(*new)->control, (*new)->object_handle) ) {
        WdeWriteTrail( "WdeButtonCopyObject: Control not created!" );
        WdeMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeButtonIdentify( WdeButtonObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeButtonGetWndProc( WdeButtonObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeButtonSuperClassProc;

    return( TRUE );
}

BOOL WdeButtonGetWindowClass( WdeButtonObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    //*class = "wdebutton";
    *class = "button";

    return( TRUE );
}

BOOL WdeButtonDefine( WdeButtonObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    o_info.obj = obj->object_handle;
    o_info.obj_id = obj->object_id;
    o_info.mask = WS_VISIBLE | WS_DISABLED | WS_TABSTOP | WS_GROUP;
    o_info.set_func = (WdeSetProc)WdeButtonSetDefineInfo;
    o_info.get_func = (WdeGetProc)WdeButtonGetDefineInfo;
    o_info.hook_func = NULL;
    o_info.win = NULL;

    return( WdeControlDefine( &o_info ) );
}

void WdeButtonSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    BOOL        check_lefttext;
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;

    mask = GETCTL_STYLE( o_info->info.c.info ) & (0x0000ffff ^ BS_LEFTTEXT);

    check_lefttext = FALSE;

    if( id == PBUTTON_OBJ ) {
#if __NT__XX
        EnableWindow( GetDlgItem( hDlg, IDB_BS_BITMAP ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_ICON ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_TEXT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_TOP ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_BOTTOM ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_CENTER ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_RIGHT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_VCENTER ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_MULTILINE ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_NOTIFY ), TRUE );
        mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000000f;
        if( mask == BS_OWNERDRAW ) {
            CheckDlgButton( hDlg, IDB_BS_OWNERDRAW, 1 );
        } else {
            if( mask == BS_PUSHBUTTON ) {
                CheckDlgButton( hDlg, IDB_BS_PUSHBUTTON, 1 );
            } else if( mask == BS_DEFPUSHBUTTON ) {
                CheckDlgButton( hDlg, IDB_BS_DEFPUSHBUTTON, 1 );
            } else {
                WdeWriteTrail( "WdeButtonSetDefineInfo: Bad Push mask!" );
            }

            // set any new styles
            mask = GETCTL_STYLE( o_info->info.c.info ) & 0x00000f00;
            if( (mask & BS_CENTER) == BS_CENTER ) {
                CheckDlgButton( hDlg, IDB_BS_CENTER, 1 );
            } else if( mask & BS_RIGHT ) {
                CheckDlgButton( hDlg, IDB_BS_RIGHT, 1 );
            } else if( mask & BS_LEFT ) {
                CheckDlgButton( hDlg, IDB_BS_LEFT, 1 );
            } else {
                CheckDlgButton( hDlg, IDB_BS_CENTER, 1 );
            }

            if( (mask & BS_VCENTER) == BS_VCENTER ) {
                CheckDlgButton( hDlg, IDB_BS_VCENTER, 1 );
            } else if( mask & BS_TOP ) {
                CheckDlgButton( hDlg, IDB_BS_TOP, 1 );
            } else if( mask & BS_BOTTOM ) {
                CheckDlgButton( hDlg, IDB_BS_BOTTOM, 1 );
            } else {
                CheckDlgButton( hDlg, IDB_BS_VCENTER, 1 );
            }

            if( GETCTL_STYLE( o_info->info.c.info ) & BS_BITMAP ) {
                CheckDlgButton( hDlg, IDB_BS_BITMAP, 1 );
            }
            if( GETCTL_STYLE( o_info->info.c.info ) & BS_ICON ) {
                CheckDlgButton( hDlg, IDB_BS_ICON, 1 );
            }
            if( GETCTL_STYLE( o_info->info.c.info ) & BS_TEXT ) {
                CheckDlgButton( hDlg, IDB_BS_TEXT, 1 );
            }
            if( GETCTL_STYLE( o_info->info.c.info ) & BS_MULTILINE ) {
                CheckDlgButton( hDlg, IDB_BS_MULTILINE, 1 );
            }
            if( GETCTL_STYLE( o_info->info.c.info ) & BS_NOTIFY ) {
                CheckDlgButton( hDlg, IDB_BS_NOTIFY, 1 );
            }
        }
#else
        EnableWindow( GetDlgItem( hDlg, IDB_BS_BITMAP ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_ICON ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_TEXT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_TOP ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_BOTTOM ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_CENTER ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_RIGHT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_VCENTER ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_MULTILINE ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_NOTIFY ), FALSE );
        if( mask == BS_PUSHBUTTON ) {
            CheckDlgButton( hDlg, IDB_BS_PUSHBUTTON, 1 );
        } else if( mask == BS_DEFPUSHBUTTON ) {
            CheckDlgButton( hDlg, IDB_BS_DEFPUSHBUTTON, 1 );
        } else if( mask == BS_OWNERDRAW ) {
            CheckDlgButton( hDlg, IDB_BS_OWNERDRAW, 1 );
        } else {
            WdeWriteTrail( "WdeButtonSetDefineInfo: Bad Push mask!" );
        }
#endif
    } else if( id  == CBUTTON_OBJ ) {
        check_lefttext = TRUE;

#if __NT__XX
        EnableWindow( GetDlgItem( hDlg, IDB_BS_NOTIFY ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_PUSHLIKE ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFTTEXT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_RIGHT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_CENTER ), TRUE );

        mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000000f;
        if( mask == BS_3STATE ) {
            CheckDlgButton( hDlg, IDB_BS_3STATE, 1 );
        } else if( mask == BS_AUTOCHECKBOX ) {
            CheckDlgButton( hDlg, IDB_BS_AUTOCHECKBOX, 1 );
        } else if( mask == BS_AUTO3STATE ) {
            CheckDlgButton( hDlg, IDB_BS_3STATE, 1 );
            CheckDlgButton( hDlg, IDB_BS_AUTOCHECKBOX, 1 );
        }

        // set the new styles
        mask = GETCTL_STYLE( o_info->info.c.info ) & 0x00000f00;
        if( (mask & BS_CENTER) == BS_CENTER ) {
            CheckDlgButton( hDlg, IDB_BS_CENTER, 1 );
        } else if( mask & BS_RIGHT ) {
            CheckDlgButton( hDlg, IDB_BS_RIGHT, 1 );
        } else {
            CheckDlgButton( hDlg, IDB_BS_LEFT, 1 );
        }

        if( GETCTL_STYLE( o_info->info.c.info ) & BS_NOTIFY ) {
            CheckDlgButton( hDlg, IDB_BS_NOTIFY, 1 );
        }
        if( GETCTL_STYLE( o_info->info.c.info ) & BS_PUSHLIKE ) {
            CheckDlgButton( hDlg, IDB_BS_PUSHLIKE, 1 );
        }
        if( GETCTL_STYLE( o_info->info.c.info ) & BS_LEFTTEXT ) {
            CheckDlgButton( hDlg, IDB_BS_LEFTTEXT, 1 );
        }
#else
        EnableWindow( GetDlgItem( hDlg, IDB_BS_NOTIFY ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_PUSHLIKE ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFTTEXT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_RIGHT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_CENTER ), FALSE );

        if( mask == BS_3STATE ) {
            CheckDlgButton( hDlg, IDB_BS_3STATE, 1 );
        } else if( mask == BS_AUTOCHECKBOX ) {
            CheckDlgButton( hDlg, IDB_BS_AUTOCHECKBOX, 1 );
        } else if( mask == BS_AUTO3STATE ) {
            CheckDlgButton( hDlg, IDB_BS_3STATE, 1 );
            CheckDlgButton( hDlg, IDB_BS_AUTOCHECKBOX, 1 );
        }
#endif
    } else if( id  == RBUTTON_OBJ ) {
        check_lefttext = TRUE;

#if __NT__XX
        EnableWindow( GetDlgItem( hDlg, IDB_BS_NOTIFY ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFTTEXT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_PUSHLIKE ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_AUTORADIOBUTTON ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_RIGHT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_CENTER ), TRUE );

        mask = GETCTL_STYLE( o_info->info.c.info ) & 0x0000000f;
        if( mask == BS_AUTORADIOBUTTON ) {
            CheckDlgButton( hDlg, IDB_BS_AUTORADIOBUTTON, 1 );
        }

        // set new styles
        mask = GETCTL_STYLE( o_info->info.c.info ) & 0x00000f00;
        if( (mask & BS_CENTER) == BS_CENTER ) {
            CheckDlgButton( hDlg, IDB_BS_CENTER, 1 );
        } else if( mask & BS_RIGHT ) {
            CheckDlgButton( hDlg, IDB_BS_RIGHT, 1 );
        } else {
            CheckDlgButton( hDlg, IDB_BS_LEFT, 1 );
        }

        if( GETCTL_STYLE( o_info->info.c.info ) & BS_NOTIFY ) {
            CheckDlgButton( hDlg, IDB_BS_NOTIFY, 1 );
        }
        if( GETCTL_STYLE( o_info->info.c.info ) & BS_LEFTTEXT ) {
            CheckDlgButton( hDlg, IDB_BS_LEFTTEXT, 1 );
        }
        if( GETCTL_STYLE( o_info->info.c.info ) & BS_PUSHLIKE ) {
            CheckDlgButton( hDlg, IDB_BS_PUSHLIKE, 1 );
        }
#else
        EnableWindow( GetDlgItem( hDlg, IDB_BS_NOTIFY ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFTTEXT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_PUSHLIKE ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_AUTORADIOBUTTON ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFT), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_RIGHT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_CENTER ), FALSE );

        if( mask == BS_AUTORADIOBUTTON ) {
            CheckDlgButton( hDlg, IDB_BS_AUTORADIOBUTTON, 1 );
        }
#endif
    } else if( id == GBUTTON_OBJ ) {
#if __NT__XX
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_RIGHT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_CENTER ), TRUE );
        mask = GETCTL_STYLE( o_info->info.c.info ) & 0x00000f00;
        if( (mask & BS_CENTER) == BS_CENTER ) {
            CheckDlgButton( hDlg, IDB_BS_CENTER, 1 );
        } else if( mask & BS_RIGHT ) {
            CheckDlgButton( hDlg, IDB_BS_RIGHT, 1 );
        } else {
            CheckDlgButton( hDlg, IDB_BS_LEFT, 1 );
        }
#else
        EnableWindow( GetDlgItem( hDlg, IDB_BS_LEFT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_RIGHT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDB_BS_CENTER ), FALSE );
        if( mask != BS_GROUPBOX ) {
            WdeWriteTrail( "WdeButtonSetDefineInfo: Bad GroupBox mask!" );
        }
#endif
    } else {
        WdeWriteTrail( "WdeButtonSetDefineInfo: Bad OBJ_ID!" );
    }

    if( check_lefttext ) {
       if( GETCTL_STYLE( o_info->info.c.info ) & BS_LEFTTEXT ) {
           CheckDlgButton( hDlg, IDB_BS_LEFTTEXT, 1 );
       }
    }

    // Do the extended style stuff - it's the same for PushButton,
    // RadioButton, CheckBox and GroupBox.
    WdeEXSetDefineInfo( o_info, hDlg );
}

void WdeButtonGetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    BOOL        check_lefttext;
    BOOL        auto_checked;
    OBJ_ID      id;
    DialogStyle mask;

    id = o_info->obj_id;
    mask = 0;
    check_lefttext = FALSE;
    auto_checked = FALSE;

    if( id == PBUTTON_OBJ ) {

#if __NT__XX
        if( IsDlgButtonChecked( hDlg, IDB_BS_OWNERDRAW ) ) {
            mask = BS_OWNERDRAW;
        } else {
            if( IsDlgButtonChecked( hDlg, IDB_BS_DEFPUSHBUTTON ) ) {
                mask = BS_DEFPUSHBUTTON;
            } else if( IsDlgButtonChecked( hDlg, IDB_BS_PUSHBUTTON ) ) {
                mask = BS_PUSHBUTTON;
            } else {
                WdeWriteTrail( "WdeButtonGetDefineInfo: Bad Push style!" );
                return;
            }

            // check for new styles that may be set
            if( IsDlgButtonChecked( hDlg, IDB_BS_BITMAP ) ) {
                mask |= BS_BITMAP;
            }
            if( IsDlgButtonChecked( hDlg, IDB_BS_ICON ) ) {
                mask |= BS_ICON;
            }
            if( IsDlgButtonChecked( hDlg, IDB_BS_TEXT ) ) {
                mask |= BS_TEXT;
            }
            if( IsDlgButtonChecked( hDlg, IDB_BS_MULTILINE ) ) {
                mask |= BS_MULTILINE;
            }
            if( IsDlgButtonChecked( hDlg, IDB_BS_NOTIFY ) ) {
                mask |= BS_NOTIFY;
            }

            if( IsDlgButtonChecked( hDlg, IDB_BS_TOP ) ) {
                mask |= BS_TOP;
            } else if( IsDlgButtonChecked( hDlg, IDB_BS_BOTTOM ) ) {
                mask |= BS_BOTTOM;
            } else {
                mask |= BS_VCENTER;
            }

            if( IsDlgButtonChecked( hDlg, IDB_BS_RIGHT ) ) {
                mask |= BS_RIGHT;
            } else if( IsDlgButtonChecked( hDlg, IDB_BS_LEFT ) ) {
                mask |= BS_LEFT;
            } else {
                mask |= BS_CENTER;
            }
        }
#else
        if( IsDlgButtonChecked( hDlg, IDB_BS_PUSHBUTTON ) ) {
            mask = BS_PUSHBUTTON;
        } else if( IsDlgButtonChecked( hDlg, IDB_BS_DEFPUSHBUTTON ) ) {
            mask = BS_DEFPUSHBUTTON;
        } else if( IsDlgButtonChecked( hDlg, IDB_BS_OWNERDRAW ) ) {
            mask = BS_OWNERDRAW;
        } else {
            WdeWriteTrail( "WdeButtonGetDefineInfo: Bad Push style!" );
            return;
        }
#endif

    } else if( id == CBUTTON_OBJ ) {

        check_lefttext = TRUE;

        auto_checked = IsDlgButtonChecked( hDlg, IDB_BS_AUTOCHECKBOX );

        if( IsDlgButtonChecked( hDlg, IDB_BS_3STATE ) ) {
            if( auto_checked ) {
                mask = BS_AUTO3STATE;
            } else {
                mask = BS_3STATE;
            }
        } else {
            if( auto_checked ) {
                mask = BS_AUTOCHECKBOX;
            } else {
                mask = BS_CHECKBOX;
            }
        }
#if __NT__XX
        // set the new styles
        if( IsDlgButtonChecked( hDlg, IDB_BS_NOTIFY ) ) {
            mask |= BS_NOTIFY;
        }
        if( IsDlgButtonChecked( hDlg, IDB_BS_PUSHLIKE ) ) {
            mask |= BS_PUSHLIKE;
        }
        if( IsDlgButtonChecked( hDlg, IDB_BS_LEFTTEXT ) ) {
            mask |= BS_LEFTTEXT;
        }

        if( IsDlgButtonChecked( hDlg, IDB_BS_RIGHT ) ) {
            mask |= BS_RIGHT;
        } else if( IsDlgButtonChecked( hDlg, IDB_BS_CENTER ) ) {
            mask |= BS_CENTER;
        } else {
            mask |= BS_LEFT;
        }
#endif

    } else if( id == RBUTTON_OBJ ) {

        check_lefttext = TRUE;

        auto_checked = IsDlgButtonChecked( hDlg, IDB_BS_AUTORADIOBUTTON );

        if( auto_checked ) {
            mask = BS_AUTORADIOBUTTON;
        } else {
            mask = BS_RADIOBUTTON;
        }
#if __NT__XX
        if( IsDlgButtonChecked( hDlg, IDB_BS_NOTIFY ) ) {
            mask |= BS_NOTIFY;
        }
        if( IsDlgButtonChecked( hDlg, IDB_BS_PUSHLIKE ) ) {
            mask |= BS_PUSHLIKE;
        }
        if( IsDlgButtonChecked( hDlg, IDB_BS_LEFTTEXT ) ) {
            mask |= BS_LEFTTEXT;
        }

        if( IsDlgButtonChecked( hDlg, IDB_BS_RIGHT ) ) {
            mask |= BS_RIGHT;
        } else if( IsDlgButtonChecked( hDlg, IDB_BS_CENTER ) ) {
            mask |= BS_CENTER;
        } else {
            mask |= BS_LEFT;
        }
#endif

    } else if( id == GBUTTON_OBJ ) {

        mask = BS_GROUPBOX;

#if __NT__XX
        if( IsDlgButtonChecked( hDlg, IDB_BS_RIGHT ) ) {
            mask |= BS_RIGHT;
        } else if( IsDlgButtonChecked( hDlg, IDB_BS_CENTER ) ) {
            mask |= BS_CENTER;
        } else {
            mask |= BS_LEFT;
        }
#endif

    } else {
        WdeWriteTrail( "WdeButtonGetDefineInfo: Bad OBJ_ID!" );
        return;
    }

    if( check_lefttext ) {
        if( IsDlgButtonChecked( hDlg, IDB_BS_LEFTTEXT ) ) {
            mask |= BS_LEFTTEXT;
        }
    }

    SETCTL_STYLE( o_info->info.c.info,
                  (GETCTL_STYLE( o_info->info.c.info ) & 0xffff0000) | mask );

    // Set the extended mask - same for PushButton, RadioButton
    // CheckBox and GroupBox.
    WdeEXGetDefineInfo( o_info, hDlg );
}

LRESULT WINEXPORT WdeButtonSuperClassProc( HWND hWnd, UINT message, WPARAM wParam,
                                           volatile LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalButtonProc, hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}

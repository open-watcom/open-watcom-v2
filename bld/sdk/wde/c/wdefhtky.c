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


#include <windows.h>
#include <win1632.h>

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
#include "wdefhtky.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC     dispatcher;
    OBJPTR      object_handle;
    OBJ_ID      object_id;
    OBJPTR      control;
} WdeHtKyObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL    WINEXPORT WdeHtKyDispatcher  ( ACTION, WdeHtKyObject *, void *,
                                              void *);
extern LRESULT WINEXPORT WdeHtKySuperClassProc ( HWND, UINT, WPARAM, LPARAM);

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static OBJPTR   WdeMakeHtKy             ( OBJPTR, RECT *, OBJPTR, DialogStyle,
                                          char *, OBJ_ID );
static OBJPTR   WdeHKCreate             ( OBJPTR, RECT *, OBJPTR,
                                          OBJ_ID, WdeDialogBoxControl *);
static BOOL     WdeHtKyDestroy          ( WdeHtKyObject *, BOOL *, void *);
static BOOL     WdeHtKyValidateAction   ( WdeHtKyObject *, ACTION *, void *);
static BOOL     WdeHtKyCopyObject       ( WdeHtKyObject *, WdeHtKyObject **,
                                          WdeHtKyObject *);
static BOOL     WdeHtKyIdentify         ( WdeHtKyObject *, OBJ_ID *, void *);
static BOOL     WdeHtKyGetWndProc       ( WdeHtKyObject *, WNDPROC *, void *);
static BOOL     WdeHtKyGetWindowClass   ( WdeHtKyObject *, char **, void *);
static BOOL     WdeHtKyDefine           ( WdeHtKyObject *, POINT *, void *);
static void     WdeHtKySetDefineInfo    ( WdeDefineObjectInfo *, HWND );
static void     WdeHtKyGetDefineInfo    ( WdeDefineObjectInfo *, HWND );
static BOOL     WdeHtKyDefineHook       ( HWND, WORD, WPARAM, LPARAM,
                                          DialogStyle );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeHtKyDispatch;
static WdeDialogBoxControl      *WdeDefaultHtKy = NULL;
static int                      WdeHtKyWndExtra;
static WNDPROC                  WdeOriginalHtKyProc;
//static WNDPROC                        WdeHtKyProc;

#define WHOTKEY_CLASS    HOTKEY_CLASS

static DISPATCH_ITEM WdeHtKyActions[] = {
    { DESTROY           ,  WdeHtKyDestroy               }
,   { COPY              ,  WdeHtKyCopyObject            }
,   { VALIDATE_ACTION   ,  WdeHtKyValidateAction        }
,   { IDENTIFY          ,  WdeHtKyIdentify              }
,   { GET_WINDOW_CLASS  ,  WdeHtKyGetWindowClass        }
,   { DEFINE            ,  WdeHtKyDefine                }
,   { GET_WND_PROC      ,  WdeHtKyGetWndProc            }
};

#define MAX_ACTIONS      (sizeof(WdeHtKyActions)/sizeof (DISPATCH_ITEM))

OBJPTR WINEXPORT WdeHtKyCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle)
{
    if( handle == NULL ) {
        return( WdeMakeHtKy( parent, obj_rect, handle,
                              0, "", HOTKEY_OBJ ) );
    } else {
        return( WdeHKCreate( parent, obj_rect, NULL, HOTKEY_OBJ,
                             (WdeDialogBoxControl *) handle) );
    }
}

OBJPTR WdeMakeHtKy( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    DialogStyle style, char *text, OBJ_ID id )
{
    OBJPTR new;

    style |= WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD;

    SETCTL_STYLE( WdeDefaultHtKy, style );
    SETCTL_TEXT( WdeDefaultHtKy, ResStrToNameOrOrd( text ) );
    SETCTL_ID( WdeDefaultHtKy, WdeGetNextControlID() );

    WdeChangeSizeToDefIfSmallRect ( parent, id, obj_rect );

    new = WdeHKCreate ( parent, obj_rect, handle, id, WdeDefaultHtKy );

    WdeMemFree( GETCTL_TEXT(WdeDefaultHtKy) );
    SETCTL_TEXT( WdeDefaultHtKy, NULL );

    return ( new );
}

OBJPTR WdeHKCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                    OBJ_ID id, WdeDialogBoxControl *info)
{
    WdeHtKyObject *new;

    WdeDebugCreate("HtKy", parent, obj_rect, handle);

    if ( parent == NULL ) {
        WdeWriteTrail("WdeHtKyCreate: HtKy has no parent!");
        return ( NULL );
    }

    new = (WdeHtKyObject *) WdeMemAlloc ( sizeof(WdeHtKyObject) );
    if ( new == NULL ) {
        WdeWriteTrail("WdeHtKyCreate: Object malloc failed");
        return ( NULL );
    }

    new->dispatcher = WdeHtKyDispatch;

    new->object_id = id;

    if ( handle ==  NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle);

    if (new->control == NULL) {
        WdeWriteTrail("WdeHtKyCreate: CONTROL_OBJ not created!");
        WdeMemFree ( new );
        return ( NULL );
    }

    if (!Forward ( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL) ) {
        WdeWriteTrail("WdeHtKyCreate: SET_OBJECT_INFO failed!");
        Destroy ( new->control, FALSE );
        WdeMemFree ( new );
        return ( NULL );
    }

    if (!Forward ( (OBJPTR)new->object_handle, CREATE_WINDOW,
                   NULL, NULL) ) {
        WdeWriteTrail("WdeHtKyCreate: CREATE_WINDOW failed!");
        Destroy ( new->control, FALSE );
        WdeMemFree ( new );
        return ( NULL );
    }

    return ( new );
}

BOOL WINEXPORT WdeHtKyDispatcher ( ACTION act, WdeHtKyObject *obj,
                                     void *p1, void *p2)
{
    int     i;

    WdeDebugDispatch("HtKy", act, obj, p1, p2);

    for ( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeHtKyActions[i].id == act ) {
            return( (WdeHtKyActions[i].rtn)( obj, p1, p2 ) );
        }
    }

    return (Forward ((OBJPTR)obj->control, act, p1, p2));
}

Bool WdeHtKyInit( Bool first )
{
    WNDCLASS    wc;

    WdeApplicationInstance = WdeGetAppInstance();
    GetClassInfo( (HINSTANCE)NULL, WHOTKEY_CLASS, &wc );
    WdeOriginalHtKyProc = wc.lpfnWndProc;
    WdeHtKyWndExtra = wc.cbWndExtra;

    if( first ) {
#if 0
        if ( wc.style & CS_GLOBALCLASS ) {
            wc.style ^= CS_GLOBALCLASS;
        }
        if ( wc.style & CS_PARENTDC ) {
            wc.style ^= CS_PARENTDC;
        }
        wc.style |= ( CS_HREDRAW | CS_VREDRAW );
        wc.hInstance     = WdeApplicationInstance;
        wc.lpszClassName = "wdeedit";
        wc.cbWndExtra  += sizeof( OBJPTR );
        //wc.lpfnWndProc      = WdeHtKySuperClassProc;
        if( !RegisterClass( &wc ) ) {
            WdeWriteTrail("WdeHtKyInit: RegisterClass failed.");
        }
#endif
    }

    WdeDefaultHtKy = WdeAllocDialogBoxControl ();
    if( !WdeDefaultHtKy ) {
        WdeWriteTrail ("WdeHtKyInit: Alloc of control failed!");
        return( FALSE );
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
    SETCTL_CLASSID( WdeDefaultHtKy, WdeStrDup( WHOTKEY_CLASS ) );

    WdeHtKyDispatch = MakeProcInstance((FARPROC)WdeHtKyDispatcher,
                                           WdeGetAppInstance());
    return( TRUE );
}

void WdeHtKyFini ( void )
{
    WdeFreeDialogBoxControl ( &WdeDefaultHtKy );
    FreeProcInstance        ( WdeHtKyDispatch );
}

BOOL WdeHtKyDestroy ( WdeHtKyObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    if ( !Forward ( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail("WdeHtKyDestroy: Control DESTROY failed");
        return ( FALSE );
    }

    WdeMemFree( obj );

    return ( TRUE );
}

BOOL WdeHtKyValidateAction ( WdeHtKyObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    for ( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeHtKyActions[i].id == *act ) {
            return ( TRUE );
        }
    }

    return ( ValidateAction( (OBJPTR) obj->control, *act, p2 ) );
}

BOOL WdeHtKyCopyObject ( WdeHtKyObject *obj, WdeHtKyObject **new,
                           WdeHtKyObject *handle )
{
    if (new == NULL) {
        WdeWriteTrail("WdeHtKyCopyObject: Invalid new object!");
        return ( FALSE );
    }

    *new = (WdeHtKyObject *) WdeMemAlloc ( sizeof(WdeHtKyObject) );

    if ( *new == NULL ) {
        WdeWriteTrail("WdeHtKyCopyObject: Object malloc failed");
        return ( FALSE );
    }

    (*new)->dispatcher    = obj->dispatcher;
    (*new)->object_id     = obj->object_id;

    if ( handle ==  NULL ) {
        (*new)->object_handle = *new;
    } else {
        (*new)->object_handle = handle;
    }

    if (!CopyObject(obj->control, &((*new)->control), (*new)->object_handle)) {
        WdeWriteTrail("WdeHtKyCopyObject: Control not created!");
        WdeMemFree ( (*new) );
        return ( FALSE );
    }

    return ( TRUE );
}

BOOL WdeHtKyIdentify ( WdeHtKyObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    *id = obj->object_id;

    return ( TRUE );
}

BOOL WdeHtKyGetWndProc( WdeHtKyObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(obj);
    _wde_touch(p2);

    *proc = WdeHtKySuperClassProc;

    return ( TRUE );
}

BOOL WdeHtKyGetWindowClass ( WdeHtKyObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(obj);
    _wde_touch(p2);

    *class = WHOTKEY_CLASS;

    return ( TRUE );
}

BOOL WdeHtKyDefine ( WdeHtKyObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch(pnt);
    _wde_touch(p2);

    o_info.obj       = obj->object_handle;
    o_info.obj_id    = obj->object_id;
    o_info.mask      = WS_VISIBLE | WS_DISABLED |
                        WS_TABSTOP | WS_GROUP | WS_BORDER;
    o_info.set_func  = WdeHtKySetDefineInfo;
    o_info.get_func  = WdeHtKyGetDefineInfo;
    o_info.hook_func = WdeHtKyDefineHook;
    o_info.win       = NULL;

    return ( WdeControlDefine ( &o_info ) );
}

void WdeHtKySetDefineInfo ( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    // there are no styles for the hotkey control
    // set the extended style controls only
    WdeEXSetDefineInfo( o_info, hDlg );
}

void WdeHtKyGetDefineInfo ( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    // clear the style flags, since there are none for a progress bar
    SETCTL_STYLE( o_info->info.c.info,
                  ( GETCTL_STYLE(o_info->info.c.info) & 0xffff0000 ) );

    // get the extended control settings
    WdeEXGetDefineInfo ( o_info, hDlg );
}

BOOL WdeHtKyDefineHook( HWND hDlg, WORD message,
                        WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(hDlg);
    _wde_touch(message);
    _wde_touch(wParam);
    _wde_touch(lParam);
    _wde_touch(mask);

    return( FALSE );
}

LRESULT WINEXPORT WdeHtKySuperClassProc( HWND hWnd, UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
    if( !WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( CallWindowProc( WdeOriginalHtKyProc,
                                 hWnd, message, wParam, lParam ) );
    }
    return( FALSE );
}



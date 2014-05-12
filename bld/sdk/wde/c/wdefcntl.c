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
#include "wdeactn.h"
#include "wde_wres.h"
#include "wderes.h"
#include "wdeinfo.h"
#include "wdelist.h"
#include "wdesdup.h"
#include "wdefmenu.h"
#include "wdefdiag.h"
#include "wdectool.h"
#include "wdefont.h"
#include "wdemain.h"
#include "wdecust.h"
#include "wdefutil.h"
#include "wdetxtsz.h"
#include "wdedefsz.h"
#include "wdefordr.h"
#include "wdezordr.h"
#include "wdectl3d.h"
#include "wdeopts.h"
#include "wdedebug.h"
#include "wde_rc.h"
#include "wdefcntl.h"
#include "wdefbase.h"
#include "windlg.h"
#include "windlg32.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_CONTROL_MIN_WIDTH  1
#define WDE_CONTROL_MIN_HEIGHT 1
#define WDE_SIZE_TO_TEXT_PAD   2
#define WDE_CKECK_RADIO_ADJUST 16

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC             dispatcher;
    HWND                window_handle;
    HWND                parent_handle;
    OBJPTR              object_handle;
    OBJPTR              parent;
    OBJPTR              o_item;
    bool                sizeable;
    bool                clear_interior;
    HFONT               font;
    char                *window_class;
    char                *symbol;
    char                *helpsymbol;
    WdeDialogBoxControl *control_info;
    WdeResInfo          *res_info;
    OBJPTR              base_obj;
    HWND                tag;
    WdeOrderMode        mode;
    WNDPROC             old_proc;
} WdeControlObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL CALLBACK WdeControlDispatcher( ACTION, WdeControlObject *, void *, void * );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static BOOL WdeControlTest( WdeControlObject *, GLOBALHANDLE *, void * );
static BOOL WdeControlTestEX( WdeControlObject *, GLOBALHANDLE *, void * );
static BOOL WdeControlDraw( WdeControlObject *, RECT *, HDC * );
static BOOL WdeControlResolveSymbol( WdeControlObject *, bool *, bool * );
static BOOL WdeControlResolveHelpSymbol( WdeControlObject *, bool *, bool * );
static BOOL WdeControlOnTop( WdeControlObject *, void *, void * );
static BOOL WdeControlCreateWindow( WdeControlObject *, void *, void * );
static BOOL WdeControlDestroyWindow( WdeControlObject *, bool *, bool * );
static BOOL WdeControlShowWindow( WdeControlObject *, bool *, void * );
static BOOL WdeControlIsMarkValid( WdeControlObject *, BOOL *, void * );
static BOOL WdeControlDestroy( WdeControlObject *, BOOL *, void * );
static BOOL WdeControlSetFont( WdeControlObject *, HFONT *, WdeResizeRatio * );
static BOOL WdeControlGetResizeInfo( WdeControlObject *, RESIZE_ID *, void * );
static BOOL WdeControlValidateAction( WdeControlObject *, ACTION *, void * );
static BOOL WdeControlGetWindowHandle( WdeControlObject *, HWND *, void * );
static BOOL WdeControlResize( WdeControlObject *, RECT *, BOOL * );
static BOOL WdeControlMove( WdeControlObject *, POINT *, BOOL * );
static BOOL WdeControlNotify( WdeControlObject *, NOTE_ID*, void * );
static BOOL WdeControlFirstChild( WdeControlObject *, void *, void * );
static BOOL WdeControlPasteObject( WdeControlObject *, OBJPTR, POINT * );
static BOOL WdeControlCutObject( WdeControlObject *, WdeControlObject **, void * );
static BOOL WdeControlCopyObject( WdeControlObject *, WdeControlObject **, WdeControlObject * );
static BOOL WdeControlGetObjectInfo( WdeControlObject *, WdeDialogBoxControl **, char ** );
static BOOL WdeControlSetObjectInfo( WdeControlObject *, WdeDialogBoxControl *, char * );
static BOOL WdeControlSetObjectHelpInfo( WdeControlObject *, WdeDialogBoxControl *, char * );
static BOOL WdeControlGetObjectHelpInfo( WdeControlObject *, WdeDialogBoxControl **, char ** );
static BOOL WdeControlSetClearInt( WdeControlObject *, BOOL *, void * );
static BOOL WdeControlGetClearInt( WdeControlObject *, BOOL *, void * );
static BOOL WdeChangeControlSize( WdeControlObject *, BOOL, BOOL );
static BOOL WdeOffsetDialogUnits( WdeControlObject *, WdeResizeRatio * );
static void WdeBringControlToTop( WdeControlObject * );
static void WdeSetClearObjectPos( WdeControlObject * );
static void WdeFreeControlObject( WdeControlObject * );
static void WdeWriteControlToInfo( WdeControlObject * );
static BOOL WdeControlModifyInfo( WdeControlObject *, WdeInfoStruct *, void * );
static BOOL WdeControlSetOrderMode( WdeControlObject *, WdeOrderMode *, WdeSetOrderLists ** );
static BOOL WdeControlGetOrderMode( WdeControlObject *, WdeOrderMode *, void * );
static BOOL WdeControlSizeToText( WdeControlObject *, void *, void * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static FARPROC           WdeControlDispatch;
static HINSTANCE         WdeAppInst;

static DISPATCH_ITEM WdeControlActions[] = {
    { MOVE,                 (DISPATCH_RTN *)WdeControlMove              },
    { NOTIFY,               (DISPATCH_RTN *)WdeControlNotify            },
    { RESIZE,               (DISPATCH_RTN *)WdeControlResize            },
    { DRAW,                 (DISPATCH_RTN *)WdeControlDraw              },
    { ADD_SUBOBJECT,        (DISPATCH_RTN *)NULL                        },
    { DESTROY,              (DISPATCH_RTN *)WdeControlDestroy           },
    { COPY,                 (DISPATCH_RTN *)WdeControlCopyObject        },
    { CUT,                  (DISPATCH_RTN *)WdeControlCutObject         },
    { PASTE,                (DISPATCH_RTN *)WdeControlPasteObject       },
    { VALIDATE_ACTION,      (DISPATCH_RTN *)WdeControlValidateAction    },
    { RESIZE_INFO,          (DISPATCH_RTN *)WdeControlGetResizeInfo     },
    { GET_WINDOW_HANDLE,    (DISPATCH_RTN *)WdeControlGetWindowHandle   },
    { CREATE_WINDOW,        (DISPATCH_RTN *)WdeControlCreateWindow      },
    { DESTROY_WINDOW,       (DISPATCH_RTN *)WdeControlDestroyWindow     },
    { SHOW_WIN,             (DISPATCH_RTN *)WdeControlShowWindow        },
    { GET_OBJECT_INFO,      (DISPATCH_RTN *)WdeControlGetObjectInfo     },
    { SET_OBJECT_INFO,      (DISPATCH_RTN *)WdeControlSetObjectInfo     },
    { GET_OBJECT_HELPINFO,  (DISPATCH_RTN *)WdeControlGetObjectHelpInfo },
    { SET_OBJECT_HELPINFO,  (DISPATCH_RTN *)WdeControlSetObjectHelpInfo },
    { SET_FONT,             (DISPATCH_RTN *)WdeControlSetFont           },
    { GET_FONT,             (DISPATCH_RTN *)NULL                        },
    { GET_NC_SIZE,          (DISPATCH_RTN *)NULL                        },
    { GET_NC_SIZE,          (DISPATCH_RTN *)NULL                        },
    { ON_TOP,               (DISPATCH_RTN *)WdeControlOnTop             },
    { TEST,                 (DISPATCH_RTN *)WdeControlTest              },
    { TESTEX,               (DISPATCH_RTN *)WdeControlTestEX            },
    { BECOME_FIRST_CHILD,   (DISPATCH_RTN *)WdeControlFirstChild        },
    { GET_RESIZE_INC,       (DISPATCH_RTN *)NULL                        },
    { GET_SCROLL_RECT,      (DISPATCH_RTN *)NULL                        },
    { SET_CLEAR_INT,        (DISPATCH_RTN *)WdeControlSetClearInt       },
    { IS_OBJECT_CLEAR,      (DISPATCH_RTN *)WdeControlGetClearInt       },
    { IS_MARK_VALID,        (DISPATCH_RTN *)WdeControlIsMarkValid       },
    { RESOLVE_SYMBOL,       (DISPATCH_RTN *)WdeControlResolveSymbol     },
    { RESOLVE_HELPSYMBOL,   (DISPATCH_RTN *)WdeControlResolveHelpSymbol },
    { MODIFY_INFO,          (DISPATCH_RTN *)WdeControlModifyInfo        },
    { SET_ORDER_MODE,       (DISPATCH_RTN *)WdeControlSetOrderMode      },
    { GET_ORDER_MODE,       (DISPATCH_RTN *)WdeControlGetOrderMode      },
    { SIZE_TO_TEXT,         (DISPATCH_RTN *)WdeControlSizeToText        },
    { GET_RESIZER,          (DISPATCH_RTN *)NULL                        }
};

#define MAX_ACTIONS      (sizeof( WdeControlActions ) / sizeof( DISPATCH_ITEM ))

void WdeControlModified ( WdeControlObject *obj )
{
    if( obj != NULL && obj->parent != NULL && obj->parent != obj->base_obj ) {
        WdeDialogModified( obj->parent );
    }
}

void WdeBringControlToTop( WdeControlObject *obj )
{
    WdeBringWindowToTop( obj->window_handle );
    //WdeSOP( obj, obj->parent );
}

void WdeSetClearObjectPos( WdeControlObject *obj )
{
    WdeBringWindowToTop( obj->window_handle );
    //WdeSOP( obj, obj->parent );
}

WINEXPORT OBJPTR CALLBACK WdeControlCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    OBJPTR              ancestor;
    OBJ_ID              id;
    WdeControlObject    *new;
    bool                ok;
    WdeOrderMode        mode;

    WdeDebugCreate( "Control", parent, obj_rect, handle );

    new = NULL;

    ok = ( parent != NULL );
    if( !ok ) {
        WdeWriteTrail( "WdeControlCreate: Control has no parent!" );
    }

    if( ok ) {
        ok = (!Forward( parent, GET_ORDER_MODE, &mode, NULL ) || mode == WdeSelect);
    }

    if( ok ) {
        ok = ( handle != NULL );
    }

    if( !ok ) {
        WdeWriteTrail( "WdeControlCreate: Control cant use its own handle!" );
    }

    if( ok ) {
        new = (WdeControlObject *)WRMemAlloc( sizeof( WdeControlObject ) );
        ok = ( new != NULL );
        if( ok ) {
            memset( new, 0, sizeof( WdeControlObject ) );
        } else {
            WdeWriteTrail( "WdeControlCreate: Object malloc failed" );
        }
    }

    if( ok ) {
        /* make sure we are setting the correct parent */
        ancestor = parent;
        Forward( (OBJPTR)ancestor, IDENTIFY, &id, NULL );
        while( id != DIALOG_OBJ && id != BASE_OBJ ) {
            GetObjectParent( ancestor, &ancestor );
            Forward( (OBJPTR)ancestor, IDENTIFY, &id, NULL );
        }
        new->parent = ancestor;
        new->dispatcher = WdeControlDispatch;
        new->sizeable = TRUE;
        new->mode = WdeSelect;
        if( handle ) {
            new->object_handle = handle;
        } else {
            new->object_handle = new;
        }
        new->base_obj = GetMainObject();
        new->res_info = WdeGetCurrentRes();
        ok = ( new->base_obj != NULL && new->res_info != NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeControlCreate: Bad base_obj or res_info!" );
        }
    }

    if( ok ) {
        ok = Forward( new->parent, GET_WINDOW_HANDLE, &new->parent_handle, NULL );
    }

    if( !ok ) {
        WdeWriteTrail( "WdeControlCreate: Couldn't get parent window handle!" );
    }

    if( ok ) {
        ok = Forward( new->parent, GET_FONT, &new->font, NULL );
    }

    if( !ok ) {
        WdeWriteTrail( "WdeControlCreate: Couldn't get parent font!" );
    }

    if( ok ) {
        new->o_item = Create( OBJ_ITEM, new->parent, obj_rect, new->object_handle );

        ok = ( new->o_item != NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeControlCreate: OITEM not created!" );
        }
    }

    if( ok ) {
        ok = Forward( new->object_handle, GET_WINDOW_CLASS, &new->window_class, NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeControlCreate: Couldn't get window class!" );
        }
    }

    if( ok ) {
        WdeControlModified( new );
        if( !WdeGetStickyMode() ) {
            WdeSetBaseObject( IDM_SELECT_MODE );
        }
    } else {
        if( new ) {
            if( new->control_info ) {
                WdeFreeDialogBoxControl( &new->control_info );
            }
            WRMemFree( new );
            new = NULL;
        }
    }

    return( new );
}

WINEXPORT BOOL CALLBACK WdeControlDispatcher( ACTION act, WdeControlObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Control", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeControlActions[i].id == act ) {
            if( WdeControlActions[i].rtn ) {
                return( WdeControlActions[i].rtn( obj, p1, p2 ) );
            } else {
                return( Forward( obj->parent, act, p1, p2 ) );
            }
        }
    }

    return( Forward( (OBJPTR)obj->o_item, act, p1, p2 ) );
}

bool WdeControlInit( bool first )
{
    _wde_touch( first );
    WdeAppInst = WdeGetAppInstance();
    WdeControlDispatch = MakeProcInstance( (FARPROC)WdeControlDispatcher, WdeAppInst );
    return( TRUE );
}

void WdeControlFini( void )
{
    FreeProcInstance( WdeControlDispatch );
}

BOOL WdeControlTest( WdeControlObject *obj, GLOBALHANDLE *template, void *p2 )
{
    char        *Text;
    char        *Class;
    uint_32     style;
    uint_16     ID;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    Text = WdeResNameOrOrdinalToStr( GETCTL_TEXT( obj->control_info ), 10 );
    if( Text == NULL ) {
        Text = WdeStrDup( "" );
        if( Text == NULL ) {
            return( FALSE );
        }
    }

    Class = WdeControlClassToStr( GETCTL_CLASSID( obj->control_info ) );
    if( Class == NULL ) {
        Class = WdeStrDup( "" );
        if( Class == NULL ) {
            WRMemFree( Text );
            return( FALSE );
        }
    }

    style = GETCTL_STYLE( obj->control_info );

    ID = GETCTL_ID( obj->control_info );
    if( !stricmp( obj->window_class, "combobox" ) ) {
        style &= 0xffffffff ^ (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE);
    } else if( !stricmp( obj->window_class, "wde_borbtn" ) ) {
        if( !WdeIsBorBtnIDSupported( GETCTL_ID( obj->control_info ) ) ) {
            ID = WDE_PREVIEW_ID;
        }
    }

    *template = AddControl( *template,
                            GETCTL_SIZEX( obj->control_info ),
                            GETCTL_SIZEY( obj->control_info ),
                            GETCTL_SIZEW( obj->control_info ),
                            GETCTL_SIZEH( obj->control_info ),
                            ID, style, Class, Text, 0, NULL );

    WRMemFree( Text );
    WRMemFree( Class );

    return( *template != NULL );
}

BOOL WdeControlTestEX( WdeControlObject *obj, GLOBALHANDLE *template, void *p2 )
{
    char        *Text;
    char        *Class;
    uint_32     style;
    uint_16     ID;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    Text = WdeResNameOrOrdinalToStr( GETCTL_TEXT( obj->control_info ), 10 );
    if( Text == NULL ) {
        Text = WdeStrDup( "" );
        if( Text == NULL ) {
            return( FALSE );
        }
    }

    Class = WdeControlClassToStr( GETCTL_CLASSID( obj->control_info ) );
    if( Class == NULL ) {
        Class = WdeStrDup( "" );
        if( Class == NULL ) {
            WRMemFree( Text );
            return( FALSE );
        }
    }

    style = GETCTL_STYLE( obj->control_info );

    ID = GETCTL_ID( obj->control_info );
    if( !stricmp( obj->window_class, "combobox" ) ) {
        style &= 0xffffffff ^ (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE);
    } else if( !stricmp( obj->window_class, "wde_borbtn" ) ) {
        if( !WdeIsBorBtnIDSupported( GETCTL_ID( obj->control_info ) ) ) {
            ID = WDE_PREVIEW_ID;
        }
    }

    *template = AddControlEX( *template,
                              GETCTL_SIZEX( obj->control_info ),
                              GETCTL_SIZEY( obj->control_info ),
                              GETCTL_SIZEW( obj->control_info ),
                              GETCTL_SIZEH( obj->control_info ),
                              ID, style,
                              GETCTL_EXSTYLE( obj->control_info ),
                              GETCTL_HELPID( obj->control_info ),
                              Class, Text, 0, NULL );

    WRMemFree( Text );
    WRMemFree( Class );

    return( *template != NULL );
}

BOOL WdeControlIsMarkValid( WdeControlObject *obj, BOOL *flag, void *p2 )
{
    uint_32 s;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( obj->mode == WdeSelect && obj->window_handle != NULL ) {
        s = (uint_32)GetWindowLong( obj->window_handle, GWL_STYLE );
        *flag = ((s & WS_VISIBLE) != 0);
    } else {
        *flag = FALSE;
    }

    return( TRUE );
}

BOOL WdeControlDestroy( WdeControlObject *obj, BOOL *flag, void *p2 )
{
    RECT        rect;
    OBJPTR      next;
    bool        check_scroll;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    check_scroll = (obj->parent == obj->base_obj);

    if( *flag ) {
        if( obj->res_info ) {
            next = WdeGetNextObject( FALSE, obj->object_handle, obj->parent );
            if( next == NULL ) {
                next = obj->parent;
            }
            obj->res_info->next_current = next;
        }
    }

    Location( obj->object_handle, &rect );

    if( obj->parent != NULL &&
        !Forward( obj->parent, REMOVE_SUBOBJECT, obj->object_handle, NULL ) ) {
        WdeWriteTrail( "WdeControlDestroy: RemoveObject failed!" );
    }

    *flag = FALSE;

    if( !Forward( obj->o_item, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeControlDestroy: Failed to destroy OITEM!" );
        if( obj->parent ) {
            AddObject( obj->parent, obj->object_handle );
        }
        return( FALSE );
    }

    if( obj->window_handle != NULL ) {
        DestroyWindow( obj->window_handle );
    }

    if( obj->res_info != NULL ) {
        if( obj->res_info->next_current == obj->object_handle ) {
            obj->res_info->next_current = NULL;
        }
    }

    WdeFreeControlObject( obj );

    MarkInvalid( &rect );

    if( check_scroll ) {
        WdeCheckBaseScrollbars( FALSE );
    }

    return( TRUE );
}

void WdeFreeControlObject( WdeControlObject *obj )
{
    if( obj->symbol != NULL ) {
        WRMemFree( obj->symbol );
    }

    if( obj->helpsymbol != NULL ) {
        WRMemFree( obj->helpsymbol );
    }

    if( obj->control_info != NULL ) {
        WdeFreeDialogBoxControl( &obj->control_info );
    }

    WRMemFree( obj );
}

BOOL WdeChangeControlSize( WdeControlObject *obj, BOOL widths_only, BOOL snap_to_grid )
{
    RECT      t;
    RECT      size;
    RECT      obj_rect;
    POINT     origin;
    POINT     pt;

    Location( obj->object_handle, &obj_rect );

    if( snap_to_grid ) {
        pt.x = GETCTL_SIZEX( obj->control_info );
        pt.y = GETCTL_SIZEY( obj->control_info );
        WdeSnapPointToGrid( &pt );
        SETCTL_SIZEX( obj->control_info, pt.x );
        SETCTL_SIZEY( obj->control_info, pt.y );
    }

    if( obj->parent != obj->base_obj ) {
        GetOffset( &origin );
        size.left = (int_16)GETCTL_SIZEX( obj->control_info );
        size.right = size.left + (int_16)GETCTL_SIZEW( obj->control_info );
        size.top = (int_16)GETCTL_SIZEY( obj->control_info );
        size.bottom = size.top + (int_16)GETCTL_SIZEH( obj->control_info );
        MapDialogRect( obj->parent_handle, &size );
        WdeMapWindowRect( obj->parent_handle, obj->res_info->edit_win, &size );
        OffsetRect( &size, origin.x, origin.y );
    } else {
        WdeDialogToScreen( obj->object_handle, NULL,
                           GETCTL_PSIZE( obj->control_info ), &size );
    }

    if( widths_only ) {
        t = obj_rect;
        t.right = t.left + (size.right - size.left);
        t.bottom = t.top + (size.bottom - size.top);
        size = t;
    }

    if( !EqualRect( &size, &obj_rect ) ) {
        if( !Resize( obj->object_handle, &size, FALSE ) ) {
            WdeWriteTrail( "WdeChangeControlSize: RESIZE failed!" );
            return( FALSE );
        }
    }

    return( TRUE );
}

BOOL WdeControlSetFont( WdeControlObject *obj, HFONT *font, WdeResizeRatio *resizer )
{
    ControlClass        *control_class;
    ResNameOrOrdinal    *rname;
    char                *name;
    char                temp[10];

    /* touch unused vars to get rid of warning */
    _wde_touch( resizer );

    obj->font = *font;

    if( !Forward( obj->parent, GET_WINDOW_HANDLE, &obj->parent_handle, NULL) ) {
        WdeWriteTrail( "WdeControlSetFont: GET_WINDOW_HANDLE failed!" );
        return( FALSE );
    }

    if( !WdeChangeControlSize( obj, FALSE, FALSE ) ) {
        WdeWriteTrail( "WdeControlSetFont: WdeChangeControlSize failed!" );
        return( FALSE );
    }

    if( obj->window_handle != NULL ) {
        control_class = GETCTL_CLASSID( obj->control_info );
        if( control_class->Class == CLASS_STATIC &&
            (GETCTL_STYLE( obj->control_info ) & 0x0000000f) == SS_ICON ) {
            return( TRUE );
        }

        name = NULL;
        rname = GETCTL_TEXT( obj->control_info );
        if( rname->ord.fFlag == 0xff ) {
            ultoa( (uint_32)rname->ord.wOrdinalID, temp, 10 );
            name = temp;
        } else {
            if( rname->name[0] ) {
                name = rname->name;
            }
        }

        SendMessage( obj->window_handle, WM_SETFONT, (WPARAM)*font, (LPARAM)TRUE );

        SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)name );
    }

    return( TRUE );
}

BOOL WdeControlDraw( WdeControlObject *obj, RECT *area, HDC *dc )
{
    RECT    rect;
    RECT    trect;
    POINT   origin;
    UINT    flags;

    Location( obj->object_handle, &rect );

    GetOffset( &origin );

    if( dc != NULL && IntersectRect( &trect, area, &rect ) ) {
        OffsetRect( &trect, -origin.x, -origin.y );
        MapWindowPoints( obj->res_info->edit_win, obj->window_handle, (POINT *)&trect, 2 );
        RedrawWindow( obj->window_handle, (RECT *)&trect, (HRGN)NULL,
                      RDW_INTERNALPAINT | RDW_UPDATENOW );
    }

    OffsetRect( &rect, -origin.x, -origin.y );

    if( obj->parent == obj->base_obj ) {
        flags = SWP_NOZORDER | SWP_NOSIZE;
        if( dc == NULL ) {
            flags |= SWP_NOREDRAW | SWP_NOACTIVATE;
        }
        SetWindowPos( obj->window_handle, (HWND)NULL, rect.left, rect.top, 0, 0, flags );
    }

    return( TRUE );
}

BOOL WdeControlOnTop( WdeControlObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    WdeBringControlToTop( obj );

    return( TRUE );
}

BOOL WdeControlSetClearInt( WdeControlObject *obj, BOOL *b, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    obj->clear_interior = *b;

    return( TRUE );
}

BOOL WdeControlGetClearInt( WdeControlObject *obj, BOOL *b, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *b = obj->clear_interior;

    return( TRUE );
}

BOOL WdeControlCreateWindow( WdeControlObject *obj, void *p1, void *p2 )
{
    ResNameOrOrdinal    *rname;
    char                *name;
    char                *cname;
    void                *crt_params;
    char                temp[10];
    POINT               pnt;
    RECT                obj_rect;
    DialogStyle         style;
    uint_32             exstyle;
    BOOL                set_font;
    BOOL                set_crt_params;
    POINT               origin;
    uint_16             ID;
    OBJ_ID              oid;
    WNDPROC             new_proc;

    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    if( !Forward( obj->parent, GET_WINDOW_HANDLE, &obj->parent_handle, NULL ) ) {
        WdeWriteTrail( "WdeControlCreateWindow: GET_WINDOW_HANDLE failed!" );
        return( FALSE );
    }

    if( !Forward( obj->object_handle, IDENTIFY, &oid, NULL ) ) {
        WdeWriteTrail( "WdeControlCreateWindow: IDENTIFY failed!" );
        return( FALSE );
    }

    new_proc = (WNDPROC)NULL;
    if( !Forward( obj->object_handle, GET_WND_PROC, &new_proc, NULL ) ||
        new_proc == (WNDPROC)NULL ) {
        WdeWriteTrail( "WdeControlCreateWindow: Could not get new wnd proc!" );
        return( FALSE );
    }

    set_font = TRUE;
    set_crt_params = TRUE;

    Location( obj->object_handle, &obj_rect );

    GetOffset( &origin );

    pnt.x = obj_rect.left - origin.x;
    pnt.y = obj_rect.top - origin.y;

    MapWindowPoints( obj->res_info->edit_win, obj->parent_handle, &pnt, 1 );

    name = NULL;
    cname = NULL;

    if( oid == ICON_OBJ ) {
        set_font = FALSE;
        name = "DefaultIcon";
    }

    if( name == NULL ) {
        rname = GETCTL_TEXT( obj->control_info );
        if( rname != NULL ) {
            if( rname->ord.fFlag == 0xff ) {
                ultoa( rname->ord.wOrdinalID, temp, 10 );
                name = temp;
            } else {
                if( rname->name[0] ) {
                    name = rname->name;
                }
            }
        }
    }

    style = GETCTL_STYLE( obj->control_info ) &
        (0xffffffff ^ (WS_DISABLED | WS_MAXIMIZE | WS_MINIMIZE | WS_POPUP));

#ifdef __NT__XX
    exstyle = GETCTL_EXSTYLE( obj->control_info );
#else
    exstyle = 0;
#endif

    if( oid == COMBOBOX_OBJ ) {
        style = GETCTL_STYLE( obj->control_info ) &
            (0xffffffff ^ (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE));
    }

    ID = GETCTL_ID( obj->control_info );
    if( !stricmp( obj->window_class, "wde_borbtn" ) ) {
        if( !WdeIsBorBtnIDSupported( GETCTL_ID( obj->control_info ) ) ) {
            ID = WDE_PREVIEW_ID;
        }
        set_font = FALSE;
        set_crt_params = FALSE;
    }

    if( !set_font ) {
        cname = name;
    }

    if( set_crt_params ) {
        crt_params = (void *)obj->object_handle;
    } else {
        crt_params = NULL;
    }

#if 1
    style |= WS_VISIBLE;
#else
    if( oid == GBUTTON_OBJ ) {
        style |= WS_VISIBLE;
    } else {
        style |= WS_VISIBLE | WS_CLIPSIBLINGS;
    }
#endif

    obj->window_handle = CreateWindowEx( exstyle, obj->window_class, cname, style,
                                         pnt.x, pnt.y,
                                         obj_rect.right - obj_rect.left,
                                         obj_rect.bottom - obj_rect.top,
                                         obj->parent_handle, (HMENU)ID, WdeAppInst,
                                         crt_params );

    if( obj->window_handle == NULL ) {
        WdeWriteTrail( "WdeControlCreateWindow: Could not create window!" );
        return( FALSE );
    }

    // 3D subclass the control
    if( WdeGetOption( WdeOptUse3DEffects ) ) {
        WdeCtl3dSubclassCtl( obj->window_handle );
    }

    // subclass the control
    obj->old_proc = (WNDPROC)GET_WNDPROC( obj->window_handle );
    if( obj->old_proc == (WNDPROC)NULL ) {
        DestroyWindow( obj->window_handle );
        obj->window_handle = NULL;
        return( FALSE );
    }
    SET_WNDPROC( obj->window_handle, (LONG_PTR)new_proc );

    if( set_font ) {
        SendMessage( obj->window_handle, WM_SETFONT, (WPARAM)obj->font, (LPARAM)TRUE );

        SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)name );
    }

    WdeBringControlToTop( obj );

    ShowWindow( obj->window_handle, SW_SHOW );

    return( TRUE );
}

BOOL WdeControlDestroyWindow( WdeControlObject *obj, bool *quick, bool *destroy_children )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( destroy_children );

    if( !quick || (!*quick && (obj->window_handle != NULL)) ) {
        DestroyWindow( obj->window_handle );
    }

    obj->window_handle = NULL;
    obj->tag = NULL;

    return( TRUE );
}

BOOL WdeControlShowWindow( WdeControlObject *obj, bool *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    WdeShowObjectWindow( obj->window_handle, *flag );

    return( TRUE );
}

BOOL WdeControlGetResizeInfo( WdeControlObject *obj, RESIZE_ID *info, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    if( obj->sizeable && obj->mode == WdeSelect ) {
        *info = R_ALL;
    } else {
        *info = R_NONE;
    }

    return( TRUE );
}

static BOOL WdeControlValidateMove( WdeControlObject *obj, POINT *pnt, ACTION act )
{
    RECT        obj_rect;
    RECT        win_rect;
    OBJ_ID      id;
    BOOL        clear;

    if( act == MOVE && obj->mode != WdeSelect ) {
        return( FALSE );
    }

    id = BASE_OBJ;
    Forward( obj->object_handle, IDENTIFY, &id, NULL );

    if( id == COMBOBOX_OBJ ) {
        Location( (OBJPTR)obj, &obj_rect );
        GetWindowRect( obj->window_handle, &win_rect );
        obj_rect.bottom = obj_rect.top + win_rect.bottom - win_rect.top;
        return( PtInRect( &obj_rect, *pnt ) );
    }

    clear = obj->clear_interior;

    if( !clear && id == FRAME_OBJ ) {
        DialogStyle     mask;

        mask = GETCTL_STYLE( obj->control_info ) & 0x0000000F;

        if( mask != SS_BLACKRECT && mask != SS_GRAYRECT && mask != SS_WHITERECT ) {
            clear = TRUE;
        }
    }


    if( clear ) {
        Location( (OBJPTR)obj, &obj_rect );
        obj_rect.left += WDE_BORDER_SIZE;
        obj_rect.right -= WDE_BORDER_SIZE;
        obj_rect.top += WDE_BORDER_SIZE;
        obj_rect.bottom -= WDE_BORDER_SIZE;
        return( !PtInRect( &obj_rect, *pnt ) );
    }

    return( TRUE );
}

BOOL WdeControlValidateAction( WdeControlObject *obj, ACTION *act, void *p2 )
{
    int     i;

    if( *act == MOVE || *act == PICK ) {
        return( WdeControlValidateMove( obj, (POINT *)p2, *act ) );
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeControlActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->o_item, *act, p2 ) );
}

BOOL WdeControlGetWindowHandle( WdeControlObject *obj, HWND *hwin, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *hwin = obj->window_handle;

    return( TRUE );
}

BOOL WdeControlPasteObject( WdeControlObject *obj, OBJPTR parent, POINT *pnt )
{
    bool ok;

    obj->base_obj = GetMainObject();
    obj->res_info = WdeGetCurrentRes();

    ok = (obj->base_obj != NULL && obj->res_info != NULL);

    if( ok ) {
        if( pnt != NULL ) {
            ok = PasteObject( obj->o_item, parent, *pnt );
            if( !ok ) {
                WdeWriteTrail( "WdeControlPasteObject: OITEM PASTE failed!" );
            }
        } else {
            obj->parent = NULL;
            ok = Notify( obj->object_handle, NEW_PARENT, parent );
        }
    }

    if( ok && pnt != NULL ) {
        ok = WdeOffsetDialogUnits( obj, NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeControlPasteObject: WdeOffsetDialogUnits failed!" );
        }
    }

    if( ok ) {
        ok = WdeChangeControlSize( obj, FALSE, TRUE );
        if( !ok ) {
            WdeWriteTrail( "WdeControlPasteObject: WdeChangeControlSize failed!" );
        }
    }

    if( ok ) {
        ok = Forward( obj->object_handle, CREATE_WINDOW, NULL, NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeControlPasteObject: create window falied!" );
        }
    }

    if( ok ) {
        WdeControlModified( obj );
        if( obj->symbol != NULL ) {
            WdeAddSymbolToObjectHashTable( obj->res_info, obj->symbol,
                                           GETCTL_ID( obj->control_info ) );
        }
        WdeControlOnTop( obj, NULL, NULL );
    } else {
        obj->base_obj = NULL;
        obj->res_info = NULL;
    }

    return( ok );
}

BOOL WdeControlCopyObject( WdeControlObject *obj, WdeControlObject **new,
                           WdeControlObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeControlCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeControlObject *)WRMemAlloc( sizeof( WdeControlObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeControlCopyObject: Object malloc failed" );
        return( FALSE );
    }
    memset( *new, 0, sizeof( WdeControlObject ) );

    (*new)->dispatcher = obj->dispatcher;
    (*new)->window_class = obj->window_class;
    (*new)->font = obj->font;
    (*new)->sizeable = obj->sizeable;
    (*new)->clear_interior = obj->clear_interior;
    (*new)->control_info = WdeCopyDialogBoxControl( obj->control_info );
    (*new)->mode = WdeSelect;

    if( !(*new)->control_info ) {
        WdeWriteTrail( "WdeControlCopyObject: NULL info!" );
        WdeFreeControlObject( *new );
        return( FALSE );
    }

    SETCTL_ID( (*new)->control_info, WdeGetNextControlID() );

    if( obj->symbol ) {
        (*new)->symbol = WdeStrDup( obj->symbol );
        if( (*new)->symbol == NULL ) {
            WdeWriteTrail( "WdeControlCopyObject: Symbol alloc failed" );
            WdeFreeControlObject( *new );
            return( FALSE );
        }
    }

    /* JPK - added for help id support */
    if( obj->helpsymbol != NULL ) {
        (*new)->helpsymbol = WdeStrDup( obj->helpsymbol );
        if( (*new)->helpsymbol == NULL ) {
            WdeWriteTrail( "WdeControlCopyObject: Help symbol alloc failed" );
            WdeFreeControlObject( *new );
            return( FALSE );
        }
    }

    if( handle == NULL ) {
        (*new)->object_handle = *new;
    } else {
        (*new)->object_handle = handle;
    }

    if( !CopyObject( obj->o_item, &(*new)->o_item, (*new)->object_handle ) ) {
        WdeWriteTrail( "WdeControlCopyObject: OITEM not created!" );
        WdeFreeControlObject( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeControlCutObject( WdeControlObject *obj, WdeControlObject **new, void *p2 )
{
    NOTE_ID     note_id;
    bool        check_scroll;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *new = NULL;

    check_scroll = (obj->parent == obj->base_obj);

    if( !RemoveObject( obj->parent, obj->object_handle ) ) {
        WdeWriteTrail( "WdeControlCutObject: Remove from parent failed!" );
        return( FALSE );
    }

    note_id = NEW_PARENT;
    if( !WdeControlNotify( obj, &note_id, NULL ) ) {
        WdeWriteTrail( "WdeControlCutObject: New parent notify failed!" );
        if( !AddObject( obj->parent, obj->object_handle ) ) {
            WdeWriteTrail( "WdeControlCutObject: Add to parent failed!" );
        }
        return( FALSE );
    }

    if( obj->window_handle != NULL ) {
        DestroyWindow( obj->window_handle );
    }

    obj->window_handle = NULL;

    *new = obj->object_handle;

    WdeControlModified( obj );

    if( check_scroll ) {
        WdeCheckBaseScrollbars( FALSE );
    }

    return( TRUE );
}

BOOL WdeControlFirstChild( WdeControlObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    if( obj->parent == NULL ) {
        return( TRUE );
    }

    if( !Forward( obj->parent, PUT_ME_FIRST, obj->object_handle, NULL ) ) {
        WdeWriteTrail( "WdeControlFirstChild: PUT_ME_FIRST failed!" );
        return( FALSE );
    }

    WdeBringControlToTop( obj );

    return( TRUE );
}

void WdeWriteControlToInfo( WdeControlObject *obj )
{
    WdeInfoStruct is;

    is.obj_id = CONTROL_OBJ;
    is.res_info = obj->res_info;
    is.obj = obj->object_handle;
    is.size = GETCTL_SIZE( obj->control_info );
    is.c.text = GETCTL_TEXT( obj->control_info );
    is.c.id = GETCTL_ID( obj->control_info );
    is.symbol = WdeStrDup( obj->symbol );
//  is.helpsymbol = WdeStrDup( obj->helpsymbol );

    WdeWriteInfo( &is );

    if( is.symbol != NULL ) {
        WRMemFree( is.symbol );
    }
//  if( is.helpsymbol != NULL ) {
//      WRMemFree( is.helpsymbol );
//  }
}

BOOL WdeControlNotify( WdeControlObject *obj, NOTE_ID *id, void *p2 )
{
    HWND   handle;
    BOOL   new_parent;
    BOOL   parent_changed;

    new_parent = FALSE;
    parent_changed = FALSE;

    switch( *id ) {
    case MOVE_START:
        HideSelectBoxes();
        break;
    case MOVE_END:
        ShowSelectBoxes();
        break;
    case PRIMARY_OBJECT:
        if( obj->tag != (HWND)NULL ) {
            WdeTagPressed( WdeGetTagInfo( obj->tag ) );
        }
        /* make sure the current object is first in the parent list */
        /* I know, a child object has implementation knowledge of its
         * parent. C'est la vie.
         */
        if( !WdeControlFirstChild( obj, NULL, NULL ) ) {
            WdeWriteTrail( "WdeControlNotify: FIRST_CHILD falied!" );
            return( FALSE );
        }

        if( !Forward( obj->parent, BECOME_FIRST_CHILD, NULL, NULL ) ) {
            WdeWriteTrail( "WdeControlNotify: BECOME_FIRST_CHILD falied!" );
            return( FALSE );
        }

        if( obj->parent == obj->base_obj ) {
            WdeSetControlObjectMenu( FALSE, FALSE, FALSE, obj->mode );
        } else {
            WdeSetControlObjectMenu( TRUE, WdeIsDialogRestorable( obj->parent ),
                                     obj->res_info && obj->res_info->hash_table,
                                     obj->mode );
        }

        WdeWriteControlToInfo( obj );

        return ( TRUE );

    case NEW_PARENT:
        if( p2 == NULL ) {
            obj->parent = NULL;
            obj->parent_handle = NULL;
            return( Notify( obj->o_item, *id, obj->parent ) );
        }

        if( obj->parent != p2 ) {
            new_parent = TRUE;
        }

        if( !Forward( p2, GET_WINDOW_HANDLE, &handle, NULL ) ) {
            WdeWriteTrail( "WdeControlNotify: GET_WINDOW_HANDLE failed!" );
            return( FALSE );
        }

        if( obj->parent_handle != handle ) {
            parent_changed = TRUE;
        }

        if( new_parent ) {
            obj->parent = p2;
            obj->parent_handle = handle;
            if( !Forward( obj->parent, GET_FONT, &obj->font, NULL ) ) {
                WdeWriteTrail( "WdeControlNotify: GET_FONT failed!" );
                return( FALSE );
            }
            parent_changed = TRUE;
        }

        if( parent_changed ) {
            if( obj->window_handle != NULL ) {
                if( !WdeControlDestroyWindow ( obj, NULL, NULL ) ) {
                    WdeWriteTrail( "WdeControlNotify: DESTROY_WINDOW failed!" );
                    return( FALSE );
                }
                if( !Forward( (OBJPTR)obj->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
                    WdeWriteTrail("WdeControlNotify: CREATE_WINDOW failed!" );
                    return( FALSE );
                }
            }
        }
        return( Notify( obj->o_item, *id, obj->parent ) );

    case SET_LOCATION:
        if( obj->clear_interior ) {
            WdeSetClearObjectPos( obj );
        }
        return( WdeChangeControlSize( obj, FALSE, FALSE ) );
    }

    return( FALSE );
}

/* NOTE: This function assumes that there was no change of parent */
static BOOL WdeOffsetDialogUnits ( WdeControlObject *obj, WdeResizeRatio *r )
{
    RECT           parent_pos;
    RECT           new_pos;
    RECT           nc_size;
    RECT           win_pos;
    DialogSizeInfo dsize;

    Location( obj->object_handle, &new_pos );

    if( obj->parent != obj->base_obj ) {
        Location( obj->parent, &parent_pos );

        if( !Forward( obj->parent, GET_NC_SIZE, &nc_size, NULL ) ) {
            WdeWriteTrail( "WdeOffsetDialogUnits: GET_NC_SIZE failed!" );
            return( FALSE );
        }

        win_pos.left = new_pos.left - parent_pos.left - nc_size.left;
        win_pos.top = new_pos.top  - parent_pos.top  - nc_size.top;
    } else {
        win_pos.left = new_pos.left;
        win_pos.top = new_pos.top;
    }

    win_pos.right = win_pos.left + (new_pos.right - new_pos.left);
    win_pos.bottom = win_pos.top + (new_pos.bottom - new_pos.top);

    if( WdeScreenToDialog( obj, r, &win_pos, &dsize ) ) {
        SETCTL_SIZEX( obj->control_info, dsize.x );
        SETCTL_SIZEY( obj->control_info, dsize.y );
        return( TRUE );
    }

    return( FALSE );
}

BOOL WdeUpdateCDialogUnits( OBJPTR obj, RECT *new, WdeResizeRatio *r )
{
    WdeControlObject    *cobj;
    DialogSizeInfo      dsize;

    cobj = (WdeControlObject *)obj;

    /* save the old dialog units */
    dsize = GETCTL_SIZE( cobj->control_info );

    if( !WdeScreenToDialog( cobj, r, new, GETCTL_PSIZE( cobj->control_info ) ) ) {
        /* restore the old dialog units */
        SETCTL_SIZE( cobj->control_info, dsize );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeControlResize( WdeControlObject *obj, RECT *new_pos, BOOL *flag )
{
    RECT           object_rect;
    RECT           nc_size;
    RECT           parent_rect;
    RECT           rect;
    POINT          origin;
    POINT          pnt;
    BOOL           error;
    WdeResizeRatio resizer;
    int            min_width;
    int            min_height;

    if( *flag && obj->mode != WdeSelect ) {
        return( FALSE );
    }

    if( obj->parent == NULL ) {
        WdeWriteTrail( "WdeDialogResize: obj has no parent!" );
        return( FALSE );
    }

    if( !Forward( obj->parent, GET_RESIZER, &resizer, NULL ) ) {
        WdeWriteTrail( "WdeDialogResize: GET_RESIZER failed!" );
        return( FALSE );
    }

    min_width = MulDiv( WDE_CONTROL_MIN_WIDTH, resizer.xmap, 4 );
    min_height = MulDiv( WDE_CONTROL_MIN_HEIGHT, resizer.ymap, 8 );

    if( new_pos->right - new_pos->left < min_width ) {
        new_pos->right = new_pos->left + min_height;
    }

    if( new_pos->bottom - new_pos->top < min_height ) {
        new_pos->bottom = new_pos->top + min_height;
    }

    Location( obj->object_handle, &object_rect );

    if( !Resize( obj->o_item, new_pos, *flag ) ) {
        WdeWriteTrail( "WdeControlResize: O_ITEM RESIZE failed!" );
        return( FALSE );
    }

    if( !*flag ) {
        error = FALSE;
        if( obj->parent != obj->base_obj ) {
            Location( obj->parent, &parent_rect );
            if( !Forward( obj->parent, GET_NC_SIZE, &nc_size, NULL ) ) {
                WdeWriteTrail( "WdeDialogAddSubObject: Couldn't get nc size!" );
                error = TRUE;
            }
            parent_rect.left += nc_size.left;
            parent_rect.right -= nc_size.right;
            parent_rect.top += nc_size.top;
            parent_rect.bottom -= nc_size.bottom;
            pnt.x = new_pos->left;
            pnt.y = new_pos->top;

            if( pnt.x < parent_rect.left || pnt.y < parent_rect.top ) {
                WdeWriteTrail( "WdeControlResize: "
                               "control not contained by dialog top left!" );
                //error = TRUE;
                error = FALSE; // experiment -- don't cause a failure
            }
        }
        if( error ) {
            if( !Resize( obj->o_item, &object_rect, FALSE ) ) {
                WdeWriteTrail( "WdeControlResize: O_ITEM RESIZE undo failed!" );
            }
            return( FALSE );
        }
    }

    rect = *new_pos;

    GetOffset( &origin );

    OffsetRect( &rect, -origin.x, -origin.y );

    WdeMapWindowRect( obj->res_info->edit_win, obj->parent_handle, &rect );

    if( obj->clear_interior ) {
        WdeSetClearObjectPos( obj );
    }

    if( obj->window_handle != NULL ) {
        SetWindowPos( obj->window_handle, (HWND)NULL, rect.left, rect.top,
                      rect.right - rect.left, rect.bottom - rect.top,
                      SWP_NOZORDER | SWP_NOREDRAW );
    }

    if( *flag ) {
        WdeControlModified( obj );
        if( obj->parent == obj->base_obj ) {
            rect = *new_pos;
        }
        WdeUpdateCDialogUnits( obj, &rect, &resizer );
        if( !WdeChangeControlSize( obj, FALSE, FALSE ) ) {
            WdeWriteTrail( "WdeControlResize: WdeChangeControlSize failed!" );
            return( FALSE );
        }
        if( obj->parent == obj->base_obj ) {
            WdeCheckBaseScrollbars( FALSE );
        }
    }

    return( TRUE );
}

BOOL WdeControlMove ( WdeControlObject *obj, POINT *off, BOOL *forms_called )
{
    RECT           parent_rect;
    RECT           object_rect;
    RECT           old_pos;
    RECT           nc_size;
    POINT          origin;
    POINT          pnt;
    POINT          offset;
    bool           ok;
    OBJPTR         clone;
    OBJPTR         old_parent;
    WdeResizeRatio resizer;

    if( *forms_called && (clone = WdeCloneObject ( obj->object_handle, off )) != NULL ) {
        offset.x = 0;
        offset.y = 0;
    } else {
        offset.x = off->x;
        offset.y = off->y;
    }

    ok = TRUE;

    old_parent = obj->parent;

    Location( obj->object_handle, &old_pos );

    /* update the location of the object */
    if( !Move( obj->o_item, &offset, *forms_called ) ) {
        return( FALSE );
    }

    Location( obj->object_handle, &object_rect );

    if( !*forms_called ) {
        Location( obj->parent, &parent_rect );
        if( obj->parent != obj->base_obj ) {
            if( !Forward( obj->parent, GET_NC_SIZE, &nc_size, NULL ) ) {
                WdeWriteTrail( "WdeDialogAddSubObject: Couldn't get nc size!" );
                ok = FALSE;
            }
            if( ok ) {
                parent_rect.left += nc_size.left;
                parent_rect.right -= nc_size.right;
                parent_rect.top += nc_size.top;
                parent_rect.bottom -= nc_size.bottom;
                pnt.x = object_rect.left;
                pnt.y = object_rect.top;
                if( pnt.x < parent_rect.left || pnt.y < parent_rect.top ) {
                    WdeWriteTrail( "WdeControlMove: control "
                                   "not contained by dialog top left!" );
                    ok = FALSE;
                }
            }
        }
    }

    GetOffset( &origin );

    if( ok && *forms_called ) {
        if( old_parent == obj->parent ) {
            WdeOffsetDialogUnits( obj, NULL );
        } else {
            if( !Forward( obj->parent, GET_RESIZER, &resizer, NULL ) ) {
                WdeWriteTrail( "WdeDialogResize: GET_RESIZER failed!" );
                ok = FALSE;
            }
            if( ok ) {
                if( obj->parent != obj->base_obj ) {
                    OffsetRect( &object_rect, -origin.x, -origin.y );
                    WdeMapWindowRect( obj->res_info->edit_win,
                                      obj->parent_handle, &object_rect );
                }
                SETCTL_SIZEX( obj->control_info,
                              (uint_16)MulDiv( object_rect.left, 4, resizer.xmap ) );
                SETCTL_SIZEY( obj->control_info,
                              (uint_16)MulDiv( object_rect.top, 8, resizer.ymap ) );
            }

            if( obj->symbol != NULL ) {
                WdeAddSymbolToObjectHashTable( obj->res_info, obj->symbol,
                                               GETCTL_ID( obj->control_info ) );
            }
        }

        if( ok && !WdeChangeControlSize( obj, FALSE, TRUE ) ) {
            WdeWriteTrail( "WdeControlMove: WdeChangeControlSize failed!" );
            ok = FALSE;
        }

        if( ok ) {
            Location( obj->object_handle, &object_rect );
        }
    }

    if( ok ) {
        OffsetRect( &object_rect, -origin.x, -origin.y );

        pnt.x = object_rect.left;
        pnt.y = object_rect.top;

        MapWindowPoints( obj->res_info->edit_win, obj->parent_handle, &pnt, 1 );

        WdeSetClearObjectPos( obj );

        if( !SetWindowPos( obj->window_handle, (HWND)NULL, pnt.x, pnt.y, 0, 0,
                           SWP_NOZORDER | SWP_NOSIZE ) ) {
            ok = FALSE;
        }
    }

    if( !ok ) {
        if( *forms_called ) {
            RemoveObject( obj->parent, obj->object_handle );
        }
        if( !Resize( obj->o_item, &old_pos, FALSE ) ) {
            WdeWriteTrail( "WdeControlMove: O_ITEM RESIZE undo failed!" );
        }
    } else if( ok && *forms_called ) {
        WdeControlModified( obj );
        if( old_parent == obj->base_obj || obj->parent == obj->base_obj ) {
            WdeCheckBaseScrollbars( FALSE );
        }
    }

    return( ok );
}

BOOL WdeControlGetObjectInfo( WdeControlObject *obj,
                              WdeDialogBoxControl **info, char **s )
{
    if( info != NULL ) {
        *info = obj->control_info;
    }

    if( s ) {
        *s = obj->symbol;
    }

    return( TRUE );
}

BOOL WdeControlSetObjectInfo( WdeControlObject *obj,
                              WdeDialogBoxControl *info, char *s )
{
    RECT                size;
    POINT               origin;
    WdeResizeRatio      resizer;

    WdeControlModified( obj );

    if( obj->symbol != NULL ) {
        WRMemFree( obj->symbol );
    }
    obj->symbol = WdeStrDup( s );


    if( info == NULL ) {
        return( TRUE );
    }

    /* JPK - Added for help ID */
    if( obj->helpsymbol != NULL ) {
        WRMemFree( obj->helpsymbol );
    }
    obj->helpsymbol = WdeStrDup( info->helpsymbol );

    if( obj->control_info != NULL ) {
        WdeFreeDialogBoxControl( &obj->control_info );
    }

    obj->control_info = WdeCopyDialogBoxControl( info );

    if( !GETCTL_SIZEX( obj->control_info ) && !GETCTL_SIZEY( obj->control_info ) &&
        !GETCTL_SIZEW( obj->control_info ) && !GETCTL_SIZEH( obj->control_info ) ) {

        Location( obj->object_handle, &size );

        if( obj->parent != obj->base_obj ) {
            GetOffset( &origin );

            OffsetRect( &size, -origin.x, -origin.y );

            WdeMapWindowRect( obj->res_info->edit_win, obj->parent_handle, &size );
        }

        if( !Forward( obj->parent, GET_RESIZER, &resizer, NULL ) ) {
            WdeWriteTrail( "WdeControlSetControlInfo: GET_RESIZER failed!" );
            return( FALSE );
        }

        if( !WdeScreenToDialog( obj, &resizer, &size, GETCTL_PSIZE( obj->control_info ) ) ) {
            WdeWriteTrail( "WdeControlSetControlInfo: WdeScreenToDialog failed!" );
            return( FALSE );
        }

    }

    return( TRUE );
}

BOOL WdeControlSetObjectHelpInfo( WdeControlObject *obj,
                                  WdeDialogBoxControl *info, char *hs )
{
    _wde_touch( info );

    if( obj->helpsymbol != NULL ) {
        WRMemFree( obj->helpsymbol );
    }
    obj->helpsymbol = WdeStrDup( hs );

    return( TRUE );
}

BOOL WdeControlGetObjectHelpInfo( WdeControlObject *obj,
                                  WdeDialogBoxControl **info, char **hs )
{
    // this is redundant
    if( info != NULL ) {
        *info = obj->control_info;
    }

    // this is needed
    if( hs != NULL ) {
        *hs = obj->helpsymbol;
    }

    return( TRUE );
}

BOOL WdeControlResolveSymbol( WdeControlObject *obj, bool *b, bool *from_id )
{
    WdeHashValue        val;
    void                *vp;
    bool                found;

    if( obj->control_info != NULL && obj->res_info != NULL ) {
        if( from_id != NULL && *from_id ) {
            vp = WdeResolveValue( obj->res_info->hash_table,
                                  (WdeHashValue)GETCTL_ID( obj->control_info ) );
            if( vp != NULL ) {
                if( obj->symbol != NULL ) {
                    WRMemFree( obj->symbol );
                }
                obj->symbol = vp;
                WdeControlModified( obj );
            }
        } else {
            if( obj->symbol != NULL ) {
                val = WdeLookupName( obj->res_info->hash_table,
                                     obj->symbol, &found );
                if( found ) {
                    SETCTL_ID( obj->control_info, (uint_16)val );
                    WdeControlModified( obj );
                } else {
                    WRMemFree( obj->symbol );
                    obj->symbol = NULL;
                }
            }
        }
        if ( b != NULL && *b && obj->object_handle == GetCurrObject() ) {
            WdeWriteControlToInfo( obj );
        }
    } else {
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeControlResolveHelpSymbol( WdeControlObject *obj, bool *b, bool *from_id )
{
    WdeHashValue        val;
    void                *vp;
    bool                found;

    if( obj->control_info != NULL && obj->res_info != NULL ) {
        if( from_id != NULL && *from_id ) {
            vp = WdeResolveValue( obj->res_info->hash_table,
                                  (WdeHashValue)GETCTL_HELPID( obj->control_info ) );
            if( vp != NULL ) {
                if( obj->helpsymbol != NULL ) {
                    WRMemFree( obj->helpsymbol );
                }
                obj->helpsymbol = vp;
                if( obj->control_info->helpsymbol != NULL ) {
                    WRMemFree( obj->control_info->helpsymbol );
                }
                obj->control_info->helpsymbol = WdeStrDup( obj->helpsymbol );
                WdeControlModified( obj );
            }
        } else {
            if( obj->helpsymbol ) {
                val = WdeLookupName( obj->res_info->hash_table, obj->helpsymbol, &found );
                if( found ) {
                    SETCTL_HELPID( obj->control_info, (uint_32)val );
                    WdeControlModified( obj );
                } else {
                    WRMemFree( obj->helpsymbol );
                    obj->helpsymbol = NULL;
                }
            }
        }
        if ( b != NULL && *b && obj->object_handle == GetCurrObject() ) {
            WdeWriteControlToInfo( obj );
        }
    } else {
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeControlModifyInfo( WdeControlObject *obj, WdeInfoStruct *in, void *p2 )
{
    WdeHashEntry        *entry;
    char                *text;
    bool                dup;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( in->c.text ) {
        if( GETCTL_TEXT( obj->control_info ) ) {
            WRMemFree( GETCTL_TEXT( obj->control_info ) );
        }
        SETCTL_TEXT( obj->control_info, in->c.text );
        text = WdeResNameOrOrdinalToStr( GETCTL_TEXT( obj->control_info ), 10 );
        if( text != NULL ) {
            SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)text );
            WRMemFree( text );
        } else {
            SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)"" );
        }
    }

    if( obj->symbol != NULL ) {
        WRMemFree( obj->symbol );
    }

    obj->symbol = in->symbol;

    if( in->symbol != NULL ) {
        dup = FALSE;
        entry = WdeDefAddHashEntry( obj->res_info->hash_table, obj->symbol, &dup );
        if( entry != NULL ) {
            SETCTL_ID( obj->control_info, entry->value );
        }
    } else {
        SETCTL_ID( obj->control_info, in->c.id );
    }

    WdeControlModified( obj );

    WdeBringControlToTop( obj );

    return( TRUE );
}

BOOL WdeControlGetOrderMode( WdeControlObject *obj, WdeOrderMode *mode, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *mode = obj->mode;

    return( TRUE );
}

BOOL WdeControlSetOrderMode( WdeControlObject *obj, WdeOrderMode *mode,
                             WdeSetOrderLists **l )
{
    WdeSetOrderStruct   *o;
    DialogStyle         style;

    style = GETCTL_STYLE( obj->control_info );

    if( obj->mode == WdeSelect ) {
        o = (WdeSetOrderStruct *)WRMemAlloc( sizeof( WdeSetOrderStruct ) );
        if( o == NULL ) {
            return( FALSE );
        }
        o->old_oe = ListElement( WdeFindOrderedEntry( (*l)->oldlist,
                                                      obj->object_handle ) );
        o->new_oe = NULL;
        o->lists = *l;
        o->old_oe->mode = *mode;
        o->old_oe->pos_set = FALSE;
        o->old_oe->tab_set = ((style & WS_TABSTOP) != 0);
        o->old_oe->grp_set = ((style & WS_GROUP) != 0);
        o->res_info = obj->res_info;
        obj->tag = WdeCreateTag( obj->window_handle, o );
        obj->mode = *mode;
    } else if( obj->mode == *mode ) {
        o = WdeGetTagInfo( obj->tag );
        WdeDestroyTag( obj->tag );
        obj->tag = NULL;
        obj->mode = WdeSelect;
        if( o == NULL ) {
            return( FALSE );
        }
        if( o->old_oe->tab_set ) {
            style |= WS_TABSTOP;
        } else {
            style &= 0xffffffff ^ WS_TABSTOP;
        }
        if( o->old_oe->grp_set ) {
            style |= WS_GROUP;
        } else {
            style &= 0xffffffff ^ WS_GROUP;
        }
        *l = o->lists;
        WRMemFree( o );
    } else {
        o = WdeGetTagInfo( obj->tag );
        if( o == NULL ) {
            return( FALSE );
        }
        o->old_oe->mode = *mode;
        if( o->new_oe != NULL ) {
            o->new_oe->mode = *mode;
        }
        *l = o->lists;
        obj->mode = *mode;
    }

    SETCTL_STYLE( obj->control_info, style );

    return( TRUE );
}

BOOL WdeControlSizeToText( WdeControlObject *obj, void *p1, void *p2 )
{
    SIZE        size;
    int         width;
    int         height;
    OBJ_ID      id;
    RECT        pos;
    BOOL        ok;

    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    ok = TRUE;

    if( obj->parent_handle == (HWND)NULL ) {
        ok = (Forward( obj->parent, GET_WINDOW_HANDLE, &obj->parent_handle, NULL ) &&
              obj->parent_handle != (HWND)NULL);
    }

    if( ok ) {
        ok = Forward( obj->object_handle, IDENTIFY, &id, NULL );
    }

    if( ok ) {
        ok = WdeGetNameOrOrdSize( obj->parent, GETCTL_TEXT( obj->control_info ), &size );
    }

    if( ok ) {
        width = 0;
        ok = FALSE;
        switch( id ) {
        case PBUTTON_OBJ:
            SetRectEmpty( &pos );
            WdeChangeSizeToDefIfSmallRect( obj->parent, PBUTTON_OBJ, &pos );
            width = max( size.cx + (WDE_SIZE_TO_TEXT_PAD * 2), pos.right );
            ok = TRUE;
            break;

        case RBUTTON_OBJ:
            width = -1;
            /* fall through */
        case CBUTTON_OBJ:
            width += WDE_CKECK_RADIO_ADJUST;
            /* fall through */
        case TEXT_OBJ:
        case EDIT_OBJ:
            width += size.cx + (WDE_SIZE_TO_TEXT_PAD * 2);
            ok = TRUE;
            break;
        }
    }

    if( ok ) {
        height = 0;
        ok = FALSE;
        switch( id ) {
        case RBUTTON_OBJ:
        case CBUTTON_OBJ:
        case TEXT_OBJ:
        case EDIT_OBJ:
            SetRectEmpty( &pos );
            WdeChangeSizeToDefIfSmallRect( obj->parent, id, &pos );
            /* fall through */
        case PBUTTON_OBJ:
            height = pos.bottom;
            ok = TRUE;
            break;
        }
    }

    if( ok ) {
        Location( obj->object_handle, &pos );
        pos.right = pos.left + width;
        pos.bottom = pos.top + height;
        HideSelectBoxes();
        ok = Resize( obj->object_handle, &pos, TRUE );
        ShowSelectBoxes();
    }

    if( ok ) {
        WdeWriteControlToInfo( obj );
    }

    return( ok );
}

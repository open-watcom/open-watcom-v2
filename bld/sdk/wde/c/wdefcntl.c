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
#include "wde.rh"
#include "wdefbase.h"
#include "windlg.h"
#include "windlg32.h"
#include "wdefcntl.h"
#include "wdedispa.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_CONTROL_MIN_WIDTH  1
#define WDE_CONTROL_MIN_HEIGHT 1
#define WDE_SIZE_TO_TEXT_PAD   2
#define WDE_CKECK_RADIO_ADJUST 16

#define pick_ACTS(o) \
    pick_ACTION_MOVE(o,pick) \
    pick_ACTION_NOTIFY(o,pick) \
    pick_ACTION_RESIZE(o,pick) \
    pick_ACTION_DRAW(o,pick) \
    pick_ACTION_ADD_SUBOBJECT(o,pick1) \
    pick_ACTION_DESTROY(o,pick) \
    pick_ACTION_COPY(o,pick) \
    pick_ACTION_CUT(o,pick) \
    pick_ACTION_PASTE(o,pick) \
    pick_ACTION_VALIDATE_ACTION(o,pick) \
    pick_ACTION_RESIZE_INFO(o,pick) \
    pick_ACTION_GET_WINDOW_HANDLE(o,pick) \
    pick_ACTION_CREATE_WINDOW(o,pick) \
    pick_ACTION_DESTROY_WINDOW(o,pick) \
    pick_ACTION_SHOW_WIN(o,pick) \
    pick_ACTION_GET_OBJECT_INFO(o,pick) \
    pick_ACTION_SET_OBJECT_INFO(o,pick) \
    pick_ACTION_GET_OBJECT_HELPINFO(o,pick) \
    pick_ACTION_SET_OBJECT_HELPINFO(o,pick) \
    pick_ACTION_SET_FONT(o,pick) \
    pick_ACTION_GET_FONT(o,pick1) \
    pick_ACTION_GET_NC_SIZE(o,pick1) \
    pick_ACTION_ON_TOP(o,pick) \
    pick_ACTION_TEST(o,pick) \
    pick_ACTION_TESTEX(o,pick) \
    pick_ACTION_BECOME_FIRST_CHILD(o,pick) \
    pick_ACTION_GET_RESIZE_INC(o,pick1) \
    pick_ACTION_GET_SCROLL_RECT(o,pick1) \
    pick_ACTION_SET_CLEAR_INT(o,pick) \
    pick_ACTION_IS_OBJECT_CLEAR(o,pick) \
    pick_ACTION_IS_MARK_VALID(o,pick) \
    pick_ACTION_RESOLVE_SYMBOL(o,pick) \
    pick_ACTION_RESOLVE_HELPSYMBOL(o,pick) \
    pick_ACTION_MODIFY_INFO(o,pick) \
    pick_ACTION_SET_ORDER_MODE(o,pick) \
    pick_ACTION_GET_ORDER_MODE(o,pick) \
    pick_ACTION_SIZE_TO_TEXT(o,pick) \
    pick_ACTION_GET_RESIZER(o,pick1)

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    DISPATCH_FN         *dispatcher;
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

/* Local Window callback functions prototypes */
WINEXPORT bool CALLBACK WdeControlDispatcher( ACTION_ID, OBJPTR, void *, void * );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool WdeChangeControlSize( WdeControlObject *, bool, bool );
static bool WdeOffsetDialogUnits( WdeControlObject *, WdeResizeRatio * );
static void WdeBringControlToTop( WdeControlObject * );
static void WdeSetClearObjectPos( WdeControlObject * );
static void WdeFreeControlObject( WdeControlObject * );
static void WdeWriteControlToInfo( WdeControlObject * );
static bool WdeControlModifyInfo( WdeControlObject *, WdeInfoStruct *, void * );
static bool WdeControlSizeToText( WdeControlObject *, void *, void * );

#define pick(e,n,c)  static bool WdeControl ## n ## c;
#define pick1(e,n,c) static bool WdeControl ## n ## c;
    pick_ACTS( WdeControlObject )
#undef pick1
#undef pick

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static DISPATCH_FN       *WdeControlDispatch;
static HINSTANCE         WdeAppInst;

static DISPATCH_ITEM WdeControlActions[] = {
    #define pick(e,n,c) {e, (DISPATCH_RTN *)WdeControl ## n},
    #define pick1(e,n,c) {e, (DISPATCH_RTN *)NULL},
    pick_ACTS( WdeControlObject )
    #undef pick1
    #undef pick
};

#define MAX_ACTIONS      (sizeof( WdeControlActions ) / sizeof( DISPATCH_ITEM ))

static void WdeControlModified ( WdeControlObject *obj )
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

OBJPTR CALLBACK WdeControlCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
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
        ok = ( !Forward( parent, GET_ORDER_MODE, &mode, NULL ) || mode == WdeSelect );
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
        Forward( ancestor, IDENTIFY, &id, NULL );
        while( id != DIALOG_OBJ && id != BASE_OBJ ) {
            GetObjectParent( ancestor, &ancestor );
            Forward( ancestor, IDENTIFY, &id, NULL );
        }
        new->parent = ancestor;
        OBJ_DISPATCHER_SET( new, WdeControlDispatch );
        new->sizeable = true;
        new->mode = WdeSelect;
        if( handle == NULL ) {
            new->object_handle = (OBJPTR)new;
        } else {
            new->object_handle = handle;
        }
        new->base_obj = GetMainObject();
        new->res_info = WdeGetCurrentRes();
        ok = ( new->base_obj != NULL && new->res_info != NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeControlCreate: Bad base_obj or res_info!" );
        }
    }

    if( ok ) {
        ok = Forward( new->parent, GET_WINDOW_HANDLE, &new->parent_handle, NULL ) != 0;
    }

    if( !ok ) {
        WdeWriteTrail( "WdeControlCreate: Couldn't get parent window handle!" );
    }

    if( ok ) {
        ok = Forward( new->parent, GET_FONT, &new->font, NULL ) != 0;
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
        ok = Forward( new->object_handle, GET_WINDOW_CLASS, &new->window_class, NULL ) != 0;
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

    return( (OBJPTR)new );
}

bool CALLBACK WdeControlDispatcher( ACTION_ID act, OBJPTR obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Control", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeControlActions[i].id == act ) {
            if( WdeControlActions[i].rtn != NULL ) {
                return( WdeControlActions[i].rtn( obj, p1, p2 ) );
            } else {
                return( Forward( ((WdeControlObject *)obj)->parent, act, p1, p2 ) );
            }
        }
    }

    return( Forward( ((WdeControlObject *)obj)->o_item, act, p1, p2 ) );
}

bool WdeControlInit( bool first )
{
    _wde_touch( first );
    WdeAppInst = WdeGetAppInstance();
    WdeControlDispatch = MakeProcInstance_DISPATCHER( WdeControlDispatcher, WdeAppInst );
    return( true );
}

void WdeControlFini( void )
{
    FreeProcInstance_DISPATCHER( WdeControlDispatch );
}

bool WdeControlTest( WdeControlObject *obj, TEMPLATE_HANDLE *dlgtemplate, size_t *templatelen )
{
    char        *Text;
    char        *Class;
    uint_32     style;
    uint_16     ID;

    Text = WdeResNameOrOrdinalToStr( GETCTL_TEXT( obj->control_info ), 10 );
    if( Text == NULL ) {
        Text = WdeStrDup( "" );
        if( Text == NULL ) {
            return( false );
        }
    }

    Class = WdeControlClassToStr( GETCTL_CLASSID( obj->control_info ) );
    if( Class == NULL ) {
        Class = WdeStrDup( "" );
        if( Class == NULL ) {
            WRMemFree( Text );
            return( false );
        }
    }

    style = GETCTL_STYLE( obj->control_info );

    ID = GETCTL_ID( obj->control_info );
    if( stricmp( obj->window_class, "combobox" ) == 0 ) {
        style &= 0xffffffff ^ (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE);
    } else if( stricmp( obj->window_class, "wde_borbtn" ) == 0 ) {
        if( !WdeIsBorBtnIDSupported( GETCTL_ID( obj->control_info ) ) ) {
            ID = (uint_16)WDE_PREVIEW_ID;
        }
    }

    *dlgtemplate = AddControl( *dlgtemplate,
                            GETCTL_SIZEX( obj->control_info ),
                            GETCTL_SIZEY( obj->control_info ),
                            GETCTL_SIZEW( obj->control_info ),
                            GETCTL_SIZEH( obj->control_info ),
                            ID, style, Class, Text, NULL, 0, templatelen );

    WRMemFree( Text );
    WRMemFree( Class );

    return( *dlgtemplate != NULL );
}

bool WdeControlTestEX( WdeControlObject *obj, TEMPLATE_HANDLE *dlgtemplate, size_t *templatelen )
{
    char        *Text;
    char        *Class;
    uint_32     style;
    uint_16     ID;

    Text = WdeResNameOrOrdinalToStr( GETCTL_TEXT( obj->control_info ), 10 );
    if( Text == NULL ) {
        Text = WdeStrDup( "" );
        if( Text == NULL ) {
            return( false );
        }
    }

    Class = WdeControlClassToStr( GETCTL_CLASSID( obj->control_info ) );
    if( Class == NULL ) {
        Class = WdeStrDup( "" );
        if( Class == NULL ) {
            WRMemFree( Text );
            return( false );
        }
    }

    style = GETCTL_STYLE( obj->control_info );

    ID = GETCTL_ID( obj->control_info );
    if( stricmp( obj->window_class, "combobox" ) == 0 ) {
        style &= 0xffffffff ^ (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE);
    } else if( stricmp( obj->window_class, "wde_borbtn" ) == 0 ) {
        if( !WdeIsBorBtnIDSupported( GETCTL_ID( obj->control_info ) ) ) {
            ID = (uint_16)WDE_PREVIEW_ID;
        }
    }

    *dlgtemplate = AddControlEX( *dlgtemplate,
                              GETCTL_SIZEX( obj->control_info ),
                              GETCTL_SIZEY( obj->control_info ),
                              GETCTL_SIZEW( obj->control_info ),
                              GETCTL_SIZEH( obj->control_info ),
                              ID, style,
                              GETCTL_EXSTYLE( obj->control_info ),
                              GETCTL_HELPID( obj->control_info ),
                              Class, Text, NULL, 0, templatelen );

    WRMemFree( Text );
    WRMemFree( Class );

    return( *dlgtemplate != NULL );
}

bool WdeControlIsMarkValid( WdeControlObject *obj, bool *flag, void *p2 )
{
    DWORD   style;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( obj->mode == WdeSelect && obj->window_handle != NULL ) {
        style = GET_WNDSTYLE( obj->window_handle );
        *flag = ((style & WS_VISIBLE) != 0);
    } else {
        *flag = false;
    }

    return( true );
}

bool WdeControlDestroy( WdeControlObject *obj, bool *flag, bool *p2 )
{
    RECT        rect;
    OBJPTR      next;
    bool        check_scroll;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    check_scroll = ( obj->parent == obj->base_obj );

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

    *flag = false;

    if( !Forward( obj->o_item, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeControlDestroy: Failed to destroy OITEM!" );
        if( obj->parent ) {
            AddObject( obj->parent, obj->object_handle );
        }
        return( false );
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
        WdeCheckBaseScrollbars( false );
    }

    return( true );
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

bool WdeChangeControlSize( WdeControlObject *obj, bool widths_only, bool snap_to_grid )
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
        WdeDialogToScreen( obj->object_handle, NULL, GETCTL_PSIZE( obj->control_info ), &size );
    }

    if( widths_only ) {
        t = obj_rect;
        t.right = t.left + (size.right - size.left);
        t.bottom = t.top + (size.bottom - size.top);
        size = t;
    }

    if( !EqualRect( &size, &obj_rect ) ) {
        if( !Resize( obj->object_handle, &size, false ) ) {
            WdeWriteTrail( "WdeChangeControlSize: RESIZE failed!" );
            return( false );
        }
    }

    return( true );
}

bool WdeControlSetFont( WdeControlObject *obj, HFONT *font, WdeResizeRatio *resizer )
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
        return( false );
    }

    if( !WdeChangeControlSize( obj, false, false ) ) {
        WdeWriteTrail( "WdeControlSetFont: WdeChangeControlSize failed!" );
        return( false );
    }

    if( obj->window_handle != NULL ) {
        control_class = GETCTL_CLASSID( obj->control_info );
        if( control_class->Class == CLASS_STATIC && (GETCTL_STYLE( obj->control_info ) & 0x0000000f) == SS_ICON ) {
            return( true );
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

        SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)(LPCSTR)name );
    }

    return( true );
}

bool WdeControlDraw( WdeControlObject *obj, RECT *area, HDC *dc )
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
        RedrawWindow( obj->window_handle, (RECT *)&trect, (HRGN)NULL, RDW_INTERNALPAINT | RDW_UPDATENOW );
    }

    OffsetRect( &rect, -origin.x, -origin.y );

    if( obj->parent == obj->base_obj ) {
        flags = SWP_NOZORDER | SWP_NOSIZE;
        if( dc == NULL ) {
            flags |= SWP_NOREDRAW | SWP_NOACTIVATE;
        }
        SetWindowPos( obj->window_handle, (HWND)NULL, rect.left, rect.top, 0, 0, flags );
    }

    return( true );
}

bool WdeControlOnTop( WdeControlObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    WdeBringControlToTop( obj );

    return( true );
}

bool WdeControlSetClearInt( WdeControlObject *obj, bool *b, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    obj->clear_interior = *b;

    return( true );
}

bool WdeControlGetClearInt( WdeControlObject *obj, bool *b, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *b = obj->clear_interior;

    return( true );
}

bool WdeControlCreateWindow( WdeControlObject *obj, bool *p1, void *p2 )
{
    ResNameOrOrdinal    *rname;
    char                *name;
    char                *cname;
    void                *crt_params;
    char                temp[10];
    POINT               pnt;
    RECT                obj_rect;
    DialogStyle         style;
    DWORD               exstyle;
    bool                set_font;
    bool                set_crt_params;
    POINT               origin;
    uint_16             ID;
    OBJ_ID              oid;
    WNDPROC             new_proc;

    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    if( !Forward( obj->parent, GET_WINDOW_HANDLE, &obj->parent_handle, NULL ) ) {
        WdeWriteTrail( "WdeControlCreateWindow: GET_WINDOW_HANDLE failed!" );
        return( false );
    }

    if( !Forward( obj->object_handle, IDENTIFY, &oid, NULL ) ) {
        WdeWriteTrail( "WdeControlCreateWindow: IDENTIFY failed!" );
        return( false );
    }

    new_proc = (WNDPROC)NULL;
    if( !Forward( obj->object_handle, GET_WND_PROC, &new_proc, NULL ) || new_proc == (WNDPROC)NULL ) {
        WdeWriteTrail( "WdeControlCreateWindow: Could not get new wnd proc!" );
        return( false );
    }

    set_font = true;
    set_crt_params = true;

    Location( obj->object_handle, &obj_rect );

    GetOffset( &origin );

    pnt.x = obj_rect.left - origin.x;
    pnt.y = obj_rect.top - origin.y;

    MapWindowPoints( obj->res_info->edit_win, obj->parent_handle, &pnt, 1 );

    name = NULL;
    cname = NULL;

    if( oid == ICON_OBJ ) {
        set_font = false;
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
    if( stricmp( obj->window_class, "wde_borbtn" ) == 0 ) {
        if( !WdeIsBorBtnIDSupported( GETCTL_ID( obj->control_info ) ) ) {
            ID = (uint_16)WDE_PREVIEW_ID;
        }
        set_font = false;
        set_crt_params = false;
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
        return( false );
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
        return( false );
    }
    SET_WNDPROC( obj->window_handle, (LONG_PTR)new_proc );

    if( set_font ) {
        SendMessage( obj->window_handle, WM_SETFONT, (WPARAM)obj->font, (LPARAM)TRUE );

        SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)(LPCSTR)name );
    }

    WdeBringControlToTop( obj );

    ShowWindow( obj->window_handle, SW_SHOW );

    return( true );
}

bool WdeControlDestroyWindow( WdeControlObject *obj, bool *quick, bool *destroy_children )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( destroy_children );

    if( !quick || (!*quick && (obj->window_handle != NULL)) ) {
        DestroyWindow( obj->window_handle );
    }

    obj->window_handle = NULL;
    obj->tag = NULL;

    return( true );
}

bool WdeControlShowWindow( WdeControlObject *obj, bool *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    WdeShowObjectWindow( obj->window_handle, *flag );

    return( true );
}

bool WdeControlGetResizeInfo( WdeControlObject *obj, RESIZE_ID *info, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    if( obj->sizeable && obj->mode == WdeSelect ) {
        *info = R_ALL;
    } else {
        *info = R_NONE;
    }

    return( true );
}

static bool WdeControlValidateMove( WdeControlObject *obj, POINT *pnt, ACTION_ID act )
{
    RECT        obj_rect;
    RECT        win_rect;
    OBJ_ID      id;
    bool        clear;

    if( act == MOVE && obj->mode != WdeSelect ) {
        return( false );
    }

    id = BASE_OBJ;
    Forward( obj->object_handle, IDENTIFY, &id, NULL );

    if( id == COMBOBOX_OBJ ) {
        Location( (OBJPTR)obj, &obj_rect );
        GetWindowRect( obj->window_handle, &win_rect );
        obj_rect.bottom = obj_rect.top + win_rect.bottom - win_rect.top;
        return( PtInRect( &obj_rect, *pnt ) != 0 );
    }

    clear = obj->clear_interior;

    if( !clear && id == FRAME_OBJ ) {
        DialogStyle     mask;

        mask = GETCTL_STYLE( obj->control_info ) & 0x0000000F;

        if( mask != SS_BLACKRECT && mask != SS_GRAYRECT && mask != SS_WHITERECT ) {
            clear = true;
        }
    }


    if( clear ) {
        Location( (OBJPTR)obj, &obj_rect );
        obj_rect.left += WDE_BORDER_SIZE;
        obj_rect.right -= WDE_BORDER_SIZE;
        obj_rect.top += WDE_BORDER_SIZE;
        obj_rect.bottom -= WDE_BORDER_SIZE;
        return( PtInRect( &obj_rect, *pnt ) == 0 );
    }

    return( true );
}

bool WdeControlValidateAction( WdeControlObject *obj, ACTION_ID *act, void *p2 )
{
    int     i;

    if( *act == MOVE || *act == PICK ) {
        return( WdeControlValidateMove( obj, (POINT *)p2, *act ) );
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeControlActions[i].id == *act ) {
            return( true );
        }
    }

    return( ValidateAction( obj->o_item, *act, p2 ) );
}

bool WdeControlGetWindowHandle( WdeControlObject *obj, HWND *hwin, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *hwin = obj->window_handle;

    return( true );
}

bool WdeControlPasteObject( WdeControlObject *obj, OBJPTR parent, POINT *pnt )
{
    bool ok;

    obj->base_obj = GetMainObject();
    obj->res_info = WdeGetCurrentRes();

    ok = ( obj->base_obj != NULL && obj->res_info != NULL );

    if( ok ) {
        if( pnt != NULL ) {
            ok = PasteObject( obj->o_item, parent, *pnt ) != 0;
            if( !ok ) {
                WdeWriteTrail( "WdeControlPasteObject: OITEM PASTE failed!" );
            }
        } else {
            obj->parent = NULL;
            ok = Notify( obj->object_handle, NEW_PARENT, parent ) != 0;
        }
    }

    if( ok && pnt != NULL ) {
        ok = WdeOffsetDialogUnits( obj, NULL ) != 0;
        if( !ok ) {
            WdeWriteTrail( "WdeControlPasteObject: WdeOffsetDialogUnits failed!" );
        }
    }

    if( ok ) {
        ok = WdeChangeControlSize( obj, false, true ) != 0;
        if( !ok ) {
            WdeWriteTrail( "WdeControlPasteObject: WdeChangeControlSize failed!" );
        }
    }

    if( ok ) {
        ok = Forward( obj->object_handle, CREATE_WINDOW, NULL, NULL ) != 0;
        if( !ok ) {
            WdeWriteTrail( "WdeControlPasteObject: create window falied!" );
        }
    }

    if( ok ) {
        WdeControlModified( obj );
        if( obj->symbol != NULL ) {
            WdeAddSymbolToObjectHashTable( obj->res_info, obj->symbol, GETCTL_ID( obj->control_info ) );
        }
        WdeControlOnTop( obj, NULL, NULL );
    } else {
        obj->base_obj = NULL;
        obj->res_info = NULL;
    }

    return( ok );
}

bool WdeControlCopyObject( WdeControlObject *obj, WdeControlObject **new, OBJPTR handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeControlCopyObject: Invalid new object!" );
        return( false );
    }

    *new = (WdeControlObject *)WRMemAlloc( sizeof( WdeControlObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeControlCopyObject: Object malloc failed" );
        return( false );
    }
    memset( *new, 0, sizeof( WdeControlObject ) );

    OBJ_DISPATCHER_COPY( *new, obj );
    (*new)->window_class = obj->window_class;
    (*new)->font = obj->font;
    (*new)->sizeable = obj->sizeable;
    (*new)->clear_interior = obj->clear_interior;
    (*new)->control_info = WdeCopyDialogBoxControl( obj->control_info );
    (*new)->mode = WdeSelect;

    if( !(*new)->control_info ) {
        WdeWriteTrail( "WdeControlCopyObject: NULL info!" );
        WdeFreeControlObject( *new );
        return( false );
    }

    SETCTL_ID( (*new)->control_info, WdeGetNextControlID() );

    if( obj->symbol ) {
        (*new)->symbol = WdeStrDup( obj->symbol );
        if( (*new)->symbol == NULL ) {
            WdeWriteTrail( "WdeControlCopyObject: Symbol alloc failed" );
            WdeFreeControlObject( *new );
            return( false );
        }
    }

    /* JPK - added for help id support */
    if( obj->helpsymbol != NULL ) {
        (*new)->helpsymbol = WdeStrDup( obj->helpsymbol );
        if( (*new)->helpsymbol == NULL ) {
            WdeWriteTrail( "WdeControlCopyObject: Help symbol alloc failed" );
            WdeFreeControlObject( *new );
            return( false );
        }
    }

    if( handle == NULL ) {
        (*new)->object_handle = (OBJPTR)*new;
    } else {
        (*new)->object_handle = handle;
    }

    if( !CopyObject( obj->o_item, &(*new)->o_item, (*new)->object_handle ) ) {
        WdeWriteTrail( "WdeControlCopyObject: OITEM not created!" );
        WdeFreeControlObject( *new );
        return( false );
    }

    return( true );
}

bool WdeControlCutObject( WdeControlObject *obj, OBJPTR *new, void *p2 )
{
    NOTE_ID     noteid;
    bool        check_scroll;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *new = NULL;

    check_scroll = ( obj->parent == obj->base_obj );

    if( !RemoveObject( obj->parent, obj->object_handle ) ) {
        WdeWriteTrail( "WdeControlCutObject: Remove from parent failed!" );
        return( false );
    }

    noteid = NEW_PARENT;
    if( !WdeControlNotify( obj, &noteid, NULL ) ) {
        WdeWriteTrail( "WdeControlCutObject: New parent notify failed!" );
        if( !AddObject( obj->parent, obj->object_handle ) ) {
            WdeWriteTrail( "WdeControlCutObject: Add to parent failed!" );
        }
        return( false );
    }

    if( obj->window_handle != NULL ) {
        DestroyWindow( obj->window_handle );
    }

    obj->window_handle = NULL;

    *new = obj->object_handle;

    WdeControlModified( obj );

    if( check_scroll ) {
        WdeCheckBaseScrollbars( false );
    }

    return( true );
}

bool WdeControlFirstChild( WdeControlObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    if( obj->parent == NULL ) {
        return( true );
    }

    if( !Forward( obj->parent, PUT_ME_FIRST, obj->object_handle, NULL ) ) {
        WdeWriteTrail( "WdeControlFirstChild: PUT_ME_FIRST failed!" );
        return( false );
    }

    WdeBringControlToTop( obj );

    return( true );
}

void WdeWriteControlToInfo( WdeControlObject *obj )
{
    WdeInfoStruct is;

    is.obj_id = CONTROL_OBJ;
    is.res_info = obj->res_info;
    is.obj = obj->object_handle;
    is.sizeinfo.x = GETCTL_SIZEX( obj->control_info );
    is.sizeinfo.y = GETCTL_SIZEY( obj->control_info );
    is.sizeinfo.width = GETCTL_SIZEW( obj->control_info );
    is.sizeinfo.height = GETCTL_SIZEH( obj->control_info );
    is.u.ctl.text = GETCTL_TEXT( obj->control_info );
    is.u.ctl.id = GETCTL_ID( obj->control_info );
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

bool WdeControlNotify( WdeControlObject *obj, NOTE_ID *noteid, void *p2 )
{
    HWND   handle;
    bool   new_parent;
    bool   parent_changed;

    new_parent = false;
    parent_changed = false;

    switch( *noteid ) {
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
            return( false );
        }

        if( !Forward( obj->parent, BECOME_FIRST_CHILD, NULL, NULL ) ) {
            WdeWriteTrail( "WdeControlNotify: BECOME_FIRST_CHILD falied!" );
            return( false );
        }

        if( obj->parent == obj->base_obj ) {
            WdeSetControlObjectMenu( FALSE, FALSE, FALSE, obj->mode );
        } else {
            WdeSetControlObjectMenu( TRUE, WdeIsDialogRestorable( obj->parent ),
                                     obj->res_info && obj->res_info->hash_table,
                                     obj->mode );
        }

        WdeWriteControlToInfo( obj );

        return( true );

    case NEW_PARENT:
        if( p2 == NULL ) {
            obj->parent = NULL;
            obj->parent_handle = NULL;
            return( Notify( obj->o_item, *noteid, obj->parent ) );
        }

        if( obj->parent != p2 ) {
            new_parent = true;
        }

        if( !Forward( p2, GET_WINDOW_HANDLE, &handle, NULL ) ) {
            WdeWriteTrail( "WdeControlNotify: GET_WINDOW_HANDLE failed!" );
            return( false );
        }

        if( obj->parent_handle != handle ) {
            parent_changed = true;
        }

        if( new_parent ) {
            obj->parent = p2;
            obj->parent_handle = handle;
            if( !Forward( obj->parent, GET_FONT, &obj->font, NULL ) ) {
                WdeWriteTrail( "WdeControlNotify: GET_FONT failed!" );
                return( false );
            }
            parent_changed = true;
        }

        if( parent_changed ) {
            if( obj->window_handle != NULL ) {
                if( !WdeControlDestroyWindow ( obj, NULL, NULL ) ) {
                    WdeWriteTrail( "WdeControlNotify: DESTROY_WINDOW failed!" );
                    return( false );
                }
                if( !Forward( obj->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
                    WdeWriteTrail("WdeControlNotify: CREATE_WINDOW failed!" );
                    return( false );
                }
            }
        }
        return( Notify( obj->o_item, *noteid, obj->parent ) );

    case SET_LOCATION:
        if( obj->clear_interior ) {
            WdeSetClearObjectPos( obj );
        }
        return( WdeChangeControlSize( obj, false, false ) );
    }

    return( false );
}

/* NOTE: This function assumes that there was no change of parent */
static bool WdeOffsetDialogUnits ( WdeControlObject *obj, WdeResizeRatio *r )
{
    RECT                parent_pos;
    RECT                new_pos;
    RECT                nc_size;
    RECT                win_pos;
    WdeDialogSizeInfo   sizeinfo;

    Location( obj->object_handle, &new_pos );

    if( obj->parent != obj->base_obj ) {
        Location( obj->parent, &parent_pos );

        if( !Forward( obj->parent, GET_NC_SIZE, &nc_size, NULL ) ) {
            WdeWriteTrail( "WdeOffsetDialogUnits: GET_NC_SIZE failed!" );
            return( false );
        }

        win_pos.left = new_pos.left - parent_pos.left - nc_size.left;
        win_pos.top = new_pos.top  - parent_pos.top  - nc_size.top;
    } else {
        win_pos.left = new_pos.left;
        win_pos.top = new_pos.top;
    }

    win_pos.right = win_pos.left + (new_pos.right - new_pos.left);
    win_pos.bottom = win_pos.top + (new_pos.bottom - new_pos.top);

    if( WdeScreenToDialog( obj, r, &win_pos, &sizeinfo ) ) {
        SETCTL_SIZEX( obj->control_info, sizeinfo.x );
        SETCTL_SIZEY( obj->control_info, sizeinfo.y );
        return( true );
    }

    return( false );
}

bool WdeUpdateCDialogUnits( OBJPTR cobj, RECT *new, WdeResizeRatio *r )
{
    WdeDialogSizeInfo   sizeinfo;

    /* save the old dialog units */
    sizeinfo = GETCTL_SIZE( ((WdeControlObject *)cobj)->control_info );

    if( !WdeScreenToDialog( cobj, r, new, GETCTL_PSIZE( ((WdeControlObject *)cobj)->control_info ) ) ) {
        /* restore the old dialog units */
        SETCTL_SIZE( ((WdeControlObject *)cobj)->control_info, sizeinfo );
        return( false );
    }

    return( true );
}

bool WdeControlResize( WdeControlObject *obj, RECT *new_pos, bool *flag )
{
    RECT           object_rect;
    RECT           nc_size;
    RECT           parent_rect;
    RECT           rect;
    POINT          origin;
    POINT          pnt;
    bool           error;
    WdeResizeRatio resizer;
    int            min_width;
    int            min_height;

    if( *flag && obj->mode != WdeSelect ) {
        return( false );
    }

    if( obj->parent == NULL ) {
        WdeWriteTrail( "WdeDialogResize: obj has no parent!" );
        return( false );
    }

    if( !Forward( obj->parent, GET_RESIZER, &resizer, NULL ) ) {
        WdeWriteTrail( "WdeDialogResize: GET_RESIZER failed!" );
        return( false );
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
        return( false );
    }

    if( !*flag ) {
        error = false;
        if( obj->parent != obj->base_obj ) {
            Location( obj->parent, &parent_rect );
            if( !Forward( obj->parent, GET_NC_SIZE, &nc_size, NULL ) ) {
                WdeWriteTrail( "WdeDialogAddSubObject: Couldn't get nc size!" );
                error = true;
            }
            parent_rect.left += nc_size.left;
            parent_rect.right -= nc_size.right;
            parent_rect.top += nc_size.top;
            parent_rect.bottom -= nc_size.bottom;
            pnt.x = new_pos->left;
            pnt.y = new_pos->top;

            if( pnt.x < parent_rect.left || pnt.y < parent_rect.top ) {
                WdeWriteTrail( "WdeControlResize: control not contained by dialog top left!" );
                //error = true;
                error = false; // experiment -- don't cause a failure
            }
        }
        if( error ) {
            if( !Resize( obj->o_item, &object_rect, false ) ) {
                WdeWriteTrail( "WdeControlResize: O_ITEM RESIZE undo failed!" );
            }
            return( false );
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
        WdeUpdateCDialogUnits( (OBJPTR)obj, &rect, &resizer );
        if( !WdeChangeControlSize( obj, false, false ) ) {
            WdeWriteTrail( "WdeControlResize: WdeChangeControlSize failed!" );
            return( false );
        }
        if( obj->parent == obj->base_obj ) {
            WdeCheckBaseScrollbars( false );
        }
    }

    return( true );
}

bool WdeControlMove( WdeControlObject *obj, POINT *off, bool *forms_called )
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

    ok = true;

    old_parent = obj->parent;

    Location( obj->object_handle, &old_pos );

    /* update the location of the object */
    if( !Move( obj->o_item, &offset, *forms_called ) ) {
        return( false );
    }

    Location( obj->object_handle, &object_rect );

    if( !*forms_called ) {
        Location( obj->parent, &parent_rect );
        if( obj->parent != obj->base_obj ) {
            if( !Forward( obj->parent, GET_NC_SIZE, &nc_size, NULL ) ) {
                WdeWriteTrail( "WdeDialogAddSubObject: Couldn't get nc size!" );
                ok = false;
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
                    ok = false;
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
                ok = false;
            }
            if( ok ) {
                if( obj->parent != obj->base_obj ) {
                    OffsetRect( &object_rect, -origin.x, -origin.y );
                    WdeMapWindowRect( obj->res_info->edit_win,
                                      obj->parent_handle, &object_rect );
                }
                SETCTL_SIZEX( obj->control_info, MulDiv( object_rect.left, 4, resizer.xmap ) );
                SETCTL_SIZEY( obj->control_info, MulDiv( object_rect.top, 8, resizer.ymap ) );
            }

            if( obj->symbol != NULL ) {
                WdeAddSymbolToObjectHashTable( obj->res_info, obj->symbol,
                                               GETCTL_ID( obj->control_info ) );
            }
        }

        if( ok && !WdeChangeControlSize( obj, false, true ) ) {
            WdeWriteTrail( "WdeControlMove: WdeChangeControlSize failed!" );
            ok = false;
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
            ok = false;
        }
    }

    if( !ok ) {
        if( *forms_called ) {
            RemoveObject( obj->parent, obj->object_handle );
        }
        if( !Resize( obj->o_item, &old_pos, false ) ) {
            WdeWriteTrail( "WdeControlMove: O_ITEM RESIZE undo failed!" );
        }
    } else if( ok && *forms_called ) {
        WdeControlModified( obj );
        if( old_parent == obj->base_obj || obj->parent == obj->base_obj ) {
            WdeCheckBaseScrollbars( false );
        }
    }

    return( ok );
}

bool WdeControlGetObjectInfo( WdeControlObject *obj, void **_info, void **s )
{
    WdeDialogBoxControl **info = (WdeDialogBoxControl **)_info;

    if( info != NULL ) {
        *info = obj->control_info;
    }

    if( s != NULL ) {
        *s = obj->symbol;
    }

    return( true );
}

bool WdeControlSetObjectInfo( WdeControlObject *obj, void *_info, void *s )
{
    WdeDialogBoxControl *info = _info;
    RECT                size;
    POINT               origin;
    WdeResizeRatio      resizer;

    WdeControlModified( obj );

    if( obj->symbol != NULL ) {
        WRMemFree( obj->symbol );
    }
    obj->symbol = WdeStrDup( s );


    if( info == NULL ) {
        return( true );
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
            return( false );
        }

        if( !WdeScreenToDialog( obj, &resizer, &size, GETCTL_PSIZE( obj->control_info ) ) ) {
            WdeWriteTrail( "WdeControlSetControlInfo: WdeScreenToDialog failed!" );
            return( false );
        }

    }

    return( true );
}

bool WdeControlSetObjectHelpInfo( WdeControlObject *obj, void *info, char *hs )
{
//    WdeDialogBoxControl *info = _info;
    _wde_touch( info );

    if( obj->helpsymbol != NULL ) {
        WRMemFree( obj->helpsymbol );
    }
    obj->helpsymbol = WdeStrDup( hs );

    return( true );
}

bool WdeControlGetObjectHelpInfo( WdeControlObject *obj, void **_info, char **hs )
{
    WdeDialogBoxControl **info = (WdeDialogBoxControl **)_info;

    // this is redundant
    if( info != NULL ) {
        *info = obj->control_info;
    }

    // this is needed
    if( hs != NULL ) {
        *hs = obj->helpsymbol;
    }

    return( true );
}

bool WdeControlResolveSymbol( WdeControlObject *obj, bool *b, bool *from_id )
{
    WdeHashValue        val;
    void                *vp;
    bool                found;

    if( obj->control_info != NULL && obj->res_info != NULL ) {
        if( from_id != NULL && *from_id ) {
            vp = WdeResolveValue( obj->res_info->hash_table, (WdeHashValue)GETCTL_ID( obj->control_info ) );
            if( vp != NULL ) {
                if( obj->symbol != NULL ) {
                    WRMemFree( obj->symbol );
                }
                obj->symbol = vp;
                WdeControlModified( obj );
            }
        } else {
            if( obj->symbol != NULL ) {
                val = WdeLookupName( obj->res_info->hash_table, obj->symbol, &found );
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
        return( false );
    }

    return( true );
}

bool WdeControlResolveHelpSymbol( WdeControlObject *obj, bool *b, bool *from_id )
{
    WdeHashValue        val;
    void                *vp;
    bool                found;

    if( obj->control_info != NULL && obj->res_info != NULL ) {
        if( from_id != NULL && *from_id ) {
            vp = WdeResolveValue( obj->res_info->hash_table, (WdeHashValue)GETCTL_HELPID( obj->control_info ) );
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
        return( false );
    }

    return( true );
}

bool WdeControlModifyInfo( WdeControlObject *obj, WdeInfoStruct *in, void *p2 )
{
    WdeHashEntry        *entry;
    char                *text;
    bool                dup;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( in->u.ctl.text ) {
        if( GETCTL_TEXT( obj->control_info ) ) {
            WRMemFree( GETCTL_TEXT( obj->control_info ) );
        }
        SETCTL_TEXT( obj->control_info, in->u.ctl.text );
        text = WdeResNameOrOrdinalToStr( GETCTL_TEXT( obj->control_info ), 10 );
        if( text != NULL ) {
            SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)(LPCSTR)text );
            WRMemFree( text );
        } else {
            SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)(LPCSTR)"" );
        }
    }

    if( obj->symbol != NULL ) {
        WRMemFree( obj->symbol );
    }

    obj->symbol = in->symbol;

    if( in->symbol != NULL ) {
        dup = false;
        entry = WdeDefAddHashEntry( obj->res_info->hash_table, obj->symbol, &dup );
        if( entry != NULL ) {
            SETCTL_ID( obj->control_info, (uint_16)entry->value );
        }
    } else {
        SETCTL_ID( obj->control_info, in->u.ctl.id );
    }

    WdeControlModified( obj );

    WdeBringControlToTop( obj );

    return( true );
}

bool WdeControlGetOrderMode( WdeControlObject *obj, WdeOrderMode *mode, WdeSetOrderLists *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *mode = obj->mode;

    return( true );
}

bool WdeControlSetOrderMode( WdeControlObject *obj, WdeOrderMode *mode, WdeSetOrderLists **l )
{
    WdeSetOrderStruct   *o;
    DialogStyle         style;

    style = GETCTL_STYLE( obj->control_info );

    if( obj->mode == WdeSelect ) {
        o = (WdeSetOrderStruct *)WRMemAlloc( sizeof( WdeSetOrderStruct ) );
        if( o == NULL ) {
            return( false );
        }
        o->old_oe = (WdeOrderedEntry *)ListElement( WdeFindOrderedEntry( (*l)->oldlist, obj->object_handle ) );
        o->new_oe = NULL;
        o->lists = *l;
        o->old_oe->mode = *mode;
        o->old_oe->pos_set = FALSE;
        o->old_oe->tab_set = ( (style & WS_TABSTOP) != 0 );
        o->old_oe->grp_set = ( (style & WS_GROUP) != 0 );
        o->res_info = obj->res_info;
        obj->tag = WdeCreateTag( obj->window_handle, o );
        obj->mode = *mode;
    } else if( obj->mode == *mode ) {
        o = WdeGetTagInfo( obj->tag );
        WdeDestroyTag( obj->tag );
        obj->tag = NULL;
        obj->mode = WdeSelect;
        if( o == NULL ) {
            return( false );
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
            return( false );
        }
        o->old_oe->mode = *mode;
        if( o->new_oe != NULL ) {
            o->new_oe->mode = *mode;
        }
        *l = o->lists;
        obj->mode = *mode;
    }

    SETCTL_STYLE( obj->control_info, style );

    return( true );
}

bool WdeControlSizeToText( WdeControlObject *obj, void *p1, void *p2 )
{
    SIZE        size;
    int         width;
    int         height;
    OBJ_ID      id;
    RECT        pos;
    bool        ok;

    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    ok = true;
    id = 0;
    size.cx = 0;
    size.cy = 0;
    width = 0;
    height = 0;
    SetRectEmpty( &pos );

    if( obj->parent_handle == (HWND)NULL ) {
        ok = ( Forward( obj->parent, GET_WINDOW_HANDLE, &obj->parent_handle, NULL ) && obj->parent_handle != (HWND)NULL );
    }

    if( ok ) {
        ok = Forward( obj->object_handle, IDENTIFY, &id, NULL );
    }

    if( ok ) {
        ok = WdeGetNameOrOrdSize( obj->parent, GETCTL_TEXT( obj->control_info ), &size );
    }

    if( ok ) {
        ok = false;
        switch( id ) {
        case PBUTTON_OBJ:
            SetRectEmpty( &pos );
            WdeChangeSizeToDefIfSmallRect( obj->parent, PBUTTON_OBJ, &pos );
            width = size.cx + (WDE_SIZE_TO_TEXT_PAD * 2);
            if( width < pos.right )
                width = pos.right;
            ok = true;
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
            ok = true;
            break;
        }
    }

    if( ok ) {
        ok = false;
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
            ok = true;
            break;
        }
    }

    if( ok ) {
        Location( obj->object_handle, &pos );
        pos.right = pos.left + width;
        pos.bottom = pos.top + height;
        HideSelectBoxes();
        ok = Resize( obj->object_handle, &pos, true );
        ShowSelectBoxes();
    }

    if( ok ) {
        WdeWriteControlToInfo( obj );
    }

    return( ok );
}

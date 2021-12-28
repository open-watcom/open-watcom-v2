/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <mbstring.h>
#include "wdeactn.h"
#include "wderes.h"
#include "wdefordr.h"
#include "wdefmenu.h"
#include "wdefont.h"
#include "wdemsgbx.h"
#include "wdeedit.h"
#include "wderesiz.h"
#include "wdeinfo.h"
#include "wdemain.h"
#include "wdesym.h"
#include "wdefutil.h"
#include "wdectool.h"
#include "wdelist.h"
#include "wdealign.h"
#include "wdeopts.h"
#include "wde.rh"
#include "wdedebug.h"
#include "wdefbase.h"
#include "wdedispa.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_WORLD_PAD     20
#define WDE_WORLD_WIDTH   1024
#define WDE_WORLD_HEIGHT  WDE_WORLD_WIDTH

#define pick_ACTS(o) \
    pick_ACTION_DRAW(o,pick) \
    pick_ACTION_LOCATE(o,pick) \
    pick_ACTION_DESTROY(o,pick) \
    pick_ACTION_VALIDATE_ACTION(o,pick) \
    pick_ACTION_NOTIFY(o,pick) \
    pick_ACTION_RESIZE_INFO(o,pick) \
    pick_ACTION_FIND_SUBOBJECTS(o,pick) \
    pick_ACTION_FIND_OBJECTS_PT(o,pick) \
    pick_ACTION_ADD_SUBOBJECT(o,pick) \
    pick_ACTION_REMOVE_SUBOBJECT(o,pick) \
    pick_ACTION_GET_WINDOW_HANDLE(o,pick) \
    pick_ACTION_GET_SUBOBJ_LIST(o,pick) \
    pick_ACTION_IDENTIFY(o,pick) \
    pick_ACTION_GET_FONT(o,pick) \
    pick_ACTION_GET_RESIZER(o,pick) \
    pick_ACTION_GET_NC_SIZE(o,pick) \
    pick_ACTION_BECOME_FIRST_CHILD(o,pick) \
    pick_ACTION_GET_FIRST_CHILD(o,pick) \
    pick_ACTION_PUT_ME_FIRST(o,pick) \
    pick_ACTION_GET_SCROLL_RECT(o,pick) \
    pick_ACTION_GET_RESIZE_INC(o,pick) \
    pick_ACTION_IS_MARK_VALID(o,pick) \
    pick_ACTION_RESOLVE_SYMBOL(o,pick) \
    pick_ACTION_RESOLVE_HELPSYMBOL(o,pick) \
    pick_ACTION_GET_NEXT_CHILD(o,pick)

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    DISPATCH_FN         *dispatcher;
    OBJPTR              object_handle;
    OBJ_ID              object_id;
    OBJPTR              parent;
    OBJPTR              o_item;
    HFONT               font;
    int                 num_children;
    LIST                *children;
    LIST                *ochildren;
    WdeResizeRatio      resizer;
    WdeResInfo          *res_info;
    bool                has_hscroll;
    bool                has_vscroll;
    bool                in_destroy;
} WdeBaseObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT bool   CALLBACK WdeBaseDispatcher( ACTION_ID, OBJPTR, void *, void * );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

#define pick(e,n,c) static bool WdeBase ## n ## c;
    pick_ACTS( WdeBaseObject )
#undef pick

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static DISPATCH_FN  *WdeBaseDispatch;

static DISPATCH_ITEM WdeBaseActions[] = {
    #define pick(e,n,c) {e, (DISPATCH_RTN *)WdeBase ## n},
    pick_ACTS( WdeBaseObject )
    #undef pick
};

#define MAX_ACTIONS      (sizeof( WdeBaseActions ) / sizeof ( DISPATCH_ITEM ))

OBJPTR CALLBACK WdeBaseCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    RECT                rect;
    WdeBaseObject       *new;
    char                *font_facename;
    char                *cp;
    int                 font_pointsize;
    bool                use_default;

    /* touch unused vars to get rid of warning */
    _wde_touch( obj_rect );

    WdeDebugCreate( "Base", parent, obj_rect, handle );

    new = (WdeBaseObject *)WRMemAlloc( sizeof( WdeBaseObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeBaseCreate: Malloc failed" );
        return( (OBJPTR)new );
    }
    memset( new, 0, sizeof( WdeBaseObject ) );

    new->object_id = BASE_OBJ;
    new->parent = parent;
    new->font = WdeGetEditFont();
    OBJ_DISPATCHER_SET( new, WdeBaseDispatch );
    new->res_info = WdeGetCurrentRes();

    if( new->font == NULL || new->res_info == NULL ) {
        WdeWriteTrail( "WdeBaseCreate: NULL font or NULL res_info!" );
        return( NULL );
    }

    if( new->res_info->edit_win != NULL ) {
        GetClientRect( new->res_info->edit_win, &rect );
    } else {
        SetRectEmpty( &rect );
    }
    InflateRect( &rect, WDE_WORLD_PAD, WDE_WORLD_PAD );

    if( handle ==  NULL ) {
        new->object_handle = (OBJPTR)new;
    } else {
        new->object_handle = handle;
    }

    new->o_item = Create( OBJ_ITEM, parent, &rect, new->object_handle );

    use_default = true;
    font_facename = WdeAllocRCString( WDE_BASEOBJECTFONT );
    if( font_facename != NULL ) {
        cp = (char *)_mbschr( (unsigned char const *)font_facename, '.' );
        if( cp ) {
            *cp = '\0';
            cp++;
            font_pointsize = atoi( cp );
            use_default = false;
        }
    }

    if( use_default ) {
        WdeGetResizerFromFont( &new->resizer, "Helv", 8 );
    } else {
        WdeGetResizerFromFont( &new->resizer, font_facename, font_pointsize );
    }

    if( font_facename != NULL ) {
        WdeFreeRCString( font_facename );
    }

    return( (OBJPTR)new );
}

bool CALLBACK WdeBaseDispatcher( ACTION_ID act, OBJPTR obj, void *p1, void *p2 )
{
    int i;

    WdeDebugDispatch( "Base", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeBaseActions[i].id == act ) {
            return( WdeBaseActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( ((WdeBaseObject *)obj)->o_item, act, p1, p2 ) );
}

bool WdeBaseInit( bool first )
{
    _wde_touch( first );
    WdeBaseDispatch = MakeProcInstance_DISPATCHER( WdeBaseDispatcher, WdeGetAppInstance() );
    return( true );
}

void WdeBaseFini( void )
{
    FreeProcInstance_DISPATCHER( WdeBaseDispatch );
}

bool WdeBaseIsMarkValid( WdeBaseObject *obj, bool *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );
    _wde_touch( obj );

    *flag = false;

    return( true );
}

bool WdeBaseGetNextChild( WdeBaseObject *obj, OBJPTR *o, bool *up )
{
    return( WdeGetNextChild( &obj->ochildren, o, *up ) );
}

bool WdeBaseResolveSymbol( WdeBaseObject *obj, bool *b, bool *from_id )
{
    LIST    *olist;
    OBJPTR  child;

    if( obj->res_info->hash_table ) {
        for( olist = obj->children; olist; olist = ListNext( olist ) ) {
            child = ListElement( olist );
            Forward( child, RESOLVE_SYMBOL, b, from_id );
        }
    }

    return( true );
}

bool WdeBaseResolveHelpSymbol( WdeBaseObject *obj, bool *b, bool *from_id )
{
    LIST    *olist;
    OBJPTR  child;

    if( obj->res_info->hash_table ) {
        for( olist = obj->children; olist; olist = ListNext( olist ) ) {
            child = ListElement( olist );
            Forward( child, RESOLVE_HELPSYMBOL, b, from_id );
        }
    }

    return( true );
}

bool WdeBaseDestroy( WdeBaseObject *obj, bool *flag, bool *p2 )
{
    OBJPTR  sub_obj;
    LIST    *clist;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );


    /* if the user initiated this destroy then don't do it! */
    if( *flag ) {
        return( false );
    }

    obj->in_destroy = true;

    /* destroy all children */
    clist = WdeListCopy( obj->children );
    for( ; clist != NULL; clist = ListConsume( clist ) ) {
        sub_obj = ListElement( clist );
        Destroy( sub_obj, *flag );
    }

    WdeFreeOrderedList( obj->ochildren );

    Destroy( obj->o_item, *flag );

    WRMemFree( obj );

    return( true );
}

bool WdeBaseGetScrollRect( WdeBaseObject *obj, RECT *r, void *p2 )
{
    return( WdeBaseLocation( obj, r, p2 ) );
}

bool WdeBaseGetResizeInc( WdeBaseObject *obj, POINT *p, void *p2 )
{
    WdeDialogSizeInfo   sizeinfo;
    RECT                r;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    p->x = 1;
    p->y = 1;

    sizeinfo.x = 0;
    sizeinfo.y = 0;
    sizeinfo.width = WdeGetOption( WdeOptReqGridX );
    sizeinfo.height = WdeGetOption( WdeOptReqGridY );

    if( WdeDialogToScreen( obj, &obj->resizer, &sizeinfo, &r ) ) {
        p->x = r.right;
        p->y = r.bottom;
    }

    return( true );
}

bool WdeBaseGetResizer( WdeBaseObject *obj, WdeResizeRatio *resizer, OBJPTR *o )
{
    *resizer = obj->resizer;
    if( o != NULL ) {
        *o = (OBJPTR)obj;
    }

    return( true );
}

bool WdeBaseGetFont( WdeBaseObject *obj, HFONT *font, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *font = obj->font;

    return( true );
}

bool WdeBaseGetNCSize( WdeBaseObject *obj, RECT *size, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    memset( size, 0, sizeof( RECT ) );

    return( true );
}

bool WdeBaseFirstChild( WdeBaseObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p1 );
    _wde_touch( p2 );

    return( true );
}

bool WdeBaseGetFirstChild( WdeBaseObject *obj, OBJPTR *first, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( obj->num_children == 0 || obj->children == NULL ) {
        *first = NULL;
    } else {
        *first = ListElement( obj->children );
    }

    return( true );
}

bool WdeBaseDraw( WdeBaseObject *obj, RECT *area, HDC *dc )
{
    POINT   origin;
    RECT    client_rect;
    RECT    intersect;
    LIST    *olist;
    OBJPTR  child;
    OBJPTR  tops;
    RECT    child_rect;
    bool    show_child;

    /* touch unused vars to get rid of warning */
    _wde_touch( dc );

    GetOffset( &origin );

    GetClientRect( obj->res_info->edit_win, &client_rect );

    OffsetRect( &client_rect, origin.x, origin.y );

    child = NULL;

    /* we must process the children in reverse order */
    if( obj->children ) {
        WdeListLastElt( obj->children, &olist );
    } else {
        olist = NULL;
    }

    tops = NULL;
    for( olist = obj->children; olist; olist = ListNext( olist ) ) {
        child = ListElement( olist );
        Location( child, &child_rect );
        show_child = ( IntersectRect( &intersect, &child_rect, &client_rect ) != 0 );
        Forward( child, SHOW_WIN, &show_child, NULL );
        if( !show_child ) {
            child = NULL;
        } else {
            if( !Forward( child, DRAW, area, NULL ) ) {
                WdeWriteTrail( "WdeBaseDraw: child draw failed!" );
            }
            if( tops == NULL ) {
                tops = child;
            }
        }
    }

    if( IntersectRect( &intersect, area, &client_rect ) ) {
        OffsetRect( &intersect, -origin.x, -origin.y );
        RedrawWindow( obj->res_info->edit_win, &intersect, (HRGN)NULL,
                      RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW );
        if( child && !Forward( tops, ON_TOP, NULL, NULL ) ) {
            WdeWriteTrail( "WdeBaseDraw: child ON_TOP failed!" );
            return( false );
        }
    }

    return( true );
}

bool WdeBaseGetResizeInfo( WdeBaseObject *obj, RESIZE_ID *info, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *info = R_NONE;

    return( true );
}

bool WdeBaseIdentify( WdeBaseObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( true );
}

bool WdeBaseValidateAction( WdeBaseObject *obj, ACTION_ID *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( *act == MOVE ) {
        return( false );
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeBaseActions[i].id == *act ) {
            return( true );
        }
    }

    return( ValidateAction( obj->o_item, *act, p2 ) );
}

bool WdeBaseGetWindowHandle( WdeBaseObject *obj, HWND *hwin, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *hwin = obj->res_info->edit_win;

    return( true );
}

bool WdeBaseAddSubObject( WdeBaseObject *base_obj, OBJPTR obj, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    /* make sure objects are inserted at the end of list */
    WdeInsertObject( &base_obj->children, obj );
    WdeAddOrderedEntry( &base_obj->ochildren, obj );
    base_obj->num_children++;
    Notify( obj, NEW_PARENT, base_obj->object_handle );
    WdeCheckBaseScrollbars( false );

    return( true );
}

bool WdeBaseFindSubObjects( WdeBaseObject *obj, SUBOBJ_REQUEST *req, LIST **obj_list )
{
    return( WdeFindSubObjects( req, obj_list, obj->children ) );
}

bool WdeBaseFindObjectsPt( WdeBaseObject *obj, POINT *pt, LIST **obj_list )
{
    LIST    *subobjs;
    OBJPTR  child;
    OBJ_ID  id;

    if( WdeFindObjectsAtPt( pt, &subobjs, obj->children ) ) {
        child = ListElement( subobjs );
        if( !(Forward( child, IDENTIFY, &id, NULL ) && id == DIALOG_OBJ) ||
            !Forward( child, FIND_OBJECTS_PT, pt, obj_list ) ) {
            ListAddElt( obj_list, child );
        }
        ListFree( subobjs );
    } else {
        ListAddElt( obj_list, (OBJPTR)obj );
    }

    return( true );
}

bool WdeBaseRemoveSubObject( WdeBaseObject *base_obj, OBJPTR obj, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( base_obj->num_children && ListFindElt( base_obj->children, obj ) ) {
        ListRemoveElt( &base_obj->children, obj );
        WdeRemoveOrderedEntry( base_obj->ochildren, obj );
        base_obj->num_children--;
        if( base_obj->num_children == 0 ) {
            base_obj->children = NULL;
            base_obj->res_info->next_current = NULL;
        }
    } else {
        return( false );
    }

    return( true );
}

bool WdeBasePutChildFirst( WdeBaseObject *obj, OBJPTR child, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( obj->num_children ) {
        if( !WdePutObjFirst( child, &obj->children ) ) {
            WdeWriteTrail( "WdeBasePutChildFirst: WdePutObjFirst failed!" );
            return( false );
        }
    } else {
        WdeWriteTrail( "WdeBasePutChildFirst: No children!" );
        return( false );
    }

    return( true );
}

bool WdeBaseNotify( WdeBaseObject *base_obj, NOTE_ID *noteid, void *p2 )
{
    OBJPTR           obj;
    WdeInfoStruct    is;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    switch( *noteid ) {
    case PRIMARY_OBJECT:
        obj = base_obj->res_info->next_current;
        if( obj != NULL && obj != (OBJPTR)base_obj ) {
            MakeObjectCurrent( obj );
        } else if( base_obj->num_children != 0 ) {
            obj = ListElement( base_obj->children );
            MakeObjectCurrent( obj );
        } else {
            WdeSetBaseObject( IDM_SELECT_MODE );
            WdeSetBaseObjectMenu( base_obj->res_info->hash_table != NULL );

            memset ( &is, 0, sizeof( WdeInfoStruct ) );

            is.obj_id = BASE_OBJ;
            is.obj = base_obj->object_handle;
            is.res_info = base_obj->res_info;

            WdeWriteInfo( &is );
        }
        return ( true );
    }

    return( Notify( base_obj->o_item, *noteid, p2 ) );
}

bool WdeBaseLocation( WdeBaseObject *base_obj, RECT *obj_rect, void *p2 )
{
    RECT onion;
    RECT rect1;
    RECT rect2;

    /* touch unused vars to get rid of warning */
    _wde_touch( base_obj );
    _wde_touch( p2 );

    if( base_obj->res_info->edit_win != NULL ) {
        GetClientRect( base_obj->res_info->edit_win, &rect1 );
        if( IsRectEmpty( &rect1 ) ) {
            rect1.right++;
            rect1.bottom++;
        }
    } else {
        SetRectEmpty( &rect1 );
    }

    WdeGetBoundingRectFromList( base_obj->children, &rect2 );
    InflateRect( &rect2, WDE_WORLD_PAD, WDE_WORLD_PAD );

    UnionRect( &onion, &rect1, &rect2 );

    *obj_rect = onion;

    return( true );
}

bool WdeBaseGetSubObjectList( WdeBaseObject *base_obj, LIST **l, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *l = base_obj->children;

    return( true );
}

static void isWorldInBaseClient( WdeBaseObject *base_obj, bool *x_in, bool *y_in )
{
    RECT        client_rect;
    RECT        bounding_rect;

    *x_in = false;
    *y_in = false;

    if( base_obj->res_info->edit_win == NULL ) {
        return;
    }

    if( base_obj->children == NULL ) {
        return;
    }

    GetClientRect( base_obj->res_info->forms_win, &client_rect );

    WdeGetBoundingRectFromList( base_obj->children, &bounding_rect );
    InflateRect( &bounding_rect, WDE_WORLD_PAD, WDE_WORLD_PAD );

    if( bounding_rect.left < client_rect.left || bounding_rect.right > client_rect.right ) {
        *x_in = true;
    }

    if( bounding_rect.top < client_rect.top || bounding_rect.bottom > client_rect.bottom ) {
        *y_in = true;
    }
}

static bool enableFormsScrollbars( WdeBaseObject *base_obj, bool enablex, bool enabley )
{
    STATE_HDL   state_handle;
    SCR_CONFIG  scroll_config;

    state_handle = GetCurrFormID();
    if( state_handle == 0 ) {
        return( false );
    }

    scroll_config = SCROLL_NONE;

    if( enablex ) {
        scroll_config |= SCROLL_HORZ;
    }

    if( enabley ) {
        scroll_config |= SCROLL_VERT;
    }

    SetFormEditWnd( state_handle, base_obj->res_info->forms_win, MENU_NONE, scroll_config );

    return( true );
}

bool WdeCheckBaseScrollbars( bool in_resize )
{
    WdeBaseObject       *base_obj;
    bool                enablex;
    bool                enabley;
    RECT                rect;

    if( WdeInCleanup() ) {
        return( true );
    }

    base_obj = (WdeBaseObject *)GetMainObject();

    if( !base_obj || !base_obj->res_info ) {
        return( false );
    }

    if( base_obj->in_destroy ) {
        return( true );
    }

    enablex = false;
    enabley = false;

    isWorldInBaseClient( base_obj, &enablex, &enabley );

    if( enablex == base_obj->has_hscroll && enabley == base_obj->has_vscroll ) {
        return( true );
    }

    if( enablex != base_obj->has_hscroll ) {
        if( !enablex ) {
            SetScrollPos( base_obj->res_info->forms_win, SB_HORZ, 0, FALSE );
        }
        SetScrollRange( base_obj->res_info->forms_win, SB_HORZ, 0, enablex ? 100 : 0, FALSE );
        ShowScrollBar( base_obj->res_info->forms_win, SB_HORZ, enablex );
    }

    if( enabley != base_obj->has_vscroll ) {
        if( !enabley ) {
            SetScrollPos( base_obj->res_info->forms_win, SB_VERT, 0, FALSE );
        }
        SetScrollRange( base_obj->res_info->forms_win, SB_VERT, 0, enabley ? 100 : 0, FALSE );
        ShowScrollBar( base_obj->res_info->forms_win, SB_VERT, enabley );
    }

    enableFormsScrollbars( base_obj, enablex, enabley );

    if( !in_resize ) {
        GetClientRect( base_obj->res_info->forms_win, &rect );
        MoveWindow( base_obj->res_info->edit_win, 0, 0, rect.right - rect.left, rect.bottom - rect.top, TRUE );
    }

    UpdateScroll();

    base_obj->has_hscroll = enablex;
    base_obj->has_vscroll = enabley;

    return( true );
}

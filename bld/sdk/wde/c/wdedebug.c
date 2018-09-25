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
#include <ctype.h>
#include "wdestat.h"
#include "wdemain.h"
#include "wdedebug.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
#ifdef WDE_DEBUG

#ifdef WDE_DEBUG_DISPATCH
typedef enum {
    wdet_null,
    wdet_P1,
    wdet_P2,
    wdet_Point,
    wdet_boolean,
    wdet_Rect,
    wdet_Req,
    wdet_Listp,
    wdet_Hdc,
    wdet_Note_id,
    wdet_Action,
    wdet_Objptr,
    wdet_Char,
    wdet_Objptrp,
    wdet_Int,
    wdet_Hwnd,
    wdet_Str,
    wdet_Infop,
    wdet_Info,
    wdet_Obj_id,
    wdet_Hfont,
    wdet_Uint32,
    wdet_Uint16,
    wdet_Fileid
} WdeDebugParamTypes;

static const char *WdeDebugParamNames[] = {
    "NULL",
    "P1",
    "P2",
    "POINT",
    "BOOL",
    "RECT",
    "REQ",
    "LISTP",
    "HDC",
    "NOTE_ID",
    "ACTION",
    "OBJPTR",
    "CHAR",
    "OBJPTRP",
    "INT",
    "HWND",
    "STR",
    "INFOP",
    "INFO",
    "OBJ_ID",
    "HFONT",
    "UINT32",
    "UINT16",
    "FILEID"
};

typedef struct {
    ACTION_ID           act;
    const char          *act_name;
    WdeDebugParamTypes  p1;
    WdeDebugParamTypes  p2;
} WdeDebugActionItem;

static WdeDebugActionItem WdeDebugActions[] = {
    { REGISTER,           "REGISTER          ", wdet_P1,      wdet_P2      },
    { MOVE,               "MOVE              ", wdet_Point,   wdet_boolean },
    { RESIZE,             "RESIZE            ", wdet_Rect,    wdet_boolean },
    { LOCATE,             "LOCATE            ", wdet_Rect,    wdet_P2      },
    { FIND_SUBOBJECTS,    "FIND_SUBOBJECTS   ", wdet_Req,     wdet_Listp   },
    { DESTROY,            "DESTROY           ", wdet_boolean, wdet_P2      },
    { DRAW,               "DRAW              ", wdet_Rect,    wdet_Hdc     },
    { DEFINE,             "DEFINE            ", wdet_Point,   wdet_P2      },
    { NOTIFY,             "NOTIFY            ", wdet_Note_id, wdet_P2      },
    { FORWARD,            "FORWARD           ", wdet_Action,  wdet_P2      },
    { VALIDATE_ACTION,    "VALIDATE_ACTION   ", wdet_Action,  wdet_P2      },
    { ADD_SUBOBJECT,      "ADD_SUBOBJECT     ", wdet_Objptr,  wdet_P2      },
    { REMOVE_SUBOBJECT,   "REMOVE_SUBOBJECT  ", wdet_Objptr,  wdet_P2      },
    { RESIZE_INFO,        "RESIZE_INFO       ", wdet_Int,     wdet_P2      },
    { CUT,                "CUT               ", wdet_Objptrp, wdet_P2      },
    { COPY,               "COPY              ", wdet_Objptrp, wdet_Objptr  },
    { PASTE,              "PASTE             ", wdet_Objptr,  wdet_Point   },
    { GET_PARENT,         "GET_PARENT        ", wdet_Objptr,  wdet_P2      },
    { GET_PRIORITY,       "GET_PRIORITY      ", wdet_Int,     wdet_P2      },
    { GET_RESIZE_INC,     "GET_RESIZE_INC    ", wdet_Point,   wdet_P2      },
    { GET_SCROLL_RECT,    "GET_SCROLL_RECT   ", wdet_Rect,    wdet_P2      },
    { IS_MARK_VALID,      "IS_MARK_VALID     ", wdet_boolean, wdet_P2      },
    { FIND_OBJECTS_PT,    "FIND_OBJECTS_PT   ", wdet_Point,   wdet_Listp   },
    { GET_WINDOW_HANDLE,  "GET_WINDOW_HANDLE ", wdet_Hwnd,    wdet_P2      },
    { GET_WINDOW_CLASS,   "GET_WINDOW_CLASS  ", wdet_Str,     wdet_P2      },
    { GET_SUBOBJ_LIST,    "GET_SUBOBJ_LIST   ", wdet_Listp,   wdet_P2      },
    { CREATE_WINDOW,      "CREATE_WINDOW     ", wdet_P1,      wdet_P2      },
    { DESTROY_WINDOW,     "DESTROY_WINDOW    ", wdet_boolean, wdet_boolean },
    { SHOW_WIN,           "SHOW_WIN          ", wdet_boolean, wdet_P2      },
    { GET_OBJECT_INFO,    "GET_OBJECT_INFO   ", wdet_Infop,   wdet_P2      },
    { SET_OBJECT_INFO,    "SET_OBJECT_INFO   ", wdet_Info,    wdet_P2      },
    { IDENTIFY,           "IDENTIFY          ", wdet_Obj_id,  wdet_P2      },
    { GET_RESIZER,        "GET_RESIZER       ", wdet_P1,      wdet_Objptrp },
    { GET_FONT,           "GET_FONT          ", wdet_Hfont,   wdet_P2      },
    { SET_FONT,           "SET_FONT          ", wdet_Hfont,   wdet_P2      },
    { BECOME_FIRST_CHILD, "BECOME_FIRST_CHILD", wdet_P1,      wdet_P2      },
    { PUT_ME_FIRST,       "PUT_ME_FIRST      ", wdet_Objptr,  wdet_P2      },
    { GET_FIRST_CHILD,    "GET_FIRST_CHILD   ", wdet_Objptr,  wdet_P2      },
    { GET_NC_SIZE,        "GET_NC_SIZE       ", wdet_Rect,    wdet_P2      },
    { SAVE_OBJECT,        "SAVE_OBJECT       ", wdet_Uint32,  wdet_P2      },
    { ON_TOP,             "ON_TOP            ", wdet_P1,      wdet_P2      },
    { TEST,               "TEST              ", wdet_P1,      wdet_P2      },
    { IS_OBJECT_CLEAR,    "IS_OBJECT_CLEAR   ", wdet_boolean, wdet_P2      },
    { SET_CLEAR_INT,      "SET_CLEAR_INT     ", wdet_boolean, wdet_P2      },
    { RESTORE_OBJECT,     "RESTORE_OBJECT    ", wdet_P1,      wdet_P2      },
    { RESOLVE_SYMBOL,     "RESOLVE_SYMBOL    ", wdet_boolean, wdet_P2      },
    { MODIFY_INFO,        "MODIFY_INFO       ", wdet_P1,      wdet_P2      },
    { GET_NEXT_CHILD,     "GET_NEXT_CHILD    ", wdet_Objptrp, wdet_boolean },
    { SET_ORDER_MODE,     "SET_ORDER_MODE    ", wdet_Int,     wdet_Int     },
    { SIZE_TO_TEXT,       "SIZE_TO_TEXT      ", wdet_P1,      wdet_P2      },
    { -1,                 (const char *)NULL,   wdet_null,    wdet_null    }
};

#endif

#endif

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
#ifdef WDE_DEBUG
#ifdef WDE_DEBUG_DISPATCH
static WdeDebugActionItem *WdeDebugGetActionItem( ACTION_ID );
static void WdeDebugInterpretParam( WdeDebugParamTypes, void *, char * );
#endif
#endif

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
#if defined( WDE_DEBUG )
static bool WdeTrailOpen = false;
static FILE *WdeTrailFile = NULL;
#if defined( WDE_DEBUG_DISPATCH ) || defined( WDE_DEBUG_CREATE )
static char WdeDebugString[512];
#endif
#endif

#if defined( WDE_DEBUG ) && defined( WDE_DEBUG_DISPATCH )
static WdeDebugActionItem WdeDebugDefaultAction = { -1 , "" , wdet_P1, wdet_P2 };

static WdeDebugActionItem *WdeDebugGetActionItem( ACTION_ID act )
{
    int         i;

    for( i = 0; WdeDebugActions[i].act != -1; i++ ) {
        if( WdeDebugActions[i].act == act ) {
            return( &WdeDebugActions[i] );
        }
    }

    return( &WdeDebugDefaultAction );
}

static void WdeDebugInterpretParam( WdeDebugParamTypes type, void *param, char *temp )
{
    WdeDebugActionItem *act_item;

    if( param == NULL ) {
        sprintf( temp, "%7s: NULL, ", WdeDebugParamNames[type] );
        return;
    }

    switch( type ) {
    case wdet_P1:
    case wdet_P2:
    case wdet_Objptr:
    case wdet_Info:
        sprintf( temp, "%7s: %p, ", WdeDebugParamNames[type], (void *)param );
        break;

    case wdet_Listp:
    case wdet_Infop:
    case wdet_Objptrp:
    case wdet_Str:
        if( *(int **)param == NULL ) {
            sprintf( temp, "%7s: NULL, ", WdeDebugParamNames[type] );
        } else {
            sprintf( temp, "%7s: %p, ", WdeDebugParamNames[type], (void *)*(int **)param );
        }
        break;

    case wdet_boolean:
        if ( *(bool *)param ) {
            sprintf( temp, "%7s: TRUE, ", WdeDebugParamNames[type] );
        } else {
            sprintf( temp, "%7s: FALSE, ", WdeDebugParamNames[type] );
        }
        break;

    case wdet_Int:
    case wdet_Uint32:
    case wdet_Uint16:
    case wdet_Fileid:
    case wdet_Obj_id:
    case wdet_Note_id:
    case wdet_Hwnd:
    case wdet_Hdc:
    case wdet_Hfont:
        sprintf( temp, "%7s: %d, ", WdeDebugParamNames[type], *(int *)param );
        break;

    case wdet_Char:
        sprintf( temp, "%7s: %c, ", WdeDebugParamNames[type], *(char *)param );
        break;

    case wdet_Rect:
        sprintf( temp, "%7s: (%d,%d),(%d,%d), ", WdeDebugParamNames[type],
                 ((RECT *)param)->left, ((RECT *)param)->top,
                 ((RECT *)param)->right, ((RECT *)param)->bottom );
        break;

    case wdet_Point:
        sprintf( temp, "%7s: (%d,%d), ", WdeDebugParamNames[type],
                 ((POINT *)param)->x, ((POINT *)param)->y );
        break;

    case wdet_Action:
        act_item = WdeDebugGetActionItem( *(ACTION_ID *)param );
        if( act_item->act == -1 ) {
            sprintf( temp, "%7s: %21.21d, ", WdeDebugParamNames[type], *(ACTION_ID *)param );
        } else {
            sprintf( temp, "%7s: %s, ", WdeDebugParamNames[type], act_item->act_name );
        }
        break;

    case wdet_Req:
        if ( ((SUBOBJ_REQUEST *)param)->a.ty == ALL ) {
            sprintf( temp, "%7s: ALL, ", WdeDebugParamNames[type] );
        } else if( ((SUBOBJ_REQUEST *)param)->a.ty == BY_POINT ) {
            sprintf( temp, "%7s: BY_POINT (%d,%d), ", WdeDebugParamNames[type],
                     ((SUBOBJ_REQUEST *)param)->p.pt.x, ((SUBOBJ_REQUEST *)param)->p.pt.y );
        } else {
            sprintf( temp, "%7s: AT_POINT (%d,%d), ", WdeDebugParamNames[type],
                     ((SUBOBJ_REQUEST *)param)->p.pt.x, ((SUBOBJ_REQUEST *)param)->p.pt.y );
        }
        break;

    default:
        sprintf( temp, "%7s: %p, ", WdeDebugParamNames[type], (void *)param );
    }

    return;
}
#endif

#if defined( WDE_DEBUG ) && defined( WDE_DEBUG_DISPATCH )
void WdeDebugDispatch( const char *name, ACTION_ID act, void * obj, void *p1, void *p2 )
{
    WdeDebugActionItem  *act_item;
    char                temp[512];

    if ( act != LOCATE && act != RESIZE_INFO ) {

        act_item = WdeDebugGetActionItem( act );

        if( act_item->act == -1 ) {
            sprintf( WdeDebugString, "%7s: %21.21d, ", name, act );
        } else {
            sprintf( WdeDebugString, "%7s: %s, ", name, act_item->act_name );
        }

        if( obj == NULL ) {
            strcat( WdeDebugString, "OBJ: NULL, " );
        } else {
            sprintf( temp, "OBJ: %p, ", (void *)obj );
            strcat( WdeDebugString, temp );
        }

        WdeDebugInterpretParam( act_item->p1, p1, temp );
        strcat( WdeDebugString, temp );

        WdeDebugInterpretParam( act_item->p2, p2, temp );
        strcat( WdeDebugString, temp );

        WdeWriteTrail( WdeDebugString );

    }

}
#endif

#if defined( WDE_DEBUG ) && defined( WDE_DEBUG_CREATE )
void WdeDebugCreate( const char *name, OBJPTR parent, RECT *obj_rect, void * handle )
{
    char        temp[512];

    sprintf( WdeDebugString, "%7s: Create: ", name );

    if( parent == NULL ) {
        strcat( WdeDebugString, "Parent: NULL," );
    } else {
        sprintf( temp, "Handle: %p, ", (void *)parent );
        strcat( WdeDebugString, temp );
    }

    if( handle == NULL ) {
        strcat( WdeDebugString, "Handle: NULL, " );
    } else {
        sprintf( temp, "Handle: %p, ", (void *)handle );
        strcat( WdeDebugString, temp );
    }

    if( obj_rect == NULL ) {
        strcat( WdeDebugString, "Rect: NULL, " );
    } else {
        sprintf( temp, "Rect: (%d,%d),(%d,%d), ",
                 obj_rect->left, obj_rect->top, obj_rect->right, obj_rect->bottom );
        strcat( WdeDebugString, temp );
    }

    WdeWriteTrail( WdeDebugString );
}
#endif

#ifdef WDE_DEBUG
/* routine to write a trail string */
bool WdeWriteTrail( const char * msg )
{
    if( !WdeTrailOpen ) {
        WdeTrailFile = fopen( "d:\\src\\wde\\wde.trl", "wa" );
        WdeTrailOpen = true;
    } else {
        WdeTrailFile = fopen( "d:\\src\\wde\\wde.trl", "a" );
    }

    if( WdeTrailFile == NULL ) {
        WdeTrailOpen = false;
        return( false );
    } else {
        if( setvbuf( WdeTrailFile, NULL, _IONBF, 0 ) ) {
            WdeTrailOpen = false;
            return( false );
        }
    }

    fprintf( WdeTrailFile, "%s\n", msg );

    fflush( WdeTrailFile );

    fclose( WdeTrailFile );

#if defined( WDE_DEBUG_ON_STATUS )
    return( WdeWriteStatus ( msg ) );
#else
    return( true );
#endif
}

bool WdeWriteTrailn( const char * msg, int len )
{
    if( !WdeTrailOpen ) {
        WdeTrailFile = fopen( "d:\\src\\wde\\wde.trl", "wa" );
        WdeTrailOpen = true;
    } else {
        WdeTrailFile = fopen( "d:\\src\\wde\\wde.trl", "a" );
    }

    if( WdeTrailFile == NULL ) {
        WdeTrailOpen = false;
        return( false );
    } else {
        if( setvbuf( WdeTrailFile, NULL, _IONBF, 0 ) ) {
            WdeTrailOpen = false;
            return( false );
        }
    }

    fprintf( WdeTrailFile, "\n%*s\n", len, msg );

    fflush( WdeTrailFile );

    fclose( WdeTrailFile );

    return( true );
}

#ifdef WDE_DEBUG_ON_STATUS
bool WdeWriteStatus( const char *msg )
{
    WdeSetStatusText( NULL, msg, true );
    return( true );
}
#endif

#endif

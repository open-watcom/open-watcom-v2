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
#include <stdio.h>
#include "wdestat.h"
#include "wdemain.h"
#include "wdedebug.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
#ifdef WDE_DEBUG

#ifdef WDE_DEBUG_DISPATCH
typedef enum {
    P1 = 1,
    P2,
    Point,
    Boolean,
    Rect,
    Req,
    Listp,
    Hdc,
    Note_id,
    Action,
    Objptr,
    Char,
    Objptrp,
    Int,
    Hwnd,
    Str,
    Infop,
    Info,
    Obj_id,
    Hfont,
    Uint32,
    Uint16,
    Fileid
} WdeDebugParamTypes;

char *WdeDebugParamNames[] = {
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
    ACTION              act;
    char                *act_name;
    WdeDebugParamTypes  p1;
    WdeDebugParamTypes  p2;
} WdeDebugActionItem;

static WdeDebugActionItem WdeDebugActions[] = {
    { REGISTER,           "REGISTER          ", P1,      P2      },
    { MOVE,               "MOVE              ", Point,   Boolean },
    { RESIZE,             "RESIZE            ", Rect,    Boolean },
    { LOCATE,             "LOCATE            ", Rect,    P2      },
    { FIND_SUBOBJECTS,    "FIND_SUBOBJECTS   ", Req,     Listp   },
    { DESTROY,            "DESTROY           ", Boolean, P2      },
    { DRAW,               "DRAW              ", Rect,    Hdc     },
    { DEFINE,             "DEFINE            ", Point,   P2      },
    { NOTIFY,             "NOTIFY            ", Note_id, P2      },
    { FORWARD,            "FORWARD           ", Action,  P2      },
    { VALIDATE_ACTION,    "VALIDATE_ACTION   ", Action,  P2      },
    { ADD_SUBOBJECT,      "ADD_SUBOBJECT     ", Objptr,  P2      },
    { REMOVE_SUBOBJECT,   "REMOVE_SUBOBJECT  ", Objptr,  P2      },
    { RESIZE_INFO,        "RESIZE_INFO       ", Int,     P2      },
    { CUT,                "CUT               ", Objptrp, P2      },
    { COPY,               "COPY              ", Objptrp, Objptr  },
    { PASTE,              "PASTE             ", Objptr,  Point   },
    { GET_PARENT,         "GET_PARENT        ", Objptr,  P2      },
    { GET_PRIORITY,       "GET_PRIORITY      ", Int,     P2      },
    { GET_RESIZE_INC,     "GET_RESIZE_INC    ", Point,   P2      },
    { GET_SCROLL_RECT,    "GET_SCROLL_RECT   ", Rect,    P2      },
    { IS_MARK_VALID,      "IS_MARK_VALID     ", Boolean, P2      },
    { FIND_OBJECTS_PT,    "FIND_OBJECTS_PT   ", Point,   Listp   },
    { GET_WINDOW_HANDLE,  "GET_WINDOW_HANDLE ", Hwnd,    P2      },
    { GET_WINDOW_CLASS,   "GET_WINDOW_CLASS  ", Str,     P2      },
    { GET_SUBOBJ_LIST,    "GET_SUBOBJ_LIST   ", Listp,   P2      },
    { CREATE_WINDOW,      "CREATE_WINDOW     ", P1,      P2      },
    { DESTROY_WINDOW,     "DESTROY_WINDOW    ", Boolean, Boolean },
    { SHOW_WIN,           "SHOW_WIN          ", Boolean, P2      },
    { GET_OBJECT_INFO,    "GET_OBJECT_INFO   ", Infop,   P2      },
    { SET_OBJECT_INFO,    "SET_OBJECT_INFO   ", Info,    P2      },
    { IDENTIFY,           "IDENTIFY          ", Obj_id,  P2      },
    { GET_RESIZER,        "GET_RESIZER       ", P1,      Objptrp },
    { GET_FONT,           "GET_FONT          ", Hfont,   P2      },
    { SET_FONT,           "SET_FONT          ", Hfont,   P2      },
    { BECOME_FIRST_CHILD, "BECOME_FIRST_CHILD", P1,      P2      },
    { PUT_ME_FIRST,       "PUT_ME_FIRST      ", Objptr,  P2      },
    { GET_FIRST_CHILD,    "GET_FIRST_CHILD   ", Objptr,  P2      },
    { GET_NC_SIZE,        "GET_NC_SIZE       ", Rect,    P2      },
    { SAVE_OBJECT,        "SAVE_OBJECT       ", Uint32,  P2      },
    { ON_TOP,             "ON_TOP            ", P1,      P2      },
    { TEST,               "TEST              ", P1,      P2      },
    { IS_OBJECT_CLEAR,    "IS_OBJECT_CLEAR   ", Boolean, P2      },
    { SET_CLEAR_INT,      "SET_CLEAR_INT     ", Boolean, P2      },
    { RESTORE_OBJECT,     "RESTORE_OBJECT    ", P1,      P2      },
    { RESOLVE_SYMBOL,     "RESOLVE_SYMBOL    ", Boolean, P2      },
    { MODIFY_INFO,        "MODIFY_INFO       ", P1,      P2      },
    { GET_NEXT_CHILD,     "GET_NEXT_CHILD    ", Objptrp, Boolean },
    { SET_ORDER_MODE,     "SET_ORDER_MODE    ", Int,     Int     },
    { SIZE_TO_TEXT,       "SIZE_TO_TEXT      ", P1,      P2      },
    { -1,                 NULL,                 NULL,    NULL    }
};

#endif

#endif

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
#ifdef WDE_DEBUG
#ifdef WDE_DEBUG_DISPATCH
static WdeDebugActionItem *WdeDebugGetActionItem( ACTION );
static void WdeDebugInterpretParam( WdeDebugParamTypes, void *, char * );
#endif
#endif

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
#if defined( WDE_DEBUG )
static Bool WdeTrailOpen = FALSE;
static FILE *WdeTrailFile = NULL;
#if defined( WDE_DEBUG_DISPATCH ) || defined( WDE_DEBUG_CREATE )
static char WdeDebugString[512];
#endif
#endif

#if defined( WDE_DEBUG ) && defined( WDE_DEBUG_DISPATCH )
static WdeDebugActionItem WdeDebugDefaultAction = { -1 , "" , P1, P2 };

static WdeDebugActionItem *WdeDebugGetActionItem( ACTION act )
{
    int i;

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
    case P1:
    case P2:
    case Objptr:
    case Info:
        sprintf( temp, "%7s: %p, ", WdeDebugParamNames[type], (void *)param );
        break;

    case Listp:
    case Infop:
    case Objptrp:
    case Str:
        if( *(int **)param == NULL ) {
            sprintf( temp, "%7s: NULL, ", WdeDebugParamNames[type] );
        } else {
            sprintf( temp, "%7s: %p, ", WdeDebugParamNames[type], (void *)*(int **)param );
        }
        break;

    case Boolean:
        if ( *(Bool *)param ) {
            sprintf( temp, "%7s: TRUE, ", WdeDebugParamNames[type] );
        } else {
            sprintf( temp, "%7s: FALSE, ", WdeDebugParamNames[type] );
        }
        break;

    case Int:
    case Uint32:
    case Uint16:
    case Fileid:
    case Obj_id:
    case Note_id:
    case Hwnd:
    case Hdc:
    case Hfont:
        sprintf( temp, "%7s: %d, ", WdeDebugParamNames[type], *(int *)param );
        break;

    case Char:
        sprintf( temp, "%7s: %c, ", WdeDebugParamNames[type], *(char *)param );
        break;

    case Rect:
        sprintf( temp, "%7s: (%d,%d),(%d,%d), ", WdeDebugParamNames[type],
                 ((RECT *)param)->left, ((RECT *)param)->top,
                 ((RECT *)param)->right, ((RECT *)param)->bottom );
        break;

    case Point:
        sprintf( temp, "%7s: (%d,%d), ", WdeDebugParamNames[type],
                 ((POINT *)param)->x, ((POINT *)param)->y );
        break;

    case Action:
        act_item = WdeDebugGetActionItem( *(ACTION *)param );
        if( act_item->act == -1 ) {
            sprintf( temp, "%7s: %21.21d, ", WdeDebugParamNames[type], *(ACTION *)param );
        } else {
            sprintf( temp, "%7s: %s, ", WdeDebugParamNames[type], act_item->act_name );
        }
        break;

    case Req:
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
void WdeDebugDispatch( char *name, ACTION act, void * obj, void *p1, void *p2 )
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
void WdeDebugCreate ( char *name, void * parent, RECT *obj_rect, void * handle )
{
    char temp[512];

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
Bool WdeWriteTrail( const char * msg )
{
    if( !WdeTrailOpen ) {
        WdeTrailFile = fopen( "d:\\src\\wde\\wde.trl", "wa" );
        WdeTrailOpen = TRUE;
    } else {
        WdeTrailFile = fopen( "d:\\src\\wde\\wde.trl", "a" );
    }

    if( WdeTrailFile == NULL ) {
        WdeTrailOpen = FALSE;
        return( FALSE );
    } else {
        if( setvbuf( WdeTrailFile, NULL, _IONBF, 0 ) ) {
            WdeTrailOpen = FALSE;
            return( FALSE );
        }
    }

    fprintf( WdeTrailFile, "%s\n", msg );

    fflush( WdeTrailFile );

    fclose( WdeTrailFile );

#if defined( WDE_DEBUG_ON_STATUS )
    return( WdeWriteStatus ( msg ) );
#else
    return( TRUE );
#endif
}

Bool WdeWriteTrailn( const char * msg, int len )
{
    if( !WdeTrailOpen ) {
        WdeTrailFile = fopen( "d:\\src\\wde\\wde.trl", "wa" );
        WdeTrailOpen = TRUE;
    } else {
        WdeTrailFile = fopen( "d:\\src\\wde\\wde.trl", "a" );
    }

    if( WdeTrailFile == NULL ) {
        WdeTrailOpen = FALSE;
        return( FALSE );
    } else {
        if( setvbuf( WdeTrailFile, NULL, _IONBF, 0 ) ) {
            WdeTrailOpen = FALSE;
            return( FALSE );
        }
    }

    fprintf( WdeTrailFile, "\n%*s\n", len, msg );

    fflush( WdeTrailFile );

    fclose( WdeTrailFile );

    return( TRUE );
}

Bool WdeWriteStatus( const char * msg )
{
    WdeSetStatusText( NULL, msg, TRUE );
    return( TRUE );
}
#endif

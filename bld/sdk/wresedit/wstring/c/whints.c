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
#include <stdio.h>
#include <win1632.h>
#include "wglbl.h"
#include "wmem.h"
#include "wstat.h"
#include "sys_rc.h"
#include "wlist.h"
#include "whints.h"
#include "wmsgfile.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define MAX_NESTED_POPUPS 2

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    int         id;
    DWORD       hint;
} WHintItem;

typedef struct {
    int         loc[MAX_NESTED_POPUPS];
    HMENU       popup;
    DWORD       hint;
} WPopupHintItem;

WPopupListItem;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WHintItem      *WGetHintItem          ( int id );
static void            WHandlePopupHint      ( wstatbar *, HMENU, HMENU );
static DWORD           WGetPopupHint         ( WPopupHintItem *, int, HMENU );
static Bool            WInitHintItems        ( int, HMENU, WPopupHintItem * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HMENU  WLastMenu  = NULL;

static WHintItem WHints[] =
{
    { IDM_STR_CLEAR     , W_STR_CLEAR           }
,   { IDM_STR_UPDATE    , W_STR_UPDATE          }
,   { IDM_STR_SAVE      , W_STR_SAVE            }
,   { IDM_STR_SAVEAS    , W_STR_SAVEAS          }
,   { IDM_STR_SAVEINTO  , W_STR_SAVEINTO        }
,   { IDM_STR_EXIT      , W_STR_EXIT            }
,   { IDM_STR_CUT       , W_STR_CUT             }
,   { IDM_STR_COPY      , W_STR_COPY            }
,   { IDM_STR_PASTE     , W_STR_PASTE           }
,   { IDM_STR_DELETE    , W_STR_DELETE          }
,   { IDM_STR_SHOWRIBBON, W_STR_SHOWRIBBON      }
,   { IDM_STR_LOAD_SYMBOLS      , W_STR_LOAD_SYMBOLS            }
,   { IDM_STR_MEM_FLAGS , W_STR_MEM_FLAGS       }
,   { IDM_STR_NEWITEM   , W_STR_NEWITEM         }
,   { IDM_STR_ABOUT     , W_STR_ABOUT           }
,   { IDM_HELP          , W_STR_HELP            }
,   { -1                , NULL                  }
};

static WPopupHintItem WPopupHints[] =
{
    { { 0, -1 },  NULL, W_STR_FILEMENU  }
,   { { 1, -1 },  NULL, W_STR_EDITMENU  }
,   { { 2, -1 },  NULL, W_STR_RESMENU   }
,   { { 3, -1 },  NULL, W_STR_STRMENU   }
,   { { 4, -1 },  NULL, W_STR_HELPMENU  }
};

#define NUM_POPUPS (sizeof(WPopupHints)/sizeof(WPopupHintItem))

void WHandleMenuSelect ( wstatbar *wsb, HMENU menu, WPARAM wParam,
                         LPARAM lParam )
{
    HMENU popup;
    WORD  flags;

    if ( !wsb || !menu ) {
        return;
    }

    flags = GET_WM_MENUSELECT_FLAGS(wParam,lParam);

    if ( ( flags == (WORD)-1 ) &&
         ( GET_WM_MENUSELECT_HMENU(wParam,lParam) == (HMENU)NULL ) ) {
        WSetStatusText ( wsb, NULL, "" );
    } else if ( flags & (MF_SYSMENU | MF_SEPARATOR) ) {
        WSetStatusText ( wsb, NULL, "" );
    } else if ( flags & MF_POPUP ) {
        popup = (HMENU) GET_WM_MENUSELECT_ITEM(wParam,lParam);
        #ifdef __NT__
            popup = GetSubMenu( (HMENU)lParam, (int)popup );
        #endif
        WHandlePopupHint ( wsb, menu, popup );
    } else {
        WDisplayHint ( wsb, (int) GET_WM_MENUSELECT_ITEM(wParam,lParam) );
    }
}

void WDisplayHint( wstatbar *wsb, int id )
{
    WHintItem *hint;

    hint = WGetHintItem( id );
    if( hint ) {
        WSetStatusByID( wsb, -1, hint->hint );
    }
}

WHintItem *WGetHintItem ( int id )
{
    int i;

    for ( i = 0; WHints[i].id != -1; i++ ) {
        if ( WHints[i].id == id ) {
            return ( &(WHints[i]) );
        }
    }

    return ( NULL );
}

DWORD WGetPopupHint( WPopupHintItem *items, int num, HMENU popup )
{
    int i;

    for( i = 0; i < num; i++ ) {
        if( items[i].popup == popup ) {
            return( items[i].hint );
        }
    }

    return( NULL );
}

void WHandlePopupHint( wstatbar *wsb, HMENU menu, HMENU popup )
{
    DWORD       hint;

    if( menu != WLastMenu ) {
        WInitHintItems( NUM_POPUPS, menu, WPopupHints );
        WLastMenu = menu;
    }

    hint = WGetPopupHint( WPopupHints, NUM_POPUPS, popup );
    if( hint ) {
        WSetStatusByID( wsb, -1, hint );
    } else {
        WSetStatusText( wsb, NULL, "" );
    }
}

Bool WInitHintItems ( int num, HMENU menu, WPopupHintItem *hint_items )
{
    int   i;
    int   j;
    HMENU popup;

    for ( i = 0; i < num; i++ ) {
        popup = menu;
        for ( j = 0;
              (j < MAX_NESTED_POPUPS) && (hint_items[i].loc[j] != -1);
              j++ ) {
            popup = GetSubMenu ( popup, hint_items[i].loc[j] );
        }
        hint_items[i].popup = popup;
    }

    return ( TRUE );
}


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
#include <stdio.h>
#include "win1632.h"
#include "wglbl.h"
#include "wmem.h"
#include "wstat.h"
#include "sys_rc.h"
#include "wlist.h"
#include "whints.h"
#include "rcstr.gh"

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

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WHintItem    *WGetHintItem( int id );
static void         WHandlePopupHint( WStatBar *, HMENU, HMENU );
static DWORD        WGetPopupHint( WPopupHintItem *, int, HMENU );
static Bool         WInitHintItems( int, HMENU, WPopupHintItem * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HMENU  WLastMenu  = NULL;

static WHintItem WHints[] = {
    { IDM_ACC_CLEAR,        W_ACC_CLEAR         },
    { IDM_ACC_UPDATE,       W_ACC_UPDATE        },
    { IDM_ACC_SAVE,         W_ACC_SAVE          },
    { IDM_ACC_SAVEAS,       W_ACC_SAVEAS        },
    { IDM_ACC_SAVEINTO,     W_ACC_SAVEINTO      },
    { IDM_ACC_EXIT,         W_ACC_EXIT          },
    { IDM_ACC_CUT,          W_ACC_CUT           },
    { IDM_ACC_COPY,         W_ACC_COPY          },
    { IDM_ACC_PASTE,        W_ACC_PASTE         },
    { IDM_ACC_DELETE,       W_ACC_DELETE        },
    { IDM_ACC_SHOWRIBBON,   W_ACC_SHOWRIBBON    },
    { IDM_ACC_SYMBOLS,      W_ACC_SYMBOLS       },
    { IDM_ACC_LOAD_SYMBOLS, W_ACC_LOAD_SYMBOLS  },
    { IDM_ACC_RENAME,       W_ACC_RENAME        },
    { IDM_ACC_MEM_FLAGS,    W_ACC_MEM_FLAGS     },
    { IDM_ACC_NEWITEM,      W_ACC_NEWITEM       },
    { IDM_ACC_KEYVALUE,     W_ACC_KEYVALUE      },
    { IDM_ACC_ABOUT,        W_ACC_ABOUT         },
    { IDM_HELP,             W_ACC_HELP          },
    { IDM_HELP_SEARCH,      W_ACC_HELP_SEARCH   },
    { IDM_HELP_ON_HELP,     W_ACC_HELP_ON_HELP  },
    { -1,                   0                   }
};

static WPopupHintItem WPopupHints[] = {
    { { 0, -1 },  NULL, W_ACC_FILEMENU  },
    { { 1, -1 },  NULL, W_ACC_EDITMENU  },
    { { 2, -1 },  NULL, W_ACC_RESMENU   },
    { { 3, -1 },  NULL, W_ACC_ACCMENU   },
    { { 4, -1 },  NULL, W_ACC_HELPMENU  }
};

#define NUM_POPUPS (sizeof( WPopupHints ) / sizeof( WPopupHintItem ))

void WHandleMenuSelect( WStatBar *wsb, HMENU menu, WPARAM wParam,
                        LPARAM lParam )
{
    HMENU   popup;
    WORD    flags;

    if( wsb == NULL || menu == NULL ) {
        return;
    }

    flags = GET_WM_MENUSELECT_FLAGS( wParam, lParam );

    if( flags == (WORD)-1 && GET_WM_MENUSELECT_HMENU( wParam, lParam ) == (HMENU)NULL ) {
        WSetStatusText( wsb, NULL, "" );
    } else if( flags & (MF_SYSMENU | MF_SEPARATOR) ) {
        WSetStatusText( wsb, NULL, "" );
    } else if( flags & MF_POPUP ) {
        popup = (HMENU)GET_WM_MENUSELECT_ITEM( wParam, lParam );
#ifdef __NT__
        popup = GetSubMenu( (HMENU)lParam, (int)popup );
#endif
        WHandlePopupHint( wsb, menu, popup );
    } else {
        WDisplayHint( wsb, (int)GET_WM_MENUSELECT_ITEM( wParam, lParam ) );
    }
}

void WDisplayHint( WStatBar *wsb, int id )
{
    WHintItem *hint;

    hint = WGetHintItem( id );
    if( hint != NULL ) {
        WSetStatusByID( wsb, -1, hint->hint );
    }
}

WHintItem *WGetHintItem( int id )
{
    int i;

    for( i = 0; WHints[i].id != -1; i++ ) {
        if( WHints[i].id == id ) {
            return( &WHints[i] );
        }
    }

    return( NULL );
}

DWORD WGetPopupHint( WPopupHintItem *items, int num, HMENU popup )
{
    int i;

    for( i = 0; i < num; i++ ) {
        if( items[i].popup == popup ) {
            return( items[i].hint );
        }
    }

    return( 0L );
}

void WHandlePopupHint( WStatBar *wsb, HMENU menu, HMENU popup )
{
    DWORD       hint;

    if( menu != WLastMenu ) {
        WInitHintItems( NUM_POPUPS, menu, WPopupHints );
        WLastMenu = menu;
    }

    hint = WGetPopupHint( WPopupHints, NUM_POPUPS, popup );
    if( hint != 0 ) {
        WSetStatusByID( wsb, -1, hint );
    } else {
        WSetStatusText( wsb, NULL, "" );
    }
}

Bool WInitHintItems( int num, HMENU menu, WPopupHintItem *hint_items )
{
    int     i;
    int     j;
    HMENU   popup;

    for( i = 0; i < num; i++ ) {
        popup = menu;
        for( j = 0; j < MAX_NESTED_POPUPS && hint_items[i].loc[j] != -1; j++ ) {
            popup = GetSubMenu( popup, hint_items[i].loc[j] );
        }
        hint_items[i].popup = popup;
    }

    return( TRUE );
}

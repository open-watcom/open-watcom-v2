/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "wglbl.h"
#include "w_menu.h"
#include "wmain.h"
#include "wstat.h"
#include "sysall.rh"
#include "wlist.h"
#include "whints.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define MAX_NESTED_POPUPS 2

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    ctl_id      id;
    msg_id      hint;
} WHintItem;

typedef struct {
    int         loc[MAX_NESTED_POPUPS];
    HMENU       hpopup;
    msg_id      hint;
} WPopupHintItem;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void         WHandlePopupHint( WStatBar *, HMENU, HMENU );
static msg_id       WGetPopupHint( WPopupHintItem *, int, HMENU );
static bool         WInitHintItems( int, HMENU, WPopupHintItem * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HMENU WLastMenu = NULL;

static WHintItem WHints[] = {
    { IDM_MENU_CLEAR,           W_MENU_CLEAR          },
    { IDM_MENU_UPDATE,          W_MENU_UPDATE         },
    { IDM_MENU_SAVE,            W_MENU_SAVE           },
    { IDM_MENU_SAVEAS,          W_MENU_SAVEAS         },
    { IDM_MENU_SAVEINTO,        W_MENU_SAVEINTO       },
    { IDM_MENU_EXIT,            W_MENU_EXIT           },
    { IDM_MENU_CUT,             W_MENU_CUT            },
    { IDM_MENU_COPY,            W_MENU_COPY           },
    { IDM_MENU_PASTE,           W_MENU_PASTE          },
    { IDM_MENU_DELETE,          W_MENU_DELETE         },
    { IDM_MENU_SHOWRIBBON,      W_MENU_SHOWRIBBON     },
    { IDM_MENU_SYMBOLS,         W_MENU_SYMBOLS        },
    { IDM_MENU_LOAD_SYMBOLS,    W_MENU_LOAD_SYMBOLS   },
    { IDM_MENU_RENAME,          W_MENU_RENAME         },
    { IDM_MENU_MEM_FLAGS,       W_MENU_MEM_FLAGS      },
    { IDM_MENU_INSERTAFTER,     W_MENU_INSERTAFTER    },
    { IDM_MENU_INSERTBEFORE,    W_MENU_INSERTBEFORE   },
    { IDM_MENU_INSERTTOGGLE,    W_MENU_INSERTTOGGLE   },
    { IDM_MENU_INSERTSUBITEMS,  W_MENU_INSERTSUBITEMS },
    { IDM_MENU_NEWITEM,         W_MENU_NEWITEM        },
    { IDM_MENU_NEWPOPUP,        W_MENU_NEWPOPUP       },
    { IDM_MENU_NEWSEPARATOR,    W_MENU_NEWSEPARATOR   },
    { IDM_MENU_ABOUT,           W_MENU_ABOUT          },
    { IDM_HELP,                 W_MENU_HELP           },
    { IDM_HELP_SEARCH,          W_MENU_HELP_SEARCH    },
    { IDM_HELP_ON_HELP,         W_MENU_HELP_ON_HELP   },
    { 0,                        0                     }
};

static WPopupHintItem WPopupHints[] = {
    { { 0, -1 },  NULL, W_MENU_FILEMENU },
    { { 1, -1 },  NULL, W_MENU_EDITMENU },
    { { 2, -1 },  NULL, W_MENU_RESMENU  },
    { { 3, -1 },  NULL, W_MENU_MENUMENU },
    { { 4, -1 },  NULL, W_MENU_HELPMENU }
};

#define NUM_POPUPS (sizeof( WPopupHints ) / sizeof( WPopupHintItem ))

void WHandleMenuSelect( WStatBar *wsb, HMENU hmenu, WPARAM wParam, LPARAM lParam )
{
    HMENU   hpopup;
    WORD    flags;

    if( wsb == NULL || hmenu == NULL ) {
        return;
    }

    if( MENU_CLOSED( wParam, lParam ) ) {
        WSetStatusText( wsb, NULL, "" );
    } else {
        flags = GET_WM_MENUSELECT_FLAGS( wParam, lParam );
        if( flags & (MF_SYSMENU | MF_SEPARATOR) ) {
            WSetStatusText( wsb, NULL, "" );
        } else if( flags & MF_POPUP ) {
#ifdef __NT__
            hpopup = GetSubMenu( (HMENU)lParam, GET_WM_MENUSELECT_ITEM( wParam, lParam ) );
#else
            hpopup = (HMENU)GET_WM_MENUSELECT_ITEM( wParam, lParam );
#endif
            WHandlePopupHint( wsb, hmenu, hpopup );
        } else {
            WDisplayHint( wsb, GET_WM_MENUSELECT_ITEM( wParam, lParam ) );
        }
    }
}

static WHintItem *WGetHintItem( ctl_id id )
{
    int i;

    for( i = 0; WHints[i].id > 0; i++ ) {
        if( WHints[i].id == id ) {
            return( &WHints[i] );
        }
    }

    return( NULL );
}

void WDisplayHint( WStatBar *wsb, ctl_id id )
{
    WHintItem           *hint;
    WMenuEditInfo       *einfo;

    if( id == IDM_MENU_INSERTTOGGLE ) {
        einfo = WGetCurrentEditInfo();
        if( einfo != NULL ) {
            if( einfo->insert_before ) {
                id = IDM_MENU_INSERTBEFORE;
            } else {
                id = IDM_MENU_INSERTAFTER;
            }
        }
    }

    hint = WGetHintItem( id );
    if( hint != NULL ) {
        WSetStatusByID( wsb, 0, hint->hint );
    }
}

msg_id WGetPopupHint( WPopupHintItem *items, int num, HMENU hpopup )
{
    int i;

    for( i = 0; i < num; i++ ) {
        if( items[i].hpopup == hpopup ) {
            return( items[i].hint );
        }
    }

    return( 0 );
}

void WHandlePopupHint( WStatBar *wsb, HMENU hmenu, HMENU hpopup )
{
    msg_id      hint;

    if( hmenu != WLastMenu ) {
        WInitHintItems( NUM_POPUPS, hmenu, WPopupHints );
        WLastMenu = hmenu;
    }

    hint = WGetPopupHint( WPopupHints, NUM_POPUPS, hpopup );
    if( hint > 0 ) {
        WSetStatusByID( wsb, 0, hint );
    } else {
        WSetStatusText( wsb, NULL, "" );
    }
}

bool WInitHintItems( int num, HMENU hmenu, WPopupHintItem *hint_items )
{
    int     i;
    int     j;
    HMENU   hpopup;

    for( i = 0; i < num; i++ ) {
        hpopup = hmenu;
        for( j = 0; j < MAX_NESTED_POPUPS && hint_items[i].loc[j] != -1; j++ ) {
            hpopup = GetSubMenu( hpopup, hint_items[i].loc[j] );
        }
        hint_items[i].hpopup = hpopup;
    }

    return( TRUE );
}

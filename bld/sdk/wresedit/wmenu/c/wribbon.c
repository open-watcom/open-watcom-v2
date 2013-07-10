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

#include "watcom.h"
#include "wglbl.h"
#include "wmain.h"
#include "wtoolbar.h"
#include "wstat.h"
#include "whints.h"
#include "wmsg.h"
#include "wprev.h"
#include "sys_rc.h"
#include "wribbon.h"
#include "rcstr.gh"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define BLANK_PAD       8
#define BUTTONX         22
#define BUTTONY         18
#define BUTTON_PAD      4
#define TOOL_BORDERX    4
#define TOOL_BORDERY    2

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WRibbonHook( HWND, UINT, WPARAM, LPARAM );
extern void WRibbonHelpHook( HWND hwnd, WPARAM wParam, BOOL pressed );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    char    *up;
    char    *down;
    UINT    menu_id;
    int     tip_id;
} WRibbonName;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
WRibbonName WRibbonNames[] = {
    { "Clear",          NULL, IDM_MENU_CLEAR,           W_TIP_CLEAR        },
    { "Save",           NULL, IDM_MENU_UPDATE,          W_TIP_UPDATE       },
    { NULL,             NULL, BLANK_PAD,                -1                 },
    { "Cut",            NULL, IDM_MENU_CUT,             W_TIP_CUT          },
    { "Copy",           NULL, IDM_MENU_COPY,            W_TIP_COPY         },
    { "Paste",          NULL, IDM_MENU_PASTE,           W_TIP_PASTE        },
    { NULL,             NULL, BLANK_PAD * 2,            -1                 },
    { "DeleteItem",     NULL, IDM_MENU_DELETE,          W_TIP_DELETE       },
    { NULL,             NULL, BLANK_PAD * 2,            -1                 },
    { "InsertAfter",    NULL, IDM_MENU_INSERTTOGGLE,    W_TIP_INSERTTOGGLE },
    { NULL,             NULL, BLANK_PAD,                -1                 },
//  { "InsertNoSub",    NULL, IDM_MENU_INSERTSUBITEMS,  -1                 },
//  { NULL,             NULL, BLANK_PAD * 2,            -1                 },
//  { "InsertPopup",    NULL, IDM_MENU_NEWPOPUP,        -1                 },
    { "InsertItem",     NULL, IDM_MENU_NEWITEM,         W_TIP_NEWITEM      }
//  { "InsertSep",      NULL, IDM_MENU_NEWSEPARATOR,    -1                 }
};
#define NUM_TOOLS (sizeof( WRibbonNames ) / sizeof( WRibbonName ))

WRibbonName WSORibbonNames[] = {
    { "New",            NULL, IDM_MENU_CLEAR,           W_TIP_NEW          },
    { "Open",           NULL, IDM_MENU_OPEN,            W_TIP_OPEN         },
    { "Save",           NULL, IDM_MENU_SAVE,            W_TIP_SAVE         },
    { NULL,             NULL, BLANK_PAD,                -1                 },
    { "Cut",            NULL, IDM_MENU_CUT,             W_TIP_CUT          },
    { "Copy",           NULL, IDM_MENU_COPY,            W_TIP_COPY         },
    { "Paste",          NULL, IDM_MENU_PASTE,           W_TIP_PASTE        },
    { NULL,             NULL, BLANK_PAD * 2,            -1                 },
    { "DeleteItem",     NULL, IDM_MENU_DELETE,          W_TIP_DELETE       },
    { NULL,             NULL, BLANK_PAD * 2,            -1                 },
    { "InsertAfter",    NULL, IDM_MENU_INSERTTOGGLE,    W_TIP_INSERTTOGGLE },
    { NULL,             NULL, BLANK_PAD,                -1                 },
//  { "InsertNoSub",    NULL, IDM_MENU_INSERTSUBITEMS,  -1                 },
//  { NULL,             NULL, BLANK_PAD * 2,            -1                 },
//  { "InsertPopup",    NULL, IDM_MENU_NEWPOPUP,        -1                 },
    { "InsertItem",     NULL, IDM_MENU_NEWITEM,         W_TIP_NEWITEM      }
//  { "InsertSep",      NULL, IDM_MENU_NEWSEPARATOR,    -1                 }
};
#define NUM_SOTOOLS (sizeof( WSORibbonNames ) / sizeof( WRibbonName ))

static WToolBarInfo *WRibbonInfo = NULL;
static WToolBarInfo *WSORibbonInfo = NULL;

static int          WRibbonHeight = 0;

int WGetRibbonHeight( void )
{
    return( WRibbonHeight );
}

Bool WInitRibbons( HINSTANCE inst )
{
    int i;

    WRibbonInfo = WAllocToolBarInfo( NUM_TOOLS );
    WSORibbonInfo = WAllocToolBarInfo( NUM_SOTOOLS );

    if( WRibbonInfo == NULL || WSORibbonInfo == NULL ) {
        return( FALSE );
    }

    for( i = 0; i < NUM_TOOLS; i++ ) {
        if( WRibbonNames[i].up != NULL ) {
            WRibbonInfo->items[i].u.bmp = LoadBitmap( inst, WRibbonNames[i].up );
            WRibbonInfo->items[i].id = WRibbonNames[i].menu_id;
            WRibbonInfo->items[i].flags = ITEM_DOWNBMP;
            if( WRibbonNames[i].down != NULL ) {
                WRibbonInfo->items[i].depressed = LoadBitmap( inst, WRibbonNames[i].down );
            } else {
                WRibbonInfo->items[i].depressed = WRibbonInfo->items[i].u.bmp;
            }
            if( WRibbonNames[i].tip_id >= 0 ) {
                LoadString( inst, WRibbonNames[i].tip_id, WRibbonInfo->items[i].tip, MAX_TIP );
            } else {
                WRibbonInfo->items[i].tip[0] = '\0';
            }
        } else {
            WRibbonInfo->items[i].flags = ITEM_BLANK;
            WRibbonInfo->items[i].u.blank_space = WRibbonNames[i].menu_id;
        }
    }

    for( i = 0; i < NUM_SOTOOLS; i++ ) {
        if( WSORibbonNames[i].up != NULL ) {
            WSORibbonInfo->items[i].u.bmp = LoadBitmap( inst, WSORibbonNames[i].up );
            WSORibbonInfo->items[i].id = WSORibbonNames[i].menu_id;
            WSORibbonInfo->items[i].flags = ITEM_DOWNBMP;
            if( WSORibbonNames[i].down != NULL ) {
                WSORibbonInfo->items[i].depressed =
                    LoadBitmap( inst, WSORibbonNames[i].down );
            } else {
                WSORibbonInfo->items[i].depressed = WSORibbonInfo->items[i].u.bmp;
            }
            if( WSORibbonNames[i].tip_id >= 0 ) {
                LoadString( inst, WSORibbonNames[i].tip_id, WSORibbonInfo->items[i].tip, MAX_TIP );
            } else {
                WSORibbonInfo->items[i].tip[0] = '\0';
            }
        } else {
            WSORibbonInfo->items[i].flags = ITEM_BLANK;
            WSORibbonInfo->items[i].u.blank_space = WSORibbonNames[i].menu_id;
        }
    }

    WRibbonInfo->dinfo.button_size.x = BUTTONX + BUTTON_PAD;
    WRibbonInfo->dinfo.button_size.y = BUTTONY + BUTTON_PAD;
    WRibbonInfo->dinfo.border_size.x = TOOL_BORDERX;
    WRibbonInfo->dinfo.border_size.y = TOOL_BORDERY;
    WRibbonInfo->dinfo.style = TOOLBAR_FIXED_STYLE;
    WRibbonInfo->dinfo.hook = WRibbonHook;
    WRibbonInfo->dinfo.helphook = WRibbonHelpHook;
    WRibbonInfo->dinfo.foreground = NULL;
    WRibbonInfo->dinfo.background = NULL;
    WRibbonInfo->dinfo.is_fixed = TRUE;
    WRibbonInfo->dinfo.use_tips = TRUE;

    WSORibbonInfo->dinfo.button_size.x = BUTTONX + BUTTON_PAD;
    WSORibbonInfo->dinfo.button_size.y = BUTTONY + BUTTON_PAD;
    WSORibbonInfo->dinfo.border_size.x = TOOL_BORDERX;
    WSORibbonInfo->dinfo.border_size.y = TOOL_BORDERY;
    WSORibbonInfo->dinfo.style = TOOLBAR_FIXED_STYLE;
    WSORibbonInfo->dinfo.hook = WRibbonHook;
    WSORibbonInfo->dinfo.helphook = WRibbonHelpHook;
    WSORibbonInfo->dinfo.foreground = NULL;
    WSORibbonInfo->dinfo.background = NULL;
    WSORibbonInfo->dinfo.is_fixed = TRUE;
    WSORibbonInfo->dinfo.use_tips = TRUE;

    WRibbonHeight = 2 * WRibbonInfo->dinfo.border_size.y +
                    WRibbonInfo->dinfo.button_size.y +
                    2 * GetSystemMetrics( SM_CYBORDER );

    WRibbonInfo->dinfo.area.bottom = WRibbonHeight;
    WSORibbonInfo->dinfo.area.bottom = WRibbonHeight;

    return( TRUE );
}

void WShutdownRibbons( void )
{
    int i;

    if( WRibbonInfo != NULL ) {
        for( i = 0; i < NUM_TOOLS; i++ ) {
            if( WRibbonInfo->items[i].flags != ITEM_BLANK ) {
                if( WRibbonInfo->items[i].u.bmp == WRibbonInfo->items[i].depressed ) {
                    WRibbonInfo->items[i].depressed = (HBITMAP)NULL;
                }
                if( WRibbonInfo->items[i].u.bmp != NULL ) {
                    DeleteObject( WRibbonInfo->items[i].u.bmp );
                }
                if( WRibbonInfo->items[i].depressed != NULL ) {
                    DeleteObject( WRibbonInfo->items[i].depressed );
                }
            }
        }
        WFreeToolBarInfo( WRibbonInfo );
        WRibbonInfo = NULL;
    }

    if( WSORibbonInfo != NULL ) {
        for( i = 0; i < NUM_SOTOOLS; i++ ) {
            if( WSORibbonInfo->items[i].flags != ITEM_BLANK ) {
                if( WSORibbonInfo->items[i].u.bmp == WSORibbonInfo->items[i].depressed ) {
                    WSORibbonInfo->items[i].depressed = (HBITMAP)NULL;
                }
                if( WSORibbonInfo->items[i].u.bmp != NULL ) {
                    DeleteObject( WSORibbonInfo->items[i].u.bmp );
                }
                if( WSORibbonInfo->items[i].depressed != NULL ) {
                    DeleteObject( WSORibbonInfo->items[i].depressed );
                }
            }
        }
        WFreeToolBarInfo( WSORibbonInfo );
        WSORibbonInfo = NULL;
    }
}

Bool WCreateRibbon( WMenuEditInfo *einfo )
{
    RECT                r;
    WToolBarInfo        *rinfo;

    if( einfo == NULL || einfo->win == NULL ) {
        return( FALSE );
    }

    rinfo = WRibbonInfo;
    if( einfo->info->stand_alone ) {
        rinfo = WSORibbonInfo;
    }

    if( rinfo == NULL ) {
        return( FALSE );
    }

    GetClientRect( einfo->win, &r );

    rinfo->dinfo.area.right = r.right;

    einfo->ribbon = WCreateToolBar( rinfo, einfo->win );

    if( einfo->ribbon != NULL ) {
        einfo->show_ribbon = TRUE;
        return( TRUE );
    } else {
        return( FALSE );
    }
}

Bool WResizeRibbon( WMenuEditInfo *einfo, RECT *prect )
{
    if( einfo == NULL || einfo->ribbon == NULL || !einfo->show_ribbon || prect == NULL ||
        einfo->ribbon->win == (HWND)NULL ) {
        return( FALSE );
    }

    MoveWindow( einfo->ribbon->win, 0, 0, prect->right - prect->left,
                WRibbonHeight, TRUE );

    return( TRUE );
}

void WShowRibbon( WMenuEditInfo *einfo, HMENU menu )
{
    char        *mtext;

    if( einfo == NULL && menu == NULL ) {
        return;
    }

    mtext = NULL;

    if( einfo->show_ribbon ) {
        mtext = WAllocRCString( W_SHOWTOOLBAR );
        ShowWindow( einfo->ribbon->win, SW_HIDE );
        WSetStatusByID( einfo->wsb, -1, W_TOOLBARHIDDEN );
    } else {
        mtext = WAllocRCString( W_HIDETOOLBAR );
        ShowWindow( einfo->ribbon->win, SW_SHOW );
        WSetStatusByID( einfo->wsb, -1, W_TOOLBARSHOWN );
    }

    einfo->show_ribbon = !einfo->show_ribbon;
    WResizeWindows( einfo );
    WMovePrevWindow( einfo );
    ModifyMenu( menu, IDM_MENU_SHOWRIBBON, MF_BYCOMMAND | MF_STRING,
                IDM_MENU_SHOWRIBBON, mtext );

    if( mtext != NULL ) {
        WFreeRCString( mtext );
    }
}

void WDestroyRibbon( WMenuEditInfo *einfo )
{
    if( einfo != NULL && einfo->ribbon != NULL ) {
        WDestroyToolBar( einfo->ribbon );
        einfo->show_ribbon = FALSE;
        einfo->ribbon = NULL;
    }
}

void WRibbonHelpHook( HWND hwnd, WPARAM wParam, BOOL pressed )
{
    _wtouch( hwnd );
    if( !pressed ) {
        WSetStatusText( NULL, NULL, "" );
    } else {
        WDisplayHint( NULL, wParam );
    }
}

BOOL WRibbonHook( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    Bool            ret;
    WMenuEditInfo   *einfo;

    _wtouch( hwnd );
    _wtouch( wParam );
    _wtouch( lParam );

    einfo = WGetCurrentEditInfo();

    if( einfo == NULL || einfo->ribbon == NULL ) {
        return( FALSE );
    }

    ret = FALSE;

    switch( msg ) {
    case WM_DESTROY:
        WCloseToolBar( einfo->ribbon );
        einfo->ribbon = NULL;
        break;
    }

    return( ret );
}

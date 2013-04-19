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
#include "wreglbl.h"
#include "wremain.h"
#include "wretoolb.h"
#include "wrestat.h"
#include "wrehints.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wre_rc.h"
#include "wreribbn.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define BLANK_PAD    8
#define BUTTONX      22
#define BUTTONY      18
#define BUTTON_PAD   4
#define TOOL_BORDERX 4
#define TOOL_BORDERY 2

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
BOOL WRERibbonHook( HWND, UINT, WPARAM, LPARAM );
void WRERibbonHelpHook( HWND hwnd, WPARAM wParam, BOOL pressed );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    char    *up;
    char    *down;
    UINT    menu_id;
    int     tip_id;
} WRERibbonName;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
WRERibbonName WRERibbonNames[] = {
    { "New",    NULL, IDM_NEW,      WRE_TIP_NEW   },
    { "Open",   NULL, IDM_OPEN,     WRE_TIP_OPEN  },
    { "Save",   NULL, IDM_SAVE,     WRE_TIP_SAVE  },
    { NULL,     NULL, BLANK_PAD,    -1            },
    { "Cut",    NULL, IDM_CUT,      WRE_TIP_CUT   },
    { "Copy",   NULL, IDM_COPY,     WRE_TIP_COPY  },
    { "Paste",  NULL, IDM_PASTE,    WRE_TIP_PASTE }
};
#define NUM_TOOLS (sizeof( WRERibbonNames ) / sizeof( WRERibbonName ))

static WREToolBarInfo   *WRERibbonInfo  = NULL;
static WREToolBar       *WRERibbon      = NULL;
static int              WRERibbonHeight = 0;

int WREGetRibbonHeight( void )
{
    return( WRERibbonHeight );
}

Bool WREInitRibbon( HINSTANCE inst )
{
    int i;

    WRERibbonInfo = WREAllocToolBarInfo( NUM_TOOLS );

    if( WRERibbonInfo == NULL ) {
        return( FALSE );
    }

    for( i = 0; i < NUM_TOOLS; i++ ) {
        if( WRERibbonNames[i].up ) {
            WRERibbonInfo->items[i].u.bmp = LoadBitmap( inst, WRERibbonNames[i].up );
            WRERibbonInfo->items[i].id = WRERibbonNames[i].menu_id;
            WRERibbonInfo->items[i].flags = ITEM_DOWNBMP;
            if( WRERibbonNames[i].down ) {
                WRERibbonInfo->items[i].depressed =
                    LoadBitmap( inst, WRERibbonNames[i].down );
            } else {
                WRERibbonInfo->items[i].depressed = WRERibbonInfo->items[i].u.bmp;
            }
            if( WRERibbonNames[i].tip_id >= 0 ) {
                LoadString( inst, WRERibbonNames[i].tip_id, WRERibbonInfo->items[i].tip,
                            MAX_TIP );
            } else {
                WRERibbonInfo->items[i].tip[0] = '\0';
            }
        } else {
            WRERibbonInfo->items[i].flags = ITEM_BLANK;
            WRERibbonInfo->items[i].u.blank_space = WRERibbonNames[i].menu_id;
        }
    }

    WRERibbonInfo->dinfo.button_size.x = BUTTONX + BUTTON_PAD;
    WRERibbonInfo->dinfo.button_size.y = BUTTONY + BUTTON_PAD;
    WRERibbonInfo->dinfo.border_size.x = TOOL_BORDERX;
    WRERibbonInfo->dinfo.border_size.y = TOOL_BORDERY;
    WRERibbonInfo->dinfo.style = TOOLBAR_FIXED_STYLE;
    WRERibbonInfo->dinfo.hook = WRERibbonHook;
    WRERibbonInfo->dinfo.helphook = WRERibbonHelpHook;
    WRERibbonInfo->dinfo.foreground = NULL;
    WRERibbonInfo->dinfo.background = NULL;
    WRERibbonInfo->dinfo.is_fixed = TRUE;
    WRERibbonInfo->dinfo.use_tips = TRUE;

    return( TRUE );
}

void WREShutdownRibbon( void )
{
    int i;

    WREDestroyRibbon();

    if( WRERibbonInfo == NULL ) {
        return;
    }

    for( i = 0; i < NUM_TOOLS; i++ ) {
        if( WRERibbonInfo->items[i].flags != ITEM_BLANK ) {
            if( WRERibbonInfo->items[i].u.bmp == WRERibbonInfo->items[i].depressed ) {
                WRERibbonInfo->items[i].depressed = (HBITMAP)NULL;
            }
            if( WRERibbonInfo->items[i].u.bmp != NULL ) {
                DeleteObject( WRERibbonInfo->items[i].u.bmp );
            }
            if( WRERibbonInfo->items[i].depressed != NULL ) {
                DeleteObject( WRERibbonInfo->items[i].depressed );
            }
        }
    }

    WREFreeToolBarInfo( WRERibbonInfo );
}

Bool WRECreateRibbon( HWND parent )
{
    if( WRERibbon != NULL || WRERibbonInfo == NULL || parent == (HWND)NULL ) {
        return( FALSE );
    }

    GetClientRect( parent, &WRERibbonInfo->dinfo.area );

    WRERibbonHeight = 2 * WRERibbonInfo->dinfo.border_size.y +
                      WRERibbonInfo->dinfo.button_size.y +
                      2 * GetSystemMetrics( SM_CYBORDER );
    WRERibbonInfo->dinfo.area.bottom = WRERibbonHeight;

    WRERibbon = WRECreateToolBar( WRERibbonInfo, parent );

    WREResizeWindows();

    return( WRERibbon != NULL );
}

Bool WREResizeRibbon( RECT *prect )
{
    if( WRERibbon == NULL || WRERibbonHeight == NULL ||
        WRERibbon->win == (HWND)NULL || prect == NULL ) {
        return( FALSE );
    }

    MoveWindow( WRERibbon->win, 0, 0, prect->right - prect->left,
                WRERibbonHeight, TRUE );

    return( TRUE );
}

void WREShowRibbon( HMENU menu )
{
    char        *mtext;

    if( WRERibbonHeight != 0 ) {
        ShowWindow( WRERibbon->win, SW_HIDE );
        WRERibbonHeight = 0;
        WREResizeWindows();
        mtext = WREAllocRCString( WRE_SHOWTOOLBAR );
    } else {
        ShowWindow( WRERibbon->win, SW_SHOW );
        WRERibbonHeight = 2 * WRERibbonInfo->dinfo.border_size.y +
                          WRERibbonInfo->dinfo.button_size.y +
                          2 * GetSystemMetrics( SM_CYBORDER );
        WREResizeWindows();
        mtext = WREAllocRCString( WRE_HIDETOOLBAR );
    }

    ModifyMenu( menu, IDM_SHOW_RIBBON, MF_BYCOMMAND | MF_STRING,
                IDM_SHOW_RIBBON, mtext );

    if( mtext != NULL ) {
        WREFreeRCString( mtext );
    }
}

void WREDestroyRibbon( void )
{
    if( WRERibbon != NULL ) {
        WREDestroyToolBar( WRERibbon );
    }

    WRERibbonHeight = 0;

    WREResizeWindows();
}

void WRERibbonHelpHook( HWND hwnd, WPARAM wParam, BOOL pressed )
{
    _wre_touch( hwnd );

    if( !pressed ) {
        WRESetStatusText( NULL, "", TRUE );
    } else {
        WREDisplayHint( wParam );
    }
}

BOOL WRERibbonHook( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    Bool         ret;

    _wre_touch( hwnd );
    _wre_touch( wParam );
    _wre_touch( lParam );

    if( WRERibbon == NULL ) {
        return( FALSE );
    }

    ret = FALSE;

    switch( msg ) {
    case WM_DESTROY:
        WRECloseToolBar( WRERibbon );
        WRERibbon = NULL;
        break;
    }

    return( ret );
}

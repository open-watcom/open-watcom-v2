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

#include "wglbl.h"
#include "wmain.h"
#include "wtoolbar.h"
#include "wstat.h"
#include "whints.h"
#include "wmsg.h"
#include "sys_rc.h"
#include "wribbon.h"
#include "wmsgfile.h"

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
extern BOOL WRibbonHook         ( HWND, UINT, WPARAM, LPARAM );
extern void WRibbonHelpHook     ( HWND hwnd, WPARAM wParam, BOOL pressed );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    char    *up;
    char    *down;
    UINT     menu_id;
} WRibbonName;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
WRibbonName WRibbonNames[] =
{
    { "Clear"     , NULL , IDM_STR_CLEAR    }
,   { "Save"      , NULL , IDM_STR_UPDATE   }
,   { NULL        , NULL , BLANK_PAD        }
,   { "Cut"       , NULL , IDM_STR_CUT      }
,   { "Copy"      , NULL , IDM_STR_COPY     }
,   { "Paste"     , NULL , IDM_STR_PASTE    }
,   { NULL        , NULL , BLANK_PAD*3      }
,   { "InsertKey" , NULL , IDM_STR_NEWITEM  }
,   { "DeleteKey" , NULL , IDM_STR_DELETE   }
};
#define NUM_TOOLS (sizeof(WRibbonNames)/sizeof(WRibbonName))

WRibbonName WSORibbonNames[] =
{
    { "New"       , NULL , IDM_STR_CLEAR    }
,   { "Open"      , NULL , IDM_STR_OPEN     }
,   { "Save"      , NULL , IDM_STR_SAVE     }
,   { NULL        , NULL , BLANK_PAD        }
,   { "Cut"       , NULL , IDM_STR_CUT      }
,   { "Copy"      , NULL , IDM_STR_COPY     }
,   { "Paste"     , NULL , IDM_STR_PASTE    }
,   { NULL        , NULL , BLANK_PAD*3      }
,   { "InsertKey" , NULL , IDM_STR_NEWITEM  }
,   { "DeleteKey" , NULL , IDM_STR_DELETE   }
};
#define NUM_SOTOOLS (sizeof(WSORibbonNames)/sizeof(WRibbonName))

static WToolBarInfo *WRibbonInfo   = NULL;
static WToolBarInfo *WSORibbonInfo = NULL;

static int           WRibbonHeight = 0;

int WGetRibbonHeight ( void )
{
    return ( WRibbonHeight );
}

Bool WInitRibbons ( HINSTANCE inst )
{
    int  i;

    WRibbonInfo = WAllocToolBarInfo( NUM_TOOLS );
    WSORibbonInfo = WAllocToolBarInfo( NUM_SOTOOLS );

    if( !WRibbonInfo || !WSORibbonInfo ) {
        return( FALSE );
    }

    for ( i=0; i<NUM_TOOLS; i++ ) {
        if ( WRibbonNames[i].up ) {
            WRibbonInfo->items[i].bmp   =
                LoadBitmap ( inst, WRibbonNames[i].up );
            WRibbonInfo->items[i].id    = WRibbonNames[i].menu_id;
            WRibbonInfo->items[i].flags = ITEM_DOWNBMP;
            if ( WRibbonNames[i].down ) {
                WRibbonInfo->items[i].depressed =
                    LoadBitmap ( inst, WRibbonNames[i].down );
            } else {
                WRibbonInfo->items[i].depressed =
                    WRibbonInfo->items[i].bmp;
            }
        } else {
            WRibbonInfo->items[i].flags       = ITEM_BLANK;
            WRibbonInfo->items[i].blank_space = WRibbonNames[i].menu_id;
        }
    }

    for( i=0; i<NUM_SOTOOLS; i++ ) {
        if( WSORibbonNames[i].up ) {
            WSORibbonInfo->items[i].bmp   =
                LoadBitmap ( inst, WSORibbonNames[i].up );
            WSORibbonInfo->items[i].id    = WSORibbonNames[i].menu_id;
            WSORibbonInfo->items[i].flags = ITEM_DOWNBMP;
            if ( WSORibbonNames[i].down ) {
                WSORibbonInfo->items[i].depressed =
                    LoadBitmap ( inst, WSORibbonNames[i].down );
            } else {
                WSORibbonInfo->items[i].depressed =
                    WSORibbonInfo->items[i].bmp;
            }
        } else {
            WSORibbonInfo->items[i].flags       = ITEM_BLANK;
            WSORibbonInfo->items[i].blank_space = WSORibbonNames[i].menu_id;
        }
    }

    WRibbonInfo->dinfo.button_size.x = BUTTONX + BUTTON_PAD;
    WRibbonInfo->dinfo.button_size.y = BUTTONY + BUTTON_PAD;
    WRibbonInfo->dinfo.border_size.x = TOOL_BORDERX;
    WRibbonInfo->dinfo.border_size.y = TOOL_BORDERY;
    WRibbonInfo->dinfo.style         = TOOLBAR_FIXED_STYLE;
    WRibbonInfo->dinfo.hook          = WRibbonHook;
    WRibbonInfo->dinfo.helphook      = WRibbonHelpHook;
    WRibbonInfo->dinfo.foreground    = NULL;
    WRibbonInfo->dinfo.background    = NULL;
    WRibbonInfo->dinfo.is_fixed      = TRUE;

    WSORibbonInfo->dinfo.button_size.x = BUTTONX + BUTTON_PAD;
    WSORibbonInfo->dinfo.button_size.y = BUTTONY + BUTTON_PAD;
    WSORibbonInfo->dinfo.border_size.x = TOOL_BORDERX;
    WSORibbonInfo->dinfo.border_size.y = TOOL_BORDERY;
    WSORibbonInfo->dinfo.style         = TOOLBAR_FIXED_STYLE;
    WSORibbonInfo->dinfo.hook          = WRibbonHook;
    WSORibbonInfo->dinfo.helphook      = WRibbonHelpHook;
    WSORibbonInfo->dinfo.foreground    = NULL;
    WSORibbonInfo->dinfo.background    = NULL;
    WSORibbonInfo->dinfo.is_fixed      = TRUE;

    WRibbonHeight = 2 * WRibbonInfo->dinfo.border_size.y +
                    WRibbonInfo->dinfo.button_size.y +
                    2 * GetSystemMetrics(SM_CYBORDER);

    WRibbonInfo->dinfo.area.bottom = WRibbonHeight;
    WSORibbonInfo->dinfo.area.bottom = WRibbonHeight;

    return ( TRUE );
}

void WShutdownRibbons ( void )
{
    int  i;

    if ( WRibbonInfo ) {
        for ( i=0; i<NUM_TOOLS; i++ ) {
            if ( WRibbonInfo->items[i].flags != ITEM_BLANK ) {
                if ( WRibbonInfo->items[i].bmp ==
                     WRibbonInfo->items[i].depressed ) {
                    WRibbonInfo->items[i].depressed = (HBITMAP) NULL;
                }
                if ( WRibbonInfo->items[i].bmp ) {
                    DeleteObject ( WRibbonInfo->items[i].bmp );
                }
                if ( WRibbonInfo->items[i].depressed ) {
                    DeleteObject ( WRibbonInfo->items[i].depressed );
                }
            }
        }
        WFreeToolBarInfo ( WRibbonInfo );
        WRibbonInfo = NULL;
    }

    if ( WSORibbonInfo ) {
        for ( i=0; i<NUM_SOTOOLS; i++ ) {
            if ( WSORibbonInfo->items[i].flags != ITEM_BLANK ) {
                if ( WSORibbonInfo->items[i].bmp ==
                     WSORibbonInfo->items[i].depressed ) {
                    WSORibbonInfo->items[i].depressed = (HBITMAP) NULL;
                }
                if ( WSORibbonInfo->items[i].bmp ) {
                    DeleteObject ( WSORibbonInfo->items[i].bmp );
                }
                if ( WSORibbonInfo->items[i].depressed ) {
                    DeleteObject ( WSORibbonInfo->items[i].depressed );
                }
            }
        }
        WFreeToolBarInfo ( WSORibbonInfo );
        WSORibbonInfo = NULL;
    }
}

Bool WCreateRibbon ( WStringEditInfo *einfo )
{
    RECT                r;
    WToolBarInfo        *rinfo;

    if( !einfo || !einfo->win ) {
        return( FALSE );
    }

    rinfo = WRibbonInfo;
    if( einfo->info->stand_alone ) {
        rinfo = WSORibbonInfo;
    }

    if( !rinfo ) {
        return( FALSE );
    }

    GetClientRect( einfo->win, &r );

    rinfo->dinfo.area.right  = r.right;

    einfo->ribbon = WCreateToolBar( rinfo, einfo->win );

    if( einfo->ribbon ) {
        einfo->show_ribbon = TRUE;
        return( TRUE );
    } else {
        return( FALSE );
    }
}

Bool WResizeRibbon ( WStringEditInfo *einfo, RECT *prect )
{
    if ( !einfo || !einfo->ribbon || !einfo->show_ribbon || !prect ||
         ( einfo->ribbon->win == (HWND)NULL ) ) {
        return ( FALSE );
    }

    MoveWindow ( einfo->ribbon->win, 0, 0, ( prect->right - prect->left ),
                 WRibbonHeight, TRUE );

    return ( TRUE );
}

void WShowRibbon( WStringEditInfo *einfo, HMENU menu )
{
    char        *mtext;

    if ( !einfo && !menu ) {
        return;
    }

    mtext = NULL;

    if( einfo->show_ribbon ) {
        mtext = WAllocRCString( W_SHOWTOOLBAR );
        ShowWindow( einfo->ribbon->win, SW_HIDE );
        WSetStatusByID( einfo->wsb, -1, W_TOOLBARHIDDEN );
    } else {
        mtext = WAllocRCString( W_HIDETOOLBAR );
        ShowWindow ( einfo->ribbon->win, SW_SHOW );
        WSetStatusByID( einfo->wsb, -1, W_TOOLBARSHOWN );
    }

    einfo->show_ribbon = !einfo->show_ribbon;
    WResizeWindows( einfo );
    ModifyMenu( menu, IDM_STR_SHOWRIBBON, MF_BYCOMMAND | MF_STRING,
                IDM_STR_SHOWRIBBON, mtext );

    if( mtext ) {
        WFreeRCString( mtext );
    }
}

void WDestroyRibbon ( WStringEditInfo *einfo )
{
    if ( einfo && einfo->ribbon ) {
        WDestroyToolBar ( einfo->ribbon );
        einfo->show_ribbon = FALSE;
        einfo->ribbon      = NULL;
    }
}

void WRibbonHelpHook ( HWND hwnd, WPARAM wParam, BOOL pressed )
{
    _wtouch(hwnd);
    if ( !pressed ) {
        WSetStatusText ( NULL, NULL, "" );
    } else {
        WDisplayHint ( NULL, wParam );
    }
}

BOOL WRibbonHook ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    Bool            ret;
    WStringEditInfo *einfo;

    _wtouch(hwnd);
    _wtouch(wParam);
    _wtouch(lParam);

    einfo = WGetCurrentEditInfo ();

    if ( !einfo || !einfo->ribbon ) {
        return( FALSE );
    }

    ret = FALSE;

    switch ( msg ) {
        case WM_DESTROY:
            WCloseToolBar ( einfo->ribbon );
            einfo->ribbon = NULL;
            break;
    }

    return ( ret );
}


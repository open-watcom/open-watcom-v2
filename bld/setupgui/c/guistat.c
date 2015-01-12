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
* Description:  Installer GUI status bar.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gui.h"
#include "guiutil.h"
#include "guikey.h"
#include "dlggen.h"
#include "setup.h"
#include "setupwpi.h"
#include "setupinf.h"
#include "guistr.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "genvbl.h"
#include "utils.h"
#if defined( _UI )
  #include "stdui.h"
  #include "uigchar.h"
#endif

#include "clibext.h"

#define LINE0_ROW       1
#define LINE1_ROW       2
#define LINE0_COL       1
#define LINE1_COL       1
#define STATUS_WIDTH    70
#define STATUS_HEIGHT   10
#define CANNERY_SIZE    10
#define CANNERY_ROW     6
#define STATUS_ROW      4
#define BAR_INDENT      4

extern gui_colour_set   StatusColours[];
extern gui_colour_set   StatusBackground;
extern gui_window       *MainWnd;
extern int              IsPatch;
extern gui_ord          BitMapBottom;
extern gui_coord        GUIScale;

int                     MsgLine0 = STAT_BLANK;
bool                    CancelSetup = false;
gui_colour_set          ToolPlain = { GUI_BLACK, GUI_BLUE, };
gui_colour_set          ToolStandout = { GUI_BRIGHT_WHITE, GUI_BLUE };

static gui_window       *StatusWnd;
static gui_rect         StatusBarRect;
static gui_coord        CharSize;
static int              Percent;
static long             Parts_Injob;
static long             Parts_Complete;
static char             StatusLine1[_MAX_PATH];
static gui_ord          StatusBarLen;
static gui_rect         StatusRect;
static char             StatusBarBuf[256];
static GUICALLBACK      StatusEventProc;

static const char *Messages[] = {
    #define pick( x, y ) y,
    #include "status.h"
    #undef pick
};

static gui_create_info StatusInfo = {
    NULL,                               // Title
    { 1500, 2500, 6500, 6000 },         // Position
    GUI_NOSCROLL,                       // Scroll Styles
    GUI_VISIBLE                         // Window Styles
//  | GUI_CLOSEABLE
    | GUI_SYSTEM_MENU
//  | GUI_RESIZEABLE
//  | GUI_MAXIMIZE
//  | GUI_MINIMIZE
    /*| GUI_DIALOG_LOOK*/,
    NULL,                               // Parent
    0,                                  // Number of menus
    NULL,                               // Menu's
    WND_NUMBER_OF_COLORS,               // number of color attributes
                                        // ArraySize( StatusColours );
    StatusColours,                      // Array of color attributes
    &StatusEventProc,                   // Callback function
    NULL,                               // Extra
    NULL                                // Icon
};

static gui_control_info Cancel = {
      GUI_DEFPUSH_BUTTON, NULL, 0, 0, 0, 0, NULL, // nyi - kanji
      GUI_NOSCROLL, GUI_TAB_GROUP | GUI_AUTOMATIC, CTL_CANCEL
};

void StatusShow( bool show )
/**************************/
{
    if( StatusWnd == NULL && show ) {
        StatusInit();
    }
    if( StatusWnd != NULL ) {
        if( show ) {
            GUIShowWindow( StatusWnd );
        } else {
            GUIHideWindow( StatusWnd );
        }
        GUIWndDirty( StatusWnd );
    }
}

void StatusFini( void )
/*********************/
{
    if( StatusWnd == NULL ){
        return;
    } else {
        if( StatusInfo.title != NULL ) {
            GUIMemFree( (void *)StatusInfo.title );
        }
        GUIDestroyWnd( StatusWnd );
        StatusWnd = NULL;
    }
}

void StatusLines( int msg0, const char *message1 )
/******************************************/
{
    if( StatusWnd != NULL ) {
        if( message1 != NULL ) {
            if( strcmp( message1, StatusLine1 ) != 0 ) {
                strcpy( StatusLine1, message1 );
                GUIWndDirtyRow( StatusWnd, LINE1_ROW );
            }
        }
        if( msg0 != STAT_SAME ) {
            if( msg0 != MsgLine0 ) {
                MsgLine0 = msg0;
                GUIWndDirty( StatusWnd );
            }
        }
    }
}

void BumpStatus( long by )
/************************/
{
    if( !IsPatch ) {
        // if a patch, don't change status because denominator of status
        // fraction is the number of operations, not a number of bytes
        StatusAmount( Parts_Complete + by, Parts_Injob );
    }
}

void StatusAmount( long parts_complete, long parts_injob )
/********************************************************/
// Display slider bar indicating percentage complete
{
    int                 old_percent;

    Parts_Injob = parts_injob;
    Parts_Complete = parts_complete;
    old_percent = Percent;
    if( parts_injob == 0 ) {
        if( parts_complete == 0 ) {
            Percent = 0;
        } else {
            Percent = 100;
        }
    } else {
        if( Parts_Complete > Parts_Injob ) {
            Parts_Complete = Parts_Injob;
        }
        if( Parts_Injob > 100000 ) {
            Percent = Parts_Complete / (Parts_Injob / 100);
        } else {
            Percent = (100 * Parts_Complete) / Parts_Injob;
        }
        if( Percent > 100 ) {
            Percent = 100;
        }
    }
    if( old_percent == Percent ) return;
    if( Percent != 0 && Percent < old_percent ) {
        Percent = old_percent;
        return;
    }
    if( StatusWnd == NULL ) return;
#ifdef _UI
    GUIWndDirty( StatusWnd );
#else
    {
        gui_ord         bar_width, old_divider, divider;
        gui_rect        rect;

        sprintf( StatusBarBuf, "%d%%", Percent );

        // calculate where divider splits rectangle
        bar_width = StatusBarRect.width;
        divider = (bar_width * (long)Percent) / 100;
        if( divider < 0 ) {
            divider = 0;
        } else if( divider > bar_width ) {
            divider = bar_width;
        }
        old_divider = (bar_width * (long)old_percent) / 100;
        if( old_divider < 0 ) {
            old_divider = 0;
        } else if( old_divider > bar_width ) {
            old_divider = bar_width;
        }
        if( divider <= old_divider ) {
            GUIWndDirty( StatusWnd );
        } else {
            // dirty new bit of bar
            divider += StatusBarRect.x;
            old_divider += StatusBarRect.x;
            rect = StatusBarRect;
            rect.width = GUIGetExtentX( StatusWnd, StatusBarBuf, strlen( StatusBarBuf ) );
            rect.x = StatusBarRect.x + (StatusBarRect.width - rect.width) / 2;
            rect.x -= CharSize.x / 2;
            rect.width += CharSize.x;
            GUIWndDirtyRect( StatusWnd, &rect ); // dirty text
            rect.x = old_divider - CharSize.x;
            rect.width = divider - old_divider + 2 * CharSize.x;
            GUIWndDirtyRect( StatusWnd, &rect ); // dirty new bit of bar
        }
    }
#endif
}

bool StatusCancelled( void )
/**************************/
{
    // update windows and let other apps execute
    GUIDrainEvents();
    return( CancelSetup );
}

static bool StatusEventProc( gui_window *gui, gui_event gui_ev, void *parm )
/**************************************************************************/
{
    static bool         button_pressed = false;
    unsigned            id;
    gui_key             key;
    gui_keystate        state;
    const char          *msg;

    parm = parm;
    if( gui == NULL )
        return( false );

    switch( gui_ev ) {

    case GUI_INIT_WINDOW:
        return( true );

    case GUI_PAINT:
        {
            if( StatusBarLen == 0 ) {
                break;
            }
            msg = GetVariableStrVal( Messages[MsgLine0] );
            GUIDrawTextExtent( gui, msg, strlen( msg ), LINE0_ROW,
                               LINE0_COL * CharSize.x, WND_STATUS_TEXT, GUI_NO_COLUMN );
            GUIDrawTextExtent( gui, StatusLine1, strlen( StatusLine1 ), LINE1_ROW,
                               LINE1_COL * CharSize.x, WND_STATUS_TEXT, GUI_NO_COLUMN );
#ifdef _UI
            {
                int         len1, len2;
                char        num[20];

                memset( StatusBarBuf, ' ', StatusBarLen );
                StatusBarBuf[StatusBarLen] = '\0';
                itoa( Percent, num, 10 );
                strcat( num, "%" );
                memcpy( StatusBarBuf + StatusBarLen / 2 - 1, num, strlen( num ) );
                // draw bar in two parts
                len1 = (StatusBarLen * (long)Percent) / 100;
                if( len1 < 0 ) {
                    len1 = 0;
                } else if( len1 > StatusBarLen ) {
                    len1 = StatusBarLen;
                }
                len2 = StatusBarLen - len1;
                if( len1 > 0 ) {
                    GUIDrawText( gui, StatusBarBuf, len1, STATUS_ROW,
                                 StatusBarRect.x, WND_STATUS_BAR );
                }
                if( len2 > 0 ) {
                    GUIDrawText( gui, StatusBarBuf + len1, len2, STATUS_ROW,
                                 StatusBarRect.x + len1 * CharSize.x, WND_STATUS_TEXT );
                }
                memset( StatusBarBuf, UiGChar[UI_SBOX_TOP_LINE], StatusBarLen );
                GUIDrawText( gui, StatusBarBuf, StatusBarLen, STATUS_ROW - 1,
                             StatusBarRect.x, WND_STATUS_TEXT );
                GUIDrawText( gui, StatusBarBuf, StatusBarLen, STATUS_ROW + 1,
                             StatusBarRect.x, WND_STATUS_TEXT );
            }
#else
            {
                gui_coord   coord;
                int         str_len, width, height;
                int         bar_width, len1, len2, divider;
                gui_point   start, end;
                gui_rect    rStatusBar;

//              sprintf( StatusBarBuf, "%d%%", Percent );
                // clear whole bar
                GUIFillRect( gui, &StatusBarRect, WND_STATUS_BAR );
                // calculate where divider splits rectangle
                bar_width = StatusBarRect.width;
                divider = (bar_width * (long)Percent) / 100;
                if( divider < 0 ) {
                    divider = 0;
                } else if( divider > bar_width ) {
                    divider = bar_width;
                }
                rStatusBar = StatusBarRect;
                rStatusBar.width = divider;
                // calculate position for text (centre it)
                str_len = strlen( StatusBarBuf );
                width = GUIGetExtentX( gui, StatusBarBuf, str_len );
                height = GUIGetExtentY( gui, StatusBarBuf );
                coord.y = StatusBarRect.y + (StatusBarRect.height - height) / 2;
                coord.x = StatusBarRect.x + (StatusBarRect.width - width) / 2;
                divider += StatusBarRect.x;
                if( coord.x > divider ) {
                    // text is completely to right of divider
                    GUIFillRect( gui, &rStatusBar, WND_STATUS_TEXT );
                    GUIDrawTextPos( gui, StatusBarBuf, str_len, &coord,
                                    WND_STATUS_TEXT );
                } else if( coord.x + width < divider ) {
                    // text is completely to left of divider
                    GUIFillRect( gui, &rStatusBar, WND_STATUS_TEXT );
                    GUIDrawTextPos( gui, StatusBarBuf, str_len, &coord,
                                    WND_STATUS_BAR );
                } else {
                    // need to split text
                    len1 = ((long)(divider - coord.x) * str_len) / width;
                    if( len1 < 0 ) {
                        len1 = 0;
                    } else if( len1 > str_len ) {
                        len1 = str_len;
                    }
                    len2 = str_len - len1;
                    // recalc divider, so it falls on a character boundary
                    divider = coord.x + GUIGetExtentX( gui, StatusBarBuf, len1 );
                    rStatusBar.width = divider - StatusBarRect.x;
                    GUIFillRect( gui, &rStatusBar, WND_STATUS_TEXT );
                    if( len1 > 0 ) {
                        GUIDrawTextPos( gui, StatusBarBuf, len1, &coord,
                                        WND_STATUS_BAR );
                    }
                    if( len2 > 0 ) {
                        coord.x = divider;
                        GUIDrawTextPos( gui, StatusBarBuf + len1, len2, &coord,
                                        WND_STATUS_TEXT );
                    }
                }
                // draw frame
                start.x = StatusBarRect.x;
                start.y = StatusBarRect.y;
                end.x = StatusBarRect.width + StatusBarRect.x;
                end.y = start.y;                                // top line
                GUIDrawLine( gui, &start, &end, GUI_PEN_SOLID, 1, WND_STATUS_FRAME );
                start.y = StatusBarRect.y + StatusBarRect.height; // bottom line
                end.y = start.y;
                GUIDrawLine( gui, &start, &end, GUI_PEN_SOLID, 1, WND_STATUS_FRAME );
                end.y = StatusBarRect.y;
                start.x = StatusBarRect.x;
                end.x = start.x;                            // left side
                GUIDrawLine( gui, &start, &end, GUI_PEN_SOLID, 1, WND_STATUS_FRAME );
                start.x = StatusBarRect.x + StatusBarRect.width;
                end.x = start.x;                           // right side
                GUIDrawLine( gui, &start, &end, GUI_PEN_SOLID, 1, WND_STATUS_FRAME );
            }
#endif
            return( false );
        }

    case GUI_DESTROY:
        StatusWnd = NULL;
        return( false );

    case GUI_CONTROL_CLICKED:
        GUIGetFocus( gui, &id );
        GUI_GETID( parm, id );
        switch( id ) {
        case CTL_CANCEL:
            if( !button_pressed ) {
                button_pressed = true;
                if( MsgBox( gui, "IDS_QUERYABORT", GUI_YES_NO ) == GUI_RET_YES ) {
                    CancelSetup = true;
                }
                button_pressed = false;
                break;
            }
        case CTL_DONE:
            if( !button_pressed ) {
                CancelSetup = true;
                break;
            }
        }
        return( true );
    case GUI_KEYDOWN:
        GUI_GET_KEY_STATE( parm, key, state );
        state = state;
        switch( key ) {
        case GUI_KEY_ESCAPE:
            if( !button_pressed ) {
                button_pressed = true;
                if( MsgBox( gui, "IDS_QUERYABORT", GUI_YES_NO ) == GUI_RET_YES ) {
                    CancelSetup = true;
                }
                button_pressed = false;
                break;
            }
        default:
            break;
        }
        return( true );
    default:
        break;
    }
    return( false );
}

static bool OpenStatusWindow( const char *title )
/***********************************************/
{
    gui_text_metrics    metrics;
//    int                 i;
    gui_rect            rect;

//    for( i = STAT_BLANK; i < sizeof( Messages ) / sizeof( Messages[0] ); ++i ) {
//      Messages[i] = GetVariableStrVal( Messages[i] );
//    }
    GUIGetDlgTextMetrics( &metrics );
    CharSize.x = metrics.avg.x;
    CharSize.y = 5 * metrics.avg.y / 4;
    GUITruncToPixel( &CharSize );

    StatusInfo.parent = MainWnd;
    StatusInfo.title = GUIStrDup( title, NULL );
    StatusInfo.rect.width = STATUS_WIDTH * CharSize.x;
    StatusInfo.rect.height = STATUS_HEIGHT * CharSize.y;
    GUIGetClientRect( MainWnd, &rect );
    if( GUIIsGUI() ) {
        StatusInfo.rect.y = BitMapBottom;
    } else {
        StatusInfo.rect.y = (GUIScale.y - StatusInfo.rect.height) / 2;
    }
    if( StatusInfo.rect.y > rect.height - StatusInfo.rect.height ) {
        StatusInfo.rect.y = rect.height - StatusInfo.rect.height;
    }
    StatusInfo.rect.x = (GUIScale.x - StatusInfo.rect.width) / 2;

    StatusBarLen = 0;

    StatusWnd = GUICreateWindow( &StatusInfo );

    GUIGetClientRect( StatusWnd, &StatusRect );

    Cancel.parent = StatusWnd;
    Cancel.text = LIT( Cancel );
    Cancel.rect.height = 7 * CharSize.y / 4;
    Cancel.rect.width = (strlen( Cancel.text ) + 4) * CharSize.x;
    Cancel.rect.x = (StatusRect.width - Cancel.rect.width) / 2;
    Cancel.rect.y = CANNERY_ROW * CharSize.y;

    StatusBarRect.x = BAR_INDENT * CharSize.x;
    StatusBarRect.width = StatusRect.width - 2 * BAR_INDENT * CharSize.x;
    StatusBarRect.y = STATUS_ROW * CharSize.y;
    StatusBarRect.height = CharSize.y;
#ifndef _UI
    StatusBarRect.y -= CharSize.y / 2;
    StatusBarRect.height += CharSize.y;
#endif

    StatusBarLen = StatusBarRect.width / CharSize.x;

    if( !GUIAddControl( &Cancel, &ToolPlain, &ToolStandout ) ) {
        SetupError( "IDS_CONTROLERROR" );
        return( false );
    }
    return( true );
}

bool StatusInit( void )
/*********************/
{
    char        buff[MAXBUF];

    ReplaceVars( buff, sizeof( buff ), GetVariableStrVal( "AppName" ) );
    return( OpenStatusWindow( buff ) );
}

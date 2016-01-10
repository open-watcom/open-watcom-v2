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


#include "imgedit.h"
#include "funcbar.h"

#define NUM_TOOLS       26
#define NUM_TOOLS_DDE   23

static void             *functionBar;
static HWND             hFunctionBar = NULL;

static button           toolList[NUM_TOOLS] = {
    { NEWBMP,       IMGED_NEW,      FALSE,  NONE,           0, 0, WIE_TIP_NEW       },
    { OPENBMP,      IMGED_OPEN,     FALSE,  NONE,           0, 0, WIE_TIP_OPEN      },
    { SAVEBMP,      IMGED_SAVE,     FALSE,  NONE,           0, 0, WIE_TIP_SAVE      },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { GRIDBMP,      IMGED_GRID,     TRUE,   GRIDDBMP,       0, 0, WIE_TIP_GRID      },
    { MAXIMIZEBMP,  IMGED_MAXIMIZE, FALSE,  MAXIMIZEDBMP,   0, 0, WIE_TIP_MAXIMIZE  },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { CUTBMP,       IMGED_CUT,      FALSE,  NONE,           0, 0, WIE_TIP_CUT       },
    { COPYBMP,      IMGED_COPY,     FALSE,  NONE,           0, 0, WIE_TIP_COPY      },
    { PASTEBMP,     IMGED_PASTE,    FALSE,  NONE,           0, 0, WIE_TIP_PASTE     },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { UNDOBMP,      IMGED_UNDO,     FALSE,  NONE,           0, 0, WIE_TIP_UNDO      },
    { REDOBMP,      IMGED_REDO,     FALSE,  NONE,           0, 0, WIE_TIP_REDO      },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { CLEARBMP,     IMGED_CLEAR,    FALSE,  NONE,           0, 0, WIE_TIP_CLEAR     },
    { SNAPBMP,      IMGED_SNAP,     FALSE,  NONE,           0, 0, WIE_TIP_SNAP      },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { RIGHTBMP,     IMGED_RIGHT,    FALSE,  NONE,           0, 0, WIE_TIP_RIGHT     },
    { LEFTBMP,      IMGED_LEFT,     FALSE,  NONE,           0, 0, WIE_TIP_LEFT      },
    { UPBMP,        IMGED_UP,       FALSE,  NONE,           0, 0, WIE_TIP_UP        },
    { DOWNBMP,      IMGED_DOWN,     FALSE,  NONE,           0, 0, WIE_TIP_DOWN      },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { HFLIPBMP,     IMGED_FLIPHORZ, FALSE,  HFLIPDBMP,      0, 0, WIE_TIP_FLIPHORZ  },
    { VFLIPBMP,     IMGED_FLIPVERT, FALSE,  VFLIPDBMP,      0, 0, WIE_TIP_FLIPVERT  },
    { CLROTBMP,     IMGED_ROTATECL, FALSE,  CLROTDBMP,      0, 0, WIE_TIP_ROTATECL  },
    { CCROTBMP,     IMGED_ROTATECC, FALSE,  CCROTDBMP,      0, 0, WIE_TIP_ROTATECC  }
};

static button           toolListDDE[NUM_TOOLS_DDE] = {
    { CLEARBMP,     IMGED_CLEAR,    FALSE,  NONE,           0, 0, WIE_TIP_CLEAR     },
    { SAVEBMP,      IMGED_DDE_UPDATE_PRJ, FALSE, NONE,      0, 0, WIE_TIP_UPDATE    },
    { GRIDBMP,      IMGED_GRID,     FALSE,  GRIDDBMP,       0, 0, WIE_TIP_GRID      },
    { MAXIMIZEBMP,  IMGED_MAXIMIZE, FALSE,  MAXIMIZEDBMP,   0, 0, WIE_TIP_MAXIMIZE  },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { CUTBMP,       IMGED_CUT,      FALSE,  NONE,           0, 0, WIE_TIP_CUT       },
    { COPYBMP,      IMGED_COPY,     FALSE,  NONE,           0, 0, WIE_TIP_COPY      },
    { PASTEBMP,     IMGED_PASTE,    FALSE,  NONE,           0, 0, WIE_TIP_PASTE     },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { UNDOBMP,      IMGED_UNDO,     FALSE,  NONE,           0, 0, WIE_TIP_UNDO      },
    { REDOBMP,      IMGED_REDO,     FALSE,  NONE,           0, 0, WIE_TIP_REDO      },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { SNAPBMP,      IMGED_SNAP,     FALSE,  NONE,           0, 0, WIE_TIP_SNAP      },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { RIGHTBMP,     IMGED_RIGHT,    FALSE,  NONE,           0, 0, WIE_TIP_RIGHT     },
    { LEFTBMP,      IMGED_LEFT,     FALSE,  NONE,           0, 0, WIE_TIP_LEFT      },
    { UPBMP,        IMGED_UP,       FALSE,  NONE,           0, 0, WIE_TIP_UP        },
    { DOWNBMP,      IMGED_DOWN,     FALSE,  NONE,           0, 0, WIE_TIP_DOWN      },
    { NONE,         0,              FALSE,  NONE,           0, 0, -1                },
    { HFLIPBMP,     IMGED_FLIPHORZ, FALSE,  HFLIPDBMP,      0, 0, WIE_TIP_FLIPHORZ  },
    { VFLIPBMP,     IMGED_FLIPVERT, FALSE,  VFLIPDBMP,      0, 0, WIE_TIP_FLIPVERT  },
    { CLROTBMP,     IMGED_ROTATECL, FALSE,  CLROTDBMP,      0, 0, WIE_TIP_ROTATECL  },
    { CCROTBMP,     IMGED_ROTATECC, FALSE,  CCROTDBMP,      0, 0, WIE_TIP_ROTATECC  }
};
    
/*
 * addFunctionButton - add a button to the function bar
 */
static void addFunctionButton( button *tb )
{
    TOOLITEMINFO        info;

    if( tb->id > 0 ) {
        tb->hbmp = _wpi_loadbitmap( Instance, tb->name );
        info.u.bmp = tb->hbmp;
        info.id = tb->id;
        info.flags = ITEM_DOWNBMP;

        if( tb->sticky ) {
            info.flags |= ITEM_STICKY;
        }
        if( tb->downbmp != NONE ) {
            tb->downbmp = _wpi_loadbitmap( Instance, tb->downname );
        } else {
            tb->downbmp = tb->hbmp;
        }
        if( tb->tip_id >= 0 ) {
            _wpi_loadstring( Instance, tb->tip_id, info.tip, MAX_TIP );
        } else {
            info.tip[0] = '\0';
        }

        info.depressed = tb->downbmp;
    } else {
        info.flags = ITEM_BLANK;
        info.u.blank_space = 5;
    };
    ToolBarAddItem( functionBar, &info );

} /* addFunctionButton */

/*
 * addItems - add the buttons to the function bar
 */
static void addItems( void )
{
    int i;

    if( ImgedIsDDE ) {
        for( i = 0; i < NUM_TOOLS_DDE; i++ ) {
            addFunctionButton( &toolListDDE[i] );
        }
    } else {
        for( i = 0; i < NUM_TOOLS; i++ ) {
            addFunctionButton( &toolList[i] );
        }
    }

} /* addItems */

/*
 * FunctionBarHelpProc
 */
void FunctionBarHelpProc( HWND hwnd, int id, bool pressed )
{
    hwnd = hwnd;
    if( pressed ) {
        ShowHintText( id );
    } else {
        SetHintText( " " );
    }

} /* FunctionBarHelpProc */

/*
 * FunctionBarProc - hook function which intercepts messages to the toolbar
 */
bool FunctionBarProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    short               i;
    static BOOL         gridButtonDown = FALSE;
    int                 id;

    hwnd = hwnd;

    switch( msg ) {
    case WM_CREATE:
        break;

    case WM_USER:
        id = LOWORD( wparam );
        if( !lparam ) {
            ShowHintText( id );
        }

        if( id == IMGED_GRID ) {
            ToolBarSetState( functionBar, id, BUTTON_DOWN );
        } else {
            return( true );
        }
        break;

    case WM_COMMAND:
        id = LOWORD( wparam );
        if( id == IMGED_GRID ) {
            if( !gridButtonDown ) {
                gridButtonDown = TRUE;
            } else {
                gridButtonDown = FALSE;
            }
            if( HMainWindow == NULL ) {
                break;
            }
            CheckGridItem( _wpi_getmenu( _wpi_getframe( HMainWindow ) ) );
            return( true );
        }

        ToolBarSetState( functionBar, id, BUTTON_UP );
        break;

    case WM_DESTROY:
        if( ImgedIsDDE ) {
            for( i = 0; i < NUM_TOOLS_DDE; i++ ) {
                if( toolListDDE[i].name != NONE ) {
                    _wpi_deletebitmap( toolListDDE[i].hbmp );
                }
                if( toolListDDE[i].downbmp != NONE ) {
                    _wpi_deletebitmap( toolListDDE[i].downbmp );
                }
            }
        } else {
            for( i = 0; i < NUM_TOOLS; i++ ) {
                if( toolList[i].name != NONE ) {
                    _wpi_deletebitmap( toolList[i].hbmp );
                }
                if( toolList[i].downbmp != NONE ) {
                    _wpi_deletebitmap( toolList[i].downbmp );
                }
            }
        }
        break;
    }
    return( false );

} /* FunctionBarProc */

/*
 * InitFunctionBar - initializes the toolbar at the top of the image editor
 */
void InitFunctionBar( HWND hparent )
{
    WPI_POINT           buttonsize = { FUNC_BUTTON_WIDTH, FUNC_BUTTON_HEIGHT };
    WPI_POINT           border = { FUNC_BORDER_X, FUNC_BORDER_Y };
    WPI_RECT            functionbar_loc;
    TOOLDISPLAYINFO     tdi;
    WPI_RECT            rect;
    int                 width;
    int                 height;
    int                 max_width;

    _wpi_getclientrect( hparent, &rect );
    width = _wpi_getwidthrect( rect );
    height = _wpi_getheightrect( rect );

    max_width = width;
    if( max_width < MIN_WIDTH )
        max_width = MIN_WIDTH;
    _wpi_setwrectvalues( &functionbar_loc, 0, 0, max_width, FUNC_BUTTON_HEIGHT + 5 );
    _wpi_cvth_rect( &functionbar_loc, height );
#ifdef __OS2_PM__
    functionbar_loc.yBottom += 1;
#endif

    functionBar = ToolBarInit( hparent );
    tdi.button_size = buttonsize;
    tdi.border_size = border;
    tdi.area = functionbar_loc;
    tdi.style = TOOLBAR_FIXED_STYLE;
    tdi.hook = FunctionBarProc;
    tdi.helphook = FunctionBarHelpProc;
    tdi.background = (HBITMAP)0;
    tdi.foreground = (HBRUSH)0;
    tdi.is_fixed = 1;
    tdi.use_tips = 1;

    ToolBarDisplay( functionBar, &tdi );

    addItems();
    hFunctionBar = ToolBarWindow( functionBar );

    _wpi_showwindow( hFunctionBar, SW_SHOWNORMAL );
    _wpi_updatewindow( hFunctionBar );

    GrayEditOptions();

} /* InitFunctionBar */

/*
 * CloseFunctionBar - call the clean up routine
 */
void CloseFunctionBar( void )
{
    ToolBarFini( functionBar );

} /* CloseFunctionBar */

/*
 * ResizeFunctionBar - handle the resizing of the function bar
 */
void ResizeFunctionBar( WPI_PARAM2 lparam )
{
    short       width;
    int         top;
    int         bottom;
    HWND        hwnd;
#ifdef __OS2_PM__
    short       height;
    WPI_RECT    rect;
#endif

    if( hFunctionBar == NULL ) {
        return;
    }
#ifndef __OS2_PM__
    width = LOWORD( lparam );
    if( width < MIN_WIDTH )
        width = MIN_WIDTH;
    top = 0;
    bottom = FUNC_BUTTON_HEIGHT + 5;
#else
    width = SHORT1FROMMP( lparam );
    if( width < MIN_WIDTH )
        width = MIN_WIDTH;
    _wpi_getclientrect( HMainWindow, &rect );
    height = _wpi_getheightrect( rect );
    /*
     * These are actually switched, but it's necessary for the _wpi_setwindowpos macro.
     */
    bottom = _wpi_cvth_y( 0, height );
    top = _wpi_cvth_y( FUNC_BUTTON_HEIGHT + 3, height );
#endif

    hwnd = ToolBarWindow( functionBar );
    _wpi_setwindowpos( hwnd, HWND_TOP, 0, top, width, bottom,
                       SWP_SHOWWINDOW | SWP_MOVE | SWP_SIZE );

} /* ResizeFunctionBar */

/*
 * PressGridButton - (de)press the grid button as necessary
 */
void PressGridButton( void )
{
    if( functionBar == NULL ) {
        return;
    }

    if( ImgedConfigInfo.grid_on ) {
        ToolBarSetState( functionBar, IMGED_GRID, BUTTON_DOWN );
    } else {
        ToolBarSetState( functionBar, IMGED_GRID, BUTTON_UP );
    }

} /* PressGridButton */

/*
 * GetFunctionBarHeight - get the height of the function bar
 */
int GetFunctionBarHeight( void )
{
    WPI_RECT    rect;

    if( functionBar == NULL ) {
        return( 0 );
    }

    _wpi_getwindowrect( hFunctionBar, &rect );
    return( _wpi_getheightrect( rect ) );

} /* GetFunctionBarHeight */

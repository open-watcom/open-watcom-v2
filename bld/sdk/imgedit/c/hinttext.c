/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "hinttext.h"

#ifndef __OS2_PM__              // ************** Windows part

static a_hint_text_line         hintText[MAX_HINT] = {
    { IMGED_NEW,                WIE_HINT_IMGED_NEW              },
    { IMGED_OPEN,               WIE_HINT_IMGED_OPEN             },
    { IMGED_SAVE,               WIE_HINT_IMGED_SAVE             },
    { IMGED_SAVE_AS,            WIE_HINT_IMGED_SAVE_AS          },
    { IMGED_EXIT,               WIE_HINT_IMGED_EXIT             },
    { IMGED_ABOUT,              WIE_HINT_IMGED_ABOUT            },
    { IMGED_HELP,               WIE_HINT_IMGED_HELP             },
    { IMGED_HELP_SEARCH,        WIE_HINT_IMGED_HELP_SEARCH      },
    { IMGED_HELP_ON_HELP,       WIE_HINT_IMGED_HELP_ON_HELP     },
    { IMGED_REST,               WIE_HINT_IMGED_REST             },
    { IMGED_LCOLOR,             WIE_HINT_IMGED_LCOLOR           },
    { IMGED_SCOLOR,             WIE_HINT_IMGED_SCOLOR           },
    { IMGED_RCOLOR,             WIE_HINT_IMGED_RCOLOR           },
    { IMGED_CLOSE,              WIE_HINT_IMGED_CLOSE            },
    { IMGED_ARRANGE,            WIE_HINT_IMGED_ARRANGE          },
    { IMGED_CLOSEALL,           WIE_HINT_IMGED_CLOSEALL         },
    { IMGED_COLOR,              WIE_HINT_IMGED_COLOR            },
    { IMGED_VIEW,               WIE_HINT_IMGED_VIEW             },
    { IMGED_TOOLBAR,            WIE_HINT_IMGED_TOOLBAR          },
    { IMGED_FREEHAND,           WIE_HINT_IMGED_FREEHAND         },
    { IMGED_LINE,               WIE_HINT_IMGED_LINE             },
    { IMGED_CIRCLEO,            WIE_HINT_IMGED_CIRCLEO          },
    { IMGED_CIRCLEF,            WIE_HINT_IMGED_CIRCLEF          },
    { IMGED_RECTO,              WIE_HINT_IMGED_RECTO            },
    { IMGED_RECTF,              WIE_HINT_IMGED_RECTF            },
    { IMGED_FILL,               WIE_HINT_IMGED_FILL             },
    { IMGED_BRUSH,              WIE_HINT_IMGED_BRUSH            },
    { IMGED_CLIP,               WIE_HINT_IMGED_CLIP             },
    { IMGED_HOTSPOT,            WIE_HINT_IMGED_HOTSPOT          },
    { IMGED_2x2,                WIE_HINT_IMGED_2x2              },
    { IMGED_3x3,                WIE_HINT_IMGED_3x3              },
    { IMGED_4x4,                WIE_HINT_IMGED_4x4              },
    { IMGED_5x5,                WIE_HINT_IMGED_5x5              },
    { IMGED_TILE,               WIE_HINT_IMGED_TILE             },
    { IMGED_NEWIMG,             WIE_HINT_IMGED_NEWIMG           },
    { IMGED_DELIMG,             WIE_HINT_IMGED_DELIMG           },
    { IMGED_CASCADE,            WIE_HINT_IMGED_CASCADE          },
    { IMGED_GRID,               WIE_HINT_IMGED_GRID             },
    { IMGED_CLEAR,              WIE_HINT_IMGED_CLEAR            },
    { IMGED_UNDO,               WIE_HINT_IMGED_UNDO             },
    { IMGED_REDO,               WIE_HINT_IMGED_REDO             },
    { IMGED_CUT,                WIE_HINT_IMGED_CUT              },
    { IMGED_COPY,               WIE_HINT_IMGED_COPY             },
    { IMGED_PASTE,              WIE_HINT_IMGED_PASTE            },
    { IMGED_SNAP,               WIE_HINT_IMGED_SNAP             },
    { IMGED_RIGHT,              WIE_HINT_IMGED_RIGHT            },
    { IMGED_LEFT,               WIE_HINT_IMGED_LEFT             },
    { IMGED_UP,                 WIE_HINT_IMGED_UP               },
    { IMGED_DOWN,               WIE_HINT_IMGED_DOWN             },
    { IMGED_SELIMG,             WIE_HINT_IMGED_SELIMG           },
    { IMGED_SQUARE,             WIE_HINT_IMGED_SQUARE           },
    { IMGED_SIZE,               WIE_HINT_IMGED_SIZE             },
    { IMGED_SETTINGS,           WIE_HINT_IMGED_SETTINGS         },
    { IMGED_FLIPHORZ,           WIE_HINT_IMGED_FLIPHORZ         },
    { IMGED_FLIPVERT,           WIE_HINT_IMGED_FLIPVERT         },
    { IMGED_ROTATECC,           WIE_HINT_IMGED_ROTATECC         },
    { IMGED_ROTATECL,           WIE_HINT_IMGED_ROTATECL         },
    { IMGED_CEDIT,              WIE_HINT_IMGED_CEDIT            },
    { IMGED_CRESET,             WIE_HINT_IMGED_CRESET           },
    { IMGED_CSCREEN,            WIE_HINT_IMGED_CSCREEN          },
    { IMGED_MAXIMIZE,           WIE_HINT_IMGED_MAXIMIZE         },
    { IMGED_DDE_UPDATE_PRJ,     WIE_HINT_IMGED_DDE_UPDATE_PRJ   },
    /* The following values are set in the InitMenus routine. */
    { -1,                       WIE_HINT_FILEMENU               },
    { -1,                       WIE_HINT_EDITMENU               },
    { -1,                       WIE_HINT_SHIFTMENU              },
    { -1,                       WIE_HINT_MIRRORMENU             },
    { -1,                       WIE_HINT_ROTATEMENU             },
    { -1,                       WIE_HINT_SETTINGSMENU           },
    { -1,                       WIE_HINT_BRUSHSIZEMENU          },
    { -1,                       WIE_HINT_PALETTEMENU            },
    { -1,                       WIE_HINT_DRAWTOOLSMENU          },
    { -1,                       WIE_HINT_ELLIPSEMENU            },
    { -1,                       WIE_HINT_RECTANGLEMENU          },
    { -1,                       WIE_HINT_HELPMENU               }
};

/*
 * ShowHintText - given a tool identifier, display the hint text that goes with it
 */
void ShowHintText( int id )
{
    int         i;

    for( i = 0; i < MAX_HINT; i++ ) {
        if( hintText[i].id <= 0 ) {
            break;
        } else if( hintText[i].id == id ) {
            PrintHintTextByID( hintText[i].hint, NULL );
            return;
        }
    }
    PrintHintTextByID( WIE_SELECTSIMAGETOEDIT, NULL );

} /* ShowHintText */

/*
 * InitMenus - initialize the popup menus so that hint text can be given for them
 */
void InitMenus( HMENU hmenu )
{
    HMENU       filemenu;
    HMENU       editmenu;
    HMENU       optionsmenu;
    HMENU       palettemenu;
    HMENU       toolsmenu;
    HMENU       windowsmenu;
    HMENU       helpmenu;

    filemenu = GetSubMenu( hmenu, 0 );
    editmenu = GetSubMenu( hmenu, 1 );
    optionsmenu = GetSubMenu( hmenu, 2 );
    palettemenu = GetSubMenu( hmenu, 3 );
    toolsmenu = GetSubMenu( hmenu, 4 );
    windowsmenu = GetSubMenu( hmenu, 5 );
    helpmenu = GetSubMenu( hmenu, 6 );
    if( ImgedIsDDE ) {
        // shift menu
        hintText[POPUP_START + 2].id = (int)(pointer_int)GetSubMenu( editmenu, 9 );
        // flip menu
        hintText[POPUP_START + 3].id = (int)(pointer_int)GetSubMenu( editmenu, 10 );
        // rotate menu
        hintText[POPUP_START + 4].id = (int)(pointer_int)GetSubMenu( editmenu, 11 );
    } else {
        // shift menu
        hintText[POPUP_START + 2].id = (int)(pointer_int)GetSubMenu( editmenu, 11 );
        // flip menu
        hintText[POPUP_START + 3].id = (int)(pointer_int)GetSubMenu( editmenu, 12 );
        // rotate menu
        hintText[POPUP_START + 4].id = (int)(pointer_int)GetSubMenu( editmenu, 13 );
    }

    // File menu option
    hintText[POPUP_START].id = (int)(pointer_int)filemenu;
    // Edit menu option
    hintText[POPUP_START + 1].id = (int)(pointer_int)editmenu;
    // options menu
    hintText[POPUP_START + 5].id = (int)(pointer_int)optionsmenu;
    // brush size menu
    hintText[POPUP_START + 6].id = (int)(pointer_int)GetSubMenu( optionsmenu, 6 );
    // palette menu
    hintText[POPUP_START + 7].id = (int)(pointer_int)palettemenu;
    // tools menu
    hintText[POPUP_START + 8].id = (int)(pointer_int)toolsmenu;
    // ellipse menu
    hintText[POPUP_START + 9].id = (int)(pointer_int)GetSubMenu( toolsmenu, 2 );
    // rectangle menu
    hintText[POPUP_START + 10].id = (int)(pointer_int)GetSubMenu( toolsmenu, 3 );
    // help menu
    hintText[POPUP_START + 11].id = (int)(pointer_int)helpmenu;

} /* InitMenus */

#else           // ************** PM part

static a_hint_text_line         hintText[MAX_HINT] = {
    // example      "1234567890123456789012345678901234567890"
    IMGED_NEW,      "Creates a new image",
    IMGED_OPEN,     "Opens an image",
    IMGED_SAVE,     "Saves current image",
    IMGED_SAVE_AS,  "Saves current image",
    IMGED_EXIT,     "Exits the image editor",
    IMGED_ABOUT,    "Describes the Open Watcom Image Editor",
    IMGED_REST,     "Restores image to opened form",
    IMGED_LCOLOR,   "Loads a color palette",
    IMGED_SCOLOR,   "Saves the current color palette",
    IMGED_RCOLOR,   "Restores color palette to default",
    IMGED_CLOSE,    "Closes the current image",
    IMGED_ARRANGE,  "Arranges the icons",
    IMGED_CLOSEALL, "Closes all images",
    IMGED_COLOR,    "Toggles display of the color palette",
    IMGED_VIEW,     "Toggles display of the view window",
    IMGED_TOOLBAR,  "Toggles display of the tool window",
    IMGED_FREEHAND, "Draw freehand with pencil",
    IMGED_LINE,     "Draw lines",
    IMGED_CIRCLEO,  "Draw outlined ellipses",
    IMGED_CIRCLEF,  "Draw filled ellipses",
    IMGED_RECTO,    "Draw outlined rectangles",
    IMGED_RECTF,    "Draw filled rectangles",
    IMGED_FILL,     "Fill area under the cursor",
    IMGED_BRUSH,    "Draw freehand with wide brush",
    IMGED_CLIP,     "Select region to edit",
    IMGED_HOTSPOT,  "Set hotspot location",
    IMGED_2x2,      "Set brush size to 2x2 pixels",
    IMGED_3x3,      "Set brush size to 3x3 pixels",
    IMGED_4x4,      "Set brush size to 4x4 pixels",
    IMGED_5x5,      "Set brush size to 5x5 pixels",
    IMGED_TILE,     "Tile image windows",
    IMGED_NEWIMG,   "Add an icon to current icon file",
    IMGED_DELIMG,   "Delete an icon from current icon file",
    IMGED_CASCADE,  "Cascade all the image windows",
    IMGED_GRID,     "Toggles the displaying of the grid",
    IMGED_CLEAR,    "Clear image or selected area",
    IMGED_UNDO,     "Undoes the previous operation",
    IMGED_REDO,     "Reperforms an undone operation",
    IMGED_CUT,      "Cuts selected area to clipboard",
    IMGED_COPY,     "Copies selected area to clipboard",
    IMGED_PASTE,    "Pastes into current image",
    IMGED_SNAP,     "Captures a bitmap from the desktop",
    IMGED_RIGHT,    "Shifts the image right",
    IMGED_LEFT,     "Shifts the image left",
    IMGED_UP,       "Shifts the image up",
    IMGED_DOWN,     "Shifts the image down",
    IMGED_SELIMG,   "Selects icon from a multiple icon file",
    IMGED_SQUARE,   "Maintains a square grid",
    IMGED_SIZE,     "Changes size of current image",
    IMGED_SETTINGS, "Displays the current settings",
    IMGED_FLIPHORZ, "Flips image over horizontal axis",
    IMGED_FLIPVERT, "Flips image over vertical axis",
    IMGED_ROTATECC, "Rotates image counterclockwise",
    IMGED_ROTATECL, "Rotates image clockwise",
    IMGED_CEDIT,    "Edits the current left color",
    IMGED_CRESET,   "Restores palette to unedited form",
    IMGED_CSCREEN,  "Sets color to represent screen/inverse",
    IMGED_MAXIMIZE, "Maximizes the current edit window",
    IMGED_FILE,     "File operations",
    IMGED_EDIT,     "Edit operations for current image",
    IMGED_SHIFT,    "Shifts the image",
    IMGED_FLIP,     "Creates a mirror image",
    IMGED_ROTATE,   "Rotates image 90 degrees",
    IMGED_OPTIONS,  "Settings for image editor session",
    IMGED_BS,       "Set the size for the paint brush",
    IMGED_PAL,      "Perform colour palette operations",
    IMGED_TOOLS,    "Selects tools with which to draw",
    IMGED_ELLIPSE,  "Ellipse drawing tool",
    IMGED_RECTANGLE,"Rectangle drawing tool",
    IMGED_WINDOWS,  "Perform Window operations",
    IMGED_HELP,     "Provides help for the Open Watcom Image Editor"
};

/*
 * ShowHintText - given a tool identifier, display the hint text that goes with it
 */
void ShowHintText( int id )
{
    int         i;

    for( i = 0; i < MAX_HINT; i++ ) {
        if( hintText[i].id <= 0 ) {
            break;
        } else if( hintText[i].id == id ) {
            SetHintText( hintText[i].text );
            return;
        }
    }

} /* ShowHintText */

#endif

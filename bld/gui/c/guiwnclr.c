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
* Description:  Defines basic color scheme for each window component.
*
****************************************************************************/


#include "guiwind.h"
#include <string.h>
#include "guiwnclr.h"
#include "guicolor.h"
#include "guix.h"

static gui_colour_set Default[GUI_NUM_ATTRS] =
{
#if defined (__NT__) && defined( GUI_IS_GUI )
    /* GUIEX_DLG_BKGRND will be GetSysColor(COLOR_BTNFACE)       */
    /* and so on and so forth. See InitSystemRGB() in guicolor.c */
    { GUI_BRIGHT_GREEN,     GUI_BRIGHT_BLUE   }, // GUI_MENU_PLAIN
    { GUI_BLUE,             GUI_BRIGHT_GREEN  }, // GUI_MENU_STANDOUT
    { GUI_GREY,             GUI_BRIGHT_BLUE   }, // GUI_MENU_GRAYED
    { GUIEX_HIGHLIGHTTEXT,  GUIEX_HIGHLIGHT   }, // GUI_MENU_ACTIVE    (selected in list)
    { GUI_BRIGHT_BLUE,      GUI_BRIGHT_GREEN  }, // GUI_MENU_ACTIVE_STANDOUT
    { GUI_BLACK,            GUIEX_DLG_BKGRND  }, // GUI_BACKGROUND
    { GUI_BLUE,             GUI_GREY          }, // GUI_MENU_FRAME
    { GUI_GREY,             GUI_WHITE         }, // GUI_TITLE_INACTIVE
    { GUI_BLUE,             GUI_GREY          }, // GUI_FRAME_ACTIVE,
    { GUI_BLACK,            GUI_GREY          }, // GUI_FRAME_INACTIVE
    { GUI_BLUE,             GUI_GREY          }, // GUI_ICON
    { GUI_BLUE,             GUI_GREY          }, // GUI_MENU_GRAYED_ACTIVE
    { GUI_BLACK,            GUI_WHITE         }, // GUI_FRAME_RESIZE
    { GUI_BLACK,            GUIEX_WND_BKGRND  }  // GUI_CONTROL_BACKGROUND
#else
    /*  fore/text           backgr. */
    { GUI_WHITE,            GUI_BLUE          }, // GUI_MENU_PLAIN
    { GUI_BRIGHT_WHITE,     GUI_BLUE          }, // GUI_MENU_STANDOUT
    { GUI_GREY,             GUI_BLUE          }, // GUI_MENU_GRAYED
    { GUI_WHITE,            GUI_BLACK         }, // GUI_MENU_ACTIVE
    { GUI_BRIGHT_WHITE,     GUI_BLACK         }, // GUI_MENU_ACTIVE_STANDOUT
    { GUI_BLUE,             GUI_WHITE         }, // GUI_BACKGROUND
    { GUI_BRIGHT_YELLOW,    GUI_BLUE          }, // GUI_MENU_FRAME
    { GUI_GREY,             GUI_WHITE         }, // GUI_TITLE_INACTIVE
    { GUI_BLUE,             GUI_CYAN          }, // GUI_FRAME_ACTIVE
    { GUI_GREY,             GUI_WHITE         }, // GUI_FRAME_INACTIVE
    { GUI_BRIGHT_WHITE,     GUI_RED           }, // GUI_ICON
    { GUI_GREY,             GUI_BLACK         }, // GUI_MENU_GRAYED_ACTIVE
    { GUI_GREY,             GUI_CYAN          }, // GUI_FRAME_RESIZE
    { GUI_BLUE,             GUI_WHITE         }  // GUI_CONTROL_BACKGROUND
#endif
};

bool GUISetColours( gui_window * wnd, int num_attrs, gui_colour_set * colours )
{
    if( num_attrs < GUI_NUM_ATTRS ) {
        num_attrs = GUI_NUM_ATTRS;
        colours = Default;
    }
    wnd->num_attrs = num_attrs;
    return( GUIXSetColours( wnd, colours ) );
}

void GUIFreeColours( gui_window * wnd )
{
    GUIMemFree( wnd->colours );
    wnd->num_attrs = 0;
    wnd->colours = NULL;
}

int GUIGetNumWindowColours( gui_window * wnd )
{
    return( wnd->num_attrs );
}

gui_colour_set * GUIGetWindowColours( gui_window * wnd )
{
    gui_colour_set * colours;

    colours = GUIMemAlloc( wnd->num_attrs * sizeof( gui_colour_set ) );
    if( colours != NULL ) {
        GUIXGetWindowColours( wnd, colours );
    }
    return( colours );
}

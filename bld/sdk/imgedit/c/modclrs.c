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
#include <commdlg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "imgedit.h"
#include "ieclrpal.h"
#include "ieprofil.h"

static BOOL     fRestEnabled = FALSE;
static DWORD    customcolours[16] = {
#if 1
                RGB(255, 255, 255), RGB(239, 239, 239),
                RGB(223, 223, 223), RGB(207, 207, 207),
                RGB(191, 191, 191), RGB(175, 175, 175),
                RGB(159, 159, 159), RGB(143, 143, 143),
                RGB(127, 127, 127), RGB(111, 111, 111),
                RGB(95, 95, 95),    RGB(79, 79, 79),
                RGB(63, 63, 63),    RGB(47, 47, 47),
                RGB(31, 31, 31),    RGB(15, 15, 15) };
#else
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255),
                RGB(255, 255, 255), RGB(255, 255, 255) };
#endif

/*
 * EditColours - edits the current colour selection.
 */
void EditColours( void )
{
    static CHOOSECOLOR          choosecolour;
    BOOL                        setcolour = FALSE;
    wie_clrtype                 type;
    COLORREF                    current_colour;
    HMENU                       hmenu;
    HWND                        parent;

    if( ImgedConfigInfo.show_state & SET_SHOW_CLR ) {
        parent = HColourPalette;
    } else {
        parent = HMainWindow;
    }

    current_colour = GetSelectedColour( LMOUSEBUTTON, NULL, &type );

    choosecolour.lStructSize = sizeof(CHOOSECOLOR);
    choosecolour.hwndOwner = parent;
    choosecolour.hInstance = NULL;
    choosecolour.rgbResult = current_colour;
    choosecolour.lpCustColors = (LPDWORD)customcolours;
    choosecolour.Flags = CC_RGBINIT | CC_FULLOPEN;
    choosecolour.lCustData = 0l;
    choosecolour.lpfnHook = 0;
    choosecolour.lpTemplateName = (LPSTR)NULL;

    setcolour = ChooseColor( &choosecolour );

    if (!setcolour) {
        return;
    }

    ReplacePaletteEntry( choosecolour.rgbResult );
    fRestEnabled = TRUE;
    hmenu = GetMenu( HMainWindow );
    EnableMenuItem( hmenu, IMGED_CRESET, MF_ENABLED );

    PrintHintTextByID( WIE_COLOURPALETTEMODIFIED, NULL );
} /* EditColours */

/*
 * RestoreColours - restore the original colour palette.
 */
void RestoreColours( void )
{
    ResetColourPalette();
    PrintHintTextByID( WIE_COLOURPALETTERESTORED, NULL );
} /* RestoreColours */

/*
 * SetColourMenus - enables/disables those items that need it in the colour
 *                  palette menu.
 */
void SetColourMenus( img_node *node )
{
    HMENU       hmenu;

    hmenu = GetMenu( HMainWindow );
    if (node->imgtype == BITMAP_IMG) {
        EnableMenuItem( hmenu, IMGED_CSCREEN, MF_GRAYED );
    } else {
        EnableMenuItem( hmenu, IMGED_CSCREEN, MF_ENABLED );
    }

    if (node->bitcount > 1) {
        EnableMenuItem( hmenu, IMGED_CEDIT, MF_ENABLED );
        if (fRestEnabled) {
            EnableMenuItem( hmenu, IMGED_CRESET, MF_ENABLED );
        } else {
            EnableMenuItem( hmenu, IMGED_CRESET, MF_GRAYED );
        }
    } else {
        EnableMenuItem( hmenu, IMGED_CEDIT, MF_GRAYED );
        EnableMenuItem( hmenu, IMGED_CRESET, MF_GRAYED );
    }
} /* SetColourMenus */


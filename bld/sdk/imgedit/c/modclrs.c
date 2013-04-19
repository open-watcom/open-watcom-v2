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
#include <commdlg.h>
#include "ieclrpal.h"
#include "ieprofil.h"

static BOOL     fRestEnabled = FALSE;
static DWORD    customcolors[16] = {
#if 1
                RGB( 255, 255, 255 ), RGB( 239, 239, 239 ),
                RGB( 223, 223, 223 ), RGB( 207, 207, 207 ),
                RGB( 191, 191, 191 ), RGB( 175, 175, 175 ),
                RGB( 159, 159, 159 ), RGB( 143, 143, 143 ),
                RGB( 127, 127, 127 ), RGB( 111, 111, 111 ),
                RGB( 95, 95, 95 ),    RGB( 79, 79, 79 ),
                RGB( 63, 63, 63 ),    RGB( 47, 47, 47 ),
                RGB( 31, 31, 31 ),    RGB( 15, 15, 15 )
#else
                RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
                RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
                RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
                RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
                RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
                RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
                RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
                RGB( 255, 255, 255 ), RGB( 255, 255, 255 )
#endif
};

/*
 * EditColors - edit the current color selection
 */
void EditColors( void )
{
    static CHOOSECOLOR          choosecolor;
    BOOL                        setcolor = FALSE;
    wie_clrtype                 type;
    COLORREF                    current_color;
    HMENU                       hmenu;
    HWND                        parent;

    if( ImgedConfigInfo.show_state & SET_SHOW_CLR ) {
        parent = HColorPalette;
    } else {
        parent = HMainWindow;
    }

    current_color = GetSelectedColor( LMOUSEBUTTON, NULL, &type );

    choosecolor.lStructSize = sizeof( CHOOSECOLOR );
    choosecolor.hwndOwner = parent;
    choosecolor.hInstance = NULL;
    choosecolor.rgbResult = current_color;
    choosecolor.lpCustColors = (LPDWORD)customcolors;
    choosecolor.Flags = CC_RGBINIT | CC_FULLOPEN;
    choosecolor.lCustData = 0l;
    choosecolor.lpfnHook = 0;
    choosecolor.lpTemplateName = (LPSTR)NULL;

    setcolor = ChooseColor( &choosecolor );

    if( !setcolor ) {
        return;
    }

    ReplacePaletteEntry( choosecolor.rgbResult );
    fRestEnabled = TRUE;
    hmenu = GetMenu( HMainWindow );
    EnableMenuItem( hmenu, IMGED_CRESET, MF_ENABLED );

    PrintHintTextByID( WIE_COLORPALETTEMODIFIED, NULL );

} /* EditColors */

/*
 * RestoreColors - restore the original color palette
 */
void RestoreColors( void )
{
    ResetColorPalette();
    PrintHintTextByID( WIE_COLORPALETTERESTORED, NULL );

} /* RestoreColors */

/*
 * SetColorMenus - enables/disables those items that need it in the color palette menu
 */
void SetColorMenus( img_node *node )
{
    HMENU       hmenu;

    hmenu = GetMenu( HMainWindow );
    if( node->imgtype == BITMAP_IMG ) {
        EnableMenuItem( hmenu, IMGED_CSCREEN, MF_GRAYED );
    } else {
        EnableMenuItem( hmenu, IMGED_CSCREEN, MF_ENABLED );
    }

    if( node->bitcount > 1 ) {
        EnableMenuItem( hmenu, IMGED_CEDIT, MF_ENABLED );
        if( fRestEnabled ) {
            EnableMenuItem( hmenu, IMGED_CRESET, MF_ENABLED );
        } else {
            EnableMenuItem( hmenu, IMGED_CRESET, MF_GRAYED );
        }
    } else {
        EnableMenuItem( hmenu, IMGED_CEDIT, MF_GRAYED );
        EnableMenuItem( hmenu, IMGED_CRESET, MF_GRAYED );
    }

} /* SetColorMenus */

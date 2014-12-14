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


#include "guiwind.h"
#include "guiicon.h"
#include "uiattrs.h"
#include <string.h>

void GUIDrawIcon( gui_window * wnd )
{
    SAREA       area;
    int         length;
    ATTR        attr;
    char        *str;

    COPYAREA( wnd->screen.area, area );
    area.row = 0;
    area.col = 0;
    attr = wnd->colours[GUI_ICON];
    uivfill( &wnd->screen, area, attr, ' ' );
    uidrawbox( &wnd->screen, &area, attr, NULL );
    if( wnd->icon_name != NULL ) {
        str = wnd->icon_name;
    } else {
        str = wnd->screen.title;
    }
    if( str != NULL ) {
        length = strlen( str );
        if( length > ( area.width - 2 ) ) {
            length = area.width - 2;
        }
        uivtextput( &wnd->screen, ( area.height / 2 ), 1, attr, str, length );
    }
}

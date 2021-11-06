/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "guiutil.h"
#include "guihot.h"
#include <string.h>


bool GUIXInitHotSpots( int num, gui_resource *hot )
{
    int i;

    memset( GUIHotSpots, 0, sizeof( hotspot_info ) * num );
    for( i = 0; i < num; i++ ) {
        if( hot[i].chars != NULL ) {
            bool    ok;

            GUIHotSpots[i].text = GUIStrDup( hot[i].chars, &ok );
            if( !ok )
                return( false );
            GUIHotSpots[i].size.x = strlen( GUIHotSpots[i].text );
            GUIHotSpots[i].size.y = 1;
        } else {
            GUIHotSpots[i].text = NULL;
            GUIHotSpots[i].size.x = 0;
            GUIHotSpots[i].size.y = 0;
        }
    }
    return( true );
}


void GUIAPI GUIDrawHotSpot( gui_window *wnd, int hotspot_no, gui_text_ord row,
                     gui_ord indent, gui_attr attr )
{
    if( ( hotspot_no > 0 ) && ( hotspot_no <= GUINumHotSpots ) ) {
        GUIDrawText( wnd, GUIHotSpots[hotspot_no - 1].text,
                     GUIHotSpots[hotspot_no - 1].size.x, row, indent, attr );
    }
}

void GUIXCleanupHotSpots( void )
{
    int i;

    for( i = 0; i < GUINumHotSpots; i++ ) {
        GUIMemFree( GUIHotSpots[i].text );
    }
}

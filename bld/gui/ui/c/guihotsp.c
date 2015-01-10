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
#include "guicontr.h"
#include "guihotsp.h"
#include "guicolor.h"
#include "guiutil.h"
#include "guixutil.h"
#include "guixdlg.h"
#include <string.h>

/*
 * GUICreateHot -- called by GUIDoAddControl
 */

bool GUICreateHot( gui_control_info *ctl_info, VFIELD *field )
{
    a_hot_spot  *hot_spot;
    bool        ok;

    if( field == NULL ) {
        return( false );
    }
    hot_spot = (a_hot_spot *)GUIMemAlloc( sizeof( a_hot_spot ) );
    field->u.hs = hot_spot;
    if( hot_spot == NULL ) {
        return( false );
    }
    field->typ = FLD_HOT;
    hot_spot->str = GUIStrDup( ctl_info->text, &ok );
    if( !ok ) {
        return( false );
    }
    hot_spot->event = ctl_info->id + GUI_FIRST_USER_EVENT;
    hot_spot->row = field->area.row;
    hot_spot->startcol = field->area.col;
    hot_spot->length = field->area.width;
    if( ctl_info->control_class == GUI_DEFPUSH_BUTTON ) {
        hot_spot->flags = HOT_DEFAULT;
    } else {
        hot_spot->flags = 0;
    }
    return( true );
}

/*
 * GUIFreeHotSpot -- free an a_hot_spot structure
 */

void GUIFreeHotSpot( a_hot_spot *hot_spot )
{
    if( hot_spot != NULL ) {
        GUIMemFree( hot_spot->str );
    }
    GUIMemFree( hot_spot );
}

bool GUISetHotSpotText( a_hot_spot *hot_spot, const char *text )
{
    char        *new_str;
    bool        ok;

    if( text == NULL )
        text = "";
    new_str = GUIStrDup( text, &ok );
    if( ok ) {
        GUIMemFree( hot_spot->str );
        hot_spot->str = new_str;
    }
    return( ok );
}

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

bool GUICreateHot( gui_control_info *info, a_hot_spot_field *hot_spot_field )
{
    a_hot_spot  *hot_spot;

    if( hot_spot_field == NULL ) {
        return( FALSE );
    }
    hot_spot = (a_hot_spot *)GUIMemAlloc( sizeof( a_hot_spot ) );
    hot_spot_field->ptr = hot_spot;
    if( hot_spot == NULL ) {
        return( FALSE );
    }
    hot_spot_field->typ = FLD_HOT;
    if( !GUIStrDup( info->text, &hot_spot->str ) ) {
        return( FALSE );
    }
    hot_spot->event = info->id + GUI_FIRST_USER_EVENT;
    hot_spot->row = hot_spot_field->area.row;
    hot_spot->startcol = hot_spot_field->area.col;
    hot_spot->length = hot_spot_field->area.width;
    if( info->control_class == GUI_DEFPUSH_BUTTON ) {
        hot_spot->flags = HOT_DEFAULT;
    } else {
        hot_spot->flags = (unsigned short)NULL;
   }
   return( TRUE );
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
    char        *new;

    if( text == NULL ) {
        if( !GUIStrDup( "", &new ) ) {
            return( FALSE );
        }
    } else {
        if( !GUIStrDup( text, &new) ) {
            return( FALSE );
        }
    }
    GUIMemFree( hot_spot->str );
    hot_spot->str = new;
    return( TRUE );
}

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


#include <stdio.h>
#include <string.h>
#include "vi.h"
#include "keys.h"

/*
 * DoDeleteRegion - delete currently selected region
 */
int DoDeleteRegion( event **ev, event **next )
{
    #ifdef __WIN__
        SavebufNumber = CLIPBOARD_SAVEBUF;
    #endif
    *ev = &EventList[ 'd' ];
    *next = &EventList[ 'r' ];
    LastEvent = 'r';
    return( ERR_NO_ERR );

} /* DoDeleteRegion */

/*
 * DoDeleteLineEnd - delete to end of line
 */
int DoDeleteLineEnd( event **ev, event **next )
{
    *ev = &EventList[ 'd' ];
    *next = &EventList[ '$' ];
    LastEvent = '$';
    return( ERR_NO_ERR );

} /* DoDeleteLineEnd */

/*
 * DoDeleteCharAtCursor - delete character at cursor (DEL, x)
 *                        (or range if one exists)
 */
int DoDeleteCharAtCursor( event **ev, event **next )
{
    *ev = &EventList[ 'd' ];
    if( SelRgn.selected && EditFlags.Modeless ) {
        *next = &EventList[ 'r' ];
        LastEvent = 'r';
    } else {
        *next = &EventList[ 'l'];
        LastEvent = 'l';
    }
    return( ERR_NO_ERR );

} /* DoDeleteCharAtCursor */

/*
 * DoDeleteCharBeforeCursor - delete character before cursor (DEL, x)
 *                            (or range if modeless & one exists)
 */
int DoDeleteCharBeforeCursor( event **ev, event **next )
{
    *ev = &EventList[ 'd' ];
    if( SelRgn.selected && EditFlags.Modeless ) {
        *next = &EventList[ 'r' ];
    } else {
        *next = &EventList[ 'h'];
    }
    LastEvent = ' ';
    return( ERR_NO_ERR );

} /* DoDeleteCharBeforeCursor */

/*
 * DeleteSelectedRegion - delete selected region
 */
int DeleteSelectedRegion( void )
{
    range r;

    if( SelRgn.selected ) {
        GetSelectedRegion( &r );
        NormalizeRange( &r );
        return( Delete( &r ) );
    }
    return( ERR_NO_ERR );
}

/*
 * YankSelectedRegion - yank selected region
 */
int YankSelectedRegion( void )
{
    range r;

    if( SelRgn.selected ) {
        GetSelectedRegion( &r );
        NormalizeRange( &r );
        return( Yank( &r ) );
    }
    return( ERR_NO_ERR );
}


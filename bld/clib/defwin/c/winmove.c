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


#include "variety.h"
#include "win.h"

/*
 * _MovePageUp - move up a page
 */
void _MovePageUp( LPWDATA w )
{

    if( w->TopLineNumber == 1 )  return;
    w->CurrentLineNumber -= (w->height-1);
    w->TopLineNumber -= (w->height-1);
    if( (signed long) w->CurrentLineNumber < 1 ) {
        w->CurrentLineNumber = 1;
    }
    if( (signed long) w->TopLineNumber < 1 ) {
        w->TopLineNumber = 1;
    }
    _PositionScrollThumb( w );
    _DisplayAllLines( w, TRUE );

} /* _MovePageUp */

/*
 * _MovePageDown - move down a page
 */
void _MovePageDown( LPWDATA w )
{
    DWORD       ll;

    ll = _GetLastLineNumber( w );
    w->CurrentLineNumber += (w->height-1);
    w->TopLineNumber += (w->height-1);
    if( w->CurrentLineNumber > ll ) {
        w->CurrentLineNumber = ll;
        w->TopLineNumber = w->CurrentLineNumber - (w->height/2);
        if( (signed long) w->TopLineNumber < 1 ) w->TopLineNumber = 1;
    }
    _PositionScrollThumb( w );
    _DisplayAllLines( w, TRUE );

} /* _MovePageDown */

/*
 * _MoveLineUp - move up one line
 */
void _MoveLineUp( LPWDATA w )
{
    LPLDATA     lp;

    if( w->TopLineNumber != 1 ) {
        w->CurrentLineNumber--;
        w->TopLineNumber--;
        _ShiftWindow( w, -1 );
        lp = _GetLineDataPointer( w, w->TopLineNumber );
        _DisplayLineInWindow( w, 1, lp->data );
        _PositionScrollThumb( w );
    }

} /* _MoveLineUp */

/*
 * _MoveLineDown - move down one line
 */
void _MoveLineDown( LPWDATA w )
{
    LPLDATA     lp;
    DWORD       bot;
    DWORD       ll;

    ll = _GetLastLineNumber( w );
    if( w->CurrentLineNumber != ll ) {
        bot = w->TopLineNumber + w->height-1;
        if( bot < ll ) {
            _ShiftWindow( w, 1 );
            lp = _GetLineDataPointer( w, bot );
            if( lp != NULL ) {
                _DisplayLineInWindow( w, w->height-1, lp->data );
            }
            w->CurrentLineNumber++;
            w->TopLineNumber++;
            _PositionScrollThumb( w );
        }
    }

} /* _MoveLineDown */

/*
 * _MoveToLine - move to a line, given a thumb position
 */
void _MoveToLine( LPWDATA w, DWORD line, BOOL center )
{
    int         height;
    DWORD       bottom;
    DWORD       ll;

    height = w->height-2;
    bottom = w->TopLineNumber + height;
    ll = _GetLastLineNumber( w );
    if( (line >= w->TopLineNumber) && (line <= bottom) && !center ) return;
    if( center || bottom < ll ) {
        w->TopLineNumber = line - height/2;
    } else {
        if( bottom > ll ) bottom = ll;
        if( bottom > line ) {
            w->TopLineNumber = line - height;
        }
    }
    if( (signed long) w->TopLineNumber < 1L ) w->TopLineNumber = 1;
    w->CurrentLineNumber = line;
    _PositionScrollThumb( w );
    _DisplayAllLines( w, TRUE );

} /* _MoveToLine */

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


#include <wrect.hpp>
#include <wdialog.hpp>
#include <wname.hpp>
#include <wpshbttn.hpp>
#include <wlistbox.hpp>

#include "twolist.h"

void TwoList::listBoxChanged( WWindow * win )
//-------------------------------------------
{
    if( win == leftBox() ) {
        rightBox()->select( -1 );
    } else if( win == rightBox() ) {
        leftBox()->select( -1 );
    }
}

void TwoList::toRightPressed( WWindow * )
//---------------------------------------
{
    WString s;
    int     index = leftBox()->selected();

    if( index < 0 ) return;

    leftBox()->getString( index, s );

    /*
     * Add string to right list box, make sure it is visible to the user,
     * and make sure nothing is selected in the right list box.
     */
    rightBox()->insertString( s );
    rightBox()->setTopIndex( rightBox()->count() - 1 );
    rightBox()->select( -1 );

    moveElement( LeftList, index );
    leftBox()->deleteString( index );
    if( index >= leftBox()->count() ) {
        leftBox()->select( leftBox()->count() - 1 );
    } else {
        leftBox()->select( index );
    }
}

void TwoList::toLeftPressed( WWindow * )
//--------------------------------------
{
    WString s;
    int     index = rightBox()->selected();

    if( index < 0 ) return;

    rightBox()->getString( index, s );

    /*
     * Add string to left list box, make sure it is visible to the user,
     * and make sure nothing is selected in the left list box.
     */
    leftBox()->insertString( s );
    leftBox()->setTopIndex( leftBox()->count() - 1 );
    leftBox()->select( -1 );

    moveElement( RightList, index );
    rightBox()->deleteString( index );
    if( index >= rightBox()->count() ) {
        rightBox()->select( rightBox()->count() - 1 );
    } else {
        rightBox()->select( index );
    }
}

void TwoList::allToRightPressed( WWindow * )
//------------------------------------------
{
    moveAll( LeftList );
    fillBoxes();
}

void TwoList::allToLeftPressed( WWindow * )
//-----------------------------------------
{
    moveAll( RightList );
    fillBoxes();
}


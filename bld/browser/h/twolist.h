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


/* implements a dialog box that has two list boxes, allowing you to
 * remove elements from one box and add them to the other.  This is
 * an abstract base class with pure-virtual methods to access elements of
 * the lists.  the list boxes should be made muliple-selection when possible
 */

#ifndef __TWOLIST_H__

class WListBox;
class WPushButton;

class TwoList
{
public:
                        TwoList() {};
    virtual             ~TwoList() {};

protected:

    enum WhichList { LeftList, RightList };

    virtual void        moveElement( WhichList fromList, int index ) = 0;
    virtual void        moveAll( WhichList fromList ) = 0;
    virtual void        fillBoxes( void ) = 0;

    virtual WListBox *  leftBox() = 0;
    virtual WListBox *  rightBox() = 0;

            void        toRightPressed( WWindow * );
            void        listBoxChanged( WWindow * );
            void        toLeftPressed( WWindow * );
            void        allToRightPressed( WWindow * );
            void        allToLeftPressed( WWindow * );
};

#define __TWOLIST_H__
#endif

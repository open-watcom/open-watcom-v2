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


#ifndef __GTROOTSL_H__

#include <wdialog.hpp>      // this should be in dgedproj.gh

#include "twolist.h"
#include "dggtroot.gh"
#include "gtbase.h"

class WPushButton;
class WDefPushButton;
class WName;
class WListBox;

class TreeRootSelect : public GTRootSelectDlg, public TwoList, public WDialog
{
public:
                        TreeRootSelect( WWindow * prt, int x, int y,
                                        TreeRootList * visible,
                                        TreeRootList * hidden );
                        ~TreeRootSelect() {};

protected:

    virtual void        moveElement( WhichList fromList, int index );
    virtual void        moveAll( WhichList fromList );
    virtual void        fillBoxes( void );
    virtual void        initialize( void );

            void        okButton( WWindow * ) { quit( 1 ); }
            void        cancelButton( WWindow* ) { quit( 0 ); }

            void        help( WWindow * );

            void        disablePressed( WWindow * w ) { TwoList::toRightPressed( w ); }
            void        enablePressed( WWindow * w ) { TwoList::toLeftPressed( w ); }
            void        disableAllPressed( WWindow * w ) { TwoList::allToRightPressed( w ); }
            void        enableAllPressed( WWindow * w ) { TwoList::allToLeftPressed( w ); }

    virtual WListBox *  leftBox() { return _leftBox; }
    virtual WListBox *  rightBox() { return _rightBox; }

private:

            TreeRootList *      _visible;
            TreeRootList *      _hidden;

            /*-------------- controls ------------- */

            WName *             _leftText;
            WName *             _rightText;
            WPushButton *       _toRightButton;
            WPushButton *       _allRightButton;
            WPushButton *       _toLeftButton;
            WPushButton *       _allLeftButton;
            WDefPushButton *    _okButton;
            WPushButton *       _cancelButton;
            WPushButton *       _helpButton;
            WListBox *          _leftBox;
            WListBox *          _rightBox;
};

#define __GTROOTSL_H__
#endif

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


#ifndef wtoolbar_class
#define wtoolbar_class

#include "_windows.hpp"
#include "wobject.hpp"
#include "wvlist.hpp"
#include "wrect.hpp"

WCLASS  WToolBarItem;
WCLASS  WToolBar;
WCLASS  WWindow;

// Tool Bar states:

typedef unsigned WToolBarState;

#define WToolBarClosed          0
#define WToolBarFloating        1
#define WToolBarFixed           2

#define WToolFlagsFixed         1
#define WToolFlagsUseRect       2
#define WToolFlagsUseTips       4

typedef bool (WObject::*cbtb)( WToolBar *, WToolBarState );

WCLASS WToolBar : public WObject {
    public:
        WEXPORT WToolBar( bool fixed );
        WEXPORT WToolBar( bool fixed, bool use_tips );
        WEXPORT WToolBar( WRect );
        WEXPORT ~WToolBar();

        void WEXPORT addTool( WToolBarItem * );
        void WEXPORT removeTool( WToolBarItem * );
        void WEXPORT attach( WWindow * );
        void WEXPORT detach();

        void WEXPORT onChanged( WObject *, cbtb );
        bool WEXPORT changed( WToolBarState );

    private:
        WWindow                 *_parent;
        WVList                  _children;
        unsigned                _flags;
        gui_toolbar_struct      *_toolBarItems;
        WObject                 *_changedClient;
        cbtb                    _changed;
        WRect                   _rect;
};

#endif

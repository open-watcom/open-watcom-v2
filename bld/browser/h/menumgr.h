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


#ifndef __MENUMGR_H__
#define __MENUMGR_H__

#include <wobject.hpp>

#include "wbrdefs.h"
#include "view.h"

class WWindow;
class WPopupMenu;
class WMenuItem;
class Browse;
class MenuManager;
class SubMenuInfo;
class ViewManager;

template <class Key, class Type> class WCValHashDict;

typedef unsigned MIMainMenu;
typedef unsigned MISubMenu;

class MIMenuID {
public:
                MIMenuID();

                MIMenuID( const MIMenuID & o )
                    : _main( o._main ), _sub( o._sub ) {}

                MIMenuID( MIMainMenu mn, MISubMenu sub )
                    : _main( mn ), _sub( sub ) {}

    bool        operator == ( const MIMenuID & o ) const;

    MIMainMenu  mainID() const { return _main; }
    MISubMenu   subID() const { return _sub; }

private:
    MIMainMenu  _main;
    MISubMenu   _sub;
};

class MenuHandler {
public:
    virtual void            menuSelected( const MIMenuID & id ) = 0;
};


class MenuManager : public ViewEventReceiver, public WObject {
public:
                            MenuManager();
                            ~MenuManager();

            void            enableMenu( const MIMenuID & id, bool = TRUE );
            bool            menuEnabled( const MIMenuID & id );
            void            checkMenu( const MIMenuID & id, bool );
            void            registerForMenu( MenuHandler *, const MIMenuID &, bool enable = TRUE );
            void            unRegister( const MIMenuID & );
            void            setupMenus( Browse * client );

            //---- from ViewEventReceiver -----
    virtual void            event( ViewEvent ve, View * view );
    virtual ViewEvent       wantEvents();

            void            registerForViewEvents( ViewManager * );

            void            trackPopup( WWindow * win, MIMainMenu pop );

private:
            void            menuPopup( WPopupMenu * );
            void            menuSelected( WMenuItem * );
            void            hintText( WMenuItem *, const char * hint );
            void            makeItem( WPopupMenu * pop, const SubMenuInfo * info,
                                      int idx, bool disable=TRUE );

            WPopupMenu **                               _topMenus;
            Browse *                                    _clientWin;
            WCValHashDict<MIMenuID, MenuHandler *> *    _receivers;
};

#endif // __MENUMGR_H__

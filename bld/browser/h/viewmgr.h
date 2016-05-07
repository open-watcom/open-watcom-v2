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


#ifndef __VIEWMGR_H__
#define __VIEWMGR_H__

#include "wbrdefs.h"
#include "view.h"
#include "dtview.h"
#include "gbview.h"
#include "menumgr.h"

template <class Type> class WCPtrOrderedVector;
template <class Type> class WCValOrderedVector;

class ViewManager : public MenuHandler {
public:
                            ViewManager();
                            ~ViewManager();

            void            registerForEvents( ViewEventReceiver * );
            void            unregister( ViewEventReceiver * );

            void            setMenus( MenuManager * );
            void            unsetMenus();
    virtual void            menuSelected( const MIMenuID & );

            void            enableMenus( bool enabled );

            DetailView *    findDetailView( const Symbol * sym, DetailView::ViewType );
            DetailView *    findDetailView( drmem_hdl, DetailView::ViewType );

            void            showDetailView( const Symbol * sym, DetailView::ViewType );
            void            showGlobalView( GlobalView::GlobalViewType );

            void            viewDying( View * );
            void            eventOccured( ViewEvent, View * );
            void            kill();

private:

            void            sendEvent( ViewEvent, View * );

            class EventReceiver {
                friend class ViewManager;

            public:
                                    EventReceiver();
                                    EventReceiver( const EventReceiver & o );
                                    EventReceiver( ViewEventReceiver * );

                bool                operator == ( const EventReceiver & o ) const;
                EventReceiver &     operator = ( const EventReceiver & o );

            private:
                ViewEventReceiver * _receiver;
                ViewEvent           _wantsEvents;
            };

            View *                                  _hasFocus;
            MenuManager *                           _menuManager;
            WCPtrOrderedVector<DetailView> *        _detailViews;
            WCValOrderedVector<EventReceiver> *     _eventReceivers;
            WCValOrderedVector<View *> *            _killList;
};

#endif // __VIEWMGR_H__

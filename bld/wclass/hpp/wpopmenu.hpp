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


#ifndef wpopupmenu_class
#define wpopupmenu_class

#include "wmenu.hpp"
#include "wstring.hpp"

WCLASS WWindow;

WCLASS WPopupMenu;
typedef void (WObject::*cbp)( WPopupMenu * );

WCLASS WPopupMenu : public WMenu {
    public:
        WEXPORT WPopupMenu( const char *text );
        WEXPORT ~WPopupMenu();
        void WEXPORT setMdiPopup( void );
        virtual void WEXPORT popup( void );
        virtual void WEXPORT hilighted( bool );
        void WEXPORT insertSeparator( int index=-1 );
        WMenuItem* WEXPORT insertItem( WMenuItem* item, int index=-1 );
        WMenuItem* WEXPORT removeItem( WMenuItem* item );
        WMenuItem* WEXPORT removeItemAt( int index );
        virtual void WEXPORT track( WWindow* w );
        virtual void WEXPORT onPopup( WObject* client, cbp popup );

        virtual void attachMenu( WWindow *, int );
        virtual void detachMenu();
        void attachItem( WWindow *, int );
        void attachChildren( WWindow * );

    private:
        WObject*        _client;
        cbp             _popup;
        bool            _isMdiPopup;
};

#endif

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


#ifndef wmenuobj_class
#define wmenuobj_class

#include "_windows.hpp"

#include "wobject.hpp"
#include "wstring.hpp"

#define MENU_SEPARATOR  0x0001
#define MENU_ITEM       0x0002
#define MENU_MAIN       0x0004
#define MENU_POPUP      0x0008

#define WMAIN_MENU_ID   (0)

WCLASS WWindow;
WCLASS WPopupMenu;

WCLASS WMenuObject : public WObject {
    public:
        WEXPORT WMenuObject();
        WEXPORT ~WMenuObject();

        virtual void attachMenu( WWindow *, int ) {};
        virtual void detachMenu() {};

        void setEnable( bool enable ) { _enabled = enable; }
        unsigned enabled() { return( _enabled ); }

        void setCheck( bool check ) { _checked = check; }
        unsigned checked() { return( _checked ); }

        void setMenuId( WControlId id ) { _id = id; }
        WControlId menuId() { return( _id ); }

        void setText( const char *text ) { _text = text; }
        const char *text() { return( _text ); }

        void setOwner( WWindow* owner ) { _owner = owner; }
        WWindow *owner( void ) { return( _owner ); }

    private:
        WControlId      _id;
        WWindow         *_owner;
        bool            _enabled;
        bool            _checked;
        WString         _text;
};

#endif

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


#ifndef wmenu_class
#define wmenu_class

#include "_windows.hpp"

#include "wmenuobj.hpp"
#include "wvlist.hpp"

WCLASS WWindow;
WCLASS WMenuItem;
WCLASS WPopupMenu;
WCLASS WMenu : public WMenuObject {
    public:
        WEXPORT WMenu( bool create=true );
        WEXPORT ~WMenu();

        WPopupMenu * WEXPORT insertPopup( WPopupMenu *popup, int index=-1 );
        WPopupMenu * WEXPORT removePopup( WPopupMenu *popup );
        WPopupMenu * WEXPORT removePopupAt( int index );
        void setFloatingPopup( bool fpopup ) { _isFloatingPopup = fpopup; }
        bool isFloatingPopup( void );
        bool isFloatingMain() { return( _isFloatingPopup ); }
        void WEXPORT enableItem( bool enable, int index );
        bool WEXPORT itemEnabled( int index );
        void WEXPORT checkItem( bool check, int index );
        void WEXPORT setItemText( const char *text, int index );
        int WEXPORT childCount( void ) { return( _children.count() ); }
        void setParent( WMenu *parent ) { _parent = parent; }
        WMenu * WEXPORT parent( void ) { return( _parent ); }
        virtual void attachMenu( WWindow *, int );
        virtual void detachMenu();

    protected:
        WMenu           *_parent;
        WVList          _children;
        bool            _isFloatingPopup;
};

#endif

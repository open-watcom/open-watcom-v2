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


#ifndef winflist_class
#define winflist_class

#include "wwindow.hpp"
#include "wvlist.hpp"

typedef char const * (WObject::*ccbo)( WObject * );
typedef WObject *    (WObject::*ocbloi)( WVList *, WObject *, int );

class WInfiniteList : public WWindow
{
    public:
        WEXPORT WInfiniteList( WWindow* parent, const WRect& r, char * text,
                               WObject * client, ocbloi fillcb, ccbo namecb,
                               uint loadSize=100,
                               WStyle wstyle= WStyleHScrollAll
                                            | WStyleVScroll
                                            | WStyleVRows
                                            | WStyleVScrollEvents );

        WEXPORT ~WInfiniteList();

        /* accessible to listbox user */
        void WEXPORT        onChanged( WObject* obj, cbw changed );
        void WEXPORT        onDblClick( WObject* obj, cbw dblClick );
        WObject * WEXPORT   selected( void );
        int WEXPORT         count();
        void WEXPORT        reset();

        virtual bool WEXPORT gettingFocus( WWindow* );
        virtual bool WEXPORT paint();
        virtual bool WEXPORT mouseMove( int x, int y, WMouseKeyFlags );
        virtual bool WEXPORT leftBttnDn( int x, int y, WMouseKeyFlags );
        virtual bool WEXPORT leftBttnUp( int x, int y, WMouseKeyFlags );
        virtual bool WEXPORT leftBttnDbl( int x, int y, WMouseKeyFlags );
        virtual bool WEXPORT scrollNotify( WScrollNotification, int );
        virtual bool WEXPORT keyDown( WKeyCode, WKeyState );


    private:
        class Context : public WObject {
            public:
                Context( WVList * l, WObject * c );
                ~Context();

                WObject *   getContext() { return( _context ); }
                WObject *   getObject( int idx ) { return( (*_items)[idx] ); }
                bool        isLoaded() { return( _items != NULL ); }
                void        unLoad();
                void        reLoad( WVList * l ) { _items = l; }
                int         count();
            private:
                WVList *    _items;
                WObject *   _context;
        };

        void            load( int contextIdx = -1 );
        void            unLoadAllBut( int contextIdx );
        const char *    getString( int index );
        WObject *       getObject( int index );
        int             getLastIndex();
        void            changed();
        void            scrollToSelected();
        void            performScroll( int diff, bool absolute = false );

        WVList          _context;
        uint            _loadSize;
        int             _top;
        int             _selected;
        bool            _full;
        int             _extra;     // #items in last context

        bool            _leftDown;

        WObject*        _fillClient;
        ocbloi          _fill;
        ccbo            _name;
        WObject*        _changedClient;
        cbw             _changed;
        WObject*        _dblClickClient;
        cbw             _dblClick;
};

#endif  // winflist_class

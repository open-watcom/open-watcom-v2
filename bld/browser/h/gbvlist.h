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


#ifndef __GBVLIST_H__
#define __GBVLIST_H__

#include "gbview.h"
#include "hotlist.h"
#include "menumgr.h"

class KeySymbol;
class QueryConfig;
class Symbol;
class GBVListPage;

class GlobalViewList : public MenuHandler, public GlobalView,
                       public HotWindowList
{
public:
                                GlobalViewList( const char * text );
        virtual                 ~GlobalViewList();

        /* ---------------- from WWindow ---------------------- */

        virtual bool            reallyClose();
        virtual bool            gettingFocus( WWindow * );
        virtual bool            losingFocus( WWindow * );
        virtual bool            paint();

        /* ---------------- from View ---------------------- */

        virtual void            event( ViewEvent ve, View * view );
        virtual ViewEvent       wantEvents();
        virtual void            setMenus( MenuManager * );
        virtual void            unsetMenus( MenuManager * );
        virtual ViewIdentity    identity(){ return VIListView; }

        /* ---------------- from MenuHandler ------------------- */

        virtual void            menuSelected( const MIMenuID & id );

        /* ---------------- from HotList ------------------- */

        virtual int             count();
        virtual const char *    getString( int index );
        virtual int             getHotSpot( int index, bool pressed );
        virtual bool            full() { return _full; }

                Symbol *        getSymbol( int index );
                void            reLoad();
                void            toggleDetail( WWindow * );    // callback
                void            changed( WWindow * );         // callback
                void            findFirst();    // get new filters, find first
                void            findNext();     // get new filters, find first

                bool            contextHelp( bool );
private:
        WVList *        _pages;
        bool            _full;
        QueryConfig *   _queryConfig;
        KeySymbol *     _findFilter;
        int             _lastFound;
};

#endif // __GBVLIST_H__

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


#ifndef __DTVSMREF_H__
#define __DTVSMREF_H__

#include <wvlist.hpp>

#include "dtview.h"
#include "hotlist.h"

class WPushButton;
class WDefPushButton;
class WText;
class WListBox;

class DTVSymbolRefs : public DetailView, public HotWindowList
{
public:
                        DTVSymbolRefs( const Symbol * sym );
                        ~DTVSymbolRefs();

    /* ----------------- from DTView ------------------- */

    virtual dr_handle   symHandle() const { return _symbol->getHandle(); }
    virtual ViewType    viewType() const { return ViewSymbolRefs; }
    virtual void        showWin( WWindowState state=WWinStateShow ) { show( state ); }
    virtual void        setFocusWin(){ setFocus(); }
    virtual void        event( ViewEvent, View * );
    virtual ViewEvent   wantEvents();

    /* ---------------- from HotSpotList ------------------- */

    virtual int         count();
    virtual const char *getString( int index );
    virtual int         getHotSpot( int index, bool pressed );

    /* ------------------- from WWindow ------------------- */

    virtual bool        reallyClose() { return aboutToClose(); }
            bool        contextHelp( bool );

            void        toggleDetail( WWindow * );

protected:

    Symbol *            _symbol;
    WVList              _users;

private:

};

#endif // __DTVSMREF_H__

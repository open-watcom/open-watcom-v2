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


#ifndef __DTVSRREF_H__
#define __DTVSRREF_H__

#include "dtview.h"
#include "hotlist.h"

class WPushButton;
class WDefPushButton;
class WText;
class WListBox;

class DescriptionPaint;

class DTVSourceRefs : public DetailView, public HotWindowList
{
public:
                        DTVSourceRefs( const Symbol * sym );
                        ~DTVSourceRefs();

    /* ----------------- from DTView ------------------- */

    virtual dr_handle   symHandle() const { return _symbol->getHandle(); }
    virtual ViewType    viewType() const { return ViewSourceRefs; }
    virtual void        showWin( WWindowState state=WWinStateShow ) { show( state ); }
    virtual void        setFocusWin(){ setFocus(); }

    /* ---------------- from HotSpotList ------------------- */

    virtual int         count();
    virtual const char *getString( int index );
    virtual int         getHotSpot( int index, bool pressed );

    /* ------------------- from WWindow ------------------- */

    virtual bool        reallyClose() { return aboutToClose(); }
            bool        contextHelp( bool );

            void        showSource( WWindow * );

protected:

    Symbol *            _symbol;
    WVList              _references;
    WVList              _data;

private:
};

#endif // __DTVSRREF_H__

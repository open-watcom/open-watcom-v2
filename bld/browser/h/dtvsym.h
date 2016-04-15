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


#ifndef __DTVSYM_H__
#define __DTVSYM_H__

#include <wstring.hpp>

#include "dtview.h"
#include "dgvwsym.gh"
#include "menumgr.h"
#include "wbrwin.h"

class WPushButton;
class WDefPushButton;
class WListBox;

class DescriptionPaint;

class DTViewSymbol : public MenuHandler, public DetailView,
                     public ViewSymbolDlg, public WBRWindow
{
public:
                        DTViewSymbol( const Symbol * sym, bool box = false );
                        ~DTViewSymbol();

            void        initialize();

    virtual dr_handle   symHandle() const { return _symbol->getHandle(); }
    virtual ViewType    viewType() const { return ViewDetail; }

    virtual bool        paint();
    virtual bool        leftBttnUp( int, int, WMouseKeyFlags );
    virtual bool        leftBttnDbl( int, int, WMouseKeyFlags );
    virtual bool        keyDown( WKeyCode, WKeyState );

    virtual void        showWin( WWindowState state=WWinStateShow ) { show( state ); }
    virtual void        setFocusWin() { setFocus(); }

    virtual bool        gettingFocus( WWindow * ) { return focusChange( true ); }
    virtual bool        losingFocus( WWindow * ) { return focusChange( false ); }
    virtual bool        reallyClose() { return aboutToClose(); }

    virtual void        setMenus( MenuManager * );
    virtual void        unsetMenus( MenuManager * );
    virtual void        menuSelected( const MIMenuID & id );

            void        showSource();

            bool        contextHelp( bool );

    static  DTViewSymbol *  createView( const Symbol * sym );

protected:
    Symbol *            _symbol;
    bool                _useBox;

    WString             _fileText;
    WListBox *          _symbolBox;
private:

    DescriptionPaint *  _descPaint;
};

#endif // __DTVSYM_H__

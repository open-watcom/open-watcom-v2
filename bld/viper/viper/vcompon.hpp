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


#ifndef vcomponent_class
#define vcomponent_class

#include "wmdichld.hpp"
#include "whotpbox.hpp"
#include "wview.hpp"
#include "wvlist.hpp"
#include "mcompon.hpp"

WCLASS WMenuItem;
WCLASS VpeMain;
WCLASS MAction;
WCLASS WText;
WCLASS VComponent : public WMdiChild, public WView
{
    Declare( VComponent )
    public:
        VComponent( VpeMain* parent, MComponent* comp, WStyle style=WStyleDefault );
        VComponent( VpeMain* parent, const WRect& r, MComponent* comp, WStyle style=WStyleDefault );
        ~VComponent();
        void resetPopups();
        void createControls();
        bool gettingFocus( WWindow* );
        bool losingFocus( WWindow* );
        void WEXPORT updateView();
        void WEXPORT modelGone();
        void WEXPORT mAddItem( WMenuItem * item=NULL );
        void WEXPORT mRemoveItem( WMenuItem* item=NULL );
        void WEXPORT removeItem( WFileName &fn );
        void WEXPORT mRenameItem( WMenuItem* item=NULL );
        void WEXPORT setupItem( MItem* m );
        void WEXPORT showItemCommand();
        void WEXPORT mIncludedItems( WMenuItem* item=NULL );
        void WEXPORT touchItem( MItem* m );
        void WEXPORT doAction( MItem* f, const WString& actionName );
        void WEXPORT doAction( MItem* f, MAction* action );
        void WEXPORT actionSetup( MItem* item, MAction* action );
        void WEXPORT bActionComponent( WWindow* w );
        void WEXPORT mAutodepend();
        void WEXPORT mAutotrack();
        void WEXPORT mDebugMode();
        bool WEXPORT debugMode() { return mode() == SWMODE_DEBUG; }
        void WEXPORT renameComponent( WFileName& fn, MRule* rule, WString& mask );
        void WEXPORT setupComponent();
        void WEXPORT setCompBefore();
        void WEXPORT setCompAfter();
        void WEXPORT showCompCommand();
        void WEXPORT touchComponent( bool quiet = FALSE );
        bool WEXPORT keyDown( WKeyCode, WKeyState );
        MItem* WEXPORT selectedItem();
        MComponent* component() { return _component; }
        MItem* WEXPORT target() { return _component->target(); }
        bool newItem( WFileName&, bool warn = TRUE, bool mark = FALSE, unsigned owner=0 );
        void beginFileList( unsigned owner );
        void markFile( WFileName &file, unsigned owner );
        void endFileList( unsigned owner );
    private:
        VpeMain*        _parent;
        MComponent*     _component;
        WHotPickBox*    _vItems;
        WText*          _vTitle;
        WText*          _tBox;
        WVList          _controls;
        void initCtor();
        void dblClicked( WWindow* );
        void expandItem( WWindow* );
        SwMode mode() { return _component->mode(); }
        bool okToInclude( MItem* item, bool nowarn, MItem* dupitem=NULL );
        void actionError( MItem* item, const WString& actionName );
};

#endif

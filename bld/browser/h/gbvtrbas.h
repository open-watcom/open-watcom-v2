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


#ifndef __GBVTRBAS_H__
#define __GBVTRBAS_H__

#include <wwindow.hpp>

#include "gbview.h"
#include "menumgr.h"

class Symbol;
class MenuManager;

class GlobalViewTree : public GlobalView, public MenuHandler {
public:
                                GlobalViewTree( MenuManager * );
        virtual                 ~GlobalViewTree();

        /* ---------------- from View - re-declare pure --------*/

        virtual void            event( ViewEvent ve, View * view ) = 0;
        virtual ViewEvent       wantEvents() = 0;
        virtual void            setMenus( MenuManager * );
        virtual void            unsetMenus( MenuManager * );
        virtual ViewIdentity    identity() = 0;

        /* ---------------- from MenuHandler ------------------- */

        virtual void            menuSelected( const MIMenuID & id );

protected:
                enum TreeViewEvent {
                    ShowDetail,
                    ShowDefinition,
                    ShowReferences,
                    ShowUsers,
                    ScrollToSelected,
                    Arrange,
                    SelectRoots,
                    ExpandOne,
                    ExpandBranch,
                    ExpandAll,
                    CollapseBranch,
                    CollapseAll,
                };

                enum ExpandState {
                    Expanded,
                    Collapsed,
                    Leaf,
                    NoSymbol
                };

                void            selChange();
                bool            key( WKeyCode, WKeyState );

        virtual void            treeViewEvent( TreeViewEvent ) = 0;
        virtual ExpandState     state() = 0;

                MenuManager *   _menuManager;
};

#endif // __GBVTRBAS_H__

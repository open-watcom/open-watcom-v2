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


#ifndef picksym_h
#define picksym_h

#include <wstring.hpp>
#include <wvlist.hpp>
#include "keysym.h"
#include "wbrdefs.h"

class WInfiniteList;
class WEditBox;
class WCheckBox;
class WCommandList;
class WComboBox;
class WPushButton;
class WText;
class Symbol;
class ViewSymbol;
class ProjectModel;

class PickSymbol : public WBRWindow
{
public:
                        PickSymbol( ProjectModel *project, int x, int y,
                                    WBRWindow *, dr_search=DR_SEARCH_ALL,
                                    bool showPin=FALSE );
                        ~PickSymbol();

//  callbacks
    void                dblSelect( WWindow * );
    void                select( WWindow * );
    void                symbolTypeSet( WWindow * );
    void                detailSelected( WWindow * );
    void                okSelected( WWindow * );

//      event handlers
    void                enterHit();

  private:
    WObject *           findSymbols( WVList * list, ProjSearchCtxt * obj, int n );
    void                locateSymbols( WWindow * );
    char const *        getName( Symbol * sym );
    void                setTitle( dr_search );

    KeySymbol           _filter;
    bool                _readyToLoad;   // true when search has been hit
    ProjectModel *      _project;
    WCommandList *      _symbolName;
    WCommandList *      _fileName;
    WCommandList *      _baseClass;
    WCommandList *      _containingFunc;
    WComboBox *         _symbolTypeSelect;
    WPushButton *       _locateButton;
    WPushButton *       _detailButton;
    WPushButton *       _okButton;
    WInfiniteList *     _list;
    WText *             _symbolDescription;
    static bool         _showDetails;
};
#endif /* picksym_h */

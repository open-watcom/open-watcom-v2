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


#ifndef __TEXTTREE_H__
#define __TEXTTREE_H__

#include <wvlist.hpp>
#include "wbrwin.h"
#include "wbrdefs.h"

class WText;
class ListBoxKeyCtrl;

class TextTree : public WBRWindow
{
public:
                TextTree();
                TextTree( int x, int y, char * prntListName,
                          char * kidListName, Symbol * sym,
                          bool bottom = FALSE );
                ~TextTree();

    static WBRWindow * createView( int, int, Symbol * );
           bool        isEqual( WObject * sym );

    // event handlers
    void        parentOrKidSelected( WWindow* );
    void        historySelected( WWindow * );
    void        resized( WORD, WORD );
    bool        setMinimumSize( short *w, short *h );

protected:
    virtual char *      getName( WObject &, char * ) = 0;
    virtual WVList &    getParents( WObject &, WVList & ) = 0;
    virtual WVList &    getKids( WObject &, WVList & ) = 0;
    virtual void        doubleClick( WObject * );
    virtual char *      selected( WObject * );
            WObject *   getHistoryObj( void );
    void        loadMyObj( WObject & startObj, bool destroyListContents=TRUE );
    Symbol *    _info;  // used to know who the start of the tree is

private:
    void        reset();
    void        loadParentKidLists( WObject & focusObject );
    void        doDoubleClick( WWindow * );
    void        doSelected( WWindow * );
    void        loadHistoryList();
    bool        _destroyListContents;

    WText *     _historyTitle;
    WText *     _parentTitle;
    WText *     _kidTitle;

    ListBoxKeyCtrl *  _parentList;
    ListBoxKeyCtrl *  _kidList;
    ListBoxKeyCtrl *  _historyList;

    WText *     _parentDesc;
    WText *     _kidDesc;

    WVList      _parents;
    WVList      _kids;
    WVList      _history;

    WString     _historyPath;
    uint        _currentHistoryItem;

    bool        _bottom;
};

#endif //__TEXTTREE_H__

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


#ifndef __NESTEDLV_H__
#define __NESTEDLV_H__

#include <wtptlist.hpp>

#include "wbrwin.h"
#include "verblist.h"

#include "frstintf.h"

typedef void (* CBGetChildData) ( void *, int, void * &, char * &, int & );
typedef int  (* CBGetNumChildren) ( void * );
typedef void (* CBDestroySelf) ();

/*
 * only used in NestedListView
 */

class Item {
private:
    friend class NestedListView;

    Item( void * pData, int numChildren, int indentation, char * pChildName );
    ~Item() {}
    void * _pData;
    char * _pChildName;
    int    _numChildren;
    int    _indentation;
    bool   _isExpanded;
};

typedef TemplateList<Item *> ItemList;

class NestedListView : public WBRWindow {
public :

    NestedListView( int x, int y, char * title,
                    ForestListInterface * pFLI );

    ~NestedListView();

protected :

private :
    void        setup( char * title );
    void        fillRoots();
    bool        keyPressed( WObject *, int key );
    void        plusSelected( WObject * );
    void        minusSelected( WObject * );
    void        starSelected( WObject * );
    void        itemDblClicked( WObject * );
    void        expandNode( int index );
    void        buildString( WString & str, Item * pItem,
                             bool isExpanded );
    void        collapseNode( int index );
    void        expandAllNode( int index );

    VerboseListBox *    _listBox;
    ItemList            _itemList;
    ForestListInterface * _fli;
};

#endif // __NESTEDLV_H__

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


#ifndef __STRCVIEW_H__
#define __STRCVIEW_H__

#include "wbrdefs.h"
#include "symbol.h"
#include <wvlist.hpp>
#include <wstring.hpp>
#include "wbrwin.h"
#include "methfilt.h"

class WTextBox;
class VerboseListBox;
class StrucViewItem;
class ClassType;
class ClassLattice;
class DerivationPtr;

class StrucView : public WBRWindow
{
public:
                StrucView();
                StrucView(int x, int y, Symbol * info );
                ~StrucView();

    WVList *            getObjs( void ) const {
                            return _listobjs;
                        }
    WVList *            getSeen( void ) const {
                            return _nodesSeen;
                        }
    VerboseListBox *    getListBox( void ) const {
                            return _listbox;
                        }
    FilterFlags         getFilter( void ) const {
                            return _filter;
                        }
    ClassLattice *      getLattice( void ) const {
                            return _lattice;
                        }

    void                addSeen( StrucViewItem * );
    bool                isSeen( Symbol * );
    bool                isEqual( WObject * );
    static WBRWindow *  createView( int x, int y, Symbol * info);

        // event handlers
    void             reset();
    bool             setMinimumSize( short *w, short *h );
    void             itemSelected( WWindow * );
    int              keyPressed( WWindow *, int key );
    void             itemDblClicked( WWindow * );
    void             plusSelected( WWindow * );
    void             minusSelected( WWindow * );
    void             starSelected( WWindow * );
    void             filterSelected( WWindow * );

private:

    // _listobjs is a list of all the StrucViewItems in the order they appear
    // in _listbox.  _listobjs does not own these objects -- don't delete them
    WVList *            _listobjs;
    VerboseListBox *    _listbox;
    WText *             _type;
    StrucViewItem *     _treeRoot;
    ClassLattice *      _lattice;

    //_nodesSeen is a list of all nodes that have been seen as expandable
    WVList *            _nodesSeen;

    FilterFlags         _filter;
};

enum ExpandState {LeafNode, Collapsed, Expanded};

class StrucViewItem : public WObject
{
  public:
                    StrucViewItem( StrucView * parent, Symbol * strucInfo,
                                   uchar indent=0 );
                    StrucViewItem( StrucView * parent, ClassLattice *,
                                   uchar indent=0 );
    virtual        ~StrucViewItem();

    void            expandNode( int index );
    void            collapseNode( int & );
    void            expandAllNode( int & );
    const char *    typeStr(void);
    const Symbol *  info(void) const{ return _info; }
    char *          name(void);
    ExpandState     state(void) const{ return _expandState; }
    void            reset(void);
    bool            isEqual( WObject * );

  private:

    ClassType *     flattenTypeDesc( Symbol *info, WString &desc );
    void            initState();
    Symbol *        _info;
    WVList          _kids;// [StrucViewItems]
    WString         _name;
    WString         _type;
    StrucView *     _parent;
    ClassLattice *  _classNode;
    uchar           _indentLevel;
    ExpandState     _expandState;
    bool            _initialized;

            void startInherited( void );
static      bool inhHook( DerivationPtr & ptr, void * info );
};

#endif // strcview_h

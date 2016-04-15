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


#ifndef __OUTLINE_H__
#define __OUTLINE_H__

#include "gbvtrbas.h"
#include "hotlist.h"
#include "mempool.h"

class Symbol;
class KeySymbol;
class QueryConfig;
class TreeNode;

template <class Type> class WCPtrOrderedVector;

class OutlineElement {
    friend class Outline;

public:
            enum ElementState {
                ESLeaf      = 0x00,
                ESCollapsed = 0x01,
                ESExpanded  = 0x02,
                ESHidden    = 0x04,
            };

                                OutlineElement();
                                OutlineElement( int level, TreeNode *, OutlineElement * );
                                ~OutlineElement();

            void                toggleExpand();
            void                expand( bool all = false );
            void                collapse( bool all = false );

            bool                operator==( const OutlineElement & o ) const {
                                    return this == &o;
                                }

            void *              operator new( size_t );
            void                operator delete( void * );

            OutlineElement *    next() const;
            OutlineElement *    sibling() const { return _sibling; }
            OutlineElement *    visibleSib();   // next visible sibling
            const char *        string() const;
            ElementState        state() const { return (ElementState) _state; }
            uint                level() const { return _level; }
            Symbol *            symbol() const { return _symbol; }

            void                drawLine( WWindow * prt, int & idx,
                                          int width, int height );

            bool                enabled() { return !(_state & ESHidden); }
            void                setEnable( bool en );

protected:
            OutlineElement *    _sibling;
            OutlineElement *    _child;
            Symbol *            _symbol;
            char *              _name;

private:
            ubit                _state   : 3;
            ubit                _lastSib : 1;
            uint_16             _level   : 12;

    static  MemoryPool          _pool;
};

class Outline : public HotWindowList, public GlobalViewTree
{
public:
                                Outline( const char * text );
    virtual                     ~Outline();

            //------ from WWindow ----------//

    virtual bool                paint();
    virtual bool                keyDown( WKeyCode, WKeyState );

            //------ from View - re-declare pure ------//

        virtual void            event( ViewEvent ve, View * view );
        virtual ViewEvent       wantEvents();
        virtual ViewIdentity    identity() = 0;

        virtual void            setMenus( MenuManager * );
        virtual void            unsetMenus( MenuManager * );

            //------ from Hotlist ----------//
    virtual int                 count();
    virtual const char *        getString( int index );
    virtual int                 getHotSpot( int index, bool pressed );
    virtual int                 getHotOffset( int );

            void                addLevel0( TreeNode * );
            void                toggleExpand( WWindow * );
            void                detailView( WWindow * );
            void                changed( WWindow * ) { selChange(); }

protected:
            //---- from GlobalViewTree ----//

    virtual void                treeViewEvent( TreeViewEvent );
    virtual ExpandState         state();
    virtual void                load() = 0;
    virtual const char *        emptyText() = 0;

    virtual void                menuSelected( const MIMenuID & id );

            void                selectRoots();

            void                resetOutline();    // clear to initial state

            OutlineElement *    element( int index );
            void                resetCache();

            OutlineElement *    _sentinel;

            KeySymbol *         _findFilter;
            KeySymbol *         _loadFilter;
            QueryConfig *       _queryConfig;

private:
            void                findFirst();
            void                findNext();
            void                query();

            OutlineElement *    _cacheElement;
            int                 _cacheIndex;

            int                 _findElement;

            WCPtrOrderedVector<OutlineElement> *    _findStack;
};

#endif // __OUTLINE_H__

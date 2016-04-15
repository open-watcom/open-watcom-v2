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


#ifndef __GTWIN_H__
#define __GTWIN_H__

#include "wbrwin.h"
#include "gtbase.h"
#include "gbvtrbas.h"

#define TreeStyleDefault (WBRWinStyleDefault \
                         |WStyleHScrollAll|WStyleHScrollEvents \
                         |WStyleVScrollAll|WStyleVScrollEvents)

class KeySymbol;
class QueryConfig;

class TreeWindow : public WBRWindow, public GlobalViewTree
{
public:
            TreeWindow( char * title,
                        WStyle s = TreeStyleDefault );
    virtual ~TreeWindow();


        /* ---------------- from WWindow ---------------------- */

    virtual bool            reallyClose();
    virtual bool            gettingFocus( WWindow * );
    virtual bool            losingFocus( WWindow * );
    virtual void            resized( WOrdinal width, WOrdinal height );
    virtual bool            paint();
    virtual bool            leftBttnDn( int x, int y,
                                        WMouseKeyFlags flags );
    virtual bool            leftBttnDbl( int x, int y,
                                         WMouseKeyFlags flags );
    virtual bool            rightBttnDn( int x, int y,
                                        WMouseKeyFlags flags );
    virtual bool            scrollNotify( WScrollNotification sb, int pos );
    virtual bool            keyDown( WKeyCode, WKeyState );

        /* ---------------- from View ---------------------- */

    virtual void            event( ViewEvent ve, View * view );
    virtual ViewEvent       wantEvents();
    virtual void            setMenus( MenuManager * );
    virtual void            unsetMenus( MenuManager * );
    virtual ViewIdentity    identity() = 0;

            TreeDirection   getDirection() const;
            bool            getSmartEdges() const;
            void            enableAll( bool enable = true,
                                       bool rePlace = false );
            void            rePlaceAll();

            void            scrollToNode( TreeNode *, WRect & );

            TreeCoord       getXOff() const;
            TreeCoord       getYOff() const;
            void            scrollTo( TreeCoord x, TreeCoord y );
            void            resetScrollRange();

protected:
        /* -------------- from GlobalViewTree ------------- */

    virtual void            menuSelected( const MIMenuID & id );

    virtual void            treeViewEvent( TreeViewEvent );
    virtual ExpandState     state();

            bool            matches( int rt, int nd, KeySymbol * filt );

            void            selectRoots();

            void            showDetail();
            void            showDefinition();
            void            showReferences();
            void            showUsers();

    virtual void            fillRoots() = 0;
    virtual const char *    emptyText() = 0;

            //--------- data -----------//

            TreeRootList    _roots;
            TreeNode *      _currNode;

            KeySymbol *     _loadFilter;
            KeySymbol *     _findFilter;
private:
            void            giveFocusToNode( TreeNode * pNode );
            void            setCurrentPosition( TreeRoot * pRoot,
                                                TreeRect *oldPos,
                                                TreeRect *oldRootPos,
                                                bool changeDirection );
            TreeNode*       hitTest( int x, int y );

            void            toggleEdges();
            void            toggleAutoArrange();
            void            changeDirection();
            void            reLoad();       // empty roots, re-paint

            #ifdef DEBUGTREE
            void            showDebug();
            void            showSib();
            #endif

            void            arrangeAll();
            void            arrangeAllSelectRoots();

            void            query();        // get new parameter, re-load
            void            findFirst();    // get new filters, find first
            void            findNext();     // get new filters, find first

            //------------- data ---------------//

            bool            _rootsLoaded;
            bool            _empty;

            bool            _autoArrange;
            EdgeType        _edgeType;
            TreeDirection   _direction;


            // ------ Scroll Data --------//

            TreeCoord       _xOffset;
            TreeCoord       _yOffset;
            TreeCoord       _hScrollFactor;
            TreeCoord       _vScrollFactor;
            int             _hScrollColMult;
            int             _vScrollRowMult;

            QueryConfig *   _queryConfig;
            int             _findRoot;
            int             _findNode;

            TreeRect        _world;
};

#endif // __GTWIN_H__

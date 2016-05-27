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


#ifndef __GTEDGES_H__
#define __GTEDGES_H__

#include <wtptlist.hpp>
#include "gtcoord.h"
#include "mempool.h"
#include "outdev.h"

class TreePtr;
class TreeRing;
class TreeEdge;

/*
 * (x1,y1) -- starting co-ordinates of this line
 * (x2,y2) -- ending co-ordinates
 */
class TreeLineCoords {
public:
    TreeLineCoords( TreeCoord xa, TreeCoord ya, TreeCoord xb, TreeCoord yb )
        : x1( xa ), y1( ya ), x2( xb ), y2( yb ) {}

    TreeCoord x1, y1;
    TreeCoord x2, y2;

};

typedef TemplateList<TreeLineCoords *> TreeLineCoordList;

class TreeLine {
public :

    enum LineType {
        Edge,
        RingBottom,
        RingTop,
        RingConnector,
        Node
    };

    TreeLine( TreeCoord _x1, TreeCoord _y1,
              TreeCoord _x2, TreeCoord _y2,
              LineType lineType );
    TreeLine();
    ~TreeLine( void );
    void *      operator new( size_t );
    void        operator delete( void * );

    void        set( TreeCoord _x1, TreeCoord _y1,
                     TreeCoord _x2, TreeCoord _y2,
                     LineType lineType );
    void        addSeg( TreeCoord _x1, TreeCoord _y1,
                        TreeCoord _x2, TreeCoord _y2 );
    void        purgeList( void );
    LineType    getLineType( void );
    bool        isBroken( void );
    void        paint( OutputDevice *, TreeCoord xOff, TreeCoord yOff );

    TreeLineCoords _lineCoords;


private :
    TreeLineCoordList * _pBrokenLineList;
    LineType _lineType;

    static MemoryPool   _pool;
};

typedef TemplateList<TreeLine *> TreeLineList;

class TreeEdgeBreaker
{
public:
            TreeEdgeBreaker() : _horizontalSorted( false ), _verticalSorted( false ) {}
            ~TreeEdgeBreaker() {}
            void addLine( TreeLine * _line );
            void purgeContents( void );
            void breakLinesH( void );
            void breakLinesV( void );
    static  int compareLinesH( const TreeLine ** lineLhs,
                              const TreeLine ** lineRhs );
    static  int compareLinesV( const TreeLine ** lineLhs,
                              const TreeLine ** lineRhs );

private:
            TreeLineList _verticalLines;
            TreeLineList _horizontalLines;
            bool         _horizontalSorted;
            bool         _verticalSorted;
};

#endif // __GTEDGES_H__

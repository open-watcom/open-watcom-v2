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


#include "gtbase.h"
#include "gtring.h"
#include "gtedges.h"
#include "assure.h"

const int HSKIP = 5;
const int VSKIP = 5;

const int POOLSIZE = 64;

MemoryPool TreeLine::_pool( sizeof( TreeLine ), "TreeLine", POOLSIZE );

TreeLine::TreeLine( TreeCoord _x1, TreeCoord _y1,
                    TreeCoord _x2, TreeCoord _y2,
                    LineType lineType )

    : _lineCoords( _x1, _y1, _x2, _y2 )
    , _lineType( lineType )
    , _pBrokenLineList( NULL )
//-----------------------------------------------
{
}

TreeLine::TreeLine()
    : _lineCoords( 0, 0, 0, 0 )
    , _lineType( TreeLine::Edge )
    , _pBrokenLineList( NULL )
//-------------------------------
{
}

TreeLine::~TreeLine()
{
    if( _pBrokenLineList ) {
        purgeList();
    }
}

void * TreeLine::operator new( size_t )
//-------------------------------------
{
    return( _pool.alloc() );
}

void TreeLine::operator delete( void * mem )
//------------------------------------------
{
    _pool.free( mem );
}

void TreeLine::purgeList( void )
//------------------------------
{
    int i;

    if( _pBrokenLineList ) {
        for( i = _pBrokenLineList->count() - 1; i >= 0; i -= 1 ) {
            delete (*_pBrokenLineList)[ i ];
        }
        _pBrokenLineList->reset();
        delete _pBrokenLineList;
        _pBrokenLineList = NULL;
    }
}

void TreeLine::set( TreeCoord _x1, TreeCoord _y1,
                    TreeCoord _x2, TreeCoord _y2,
                    LineType lineType )
//-----------------------------------------------
{
    _lineCoords.x1 = _x1;
    _lineCoords.x2 = _x2;
    _lineCoords.y1 = _y1;
    _lineCoords.y2 = _y2;
    _lineType = lineType;
}

void TreeLine::addSeg( TreeCoord x1, TreeCoord y1,
                       TreeCoord x2, TreeCoord y2 )
//-------------------------------------------------
{
    if( _pBrokenLineList == NULL ) {
        _pBrokenLineList = new TreeLineCoordList;
    }

    _pBrokenLineList->add( new TreeLineCoords( x1, y1, x2, y2 ));
}

TreeLine::LineType TreeLine::getLineType( void )
//----------------------------------------------
{
    return( _lineType );
}

bool TreeLine::isBroken( void )
//-----------------------------
{
    return( _pBrokenLineList != NULL );
}

void TreeLine::paint( OutputDevice *dev, TreeCoord xOff, TreeCoord yOff )
//-----------------------------------------------------------------------
{
    // NYI -- figure out paintinfo stuff (bandage)
    PaintInfo   pinf;
    PaintInfo * oPinf;

    oPinf = dev->getPaintInfo();

    if( oPinf == NULL ) {
        dev->setPaintInfo( &pinf );
    }

    if( _pBrokenLineList != NULL ) {
        int i;

        for( i = 0; i < _pBrokenLineList->count(); i += 1 ) {
            dev->moveTo( ( *_pBrokenLineList )[ i ]->x1 - xOff,
                         ( *_pBrokenLineList )[ i ]->y1 - yOff );
            dev->lineTo( ( *_pBrokenLineList )[ i ]->x2 - xOff,
                         ( *_pBrokenLineList )[ i ]->y2 - yOff );
        }
    } else {
        dev->moveTo( _lineCoords.x1 - xOff, _lineCoords.y1 - yOff );
        dev->lineTo( _lineCoords.x2 - xOff, _lineCoords.y2 - yOff );
    }

    dev->setPaintInfo( oPinf );
}

////////////////////////// TreeEdgeBreaker ///////////////////////////////

void TreeEdgeBreaker::addLine( TreeLine * _line )
//----------------------------------------------------------------------
// Categorize lines as_vertical or _horizontal
//
// isNodeLine indicates whether or not the line should be able to be
// broken -- for example, borders for nodes should remain intact
//
{
    ASSERT((( _line->_lineCoords.y1 == _line->_lineCoords.y2 ) ||
            ( _line->_lineCoords.x1 == _line->_lineCoords.x2 )),
           "Edge is neither H nor V\n", 0 );

    if( _line->_lineCoords.y1 == _line->_lineCoords.y2 ) {
        _horizontalLines.add( _line );
    } else {
        if( _line->_lineCoords.x1 == _line->_lineCoords.x2 ) {
            _verticalLines.add( _line );
        }
    }
}

void TreeEdgeBreaker::purgeContents( void )
//----------------------------------------
{
    int i;

    for( i = _verticalLines.count() - 1; i >= 0; i -= 1 ) {
        _verticalLines[ i ]->purgeList();
    }

    for( i = _horizontalLines.count() - 1; i >= 0; i -= 1 ) {
        _horizontalLines[ i ]->purgeList();
    }

    _verticalLines.reset();
    _horizontalLines.reset();

    _horizontalSorted = false;
    _verticalSorted = false;
}

static int TreeEdgeBreaker::compareLinesV( const TreeLine ** lineLhs,
                                           const TreeLine ** lineRhs )
//-------------------------------------------------------------------
// Compare two lines.
// Returns : -1 if LHS line starts above RHS
//            0 if LHS and RHS start at same y-coord
//           +1 if LHS line starts below RHS
//
{
    TreeCoord minLhsY, minRhsY;

    if(( *lineLhs )->_lineCoords.y1 < ( *lineLhs )->_lineCoords.y2 ) {
        minLhsY = ( *lineLhs )->_lineCoords.y1;
    } else {
        minLhsY = ( *lineLhs )->_lineCoords.y2;
    }

    if(( *lineRhs )->_lineCoords.y1 < ( *lineRhs )->_lineCoords.y2 ) {
        minRhsY = ( *lineRhs )->_lineCoords.y1;
    } else {
        minRhsY = ( *lineRhs )->_lineCoords.y2;
    }

    if( minLhsY < minRhsY ) {
        return -1;
    } else if ( minLhsY > minRhsY ) {
        return 1;
    } else {
        return 0;
    }
}

static int TreeEdgeBreaker::compareLinesH( const TreeLine ** lineLhs,
                                           const TreeLine ** lineRhs )
//-------------------------------------------------------------------
// Compare two lines.
// Returns : -1 if LHS line starts to the left of RHS
//            0 if LHS and RHS start at same x-coord
//           +1 if LHS line starts to the right of RHS
//
{
    TreeCoord minLhsX, minRhsX;

    if(( *lineLhs )->_lineCoords.x1 < ( *lineLhs )->_lineCoords.x2 ) {
        minLhsX = ( *lineLhs )->_lineCoords.x1;
    } else {
        minLhsX = ( *lineLhs )->_lineCoords.x2;
    }

    if(( *lineRhs )->_lineCoords.x1 < ( *lineRhs )->_lineCoords.x2 ) {
        minRhsX = ( *lineRhs )->_lineCoords.x1;
    } else {
        minRhsX = ( *lineRhs )->_lineCoords.x2;
    }

    if( minLhsX < minRhsX ) {
        return -1;
    } else if ( minLhsX > minRhsX ) {
        return 1;
    } else {
        return 0;
    }
}

void TreeEdgeBreaker::breakLinesV( void )
//--------------------------------------
// Breaks vertical edges into component lines so that they do not
// overlap any horizontal lines
//
{
    TreeCoord _horizontalY;
    TreeCoord minHX, maxHX;
    TreeCoord startY, firstY, lastY;
    int indexH, indexV;

    /*
     * It's important that the horizontal lines be sorted; the algorithm
     * requires this because it needs to be able to find the next
     * horizontal line that is intercepted by each vertical line
     */
    if( !_horizontalSorted ) {
        _horizontalLines.sort( (TComp) TreeEdgeBreaker::compareLinesV );
        _horizontalSorted = true;
    }

    /*
     * Break vertical lines.  If a_vertical line is broken into components,
     * these component_vertical lines are added to the end of the
     * _verticalLines container and the original pointer in _verticalLines
     * is set to NULL.
     */
    for( indexV = 0; indexV < _verticalLines.count(); indexV += 1 ) {

        /*
         * We don't want to break any of the node borders
         */
        if( _verticalLines[ indexV ]->getLineType() == TreeLine::Node ) {
            continue;
        }

        bool overlapNode = false;

        if( _verticalLines[ indexV ]->_lineCoords.y1 < _verticalLines[ indexV ]->_lineCoords.y2 ) {
            startY = _verticalLines[ indexV ]->_lineCoords.y1;
            firstY = _verticalLines[ indexV ]->_lineCoords.y1;
            lastY = _verticalLines[ indexV ]->_lineCoords.y2;
        } else {
            startY = _verticalLines[ indexV ]->_lineCoords.y2;
            firstY = _verticalLines[ indexV ]->_lineCoords.y2;
            lastY = _verticalLines[ indexV ]->_lineCoords.y1;
        }

        /*
         * Scan our way from top to bottom
         */
        for( indexH = 0; indexH < _horizontalLines.count(); indexH += 1 ) {

            /*
             * Since horizontal lines are sorted by vertical position (top
             * to bottom), we can stop looking when a horizontal line
             * is below the vertical line.
             */
            _horizontalY = _horizontalLines[ indexH ]->_lineCoords.y1;
            if( _horizontalY > lastY ) break;

            if( _horizontalLines[ indexH ]->_lineCoords.x1 < _horizontalLines[ indexH ]->_lineCoords.x2 ) {
                minHX = _horizontalLines[ indexH ]->_lineCoords.x1;
                maxHX = _horizontalLines[ indexH ]->_lineCoords.x2;
            } else {
                minHX = _horizontalLines[ indexH ]->_lineCoords.x2;
                maxHX = _horizontalLines[ indexH ]->_lineCoords.x1;
            }

            /*
             * Check if we're overlapping something
             */
            if(( _horizontalY > startY ) && ( _horizontalY < lastY ) &&
               (_verticalLines[ indexV ]->_lineCoords.x1 < maxHX ) &&
               (_verticalLines[ indexV ]->_lineCoords.x1 > minHX )) {

                /*
                 * Case 1:
                 *      - just met a node; break the ptr just above the node
                 */
                if( _horizontalLines[ indexH ]->getLineType() == TreeLine::Node
                    && !overlapNode ) {

                   _verticalLines[ indexV ]->addSeg( _verticalLines[ indexV ]->_lineCoords.x1,
                                                     startY,
                                                     _verticalLines[ indexV ]->_lineCoords.x1,
                                                     _horizontalY - HSKIP );

                    overlapNode = true;

                /*
                 * Case 2:
                 *  - other end of a node; start new line just below node
                 */
                } else if( _horizontalLines[ indexH ]->getLineType() == TreeLine::Node
                           && overlapNode ) {
                    startY = _horizontalY + HSKIP;
                    overlapNode = false;
                /*
                 * Case 3:
                 *      - met an ordinary type of line (tree ring connector or ptr)
                 *    so add new line segment ending just above the intersecting
                 *    line and start a new line just below it
                 */
                } else if( _horizontalLines[ indexH ]->getLineType() != TreeLine::Node ) {
                    _verticalLines[ indexV ]->addSeg( _verticalLines[ indexV ]->_lineCoords.x1,
                                                      startY,
                                                      _verticalLines[ indexV ]->_lineCoords.x1,
                                                      _horizontalY - HSKIP );

                    startY = _horizontalY + HSKIP;
                }
            }
        }

        if( startY != firstY ) {
            _verticalLines[ indexV ]->addSeg( _verticalLines[ indexV ]->_lineCoords.x1,
                                              startY,
                                              _verticalLines[ indexV ]->_lineCoords.x1,
                                              lastY );
        }
    }
}

void TreeEdgeBreaker::breakLinesH( void )
//--------------------------------------
// Breaks horizontal edges into component lines so that they do not
// overlap any vertical lines
//
{
    TreeCoord _verticalX;
    TreeCoord minHY, maxHY;
    TreeCoord startX, firstX, lastX;
    int indexV, indexH;

    if( !_verticalSorted ) {
        _verticalLines.sort( (TComp) TreeEdgeBreaker::compareLinesH );
        _verticalSorted = true;
    }

    /*
     * Break horizontal lines.  If a horizontal line is broken into components,
     * these component horizontal lines are added to the end of the
     * _horizontalLines container and the original pointer in _horizontalLines
     * is set to NULL.
     */
    for( indexH = 0; indexH < _horizontalLines.count(); indexH += 1 ) {

        /*
         * We don't want to break any of the node borders
         */
        if( _horizontalLines[ indexH ]->getLineType() == TreeLine::Node ) {
            continue;
        }

        bool overlapNode = false;

        if( _horizontalLines[ indexH ]->_lineCoords.x1 < _horizontalLines[ indexH ]->_lineCoords.x2 ) {
            startX = _horizontalLines[ indexH ]->_lineCoords.x1;
            firstX = _horizontalLines[ indexH ]->_lineCoords.x1;
            lastX = _horizontalLines[ indexH ]->_lineCoords.x2;
        } else {
            startX = _horizontalLines[ indexH ]->_lineCoords.x2;
            firstX = _horizontalLines[ indexH ]->_lineCoords.x2;
            lastX = _horizontalLines[ indexH ]->_lineCoords.x1;
        }

        /*
         * Scan our way from left to right
         */
        for( indexV = 0; indexV < _verticalLines.count(); indexV += 1 ) {

            /*
             * Since vertical lines are sorted by horizontal position (left
             * to right), we can stop looking when a vertical line
             * is to the right of the horizontal line.
             */
            _verticalX = _verticalLines[ indexV ]->_lineCoords.x1;
            if( _verticalX > lastX ) break;

            if( _verticalLines[ indexV ]->_lineCoords.y1 < _verticalLines[ indexV ]->_lineCoords.y2 ) {
                minHY = _verticalLines[ indexV ]->_lineCoords.y1;
                maxHY = _verticalLines[ indexV ]->_lineCoords.y2;
            } else {
                minHY = _verticalLines[ indexV ]->_lineCoords.y2;
                maxHY = _verticalLines[ indexV ]->_lineCoords.y1;
            }


            /*
             * Check if we're overlapping something
             */
            if(( _verticalX > startX ) && ( _verticalX < lastX ) &&
               (_horizontalLines[ indexH ]->_lineCoords.y1 < maxHY ) &&
               (_horizontalLines[ indexH ]->_lineCoords.y1 > minHY )) {

                /*
                 * Case 1:
                 *      - just met a node; break the ptr just above the node
                 */
                if( _verticalLines[ indexV ]->getLineType() == TreeLine::Node
                    && !overlapNode ) {

                    _horizontalLines[ indexH ]->addSeg( startX,
                                                        _horizontalLines[ indexH ]->_lineCoords.y1,
                                                        _verticalX - VSKIP,
                                                        _horizontalLines[ indexH ]->_lineCoords.y1 );
                    overlapNode = true;

                /*
                 * Case 2:
                 *  - other end of a node; start new line just below node
                 */
                } else if( _verticalLines[ indexV ]->getLineType() == TreeLine::Node
                           && overlapNode ) {
                    startX = _verticalX + VSKIP;
                    overlapNode = false;
                /*
                 * Case 3:
                 *      - met an ordinary type of line (tree ring connector or ptr)
                 *    so add new line segment ending just above the intersecting
                 *    line and start a new line just below it
                 */
                } else if( _verticalLines[ indexV ]->getLineType() != TreeLine::Node ) {
                    _horizontalLines[ indexH ]->addSeg( startX,
                                              _horizontalLines[ indexH ]->_lineCoords.y1,
                                              _verticalX - VSKIP,
                                              _horizontalLines[ indexH ]->_lineCoords.y1 );
                    startX = _verticalX + VSKIP;
                }
            }
        }

        if( startX != firstX ) {
            _horizontalLines[ indexH ]->addSeg( startX,
                                      _horizontalLines[ indexH ]->_lineCoords.y1,
                                      lastX,
                                      _horizontalLines[ indexH ]->_lineCoords.y1 );
        }
    }
}


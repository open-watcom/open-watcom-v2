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


#include "wmsgdlg.hpp"
#include "wstring.hpp"
#include "wkeydefs.hpp"
#include "winflist.hpp"

WEXPORT WInfiniteList::WInfiniteList( WWindow* prt, const WRect& r, char * t,
                            WObject * client, ocbloi fillcb, ccbo namecb,
                            uint loadSize, WStyle wstyle )
                : WWindow( prt, r, t, wstyle )
                ,_selected( -1 )
                ,_top( 0 )
                ,_loadSize( loadSize )
                ,_fillClient( client )
                ,_fill( fillcb )
                ,_name( namecb )
                ,_changedClient( NULL )
                ,_changed( NULL )
                ,_dblClickClient( NULL )
                ,_dblClick( NULL )
                ,_full( FALSE )
                ,_leftDown( FALSE )
{
    int nRows = getRows();

    setScrollTextRange( WScrollBarVertical, nRows * 2 );
    setScrollTextPos( WScrollBarVertical, nRows / 2 );
    load();
    show();
}

WInfiniteList::~WInfiniteList() {
/*******************************/

    _context.deleteContents();
}

bool WInfiniteList::gettingFocus( WWindow * ) {
/*********************************************/

    return FALSE;
}

/*
 * count -- return how many items are currently loaded
 */
int WInfiniteList::count() {
/***************************/

    return getLastIndex() + 1;
}

/*
 * reset -- kill current contents, then re-load
 */
void WInfiniteList::reset() {
/***************************/

    _selected = -1;
    _top = 0;
    _full = FALSE;
    _leftDown = FALSE;

    _context.deleteContents();
    invalidate();
}

/*
 * load -- load up an existing context, or load a new one
 *         and add it to the end
 */
void WInfiniteList::load( int contextIdx ) {
/***********************************************/

    WObject * prevCtxt;
    WObject * newCtxt;
    WVList *  newList;
    Context * myContext;

    if( contextIdx < 0  || contextIdx > _context.count() - 1 ) {
        newList = new WVList;
        if( _context.count() > 0 ) {
            myContext = (Context *)_context[ _context.count() - 1 ];
            prevCtxt = myContext->getContext();
        } else {
            myContext = new Context( NULL, NULL );
            _context.add( myContext );
            prevCtxt = NULL;
        }

        newCtxt = (_fillClient->*_fill)( newList, prevCtxt, _loadSize );

        myContext->reLoad( newList );
        if( newCtxt ) {
            _context.add( new Context( NULL, newCtxt ) );
        }
    } else {
        unLoadAllBut( contextIdx );

        myContext = (Context *)_context[ contextIdx ];

        if( myContext->isLoaded() ) return; //don't need to re-load

        newList = new WVList;
        prevCtxt = myContext->getContext();
        newCtxt = (_fillClient->*_fill)( newList, prevCtxt, _loadSize );

        myContext->reLoad( newList );
        if( contextIdx == _context.count() - 1  && newCtxt ) {
            _context.add( new Context( NULL, newCtxt ) );
        }
    }

    if( newCtxt == NULL ) {
        _full = TRUE;
        if( _context.count() > 0 ) {
            _extra = ((Context *)_context[ _context.count() - 1 ])->count();
        } else {
            _extra = 0;
        }
    }
}

void WInfiniteList::unLoadAllBut( int contextIdx ) {
/*******************************************************/

    #if 0   // has a bug, don't unload till fixed NYI ITB
    for( int i = 0; i < _context.count(); i += 1 ) {
        if( i < contextIdx - 1 || i > contextIdx + 1 ) {
            ((Context *)_context[ i ])->unLoad();
        }
    }
    #else
    contextIdx = contextIdx;
    #endif
}

WObject * WInfiniteList::getObject( int index ) {
/****************************************************/

    int ctxtIdx = index / _loadSize;
    int offset = index % _loadSize;
    Context *   ctxt;


    if( index < 0 ) {
        return NULL;
    }

    while( _context.count() <= ctxtIdx && !_full ) {
        load();
    }
    if( ctxtIdx >= _context.count() ) return NULL;

    load( ctxtIdx );
    ctxt = (Context *)_context[ ctxtIdx ];

    if( !ctxt->isLoaded() ) return NULL;        // happens when no items

    if( offset < ctxt->count() ) {
        return ctxt->getObject( offset );
    } else {
        return NULL;
    }
}

int WInfiniteList::getLastIndex() {
/**************************************/

    return (_context.count() - 1) * _loadSize + _extra - 1;
}

const char * WInfiniteList::getString( int index ) {
/*******************************************************/

    WObject * obj;
    obj = getObject( index );

    if( obj ) {
        return (_fillClient->*_name)( obj );
    } else {
        return NULL;
    }
}

bool WInfiniteList::paint() {
/***************************/

    const char *    str;
    int             maxRows = getRows() + _top;
    int             width;

    width = getScrollRange( WScrollBarHorizontal  ) * 2;

    for( int i = _top; i < maxRows; i += 1 ) {
        str = getString( i );

        if( str == NULL ) break;

        WString name( str );

        if( i == _selected ) {
            drawTextExtent( name, i - _top, 0,
                            ColorWhite, CUSTOM_RGB(0,0,128),
                            width );
        } else {
            drawTextExtent( name, i - _top, 0,
                            ColorBlack, ColorLiteGray,
                            width );
        }
    }

    return TRUE;
}

bool WInfiniteList::mouseMove( int x, int y, WMouseKeyFlags ) {

    if( _leftDown ) {
        int         row = getRow( WPoint( x, y ) );
        int         oldSel = _selected;

        if( row < 0 ) row = 0;
        if( row > getRows() - 1 ) {
            row = getRows() - 1;
        }

        row += _top;
        if( getObject( row ) == NULL ) {
            row = getLastIndex();
        }

        if( _selected != row ) {
            _selected = row;

            invalidateRow( oldSel - _top );
            invalidateRow( _selected - _top );
            scrollToSelected();
        }

        return TRUE;
    } else {
        return FALSE;
    }
}

bool WInfiniteList::leftBttnDn( int x, int y, WMouseKeyFlags f ) {
/********************************************************************/

    _leftDown = TRUE;

    mouseMove( x, y, f );

    return TRUE;
}

bool WInfiniteList::leftBttnUp( int x, int y, WMouseKeyFlags f ) {
/********************************************************************/

    mouseMove( x, y, f );
    _leftDown = FALSE;
    changed();

    return TRUE;
}

bool WInfiniteList::leftBttnDbl( int x, int y, WMouseKeyFlags ) {
/*******************************************************************/

    int      row = getRow( WPoint( x, y ) ) + _top;

    if( _selected == row ) {
        if( _dblClickClient && _dblClick ) {
            (_dblClickClient->*_dblClick)( this );
        }
    }

    return TRUE;
}

void WInfiniteList::onChanged( WObject* obj, cbw changed ) {
/**********************************************************/

    _changedClient = obj;
    _changed = changed;
}

void WInfiniteList::onDblClick( WObject* obj, cbw dblClick ) {
/************************************************************/

    _dblClickClient = obj;
    _dblClick = dblClick;
}

WObject * WInfiniteList::selected( void ) {
/*****************************************/

    return getObject( _selected );
}

void WInfiniteList::scrollToSelected() {
/**************************************/

    int nRows = getRows();

    if( _selected < _top ) {
        performScroll( _selected, TRUE );
    }
    if( _selected > _top + nRows - 1 ) {
        performScroll( _selected - nRows + 1, TRUE );
    }
}

void WInfiniteList::changed() {
/*****************************/

    if( _changedClient && _changed ) {
        (_changedClient->*_changed)( this );
    }
}

bool WInfiniteList::keyDown( WKeyCode key, WKeyState state ) {
/************************************************************/

    int nRows = getRows();
    int oldSel = _selected;

    if( state == WKeyStateNone ) {
        switch( key ) {
        case WKeyPageup:
            _selected -= nRows - 1;
            if( _selected < 0 ) _selected = 0;

            if( oldSel != _selected ) {
                invalidateRow( oldSel - _top );
                invalidateRow( _selected - _top );
            }
            scrollToSelected();
            changed();
            return TRUE;

        case WKeyPagedown:
            _selected += nRows - 1;
            if( getObject( _selected ) == NULL ) {
                 _selected = getLastIndex();
            }

            if( oldSel != _selected ) {
                invalidateRow( oldSel - _top );
                invalidateRow( _selected - _top );
            }
            scrollToSelected();
            changed();
            return TRUE;

        case WKeyUp:
            _selected -= 1;
            if( _selected < 0 ) _selected = 0;

            scrollToSelected();
            changed();

            if( oldSel != _selected ) {
                invalidateRow( oldSel - _top );
                invalidateRow( _selected - _top );
            }
            return TRUE;

        case WKeyDown:
            _selected += 1;
            if( getObject( _selected ) == NULL ) {
                 _selected = getLastIndex();
            }

            scrollToSelected();
            changed();

            if( oldSel != _selected ) {
                invalidateRow( oldSel - _top );
                invalidateRow( _selected - _top );
            }
            return TRUE;

        case WKeyEnter:
            if( _dblClickClient && _dblClick ) {
                (_dblClickClient->*_dblClick)( this );
            }
            return TRUE;

        default:
            return FALSE;
        }
    }

    return FALSE;
}

void WInfiniteList::performScroll( int pos, bool absolute ) {
/************************************************************/

    int    diff;
    int     nRows = getRows();

    if( absolute ) {
        diff = pos - _top;
    } else {
        diff = pos;
    }

    if( _top + diff < 0 ) {     // it's important this appears twice
        diff = -1 * _top;
    }
    if( getObject( _top + diff ) == NULL ) {
        diff = getLastIndex() - nRows + 1 - _top;
    }
    if( _full && _top + diff > getLastIndex() - nRows + 1 ) {
        diff = getLastIndex() - nRows + 1 - _top;
    }
    if( _top + diff < 0 ) {
        diff = -1 * _top;
    }

    _top += diff;

    scrollWindow( WScrollBarVertical, diff );
}

bool WInfiniteList::scrollNotify( WScrollNotification sn, int ) {
/***************************************************************/

    switch( sn ) {
        case WScrollUp:
            performScroll( -1 );
            return TRUE;

        case WScrollPageUp:
            performScroll( -1 * getRows() + 1 );
            return TRUE;

        case WScrollDown:
            performScroll( 1 );
            return TRUE;

        case WScrollPageDown:
            performScroll( getRows() - 1 );
            return TRUE;
    }

    return FALSE;
}

WInfiniteList::Context::Context(WVList * l, WObject * c )
                            : _items( l )
                            ,_context( c ) {}

WInfiniteList::Context::~Context() {
/*******************************/

    if( _items ) {
        _items->deleteContents();
        delete _items;
    }
    delete _context;
}

void WInfiniteList::Context::unLoad() {
/*************************************/

    if( _items ) {
        _items->deleteContents();
        delete _items;
        _items = NULL;
    }
}

int WInfiniteList::Context::count() {
/***********************************/

    if( _items ) {
        return _items->count();
    } else {
        return 0;
    }
}

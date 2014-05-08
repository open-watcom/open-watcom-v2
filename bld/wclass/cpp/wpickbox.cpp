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


#include "wpickbox.hpp"
#include "wmetrics.hpp"

#define _model ((WPickList*)model())


WEXPORT WPickBox::WPickBox( WPickList &plist, cbs callback, WWindow *win,
                            const WRect &r, WStyle style )
    : WListBox( win, r, (style & ~LStyleSorted) )
    , WView( &plist )
    , _nameCallback( callback )
    , _sorted( (style & LStyleSorted) != 0 ) {
/********************************************/

    fillBox();
}


WEXPORT WPickBox::WPickBox( WPickList &plist, cbs callback, WWindow *win,
                            const WPoint &p, WStyle style )
    : WListBox( win, WRect( p.x(), p.y(), 0, 0 ), (style & ~LStyleSorted) )
    , WView( &plist )
    , _nameCallback( callback )
    , _sorted( (style & LStyleSorted) != 0 ) {
/********************************************/

    WPoint avg;
    WPoint max;
    textMetrics( avg, max );

    int w = fillBox() + max.x() + WSystemMetrics::vScrollBarWidth();
    int h = (_model->count() + 1) * max.y() + WSystemMetrics::hScrollBarHeight();

    WRect screen;
    WSystemMetrics::screenCoordinates( screen );
    if( w > screen.w() / 2 ) {
        w = screen.w() / 2;
    }
    if( h > screen.h() / 2 ) {
        h = screen.h() / 2;
    }
    move( WRect( p.x(), p.y(), w, h ) );
}


void WEXPORT WPickBox::name( int index, WString &str ) {
/******************************************************/

    if( _nameCallback != NULL ) {
        (((*_model)[index])->*_nameCallback)( str );
    } else {
        str = "";
    }
}


int WEXPORT WPickBox::fillBox() {
/*******************************/

    int         max_extent = 0;
    int         curr_extent;

    setUpdates( false );
    if( _sorted ) {
        _model->sort();
    }
    int icount = _model->count();
    for( int i=0; i<icount; i++ ) {
        WString n;
        name( i, n );
        int index = insertString( n );
        setTagPtr( index, (*_model)[i] );
        curr_extent = getTextExtentX( n );
        if( curr_extent > max_extent ) {
            max_extent = curr_extent;
        }
    }
    setExtent( max_extent );
    setUpdates( true );
    return( max_extent );
}


void * WEXPORT WPickBox::selectedTagPtr() {
/*****************************************/

    int index = selected();
    if( index >= 0 ) {
        return( tagPtr( index ) );
    }
    return( NULL );
}


void WEXPORT WPickBox::updateView() {
/***********************************/

    if( _model != NULL ) {
        int top = topIndex();
        int cur = selected();
        reset();
        fillBox();
        setTopIndex( top );
        select( cur );
    }
}


void WEXPORT WPickBox::modelGone() {
/**********************************/

    reset();
}

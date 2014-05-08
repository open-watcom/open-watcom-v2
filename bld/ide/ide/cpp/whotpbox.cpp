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


#include "whotpbox.hpp"

#define _model ((WPickList*)model())


WEXPORT WHotPickBox::WHotPickBox( WPickList &plist, cbs gname, icb gindex, bcb gincluded,
        WWindow *win, const WRect& r, WHotSpots* hs )
/********************************************/
    : WHotSpotList( win, r, NULL, hs )
    , WView( &plist )
    , _gname( gname )
    , _gindex( gindex )
    , _gincluded( gincluded )
    , _sorted( true )
    , _tags( NULL )
    , _tagsCount( 0 )
    , _tagsWidth( 0 )
{
    fillBox();
}

WEXPORT WHotPickBox::~WHotPickBox()
/*********************************/
{
    delete _tags;
}

void WEXPORT WHotPickBox::name( int index, WString &str )
/******************************************************/
{
    if( _gname != NULL ) {
        (((WObject*)_tags[index])->*_gname)( str );
    } else {
        str = "";
    }
}


bool WHotPickBox::gettingFocus( WWindow * )
/*****************************************/
{
    parent()->setFocus();
    return( true );
}

void WEXPORT WHotPickBox::fillBox()
/********************************/
{
    int i;

    if( _sorted ) {
        _model->sort();
    }
    delete _tags;
    _tagsCount = 0;
    _tagsWidth = 0;
    int icount = _model->count();
    int jcount = 0;
    for( i=0; i<icount; i++ ) {
        if( (((*_model)[i])->*_gincluded)() ) {
            jcount += 1;
        }
    }
    _tags= new void*[jcount];
    _tagsCount = jcount;
    int maxWid = 0;
    int j = 0;
    for( i=0; i<icount; i++ ) {
        if( (((*_model)[i])->*_gincluded)() ) {
            _tags[j] = (*_model)[i];
            WString s; name( j, s );
            int wid = getTextExtentX( s );
            if( wid > maxWid ) maxWid = wid;
            j += 1;
        }
    }
    _tagsWidth = maxWid;
    reset();
}

void* WEXPORT WHotPickBox::tagPtr( int index )
/********************************************/
{
    if( _tags && index < _tagsCount ) {
        return( _tags[index] );
    }
    return( NULL );
}

void WEXPORT WHotPickBox::setTagPtr( int index, void* ptr )
/********************************************/
{
    if( _tags && index < _tagsCount ) {
        _tags[index] = ptr;
    }
}

void * WEXPORT WHotPickBox::selectedTagPtr()
/*****************************************/
{
    int index = selected();
    if( index >= 0 ) {
        return( tagPtr( index ) );
    }
    return( NULL );
}

void WEXPORT WHotPickBox::selectSameTag( void* tag )
{
    for( int i=0; i<_tagsCount; i++ ) {
        if( _tags[i] == tag ) {
            select( i );
            reset();
            break;
        }
    }
}


void WEXPORT WHotPickBox::updateView()
/***********************************/
{
    if( _model ) {
        int top = topIndex();
        int cur = selected();

        fillBox();
        setTopIndex( top );

        if( cur > 0 ) {
            int cnt = count()-1;
            if( cur > cnt ) {
                cur = cnt;
            }
        }
        select( cur );
    }
}


void WEXPORT WHotPickBox::modelGone()
/**********************************/
{
    reset();
}

int WEXPORT WHotPickBox::count()
/********************************************/
{
    if( _model ) {
        return( _tagsCount );
    }
    return( 0 );
}

void WEXPORT WHotPickBox::setTopIndex( int top )
/********************************************/
{
    performScroll( top, true );
}

const char * WEXPORT WHotPickBox::getString( int index )
/********************************************/
{
    static WString n;
    name( index, n );
    return( n );
}

int WEXPORT WHotPickBox::getHotOffset( int /*index*/ )
{
    return( 0 );
}

int WEXPORT WHotPickBox::getHotSpot( int index, bool /*pressed*/ )
/********************************************/
{
    if( _gindex ) {
        return( (((WObject*)_tags[index])->*_gindex)() );
    }
    return( 0 );
}

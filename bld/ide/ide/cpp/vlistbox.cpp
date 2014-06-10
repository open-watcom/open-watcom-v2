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


#include "vlistbox.hpp"

VListBox::VListBox( WWindow *parent, const WRect& r )
    : WHotSpotList( parent, r, NULL, NULL ),
      _maxWidth(0) {
}

WEXPORT VListBox::~VListBox() {
    _strings.deleteContents();
}

void VListBox::insertString( const char *str ) {
    WString     *tmp;
    int         len;

    tmp = new WString( str );
    _strings.add( tmp );
    len = getTextExtentX( *tmp, tmp->size() );
    if( len > _maxWidth ) _maxWidth = len;
    adjustScrollBars();
//    update();
    performScroll( count(), true );
}

bool VListBox::gettingFocus( WWindow* ) {
    parent()->setFocus();
    return( true );
}

const char *VListBox::getString( int index ) {
    WString     *str;
    str = (WString *) _strings[index];
    return( str->gets() );
}

void VListBox::deleteString( int index ) {
    delete _strings[index];
    _strings.removeAt( index );
    update();
}

void VListBox::resetContents( void ) {
    _strings.deleteContents();
    reset();
}

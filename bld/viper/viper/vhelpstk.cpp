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


#include <string.h>
#include "vhelpstk.hpp"

#define ALLOC_INCREMENT         20

VHelpStack      HelpStack;

VHelpStack::VHelpStack() {
    _stack = NULL;
    _allocated = 0;
    _used = 0;
}

VHelpStack::~VHelpStack() {
    if( _stack != NULL ) delete [] _stack;
    _allocated = 0;
    _used = 0;
}

void VHelpStack::push( unsigned long item ) {
    unsigned long       *tmp;

    _used++;
    if( _used >= _allocated ) {
        _allocated += ALLOC_INCREMENT;
        tmp = new unsigned long [_allocated];
        memcpy( tmp, _stack, ( _used - 1 ) * sizeof( unsigned long ) );
        delete [] _stack;
        _stack = tmp;
    }
    _stack[ _used - 1 ] = item;
}

unsigned long VHelpStack::pop( void ) {
    if( isempty() ) {
        return( 0 );
    } else {
        _used--;
        return( _stack[ _used ] );
    }
}

unsigned long VHelpStack::getTop( void ) {
    if( isempty() ) {
        return( 0 );
    } else {
        return( _stack[ _used - 1 ] );
    }
}

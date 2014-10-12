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


/*
CTXOMAP:  Context-number-to-topic-offset mapping.
*/

#include "ctxomap.h"
#include "hcerrors.h"
#include <stdlib.h> // for ltoa()


//  HFCtxomap::CmapRec::CmapRec

HFCtxomap::CmapRec::CmapRec( uint_32 num, uint_32 h_val )
    : _mapnum( num ),
      _offset( h_val ),
      _nextRec( NULL )
{
    // empty
}


//  HFCtxomap::CmapRec::dump    --Overrides Dumpable::dump.

int HFCtxomap::CmapRec::dump( OutFile * dest )
{
    dest->write( _mapnum );
    dest->write( _offset );
    return 1;
}


//  HFCtxomap::HFCtxomap

HFCtxomap::HFCtxomap( HFSDirectory *d_file, HFContext *offsets )
    : _numRecords( 0 ),
      _firstRec( NULL ),
      _lastRec( NULL ),
      _size( sizeof( uint_16 ) ),   // size of _numRecords
      _offsetFile( offsets ),
      _resolved( false )
{
    d_file->addFile( this, "|CTXOMAP" );
}


//  HFCtxomap::~HFCtxomap

HFCtxomap::~HFCtxomap()
{
    CmapRec *current;
    CmapRec *next;
    for( current = _firstRec; current != NULL; current = next ) {
        next = current->_nextRec;
        delete current;
    }
}


//  HFCtxomap::addMapRec    --Add a context map record.

void HFCtxomap::addMapRec( uint_32 num, uint_32 h_val )
{
    CmapRec *newrec = new CmapRec( num, h_val );
    if( _firstRec == NULL ) {
        _firstRec = _lastRec = newrec;
    } else {
        _lastRec->_nextRec = newrec;
        _lastRec = newrec;
    }
    _numRecords += 1;
    _size += 2*sizeof( uint_32 );
}


//  HFCtxomap::size --Overrides Dumpable::size.

uint_32 HFCtxomap::size()
{
    CmapRec *current;
    uint_32 true_offset;

    if( !_resolved ) {
        for( current = _firstRec; current != NULL; current = current->_nextRec ) {
            true_offset = _offsetFile->getOffset( current->_offset );
            if( true_offset == HFContext::_badValue ) {
                char str[16];
                sprintf( str, "MAP number %ld", (long)current->_mapnum );
                HCWarning( HLP_NOTOPIC, (const char *)str );
            } else {
                current->_offset = true_offset;
            }
        }
        _resolved = true;
    }
    return _size;
}


//  HFCtxomap::dump --Overrides Dumpable::dump.

int HFCtxomap::dump( OutFile * dest )
{
    dest->write( _numRecords );
    CmapRec *current;
    for( current = _firstRec; current != NULL; current = current->_nextRec ) {
        current->dump( dest );
    }
    return 1;
}

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
    dest->writebuf( &_mapnum, sizeof( uint_32 ), 1 );
    dest->writebuf( &_offset, sizeof( uint_32 ), 1 );
    return 1;
}


//  HFCtxomap::HFCtxomap

HFCtxomap::HFCtxomap( HFSDirectory *d_file, HFContext *offsets )
    : _numRecords( 0 ),
      _firstRec( NULL ),
      _lastRec( NULL ),
      _size( sizeof( uint_16 ) ),   // size of _numRecords
      _offsetFile( offsets ),
      _resolved( 0 )
{
    d_file->addFile( this, "|CTXOMAP" );
}


//  HFCtxomap::~HFCtxomap

HFCtxomap::~HFCtxomap()
{
    CmapRec *current = _firstRec;
    CmapRec *temp;
    while( current != NULL ){
    temp = current;
    current = current->_nextRec;
    delete temp;
    }
}


//  HFCtxomap::addMapRec    --Add a context map record.

void HFCtxomap::addMapRec( uint_32 num, uint_32 h_val )
{
    CmapRec *newrec = new CmapRec( num, h_val );
    if( _firstRec == NULL ){
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
    CmapRec *current = _firstRec;
    uint_32 true_offset;
    if( !_resolved ){
    while( current != NULL ){
        true_offset = _offsetFile->getOffset( current->_offset );
        if( true_offset == HFContext::_badValue ){
        char str[16] = "MAP number ";
        ltoa( current->_mapnum, str+11, 10 );
        HCWarning( HLP_NOTOPIC, (const char *) str );
        } else {
        current->_offset = true_offset;
        }
        current = current->_nextRec;
    }
    _resolved = 1;
    }
    return _size;
}


//  HFCtxomap::dump --Overrides Dumpable::dump.

int HFCtxomap::dump( OutFile * dest )
{
    dest->writebuf( &_numRecords, sizeof( uint_16 ), 1 );
    CmapRec *current = _firstRec;
    while( current != NULL ){
    current->dump( dest );
    current = current->_nextRec;
    }
    return 1;
}

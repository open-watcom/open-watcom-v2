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
TTLBTREE:  topic-offset-to-topic-title mapping
*/

#include "ttlbtree.h"
#include <string.h>


//
//  TTLKey  --"Key" data type for class HFTtlbtree.
//

class TTLKey : public BtreeData
{
protected:
    uint_32     _offset;

    // Overriding the BtreeData virtual functions.
    BtreeData       *myKey();
    int         lessThan( BtreeData *other );
    virtual uint_32 size();
    virtual int     dump( OutFile * dest );

    TTLKey( uint_32 off ) : _offset( off ) {};
};


//
//  TTLRec  --"Record" data type for class HFTtlbtree.
//

class TTLRec : public TTLKey
{
    char    *_title;

    // Overriding the TTLKey virtual functions.
    uint_32 size();
    int     dump( OutFile * dest );

    TTLRec( TTLRec const & ) : TTLKey(0) {};
    TTLRec &    operator=( TTLRec const & ) { return *this; };

public:
    TTLRec( uint_32 off, char const string[] );
    ~TTLRec();
};


//  TTLKey::myKey   --Overrides BtreeData::myKey.

BtreeData *TTLKey::myKey()
{
    return new TTLKey( _offset );
}


//  TTLKey::lessThan    --Overrides BtreeData::lessThan.

int TTLKey::lessThan( BtreeData * other )
{
    TTLKey  *trueother = (TTLKey*) other;
    return _offset < trueother->_offset;
}


//  TTLKey::size

uint_32 TTLKey::size()
{
    return sizeof( uint_32 );
}


//  TTLKey::dump

int TTLKey::dump( OutFile * dest )
{
    dest->write( _offset );
    return 1;
}


//  TTLRec::TTLRec

TTLRec::TTLRec( uint_32 off, char const string[] )
    : TTLKey( off )
{
    size_t len = strlen( string ) + 1;
    _title = new char[len];
    strncpy( _title, string, len );
}


//  TTLRec::~TTLRec

TTLRec::~TTLRec()
{
    delete[] _title;
}


//  TTLRec::size

uint_32 TTLRec::size()
{
    size_t result = 5 + strlen( _title );
    return result;
}


//  TTLRec::dump

int TTLRec::dump( OutFile * dest )
{
    dest->write( _offset );
    dest->write( _title, 1, strlen( _title ) + 1 );
    return 1;
}


char const HFTtlbtree::_titleMagic[Btree::_magNumSize] = {
    0x3B, 0x29, 0x02, 0x00, 0x00,
    0x08, 0x4C, 0x7A, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};


//  HFTtlbtree::HFTtlbtree

HFTtlbtree::HFTtlbtree( HFSDirectory * d_file )
{
    _titles = new Btree( _titleMagic );
    d_file->addFile( this, "|TTLBTREE" );
}


//  HFTtlbtree::~HFTtlbtree

HFTtlbtree::~HFTtlbtree()
{
    delete _titles;
}


//  HFTtlbtree::addTitle

void HFTtlbtree::addTitle( uint_32 offset, char const title[] )
{
    TTLRec  *newrec = new TTLRec( offset, title );
    _titles->insert( newrec );
}

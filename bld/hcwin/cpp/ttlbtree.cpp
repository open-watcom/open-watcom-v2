/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
    uint_32         _offset;

    // Overriding the BtreeData virtual functions.
    BtreeData       *myKey();
    bool            lessThan( BtreeData *other );
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

bool TTLKey::lessThan( BtreeData * other )
{
    TTLKey  *trueother = (TTLKey*) other;
    return( _offset < trueother->_offset );
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
    _title = new char[strlen( string ) + 1];
    strcpy( _title, string );
}


//  TTLRec::~TTLRec

TTLRec::~TTLRec()
{
    delete[] _title;
}


//  TTLRec::size

uint_32 TTLRec::size()
{
    return( (uint_32)( 5 + strlen( _title ) ) );
}


//  TTLRec::dump

int TTLRec::dump( OutFile * dest )
{
    dest->write( _offset );
    dest->write( _title );
    return 1;
}


//  HFTtlbtree::HFTtlbtree

HFTtlbtree::HFTtlbtree( HFSDirectory * d_file )
{
    _titles = new Btree( false, "Lz" );
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

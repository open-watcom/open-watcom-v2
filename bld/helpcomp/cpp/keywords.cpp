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
KEYWORDS:  Keyword searching support
*/

#include "keywords.h"
#include <ctype.h>      // for isalnum()


//
//  KWoffset    --Linked-list node to represent offsets into
//                the keyword files.
//

struct KWoffset
{
    uint_32     _offset;
    KWoffset    *_next, *_prev;
    KWoffset( uint_32 off );
};


//
//  KWKey               --"key" data type for the class HFKwbtree.
//

class KWKey : public BtreeData
{
    KWKey( KWKey const & ){};
    KWKey &     operator=( KWKey const & ){ return *this; };

protected:
    char        *_keyword;

    // Override the BtreeData virtual functions.
    BtreeData           *myKey();
    int                 lessThan( BtreeData * other );
    virtual uint_32     size();
    virtual int         dump( OutFile * dest );

public:
    KWKey( char const kword[] );
    ~KWKey();
};


//
//  KWRec               --"record" data type for the class HFKwbtree.
//

class KWRec : public KWKey
{
    KWoffset    *_head;
    uint_16     _count;
    uint_32     _dataOffset;

    uint_32     size();
    int         dump( OutFile * dest );

    KWRec( KWRec const & ) : KWKey("") {};
    KWRec &     operator=( KWRec const & ){ return *this; };

public:
    KWRec( char const kword[], uint_32 first_off );
    ~KWRec();

    KWoffset    *head() { return _head; };
    uint_16     count() { return _count; };
    void addOffset( uint_32 new_off );

    friend class HFKwbtree;     // for access to _dataOffset;
};


//  KWoffset::KWoffset

inline KWoffset::KWoffset( uint_32 off )
        : _offset( off ),
          _next( NULL ),
          _prev( NULL )
{
    // empty
}


//  KWKey::KWKey

KWKey::KWKey( char const kword[] )
{
    int len = strlen( kword ) + 1;
    _keyword = new char[ len ];
    strcpy( _keyword, kword );
}


//  KWKey::~KWKey

KWKey::~KWKey()
{
    delete[] _keyword;
}


//  KWKey::myKey        --Overrides BtreeData::myKey.

BtreeData *KWKey::myKey()
{
    return new KWKey( _keyword );
}


//  KWKey::lessThan     --Overrides BtreeData::lessThan.

int KWKey::lessThan( BtreeData * other )
{
    KWKey *trueother = (KWKey*) other;
    char *pleft = _keyword;
    char *pright = trueother->_keyword;
    char left, right;

    // The following is essentially stricmp, rewritten
    // to make alphanumeric characters "greater" than other chars.

    do {
        left = *pleft++; right = *pright++;
        if( left >= 'A' && left <= 'Z' ) left += 'a' - 'A';
        if( right >= 'A' && right <= 'Z' ) right += 'a' - 'A';
    } while( left == right && left != '\0' );

    int result = left < right;
    if( isalnum( left ) && !isalnum( right ) ){
        result = 0;
    } else if( !isalnum( left ) && isalnum( right ) ){
        result = 1;
    }

    return result;
}


//  KWKey::size

uint_32 KWKey::size()
{
    return (uint_32) strlen( _keyword )+1;
}


//  KWKey::dump

int KWKey::dump( OutFile * dest )
{
    dest->writebuf( _keyword, 1, strlen( _keyword )+1 );
    return 1;
}


//  KWRec::KWRec

KWRec::KWRec( char const kword[], uint_32 first_off )
        : KWKey( kword ),
          _count( 0 ),
          _head( NULL )
{
    addOffset( first_off );
}


//  KWRec::~KWRec

KWRec::~KWRec()
{
    KWoffset    *current = _head;
    KWoffset    *temp;
    while( current != NULL ){
        temp = current;
        current = current->_next;
        delete temp;
    }
}


//  KWRec::size

uint_32 KWRec::size()
{
    uint_32 len = strlen( _keyword )+1+sizeof( uint_16 )+sizeof( uint_32 );
    return len;
}


//  KWRec::dump

int KWRec::dump( OutFile * dest )
{
    int len = strlen( _keyword ) + 1;
    dest->writebuf( _keyword, 1, len );
    dest->writebuf( &_count, sizeof( uint_16 ), 1 );
    dest->writebuf( &_dataOffset, sizeof( uint_32 ), 1 );
    return 1;
}


//  KWRec::addOffset    --Add a new topic offset to this keyword.

void KWRec::addOffset( uint_32 new_off )
{
    KWoffset    *new_node = new KWoffset( new_off );
    KWoffset    *current = _head;
    while( current != NULL ){
        if( current->_offset > new_off ) break;
        if( current->_next == NULL ) break;
        current = current->_next;
    }
    if( current == NULL ){
        _head = new_node;
    } else if( current->_offset > new_off ){
        new_node->_next = current;
        new_node->_prev = current->_prev;
        if( current->_prev != NULL ){
            current->_prev->_next = new_node;
        } else {
            _head = new_node;
        }
        current->_prev = new_node;
    } else {
        new_node->_prev = current;
        current->_next = new_node;
    }
    _count += 1;
}


char const HFKwbtree::_keyMagic[Btree::_magNumSize] = {
                                        0x3B, 0x29, 0x02, 0x00, 0x00,
                                        0x08, 0x69, 0x32, 0x34, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00
};


//  HFKwbtree::HFKwbtree

HFKwbtree::HFKwbtree( HFSDirectory * d_file )
{
    _words = new Btree( _keyMagic );
    _dataFile = new HFKwdata( d_file, this );
    _mapFile = new HFKwmap( d_file, this );
    _haveSetOffsets = 0;

    d_file->addFile( this, "|KWBTREE" );
}


//  HFKwbtree::~HFKwbtree

HFKwbtree::~HFKwbtree()
{
    delete _words;
    delete _dataFile;
    delete _mapFile;
}


//  HFKwbtree::size     --Overrides Dumpable::size.

uint_32 HFKwbtree::size()
{
    if( !_haveSetOffsets ){
        BtreeIter   _iterator( *_words );
        uint_32     offset = 0;
        KWRec       *record;

        // Use an iterator to add up the sizes in the tree.
        for( ;; ){
            record = (KWRec*) _iterator.data();
            if( record == NULL ) break;
            record->_dataOffset = offset;
            offset += sizeof( uint_32 )*record->_count;
            _iterator++;
        }
        _haveSetOffsets = 1;
    }
    return _words->size();
}


//  HFKwbtree::dump     --Overrides Dumpable::dump.

int HFKwbtree::dump( OutFile * dest )
{
    return _words->dump( dest );
}


//  HFKwbtree::addKW    --Add a keyword to the keyword list.

void HFKwbtree::addKW( char const keyword[], uint_32 offset )
{
    KWKey       temp_key( keyword );
    KWRec       *found_rec = (KWRec*) _words->findNode( temp_key );

    if( found_rec == NULL ){
        found_rec = new KWRec( keyword, offset );
        _words->insert( found_rec );
    } else {
        found_rec->addOffset( offset );
    }
}


//  HFKwdata::HFKwdata

HFKwdata::HFKwdata( HFSDirectory * d_file, HFKwbtree * tree )
        : _iterator( *tree->words() ),
          _myTree( tree ),
          _size( 0 )
{
    d_file->addFile( this, "|KWDATA" );
}


//  HFKwdata::size      --Overrides Dumpable::size.

uint_32 HFKwdata::size()
{
    if( _size == 0 ){
        for( _iterator.init(); _iterator.data()!=NULL; _iterator++ ){
            _size += ( (KWRec*) _iterator.data() )->count() * sizeof( uint_32 );
        }
    }
    return _size;
}


//  HFKwdata::dump      --Overrides Dumpable::dump.

int HFKwdata::dump( OutFile * dest )
{
    KWRec       *record;
    KWoffset    *p_off;
    for( _iterator.init(); _iterator.data() != NULL; _iterator++ ){
        record = (KWRec*) _iterator.data();
        for( p_off = record->head(); p_off!=NULL; p_off = p_off->_next ){
            dest->writebuf( &p_off->_offset, sizeof( uint_32 ), 1 );
        }
    }
    return 1;
}


//  HFKwmap::HFKwmap

HFKwmap::HFKwmap( HFSDirectory * d_file, HFKwbtree * tree )
        : _iterator( *tree->words() ),
          _myTree( tree ),
          _numRecs( 0 )
{
    d_file->addFile( this, "|KWMAP" );
}


//  HFKwmap::size       --Overrides Dumpable::size.

uint_32 HFKwmap::size()
{
    if( _numRecs == 0 ){
        _iterator.init();
        while( _iterator.data() != NULL ){
            _numRecs += 1;
            _iterator.nextPage();
        }
    }

    // Note each KWmap record is 6 bytes long.
    return (uint_32) (_numRecs*6)+sizeof( uint_16 );
}


//  HFKwmap::dump       --Overrides Dumpable::dump.

int HFKwmap::dump( OutFile * dest )
{
    dest->writebuf( &_numRecs, sizeof( uint_16 ), 1 );
    _iterator.init();
    uint_32     rec_count = 0;
    uint_16     page_num;
    while( _iterator.data() != NULL ){
        page_num = _iterator.thisPage();
        dest->writebuf( &rec_count, sizeof( uint_32 ), 1 );
        dest->writebuf( &page_num, sizeof( uint_16 ), 1 );
        rec_count += _iterator.pageEntries();
        _iterator.nextPage();
    }
    return 1;
}

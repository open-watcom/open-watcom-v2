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


#include "wpch.hpp"
#include "strtable.hpp"
#include "cache.hpp"

static Pool StringTable::Link::linkPool( StrLinkPool, sizeof(StringTable::Link) );

static int const        BUF_SIZE = 0x100;
static int const        NUM_BUCKETS = 251;      // A nice prime number.

StringTable::StringTable()
/************************/
{
    _table = new Link *[NUM_BUCKETS];
    _charBuffer = new char[BUF_SIZE];
    _bufSize = BUF_SIZE;
    _bufTop = 0;
    memset( _table, 0, NUM_BUCKETS*sizeof(Link *));
    _count = 0;
}


StringTable::~StringTable()
/************************/
{
    Link *      prev;
    Link *      curr;
    int         i;

    for( i=0; i<NUM_BUCKETS; i++ ){
        prev = NULL;
        curr = _table[i];
        while( curr != NULL ){
            prev = curr;
            curr = curr->next;
            delete prev;
        }
    }
    delete[] _table;
    delete[] _charBuffer;
}


static uint_32 const bmask[5] = {
    0x00000000,
    0x000000ff,
    0x0000ffff,
    0x00ffffff,
    0xffffffff,
};


int StringTable::Hash( char const *string, int len )
/**************************************************/
{
    uint_32 *s = (uint_32*) string;
    uint_32 mask;
    uint_32 c;
    uint_32 g;
    uint_32 h;

    h = len;
    c = len;
    if( len > sizeof( uint_32 ) ) {
        do {
            c += *s;
            h = ( h << 4 ) + c;
            g = h & ~0x0ffffff;
            h ^= g;
            h ^= g >> (4+4+4+4+4);
            ++s;
            len -= sizeof( uint_32 );
        } while( len > sizeof( uint_32 ) );
    }
    mask = bmask[ len ];
    c += *s & mask;
    h = ( h << 4 ) + c;
    g = h & ~0x0ffffff;
    h ^= g;
    h ^= g >> (4+4+4+4+4);
    g = h & ~0x0fff;
    h ^= g;
    h ^= g >> (4+4+4);
    h ^= h >> (2+4);
    WAssert( ( h & ~0x0fff ) == 0 );
    return( (int) (h%NUM_BUCKETS) );
}


BRI_StringID StringTable::Insert( char const *string )
/****************************************************/
{
    Link *              current;
    Link *              prev;
    Link *              newLink;
    int                 len;
    int                 index;
    int                 comparison;
    BRI_StringID        result;

    if( string == NULL ){
        return (BRI_StringID) 0;
    }

    len = strlen(string)+1;
    index = Hash( string, len );

    current = _table[ index ];
    prev = NULL;
    while( current != NULL ){
        comparison = strcmp( _charBuffer + current->offset, string );
        if( comparison >= 0 ){
            break;
        }
        prev = current;
        current = current->next;
    }
    if( current != NULL && comparison == 0 ){
        result = current->id;
    } else {
        newLink = new Link;
        if( len + _bufTop > _bufSize ){
            int         newSize;
            char        *temp;

            newSize = 2*_bufSize;
            if( newSize < len + _bufTop ){
                newSize = len + _bufTop;
            }
            temp = new char[newSize];
            memcpy( temp, _charBuffer, _bufSize );
            delete[] _charBuffer;
            _charBuffer = temp;
            _bufSize = newSize;
        }
        newLink->offset = _bufTop;
        memcpy( &_charBuffer[_bufTop], string, len );
        _bufTop += len;
        newLink->next = current;
        if( prev != NULL ){
            prev->next = newLink;
        } else {
            _table[ index ] = newLink;
        }
        result = (BRI_StringID) (_count*NUM_BUCKETS + index);
        newLink->id = result;
        _count++;
    }

    return result;
}


char const * StringTable::Lookup( BRI_StringID id )
/*************************************************/
{
    Link        *current;
    int         index;

    index = id % NUM_BUCKETS;
    current = _table[index];
    while( current != NULL && current->id != id ){
        current = current->next;
    }
    if( current == NULL ){
        return NULL;
    } else {
        return _charBuffer + current->offset;
    }
}


BRI_StringID StringTable::Lookup( char const *str )
/*************************************************/
{
    Link *              current;
    int                 len;
    int                 index;
    int                 comparison;
    BRI_StringID        result;

    if( str == NULL ){
        return (BRI_StringID) 0;
    }

    len = strlen(str)+1;
    index = Hash( str, len );

    current = _table[ index ];
    while( current != NULL ){
        comparison = strcmp( _charBuffer + current->offset, str );
        if( comparison >= 0 ){
            break;
        }
        current = current->next;
    }
    if( current != NULL && comparison == 0 ){
        result = current->id;
    } else {
        result = (BRI_StringID) 0;
    }

    return result;
}


WBool StringTable::SaveTo( CacheOutFile *cache )
/********************************************/
{
    Link        *current;
    int         i;

    cache->StartComponent( "Strings" );
    cache->AddDword( _bufTop );
    cache->AddData( _charBuffer, _bufTop );
    cache->AddDword( _count );
    for( i=0; i<NUM_BUCKETS; i++ ){
        current = _table[i];
        while( current != NULL ){
            cache->AddDword( current->id );
            cache->AddDword( current->offset );
            current = current->next;
        }
    }
    cache->EndComponent();
    return TRUE;
}


WBool StringTable::LoadFrom( CacheInFile *cache )
/*********************************************/
{
    WBool       result;
    Link        *current;
    Link        *prev;
    Link        *newLink;
    int         index;
    int         comparison;
    int         strCount;
    char        *start;
    int         i;

    result = cache->OpenComponent( "Strings" );
    if( !result ){
        return result;
    }
    for( i=0; i<NUM_BUCKETS; i++ ){
        current = _table[i];
        while( current != NULL ){
            prev = current;
            current = current->next;
            delete prev;
        }
        _table[i] = NULL;
    }
    cache->ReadDword( &_bufTop );
    if( _bufTop > _bufSize ){
        delete[] _charBuffer;
        _charBuffer = new char[_bufTop];
        _bufSize = _bufTop;
    }
    cache->ReadData( _charBuffer, _bufTop );
    cache->ReadDword( &strCount );
    _count = strCount;
    while( strCount > 0 ){
        newLink = new Link;
        cache->ReadDword( &newLink->id );
        cache->ReadDword( &newLink->offset );
        start = _charBuffer + newLink->offset;
        // Insert the new link into the hash table.
        index = newLink->id % NUM_BUCKETS;
        current = _table[ index ];
        prev = NULL;
        while( current != NULL ){
            comparison = strcmp( _charBuffer + current->offset, start );
            if( comparison > 0 ){
                break;
            }
            prev = current;
            current = current->next;
        }
        newLink->next = current;
        if( prev != NULL ){
            prev->next = newLink;
        } else {
            _table[index] = newLink;
        }
        strCount--;
    }
    cache->CloseComponent();
    return result;
}


void StringTable::Absorb( StringTable &other )
/********************************************/
{
    int                 i;
    Link                *current;
    Link                *prev;
    BRI_StringID        oldID;

    for( i=0; i< NUM_BUCKETS; i++ ){
        prev = NULL;
        current = other._table[i];
        while( current != NULL ){
            oldID = Lookup( other._charBuffer + current->offset );
            if( oldID == BRI_NULL_ID ){
                oldID = Insert( other._charBuffer + current->offset );
            }
            // someIndex->ChangeTo( current->id, oldID );
            prev = current;
            current = current->next;
            delete prev;
        }
        other._table[i] = NULL;
    }
    delete other._charBuffer;
    other._charBuffer = 0;
    other._bufTop = 0;
    other._bufSize = 0;
}

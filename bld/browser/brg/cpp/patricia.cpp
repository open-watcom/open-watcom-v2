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


#include "patricia.h"
#include "strpool.h"

/*-------------------------- PatriciaNode ----------------------------------*/

class PatriciaNode {
public:
                        PatriciaNode();         // construct the head node
                        ~PatriciaNode() {
                            // not called as ragnarok() is used
                        };

    void *              operator new( size_t ) { return _nodePool.alloc(); };
    void                operator delete( void * p ) { _nodePool.free( p ); };

    const char *        insert( const char * str );
    static void         ragnarok();

    #if INSTRUMENTS
    void                print( int level );
    #endif

private:
                        PatriciaNode( int_16 bitPos, char * key,
                                        PatriciaNode * left,
                                        PatriciaNode * right );

    static ubit         getBit( const char * str, uint len, uint_16 bitpos );

    int_16              _bitPos;    // bit to compare for this node
    char *              _key;       // the key value
    PatriciaNode *      _left;
    PatriciaNode *      _right;
    static MemoryPool   _nodePool;
    static StringPool   _stringPool;
};

static MemoryPool PatriciaNode::_nodePool( sizeof( PatriciaNode ),
                                            "PatriciaNode", 16 );

//Note: The StringPool block size must be larger than MERGEFILESTRBUF defined
//in mrfile.h
static StringPool PatriciaNode::_stringPool( 512 * 4, "PatriciaNode" );

PatriciaNode::PatriciaNode()
        : _bitPos( -1 )
        , _key( "" )
        , _left( this )
        , _right( this )
//--------------------------
{
}

PatriciaNode::PatriciaNode( int_16 bitPos, char * key,
                            PatriciaNode * left, PatriciaNode * right )
        : _bitPos( bitPos )
        , _key( key )
        , _left( (left)   ? left : this )
        , _right( (right) ? right : this )
//---------------------------------------------------------------------
{
}

static void PatriciaNode::ragnarok()
//----------------------------------
{
    _nodePool.ragnarok();
    _stringPool.ragnarok();
}

static inline ubit PatriciaNode::getBit( const char * str, uint len,
                                         uint_16 bitPos )
//------------------------------------------------------------------
{
    #if (INSTRUMENTS == INSTRUMENTS_FULL_LOGGING)
        Log.printf( "\"%s\", bitPos=%d, %#x, %d, %#x = %#x\n", str, bitPos, *(str + (bitPos / 8)),
                                    bitPos % 8, (0x80 >> (bitPos % 8)),
                                    *(str + (bitPos / 8)) & (0x80 >> (bitPos % 8)) );
    #endif

    ubit res;

    if( bitPos == (uint_16) -1 ) return 0;

    if( (bitPos >> 3) > len ) {
        return 0;
    };

    str += bitPos >> 3 ;

    res = (ubit) (*str & ( 0x80 >> ( bitPos & 0x07 ) ) );

    return( res != 0 );
}

const char * PatriciaNode::insert( const char * str )
//---------------------------------------------------
{
    PatriciaNode *  prev;
    PatriciaNode *  curr;
    PatriciaNode *  other;      // node with key to be distinguished from this
    int_16          sameBits;   // number of same bits between str and other
    int             len;        // length of string
    char *          strCopy;    // copy of the string
    int             lenCurrStr; // length of string in current node

    prev = this;
    curr = _left;

    len = strlen( str ) + 1;

    while( prev->_bitPos < curr->_bitPos ) {
        prev = curr;
        curr = getBit( str, len, curr->_bitPos )
                ? curr->_right
                : curr->_left;
    }

    if( strcmp( str, curr->_key ) == 0 ) {   // already in tree
        #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
            Log.printf( "\"%s\" has been seen - returning \"%s\"\n", str, curr->_key );
        #endif

        return curr->_key;
    }

    lenCurrStr = strlen( curr->_key ) + 1;

    for( sameBits = 0; (((sameBits / 8) < len) && ((sameBits / 8) < lenCurrStr)); sameBits += 1 ) {
        if( getBit( curr->_key, lenCurrStr, sameBits ) != getBit( str, len, sameBits ) ) {
            break;
        }
    }

    prev = this;
    other = _left;
    while( prev->_bitPos < other->_bitPos && other->_bitPos < sameBits ) {
        prev = other;
        other = getBit( str, len, other->_bitPos )
                    ? other->_right
                    : other->_left;

    }

    strCopy = _stringPool.alloc( len );
    memcpy( strCopy, str, len );
    curr = new PatriciaNode( sameBits, strCopy,
                                getBit( str, len, sameBits ) ? other : NULL,
                                getBit( str, len, sameBits ) ? NULL : other );

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
        Log.printf( "\"%s\" is new - inserting\n", strCopy );
    #endif

    if( getBit( str, len, prev->_bitPos ) ) {
        prev->_right = curr;
    } else {
        prev->_left = curr;
    }

    return strCopy;
}

#if INSTRUMENTS
void PatriciaNode::print( int level )
//-----------------------------------
{
    if( _bitPos < _left->_bitPos ) {
        _left->print( level + 5 );
    } else {
        Log.printf( "%*c[%s|%d]\n", level + 5, ' ', _left->_key, _left->_bitPos );
    }

    Log.printf( "%*c<%s|%d>\n", level, ' ', _key, _bitPos );

    if( _bitPos < _right->_bitPos ) {
        _right->print( level + 5 );
    } else {
        Log.printf( "%*c[%s|%d]\n", level + 5, ' ', _right->_key, _right->_bitPos );
    }
}
#endif

/*-------------------------- PatriciaTree ----------------------------------*/

PatriciaTree::PatriciaTree()
//--------------------------
{
    _head = new PatriciaNode();
}

PatriciaTree::~PatriciaTree()
//---------------------------
{
    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
        Log.printf( "End of tree -- result is :\n\n" );
        _head->print( 0 );
    #endif

    ragnarok();
    delete _head;
    _head = NULL;
}

void PatriciaTree::ragnarok()
//---------------------------
{
    PatriciaNode::ragnarok();
    _head = new PatriciaNode();
}

const char * PatriciaTree::insert( const char * str )
//---------------------------------------------------
{
    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
        Log.printf( "About to insert \"%s\" in tree -- tree is currently:\n", str ? str : "NULL" );
        _head->print( 0 );
    #endif

    return (str) ? _head->insert( str ) : str;
}

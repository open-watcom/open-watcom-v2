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


// Word implementation
//
// 95/01/31 -- J.W.Welch        -- defined

#include "Carve.h"
#include "Hash.h"
#include "Utility.h"
#include "Word.h"

#define ENC_BIT   ((unsigned short)0x80)
#define LARGE_BIT ((unsigned short)0x40)


//---------------------------------------------------------------------
// Support
//---------------------------------------------------------------------

struct _chr_vect                // character vector
{   char const *_vect;          // - vector
    unsigned _size;             // - size
};

//---------------------------------------------------------------------
// Word Implementation
//---------------------------------------------------------------------


static RingHdr Word::wordRing;  // ring of words
static Carve wordCarver         // word carver
    ( sizeof( Word ), 500 );


Word::Word                      // CONSTRUCTOR
    ( const char *chrs          // - characters in word
    , unsigned size )           // - # characters
    : _code( 0 )
    , _count( 1 )
    , _word( chrs, size )
{
}


static unsigned short Word::encode // MAKE ENCODING
    ( void )
{
    WordIter iter;              // - iterator
    unsigned short enc_words    // - # encoded words
        = 0;

    for( ; ; ) {
        Word* curr = iter.next();
        if( 0 == curr ) break;
        if( enc_words < 64 ) {
            if( ( curr->_count - 1 ) * ( curr->_word.size() ) > 3 ) {
                curr->_code = enc_words | ENC_BIT;
                ++enc_words;
            }
        } else {
            if( ( curr->_count - 1 ) * ( curr->_word.size() - 1 ) > 4 ) {
                curr->_code = (unsigned short)
                            ( ( enc_words & 0x3f )
                            | ( (enc_words & 0x3fc0 ) << 2 )
                            | ENC_BIT
                            | LARGE_BIT
                            );
                ++enc_words;
            }
        }
    }
    return enc_words;
}


unsigned Word::encodedSize      // GET ENCODED SIZE
    ( void ) const
{
    unsigned len;
    if( _code & ENC_BIT ) {
        if( _code & LARGE_BIT ) {
            len = 2;
        } else {
            len = 1;
        }
    } else {
        len = _word.size() + 1;
    }
    return len;
}


static unsigned Word::hashFun   // HASH FUNCTION FOR CHARACTER VECTOR
    ( void const *_data )       // - vector
{
    _chr_vect const *v = (_chr_vect*)_data;
    unsigned hash = 0;
    for( unsigned index = 0; index < v->_size; ++ index ) {
        hash = ( hash << 1 ) ^ hash ;
        hash |= v->_vect[ index ];
    }
    return hash;
}


static int Word::hashCompare    // COMPARISON FUNCTION
    ( void const *_element      // - element
    , void const *_comp )       // - comparand
{
    int retn;
    _chr_vect const* comp = ( _chr_vect const * )_comp;
    Word const *element = ( Word const * )_element;
    char const *word = element->_word;
    for( unsigned index = 0; ; ++ index, ++ word ) {
        if( index >= comp->_size ) {
            retn = ( '\0' == *word );
            break;
        }
        if( comp->_vect[ index ] != *word ) {
            retn = 0;
            break;
        }
    }
    return retn;
}


static Word* Word::newWord      // GET NEW WORD
    ( const char *chrs          // - characters in word
    , unsigned size )           // - # characters
{

    static HashTable wordTable  // hash table
        ( &Word::hashFun
        , &Word::hashCompare);

    Word* curr;                 // - current word
    _chr_vect vect              // - character vector
        = { chrs, size };

    curr = (Word *)wordTable.find( &vect );
    if( 0 == curr ) {
        curr = (Word*)wordCarver.alloc();
        curr = new( curr ) Word( chrs, size );
        wordRing.append( curr );
        wordTable.add( curr, &vect );
    } else {
        ++ curr->_count;
    }
    return curr;
}


static int Word::word_compare   // COMPARE TWO WORDS
    ( void const *op1           // - operand(1) (Ring)
    , void const *op2 )         // - operand(2) (Ring)
{
    Ring const **a1 = (Ring const **)op1;
    Ring const **a2 = (Ring const **)op2;
    Word const *w1 = (Word const *)*a1;
    Word const *w2 = (Word const *)*a2;
    long int diff = (long int)w2->_count - (long int)w1->_count;
    int retn;
    if( diff == 0 ) {
        retn = 0;
    } else if( diff > 0 ) {
        retn = 1;
    } else {
        retn = -1;
    }
    return retn;
}


static void Word::sort          // SORT WORD RING
    ( void )
{
    wordRing.sort( &word_compare );
}


void Word::writeMsgEncoding     // WRITE WORD ENCODING IN A MESSAGE
    ( EncStats& stats           // - statistics
    , char *tgt )               // - output buffer
    const
{
    if( _code & ENC_BIT ) {
        if( _code & LARGE_BIT ) {
            stats.space_put_out += 2;
            tgt = concatStr( tgt, ", ENC_BIT | LARGE_BIT | " );
            tgt = concatHex( tgt,  (_code &  0xFF) - ENC_BIT - LARGE_BIT );
            tgt = concatStr( tgt, " , " );
            tgt = concatHex( tgt, ( _code & 0xFF00 ) >> 8 );
        } else {
            stats.space_put_out += 1;
            tgt = concatStr( tgt, ", ENC_BIT | " );
            tgt = concatHex( tgt,  _code - ENC_BIT );
        }
    } else {
        stats.writeName( tgt, _word );
    }
}


static void Word::writeEncoded  // WRITE OUT WORD ENCODING
    ( EncStats& stats           // - statistics
    , FileCtlOutput& out )      // - output file
{
    WordIter iter;              // - iterator
    char buf[256];              // - buffer

    for( ; ; ) {
        Word* curr = iter.next();
        if( 0 == curr ) break;
        if( curr->_code & ENC_BIT ) {
            stats.writeName( buf, curr->_word );
            out.write( buf );
        }
    }
}


static void Word::writeOffsets  // WRITE OUT WORD OFFSETS
    ( EncStats& stats           // - statistics
    , FileCtlOutput& out )      // - output file
{
    WordIter iter;              // - iterator
    char buf[256];              // - buffer

    for( ; ; ) {
        Word* curr = iter.next();
        if( 0 == curr ) break;
        if( curr->_code & ENC_BIT ) {
            stats.writeSizeWritten( buf, curr->_word.size() + 1 );
            out.write( buf );
        }
    }
}

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
* Description:  Phrase replacement for compression purposes.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include "phrase.h"
#include "compress.h"

#define NEW_USAGE   "Usage: phrase filename"

#define HASH_SIZE   691
#define PTBL_SIZE   1720
#define MAX_DATA_SIZE   0xFFFF


char const  PhExt[] = ".ph";


struct Edge;    // Forward declaration.


//
//  Phrase  --specialized string class for storing
//        candidate strings.
//

struct Phrase
{
    char    *_str;
    int     _len;
    int     _bufLen;

    int     _numUses;
    Edge    *_firstEdge;

    Phrase  *_next;

    int     _val;

    static Pool *_pool;
    static void initPool() { _pool = new Pool( sizeof(Phrase) , PTBL_SIZE ); };
    static void freePool() { delete _pool; };

    void *operator new( size_t ) { return _pool->get(); };
    void operator delete( void *p, size_t ) { _pool->release(p); };

    Phrase();
    Phrase( Phrase const &p );
    ~Phrase();

    Phrase &    operator=( Phrase const &p );
    int     operator>( Phrase const &p );
};

Pool *Phrase::_pool = NULL;


//  Phrase::Phrase  --default constructor.

Phrase::Phrase()
    : _bufLen( 80 )     // Arbitrary large value
{
    _str = new char[_bufLen];
}


//  Phrase::Phrase  --copy constructor.

Phrase::Phrase( Phrase const &p )
    : _numUses( 1 ),
      _firstEdge( NULL ),
      _next( NULL )
{
    _bufLen = p._len;
    _len = p._len;
    _str = new char[_len];
    memcpy( _str, p._str, _len * sizeof( char ) );
}


//  Phrase::operator=   --Assignment operator.

Phrase & Phrase::operator=( Phrase const &p )
{
    _bufLen = p._len;
    _len = p._len;
    _str = (char *) renew( _str, _len * sizeof( char ) );
    memcpy( _str, p._str, _len* sizeof( char ) );
    _numUses = 1;
    _firstEdge = NULL;
    _next = NULL;

    return *this;
}


//  Phrase::~Phrase --destructor.

Phrase::~Phrase()
{
    delete[] _str;
}


//  Phrase::operator>   --comparison operator.

int Phrase::operator>( Phrase const &p )
{
    return _val > p._val;
}


//
//  P_String    --Class to represent phrases AFTER the phrase table
//        has been selected.
//

struct P_String
{
    char    *_str;
    int     _len;
    int     _index;
    P_String    *_next;

    P_String( Phrase const &p );
    ~P_String();

private:
    // Assignment of P_Strings is not permitted.
    P_String( P_String const & ){};
    P_String &  operator=( P_String const & ) { return *this; };
};


//  P_String::P_String  --Constructor.

P_String::P_String( Phrase const &p )
    : _next( NULL )
{
    _len = p._len;
    _str = new char[_len];
    memcpy( _str, p._str, _len );
}


//  P_String::~P_String --Destructor.

P_String::~P_String()
{
    delete[] _str;
}


//
//  Edge    --Class to represent the notion "Phrase B follows Phrase
//        A".
//

struct Edge
{
    Phrase  *_dest;
    Edge    *_next;
    int     _val;
};


//
//  PTable  --Class to store a working set of Phrases.
//

class PTable
{
    Pool    *_edges;

    Phrase  **_phrases;
    int     _size;
    int     _maxSize;

    Phrase  **_htable;

    int     _iterPos;

    // Helper function for heapsort-ing.
    void    heapify( int start );

    // Assignment of PTable's is not permitted, so it's private.
    PTable( PTable const & ){};
    PTable &    operator=( PTable const & ) { return *this; };

public:
    PTable();
    ~PTable();

    Phrase  *match( char * &start );
    Phrase  *find( Phrase *other );
    int     &follows( Phrase *first, Phrase *second );

    void    insert( Phrase *p );

    //  Access function.
    int     size() { return _size; };

    // Iter functions.
    void    start();
    Phrase  *next();

    void    clear();
    void    prune();
};


//  PTable::PTable  --Constructor.

PTable::PTable()
    : _size( 0 ),
      _maxSize( PTBL_SIZE ),
      _iterPos( -1 )
{
    _edges = new Pool( sizeof(Edge) );

    _phrases = new Phrase *[_maxSize];
    // Set all of _phrases to NULL.
    memset( _phrases, 0x00, _maxSize*sizeof(Phrase *) );

    _htable = new Phrase *[HASH_SIZE];
    // Set all of _phrases to NULL.
    memset( _htable, 0x00, HASH_SIZE*sizeof(Phrase *) );
}


//  PTable::~PTable --Destructor.

PTable::~PTable()
{
    // Delete any phrases remaining in the Pool.
    if( _phrases ){
    for( int i=0; i<_size; i++ ){
        delete _phrases[i];
    }
    delete[] _phrases;
    }

    if( _edges ) delete _edges;
    if( _htable ) delete[] _htable;
}


//  PTable::match   --Find the best match for a character string.

#define PH_MIN_LEN  3

Phrase *PTable::match( char * &start )
{
    int     length;
    uint_32 h_val;
    Phrase  *result;

    length = strlen( start );
    if( length == 0 ){
    return NULL;
    }

    result = NULL;

    if( length >= PH_MIN_LEN ){
    // First, try to find a match based on the first three characters.
    h_val = 0;
    memcpy( &h_val, start, PH_MIN_LEN );
    result = _htable[h_val%HASH_SIZE];

    while( result != NULL && result->_len > length ){
        result = result->_next;
    }
    while( result != NULL &&
           memcmp( result->_str, start, result->_len ) != 0 ){

           result = result->_next;
    }
    }
    if( result != NULL ){
    start += result->_len;
    if( isspace( *start ) ){
        start++;
    }
    } else {
    // If necessary, check for a shorter match.
    if( length >= PH_MIN_LEN-1 ){
        length = 1;
        while( length < PH_MIN_LEN-1 && !isspace( start[length] ) ){
        length++;
        }
    }
    h_val = 0;
    memcpy( &h_val, start, length );
    result = _htable[h_val%HASH_SIZE];

    while( result != NULL && result->_len > length ){
        result = result->_next;
    }
    while( result != NULL &&
           memcmp( result->_str, start, result->_len ) != 0 ){
        result = result->_next;
    }
    if( result != NULL ){
        start += result->_len;
        if( isspace( *start ) ){
        start++;
        }
    } else {
        int found_text = 0;
        while( *start != '\0' ){
        if( found_text && isspace(*start) ){
            start++;
            break;
        } else if( !found_text && !isspace(*start) ){
            found_text = 1;
        }
        start++;
        }
    }
    }
    return result;
}


//  PTable::find    --Find a given Phrase in the table.

Phrase *PTable::find( Phrase *other )
{
    uint_32 h_val;
    Phrase  *result;
    int     len = other->_len;
    char    *str = other->_str;

    h_val = 0;
    if( len < PH_MIN_LEN ){
    memcpy( &h_val, str, len );
    } else {
    memcpy( &h_val, str, PH_MIN_LEN );
    }

    result = _htable[h_val%HASH_SIZE];
    while( result != NULL && result->_len > len ){
    result = result->_next;
    }
    while( result != NULL && result->_len==len &&
           memcmp( result->_str, str, len ) != 0 ){
    result = result->_next;
    }
    if( result != NULL && result->_len != len ){
    result = NULL;
    }

    return result;
}


//  PTable::follows --Return a reference to the number of times
//            "second" has followed "first".

int &PTable::follows( Phrase *first, Phrase *second )
{
    Edge    *current = first->_firstEdge;
    while( current != NULL ){
    if( current->_dest == second ){
        break;
    }
    current = current->_next;
    }
    if( current == NULL ){
    current = (Edge *) _edges->get();
    current->_dest = second;
    current->_val = 0;
    current->_next = first->_firstEdge;
    first->_firstEdge = current;
    }

    return *(& current->_val);
}


//  PTable::insert  --insert a Phrase in the table.

void PTable::insert( Phrase *p )
{
    Phrase  *current, *temp;
    uint_32 h_val = 0;
    if( p->_len < PH_MIN_LEN ){
    memcpy( &h_val, p->_str, p->_len );
    } else {
    memcpy( &h_val, p->_str, PH_MIN_LEN );
    }

    current = _htable[h_val%HASH_SIZE];
    temp = NULL;

    while( current != NULL && current->_len > p->_len ){
    temp = current;
    current = current->_next;
    }
    p->_next = current;
    if( temp != NULL ){
    temp->_next = p;
    } else {
    _htable[h_val%HASH_SIZE] = p;
    }

    if( _size == _maxSize ){
    _phrases = (Phrase **) renew( _phrases, 2*_maxSize*sizeof(Phrase*));
    // Set the new part of _phrases to NULL.
    memset( _phrases+_maxSize, 0x00, _maxSize*sizeof(Phrase *) );
    _maxSize *= 2;
    }
    _phrases[_size++] = p;
}


//  PTable::start   --Start iterating through the table.

void PTable::start()
{
    _iterPos = 0;
}


//  PTable::next    --Iterate through the phrase table.

Phrase *PTable::next()
{
    if( _iterPos < 0 || _iterPos >= _size ) return NULL;

    return _phrases[_iterPos++];
}


//  PTable::clear   --Clear the phrase table.

void PTable::clear()
{
    int     i;
    Edge    *current, *temp;

    for( i=0; i<_size; i++ ){
    current = _phrases[i]->_firstEdge;
    while( current != NULL ){
        temp = current;
        current = current->_next;
        _edges->release( temp );
    }
    delete _phrases[i];
    }
    for( i=0; i<HASH_SIZE; i++ ){
    _htable[i] = NULL;
    }
    _iterPos = -1;
    _size = 0;
}


//  PTable::heapify --Helper function for heapsort.

void PTable::heapify( int start )
{
    int     left, right;
    int     max = start;
    Phrase  *temp;

    for( ;; ){
    left = 2*start+1;
    right = left+1;

    if( left < _size && (*_phrases[left]) > (*_phrases[start]) ){
        max = left;
    }
    if( right < _size && (*_phrases[right]) > (*_phrases[max]) ){
        max = right;
    }

    if( max == start ) break;

    temp = _phrases[start];
    _phrases[start] = _phrases[max];
    _phrases[max] = temp;

    start = max;
    }
}


//  PTable::prune   --Eliminate all but the 'best' phrases.

void PTable::prune()
{
    int     old_size;
    int     i;
    Phrase  *temp;
    char    *firstc, *startc;
    int     totalsize;

    // Toss out memory we no longer need.
    delete _edges;
    _edges = NULL;
    delete[] _htable;
    _htable = NULL;

    // Heapsort the Phrases to get the top (PTBL_SIZE) Phrases.
    old_size = _size;
    for( i=0; i<_size; i++ ){
    // Get rid of leading spaces for efficiency reasons.
    firstc = _phrases[i]->_str;
    startc = _phrases[i]->_str;
    while( firstc-startc < _phrases[i]->_len ){
        if( isspace(*firstc) ){
        firstc++;
        } else {
        break;
        }
    }
    if( firstc > startc ){
        memmove( startc, firstc, _phrases[i]->_len-(firstc-startc));
        _phrases[i]->_len -= firstc-startc;
    }
    _phrases[i]->_val = (_phrases[i]->_len-2)*(_phrases[i]->_numUses-1);
    }
    for( i=(_size-2)/2; i>=0; i-- ){
    heapify( i );
    }

    totalsize = 2;  // Size of first 2-byte phrase index.
    for( i=0; i<PTBL_SIZE; i++ ){
    if( _size == 0 ) break;
    totalsize += _phrases[0]->_len+2;  // Phrase length + index length

    if( totalsize > MAX_DATA_SIZE ){
        break;
    }

    temp = _phrases[_size-1];
    _phrases[_size-1] = _phrases[0];
    _phrases[0] = temp;
    _size--;
    heapify(0);
    }

    // Delete the remainder of the phrases.
    for( i=0; i<_size; i++ ){
    delete _phrases[i];
    }
    while( _size < old_size ){
    if( _phrases[_size]->_val > 4 ) break;
    delete _phrases[_size];
    _size++;
    }
    memmove( _phrases, _phrases+_size, (old_size-_size)*sizeof(Phrase*) );
    _size = old_size-_size;
}



//  HFPhrases::HFPhrases()  --Default constructor.

HFPhrases::HFPhrases( HFSDirectory * d_file, InFile* (*firstf)(), InFile* (*nextf)() )
    : _oldPtable( NULL ),
      _newPtable( NULL ),
      _result( NULL ),
      _htable( NULL ),
      _numPhrases( 0 ),
      _size( 0 ),
      _nextf( nextf ),
      _firstf( firstf ),
      _scanner( NULL )
{
    d_file->addFile( this, "|Phrases" );
}



//  HFPhrases::~HFPhrases() --Destructor.

HFPhrases::~HFPhrases()
{
    if( _oldPtable ) delete _oldPtable;
    if( _newPtable ) delete _newPtable;
    if( _result ){
    for( int i=0; i<_resultSize ; i++ ){
        delete _result[i];
    }
    delete[] _result;
    }
    if( _htable ){
    delete[] _htable;
    }
}


//  HFPhrases::size --Overrides Dumpable::size.

uint_32 HFPhrases::size()
{
    if( _size > 0 ){
    return _size;
    }

    if( _result == NULL ){
    createQueue( "phrases.ph" );
    }

    CompWriter  riter;
    CompReader  reader( &riter );
    P_String    *string;
    int     i;

    _size = 10;     // Size of the phrase table header.
    _phSize = 0;

    for( i=0; i<_numPhrases; i++ ){
    string = _result[i];
    _phSize += string->_len;
    _size += sizeof(uint_16) + reader.compress( string->_str, string->_len );
    }

    return _size;
}



//  HFPhrases::dump --Overrides Dumpable::dump.

int HFPhrases::dump( OutFile *dest )
{
    const uint_16 magic = 0x0100;
    int       i;

    dest->writebuf( &_numPhrases, sizeof( uint_16 ), 1 );
    dest->writebuf( &magic, sizeof( uint_16 ), 1 );
    dest->writebuf( &_phSize, sizeof( uint_32 ), 1 );

    uint_16 curr_size = (uint_16) ( (_numPhrases+1) * sizeof( uint_16 ) );
    for( i=0; i<_numPhrases; i++ ){
    dest->writebuf( &curr_size, sizeof( uint_16 ), 1 );
    curr_size = (uint_16) (curr_size + _result[i]->_len);
    }
    dest->writebuf( &curr_size, sizeof( uint_16 ), 1 );

    CompOutFile riter( dest );
    CompReader  reader( &riter );
    P_String    *string;

    for( i=0; i<_numPhrases; i++ ){
    string = _result[i];
    reader.compress( string->_str, string->_len );
    }
    reader.flush();

    return 1;
}



//  HFPhrases::startInput   --Prepare to read the first block
//                of input.

void HFPhrases::startInput()
{
    InFile  *input;

    if( _scanner ) delete _scanner;
    _scanner = NULL;
    input = (*_firstf)();
    if( input == NULL ) return;
    _scanner = new Scanner( input );
}


//  HFPhrases::nextInput    --Get the next block of input.
//

char* HFPhrases::nextInput()
{
    InFile  *input;
    Token   *next;
    char    *result;

    if( _scanner == NULL ) return NULL;

    for( ;; ){
    next = _scanner->next();

    if( next->_type == TOK_END ){
        delete _scanner;
        _scanner = NULL;
        input = (*_nextf)();
        if( input == NULL ){
        return NULL;
        }
        _scanner = new Scanner( input );

    } else if( next->_type != TOK_TEXT ){
        int push_level, done = 0;

        for( ;; ){
        switch( next->_type ){
        case TOK_END:
        case TOK_TEXT:  // deliberate fall-through
            done = 1;
            break;

        case TOK_COMMAND:
            if( strcmp( next->_text, "colortbl" )==0 ||
            strcmp( next->_text, "fonttbl" )==0  ||
            strcmp( next->_text, "footnote" )==0 ||
            strcmp( next->_text, "stylesheet" )==0 ){
            push_level = 0;
            do{
                next = _scanner->next();
                if( next->_type == TOK_PUSH_STATE ){
                push_level++;
                } else if( next->_type == TOK_POP_STATE ){
                push_level--;
                } else if( next->_type == TOK_END ){
                break;
                }
            }while( push_level >= 0 );
            } else if( strcmp( next->_text, "v" )==0 &&
                   (!next->_hasValue ||
                next->_value != 0 ) ){
            push_level = 0;
            do{
                next = _scanner->next();
                if( next->_type == TOK_PUSH_STATE ){
                push_level++;
                } else if( next->_type == TOK_POP_STATE ){
                push_level--;
                } else if( next->_type == TOK_COMMAND &&
                       strcmp( next->_text, "v" )==0 &&
                       next->_hasValue &&
                       next->_value == 0 ){
                break;
                } else if( next->_type == TOK_END ){
                break;
                }
            }while( push_level >= 0 );
            }
            break;
        }

        if( done ) break;

        next = _scanner->next();
        }
        if( next->_type == TOK_END ){
        delete _scanner;
        _scanner = NULL;
        input = (*_nextf)();
        if( input == NULL ){
            return NULL;
        }
        _scanner = new Scanner( input );

        } else {
        result = next->_text;
        break;
        }
    } else {
        result = next->_text;
        break;
    }
    }

    return result;
}


//  HFPhrases::readPhrases  --Fill the phrase table with candidate
//                phrases.

void HFPhrases::readPhrases()
{
    char    *block = NULL;
    char    *end;
    int     found_text;
    int     count;
    int     getnext;
    Phrase  phr;
    Phrase  *p_phr, *last, *next, *lookahead;
    PTable  *temp;
    Edge    *current;


    Phrase::initPool();

    _oldPtable = new PTable;
    _newPtable = new PTable;

    // Put all of the words in the file in a dictionary.
    HCStartPhrase();
    HCPhraseLoop(1);
    startInput();
    while( (block = nextInput()) != NULL ){
    last = NULL;
    while( *block != '\0' ){
        found_text = 0;
        phr._len = 0;
        end = block;
        while( *end != '\0' ){
        if( found_text && isspace(*end) ){
            break;
        } else if( !found_text && !isspace(*end) ){
            found_text = 1;
        }
        if( phr._len == phr._bufLen ){
            phr._str = (char *) renew(phr._str, 2*phr._bufLen );
            phr._bufLen *= 2;
        }
        phr._str[phr._len++] = *end++;
        }

        // Create the phrase.
        p_phr = _newPtable->find( &phr );
        if( p_phr != NULL ){
        p_phr->_numUses += 1;
        } else {
        _newPtable->insert( p_phr = new Phrase( phr ) );
        }

        if( last != NULL ){
        _newPtable->follows( last, p_phr ) += 1;
        }
        last = p_phr;

        if( *end != '\0' ){
        block = end+1;
        } else {
        block = end;
        }
    }
    }


    // Build up longer phrases iteratively with extra
    // passes over the file.

    // NOTE THE ARBITRARY CUTOFF.  I have reason to suspect this
    // algorithm is non-terminating in certain cases.
    for( count=1; count<10; count++ ){
    HCPhraseLoop( count+1 );

    temp = _oldPtable;
    _oldPtable = _newPtable;
    _newPtable = temp;

    startInput();
    while( (block = nextInput()) != NULL ){
        last = next = lookahead = NULL;
        getnext = 1;
        while( *block != '\0' ){
        if( getnext ){
            next = _oldPtable->match( block );
        }
        if( *block != '\0' ){
            lookahead = _oldPtable->match( block );
        } else {
            lookahead = NULL;
        }
        if( next == NULL || lookahead == NULL ||
            _oldPtable->follows( next, lookahead ) < 2 ){
            if( next != NULL ){
            p_phr = _newPtable->find( next );
            if( p_phr != NULL ){
                p_phr->_numUses++;
            } else {
                _newPtable->insert( p_phr = new Phrase(*next) );
            }
            if( last != NULL ){
                _newPtable->follows( last, p_phr ) += 1;
            }
            } else {
            p_phr = NULL;
            }

            next = lookahead;
            getnext = 0;
        } else {
            // Set phr to (next + lookahead).
            phr._len = next->_len + lookahead->_len + 1;
            if( phr._bufLen < phr._len ){
            phr._bufLen = phr._len;
            phr._str = (char *) renew( phr._str, phr._len );
            }
            memcpy( phr._str, next->_str, next->_len );
            phr._str[next->_len] = ' ';
            memcpy( phr._str+next->_len+1, lookahead->_str,
                    lookahead->_len );

            p_phr = _newPtable->find( &phr );
            if( p_phr != NULL ){
            p_phr->_numUses++;
            } else {
            _newPtable->insert( p_phr = new Phrase(phr) );
            }
            if( last != NULL ){
            _newPtable->follows( last, p_phr ) += 1;
            }

            next = NULL;
            lookahead = NULL;
            getnext = 1;
        }

        last = p_phr;
        }
        if( next != NULL ){
        p_phr = _newPtable->find( next );
        if( p_phr != NULL ){
            p_phr->_numUses++;
        } else {
            _newPtable->insert( p_phr = new Phrase(*next) );
        }
        if( last != NULL ){
            _newPtable->follows( last, p_phr ) += 1;
        }
        }
    }

    _oldPtable->clear();

    _newPtable->start();
    while( (p_phr = _newPtable->next()) != NULL ){
        current = p_phr->_firstEdge;
        while( current != NULL ){
        if( current->_val >= 2 ) break;
        current = current->_next;
        }
        if( current != NULL ) break;
    }
    if( p_phr == NULL ) break;
    }

    HCDoneTick();

    delete _oldPtable;
    _oldPtable = NULL;
}


//  HFPhrases::initHashTable    --Initialize the hash table.

void HFPhrases::initHashTable()
{
    uint_32 hvalue;
    P_String    *curr_str;

    if( _htable == NULL ){
    _htable = new P_String *[HASH_SIZE];
    }
    memset( _htable, 0x00, HASH_SIZE * sizeof( P_String * ) );

    for( int i=0; i<_resultSize; i++ ){
    curr_str = _result[i];
    memcpy( &hvalue, curr_str->_str, PH_MIN_LEN );
    hvalue &= 0xFFFFFF;
    hvalue %= HASH_SIZE;

    curr_str->_next = _htable[hvalue];
    _htable[hvalue] = curr_str;
    }
}


//  HFPhrases::createQueue  --Find all candidate Phrases with a high
//                enough _value field, and add them to a
//                priority queue.

void HFPhrases::createQueue( char const *path )
{
    Phrase      *current;
    int         i;

    _newPtable->prune();

    _resultSize = _newPtable->size();
    _result = new P_String *[_resultSize];

    _newPtable->start();

    OutFile ph_file( path );
    if( ph_file.bad() ) {
        HCError( FILE_ERR, path );
    }
    for( i=0; (current = _newPtable->next()) != NULL; i++ ){
    _result[i] = new P_String( *current );

    ph_file.writebuf( _result[i]->_str, 1, _result[i]->_len );
    ph_file.writech( '\r' );
    ph_file.writech( '\n' );

    _result[i]->_index = i;
    }
    ph_file.close();

    // We no longer need the dictionary, or the Phrase queue.
    delete _newPtable;
    _newPtable = NULL;

    Phrase::freePool();

    // Initialize the 'hash table'.
    initHashTable();
}


//  HFPhrases::oldTable --Use a previously created phrase table.

int HFPhrases::oldTable( char const *path )
{
    InFile  ph_file( path );
    if( ph_file.bad() ){
    return 0;
    }

    Phrase  current;
    int     ptable_size = PTBL_SIZE;
//    int     done = 0;
    int     c = '\0';
    int     totalsize;  // Size of the phrase data loaded.

    _result = new P_String *[ptable_size];
    _resultSize = 0;
    current._len = 0;
    totalsize = 2;  // Size of first 2-byte phrase index.
    while( c != EOF ){
    c = ph_file.nextch();
    if( c == EOF || c == '\n' ){
        if( current._len != 0 ){

        totalsize += current._len+2;    // Phrase size + index size
        if( totalsize > MAX_DATA_SIZE ){
            break;
        }

        if( _resultSize == ptable_size ){
            _result = (P_String**) renew( _result, 2*ptable_size*sizeof(Phrase*) );
            ptable_size *= 2;
        }
        _result[_resultSize] = new P_String( current );
        _result[_resultSize]->_index = _resultSize;
        _resultSize += 1;
        current._len = 0;
        }
    } else {
        if( current._len == current._bufLen ){
        current._str = (char *) renew( current._str, 2*current._bufLen );
        current._bufLen *= 2;
        }
        current._str[current._len++] = (char) c;
    }
    }

    // Initialize the 'hash table'.
    initHashTable();

    return 1;
}



//  HFPhrases::replace  --Go through a block of text and replace
//            common phrases where they appear.

void HFPhrases::replace( char * dst, char const *src, int & len )
{
    uint_32 hvalue = 0;
    P_String    *current, *best;
    int     read_pos = 0;
    int     write_pos = 0;

    while( read_pos < len-2 ){
    memcpy( &hvalue, src + read_pos, PH_MIN_LEN );
    hvalue %= HASH_SIZE;

    current = _htable[hvalue];
    best = NULL;
    while( current != NULL ){
        if( current->_len <= len - read_pos &&
            memcmp( current->_str, src + read_pos, current->_len ) == 0 ){
        if( best == NULL || best->_len < current->_len ){
            best = current;
        }
        }
        current = current->_next;
    }

    if( best == NULL ){
        dst[write_pos++] = src[read_pos++];
    } else {
        if( best->_index >= _numPhrases ){
        if( best->_index > _numPhrases ){
            P_String *temp = _result[_numPhrases];
            _result[_numPhrases] = _result[best->_index];
            _result[best->_index] = temp;
            _result[best->_index]->_index = best->_index;
            best->_index = _numPhrases;
        }
        _numPhrases = (uint_16) (_numPhrases+1);
        }

        // Convert the index to a WinHelp "phrase code".
        // See "phrases.doc".
        dst[write_pos] = (uint_8) ((( best->_index >> 7 ) & 0xF ) + 1 );
        dst[write_pos+1] = (uint_8) (( best->_index & 0x7f ) << 1 );

        read_pos += best->_len;
        if( src[read_pos] == ' ' ){
        dst[write_pos+1] |= 0x1;
        read_pos++;
        }
        write_pos += 2;
    }
    }
    while( read_pos < len ){
    dst[write_pos++] = src[read_pos++];
    }

    len = write_pos;
}

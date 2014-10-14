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
* Description:  Write help text in .hlp format.
*
****************************************************************************/


#include <string.h>
#include "topic.h"
#include "clibext.h"

#define COMP_PAGE_SIZE 4096

#define NULLVAL32       ((uint_32)-1L)

//
//  TextAttr    --Structure to record changes to text.
//

struct TextAttr
{
    FontFlags   _type;
    uint_32 _data;
    unsigned _size;
    char    *_stringDat;
};


//
//  TopicLink   --Linked list record to store binary topic data
//        once it's generated.
//

struct TopicLink
{
    TopicLink       *_next;
    uint_32         _size;
    bool            _isFirstLink;
    Buffer<char>    _myData;

    TopicLink( uint_32 s );
};


//
//  StringNode  --A linked list record to keep track of browse sequences.
//

struct StringNode
{
    char    *_string;
    TopicLink   *_me;
    uint_32 _charOffset;
    StringNode  *_next, *_prev;

    // Handy access functions used by HFTopic::dumpBrowse().
    uint_32 &   prevBrowse() { return *((uint_32*) (_me->_myData+4)); };
    uint_32 &   nextBrowse() { return *((uint_32*) (_me->_myData+8)); };
};


//
//  PageHeader  --Structure to store paging-related information.
//

#define PAGE_HEADER_SIZE    12

struct PageHeader
{
    PageHeader  *_next;
    uint_32 _pageNums[3];

    // Some mnemonic access functions for accessing _pageNums.
    uint_32 &   lastNode() { return *_pageNums; };
    uint_32 &   nextNode() { return *( _pageNums + 1 ); };
    uint_32 &   lastTopic() { return *( _pageNums + 2 ); };

    PageHeader();
};


// NOTE:  every node in the |TOPIC linked list consistes of three parts:
// a node header, a type-specific header, and the text.  There are three
// different type-specific headers, two of which are implemented here.


//
//  GenericNode --Class to represent the 'node header'.
//

#define GENERIC_NODE_SIZE   21
class GenericNode
{
    uint_32 _topicSize;
    uint_32 _dataSize;
    uint_32 _prevNode;
    uint_32 _nextNode;
    uint_32 _dataOffset;
    uint_8  _recordType;
    uint_32 _size;
    TopicLink   *_myLink;

    GenericNode( uint_32 prev );

    void    dumpTo( TopicLink *dest );

    friend class HFTopic;
};


//
//  TopicHeader --Class to store topic headers, one kind of
//        type-specific header.
//

#define TOPIC_HEADER_SIZE   28
class TopicHeader
{
    uint_32 _totalSize;
    uint_32 _nextBrowse;
    uint_32 _prevBrowse;
    uint_32 _topicNum;
    uint_32 _startNonScroll;
    uint_32 _startScroll;
    uint_32 _nextTopic;
    uint_32 _size;
    TopicLink   *_myLink;

    TopicHeader( uint_32 tnum );

    void    dumpTo( TopicLink *dest );

    friend class HFTopic;
};


#define TEXT_ARG_MAX    10
#define TEXT_ATTR_MAX   25


//
//  TextHeader  --Class to store text headers, one type of
//        type-specific header.
//

#define TEXT_HEADER_SIZE    9
class TextHeader
{
    uint_32 _size;
    uint_32 _parAttrSize;
    uint_16 _headerSize;
    uint_16 _textSize;
    uint_8  _numColumns;
    uint_32 _flags;

    Buffer<uint_16> _tabStops;
    Buffer<uint_8>  _tabFlags;
    uint_16     _numStops, _maxStops;

    uint_32 _border;
    uint_16 _spacing[6];

    static const uint_32 _parBits[];

    Buffer<TextAttr>    _attribs;
    unsigned            _numAttribs;
    unsigned            _maxAttribs;

    static const uint_8 _attrBits[];
    static const int    _attrSizes[];

    TextHeader();
    ~TextHeader();

    // Functions to manipulate paragraph attributes (tabs, indents, ...)
    void        dumpTo( TopicLink *dest );
    int         setTab( int val, uint_8 flags );
    int         setPar( ParFlags type, int val );
    void        unsetPar( ParFlags type );
    void        clearPar();
    unsigned    addAttr( FontFlags type, uint_32 val, char const str[], int length );
    unsigned    appendAttr( unsigned index, FontFlags type, uint_32 val, char const str[], int length );
    void        chgAttr( unsigned index, FontFlags type, uint_32 val, char const str[], int length );
    uint_32     attrData( unsigned index );
    void        reset();

    friend class HFTopic;
};


//
//  TextHolder  --Class to store raw text, for topic titles and help text.
//

#define TEXT_BLOCK_SIZE 0x200
#define TEXT_ZERO_SIZE  10
class TextHolder
{
    uint_32         _size;
    uint_32         _uncompSize;
    uint_32         _maxSize;
    Buffer<char>    _text;

    // Formatting changes are signalled by 0x00 bytes in the text;
    // I must be able to insert those zeroes in the middle of the text.

    Buffer<uint_16> _zeroes;
    uint_16         _numZeroes;
    uint_16         _maxZeroes;

    TextHolder();

    void    dumpTo( TopicLink *dest );
    void    reset() { _size = 0; _uncompSize = 0; _numZeroes = 0; };

    friend class HFTopic;
};


//  TopicLink::TopicLink

TopicLink::TopicLink( uint_32 s ) : _size( s ), _isFirstLink(false), _myData( s )
{
    // empty
}


//  GenericNode::GenericNode

GenericNode::GenericNode( uint_32 prev )
    : _topicSize( GENERIC_NODE_SIZE ),
      _dataSize( 0 ),
      _prevNode( prev ),
      _nextNode( NULLVAL32 ),
      _size( GENERIC_NODE_SIZE )
{
    // empty
}


//  GenericNode::DumpTo --Convert the node to it's binary form.

void GenericNode::dumpTo( TopicLink *dest )
{
    memcpy( dest->_myData, this, GENERIC_NODE_SIZE );   // EXTREME unsafeness!
    dest->_size = _size;
}


//  TopicHeader::TopicHeader

TopicHeader::TopicHeader( uint_32 tnum )
    : _totalSize( 0 ),
      _nextBrowse( NULLVAL32 ),
      _prevBrowse( NULLVAL32 ),
      _topicNum( tnum ),
      _startNonScroll( NULLVAL32 ),
      _startScroll( NULLVAL32 ),
      _nextTopic( NULLVAL32 ),
      _size( TOPIC_HEADER_SIZE )
{
    // empty
}


//  TopicHeader::DumpTo --Convert the node to it's binary form.

void TopicHeader::dumpTo( TopicLink *dest )
{
    memcpy( dest->_myData, this, TOPIC_HEADER_SIZE );   // EXTREME unsafeness!
    dest->_size = _size;
}


//  PageHeader::PageHeader

PageHeader::PageHeader()
    : _next(NULL)
{
    // empty
}


//  TextHeader c-tor and d-tor.

#define TSTOP_BLOCKS 10
TextHeader::TextHeader()
    : _size( TEXT_HEADER_SIZE ),
      _parAttrSize( 0 ),
      _flags( 0 ),
      _tabStops( TSTOP_BLOCKS ),
      _tabFlags( TSTOP_BLOCKS ),
      _numStops( 0 ),
      _maxStops( TSTOP_BLOCKS ),
      _attribs( TEXT_ATTR_MAX ),
      _numAttribs( 0 ),
      _maxAttribs( TEXT_ATTR_MAX )
{
    // empty
}


TextHeader::~TextHeader()
{
    reset();
}


//  TextHeader::reset   --Clear the text attributes.

void TextHeader::reset()
{
    for( unsigned i = 0; i < _numAttribs; i++ ) {
        if( _attribs[i]._stringDat != NULL ) {
            delete[] _attribs[i]._stringDat;
        }
    }
    _size = TEXT_HEADER_SIZE;
    _numColumns = 0;
    _numAttribs = 0;
}


//  The bitfields corresponding to paragraph attributes.
const uint_32 TextHeader::_parBits[] = {
    0x00020000,
    0x00040000,
    0x00080000,
    0x00100000,
    0x00200000,
    0x00400000,
    0x01000000,
    0x02000000,
    0x04000000,
    0x08000000,
    0x10000000
};


//  TextHeader::setTab  --Set a (possibly non-standard) tab stop.

#define INT_SMALL_LIMIT 0x80
#define INT_LARGE_LIMIT 0x4000
#define BORDER_BYTE_SIZE 3

int TextHeader::setTab( int val, uint_8 flag )
{
    uint_16 trueval;

    if( val < 0 ) {
        return 0;
    } else if( val >= INT_LARGE_LIMIT ) {
        return 0;
    }

    // Convert the value into WinHelp's bizarre format for
    // storing integers; see "topic.doc".

    trueval = (uint_16) (val/10);
    trueval <<= 1;
    if( trueval < INT_SMALL_LIMIT ) {
        _parAttrSize += 1;
    } else {
        trueval |= 0x1;
        _parAttrSize += 2;
    }
    if( flag != 0x0 ) {
        _parAttrSize += 1;
        if( trueval < INT_SMALL_LIMIT ) {
            trueval |= 0x80;
        } else {
            trueval |= 0x8000;
        }
    }

    if( _numStops == 0 ) {
        _parAttrSize += 1;
    } else if( _numStops == _maxStops ) {
        _maxStops += TSTOP_BLOCKS;
        _tabStops.resize( _maxStops );
        _tabFlags.resize( _maxStops );
    }
    _tabStops[_numStops] = trueval;
    _tabFlags[_numStops] = flag;
    if( ++_numStops == 0x40 ) {
        _parAttrSize += 1;
    }
    _flags |= _parBits[TOP_TAB_STOPS];
    return 1;
}


//  TextHeader::setPar  --Set a paragraph attribute.

int TextHeader::setPar( ParFlags type, int val )
{
    int already_set = _flags & _parBits[type];
    int sign = 0;
    uint_16 trueval = 0;

    if( val < 0 && type == TOP_TAB_STOPS ) {
        return 0;
    }
    if( type == TOP_BORDER ) {
        _border = (uint_32) val;
        _parAttrSize += BORDER_BYTE_SIZE;
    } else {
        // Convert the value into binary form.
        // This is complicated by WinHelp's bizarre format for
        // storing integers; see "topic.doc".
        if( type < TOP_RIGHT_JUST ) {
            if( val == -1 )
                return 1;
            sign = ( val < 0 );
            if( sign ) {
                val = -val;
            }
            if( val >= INT_LARGE_LIMIT ) {
                return 0;
            }
            trueval = (uint_16)( val / 10 );
            trueval <<= 1;
        }
        if( type == TOP_TAB_STOPS ) {
            if( trueval < INT_SMALL_LIMIT ) {
                _parAttrSize += 1;
            } else {
                trueval |= 0x1;
                _parAttrSize += 2;
            }

            if( _numStops == 0 ) {
                _parAttrSize += 1;
            } else if( _numStops == _maxStops ) {
                _maxStops += TSTOP_BLOCKS;
                _tabStops.resize( _maxStops );
                _tabFlags.resize( _maxStops );
            }
            _tabStops[_numStops] = trueval;
            _tabFlags[_numStops] = 0x0;
            if( ++_numStops == 0x40 ) {
                _parAttrSize += 1;
            }
        } else if( type < TOP_TAB_STOPS ) {
            if( already_set ) {
                if( _spacing[type] & 0x1 ) {
                    _parAttrSize -= 2;
                } else {
                    _parAttrSize -= 1;
                }
            }
            if( trueval < INT_SMALL_LIMIT ) {
                trueval |= 0x80;
                _parAttrSize += 1;
            } else {
                trueval |= 0x8001;
                _parAttrSize += 2;
            }
            if( sign ) {
                if( trueval & 01 ) {
                    trueval ^= (uint_16) ~1;
                } else {
                    trueval ^= (uint_8) ~1;
                }
                trueval += 4;
            }
            _spacing[type] = trueval;
        }
    }
    _flags |= _parBits[type];
    return 1;
}


//  TextHeader::unsetPar  --Clear a paragraph attribute.

void TextHeader::unsetPar( ParFlags type )
{
    int already_set = _flags & _parBits[type];
    if( !already_set )
        return;
    _flags ^= _parBits[type];
    if( type == TOP_TAB_STOPS ) {
        _parAttrSize -= _numStops;
        for( int i=0; i < _numStops; ++i ) {
            if( _tabStops[i] & 0x1 ) {
                _parAttrSize -= 1;
            }
            if( _tabFlags[i] != 0 ) {
                _parAttrSize -= 1;
            }
        }
        if( _numStops >= 0x40 ) {
            _parAttrSize -= 2;
        } else if( _numStops > 0 ) {
            _parAttrSize -= 1;
        }
        _numStops = 0;
    } else if( type == TOP_BORDER ) {
        _parAttrSize -= BORDER_BYTE_SIZE;
    } else if( type < TOP_TAB_STOPS ) {
        if( _spacing[type] & 0x1 ) {
            _parAttrSize -= 2;
        } else {
            _parAttrSize -= 1;
        }
    }
}


//  TextHeader::clearPar  --Clear all paragraph attributes.

void TextHeader::clearPar()
{
    _flags = 0;
    _parAttrSize = 0;
    _numStops = 0;
}

// The flag values corresponding to text attributes.
const uint_8 TextHeader::_attrBits[] = {
    0x80, 0x81, 0x82, 0x83, 0x86, 0x87, 0x88, 0x89, 0xC8, 0xCC,
    0xE2, 0xE3, 0xE6, 0xE7, 0xEA, 0xEB, 0xEE, 0xEF, 0xFF
};

// used to calculate the size of a text attribute.
const int TextHeader::_attrSizes[] = {  3, 1, 1, 1, 9, 9, 9, 1, 3, 3,
                    5, 5, 5, 5, 7, 7, 7, 7, 1
};


//  TextHeader::addAttr --Add a text attribute.

unsigned TextHeader::addAttr( FontFlags type, uint_32 val, char const str[], int length )
{
    unsigned result = _numAttribs;
    if( _numAttribs == _maxAttribs ) {
        _maxAttribs += TEXT_ATTR_MAX;
        _attribs.resize( _maxAttribs );
    }
    _attribs[_numAttribs]._type = type;
    _attribs[_numAttribs]._data = val;
    if( length > 0 ) {
        _attribs[_numAttribs]._stringDat = new char[length];
        memcpy( _attribs[_numAttribs]._stringDat, str, length );
    } else {
        _attribs[_numAttribs]._stringDat = NULL;
    }

    _attribs[_numAttribs]._size = _attrSizes[type];
    if( type == TOP_MACRO_LINK || type == TOP_MACRO_INVIS ||
        ( type >= TOP_POPUP_FILE && type <= TOP_JUMP_FILE_INVIS ) ) {
        _attribs[_numAttribs]._size += length;
    }
    _size += _attribs[_numAttribs]._size;
    ++_numAttribs;

    return result;
}


//  TextHeader::appendAttr   --Append a text attribute after a given one.

unsigned TextHeader::appendAttr( unsigned index, FontFlags type, uint_32 val,
                 char const str[], int length )
{
    if( index >= _numAttribs ) {
        HCError( HLP_ATTR );
    }

    unsigned result = index + 1;

    if( _numAttribs == _maxAttribs ) {
        _maxAttribs += TEXT_ATTR_MAX;
        _attribs.resize( _maxAttribs );
    }

    if( result < _numAttribs ) {
        memmove( &_attribs[result + 1], &_attribs[result],
             ( _numAttribs - result ) * sizeof( TextAttr ) );
    }
    _attribs[result]._type = type;
    _attribs[result]._data = val;
    if( length > 0 ) {
        _attribs[result]._stringDat = new char[length];
        memcpy( _attribs[result]._stringDat, str, length );
    } else {
        _attribs[result]._stringDat = NULL;
    }

    _attribs[result]._size = _attrSizes[type];
    if( type == TOP_MACRO_LINK || type == TOP_MACRO_INVIS ||
        ( type >= TOP_POPUP_FILE && type <= TOP_JUMP_FILE_INVIS ) ) {
        _attribs[result]._size += length;
    }
    _size += _attribs[result]._size;
    ++_numAttribs;

    return result;
}


//  TextHeader::chgAttr   --Modify a text attribute.

void TextHeader::chgAttr( unsigned index, FontFlags type, uint_32 val,
                           char const str[], int length )
{
    if( index >= _numAttribs ) {
        HCError( HLP_ATTR );
    }

    _size -= _attribs[index]._size;
    _attribs[index]._type = type;
    _attribs[index]._data = val;
    if( _attribs[index]._stringDat != NULL ) {
        delete _attribs[index]._stringDat;
    }
    if( length > 0 ) {
        _attribs[index]._stringDat = new char[length];
        memcpy( _attribs[index]._stringDat, str, length );
    } else {
        _attribs[index]._stringDat = NULL;
    }

    _attribs[index]._size = _attrSizes[type];
    if( type == TOP_MACRO_LINK || type == TOP_MACRO_INVIS ||
        ( type >= TOP_POPUP_FILE && type <= TOP_JUMP_FILE_INVIS ) ) {
        _attribs[index]._size += length;
    }

    _size += _attribs[index]._size;
}


//  TextHeader::attrData    --Read the data from an attribute.

uint_32 TextHeader::attrData( unsigned index )
{
    if( index >= _numAttribs ) {
        HCError( HLP_ATTR );
    }

    return _attribs[index]._data;
}


//  TextHeader::DumpTo  --Convert the node to it's binary form.
//            Complicated, thanks to many variable-length fields.

void TextHeader::dumpTo( TopicLink *dest )
{
    char        *location = dest->_myData;
    unsigned    i;

    *( (uint_16*) location ) = (uint_16) ( (2*_headerSize) | 0x8000 );
    location += sizeof( uint_16 );
    if( _textSize < INT_SMALL_LIMIT ) {
        *location++ = (uint_8) (_textSize*2);
    } else {
        *( (uint_16*) location ) = (uint_16) (_textSize*2+1) ;
        location += sizeof( uint_16 );
    }
    *location++ = _numColumns;
    *location++ = '\x80'; // magic byte
    *((uint_32*) location ) = _flags;
    location += sizeof( uint_32 );

    // Print out the paragraph attributes.
    for( i=0; i < TOP_BORDER; i++ ) {
        if( _flags & _parBits[i] ) {
            if( _spacing[i] & 0x1 ) {
                *((uint_16*) location) = _spacing[i];
                location += sizeof( uint_16 );
            } else {
                *location++ = (uint_8)_spacing[i];
            }
        }
    }
    if( _flags & _parBits[TOP_BORDER] ) {
        *((uint_16*) location) = (uint_16) _border;
        location += sizeof( uint_16 );
        *location++ = (uint_8) (_border >> 16);
    }
    // Dump the tab stops, converting them into WinHelp's integer format.
    if( _flags & _parBits[TOP_TAB_STOPS] ) {
        uint_16 stops_num = _numStops;
        stops_num <<= 1;
        if( stops_num < INT_SMALL_LIMIT ) {
            stops_num |= 0x80;
            *location++ = (uint_8) stops_num;
        } else {
            stops_num |= 0x8001;
            *((uint_16*) location) = stops_num;
            location += sizeof( uint_16 );
        }
        for( i = 0; i < _numStops; i++ ) {
            if( _tabStops[i] & 0x1 ) {
                *((uint_16*) location) = _tabStops[i];
                location += sizeof( uint_16 );
                if( _tabStops[i] & 0x8000 ) {
                    *location++ = _tabFlags[i];
                }
            } else {
                *location++ = (uint_8)_tabStops[i];
                if( _tabStops[i] & 0x80 ) {
                    *location++ = _tabFlags[i];
                }
            }
        }
    }

    // Now the text attributes.
    uint_16 length;
    for( i = 0; i < _numAttribs; i++ ) {
        *location++ = _attrBits[_attribs[i]._type];
        switch( _attribs[i]._type ) {
            // deliberate fall-through
        case TOP_NEW_LINE:
        case TOP_NEW_PAR:
        case TOP_HTAB:
        case TOP_END_LINK:
        case TOP_END:
            // No argument for these flags.
            break;

        case TOP_CENT_BITMAP:
        case TOP_LEFT_BITMAP:
        case TOP_RIGHT_BITMAP:
            // Right now these numbers are mostly magic.
            *((uint_32*) location) = 0x02800822;
            location += sizeof( uint_32 );
            *((uint_16*) location) = (uint_16)0;
            location += sizeof( uint_16 );
            *((uint_16*) location) = (uint_16)_attribs[i]._data;
            location += sizeof( uint_16 );
            break;

        case TOP_FONT_CHANGE:
            *((uint_16*) location) = (uint_16)_attribs[i]._data;
            location += sizeof( uint_16 );
            break;

            // more fall-through
        case TOP_POPUP_LINK:
        case TOP_JUMP_LINK:
        case TOP_POPUP_INVIS:
        case TOP_JUMP_INVIS:
            *((uint_32*) location) = _attribs[i]._data;
            location += sizeof( uint_32 );
            break;

        case TOP_MACRO_LINK:
        case TOP_MACRO_INVIS:
            length = (uint_16)( _attribs[i]._size - 3 );
            *((uint_16*) location) = length;
            location += sizeof( uint_16 );
            memcpy( location, _attribs[i]._stringDat, length );
            location += length;
            break;

        default:
            length = (uint_16)( _attribs[i]._size - _attrSizes[_attribs[i]._type] );
            *((uint_16*) location) = (uint_16)( length + 4 );
            location += sizeof( uint_16 );
            *location++ = _attribs[i]._stringDat[0];
            *((uint_32*) location) = _attribs[i]._data;
            location += sizeof( uint_32 );
            memcpy( location, _attribs[i]._stringDat + 1, length - 1 );
            location += length - 1;
        }
    }

    dest->_size = _size;
}


//  TextHolder::TextHolder

TextHolder::TextHolder()
    : _size( 0 ),
      _uncompSize( 0 ),
      _maxSize( TEXT_BLOCK_SIZE ),
      _text( TEXT_BLOCK_SIZE+1 ),
      _zeroes( TEXT_ZERO_SIZE ),
      _numZeroes( 0 ),
      _maxZeroes( TEXT_ZERO_SIZE )
{
    // empty
}


//  TextHolder::DumpTo  --Convert the node to it's binary form.

void TextHolder::dumpTo( TopicLink *dest )
{
    if( _size > 0 ) {
        memcpy( dest->_myData, _text, _size );
    }
    dest->_size = _size;
}


//  HFTopic::HFTopic

HFTopic::HFTopic( HFSDirectory * d_file, HFPhrases *ph )
    : _size( PAGE_HEADER_SIZE ),  // The initial 3-DWORD page header.
      _numPages( 1 ),
      _numTopics( 0 ),
      _head( NULL ),
      _tail( NULL ),
      _phFile( ph ),
      _browseStr( NULL ),
      _bhead( NULL ),
      _btail( NULL ),
      _lastNode( NULL ),
      _lastHeader( NULL ),
      _curOffset( PAGE_HEADER_SIZE ),
      _curCharOffset( 0 ),
      _lastTopic( NULLVAL32 ),
      _lastLink( NULLVAL32 ),
      _haveCleanedUp( false )
{
    _size = PAGE_HEADER_SIZE;
    _useCompress = ( ph != NULL );
    _phead = new PageHeader;
    _ptail = _phead;

    // Set up the text-compression facilities, if necessary.
    // Note the initial writer is a dummy, used just to get the size
    // of the compressed data.
    if( _useCompress ) {
        _myWriter = new CompWriter;
        _myReader = new CompReader( _myWriter );
    } else {
        _myWriter = NULL;
        _myReader = NULL;
    }

    // Create an initial linked-list node to work with.
    _curNode = new GenericNode( NULLVAL32 );
    _curNode->_recordType = TOP_HEADER;
    _curTopic = new TopicHeader( _numTopics++ );
    _curText = new TextHolder;
    _curPar = new TextHeader;


    // Set the initial page header.
    _phead->lastNode() = _lastTopic;
    _phead->nextNode() = _curOffset;
    _phead->lastTopic() = 0;

    // Register ourself with the directory.
    d_file->addFile( this, "|TOPIC" );
}


//  HFTopic::~HFTopic

HFTopic::~HFTopic()
{
    if( _myWriter )
        delete _myWriter;
    if( _myReader )
        delete _myReader;

    if( _lastNode != _curNode ) {
        delete _lastNode;
    }
    if( _lastHeader != _curTopic ) {
        delete _lastHeader;
    }
    delete _curNode;
    delete _curTopic;
    delete _curPar;
    delete _curText;

    // Lotsa linked lists to delete.

    TopicLink   *current;
    TopicLink   *next;
    for( current = _head; current != NULL; current = next ) {
        next = current->_next;
        delete current;
    }

    PageHeader  *pcurrent;
    PageHeader  *pnext;
    for( pcurrent = _phead; pcurrent != NULL; pcurrent = pnext ) {
        pnext = pcurrent->_next;
        delete pcurrent;
    }

    StringNode  *bcurrent;
    StringNode  *bnext;
    for( bcurrent = _bhead; bcurrent != NULL; bcurrent = bnext ) {
        bnext = bcurrent->_next;
        delete[] bcurrent->_string;
        delete bcurrent;
    }
}


//  HFTopic::size   --Overridden from Dumpable.

uint_32 HFTopic::size()
{
    if( !_haveCleanedUp ) {
        // Dump the last linked-list node, if necessary.
        newNode();
        _haveCleanedUp = true;
    }
    return _size;
}


//  HFTopic::dump   --Overridden from Dumpable.

int HFTopic::dump( OutFile * dest )
{
    // Write the last few blocks of binary data.
    if( _lastNode != NULL ) {
        _lastNode->dumpTo( _lastNode->_myLink );
    }
    if( _lastHeader != NULL ) {
        _lastHeader->dumpTo( _lastHeader->_myLink );
    }

    // Put in browse sequence information.
    dumpBrowse();

    // If we're compressing, stop the dummy compressor and start
    // the real thing.
    if( _useCompress ) {
        CompWriter  *temp = new CompOutFile( dest );
        _myReader->reset( temp );
        if( _myWriter != NULL )
            delete _myWriter;
        _myWriter = temp;
    }

    TopicLink       *current = _head;
    PageHeader      *cur_page = _phead->_next;
    dest->write( _phead->_pageNums, sizeof( uint_32 ), 3 );
    uint_32     page_size = PAGE_HEADER_SIZE;

    // Write the linked list nodes in order.
    int i;
    while( current != NULL ) {
        // At the start of a page, flush the compressor and
        // write a page header.
        if( current->_isFirstLink ) {
            HCTick();
            if( _useCompress ) {
                _myReader->flush();
            }
            while( page_size < COMP_PAGE_SIZE ) {
                dest->write( (uint_8)0 );
                page_size++;
            }
            page_size = PAGE_HEADER_SIZE;
            dest->write( cur_page->_pageNums, sizeof( uint_32 ), 3 );
            cur_page = cur_page->_next;
        }
    
        if( _useCompress ) {
            page_size += _myReader->add( current->_myData, current->_size );
    
            // "Magic" check to see if the current node is a topic header
            // or a text header.  This is the only way to do it since
            // at this stage the node is just a binary data block.
            if( current->_myData[20] == 0x02 ) {
                current = current->_next;
                page_size += _myReader->add( current->_myData, current->_size );
            } else {
                current = current->_next;
                page_size += _myReader->compress( current->_myData, current->_size );
            }
            current = current->_next;
            page_size += _myReader->compress( current->_myData, current->_size );
            current = current->_next;
        } else {
            for( i=0; i<3; i++, current=current->_next ) {
                page_size += current->_size;
                dest->write( current->_myData, 1, current->_size );
            }
        }
    }

    // If we're compressing, a few bytes may be left in the writer.
    if( _useCompress ) {
        _myReader->flush();
    }
    return 1;
}


//  HFTopic::startNonScroll --Signal the start of a non-scrolling
//                region of text.

void HFTopic::startNonScroll()
{
    _lastHeader->_startNonScroll = _curOffset;
}


//  HFTopic::startScroll    --Signal the start of a scrolling
//                region of text.

void HFTopic::startScroll()
{
    _lastHeader->_startScroll = _curOffset;
    if( _lastHeader->_startNonScroll == _curOffset ) {
        _lastHeader->_startNonScroll = NULLVAL32;
    }
}


//  HFTopic::addBrowse      --Add browse sequence information.

void HFTopic::addBrowse( char const str[] )
{
    if( _browseStr != NULL ) {
        HCWarning( TOP_TWOBROWSE, _browseStr, str );
        delete[] _browseStr;
    }
    size_t length = strlen( str ) + 1;
    _browseStr = new char[length];
    memcpy( _browseStr, str, length );
    _browseOffset = _curCharOffset;
}


//  HFTopic::recordBrowse   --Get the browse sequence info, in
//                binary form.

void HFTopic::recordBrowse( TopicLink *me )
{
    StringNode  *current;
    StringNode  *prev = NULL;
    StringNode  *newnode = new StringNode;
    newnode->_me = me;
    newnode->_charOffset = _browseOffset;
    newnode->_string = _browseStr;
    strlwr( newnode->_string );
    _browseStr = NULL;
    for( current = _bhead; current != NULL; current = current->_next ) {
        if( strcmp( current->_string, newnode->_string ) > 0 )
            break;
        prev = current;
    }

    if( current == NULL ) {
        if( prev == NULL ) {
            _bhead = _btail = newnode;
            newnode->_next = newnode->_prev = NULL;
        } else {
            prev->_next = newnode;
            newnode->_prev = prev;
            newnode->_next = NULL;
            _btail = newnode;
        }
    } else {
        newnode->_next = current;
        newnode->_prev = current->_prev;
        current->_prev = newnode;
        if( prev == NULL ) {
            _bhead = newnode;
        } else {
            prev->_next = newnode;
        }
    }
}


//  HFTopic::dumpBrowse --Add binary browse sequence info to all topics.

void HFTopic::dumpBrowse()
{
    StringNode  *current;
    StringNode  *lastlocal = NULL;
    StringNode  *lastglobal = NULL;
    char        *colonpos;
    size_t      seqlen;

    for( current = _bhead; current != NULL; current = current->_next ) {
        colonpos = strchr( current->_string, ':' );
        seqlen = colonpos - current->_string + 1;
        if( colonpos == NULL ) {
            if( lastlocal != NULL ) {
                lastlocal->nextBrowse() = NULLVAL32;
            }
            lastlocal = NULL;
            if( lastglobal != NULL ) {
                lastglobal->nextBrowse() = current->_charOffset;
                current->prevBrowse() = lastglobal->_charOffset;
            } else {
                current->prevBrowse() = NULLVAL32;
            }
            lastglobal = current;
        } else {
            if( lastlocal != NULL ) {
                if( !strncmp( lastlocal->_string, current->_string, seqlen ) ) {
                    lastlocal->nextBrowse() = current->_charOffset;
                    current->prevBrowse() = lastlocal->_charOffset;
                } else {
                    lastlocal->nextBrowse() = NULLVAL32;
                    current->prevBrowse() = NULLVAL32;
                }
            } else {
                current->prevBrowse() = NULLVAL32;
            }
            lastlocal = current;
        }
    }
    if( lastlocal != NULL ) {
        lastlocal->nextBrowse() = NULLVAL32;
    }
    if( lastglobal != NULL ) {
        lastglobal->nextBrowse() = NULLVAL32;
    }
}


//  HFTopic::newNode    --Start a new current node.  Also dump the old one,
//            update the linked list, check the compression, ...

void HFTopic::newNode( bool is_new_topic )
{
    bool        forget_node = false;
    unsigned    i;
    unsigned    j,k,zero_pos;
    FontFlags   the_flag;

    if( _curNode->_recordType == TOP_TEXT && _curPar->_numAttribs == _curText->_size ) {
        for( i = 0; i < _curPar->_numAttribs; i++ ) {
            the_flag = _curPar->_attribs[i]._type;
            if( the_flag == TOP_NEW_LINE || the_flag == TOP_NEW_PAR || the_flag == TOP_HTAB ) {
                break;
            }
        }
        if( i == _curPar->_numAttribs ) {
            forget_node = true;
        }
    }

    if( forget_node ) {
        delete _curNode;
    } else {
        // Clean up the current text block and record its size.
        if( _curNode->_recordType != TOP_HEADER ) {
            // See if any successive font changes can be combined into
            // a single change.
            for( i = 0; i + 1 < _curText->_numZeroes; i++ ) {
                if( _curPar->_attribs[i]._type != TOP_FONT_CHANGE ) {
                    continue;
                }
                zero_pos = _curText->_zeroes[i];
                j = i + 1;
                while( j < _curText->_numZeroes
                  && _curPar->_attribs[j]._type == TOP_FONT_CHANGE
                  && _curText->_zeroes[j] - zero_pos == j - i ) {
                    j++;
                }
                if( j > i + 1 ) {
                    memmove( &_curPar->_attribs[i], &_curPar->_attribs[j - 1],
                             ( _curPar->_numAttribs - j + 1 ) * sizeof( TextAttr ) );
                    _curPar->_numAttribs -= ( j - 1 - i );
                    _curPar->_size -= ( j - 1 - i ) * 3;
                    memmove( _curText->_text + _curText->_zeroes[i] + 1,
                             _curText->_text + _curText->_zeroes[j - 1] + 1,
                         _curText->_size - _curText->_zeroes[j - 1] - 1 );
                    _curText->_size -= j - 1 - i;
                    _curText->_uncompSize -= j - 1 - i;
                    _curText->_numZeroes -= static_cast<uint_16>( j - 1 - i );
                    for( k = i; k < _curText->_numZeroes; k++ ) {
                        _curText->_zeroes[k] = (uint_16)( _curText->_zeroes[k + j - 1 - i] - j + 1 + i );
                    }
                }
            }
            addAttr( TOP_END );
            _curPar->_size += _curPar->_parAttrSize;
            _curPar->_textSize = (uint_16)_curText->_size;
            _curPar->_headerSize = (uint_16)( _curPar->_size - 3 );
            if( _curPar->_textSize >= INT_SMALL_LIMIT ) {
                _curPar->_size += 1;
            }
        }
        _curNode->_dataSize = _curText->_uncompSize;

        // Create memory for the current node.
        TopicLink   *first, *second, *third;

        first = new TopicLink( _curNode->_size );
        _curNode->_myLink = first;
        if( _curNode->_recordType == TOP_HEADER ) {
            second = new TopicLink( _curTopic->_size );
            _curTopic->_myLink = second;
        } else {
            second = new TopicLink( _curPar->_size );
            _curPar->dumpTo( second );
        }
        third = new TopicLink( _curText->_size );
        _curText->dumpTo( third );

        // Update a few more size and linked list variables.
        _curNode->_topicSize = first->_size + second->_size + third->_size;
        _curNode->_dataOffset = first->_size + second->_size;
        _curTopic->_totalSize += first->_size + second->_size + third->_size;

        // See if the current page can hold this node.
        if( _useCompress ) {
            if( _curNode->_recordType == TOP_HEADER ) {
                _size += _myReader->skip( first->_size + second->_size );
            } else {
                _size += _myReader->skip( first->_size );
                _size += _myReader->compress( second->_myData, second->_size );
            }
            _size += _myReader->compress( third->_myData, third->_size );
        } else {
            _size += first->_size + second->_size + third->_size;
        }

        // Create a new page, if necessary.
        if( _size >= _numPages * COMP_PAGE_SIZE ) {
            _ptail->_next = new PageHeader;
            _ptail->_next->nextNode() = _ptail->nextNode() + (COMP_PAGE_SIZE << 2);
            _ptail = _ptail->_next;
            _ptail->lastNode() = _lastLink;
            _ptail->lastTopic() = _lastTopic;
            if( _curTopic->_startNonScroll == _curOffset ) {
                _curTopic->_startNonScroll = _ptail->nextNode();
            }
            if( _curTopic->_startScroll == _curOffset ) {
                _curTopic->_startScroll = _ptail->nextNode();
            }
            _curOffset = _ptail->nextNode();
            if( _browseOffset == _curCharOffset ) {
                _browseOffset = ( _curOffset & ~((COMP_PAGE_SIZE << 2) - 1) ) << 1;
            }
            _curCharOffset = ( _curOffset & ~((COMP_PAGE_SIZE << 2) - 1) ) << 1;
            _size = (_numPages++) * COMP_PAGE_SIZE + PAGE_HEADER_SIZE;
            if( _useCompress ) {
                _myReader->flush();
                if( _curNode->_recordType == TOP_HEADER ) {
                    _size += _myReader->skip( first->_size + second->_size );
                } else {
                    _size += _myReader->skip( first->_size );
                    _size += _myReader->compress( second->_myData, second->_size );
                }
                _size += _myReader->compress( third->_myData, third->_size );
            } else {
                _size += first->_size + second->_size + third->_size;
            }
            first->_isFirstLink = true;
        }

        // Update linked list pointers in old nodes we've been saving,
        // and dump them at last.

        _lastLink = _curOffset;
        if( _lastNode != NULL ) {
            _lastNode->_nextNode = _curOffset;
            _lastNode->dumpTo( _lastNode->_myLink );
            delete _lastNode;
        }
        _lastNode = _curNode;
        if( _curNode->_recordType == TOP_HEADER ) {
            _lastTopic = _curOffset;
            if( _lastHeader != NULL ) {
                _lastHeader->_nextTopic = _curOffset;
                _lastHeader->dumpTo( _lastHeader->_myLink );
                delete _lastHeader;
            }
            _lastHeader = _curTopic;
        }

        // Increment the 'current location' counters.
        _curOffset += first->_size + second->_size + third->_size;
        if( _curNode->_recordType != TOP_HEADER ) {
            _curCharOffset += third->_size;
        }

        // Add the new data blocks to the linked list of such blocks.
        if( _tail != NULL ) {
            _tail->_next = first;
        } else {
            _head = first;
        }
        first->_next = second;
        second->_next = third;
        third->_next = NULL;
        _tail = third;
    }

    // Create the new current node.
    _curNode = new GenericNode( _lastLink );
    if( is_new_topic ) {
        if( _lastHeader != NULL && _browseStr != NULL ) {
            recordBrowse( _lastHeader->_myLink );
            _browseStr = NULL;
        }
        _curTopic = new TopicHeader( _numTopics++ );
        _curNode->_recordType = TOP_HEADER;
    } else {
        _curPar->reset();
        _curNode->_recordType = TOP_TEXT;
    }
    _curText->reset();
}


//  HFTopic::addText    --Adds text (assumed to be zero-terminated)
//            to the |TOPIC file.

void HFTopic::addText( char const source[], bool use_phr )
{
    int      length;

    if( source[0] == '\0' ) {
        length = 1;
    } else {
        length = strlen( source );
    }
    if( length + _curText->_size > _curText->_maxSize ) {
        _curText->_maxSize = (length + _curText->_size) / TEXT_BLOCK_SIZE + 1;
        _curText->_maxSize *= TEXT_BLOCK_SIZE;
        _curText->_text.resize( _curText->_maxSize );
    }

    _curText->_uncompSize += length;

    // Use phrase replacement, if appropriate.
    if( use_phr && _useCompress ) {
        _phFile->replace( _curText->_text+_curText->_size, source, length );
    } else {
        memcpy( _curText->_text + _curText->_size, source, length );
    }

    if( length + _curText->_size > COMP_PAGE_SIZE ) {
        HCError( TOP_TOOLARGE );
    }
    _curText->_size += length;
}


//  HFTopic::addZero    --Add a 0x00 byte (to signal a format change) to
//            the current text buffer.

void HFTopic::addZero( unsigned index )
{
    int zero_pos;
    if( _curText->_size == COMP_PAGE_SIZE ) {
        HCError( TOP_TOOLARGE );
    }
    if( _curText->_numZeroes == _curText->_maxZeroes ) {
        _curText->_maxZeroes += TEXT_ZERO_SIZE;
        _curText->_zeroes.resize( _curText->_maxZeroes );
    }
    if( _curText->_size == _curText->_maxSize ) {
        _curText->_maxSize += TEXT_BLOCK_SIZE;
        _curText->_text.resize( _curText->_maxSize );
    }
    if( index < _curText->_numZeroes ) {
        zero_pos = _curText->_zeroes[index] + 1;
        char    *position = _curText->_text + zero_pos;
        memmove( position + 1, position, _curText->_size - zero_pos );
        memmove( _curText->_zeroes + index + 2, _curText->_zeroes + index + 1, ( _curText->_numZeroes - index - 1 ) * 2 );
        _curText->_zeroes[index + 1] = (uint_16)zero_pos;
    } else {
        zero_pos = _curText->_size;
        _curText->_zeroes[_curText->_numZeroes] = (uint_16)zero_pos;
    }
    _curText->_text[zero_pos] = '\0';
    _curText->_size += 1;
    _curText->_uncompSize += 1;
    _curText->_numZeroes += 1;
}


//  HFTopic::presentSize    --Access function.

uint_32 HFTopic::presentSize()
{
    uint_32 result = _curNode->_size + _curText->_size;
    if( _curNode->_recordType == TOP_HEADER ) {
        result += _curTopic->_size;
    } else {
        result += _curPar->_size + _curPar->_parAttrSize;
    }
    return result;
}


//  What follows are several HFTopic member functions used as front
//  ends to the corresponding TextHeader member functions.

unsigned HFTopic::addAttr( FontFlags type, uint_32 val )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    // This had better not be a variable length attribute.
    switch( type ) {
    case TOP_MACRO_LINK:
    case TOP_MACRO_INVIS:
    case TOP_POPUP_FILE:
    case TOP_JUMP_FILE:
    case TOP_POPUP_FILE_INVIS:
    case TOP_JUMP_FILE_INVIS:
        HCError( HLP_ATTR );
        break;
    }
    addZero( _curPar->_numAttribs );
    return _curPar->addAttr( type, val, NULL, 0 );
}

unsigned HFTopic::addAttr( FontFlags type, char const str[], uint_16 len )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    switch( type ) {
    case TOP_MACRO_LINK:
    case TOP_MACRO_INVIS:
        break;

    default:
        HCError( HLP_ATTR );
        break;
    }

    addZero( _curPar->_numAttribs );
    return _curPar->addAttr( type, 0, str, len );
}

unsigned HFTopic::addAttr( FontFlags type, char const str[], uint_16 len, uint_32 val )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    switch( type ) {
    case TOP_JUMP_FILE:
    case TOP_POPUP_FILE:
    case TOP_JUMP_FILE_INVIS:
    case TOP_POPUP_FILE_INVIS:
        break;

    default:
        HCError( HLP_ATTR );
    }

    addZero( _curPar->_numAttribs );
    return _curPar->addAttr( type, val, str, len );
}

unsigned HFTopic::appendAttr( unsigned index, FontFlags type, uint_32 val )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    // This had better not be a variable length attribute.
    switch( type ) {
    case TOP_POPUP_LINK:
    case TOP_JUMP_LINK:
    case TOP_POPUP_INVIS:
    case TOP_JUMP_INVIS:
        break;

    default:
        HCError( HLP_ATTR );
    }

    unsigned result = _curPar->appendAttr( index, type, val, NULL, 0 );
    addZero( index );
    return result;
}

unsigned HFTopic::appendAttr( unsigned index, FontFlags type, char const str[], uint_16 len )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    switch( type ) {
    case TOP_MACRO_LINK:
    case TOP_MACRO_INVIS:
        break;

    default:
        HCError( HLP_ATTR );
    }

    unsigned result = _curPar->appendAttr( index, type, 0, str, len );
    addZero( index );
    return result;
}

unsigned HFTopic::appendAttr( unsigned index, FontFlags type, char const str[], uint_16 len, uint_32 val )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    switch( type ) {
    case TOP_JUMP_FILE:
    case TOP_POPUP_FILE:
    case TOP_JUMP_FILE_INVIS:
    case TOP_POPUP_FILE_INVIS:
        break;

    default:
        HCError( HLP_ATTR );
    }

    unsigned result = _curPar->appendAttr( index, type, val, str, len );
    addZero( index );
    return result;
}

void HFTopic::chgAttr( unsigned index, FontFlags type, uint_32 val )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    // This had better not be a variable length attribute.
    switch( type ) {
    case TOP_MACRO_LINK:
    case TOP_MACRO_INVIS:
    case TOP_POPUP_FILE:
    case TOP_JUMP_FILE:
    case TOP_POPUP_FILE_INVIS:
    case TOP_JUMP_FILE_INVIS:
        HCError( HLP_ATTR );
        break;
    }

    _curPar->chgAttr( index, type, val, NULL, 0 );
}

void HFTopic::chgAttr( unsigned index, FontFlags type, char const str[], uint_16 len )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    switch( type ) {
    case TOP_MACRO_LINK:
    case TOP_MACRO_INVIS:
        break;

    default:
        HCError( HLP_ATTR );
    }

    _curPar->chgAttr( index, type, 0, str, len );
}

void HFTopic::chgAttr( unsigned index, FontFlags type, char const str[], uint_16 len, uint_32 val )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    switch( type ) {
    case TOP_POPUP_FILE:
    case TOP_JUMP_FILE:
    case TOP_POPUP_FILE_INVIS:
    case TOP_JUMP_FILE_INVIS:
        break;

    default:
        HCError( HLP_ATTR );
    }

    _curPar->chgAttr( index, type, val, str, len );
}

uint_32 HFTopic::attrData( unsigned index )
{
    if( _curNode->_recordType == TOP_HEADER ) {
        HCError( HLP_ATTR );
    }

    return _curPar->attrData( index );
}

int HFTopic::setTab( int val, TabTypes flag )
{
    return _curPar->setTab( val, (uint_8) flag );
}

int HFTopic::setPar( ParFlags type, int val )
{
    return _curPar->setPar( type, val );
}

void HFTopic::unsetPar( ParFlags type )
{
    _curPar->unsetPar( type );
}

void HFTopic::clearPar()
{
    _curPar->clearPar();
}

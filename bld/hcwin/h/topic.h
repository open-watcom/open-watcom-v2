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


//
//
//  TOPIC.H -- Classes to write the |TOPIC internal file in a .HLP file.
//             For the Watcom .HLP compiler.
//
//
#ifndef TOPIC_H
#define TOPIC_H

#include <watcom.h>             // for uint_8, etc.
#include "myfile.h"             // for class Dumpable, Outfile
#include "hlpdir.h"             // for class HFSDirectory
#include "compress.h"           // for class CompWriter, CompReader
#include "phrase.h"             // for class HFPhrases


// Types of records in the TOPIC file.

typedef enum {
    TOP_HEADER  = 0x02,         // topic header data
    TOP_TEXT    = 0x20,         // text header data
    TOP_TABLE   = 0x23          // table header data, not implemented yet
} RecordType;


//
//  FontFlags   --Special text changes.
//

#define FONT_FLAGS_DEFS() \
    FONT_FLAGS_DEF( TOP_FONT_CHANGE,      0x80, 3 ) \
    FONT_FLAGS_DEF( TOP_NEW_LINE,         0x81, 1 ) \
    FONT_FLAGS_DEF( TOP_NEW_PAR,          0x82, 1 ) \
    FONT_FLAGS_DEF( TOP_HTAB,             0x83, 1 ) \
    FONT_FLAGS_DEF( TOP_CENT_BITMAP,      0x86, 9 ) \
    FONT_FLAGS_DEF( TOP_LEFT_BITMAP,      0x87, 9 ) \
    FONT_FLAGS_DEF( TOP_RIGHT_BITMAP,     0x88, 9 ) \
    FONT_FLAGS_DEF( TOP_END_LINK,         0x89, 1 ) \
    FONT_FLAGS_DEF( TOP_MACRO_LINK,       0xC8, 3 ) \
    FONT_FLAGS_DEF( TOP_MACRO_INVIS,      0xCC, 3 ) \
    FONT_FLAGS_DEF( TOP_POPUP_LINK,       0xE2, 5 ) \
    FONT_FLAGS_DEF( TOP_JUMP_LINK,        0xE3, 5 ) \
    FONT_FLAGS_DEF( TOP_POPUP_INVIS,      0xE6, 5 ) \
    FONT_FLAGS_DEF( TOP_JUMP_INVIS,       0xE7, 5 ) \
    FONT_FLAGS_DEF( TOP_POPUP_FILE,       0xEA, 7 ) \
    FONT_FLAGS_DEF( TOP_JUMP_FILE,        0xEB, 7 ) \
    FONT_FLAGS_DEF( TOP_POPUP_FILE_INVIS, 0xEE, 7 ) \
    FONT_FLAGS_DEF( TOP_JUMP_FILE_INVIS,  0xEF, 7 ) \
    FONT_FLAGS_DEF( TOP_END,              0xFF, 1 )

typedef enum {
    #define FONT_FLAGS_DEF(num,bits,sizes) num,
    FONT_FLAGS_DEFS()
    #undef FONT_FLAGS_DEF
    NOT_A_BITMAP
} FontFlags;


//
//  ParFlags    --Paragraph attribute flags.
//
//  The bitfields corresponding to paragraph attributes.

#define PAR_FLAGS_DEFS() \
    PAR_FLAGS_DEF( TOP_SPACE_BEFORE, 0x00020000 ) \
    PAR_FLAGS_DEF( TOP_SPACE_AFTER,  0x00040000 ) \
    PAR_FLAGS_DEF( TOP_LINE_SPACE,   0x00080000 ) \
    PAR_FLAGS_DEF( TOP_LEFT_INDENT,  0x00100000 ) \
    PAR_FLAGS_DEF( TOP_RIGHT_INDENT, 0x00200000 ) \
    PAR_FLAGS_DEF( TOP_FIRST_INDENT, 0x00400000 ) \
    PAR_FLAGS_DEF( TOP_BORDER,       0x01000000 ) \
    PAR_FLAGS_DEF( TOP_TAB_STOPS,    0x02000000 ) \
    PAR_FLAGS_DEF( TOP_RIGHT_JUST,   0x04000000 ) \
    PAR_FLAGS_DEF( TOP_CENTRE_JUST,  0x08000000 ) \
    PAR_FLAGS_DEF( TOP_NO_LINE_WRAP, 0x10000000 )

typedef enum {
    #define PAR_FLAGS_DEF(num,bits) num,
    PAR_FLAGS_DEFS()
    #undef PAR_FLAGS_DEF
} ParFlags;


//
//  TabTypes    --Tab stop flags
//

typedef enum {
    TAB_LEFT    = 0x00,
    TAB_RIGHT   = 0x02,
    TAB_CENTER  = 0x04
} TabTypes;


// Various forward declarations.
struct TopicLink;
struct StringNode;
struct PageHeader;
class GenericNode;
class TopicHeader;
class TextHeader;
class TextHolder;


//
//  HFTopic     --Class to write the |TOPIC file.
//

class HFTopic : public Dumpable
{
    uint_32     _size;
    unsigned    _numPages;
    unsigned    _numTopics;
    bool        _useCompress;
    TopicLink   *_head, *_tail;
    PageHeader  *_phead, *_ptail;

    CompWriter  *_myWriter;
    CompReader  *_myReader;
    HFPhrases   *_phFile;

    // The current linked list node under construction.
    GenericNode *_curNode;
    TopicHeader *_curTopic;
    TextHeader  *_curPar;
    TextHolder  *_curText;

    // Some stuff for handling browse sequences.
    char        *_browseStr;
    uint_32     _browseOffset;
    StringNode  *_bhead, *_btail;

    void         recordBrowse( TopicLink *me );
    void         dumpBrowse();

    // Some pointers to old nodes in case linked-list offsets
    // ever need to be changed.
    GenericNode *_lastNode;
    TopicHeader *_lastHeader;

    uint_32     _curOffset;
    uint_32     _curCharOffset;
    uint_32     _lastTopic;
    uint_32     _lastLink;

    bool        _haveCleanedUp;

    // A private function called by the appendAttr() functions.
    void        addZero( size_t index );

    // Assignment of HFTopic's is not allowed.
    HFTopic( HFTopic const & ) {};
    HFTopic &   operator=( HFTopic const & ) { return *this; };

public:
    HFTopic( HFSDirectory * d_file, HFPhrases *ph = NULL );
    ~HFTopic();

    // access functions.
    uint_32     offset() { return _curOffset; };
    uint_32     charOffset() { return _curCharOffset; };
    uint_32     presentSize();

    void        addBrowse( char const str[] );

    void        startNonScroll();
    void        startScroll();
    void        newNode( bool is_new_topic = false );

    // Function to change paragraph attributes.
    int         setTab( int val, TabTypes type = TAB_LEFT );
    int         setPar( ParFlags type, int val=0 );
    void        unsetPar( ParFlags type );
    void        clearPar();

    // Various ways to add things to the |TOPIC file.
    void        addText( char const source[], bool use_phr=false );
    unsigned    addAttr( FontFlags type, uint_32 val=0 );
    unsigned    addAttr( FontFlags type, char const str[], uint_16 len );
    unsigned    addAttr( FontFlags type, char const str[], uint_16 len, uint_32 val );
    unsigned    appendAttr( unsigned index, FontFlags type, uint_32 val );
    unsigned    appendAttr( unsigned index, FontFlags type, char const str[], uint_16 len );
    unsigned    appendAttr( unsigned index, FontFlags type, char const str[], uint_16 len, uint_32 val );
    void        chgAttr( unsigned index, FontFlags type, uint_32 val=0 );
    void        chgAttr( unsigned index, FontFlags type, char const str[], uint_16 len );
    void        chgAttr( unsigned index, FontFlags type, char const str[], uint_16 len, uint_32 val );

    uint_32     attrData( unsigned index );

    // Overrides of the "Dumpable" virtual functions.
    uint_32     size();
    int         dump( OutFile * dest );
};

#endif

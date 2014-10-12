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

enum
{
    TOP_HEADER  = 0x02,         // topic header data
    TOP_TEXT    = 0x20,         // text header data
    TOP_TABLE   = 0x23          // table header data, not implemented yet
};


//
//  FontFlags   --Special text changes.
//

enum FontFlags
{
    TOP_FONT_CHANGE     = 0,
    TOP_NEW_LINE        ,
    TOP_NEW_PAR         ,
    TOP_HTAB            ,
    TOP_CENT_BITMAP     ,
    TOP_LEFT_BITMAP     ,
    TOP_RIGHT_BITMAP    ,
    TOP_END_LINK        ,
    TOP_MACRO_LINK      ,
    TOP_MACRO_INVIS     ,
    TOP_POPUP_LINK      ,
    TOP_JUMP_LINK       ,
    TOP_POPUP_INVIS     ,
    TOP_JUMP_INVIS      ,
    TOP_POPUP_FILE      ,
    TOP_JUMP_FILE       ,
    TOP_POPUP_FILE_INVIS,
    TOP_JUMP_FILE_INVIS ,
    TOP_END
};


//
//  ParFlags    --Paragraph attribute flags.
//

enum ParFlags {
    TOP_SPACE_BEFORE    = 0,
    TOP_SPACE_AFTER     ,
    TOP_LINE_SPACE      ,
    TOP_LEFT_INDENT     ,
    TOP_RIGHT_INDENT    ,
    TOP_FIRST_INDENT    ,
    TOP_BORDER          ,
    TOP_TAB_STOPS       ,
    TOP_RIGHT_JUST      ,
    TOP_CENTRE_JUST     ,
    TOP_NO_LINE_WRAP
};


//
//  TabTypes    --Tab stop flags
//

enum TabTypes {
    TAB_LEFT    = 0x00,
    TAB_RIGHT   = 0x02,
    TAB_CENTER  = 0x04
};


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
    int         _useCompress;
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

    int         _haveCleanedUp;

    // A private function called by the appendAttr() functions.
    void        addZero( unsigned index );

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
    int         setTab( int val, TabTypes flags = TAB_LEFT );
    int         setPar( ParFlags type, int val=0 );
    void        unsetPar( ParFlags type );
    void        clearPar();

    // Various ways to add things to the |TOPIC file.
    void        addText( char const source[], int use_phr=0 );
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

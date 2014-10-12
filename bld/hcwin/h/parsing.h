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
//  parsing.h       Parse (okay, scan) a .RTF file.
//          For the WATCOM .HLP compiler.
//
#ifndef PARSING_H
#define PARSING_H

#include "topic.h"
#include "system.h"
#include "font.h"
#include "context.h"
#include "keywords.h"
#include "ttlbtree.h"
#include "bmx.h"
#include "scanning.h"
#include "hcmem.h"


// Extension of an RTF file.
extern char const   RtfExt[];


//
//  RTFparser   --A nice BIG module for parsing RTF files
//        as .HLP source files.
//

class RTFparser
{
    // Pointers to possible output destinations.
    HFTopic *_topFile;
    HFFont  *_fontFile;
    HFContext   *_hashFile;
    HFKwbtree   *_keyFile;
    HFTtlbtree  *_titleFile;
    HFBitmaps   *_bitFiles;
    HFSystem    *_sysFile;

    // Info about the input.
    Scanner *_input;
    char    *_fname;

    // Some state information, used when processing hotlinks
    // and non-scrolling areas.
    enum { POPUP, JUMP, MACRO } _linkType;
    enum { HEADER, NON_SCROLL, SCROLL } _writeState;
    int     _nestLevel;
    int     _hotlinkStart;

    // An all-purpose string buffer.
    Buffer<char>    _storage;
    int     _storSize, _maxStor;

    Token   *_current;
    bool    _wereWarnings;

    // The current "tab stop type".
    TabTypes    _tabType;

    // State information pertaining to fonts.
    uint_16 _curFont;
    uint_16 _lastFont;
    uint_16 _defFont;
    int     _usingDefault;

    // Fun functions.
    void    skipSection();      // Skip a block.
    void    handleCommand();    // Process non-font commands.
    void    handleFonts();      // Process the font table.
    void    handleFootnote( char Fchar );   // Process footnotes.
    void    handleHidden( int IsHotLink );  // Process hidden text.
    uint_16 closeBraces();      // Concatenate successive font changes.

    // Identify and process font commands.
    bool    isFontCommand( Token * tok, uint_16 *newfont );

    // Identify (but don't process) paragraph commands.
    bool    isParCommand();

    // Utility function
    char *  skipSpaces( char * start );

    // Assignment of RTFparser's is not allowed.
    RTFparser( RTFparser const & ) : _storage(0) {};
    RTFparser & operator=( RTFparser const & ) { return *this; };

public:
    RTFparser( Pointers *p, InFile *src );
    ~RTFparser();

    void Go();   // This is the 'main loop' function.
};

#endif

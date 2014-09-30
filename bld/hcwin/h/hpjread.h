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
//  hpjread.h   --Class to read and parse .HPJ files.
//                For the Watcom .HLP compiler.
//
#ifndef  HPJREAD_H
#define  HPJREAD_H

#include <watcom.h>
#include "hlpdir.h"
#include "system.h"
#include "baggage.h"
#include "context.h"
#include "ctxomap.h"


// Extension of a .HPJ file.
extern char const   HpjExt[];


//
//  HPJScanner  --Simple scanner class, suited to reading .HPJ files.
//

class HPJScanner
{
    InFile  *_input;

    char    *_curLine;
    int     _lineSize;
    int     _lineNum;

    // Some buffering is needed for the "tokLine" function.
    int     _bufPos;
    char    _bufChar;

    // Assignment of HPJScanner's is not allowed.
    HPJScanner( HPJScanner const & ) {};
    HPJScanner &    operator=( HPJScanner const & ) { return *this; };

public:
    HPJScanner( InFile *src );
    ~HPJScanner();

    // Functions to access the raw data.
    int     lineNum() { return _lineNum; };
    char    &operator[]( int i ) { return _curLine[i]; };
    operator char *() { return _curLine; };

    int     getLine();          // Get a new line.
    char    *getArg( int start_pos );   // Read an argument.
    char    *tokLine();         // Tokenize a line.
    char    *endTok();

    int     open( char const filename[] );
    void    close() { _input->close(); };
    char const  *name() const { return _input->name(); };
};


//
//  HPJReader   --Class to parse .HPJ files.
//

class HPJReader
{
    // Structure for storing lists of names (like search paths)
    struct  StrNode
    {
    char    *_name;
    StrNode *_next;
    };

    HPJScanner      _scanner;

    HFSystem        *_sysFile;
    HFSDirectory    *_dir;      // The directory to send info to.
    Pointers        *_theFiles; // Other components of the .HLP file.

    StrNode *_root;     // The main search path.
    StrNode *_rtfFiles; // List of rtf files to compile.

    char    *_homeDir;  // The home directory (used when searching)

    int     _oldPhrases;    // Flag -- Use old phrase table?

    Baggage     **_bagFiles;    // List of baggage files.
    int         _numBagFiles;   // number of baggage files specified.

    int     skipSection();      // Skip a section of the file.
    int     handleBaggage();    // Read the [Baggage] section.
    int     handleOptions();    // Read the [Options] section.
    int     handleConfig();     // Read the [Config] section.
    int     handleFiles();      // Read the [Files] section.
    int     handleMap();        // Read the [Map] section.
    int     handleBitmaps();    // Read the [Bitmaps] section.
    int     handleWindows();    // Read the [Windows] section.

    char    *_winParamBuf;
    char    *nextWinParam();    // Helper function for handleWindows().

    static InFile   *firstFile();
    static InFile   *nextFile();

    static StrNode  *_topFile, *_curFile;
    static StrNode  *_firstDir;
    static char const   *_startDir;

    // A recursive function for dealing with nested MAP files.
    void    includeMapFile( char i_str[] );

    // Assignment of HPJReader's is not allowed.
    HPJReader( HPJReader const & ) : _scanner(NULL) {};
    HPJReader & operator=( HPJReader const & ) { return *this; };

public:
    HPJReader( HFSDirectory *d_file, Pointers *other_files,
               InFile *src );
    ~HPJReader();

    // The "Do it!" function.
    void parseFile();
};

#endif

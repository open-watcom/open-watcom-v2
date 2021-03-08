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


// Extension of a files.
#define HPJ_EXT     "hpj"
#define HLP_EXT     "hlp"
#define RTF_EXT     "rtf"
#define PH_EXT      "ph"

#define CMPFEXT(e,c)    (e[0] == '.' && stricmp(e + 1, c) == 0)

//
//  HPJScanner  --Simple scanner class, suited to reading .HPJ files.
//

class HPJScanner
{
    InFile          *_input;

    Buffer<char>    _curLine;
    size_t          _lineSize;
    int             _lineNum;

    // Some buffering is needed for the "tokLine" function.
    size_t          _bufPos;
    char            _bufChar;

    // Assignment of HPJScanner's is not allowed.
    HPJScanner( HPJScanner const & ) : _curLine( 0 ) {};
    HPJScanner &    operator=( HPJScanner const & ) { return *this; };

public:
    HPJScanner( InFile *src );
    ~HPJScanner() {};

    // Functions to access the raw data.
    int             lineNum() { return _lineNum; };
    char            &operator[]( size_t i ) { return _curLine[i]; };
    operator char *() { return _curLine; };

    size_t          getLine();                      // Get a new line.
    char            *getArg( size_t start_pos );    // Read an argument.
    char            *tokLine();                     // Tokenize a line.
    char            *endTok();

    void            chkLineSize( size_t size );     // Check line size.

    bool            open( char const filename[] );
    void            close() { _input->close(); };
    char const      *name() const { return _input->name(); };
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

    StrNode         *_root;     // The main search path.
    StrNode         *_rtfFiles; // List of rtf files to compile.

    char            *_homeDir;  // The home directory (used when searching)

    bool            _oldPhrases;    // Flag -- Use old phrase table?

    Baggage         **_bagFiles;    // List of baggage files.
    int             _numBagFiles;   // number of baggage files specified.

    size_t  skipSection();      // Skip a section of the file.
    size_t  handleBaggage();    // Read the [Baggage] section.
    size_t  handleOptions();    // Read the [Options] section.
    size_t  handleConfig();     // Read the [Config] section.
    size_t  handleFiles();      // Read the [Files] section.
    size_t  handleMap();        // Read the [Map] section.
    size_t  handleBitmaps();    // Read the [Bitmaps] section.
    size_t  handleWindows();    // Read the [Windows] section.

    char    *_winParamBuf;
    char    *nextWinParam();    // Helper function for handleWindows().

    static bool         firstFile( InFile * );
    static bool         nextFile( InFile * );

    static StrNode      *_topFile;
    static StrNode      *_curFile;
    static StrNode      *_firstDir;
    static char const   *_startDir;

    // A recursive function for dealing with nested MAP files.
    void    includeMapFile( char *str );

    // Assignment of HPJReader's is not allowed.
    HPJReader( HPJReader const & ) : _scanner(NULL) {};
    HPJReader & operator=( HPJReader const & ) { return *this; };

public:
    HPJReader( HFSDirectory *d_file, Pointers *other_files, InFile *src );
    ~HPJReader();

    // The "Do it!" function.
    void parseFile();
};

#endif

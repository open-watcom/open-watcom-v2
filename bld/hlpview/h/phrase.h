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
//  phrase.hpp  Phrase replacement algorithm.
//
#ifndef PHRASE_H
#define PHRASE_H

#include <string.h>
#include "scanning.h"
#include "hlpdir.h" // for class HFSDirectory.


// Extension for a phrase table file.
extern char const   PhExt[];


// Forward declarations.
class PTable;
struct P_String;


//
//  HFPhrases   --Class to create the |Phrases file in a windows help file.
//

class HFPhrases : public Dumpable
{
    PTable  *_oldPtable;
    PTable  *_newPtable;

    P_String    **_result;
    P_String    **_htable;
    int     _resultSize;

    uint_16 _numPhrases;
    uint_32 _phSize;
    uint_32 _size;

    InFile* (*_nextf)();
    InFile* (*_firstf)();

    Scanner *_scanner;
    void    startInput();
    char*   nextInput();
    void    initHashTable();

    // Assignment of HFPhrases is not allowed.
    HFPhrases( HFPhrases const & ){};
    HFPhrases & operator=( HFPhrases const & ){ return *this; };

public:
    HFPhrases( HFSDirectory * d_file, InFile* (*firstf)(),
               InFile* (*nextf)() );
    ~HFPhrases();

    // Overrides of the "Dumpable" virtual functions.
    uint_32 size();
    int     dump( OutFile * dest );

    // Functions of the phrase handler.
    void    readPhrases();
    void    createQueue( char const *path );
    int     oldTable( char const *path );

    void    replace( char *dst, char const *src, int &len );
};

#endif

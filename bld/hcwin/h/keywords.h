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
//  keywords.h  --Class to write the |KW* internal files of a .HLP file.
//                For the Watcom .HLP compiler.
//
#ifndef  KEYWORDS_H
#define  KEYWORDS_H

#include <watcom.h>
#include "myfile.h"
#include "hlpdir.h"
#include "btrees.h"

// Forward declarations.
class HFKwdata;
class HFKwmap;


//
//  HFKwbtree   --Class to write the |KWBTREE file.
//

class HFKwbtree : public Dumpable
{
    Btree       *_words;
    HFKwdata    *_dataFile;
    HFKwmap     *_mapFile;
    int         _haveSetOffsets;
    static char const   _keyMagic[];

    // Assignment of HFKwbtree's is not allowed.
    HFKwbtree( HFKwbtree const & ) {};
    HFKwbtree & operator=( HFKwbtree const & ) { return *this; };

public:
    HFKwbtree( HFSDirectory * d_file );
    ~HFKwbtree();

    // Access function.
    Btree       *words() { return _words; };

    // Overrides of the Dumpable virtual functions.
    uint_32     size();
    int         dump( OutFile * dest );

    void addKW( char const keyword[], uint_32 offset );
};


//
//  HFKwdata    --Class to write the |KWDATA file.
//

class HFKwdata : public Dumpable
{
    HFKwbtree   *_myTree;
    BtreeIter   _iterator;
    uint_32     _size;
public:
    HFKwdata( HFSDirectory * d_file, HFKwbtree * tree );

    uint_32     size();                 // Overrides Dumpable::size
    int         dump( OutFile * dest ); // Overrides Dumpable::dump
};


//
//  HFKwmap     --Class to write the |KWMAP file.
//

class HFKwmap : public Dumpable
{
    HFKwbtree   *_myTree;
    BtreeIter   _iterator;
    uint_16     _numRecs;
public:
    HFKwmap( HFSDirectory * d_file, HFKwbtree * tree );

    uint_32     size();                 // Overrides Dumpable::size
    int         dump( OutFile * dest ); // Overrides Dumpable::dump
};

#endif

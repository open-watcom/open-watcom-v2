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


#ifndef _SYMBOL_INFO_H
#define _SYMBOL_INFO_H

// System includes --------------------------------------------------------

#include <wstd.h>
#include <wcvector.h>

// Project includes -------------------------------------------------------

#include "mempool.h"
#include "ref.h"
#include "dbgobj.h"
#include "abbrev.h"

/*
 | Note that this class is used in two different contexts.
 | First, in the defined symbol table (created in pass 1),
 | and secondly in pass 2 (in many places).
 |
 | Note that in pass 2, we make use of the symbols in the defined
 | symbol table in a tricky way.  We need a reference list to keep
 | track of what references are waiting to be updated when the symbol
 | is sent to the symbol stream -- instead of creating a separate list
 | we use the RefVector for the symbol in the defined symbol table.
 |
 | This differs from how symbols in pass2 use the RefVector elsewhere -- ie.
 | used to keep a list of references in that symbol which may need to be
 | updated.
 */
class SymbolInfo : public DebugObject {
public:
                        SymbolInfo( int mbrIndex,
                                    uint_32 offset = 0,
                                    uint_8 * data = NULL,
                                    Abbreviation * ab = NULL,
                                    char * name = NULL,
                                    bool isExternal = TRUE );
    virtual             ~SymbolInfo();
    void                setTag( uint_32 );
    void                setAbbrev( Abbreviation * );
    void                setNewOffset( uint_32 );
    void                setFileIndex( int );
    void                setLength( uint_32 );
    void                setSiblingRef( MergeReference * );
    void                setHasChildren( bool );
    bool                hasChildren();
    void                addReference( MergeReference * );
    uint_8 *            data();
    Abbreviation *      abbrev();
    uint_32             tag();
    uint_32             length();
    int                 mbrIndex();
    int                 fileIndex();
    uint_32             oldOffset();
    uint_32             newOffset();
    RefVector &         refs();
    bool                isExternal();
    int                 numUnresolved();
    void                referenceResolved();
    MergeReference *    siblingRef();

    #if DEBUG_DUMP
    void                dumpData();
    #endif

    void *              operator new( size_t );
    void                operator delete( void * );

protected:
    uint_8 *            _data;                  // raw data assoc. with DIE
                                                // don't delete it, since it's
                                                // allocated elsewhere as part
                                                // of a larger buffer

    uint_32             _length;                // length of raw data
    uint_32             _tag;                   // Symbol's tag
    bool                _isExternal;            // an external sym?
    int                 _mbrIndex;              // file associated with DIE
    uint_32             _oldOffset;             // position
    uint_32             _newOffset;
    RefVector           _refs;                  // references in DIE
    MergeReference *    _siblingRef;            // ref to sibling, NULL if none
    Abbreviation *      _abbrev;                // abbrev for the symbol
    int                 _unresolvedRefCount;    // # of refs left to resolve
    int                 _fileIndex;             // file that sym is defined in
    bool                _hasChildren;           // sibling flag

    static MemoryPool   _pool;
};

typedef WCValOrderedVector<SymbolInfo *> SymbolVector;

#endif

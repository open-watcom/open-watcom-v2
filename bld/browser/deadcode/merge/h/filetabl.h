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


// System includes --------------------------------------------------------

#include <string.h>

// Project includes -------------------------------------------------------

#include "mstrtble.h"
#include "replace.h"
#include "anylist.h"

/*
 * When merging two .debug_line sections together, we want to combine
 * the filename ( or directory name ) information.
 *
 * Possible methods that we may want to use are :
 * - Add a File/Dir : We have two cases.  If it already is in the _strTable
 *                    then we need to just add the required entry to the
 *                    replacement table.
 *
 *                    If it's not in _strTable, we add it there and add an
 *                    entry to the replacement table.  Also add to
 *                    _globalTable.
 *
 * - Queries :
 *   - Find by filename
 *       ...Search in _strTable, return ptr to data
 *   - Find by mbrfile # and index
 *       ...Use _repTable, return new index
 *   - Find by global index
 *       ...Use _globalTable, return ptr to data
 */

class FileTable {
public:
                            FileTable();
                            ~FileTable();

    bool                    add( int mbrIndex, char * name, void * data );

    void *                  find( char * name );
    int                     find( int mbrIndex, int subIndex );
    void *                  find( int globalIndex );
    void *                  operator[]( int globalIndex );
    int                     count();

    #if DEBUG_DUMP
    void                    dumpReplacementTable();
    #endif

protected:

    int                     _index;
    IntegerTable            _repTable;
    MergeStringTable        _strTable;
    AnyVector               _globalTable;
};

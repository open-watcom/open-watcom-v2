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


#ifndef _REFERENCE_UPDATE_TABLE_H
#define _REFERENCE_UPDATE_TABLE_H

// System includes --------------------------------------------------------

#include <string.h>

// Project includes -------------------------------------------------------

#include "mempool.h"
#include "ref.h"

class ReferenceUpdate {
friend class ReferenceUpdateTable;
public:
                        ReferenceUpdate( int, uint_32, uint_32 );
                        ReferenceUpdate( const ReferenceUpdate& orig ) {
                            *this = orig;
                        };
                        ReferenceUpdate(){};    // default constructor

    ReferenceUpdate&    operator= ( const ReferenceUpdate& orig ) {
                            _mbrIndex = orig._mbrIndex;
                            _oldOffset = orig._oldOffset;
                            _newOffset = orig._newOffset;
                            return *(this);
                        };
    bool                operator== ( const ReferenceUpdate& other ) {
                            return _oldOffset == other._oldOffset;
                        };
    bool                operator< ( const ReferenceUpdate& other ) {
                            return _oldOffset < other._oldOffset;
                        };

    void *              operator new( size_t );
    void                operator delete( void * );

private:
    int                 _mbrIndex;
    uint_32             _oldOffset;

    uint_32             _newOffset;
    static MemoryPool   _pool;
};

typedef WCPtrSortedVector<ReferenceUpdate> RefUpdateVector;

class ReferenceUpdateTable {
public:
                        ReferenceUpdateTable();
                        ~ReferenceUpdateTable();

    void                add( int mbrIndex,
                             uint_32 oldOffset,
                             uint_32 newOffset );

    bool                getNewOffset( int mbrIndex, uint_32 oldOffset, uint_32 & newOffset );

    #if DEBUG_DUMP
    void                dumpData();
    #endif
private:
    WCPtrOrderedVector<RefUpdateVector> _table;
};

#endif

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


/*
    REINDEX.HPP
    -----------
    Module to handle the remapping of index numbers that is required when
    multiple .BRM files are merged together, to prevent collisions and
    to merge the index numbers of identical symbols from different .BRM files.
*/

#ifndef _REINDEX_HPP
#define _REINDEX_HPP

#include "hashtbl.hpp"
#include "pool.hpp"
extern "C" {
    #include "brmtypes.h"
};

/*  Turn off memory tracking to redefine "new" and "delete"
*/
#undef new
#undef delete

/*  UInt32Pair:
      Structure to store pairs of uint_32's, for storage in HashTables.
*/

struct UInt32Pair : public Hashable {
    private:
        static Pool     _pairPool;

    public:
        uint_32 newIndex;

        UInt32Pair() { newIndex = 0; index = 0; }
        UInt32Pair( uint_32 id, uint_32 newID )
            { newIndex = newID; index = id; }
        UInt32Pair( UInt32Pair const &other )
            { newIndex = other.newIndex; index = other.index; }

        UInt32Pair &operator=( UInt32Pair const &other )
            { newIndex = other.newIndex; index = other.index; return *this; }

        void    *operator new(size_t) { return _pairPool.Get(); }
        void    *operator new(size_t, const WChar *, const WChar *,
                              WULong) { return _pairPool.Get(); }
        void    operator delete(void *p) { _pairPool.Release(p); }
};


/*  ReOrdering:
      Used when merging .BRM files which refer to the same source
      files, to keep all of the Hashable::index fields correct.
*/

class PCHIndexRec;
class CacheInFile;
class CacheOutFile;

class ReOrdering : private HashTable<UInt32Pair> {
    public:
        ReOrdering();
        ~ReOrdering();

        // Add a new oldID to the table, picking a newID automatically.
        uint_32 Change( uint_32 oldID );

        // Grab a "newID", which will never be returned by Change().
        uint_32 StealId();

        // Add a new pair to the table.  "newID" should be the result
        // of a previous call to Change() or StealID().
        void    ChangeTo( uint_32 oldID, uint_32 newID );

        // Lookup and access functions.
        uint_32 NewId( uint_32 oldID );
        uint_32 LastTopID() { return _lastTopID; }

        // Delete all of the stored pairs.
        void    Flush();

        // Save/Load functionality.
        WBool   SaveTo( CacheOutFile *cache, char const *name );
        WBool   LoadFrom( CacheInFile *cache, char const *name );

        // The following functions are for PCH files, which have
        // their own separate, persistent re-ordering tables.

        // Has a PCH file been included previously?
        WBool   HaveSeenPCH( BRI_StringID fileName );

        // Record calls to Change() and ChangeTo() in a PCH table.
        void    PCHRecord( BRI_StringID fileName );
        void    PCHStopRecording();

        // Make a PCH table available for searching by NewId().
        void    PCHUse( BRI_StringID fileName );
        void    PCHStopUsing(); // stop using _ALL_ PCH tables.

        // Was a new index number generated for a PCH file?
        WBool   IsPCHIndex( uint_32 newID );

    private:
        uint_32 _currentID;
        uint_32 _lastTopID;

        LList<PCHIndexRec>      _storedPCH;
        LList<PCHIndexRec>      _usingPCH;
        PCHIndexRec             *_recordingPCH;
};

/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif  // _REINDEX_HPP

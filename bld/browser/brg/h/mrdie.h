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


#ifndef __MRDIE_H__
#define __MRDIE_H__

#include "brmerge.h"
#include "mrnmkey.h"
#include "mroffset.h"
#include "mrfile.h"

class DIETree;
class MergeAbbrev;
class MergeAttrib;
class MergeInfoSection;
class MergeInfoPP;
class MergeRelocate;

typedef uint_16 DIELen_T;       // length of a die in bytes

class MergeDIE {
public:
                    MergeDIE();
                    MergeDIE( MergeDIE * parent,
                                const MergeOffset& offset,
                                const MergeNameKey& nm,
                                const MergeOffset& child,
                                const MergeOffset& sibling,
                                bool definition, DIELen_T length );
                    MergeDIE( const MergeDIE& other );
                    ~MergeDIE();

                    operator const MergeOffset&() const;
                    operator const MergeNameKey&() const;
    MergeDIE&       operator= ( const MergeDIE& o ) {
                        _parent =       o._parent;
                        _offset =       o._offset;
                        _nameKey =      o._nameKey;
                        _firstChild =   o._firstChild;
                        _sibling =      o._sibling;
                        _flagInt =      o._flagInt;
                        _length =       o._length;
                        _occurs =       o._occurs;

                        return *this;
                    };
    bool            operator== ( const MergeDIE& o ) const {
                        return( this == &o );
                    }

    void *          operator new( size_t ) { return _pool.alloc(); };
    void *          operator new( size_t, void * ptr ) { return ptr; };
    void            operator delete( void * p ) { _pool.free( p ); };
    static void     ragnarok();

    bool            assigned() const { return _flags._assigned; }
    bool            written() const { return _flags._written; }
    bool            definition() const { return _flags._definition; }

    MergeDIE *      collision( DIETree * tree );
    void            setNewOff( MergeInfoSection * sect, uint_32 & newOffset,
                                MergeInfoPP & pp );
    bool            siblingSet() const { return (_sibling.fileIdx >= 0); }
    void            setSibling( const MergeOffset& s ) { _sibling = s; }
    MergeNameKey &  name() { return _nameKey; }

    const MergeOffset&  offset() const { return _offset; }
    const MergeOffset&  firstChild() const { return _firstChild; }
    const MergeOffset&  sibling() const { return _sibling; }
    uint_32             getNewOff() const { return _newOffset; }

    #if 0
    void            writeDIE( MergeInfoSection * sect, MergeFile & out,
                                 WCPtrOrderedVector<MergeFile>& inFiles );
    void            writeSelf( MergeInfoSection * sect, MergeFile & out,
                                 WCPtrOrderedVector<MergeFile>& inFiles );
    bool            writeSpecialAttribs( MergeInfoSection * sect,
                                MergeAttrib & att, uint_32 & offset,
                                MergeFile & out,
                                WCPtrOrderedVector<MergeFile>& inFiles );
    #endif
                    #if INSTRUMENTS
                    operator const char*() const;
                    #endif

private:
    MergeDIE *          _parent;        // parent die
    MergeNameKey        _nameKey;       // name key
    MergeOffset         _offset;        // offset key

    MergeOffset         _firstChild;
    MergeOffset         _sibling;

    uint_32             _newOffset;     // offset in output .dbr file
    uint_32             _newSibOffset;  // offset of sibling in .dbr file

    union {
        uint_8          _flagInt;
        struct {
            uint_8  _definition : 1;
            uint_8  _assigned   : 1;
            uint_8  _written    : 1;
        } _flags;
    };
    DIELen_T            _length;        // length of die in bytes
    uint_16             _occurs;        // num with same name

    static MemoryPool   _pool;
};

#endif // __MRDIE_H__

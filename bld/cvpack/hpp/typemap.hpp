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


#ifndef TYPEMAP_H_INCLUDED
#define TYPEMAP_H_INCLUDED
#include "myvector.hpp"

class TypeIndexMap {

    typedef struct {
        type_index      globalIndex;
        bool            isDone;
        bool            isNewType;
    } mappingStruct;

    public :

        TypeIndexMap(const uint        count = 0,
                     const type_index  startIndex = CV_FIRST_NONPRIM);
        ~TypeIndexMap() { }

        void Set (const type_index startIndex ) {
            _currentGlobalIndex = startIndex;
        }

        type_index Lookup(const type_index) const;

        void Insert(const type_index oldTypeIndex,
                    const type_index newTypeIndex=0);

        void Reset( const uint count = 0 );

        void UnMarkDone( const type_index ti ) {
            _mappingTable[ti].isDone = FALSE;
        }

        bool IsDone( const type_index oldTypeIndex ) {
            return _mappingTable[oldTypeIndex].isDone;
        }

        bool IsNewType( const type_index oldTypeIndex ) {
            return _mappingTable[oldTypeIndex].isNewType;
        }

    private :

        void Init( uint count ) {
            for ( uint i = 0; i < count; i++ ) {
                _mappingTable[ToTypeIndex(i)].isDone = FALSE;
            }
        }

        MyValBoundVector<mappingStruct>   _mappingTable;
        type_index                        _currentGlobalIndex;
};
#endif

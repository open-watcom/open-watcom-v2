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


#include "common.hpp"
#include "cverror.hpp"
#include "typemap.hpp"

TypeIndexMap::TypeIndexMap( const uint        count,
                            const type_index  startIndex )
        : _currentGlobalIndex( startIndex ),
          _mappingTable( CV_FIRST_NONPRIM,count )
/********************************************************/
{
    _mappingTable.WCExcept::exceptions( WCExcept::out_of_memory
                                       | WCExcept::index_range  );
    Init(count);
}

//
// newTypeIndex defaulted to 0 to indicate that a new type is to be enter.
//
void TypeIndexMap::Insert( const type_index oldTypeIndex,
                           const type_index newTypeIndex )
/********************************************************/
{
    try {
    if ( newTypeIndex != 0 ) {
        _mappingTable[oldTypeIndex].globalIndex = newTypeIndex;
        _mappingTable[oldTypeIndex].isNewType = FALSE;
    } else {
        // check if running out of types.
        if ( _currentGlobalIndex >= 0xffff ) {
            throw MiscError("fatal : running out of type indices.");
        }
        _mappingTable[oldTypeIndex].globalIndex = _currentGlobalIndex++;
        _mappingTable[oldTypeIndex].isNewType = TRUE;
    }

   // testing code.
    _mappingTable[oldTypeIndex].isDone = TRUE;
    }
    catch (...) {
        cerr << "index : " << oldTypeIndex << endl;
        throw InternalError("packtype.cpp : TypeIndexMap::Lookup() index range failed.");
    }
}

type_index TypeIndexMap::Lookup( const type_index oldTypeIndex ) const
/********************************************************************/
{
    try {
        if ( oldTypeIndex < CV_FIRST_NONPRIM ) {
            return oldTypeIndex;
        }
        if ( ! _mappingTable[oldTypeIndex].isDone ) {
            return oldTypeIndex;
        }
        return _mappingTable[oldTypeIndex].globalIndex;
    }

    // Internal testing code.
    catch (...) {
        cerr << "Index : " << oldTypeIndex << " " << endl;
        throw InternalError("packtype.cpp : TypeIndexMap::Lookup() index range failed.");
    }
}

void TypeIndexMap::Reset( const uint count )
/******************************************/
{
    try {
        _mappingTable.ClearAndReset( count );
        Init(count);
    }
    // temporary internal testing code.
    catch (...) {
        throw OutOfMemory( DEF_ARRAY_SIZE * sizeof(int) );
    }
}

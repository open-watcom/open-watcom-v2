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
* Description:  Merge offset class declaration.
*
****************************************************************************/


#ifndef MROFFSET_H_INCLUDED
#define MROFFSET_H_INCLUDED

#include "brmerge.h"

class MergeOffset {
public:
                        MergeOffset();
                        MergeOffset( uint_8 f, uint_32 o );
                        MergeOffset( const MergeOffset& o );

inline  bool            operator< ( const MergeOffset& other ) const;
inline  bool            operator== ( const MergeOffset& other ) const;

inline  MergeOffset&    operator= ( const MergeOffset& other );

    #if INSTRUMENTS
        const char *    getString() const;
    #endif

    fileidx_t           fileIdx;
    uint_32             offset;
};

inline bool MergeOffset::operator< ( const MergeOffset& other ) const
//-------------------------------------------------------------------
{
    if( fileIdx > other.fileIdx ) {
        return FALSE;
    } else {
        if( fileIdx < other.fileIdx ) {
            return TRUE;
        } else {
            return offset < other.offset;
        }
    }
}

inline bool MergeOffset::operator== ( const MergeOffset& other ) const
//--------------------------------------------------------------------
{
    if( fileIdx != other.fileIdx ) {
        return FALSE;
    } else {
        return offset == other.offset;
    }
}

inline MergeOffset& MergeOffset::operator= ( const MergeOffset& other )
//---------------------------------------------------------------------
{
    fileIdx = other.fileIdx;
    offset = other.offset;
    return *this;
}

#endif // MROFFSET_H_INCLUDED

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


#ifndef __MRNMKEY_H__
#define __MRNMKEY_H__

#include "brmerge.h"
#include "mrstrhdl.h"

class MergeNameKey {
public:
        MergeNameKey();
        MergeNameKey( uint_32 tag, bool ext, const MergeStringHdl& name, uint_32 unique );
        MergeNameKey( uint_32 tag, bool ext, const char * name, uint_32 unique );
        MergeNameKey( const MergeNameKey& other );

inline  bool            operator< ( const MergeNameKey& other ) const;
inline  bool            operator== ( const MergeNameKey& other ) const;
inline  MergeNameKey&   operator= ( const MergeNameKey& other );

        #if INSTRUMENTS
        const char *    getString() const;
        #endif

        uint_32         _tag;
        bool            _extern;
        MergeStringHdl  _name;
        uint_32         _unique;    // this makes the key unique for collisions
};

bool MergeNameKey::operator< ( const MergeNameKey& other ) const
//--------------------------------------------------------------

{
    if( _tag < other._tag ) {
        return true;
    } else {
        if( _tag == other._tag ) {
            if( _extern < other._extern ) {
                return true;
            } else {
                if( _extern == other._extern ) {
                    if( _name < other._name ) {
                        return true;
                    } else {
                        if( _name == other._name ) {
                            if( _unique < other._unique ) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

inline bool MergeNameKey::operator== ( const MergeNameKey& other ) const
//----------------------------------------------------------------------
{
    return( _tag == other._tag && _extern == other._extern &&
            _name == other._name && _unique == other._unique );
}

inline  MergeNameKey& MergeNameKey::operator= ( const MergeNameKey& other )
//-------------------------------------------------------------------------
{
    _tag = other._tag;
    _extern = other._extern;
    _name.operator= ( other._name );
    _unique = other._unique;

    return *this;
};

#endif

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


#ifndef __MRSTRHDL_H__
#define __MRSTRHDL_H__

#include <string.h>     // need strlen for hashing

#include "brmerge.h"
#include "patricia.h"

class MergeStringHdl {
public:
                    MergeStringHdl() : _str( NULL ) {}
                    MergeStringHdl( const char * str ) { copyString( str ); }
                    MergeStringHdl( const MergeStringHdl& other ) {
                        _str = other._str;
                    }
                    ~MergeStringHdl() {}

    bool            operator< ( const MergeStringHdl& o ) const {
                        unsigned long me = (unsigned long) _str;
                        unsigned long other = (unsigned long) o._str;
                        return( me < other );
                    }
    bool            operator== ( const MergeStringHdl& o ) const {
                        unsigned long me = (unsigned long) _str;
                        unsigned long other = (unsigned long) o._str;
                        return( me == other );
                    }
    const char *    getString() const {
                        return _str;
                    }

    MergeStringHdl& operator= ( const MergeStringHdl& other ) {
                        _str = other._str;
                        return *this;
                    };
    MergeStringHdl& operator= ( const char * str ) {
                        copyString( str );
                        return *this;
                    };
    uint            hashVal() const;
    static uint     getHashVal( const MergeStringHdl & s ) {
                        return s.hashVal();
                    }

    static void     ragnarok();
private:
    void            copyString( const char * str ) {
                        _str = _strings.insert( str );
                    }
    const char *        _str;
    static PatriciaTree _strings;
};

#endif // __MRSTRHDL_H__

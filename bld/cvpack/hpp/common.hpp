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


#ifndef _COMMON_H_INCLUDED
#define _COMMON_H_INCLUDED

#ifdef __WATCOMC__
#pragma WARNING 389 9;
#endif

#include "watcom.h"
#include "bool.h"
#include <iostream>
#include <string.h>

static const int DEF_ARRAY_SIZE = 100;

static const int LONG_WORD = 4;
static const int WORD      = 2;
static const int BYTE      = 1;

//
// First user-defined type index.
//
typedef unsigned int uint;
typedef unsigned_16  type_index;

static const int CV_FIRST_NONPRIM   = 0x1000;
static const int CV4_HEADER = 0x1L;

// pragma away the compiler warning for overflow.  If the type index exceed
// or equal 0xffff, cvpack will give a fatal error.

extern uint ToTypeIndex( const uint tableIndex );
extern uint ToTableIndex( const type_index typeIndex );
extern uint PrefixStrLen( const char* ptr );
extern void Warning( const char* msg );

class VariantString {

    friend class SymbolStruct;
    friend class LFLeafStruct;
    friend class ExeMaker;
    friend class GlobalHashContainer;

    public :

        VariantString( const char* string, const uint strLen )
                : _string( strLen ? new char [strLen] : NULL ),
                  _strLen( strLen ) {
            if ( _strLen > 0 ) {
                memcpy(_string,string,_strLen);
            }
        }

        ~VariantString() {
            if (_string) {
                delete [] _string;
            }
        }

        bool operator == ( const VariantString& target ) const {
            return ( _strLen == target._strLen &&
                     memcmp(_string,target._string,_strLen) == 0 );
        }

    private :

        char*       _string;
        uint        _strLen;
};
#endif

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
    STRTABLE.HPP
    ------------
    String management for the browse merge DLL.
    Currently only supports ANSI strings, 'cos that's all the
    compiler uses internally anyway.
*/

#ifndef _STRTABLE_HPP
#define _STRTABLE_HPP

extern "C" {
#include "brmtypes.h"   // for BRIStringID
};
#include "pool.hpp"

/*  Turn off memory tracking to redefine "new" and "delete"
*/
#undef new
#undef delete


#define BRI_STRING_NULL ((BRI_StringID) 0x0);

class CacheOutFile;
class CacheInFile;

class StringTable {
    public:
        StringTable();
        ~StringTable();

        // String storage and retrieval

        BRI_StringID    Insert( char const *string );
        char const *    Lookup( BRI_StringID id );
        BRI_StringID    Lookup( char const *str );

        // Save and load

        WBool           SaveTo( CacheOutFile *cache );
        WBool           LoadFrom( CacheInFile *cache );

        // Merge two string tables, emptying the second.
        // Not yet used, and not completely implemented.

        void            Absorb( StringTable &other );

    private:
        int     Hash( char const *string, int len );

    private:
        struct Link {
            Link                *next;  // next string in this "bucket"
            BRI_StringID        id;     // id of this string
            int                 offset; // offset into _charBuffer

            static Pool linkPool;

            void        *operator new(size_t) { return linkPool.Get(); }
            void        *operator new(size_t, const WChar *, const WChar *,
                                      WULong) { return linkPool.Get(); }
            void        operator delete(void *p) { linkPool.Release(p); }
        };

        char *          _charBuffer;    // Array to store all strings
        int             _bufSize;       // Size of _charBuffer
        int             _bufTop;        // Amount of _charBuffer in use

        Link **         _table;         // Hash table
        int             _count;         // Number of strings in table
};


/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif // _STRTABLE_HPP

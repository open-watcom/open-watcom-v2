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
    USAGE.HPP
    ---------
    Module for handling "usages" (i.e., references to symbols) for
    the browse/merge DLL.
*/

#ifndef _USAGE_HPP
#define _USAGE_HPP

extern "C" {
#include "brmtypes.h"
};
#include "hashtbl.hpp"
#include "pool.hpp"
#include "avltree.hpp"

/*  Turn off memory tracking to redefine "new" and "delete"
*/
#undef new
#undef delete

/*  Forward Declarations.
*/

class CacheInFile;
class CacheOutFile;


/*  UsageRec:
      Stores information about a single reference.
*/

struct UsageRec {
    private:
        static Pool     _usagePool;
        uint_32                 column;
        UsageRec                *next;

    public:
        BRI_ReferenceType       type;
        union {
            BRI_SymbolID        targetID;
            BRI_TypeID          typeID;   // when type==BRI_RT_InheritFrom
        };
        BRI_ScopeID             enclosing;

        void    *operator new(size_t) { return _usagePool.Get(); }
        void    *operator new(size_t, const WChar *, const WChar *,
                              WULong) { return _usagePool.Get(); }
        void    operator delete(void *p) { _usagePool.Release(p); }

        friend class UsageList;
};


/*  UsageList:
      Storage for all information about "usages" (references to symbols).
*/

class UsageList {
    public:
        UsageList();
        ~UsageList();

        // Add a new reference.
        void            Insert( UsageRec * data );

        // Change the current "cursor location".
        void            Delta( int_8 dcol, int_16 dline );

        // Change the current source file.
        void            SetFile( BRI_StringID filename );
        void            EndFile();

        // Searching and access functions.
        UsageRec *      First();
        UsageRec *      Next();
        BRI_StringID    GetFileName();
        uint_32         GetLine();
        uint_32         GetColumn();
        int             Count() { return _count; }

        // Search for references based on locations in the source. NextAt()
        // returns NULL when no more references at that location exist.
        UsageRec *      At( BRI_StringID fname,
                            uint_32     line,
                            uint_32     column );
        UsageRec *      NextAt();

        // If At() fails, you can check the first locations to the left
        // and right at which references appear.
        UsageRec *      NextAtLeft();
        UsageRec *      NextAtRight();

        // Reset the "current file" pointers and stuff like that.
        void            Reset();

        // Save/Load functionality.
        WBool           SaveTo( CacheOutFile *cache );
        WBool           LoadFrom( CacheInFile *cache );

    private:
        struct FileData {
            FileData *          next;
            BRI_StringID        filename;
            AvlTree<UsageRec>   lines;
            uint_32             curLine;
            uint_32             curColumn;
        };

        FileData *      _firstFile;
        LList<FileData> _openFileStack;

        int             _count;

        // for iteration and while merging
        FileData *      _curFile;

        // for iteration only
        UsageRec *      _curUsage;

        // for searching based on location.
        UsageRec *      _left;
        UsageRec *      _right;

        uint_32         _curID;
};


/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif  // _USAGE_HPP

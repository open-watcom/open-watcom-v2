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
    SCOPES.HPP
    ----------
    Module for processing scope information for the browse/merge DLL.
*/

#ifndef _SCOPES_HPP
#define _SCOPES_HPP

extern "C" {
#include "brmtypes.h"
}
#include "hashtbl.hpp"
#include "pool.hpp"
#include "avltree.hpp"

/*  Turn off memory tracking to redefine "new" and "delete"
*/
#undef new
#undef delete

/*  Forward declarations.
*/

struct DeclRec;
struct TypeRec;
class CacheOutFile;
class CacheInFile;


/*  ScopeRec:
        Information about a particular scope in the scope tree.
*/

struct ScopeRec : public Hashable {
    private:
        static Pool     _scopePool;

    public:
        ScopeRec                *firstChild;
        ScopeRec                *lastChild;
        ScopeRec                *firstSibling;
        ScopeRec                *parent;

        AvlTree<DeclRec>        symbols;  // symbols defined in this scope
        AvlTree<TypeRec>        types;    // class types defined in this scope

        BRI_ScopeType           flags;
        union {
            BRI_SymbolID        symbolID;
            BRI_TypeID          typeID;
        };

        void    *operator new( size_t ) { return _scopePool.Get(); }
        void    *operator new( size_t, const WChar *, const WChar *,
                               WULong ) { return _scopePool.Get(); }
        void    operator delete( void *p ) { _scopePool.Release(p); }
};


/*  ScopeTable:
      Storage for scope information.
*/

class ScopeTable : private HashTable<ScopeRec> {
    public:
        ScopeTable();
        ~ScopeTable();

        // Open a scope.  Checks if the same scopes has been opened before.
        ScopeRec        *OpenScope( ScopeRec *scope
                                  , BRI_StringID fnName = (BRI_StringID) 0x0
                                  , BRI_TypeID fnType = (BRI_TypeID) 0x0 );

        // Mark that a scope has been opened, but ignore the contents
        // (for when we're processing a scope or source file that we
        //  don't want to examine twice)
        void            OpenDummyScope();

        // Close the current scope.
        ScopeRec        *CloseScope();

        // Various searching functions.
        ScopeRec        *Lookup( BRI_ScopeID id );
        ScopeRec        *First();
        ScopeRec        *Next();
        ScopeRec        *FindClassScope( BRI_TypeID id );
        ScopeRec        *FindFuncScope( BRI_SymbolID id );

        // Access functions.
        ScopeRec        *Head() { return _head; }
        ScopeRec        *Current() { return _current; }
        WBool           Ignore() { return _state==IGNORE; }

        // Save/Load functionality.
        WBool           SaveTo( CacheOutFile *cache );
        WBool           LoadFrom( CacheInFile *cache );

    private:
        ScopeRec        *_head;
        ScopeRec        *_current;
        LList<ScopeRec> _stack;         // currently open scopes

        enum {
            USE,
            IGNORE
        }               _state;
        int             _ignoreDepth;
};

/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif  // _SCOPES_HPP

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
    TYPES.HPP
    ---------
    Module for handling type information for the browse/merge DLL.
*/

#ifndef _TYPES_HPP
#define _TYPES_HPP

extern "C" {
#include "brmtypes.h"
}
#include "hashtbl.hpp"
#include "avltree.hpp"
#include "pool.hpp"

/*  Turn off memory tracking to redefine "new" and "delete"
*/
#undef new
#undef delete

class CacheOutFile;
class CacheInFile;


/*  TypeRec:
      Store information about one particular C++ type.
*/

struct TypeRec : public Hashable {
    private:
        static Pool     _typePool;

        TypeRec         *_next;         // for use of TypeList only
        friend class TypeList;

    public:
        BRI_TypeCode            typeCode;

        union {
            struct {                            // BaseType
                BRI_BaseTypes   baseType;
            } bt;
            struct {                            // Modifier
                uint_32         flags;
                BRI_TypeID      parent;
            } fp;
            struct {                            // Pointer, Reference, Typedef
                BRI_TypeID      parent;
            } p;
            struct {                            // PtrToMember
                BRI_TypeID      host;
                BRI_TypeID      member;
            } cm;
            struct {                            // Array
                uint_32         size;
                BRI_TypeID      element;
            } se;
            struct {                            // Function
                int             numArgs;
                BRI_TypeID *    args;           // args[0] == return type
            } ra;
            struct {                            // Class, Struct, Union, Enum
                BRI_StringID    nameID;
                BRI_SymbolID    symbolID;
            } ns;
            struct {                            // BitField
                uint_32         width;
            } w;
        };

        TypeRec() { ra.args = NULL; }
        ~TypeRec();

        void    *operator new(size_t) { return _typePool.Get(); }
        void    *operator new(size_t, const WChar *, const WChar *,
                              WULong) { return _typePool.Get(); }
        void    operator delete(void *p) { _typePool.Release(p); }
};

typedef AvlTree<TypeRec>  TypeCategory;


/*  TypeList:
      Storage for type information.
*/

class TypeList : private HashTable<TypeRec> {
    public:
        ~TypeList();

        // Add a type to the list.  Checks if an identical type has
        // been previously inserted.
        TypeRec *       AddType( TypeRec *type );

        // Searching.
        TypeRec *       Lookup( BRI_TypeID id )
                { return HashTable<TypeRec>::Lookup(id); }

        // Save/Load functionality.
        WBool           SaveTo( CacheOutFile *cache );
        WBool           LoadFrom( CacheInFile *cache );

    private:
        // To optimize AddType(), lists of specific kinds of C++ types
        // are stored separately here and in the ScopeRec's.  Saves
        // oodles of time but wastes space.
        TypeCategory    _baseTypes;
        TypeCategory    _modifiers;
        TypeCategory    _ptrTypes;
        TypeCategory    _ptrsToMembers;
        TypeCategory    _arrays;
        TypeCategory    _functions;
        TypeCategory    _bitFields;
};


/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif  // _TYPES_HPP

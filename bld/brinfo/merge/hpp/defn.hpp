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
    DEFN.HPP
    --------
    Module for handling "definitions", which associate symbols with
    with locations in the source code.
*/

#ifndef _DEFN_HPP
#define _DEFN_HPP

extern "C" {
#include "brmtypes.h"
}
#include "hashtbl.hpp"
#include "pool.hpp"

/*  Turn off memory tracking to redefine "new" and "delete"
*/
#undef new
#undef delete


class CacheOutFile;
class CacheInFile;


/*  Note:  in a DefnRec, the field Hashable::index is used to
           store the index of the symbol (DeclRec) which is being
           defined.  This is to facilitate finding the definition
           of a given symbol.
*/

struct DefnRec : public Hashable {
    private:
        static Pool _defnPool;
    public:
        uint_32         column;
        uint_32         line;
        BRI_StringID    filenameID;

        void    *operator new(size_t) { return _defnPool.Get(); }
        void    *operator new(size_t, const WChar *, const WChar *,
                              WULong) { return _defnPool.Get(); }
        void    operator delete(void *p) { _defnPool.Release(p); }
};


/*  DefnList:
      List of all definitions.
*/


class DefnList : private HashTable<DefnRec> {
    public:
        ~DefnList();

        void    AddDefinition( DefnRec * defn );
        void    AddTempDefinition( DefnRec * defn );
        void    AcceptDefns();

        // Iterate over all definitions.

        DefnRec *       First();
        DefnRec *       Next();

        // Iterate over definitions for a given symbol.

        DefnRec *       FirstDefnFor( BRI_SymbolID symbol );
        DefnRec *       NextDefn();

        // Save and load functionality.

        WBool           SaveTo( CacheOutFile *cache );
        WBool           LoadFrom( CacheInFile *cache );

    private:
        LList<DefnRec>  _tempDefns;
        Hashable *      _current;       // For iteration
        int             _currIndex;     // For iteration
};


/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif  // _DEFN_HPP

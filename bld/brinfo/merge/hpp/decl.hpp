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


#ifndef _DECL_HPP
#define _DECL_HPP

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
class ScopeRec;

struct DeclRec : public Hashable {
    private:
        static Pool     _declPool;
    public:
        DeclRec() { _nextDecl = NULL; }

        BRI_SymbolAttributes    attribs;
        BRI_StringID            nameID;
        BRI_TypeID              typeID;
        BRI_ScopeID             enclosing;

        void    *operator new(size_t) { return _declPool.Get(); }
        void    *operator new(size_t, const WChar *, const WChar *,
                              WULong) { return _declPool.Get(); }
        void    operator delete(void *p) { _declPool.Release(p); }

    private:
    // KLUDGE ALERT...the following field is used to optimize
    // searching for duplicate symbols in Browser::ReadDeclaration.
        DeclRec         *_nextDecl;

        friend class Browser;
        friend class DeclList;
        friend class ScopeTable;
};


/*  DeclList:
      List of all symbols (declarations).
*/

class DeclList : public HashTable<DeclRec> {
    public:
        DeclList();
        ~DeclList();

        //  Iteration functions
        DeclRec *First();
        DeclRec *Next();

        WBool   SaveTo( CacheOutFile *cache );
        WBool   LoadFrom( CacheInFile *cache );

    private:
        int     _currIndex;
        DeclRec *_currLink;
};


/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif  // _DECL_HPP

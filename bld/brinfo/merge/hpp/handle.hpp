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
    HANDLE.HPP
    ----------
    Definition of "Browser", the "main" class of the DLL.
    All functions exported from the DLL are wrappers around Browser member
    functions.
*/

#ifndef _HANDLE_HPP
#define _HANDLE_HPP

#include "wbrowse.hpp"

/*  Forward declarations
*/
class UsageList;
class ScopeTable;
class DefnList;
class StringTable;
class TypeList;
class DeclList;
class MacroList;
class DependList;
class ReOrdering;
class DefnRec;
class ScopeRec;
class TypeRec;
class UsageRec;
class BrmFile;
class CacheInFile;
class CacheOutFile;
template<class T> class LList;


/*  Browser:
      Functions defined in the library interface ("brmerge.hpp") are
      implemented in this class.
*/

class Browser {
    public:
        Browser();
        ~Browser();

        /* Access functions
        */
        ScopeTable *    scopeList() { return _scopes; }
        DeclList *      declList() { return _decls; }

        ReOrdering *    declIndex() { return _declIndex; }
        ReOrdering *    scopeIndex() { return _scopeIndex; }
        ReOrdering *    typeIndex() { return _typeIndex; }
        ReOrdering *    stringIndex() { return _stringIndex; }

        /* Functions corresponding to functions in the library
           interface.
        */

        WBool LoadOldBrowser( CacheInFile *cache );
        WBool SaveBrowser( CacheOutFile *cache );

        WBool AddFile( char const *filepath );
        int DefnNameIter( char const *name, BCallBack cb, void * cookie );
        int DefnAtIter( SourceLocn *locn, BCallBack cb, void * cookie );
        int DefnOfIter( DeclRec const *sym, BCallBack cb, void * cookie );
        int FileClassesIter( const char *filepath, BCallBack cb,
                             void * cookie );
        int ReferenceIter( DeclRec const *symbol, BRI_ReferenceType type,
                           BCallBack cb, void * cookie );
        int SymbolIter( BRI_SymbolAttributes symType,
                        BCallBack cb, void * cookie );
        int UsedFnsIter( BCallBack cb, void * cookie );
        int UserClassesIter( BCallBack cb, void * cookie );
        int CallingFnsIter( DeclRec const *sym, BCallBack cb, void * cookie );
        int CalledFnsIter( DeclRec const *sym, BCallBack cb, void * cookie );
        int ParentClassesIter( DeclRec const *sym, BCallBack cb, void *cookie );
        int ChildClassesIter( DeclRec const *sym, BCallBack cb, void *cookie );
        int MemberFnsIter( DeclRec const *sym, BCallBack cb, void *cookie );
        WString UnScopedName( DeclRec const *sym );
        WString ScopedName( DeclRec const *sym );
        WString NameOfScope( DeclRec const *sym );
        WString TypedName( DeclRec const *sym );
        WString NameOfType( DeclRec const *sym );

    private:
        /* Utility functions
        */

        BRI_StringID renameSym( BRI_StringID nameID,
                        BRI_TypeID typeID,
                        ScopeRec *scope );
        WBool ProcessFile(BrmFile &infile);
        void CleanUp();
        WString &writeType( BRI_TypeID id, WString &current,
                            DeclRec const * symbol=NULL );

        /* The following functions read a browse record of a
           certain type from the given file.  They return the
           number of bytes read.
        */
        int ReadDeclaration(BrmFile &infile);
        int ReadFile(BrmFile &infile);
        int ReadFileEnd(BrmFile &infile);
        int ReadTemplate(BrmFile &infile);
        int ReadTemplateEnd(BrmFile &infile);
        int ReadScope(BrmFile &infile);
        int ReadScopeEnd(BrmFile &infile);
        int ReadDelta(BrmFile &infile);
        int ReadUsage(BrmFile &infile);
        int ReadString(BrmFile &infile);
        int ReadType(BrmFile &infile);
        int ReadDefinition(BrmFile &infile);
        int ReadGuard(BrmFile &infile);
        int ReadPCHInclude(BrmFile &infile);

    private:
        // Various bits of browse information.

        UsageList *     _usages;
        ScopeTable *    _scopes;
        DefnList *      _defns;
        StringTable *   _strings;
        TypeList *      _types;
        DeclList *      _decls;
        DependList *    _guards;
        MacroList *     _macros;

        // Indices used when merging .BRM files.

        ReOrdering *    _declIndex;
        ReOrdering *    _scopeIndex;
        ReOrdering *    _typeIndex;
        ReOrdering *    _stringIndex;
        ReOrdering *    _macrosToSyms;

        LList<DefnRec>  *_defnReIndex;
        LList<TypeRec>  *_typeReIndex;
        LList<UsageRec> *_usageReIndex;

        // Small piece of generic temporary memory used by various
        // Browser functions.

        uint_8          * _buffer;
        int             _bufLen;

        enum {
            NORMAL,
            GUARD_SEARCH
        }               _state;
};

#endif  // _HANDLE_HPP

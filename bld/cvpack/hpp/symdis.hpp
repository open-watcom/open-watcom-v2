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


#ifndef SYMDIS_H_INCLUDED
#define SYMDIS_H_INCLUDED

#include "subsect.hpp"

class SymbolStruct;

typedef SymbolStruct* (*PCFN)( const char* );

static const BASE_ADR1 = S_ENTRYTHIS + 1;
static const BASE_ADR2 = BASE_ADR1 + (S_REGREL16-S_BPREL16) + 1;
static const BASE_ADR3 = BASE_ADR2 + (S_GTHREAD32-S_BPREL32) + 1;

class SymbolDistributor {

    friend class CSEntryThis;

    public :

        SymbolDistributor( Retriever&    retriever,
                           SstGlobalSym& globalSym,
                           SstStaticSym& staticSym )
                : _retriever( retriever ),
                  _globalSym( globalSym ),
                  _staticSym( staticSym ) { }

        ~SymbolDistributor() { }

        bool Distribute( const uint, SstAlignSym& ) const;

    private :

        static uint ConvertIndex( const symbol_index index ) {
            if ( index < S_BPREL16 ) {
                return index;
            }
            if ( index < S_BPREL32 ) {
                return BASE_ADR1 + (index - S_BPREL16);
            }
            if ( index < S_PROCREF ) {
                return BASE_ADR2 + (index - S_BPREL32);
            }
            if ( index <= S_ALIGN ) {
                return BASE_ADR3 + (index - S_PROCREF);
            }
            return 0;
        }

        static SymbolStruct* CreateSym( const char* ptr ) {
            symbol_index index = * (symbol_index *) ( ptr + WORD );
            return _symConstructorTable[ConvertIndex(index)](ptr);
        }

        static CSProcRef* CreateProcRef( SymbolStruct* sym,
                                         const uint    module ) {
            return new CSProcRef( sym, module );
        }

        static CSDataRef* CreateDataRef( SymbolStruct* sym,
                                         const uint    module ) {
            return new CSDataRef( sym, module );
        }

        void Build( const char*, const uint, SstAlignSym& ) const;

        Retriever&      _retriever;
        SstGlobalSym&   _globalSym;
        SstStaticSym&   _staticSym;

        static PCFN     _symConstructorTable[];
};
#endif

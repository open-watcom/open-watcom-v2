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


#ifndef _SUBSECTION_H_INCLUDED
#define _SUBSECTION_H_INCLUDED

#include <wclist.h>
#include <wclistit.h>
#include <wcstack.h>
#include <wcvector.h>

#ifndef _CV4W_H_INCLUDED
#define _CV4W_H_INCLUDED
#include "cv4w.h"
#endif

#include "typemap.hpp"
#include "makeexe.hpp"
#include "cssymbol.hpp"
#include "typearay.hpp"
#include "retrieve.hpp"

class SymbolStruct;

extern TypeIndexMap TypeMap;

static const int ALIGN_LENGTH = 0x1000;

static const int DEFAULT_NAME_HASH = 10;
static const int DEFAULT_ADDR_HASH = 12;


/******* TYPE *********/

class SstGlobalTypes {

    public :

        SstGlobalTypes() {
            LFLeafStruct::SetGlobalTypeArray(_globalTypingInfo);
            LFLeafStruct::SetLocalTypeArray(_localTypingInfo);
        }

        virtual ~SstGlobalTypes() { }

        void InsertNewType( LFLocalTypeRecord* record) {
            TypeMap.Insert(record -> TypeIndex());
            Append( record );
        }

        static void InsertOldType( const type_index globalIndex,
                                   const type_index localIndex ) {
            TypeMap.Insert(localIndex,globalIndex);
        }

        void Put( ExeMaker& ) const;

        uint Entries() {
            return _globalTypingInfo.Entries();
        }

        bool LoadTypes( Retriever&, const module );

    private :

        void FixUpTypes(const type_index);
        void Append( LFLocalTypeRecord* );
        void ProcessTypes(LFLocalTypeRecord*);

        GlobalTypeArray  _globalTypingInfo;
        LocalTypeArray   _localTypingInfo;
};

/********* END TYPE ***********/

class SymbolSubsection {

    protected :

        static uint PadCount( const uint,
                              const unsigned_32);

        static uint DumpPageAlign( ExeMaker&         eMaker,
                                   const uint        padCount,
                                   const unsigned_8  padVal = 0 );

        static uint PageAlign( ExeMaker&,
                               const uint,
                               const unsigned_32 );
};

class SstAlignSym : public SymbolSubsection {

    enum {
        // size of start search symbol including padding word.
        S_SEARCH_SIZE = sizeof(s_ssearch)+WORD,
    };

    public :

        SstAlignSym( const uint cSeg )
                : _currentOffset(LONG_WORD + S_SEARCH_SIZE*cSeg) { }

        ~SstAlignSym() {
            _symbolInfo.clearAndDestroy();
        }

        int Insert( SymbolStruct* );

        bool IsInGlobalScope() {
            return symStack.isEmpty();
        }

        void Put( ExeMaker& ) const;

        unsigned_32 Length() const {
            return CurrentOffset();
        }

    private :

        int InsertOneRecord( SymbolStruct* );

        static WCStack< SymbolStruct*, WCPtrSList<SymbolStruct> > symStack;

        uint PadCount() const;

        unsigned_32 CurrentOffset() const {
            return _currentOffset;
        }

        unsigned_32                 _currentOffset;
        WCPtrSList<SymbolStruct>    _symbolInfo;
        WCPtrVector<SymbolStruct>   _lastStartSym;
};

class SstGlobalSym;

class NameHashTable {

    enum {
        ITEM_PER_BUCKET = 7,
    };

    struct chain_table {

        chain_table( SymbolStruct*     sym,
                     const unsigned_32 cSum )
                : _symPtr(sym),
                  _offset(sym->Offset()),
                  _checkSum(cSum) { }

        ~chain_table() { }

        bool operator == ( const chain_table& target ) const {
            return _checkSum == target._checkSum;
        }

        SymbolStruct*   _symPtr;
        unsigned_32     _offset;
        unsigned_32     _checkSum;
    };

    public :

        NameHashTable( const uint count )
                : _cHash(count < ITEM_PER_BUCKET ? 1 : count/ITEM_PER_BUCKET),  // determine table size later.
                  _cTab( new WCPtrSList<chain_table> [_cHash] ) { }

        ~NameHashTable() {
            for ( uint i = 0; i < _cHash; i++ ) {
                _cTab[i].clearAndDestroy();
            }
            delete [] _cTab;
        }

        bool TryToInsert( SymbolStruct* );

        void Put( ExeMaker& ) const;

    private :

        SymbolStruct* Find( const WCPtrSList<chain_table>&,
                            const unsigned_32 ) const;

        uint                        _cHash;  // remember to pad when output.
        WCPtrSList<chain_table>*    _cTab;
};

class AddrHashTable {

    struct offset_table {

        offset_table( const unsigned_32 fileOffset,
                      const unsigned_32 memOffset )
                : _fileOffset(fileOffset),
                  _memOffset(memOffset) { }

        ~offset_table() { }

        bool operator == ( const offset_table& target ) const {
            return _memOffset == target._memOffset;
        }

        unsigned_32     _fileOffset;
        unsigned_32     _memOffset;
    };

    public :

        AddrHashTable( const uint cSeg )
                : _cSeg(cSeg),
                  _oTab( new WCPtrSList<offset_table> [cSeg] ) { }

        ~AddrHashTable() {
            for ( uint i = 0; i < _cSeg; i++ ) {
                _oTab[i].clearAndDestroy();
            }
            delete [] _oTab;
        }

        bool TryToInsert( SymbolStruct* );
        void Put( ExeMaker& ) const;

    private :

        static int Compare( const void*, const void* );

        uint                        _cSeg;  // remember to pad when output.
        WCPtrSList<offset_table>*   _oTab;
};

class SstGlobalSym : public SymbolSubsection {

    public :

        SstGlobalSym() { }

        ~SstGlobalSym() {
            _symbolInfo.clearAndDestroy();
        }

        int Insert( SymbolStruct* sym ) {
            return _symbolInfo.append(sym);
        }

        void Put( ExeMaker&, const uint );

    private :

        WCPtrSList<SymbolStruct>    _symbolInfo;
};

typedef SstGlobalSym SstGlobalPub;
typedef SstGlobalSym SstStaticSym;

#endif

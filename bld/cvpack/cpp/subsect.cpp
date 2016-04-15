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


#include <assert.h>
#include <wcstack.h>
#include <string.hpp>
#include "common.hpp"
#include "cverror.hpp"
#include "subsect.hpp"

WCStack< SymbolStruct*, WCPtrSList<SymbolStruct> > SstAlignSym::symStack;

/*SstGlobalTypes::~SstGlobalTypes()
{
    for ( unsigned_16 i = ToTypeIndex(0);
          i < ToTypeIndex(_globalTypingInfo.Entries()); i++ ) {
        delete */

void SstGlobalTypes::FixUpTypes( const type_index startIndex )
/************************************************************/
{
    for ( uint i = startIndex;
          i < ToTypeIndex(_globalTypingInfo.Entries()); i++ ) {
        _globalTypingInfo[i] -> FixTypeIndex();
    }
}

void SstGlobalTypes::ProcessTypes( LFLocalTypeRecord* target )
/************************************************************/
{
    WCPtrSListIter<LFTypeRecord> iter(_globalTypingInfo.Find(target));
    while ( ++iter ) {
        if ( iter.current() -> IsEquivalent( *target ) ) {
            return;
        }
    }
    InsertNewType(target);
}

bool SstGlobalTypes::LoadTypes( Retriever& retriever, const module mod )
/**********************************************************************/
{
    char*       buffer;
    unsigned_32 length;
    if ( ! retriever.ReadSubsection(buffer,length,sstTypes,mod) ) {
        return false;
    }
    char*       end = &buffer[length];
    if ( * (unsigned_32 *) buffer != CV4_HEADER ) {
        ::Warning("invalid header dectected in types.");
    }
    // skip the header.
    buffer += LONG_WORD;
    // if it is the first module, just load the types into the global
    // table without packing.
    if ( mod == 1 ) {
        while ( buffer < end ) {
            _globalTypingInfo.Insert( new LFGlobalTypeRecord(ToTypeIndex(_globalTypingInfo.Entries()),buffer));
            buffer += WORD + * (unsigned_16 *) buffer;
        }
        TypeMap.Set( CV_FIRST_NONPRIM + _globalTypingInfo.Entries() );
        return true;
    } else {
        while ( buffer < end ) {
            _localTypingInfo.Insert( new LFLocalTypeRecord(ToTypeIndex(_localTypingInfo.Entries()),buffer));
            buffer += WORD + * (unsigned_16 *) buffer;
        }
        TypeMap.Reset(_localTypingInfo.Entries());
    }
    uint oldCount = _globalTypingInfo.Entries();

    // Full type packing is not implemented due to its long running time
    // when input is relatively big.  Here only packs records that can be
    // hashed, these records are structs, unions, enums, class.  And they
    // will likely reference a lot of other records, so partial packing
    // is not that bad after all.

    WCPtrSListIter<LFTypeRecord> iter(_localTypingInfo._hashRecords);
    while ( ++iter ) {
        if ( ! TypeMap.IsDone( iter.current() -> TypeIndex() ) ) {
            ProcessTypes( iter.current() );
        }
    }
    // Anything that is left will be inserted as a new type into the global
    // table.
    iter.reset(_localTypingInfo._otherRecords);
    while ( ++iter ) {
        if ( ! TypeMap.IsDone( iter.current() -> TypeIndex() ) ) {
            InsertNewType( iter.current() );
        }
    }
    FixUpTypes(ToTypeIndex(oldCount));

    // Selectively destruct the types in the local table.  If the type is
    // a new type, then only release the memory of a bare LFLocalTypeRecord.
    // Otherwise, release the memory of a bare record plus the pointer to
    // LFLeafStruct.  This is bad style, but can boost performance by not
    // having to create a constructor and do a pointer copy only when
    // transfer types from the local table to the global table.  See the
    // LFGlobalTypeRecord declaration for more details.

    for ( uint i = CV_FIRST_NONPRIM;
          i < ToTypeIndex(_localTypingInfo.Entries()); i++ ) {
        if ( ! TypeMap.IsNewType(i) ) {
        // If not a new type, release pointer memory as well.
            _localTypingInfo[i]->ManualDestruct();
        }
        // release bare LFLocalTypeRecord memory.
        /*
        if (_localTypingInfo[i]) {
        delete _localTypingInfo[i];
            _localTypingInfo[i]=NULL;
        }
        */
    }
    _localTypingInfo.Clear();
    return true;
}

void SstGlobalTypes::Append( LFLocalTypeRecord* typeRecord )
/**********************************************************/
{
    _globalTypingInfo.Insert( new LFGlobalTypeRecord (typeRecord));
}

void SstGlobalTypes::Put( ExeMaker& eMaker ) const
{
    unsigned_32 offset = 0;
    uint        i;

    eMaker.DumpToExe( (unsigned_32) 0 );
    eMaker.DumpToExe( (unsigned_32) _globalTypingInfo.Entries() );
    for ( i = ToTypeIndex(0); i < ToTypeIndex(_globalTypingInfo.Entries()); i++ ) {
        eMaker.DumpToExe( offset );
        offset += _globalTypingInfo[i] -> Length();
    }
    for ( i = ToTypeIndex(0); i < ToTypeIndex(_globalTypingInfo.Entries()); i++ ) {
        _globalTypingInfo[i] -> Put( eMaker );
    }
}

uint SymbolSubsection::DumpPageAlign( ExeMaker&         eMaker,
                                      const uint        padCount,
                                      const unsigned_8  padVal )
/**************************************************************/
{
    eMaker.DumpToExe((unsigned_16) (padCount+WORD));
    eMaker.DumpToExe((unsigned_16) S_ALIGN);
    for ( uint i = 0; i < padCount; i++ ) {
        eMaker.DumpToExe(padVal);
    }
    return ( LONG_WORD + padCount );
}

uint SymbolSubsection::PageAlign( ExeMaker&         eMaker,
                                  const uint        length,
                                  const unsigned_32 offset )
/*****************************************************************/
{
    uint pad = SymbolSubsection::PadCount(length,offset);
    if ( pad > 0 ) {
        return SymbolSubsection::DumpPageAlign(eMaker,pad);
    }
    return 0;
}

uint SymbolSubsection::PadCount( const uint        length,
                                 const unsigned_32 offset )
/***************************************************************/
{
    uint pad = ALIGN_LENGTH - offset % ALIGN_LENGTH;
    if ( length > pad ) {
        return pad;
    }
    return 0;
}

int SstAlignSym::InsertOneRecord( SymbolStruct* sym )
/****************************************************/
{
    uint pad = SymbolSubsection::PadCount(sym->Length(),_currentOffset);
    if ( pad > 0 ) {
        if (!_symbolInfo.append( new CSPageAlign(pad) )) {
            return 0;
    }
        _currentOffset += pad + LONG_WORD;
    }
    sym -> SetOffset( _currentOffset );
    _currentOffset += sym -> Length();
    return _symbolInfo.append( sym );
}

int SstAlignSym::Insert( SymbolStruct* sym )
/*******************************************/
{
    uint seg = sym -> CodeSegment();
    if ( seg != NO_SEGMENT ) {
        if ( _lastStartSym[seg] == 0 ) {
            SymbolStruct* newSSym = new CSStartSearch(seg);
            _lastStartSym[seg] = newSSym;
            if (!_symbolInfo.insert(newSSym)) {
                cerr << "Failed to insert in SstAlignSym::Insert(), A\n";
                cerr.flush();
                return 0;
            }
        }
    }
    if (!InsertOneRecord( sym )) {
        cerr << "Failed to insert in SstAlignSym::Insert(), B\n";
        cerr.flush();
        return 0;
    }
    if ( sym -> IsStartSym() ) {
        if ( ! symStack.isEmpty() ) {
            sym -> SetParent( symStack.top() -> Offset() );
        } else {
            sym -> SetParent( 0 );
            assert( _lastStartSym[seg] );
            _lastStartSym[seg] -> SetNext( sym -> Offset() );
            _lastStartSym[seg] = sym;
        }
        if (!symStack.push(sym)) {
            cerr << "Failed to push sym to symStack\n";
            cerr.flush();
            return 0;
        }
        return 1;
    }
    if ( sym -> IsEndSym() ) {
        symStack.pop() -> SetEnd( sym -> Offset() );
    }
    return 1;
}

void SstAlignSym::Put( ExeMaker& eMaker ) const
/*********************************************/
{
    WCPtrConstSListIter<SymbolStruct> iter(_symbolInfo);
    eMaker.DumpToExe((unsigned_32) CV4_HEADER );
    while ( ++iter ) {
        iter.current() -> Put( eMaker );
    }
}

void SstGlobalSym::Put( ExeMaker& eMaker, const uint cSeg )
/***************************************************************/
{
    if ( _symbolInfo.isEmpty() ) {
        eMaker.DumpToExe( (unsigned_16) 0 );
        eMaker.DumpToExe( (unsigned_16) 0 );
        eMaker.DumpToExe( (unsigned_32) 0 );
        eMaker.DumpToExe( (unsigned_32) 0 );
        eMaker.DumpToExe( (unsigned_32) 0 );
        return;
    }
    unsigned_32 currentOffset = 0;
    /*
    cerr << "entered SstGlobalSym::Put()\n";
    cerr << "creating nameHash for ";
    cerr << _symbolInfo.entries();
    cerr << " symbols.\n";
    cerr.flush();
    */
    eMaker.DumpToExe( (unsigned_16) DEFAULT_NAME_HASH );
    eMaker.DumpToExe( (unsigned_16) DEFAULT_ADDR_HASH );
    streampos pos = eMaker.TellPos();
    eMaker.Reserve(3*LONG_WORD);
    NameHashTable nameHash(_symbolInfo.entries());
    /*
    cerr << "NameHashTable complete\n";
    cerr.flush();
    */
    AddrHashTable addrHash(cSeg);
    /*
    cerr << "AddrHashTable complete\n";
    cerr.flush();
    */

    //WCPtrConstSListIter<SymbolStruct> iter(_symbolInfo);
    SymbolStruct* currentPtr = NULL;
    long cnt=0, mcnt;
    mcnt = _symbolInfo.entries();
    /*
    cerr << "_symbolInfo has ";
    cerr << mcnt;
    cerr << " entries.\n";
    cerr.flush();
    */
    //while ( ++iter ) {
    while ( cnt++ < mcnt) {
        /*
        cerr << "global Symbol count: ";
        cerr << cnt;
        cerr << "\n";
        cerr.flush();
        */
        //currentPtr = iter.current();
        currentPtr = _symbolInfo.get(cnt);

        //cerr << "OK\n";
        //cerr.flush();

        currentPtr -> SetOffset(currentOffset);
        if ( currentPtr -> cSum() == NO_CHKSUM ) {
            if ( addrHash.TryToInsert(currentPtr) ) {
                currentOffset += SymbolSubsection::PageAlign(eMaker,currentPtr->Length(),currentOffset);
                currentOffset += currentPtr -> Length();
                currentPtr -> Put(eMaker);
            }
        } else if ( nameHash.TryToInsert(currentPtr) ) {
                currentOffset += SymbolSubsection::PageAlign(eMaker,currentPtr->Length(),currentOffset);
                currentOffset += currentPtr -> Length();
                currentPtr -> Put(eMaker);
                addrHash.TryToInsert(currentPtr);
        }
    }
    currentOffset += SymbolSubsection::DumpPageAlign(eMaker,LONG_WORD,0xff);
    unsigned_32 preNameHasPos = eMaker.TellPos();
    nameHash.Put(eMaker);
    unsigned_32 suNameHasPos = eMaker.TellPos();
    addrHash.Put(eMaker);
    unsigned_32 currentPos = eMaker.TellPos();
    eMaker.SeekTo(pos);
    eMaker.DumpToExe(currentOffset);
    eMaker.DumpToExe(suNameHasPos - preNameHasPos);
    eMaker.DumpToExe(currentPos - suNameHasPos);
    eMaker.SeekTo(currentPos);
}

SymbolStruct* NameHashTable::Find( const WCPtrSList<chain_table>& lst,
                                   const unsigned_32              cSum ) const
/****************************************************************************/
{
    WCPtrConstSListIter<chain_table> iter(lst);
    while ( ++iter ) {
        if ( iter.current() -> _checkSum == cSum ) {
            return iter.current() -> _symPtr;
        }
    }
    return NULL;
}

bool NameHashTable::TryToInsert( SymbolStruct* sym )
/**************************************************/
{
    unsigned_32 ckSum = sym -> cSum();
    if ( ckSum == NO_CHKSUM ) {
        return false;
    }
    uint   bucket = ckSum % _cHash;
    SymbolStruct* eqSym = Find(_cTab[bucket],ckSum);
    if ( eqSym != NULL ) {
        const char* name = sym -> Name();
        if ( memcmp(name,eqSym->Name(),*name+1) != 0 ) {
            _cTab[bucket].append( new chain_table(sym,ckSum) );
            return true;
        }
        return false;
    }
    _cTab[bucket].append( new chain_table(sym,ckSum) );
    return true;
}

void NameHashTable::Put( ExeMaker& eMaker ) const
/***********************************************/
{
    uint    i;

    eMaker.DumpToExe( (unsigned_16) _cHash);
    eMaker.DumpToExe((unsigned_16) 0);
    unsigned_32  chain_offset = 0;
    for ( i = 0; i < _cHash; i++ ) {
        eMaker.DumpToExe(chain_offset);
        chain_offset += _cTab[i].entries() * (2*LONG_WORD);
    }
    for ( i = 0; i < _cHash; i++ ) {
        eMaker.DumpToExe((unsigned_32) _cTab[i].entries());
    }
    chain_table* currentPtr;
    WCPtrConstSListIter<chain_table> iter;
    for ( i = 0; i < _cHash; i++ ) {
        iter.reset(_cTab[i]);
        while ( ++iter ) {
            currentPtr = iter.current();
            eMaker.DumpToExe(currentPtr -> _offset);
            eMaker.DumpToExe(currentPtr -> _checkSum);
        }
    }
}

bool AddrHashTable::TryToInsert( SymbolStruct* sym )
/**************************************************/
{
    uint seg = sym -> CodeSegment();
    if ( seg == NO_SEGMENT ) {
        seg = sym -> DataSegment();
        if ( seg == NO_SEGMENT ) {
            return false;
        }
    }
    if ( seg > 0 ) {
        --seg; // segments are 1 based.
        _oTab[seg].append( new offset_table(sym->Offset(),sym->MemOffset()));
        return true;
    }
    return false;
}

int AddrHashTable::Compare( const void* op1, const void* op2 )
/************************************************************/
{
    offset_table* otPtr1 = * (offset_table **) op1;
    offset_table* otPtr2 = * (offset_table **) op2;
    if ( otPtr1 -> _memOffset > otPtr2 -> _memOffset ) {
        return 1;
    }
    if ( otPtr1 -> _memOffset < otPtr2 -> _memOffset ) {
        return -1;
    }
    return 0;
}

void AddrHashTable::Put( ExeMaker& eMaker ) const
/***********************************************/
{
    unsigned_32  chain_offset = 0;
    uint         i;
    uint         j;

    eMaker.DumpToExe( (unsigned_16) _cSeg);
    eMaker.DumpToExe((unsigned_16) 0);
    for ( i = 0; i < _cSeg; i++ ) {
        eMaker.DumpToExe(chain_offset);
        chain_offset += _oTab[i].entries() * (2*LONG_WORD);
    }
    for ( i = 0; i < _cSeg; i++ ) {
        eMaker.DumpToExe((unsigned_32) _oTab[i].entries());
    }
    offset_table** tempTable;
    WCPtrConstSListIter<offset_table> iter;
    for ( i = 0; i < _cSeg; i++ ) {
        /*
        cerr << "AddrHashTable::Put(), _cSeg ";
        cerr << i;
        cerr << "\n";
        cerr.flush();
        */
        tempTable = new offset_table* [_oTab[i].entries()];
        iter.reset(_oTab[i]);
        for ( j = 0; ++iter; j++ ) {
            tempTable[j] = iter.current();
        }
        qsort(tempTable,_oTab[i].entries(),sizeof(offset_table*),AddrHashTable::Compare);
        /*
        cerr << "AddrHashTable has ";
        cerr << _oTab[i].entries();
        cerr << " entries\n";
        cerr.flush();
        */

        for ( j = 0; j < _oTab[i].entries(); j++ ) {
            eMaker.DumpToExe(tempTable[j] -> _fileOffset);
            eMaker.DumpToExe(tempTable[j] -> _memOffset);
        }
        delete [] tempTable;
    }
    /*
    cerr << "AddHashTable done.\n";
    cerr.flush();
    */
}

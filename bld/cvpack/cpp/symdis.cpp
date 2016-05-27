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


#include "common.hpp"
#include "cverror.hpp"
#include "symdis.hpp"

#include "cssymbol.hpp"

PCFN SymbolDistributor::_symConstructorTable[] = {
    SymbolStruct::Error,
    CSCompile::Construct,
    CSRegister::Construct,
    CSConstant::Construct,
    CSUdt::Construct,
    CSStartSearch::Construct,
    CSEndBlock::Construct,
    CSSkipRecord::Construct,
    SymbolStruct::Error,
    CSObjName::Construct,
    CSEndOfArg::Construct,
    CSCobolUdt::Construct,
    CSManyReg::Construct,
    CSReturn::Construct,
    CSEntryThis::Construct,
    CSBPRel16::Construct,
    CSLData16::Construct,
    CSGData16::Construct,
    CSPub16::Construct,
    CSLProc16::Construct,
    CSGProc16::Construct,
    CSThunk16::Construct,
    CSBlock16::Construct,
    CSWith16::Construct,
    CSLabel16::Construct,
    CSCExModel16::Construct,
    CSVftPath16::Construct,
    CSRegRel16::Construct,
    CSBPRel32::Construct,
    CSLData32::Construct,
    CSGData32::Construct,
    CSPub32::Construct,
    CSLProc32::Construct,
    CSGProc32::Construct,
    CSThunk32::Construct,
    CSBlock32::Construct,
    CSWith32::Construct,
    CSLabel32::Construct,
    CSCExModel32::Construct,
    CSVftPath32::Construct,
    CSRegRel32::Construct,
    CSLThread32::Construct,
    CSGThread32::Construct,
    // MIPS.
    SymbolStruct::Error,
    SymbolStruct::Error,
    // Ref symbols.
    SymbolStruct::Error,
    SymbolStruct::Error,
    SymbolStruct::Error
};

void SymbolDistributor::Build( const char*  ptr,
                               const uint   module,
                               SstAlignSym& alignSym ) const
/**********************************************************/
{
    SymbolStruct*   symPtr = CreateSym(ptr);

    // No packing done for initial module.
    if ( module > 1 ) {
        symPtr -> FixType();
    }

    if ( symPtr ) {
        if ( symPtr -> IsModuleSpecific() ) {
            alignSym.Insert( symPtr );
            return;
        }
        if ( symPtr -> IsGlobalProc() ) {
            alignSym.Insert( symPtr );
            _globalSym.Insert(CreateProcRef(symPtr,module));
            return;
        }
        if ( symPtr -> IsStaticProc() ) {
            alignSym.Insert( symPtr );
            _staticSym.Insert(CreateProcRef(symPtr,module));
            return;
        }
        if ( symPtr -> IsStaticData() ) {
            alignSym.Insert( symPtr );
            _staticSym.Insert(CreateDataRef(symPtr,module));
            return;
        }
        if ( alignSym.IsInGlobalScope() ) {
            // what if globalSym fail to insert the record?
            int rc;
            rc=_globalSym.Insert(symPtr);
            if (!rc) {
                cerr << "Error: failed to insert symbol to _globalSym";
                cerr.flush();
            }
            return;
        }
        int rc;
        rc=alignSym.Insert(symPtr);
        if (!rc) {
            cerr << "Error: failed to insert symbol to alignSym";
            cerr.flush();
        }
    }
}

bool SymbolDistributor::Distribute( const uint   module,
                                    SstAlignSym& alignSym ) const
/***************************************************************/
{
    unsigned_32 length;
    char*       dataPtr;
    if ( ! _retriever.ReadSubsection(dataPtr,length,sstSymbols,module) ) {
        return false;
    }
    char*       end = &dataPtr[length];
    if ( * (unsigned_32 *) dataPtr != CV4_HEADER ) {
        ::Warning("invalied signature detected in symbol table");
    }
    // skip the header of value 0x00000001 (CV4).
    dataPtr += LONG_WORD;
    while ( dataPtr < end ) {
        Build(dataPtr, module, alignSym);
        dataPtr += * ( unsigned_16 * ) dataPtr + WORD;
    }
    return true;
}


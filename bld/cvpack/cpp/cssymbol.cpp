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


#include <string.hpp>
#include <stdlib.h>
#include "common.hpp"
#include "cverror.hpp"
#include "cssymbol.hpp"
#include "symdis.hpp"

extern uint NumLeafLength( const char* );

char* SymbolStruct::_padTable[] = {
    NULL,"\0","\0\0","\0\0\0",
};

// unused function section :
bool SymbolStruct::operator == ( const SymbolStruct& ) const
{
    throw InternalError("SymbolStruct == gets call");
}
// end section.


SymbolStruct::SymbolStruct( const unsigned_16  length,
                            const symbol_index leaf )
/****************************************************/
        : _length( length ),
          _leaf( leaf ),
          _offset( 0 ),
          _pad( 0 ),
          _namePlusVar(NULL,0)
{
    Init();
}

SymbolStruct::SymbolStruct( const s_common common )
        : _length( common.length ),
          _leaf( common.code ),
          _offset( 0 ),
          _pad( 0 ),
          _namePlusVar(NULL,0)
/*************************************************/
{
    Init();
}

SymbolStruct::SymbolStruct( const s_common    common,
                            const char*       namePVar,
                            const uint        vLen )
        : _length( common.length ),
          _leaf( common.code ),
          _offset( 0 ),
          _pad( 0 ),
          _namePlusVar(namePVar,vLen)
/*****************************************************/
{
    Init();
}

unsigned_8 SymbolStruct::byt_toupper( const unsigned_8 b )
/********************************************************/
{
    return ( b&0xdf );
}

unsigned_32 SymbolStruct::dwrd_toupper( const unsigned_32 dw )
/************************************************************/
{
    return ( dw&0xdfdfdfdf );
}

// Assume length prefix name.
unsigned_32 SymbolStruct::checkSum( const char* name )
/****************************************************/
{
    unsigned_32 cb = *name;
    name += BYTE;
    unsigned_32 ulEnd = 0;
    while ( cb & 3 ) {
        ulEnd |= byt_toupper( name[cb-1] );
        ulEnd <<= 8;
        cb -= 1;
    }

    unsigned_32  cul = cb/4;
    unsigned_32  ulSum = 0;
    unsigned_32* lpulName = (unsigned_32 *) name;

    for ( unsigned_32 iul = 0; iul < cul; iul++ ) {
        ulSum ^= dwrd_toupper(lpulName[iul]);
        ulSum = _lrotl(ulSum,4);
    }
    ulSum ^= ulEnd;
    return ulSum;
}

SymbolStruct* SymbolStruct::Error( const char* )
/**********************************************/
{
    throw InternalError("encounter undefined symbol in sstSymbol.");
}

SymbolStruct* CSCompile::Construct( const char* ptr )
/***************************************************/
{
    return new CSCompile ( * (s_common *) ptr,
                           * (cs_compile *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_compile) );
}

SymbolStruct* CSRegister::Construct( const char* ptr )
/****************************************************/
{
    return new CSRegister ( * (s_common *) ptr,
                            * (cs_register *) (ptr+sizeof(s_common)),
                            ptr + sizeof(s_register),
                            GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_register)));
}


SymbolStruct* CSConstant::Construct( const char* ptr )
/****************************************************/
{
    const char* valuePtr = ptr + sizeof(s_common) + sizeof(cs_constant);
    unsigned_8  valueLen = NumLeafLength(valuePtr);
    const char* varPtr = valuePtr + valueLen;
    return new CSConstant ( * (s_common *) ptr,
                            * (cs_constant *) (ptr+sizeof(s_common)),
                            valuePtr, valueLen, varPtr,
                            GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_constant)+valueLen));

}

SymbolStruct* CSUdt::Construct( const char* ptr )
/***********************************************/
{
    return new CSUdt ( * (s_common *) ptr,
                       * (cs_udt *) (ptr+sizeof(s_common)),
                       ptr + sizeof(s_udt),
                       GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_udt)));
}

SymbolStruct* CSStartSearch::Construct( const char* ptr )
/*******************************************************/
{
    ::Warning("language processor emitted start search symbol.");
    return new CSStartSearch ( * (s_common *) ptr,
                               * (cs_ssearch *) (ptr+sizeof(s_common)) );
}

SymbolStruct* CSEndBlock::Construct( const char* ptr )
/****************************************************/
{
    return new CSEndBlock ( * (s_common *) ptr);
}

SymbolStruct* CSSkipRecord::Construct( const char* )
/**************************************************/
{
    return NULL;
}

SymbolStruct* CSObjName::Construct( const char* ptr )
/***************************************************/
{
    return new CSObjName ( * (s_common *) ptr,
                           * (cs_objname *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_objname),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_objname)));
}

SymbolStruct* CSEndOfArg::Construct( const char* ptr )
/****************************************************/
{
    return new CSEndOfArg ( * (s_common *) ptr );
}

SymbolStruct* CSCobolUdt::Construct( const char* ptr )
/**************************************************/
{
    return new CSCobolUdt ( * (s_common *) ptr,
                            * (cs_coboludt *) (ptr+sizeof(s_common)),
                            ptr + sizeof(s_coboludt),
                            GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_coboludt)));
}

SymbolStruct* CSManyReg::Construct( const char* ptr )
/***************************************************/
{
    return new CSManyReg ( * (s_common *) ptr,
                           * (cs_manyreg *) (ptr + sizeof(s_common)),
                           ptr + sizeof(s_manyreg),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_manyreg)));
}

SymbolStruct* CSReturn::Construct( const char* ptr )
/**************************************************/
{
    return new CSReturn ( * (s_common *) ptr,
                          * (cs_return *) (ptr+sizeof(s_common)),
                          ptr + sizeof(s_return),
                          GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_return)));
}

SymbolStruct* CSEntryThis::Construct( const char* ptr )
/*****************************************************/
{
    return new CSEntryThis ( * (s_common *) ptr,
                             SymbolDistributor::CreateSym(ptr+sizeof(s_common)));
}

SymbolStruct* CSBPRel16::Construct( const char* ptr )
/***************************************************/
{
    return new CSBPRel16 ( * (s_common *) ptr,
                           * (cs_bprel16 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_bprel16),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_bprel16)));
}

SymbolStruct* CSLData16::Construct( const char* ptr )
/***************************************************/
{
    return new CSLData16 ( * (s_common *) ptr,
                           * (cs_ldata16 *) (ptr+sizeof(s_common)),
                            ptr + sizeof(s_ldata16),
                            GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_ldata16)));
}

SymbolStruct* CSPub16::Construct( const char* ptr )
/**************************************************/
{
    return new CSPub16 ( * (s_common *) ptr,
                         * (cs_pub16 *) (ptr+sizeof(s_common)),
                         ptr + sizeof(s_pub16),
                         GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_pub16)));
}

SymbolStruct* CSLProc16::Construct( const char* ptr )
/**************************************************/
{
    return new CSLProc16 ( * (s_common *) ptr,
                           * (cs_lproc16 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_lproc16),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_lproc16)));
}

SymbolStruct* CSThunk16::Construct( const char* ptr )
/**************************************************/
{
    return new CSThunk16 ( * (s_common *) ptr,
                           * (cs_thunk16 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_thunk16),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_thunk16)));
}

SymbolStruct* CSBlock16::Construct( const char* ptr )
/**************************************************/
{
    return new CSBlock16 ( * (s_common *) ptr,
                           * (cs_block16 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_block16),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_block16)));
}

SymbolStruct* CSWith16::Construct( const char* ptr )
/**************************************************/
{
    return new CSWith16 ( * (s_common *) ptr,
                          * (cs_with16 *) (ptr+sizeof(s_common)),
                          ptr + sizeof(s_with16) );
}

SymbolStruct* CSLabel16::Construct( const char* ptr )
/**************************************************/
{
    return new CSLabel16 ( * (s_common *) ptr,
                           * (cs_label16 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_label16),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_label16)));
}

SymbolStruct* CSCExModel16::Construct( const char* ptr )
/*****************************************************/
{
    return new CSCExModel16 ( * (s_common *) ptr,
                              * (cs_cexmodel16 *) (ptr+sizeof(s_common)),
                              ptr + sizeof(s_cexmodel16),
                              GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_cexmodel16)));
}

SymbolStruct* CSVftPath16::Construct( const char* ptr )
/**************************************************/
{
    return new CSVftPath16 ( * (s_common *) ptr,
                             * (cs_vftpath16 *) (ptr+sizeof(s_common)) );
}

SymbolStruct* CSRegRel16::Construct( const char* ptr )
/**************************************************/
{
    return new CSRegRel16 ( * (s_common *) ptr,
                            * (cs_regrel16 *) (ptr+sizeof(s_common)),
                            ptr + sizeof(s_regrel16),
                            GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_regrel16)));
}

SymbolStruct* CSBPRel32::Construct( const char* ptr )
/***************************************************/
{
    return new CSBPRel32 ( * (s_common *) ptr,
                           * (cs_bprel32 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_bprel32),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_bprel32)));
}

SymbolStruct* CSLData32::Construct( const char* ptr )
/***************************************************/
{
    return new CSLData32 ( * (s_common *) ptr,
                           * (cs_ldata32 *) (ptr+sizeof(s_common)),
                            ptr + sizeof(s_ldata32),
                            GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_ldata32)));
}

SymbolStruct* CSPub32::Construct( const char* ptr )
/*************************************************/
{
    return new CSPub32 ( * (s_common *) ptr,
                         * (cs_pub32 *) (ptr+sizeof(s_common)),
                         ptr + sizeof(s_pub32),
                         GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_pub32)));
}

SymbolStruct* CSPub32_new::Construct( const char* ptr )
/*************************************************/
{
    return new CSPub32_new ( * (s_common *) ptr,
                         * (cs_pub32_new *) (ptr+sizeof(s_common)),
                         ptr + sizeof(s_pub32_new),
                         GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_pub32_new)));
}

SymbolStruct* CSLProc32::Construct( const char* ptr )
/***************************************************/
{
    return new CSLProc32 ( * (s_common *) ptr,
                           * (cs_lproc32 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_lproc32),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_lproc32)));
}

SymbolStruct* CSThunk32::Construct( const char* ptr )
/***************************************************/
{
    return new CSThunk32 ( * (s_common *) ptr,
                           * (cs_thunk32 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_thunk32),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_thunk32)));
}

SymbolStruct* CSBlock32::Construct( const char* ptr )
/***************************************************/
{
    return new CSBlock32 ( * (s_common *) ptr,
                           * (cs_block32 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_block32),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_block32)));
}

SymbolStruct* CSWith32::Construct( const char* ptr )
/**************************************************/
{
    return new CSWith32 ( * (s_common *) ptr,
                          * (cs_with32 *) (ptr+sizeof(s_common)),
                          ptr + sizeof(s_with32) );
}

SymbolStruct* CSLabel32::Construct( const char* ptr )
/***************************************************/
{
    return new CSLabel32 ( * (s_common *) ptr,
                           * (cs_label32 *) (ptr+sizeof(s_common)),
                           ptr + sizeof(s_label32),
                           GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_label32)));
}

SymbolStruct* CSCExModel32::Construct( const char* ptr )
/******************************************************/
{
    return new CSCExModel32 ( * (s_common *) ptr,
                              * (cs_cexmodel32 *) (ptr+sizeof(s_common)),
                              ptr + sizeof(s_cexmodel32),
                              GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_cexmodel32)));
}

SymbolStruct* CSVftPath32::Construct( const char* ptr )
/*****************************************************/
{
    return new CSVftPath32 ( * (s_common *) ptr,
                             * (cs_vftpath32 *) (ptr+sizeof(s_common)) );
}

SymbolStruct* CSRegRel32::Construct( const char* ptr )
/****************************************************/
{
    return new CSRegRel32 ( * (s_common *) ptr,
                            * (cs_regrel32 *) (ptr+sizeof(s_common)),
                            ptr + sizeof(s_regrel32),
                            GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_regrel32)));
}

SymbolStruct* CSLThread32::Construct( const char* ptr )
/*****************************************************/
{
    return new CSLThread32 ( * (s_common *) ptr,
                             * (cs_lthread32 *) (ptr+sizeof(s_common)),
                             ptr + sizeof(s_lthread32),
                             GetVarLength(*(unsigned_8 *)ptr,sizeof(cs_lthread32)));
}

void SymbolStruct::Put( ExeMaker& eMaker ) const
/**********************************************/
{
    eMaker.DumpToExe(_length);
    eMaker.DumpToExe(_leaf);
    DerivedPut(eMaker);
    eMaker.DumpToExe(_namePlusVar);
    if ( _pad > 0 ) {
        eMaker.DumpToExe( _padTable[_pad], _pad );
    }
}

void CSCompile::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe( &_compile, sizeof(cs_compile) );
    eMaker.DumpToExe( _version, _verlen );
}

void CSRegister::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_register,sizeof(cs_register));
}

void CSConstant::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_constant, sizeof(cs_constant));
    eMaker.DumpToExe(_value);
}

void CSUdt::DerivedPut( ExeMaker& eMaker ) const
/**********************************************/
{
    eMaker.DumpToExe(&_udt, sizeof(cs_udt));
}

void CSStartSearch::DerivedPut( ExeMaker& eMaker ) const
/******************************************************/
{
    eMaker.DumpToExe(&_ssearch,sizeof(cs_ssearch));
}

void CSObjName::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_objName,sizeof(cs_objname));
}

void CSCobolUdt::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_cobolUdt,sizeof(cs_coboludt));
}

void CSManyReg::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_manyReg,sizeof(cs_manyreg));
    eMaker.DumpToExe(_regList);
}

void CSReturn::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_return,sizeof(cs_return));
    eMaker.DumpToExe(_data);
}

void CSEntryThis::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    _symPtr -> Put(eMaker);
}

void CSBPRel16::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_bpRel16,sizeof(cs_bprel16));
}

void CSLData16::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_lData16,sizeof(cs_ldata16));
}

void CSPub16::DerivedPut( ExeMaker& eMaker ) const
/************************************************/
{
    eMaker.DumpToExe(&_pub16,sizeof(cs_pub16));
}

void CSLProc16::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_lProc16,sizeof(cs_lproc16));
}

void CSThunk16::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_thunk16,sizeof(cs_thunk16));
}

void CSBlock16::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_block16,sizeof(cs_block16));
}

void CSWith16::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_with16,sizeof(cs_with16));
}

void CSLabel16::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_label16,sizeof(cs_label16));
}

void CSCExModel16::DerivedPut( ExeMaker& eMaker ) const
/*****************************************************/
{
    eMaker.DumpToExe(&_cExModel16,sizeof(cs_cexmodel16));
    eMaker.DumpToExe(_variant);
}

void CSVftPath16::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_vftPath16,sizeof(cs_vftpath16));
}

void CSRegRel16::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_regRel16,sizeof(cs_regrel16));
}

void CSBPRel32::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_bpRel32,sizeof(cs_bprel32));
}

void CSLData32::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_lData32,sizeof(cs_ldata32));
}

void CSPub32::DerivedPut( ExeMaker& eMaker ) const
/************************************************/
{
    eMaker.DumpToExe(&_pub32,sizeof(cs_pub32));
}

void CSPub32_new::DerivedPut( ExeMaker& eMaker ) const
/************************************************/
{
    eMaker.DumpToExe(&_pub32_new,sizeof(cs_pub32_new));
}

void CSLProc32::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_lProc32,sizeof(cs_lproc32));
}

void CSThunk32::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_thunk32,sizeof(cs_thunk32));
}

void CSBlock32::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_block32,sizeof(cs_block32));
}

void CSWith32::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_with32,sizeof(cs_with32));
}

void CSLabel32::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_label32,sizeof(cs_label32));
}

void CSCExModel32::DerivedPut( ExeMaker& eMaker ) const
/*****************************************************/
{
    eMaker.DumpToExe(&_cExModel32,sizeof(cs_cexmodel32));
    eMaker.DumpToExe(_variant);
}

void CSVftPath32::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_vftPath32,sizeof(cs_vftpath32));
}

void CSRegRel32::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_regRel32,sizeof(cs_regrel32));
}

void CSLThread32::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_lThread32,sizeof(cs_lthread32));
}

void CSProcRef::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(_checkSum);
    eMaker.DumpToExe(_symOffset);
    eMaker.DumpToExe( (unsigned_16) _module);
}

void CSDataRef::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(_checkSum);
    eMaker.DumpToExe(_symOffset);
    eMaker.DumpToExe( (unsigned_16) _module);
}

void CSPageAlign::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    for ( uint i = 0; i < _padLength; i++ ) {
        eMaker.DumpToExe((unsigned_8) _padValue);
    }
}

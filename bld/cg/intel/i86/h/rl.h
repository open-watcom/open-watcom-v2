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


RL( &Empty,             RL_,                     OTHER,          0 ),
RL( &ALReg,             RL_AL,                   ONE_BYTE,       0 ),
RL( &AHReg,             RL_AH,                   ONE_BYTE,       1 ),
RL( &AXReg,             RL_AX,                   TWO_BYTE,       0 ),
RL( &CLReg,             RL_CL,                   ONE_BYTE,       2 ),
RL( &CXReg,             RL_CX,                   TWO_BYTE,       1 ),
RL( &DXReg,             RL_DX,                   TWO_BYTE,       2 ),
RL( &DXAXReg,           RL_DX_AX,                FOUR_BYTE,      0 ),
RL( &ESBXReg,           RL_ES_BX,                FOUR_BYTE,      1 ),
RL( &CXBXReg,           RL_CX_BX,                FOUR_BYTE,      2 ),
RL( &ByteRegs,          RL_BYTE,                 ONE_BYTE,       3 ),
RL( &LowByteRegs,       RL_LOWBYTE,              ONE_BYTE,       4 ),
RL( &WordRegs,          RL_WORD,                 TWO_BYTE,       3 ),
RL( &TwoByteRegs,       RL_TWOBYTE,              TWO_BYTE,       4 ),
RL( &IndexRegs,         RL_INDEX,                TWO_BYTE,       5 ),
RL( &SegRegs,           RL_SEG,                  TWO_BYTE,       6 ),
RL( &LongIndexRegs,     RL_LONG_INDEX,           FOUR_BYTE,      3 ),
RL( &DoubleRegs,        RL_DOUBLE,               FOUR_BYTE,      4 ),
RL( &DblPtrRegs,        RL_DBL_OR_PTR,           FOUR_BYTE,      5 ),
RL( &QuadReg,           RL_8,                    EIGHT_BYTE,     0 ),
RL( &TempIndex,         RL_TEMP_INDEX,           TWO_BYTE,       7 ),
RL( &ST0Reg,            RL_ST0,                  FLOATING,       0 ),
RL( &STIReg,            RL_STI,                  FLOATING,       1 ),
RL( &WordOrSegReg,      RL_ANYWORD,              TWO_BYTE,       8 ),
RL( &EEch,              RL_EECH,                 OTHER,          0 ),
RL( &DXCLReg,           RL_DX_CL,                OTHER,          0 ),
RL( &AHCLReg,           RL_AH_CL,                OTHER,          0 ),

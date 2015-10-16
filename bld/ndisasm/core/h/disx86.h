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
* Description:  Instruction decoding for x86 architecture.
*
****************************************************************************/


extern const dis_range          X86RangeTable[];
extern const int                X86RangeTablePos[];
extern const unsigned char      X86MaxInsName;

extern dis_handler_return X86PrefixFwait( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixOpnd( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixAddr( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixRepe( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixRepne( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixLock( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixCS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixSS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixDS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixES( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixFS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86PrefixGS( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X86SReg_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86NoOp_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86String_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86ImmAcc_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MemAbsAcc_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86Abs_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86Rel_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86Imm_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86ImmReg_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86ImmImm_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86Reg_8( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X86AccAcc_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86JmpCC_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86Shift_16 ( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86SReg_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86NoOp_16( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X86ModRM_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86ModRMImm_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86ModRMAcc_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRM_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRM_test( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRM_bound( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRM_16B( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86Reg_16( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X86JmpCC_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86SRegModRM_16( dis_handle *h, void * d, dis_dec_ins *ins );
extern dis_handler_return X86SetCC( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MovCC_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86CRegReg_24( dis_handle *h, void * d, dis_dec_ins *ins );
extern dis_handler_return X86DRegReg_24( dis_handle *h, void * d, dis_dec_ins *ins );
extern dis_handler_return X86TRegReg_24( dis_handle *h, void * d, dis_dec_ins *ins );
extern dis_handler_return X86ModRMImm_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86NoOp_24( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X86ModRM_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRM_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRM_24B( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRM_24C( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86FTypeCond( dis_handle *h, void *d, dis_dec_ins * ins );
extern dis_handler_return X86FType3( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86FType3B( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86FType3C( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86FType2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86FTypeFSTSWAX( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86FType4( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86FType1( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86FTypeEnv( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MMRegModRM( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MMRegModRMImm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MMRegModRM_Rev( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MMRegImm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRM( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRMImm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRM_Rev( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegImm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRM64( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRM64Imm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRM64_Rev( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRM32( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRM32Imm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRM32_Rev( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRM32_Rev( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MMRegModRMMixed( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MMRegModRMMixedImm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86MMRegModRMMixed_Rev( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRMMixed( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRMMixedImm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86XMMRegModRMMixed_Rev( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRMMixed( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86RegModRMMixedImm( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X86_3DNow( dis_handle *h, void *d, dis_dec_ins *ins );

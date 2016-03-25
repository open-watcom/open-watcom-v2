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
* Description:  Instruction decoding for AMD64 architecture.
*
****************************************************************************/


extern const dis_range          X64RangeTable[];
extern const int                X64RangeTablePos[];
extern const unsigned char      X64MaxInsName;

extern dis_handler_return X64PrefixOpnd( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixRex( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixCS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixFS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixGS( dis_handle *h, void *d , dis_dec_ins *ins );
//extern dis_handler_return X64PrefixFwait( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixAddr( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixRepe( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixRepne( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixLock( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixSS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixDS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64PrefixES( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64SReg_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64NoOp_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64String_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64ImmAcc_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64MemAbsAcc_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64Abs_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64Rel_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64Imm_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64ImmReg_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64ImmImm_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64Reg_8( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X64AccAcc_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64JmpCC_8( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64Shift_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64SReg_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64NoOp_16( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X64ModRM_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64RegModRM_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64RegModRM_test( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64RegModRM_16B( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64RegModRM_16C( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64ModRMImm_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64Reg_16( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X64JmpCC_16( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64SRegModRM_16( dis_handle *h, void * d, dis_dec_ins *ins );
extern dis_handler_return X64NoOp_24( dis_handle *h, void *d , dis_dec_ins *ins );
extern dis_handler_return X64SetCC( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64MovCC_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64CRegReg_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64DRegReg_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64ModRM_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64RegModRM_24( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64RegModRM_24B( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64RegModRM_24C( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return X64ModRMImm_24( dis_handle *h, void *d, dis_dec_ins *ins );

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
* Description:  Instruction decoding for MIPS R4000 architecture.
*
****************************************************************************/


extern const dis_range          MIPSRangeTable[];
extern const int                MIPSRangeTablePos[];
extern const unsigned char      MIPSMaxInsName;

extern dis_handler_return MIPSNull( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSJType( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSCode( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSImmed1( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSImmed2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSImmed2U( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSShift( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSTrap1( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSTrap2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSRegD( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSRegS( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSReg2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSReg3( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSMulDiv( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSCache( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSMemory( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSJump1( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSJump2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSBranch1( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSBranch2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSBranch3( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSFGMove( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSFPUOp2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSFPUOp3( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSFPUCmp( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSFPUMemory( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return MIPSBranchCop( dis_handle *h, void *d, dis_dec_ins *ins );

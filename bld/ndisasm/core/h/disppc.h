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
* Description:  Instruction decoding for PowerPC architecture.
*
****************************************************************************/


extern const dis_range          PPCRangeTable[];
extern const int                PPCRangeTablePos[];
extern const unsigned char      PPCMaxInsName;

extern dis_handler_return PPCMath( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMathb( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMathd( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMathab( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMathda( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMathdb( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMath2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMathsa( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCImmediate( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCImmed2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCFloat( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCFloatdab( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCFloatdac( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCFloatCmpab( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCFloatdb( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCFloato( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMem1( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMemD1( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMem2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMemD2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCMem3( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCFloatMem1( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCFloatMem2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCBranch( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCCompare( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCCondition( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCConditionField( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCSpecial( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCShiftImmed( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCShiftImmedD( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCRotate( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCRotateD( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCRotateImmD( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCTrap( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return PPCNull( dis_handle *h, void *d, dis_dec_ins *ins );

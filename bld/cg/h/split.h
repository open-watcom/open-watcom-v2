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
* Description:  Prototypes for split.c
*
****************************************************************************/
extern  type_class_def  Unsigned[];
extern  type_class_def  Signed[]; 
extern  hw_reg_set      Op1Reg( instruction *ins );
extern  hw_reg_set      ResultReg( instruction *ins );
extern  hw_reg_set      ZapReg( instruction *ins );
extern  instruction     *MoveConst( unsigned_32 value, name *result, type_class_def class );
extern  instruction     *Reduce( instruction *ins );
extern  instruction     *rMOVOP1TEMP( instruction *ins );
extern  instruction     *rMOVOP2TEMP( instruction *ins );
extern  instruction     *rOP1REG( instruction *ins );
extern  instruction     *rOP2REG( instruction *ins );
extern  instruction     *rMOVRESREG( instruction *ins );
extern  instruction     *rMOVRESTEMP( instruction *ins );
extern  instruction     *rRESREG( instruction *ins );
extern  instruction     *rOP1RESREG( instruction *ins );
extern  instruction     *rUSEREGISTER( instruction *ins );
extern  instruction     *rCHANGESHIFT( instruction *ins );
extern  instruction     *rCLRHI_BW( instruction *ins );
extern  instruction     *rCLRHI_R( instruction *ins );
extern  instruction     *rCONVERT_LOW( instruction *ins );
extern  instruction     *rCYPHIGH( instruction *ins );
extern  instruction     *rCYPLOW( instruction *ins );
extern  instruction     *rDOUBLEHALF( instruction *ins );
extern  instruction     *rOP1MEM( instruction *ins );
extern  instruction     *rOP2MEM( instruction *ins );
extern  instruction     *rFORCERESMEM( instruction *ins );
extern  instruction     *rMAKEMOVE( instruction *ins );
extern  instruction     *rMAKEXORRR( instruction *ins );
extern  instruction     *rADDRR( instruction *ins );
extern  instruction     *rMOVOP2( instruction *ins );
extern  instruction     *rMOVOP1RES( instruction *ins );
extern  instruction     *rMOVOP2RES( instruction *ins );
extern  instruction     *rSWAPOPS( instruction *ins );
extern  instruction     *rSWAPCMP( instruction *ins );
extern  instruction     *rMOVEINDEX( instruction *ins );
extern  instruction     *rLOADOP2( instruction *ins );
extern  instruction     *rNEGADD( instruction *ins );
extern  instruction     *rMAKEADD( instruction *ins );
extern  instruction     *rMAKENEG( instruction *ins );
extern  instruction     *rMAKESUB( instruction *ins );
extern  instruction     *rCMPtrue( instruction *ins );
extern  instruction     *rCMPfalse( instruction *ins );
extern  instruction     *rOP1RESTEMP( instruction *ins );
extern  void    HalfType( instruction *ins );
extern  void    ChangeType( instruction *ins, type_class_def class );

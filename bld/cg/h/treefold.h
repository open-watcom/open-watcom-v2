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
* Description:  Prototypes for treefold.c
*
****************************************************************************/

extern  int     GetLog2( unsigned_32 value );
extern  tn      FoldTimes( tn left, tn rite, type_def *tipe );
extern  cfloat  *OkToNegate( cfloat *value, type_def *tipe );
extern  tn      FoldMinus( tn left, tn rite, type_def *tipe );
extern  tn      FoldPlus( tn left, tn rite, type_def *tipe );
extern  tn      FoldPow( tn left, tn rite, type_def *tipe );
extern  tn      FoldAnd( tn left, tn rite, type_def *tipe );
extern  tn      FoldOr( tn left, tn rite, type_def *tipe );
extern  tn      FoldXor( tn left, tn rite, type_def *tipe );
extern  tn      FoldRShift( tn left, tn rite, type_def *tipe );
extern  tn      FoldLShift( tn left, tn rite, type_def *tipe );
extern  tn      FoldDiv( tn left, tn rite, type_def *tipe );
extern  tn      FoldMod( tn left, tn rite, type_def *tipe );
extern  tn      Fold1sComp( tn left, type_def *tipe );
extern  tn      FoldUMinus( tn left, type_def *tipe );
extern  tn      FoldSqrt( tn left, type_def *tipe );
extern  tn      FoldLog( cg_op op, tn left, type_def *tipe );
extern  tn      FoldFlAnd( tn left, tn rite );
extern  tn      FoldFlOr( tn left, tn rite );
extern  tn      FoldFlNot( tn left );
extern  tn      FoldBitCompare( cg_op op, tn_btn left, tn rite );
extern  cfloat  *CnvCFToType( cfloat *cf, type_def *tipe );
extern  tn      FoldCompare( cg_op op, tn left, tn rite, type_def *tipe );
extern  tn      FoldPostGetsCompare( cg_op op, tn left, tn rite, type_def *tipe );
extern  an      FoldConsCompare( cg_op op, tn left, tn rite, type_def *tipe );
extern  bool    FoldIfTrue( tn left, label_handle lbl );
extern  bool    FoldIfFalse( tn left, label_handle lbl );

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


//
// FCTAB     : table of F-Code processing routines
//

#include "ftnstd.h"
#include "emitobj.h"

extern  void            FCPause(void);
extern  void            FCStop(void);
extern  void            FCSetIOCB(void);
extern  void            FCSetFmt(void);
extern  void            FCFmtString(void);
extern  void            FCSetUnit(void);
extern  void            FCSetIntl(void);
extern  void            FCSetErr(void);
extern  void            FCSetEnd(void);
extern  void            FCSetRec(void);
extern  void            FCSetIOS(void);
extern  void            FCSetAcc(void);
extern  void            FCSetBlnk(void);
extern  void            FCSetFile(void);
extern  void            FCSetForm(void);
extern  void            FCSetLen(void);
extern  void            FCSetStat(void);
extern  void            FCSetDir(void);
extern  void            FCSetFmtd(void);
extern  void            FCSetName(void);
extern  void            FCSetSeq(void);
extern  void            FCSetUFmtd(void);
extern  void            FCSetExst(void);
extern  void            FCSetNmd(void);
extern  void            FCSetNRec(void);
extern  void            FCSetNumb(void);
extern  void            FCSetOpen(void);
extern  void            FCSetRecl(void);
extern  void            FCSetNoFmt(void);
extern  void            FCExRead(void);
extern  void            FCExWrite(void);
extern  void            FCExOpen(void);
extern  void            FCExClose(void);
extern  void            FCExBack(void);
extern  void            FCExEndf(void);
extern  void            FCExRew(void);
extern  void            FCExInq(void);
extern  void            FCOutLOG1(void);
extern  void            FCOutLOG4(void);
extern  void            FCOutINT1(void);
extern  void            FCOutINT2(void);
extern  void            FCOutINT4(void);
extern  void            FCOutREAL(void);
extern  void            FCOutDBLE(void);
extern  void            FCOutXTND(void);
extern  void            FCOutCPLX(void);
extern  void            FCOutDBCX(void);
extern  void            FCOutXTCX(void);
extern  void            FCOutCHAR(void);
extern  void            FCInpLOG1(void);
extern  void            FCInpLOG4(void);
extern  void            FCInpINT1(void);
extern  void            FCInpINT2(void);
extern  void            FCInpINT4(void);
extern  void            FCInpREAL(void);
extern  void            FCInpDBLE(void);
extern  void            FCInpXTND(void);
extern  void            FCInpCPLX(void);
extern  void            FCInpDBCX(void);
extern  void            FCInpXTCX(void);
extern  void            FCInpCHAR(void);
extern  void            FCEndIO(void);
extern  void            FCPrologue(void);
extern  void            FCEpilogue(void);
extern  void            FCSubscript(void);
extern  void            FCCat(void);
extern  void            FCSubString(void);
extern  void            FCFmtAssign(void);
extern  void            FCFmtScan(void);
extern  void            FCAdd(void);
extern  void            FCSub(void);
extern  void            FCMul(void);
extern  void            FCDiv(void);
extern  void            FCPow(void);
extern  void            FCUMinus(void);
extern  void            FCPush(void);
extern  void            FCPop(void);
extern  void            FCDone(void);
extern  void            FCEqv(void);
extern  void            FCNEqv(void);
extern  void            FCOr(void);
extern  void            FCAnd(void);
extern  void            FCNot(void);
extern  void            FCPushConst(void);
extern  void            FCCmpEQ(void);
extern  void            FCCmpNE(void);
extern  void            FCCmpLT(void);
extern  void            FCCmpGE(void);
extern  void            FCCmpLE(void);
extern  void            FCCmpGT(void);
extern  void            FCJmpAlways(void);
extern  void            FCJmpFalse(void);
extern  void            FCDefineLabel(void);
extern  void            FCSelect(void);
extern  void            FCPushLit(void);
extern  void            FCAssign(void);
extern  void            FCIfArith(void);
extern  void            FCExecute(void);
extern  void            FCEndRB(void);
extern  void            FCWarpReturn(void);
extern  void            FCSFCall(void);
extern  void            FCDbgLine(void);
extern  void            FCCall(void);
extern  void            FCConvert(void);
extern  void            FCMakeComplex(void);
extern  void            FCMakeDComplex(void);
extern  void            FCMakeXComplex(void);
extern  void            FCDArgInit(void);
extern  void            FCAdvFillLo(void);
extern  void            FCAdvFillHi(void);
extern  void            FCAdvFillHiLo1(void);
extern  void            FCTrash(void);
extern  void            FCAltReturn(void);
extern  void            FCPushSCBLen(void);
extern  void            FCAddCmplx(void);
extern  void            FCSubCmplx(void);
extern  void            FCMulCmplx(void);
extern  void            FCDivCmplx(void);
extern  void            FCExpCmplx(void);
extern  void            FCAddMixCX(void);
extern  void            FCSubMixCX(void);
extern  void            FCMulMixCX(void);
extern  void            FCDivMixCX(void);
extern  void            FCExpMixCX(void);
extern  void            FCAddMixXC(void);
extern  void            FCSubMixXC(void);
extern  void            FCMulMixXC(void);
extern  void            FCDivMixXC(void);
extern  void            FCExpMixXC(void);
extern  void            FCUMinusCmplx(void);
extern  void            FCCmplxDone(void);
extern  void            FCStartDataStmt(void);
extern  void            FCPassCharArray(void);
extern  void            FCPassFieldCharArray(void);
extern  void            FCPrtArray(void);
extern  void            FCInpArray(void);
extern  void            FCFmtArrScan(void);
extern  void            FCIntlArrSet(void);
extern  void            FCNull(void);
extern  void            FCAssignAltRet(void);
extern  void            FCCCCmpEQ(void);
extern  void            FCCCCmpNE(void);
extern  void            FCXCCmpEQ(void);
extern  void            FCXCCmpNE(void);
extern  void            FCCXCmpEQ(void);
extern  void            FCCXCmpNE(void);
extern  void            FCCharCmpEQ(void);
extern  void            FCCharCmpNE(void);
extern  void            FCCharCmpLT(void);
extern  void            FCCharCmpGE(void);
extern  void            FCCharCmpLE(void);
extern  void            FCCharCmpGT(void);
extern  void            FCChar1CmpEQ(void);
extern  void            FCChar1CmpNE(void);
extern  void            FCChar1CmpLT(void);
extern  void            FCChar1CmpGE(void);
extern  void            FCChar1CmpLE(void);
extern  void            FCChar1CmpGT(void);
extern  void            FCFreeLabel(void);
extern  void            FCDoBegin(void);
extern  void            FCDoEnd(void);
extern  void            FCSetCCtrl(void);
extern  void            FCSetRecType(void);
extern  void            FCSetAction(void);
extern  void            FCStmtDefineLabel(void);
extern  void            FCStmtJmpAlways(void);
extern  void            FCComputedGOTO(void);
extern  void            FCStartRB(void);
extern  void            FCWarp(void);
extern  void            FCSetAtEnd(void);
extern  void            FCAssignedGOTOList(void);
extern  void            FCStartSF(void);
extern  void            FCEndSF(void);
extern  void            FCSFReferenced(void);
extern  void            FCPassLabel(void);
extern  void            FCFlip(void);
extern  void            FCCmplxFlip(void);
extern  void            FCXCFlip(void);
extern  void            FCCXFlip(void);
extern  void            FCChkIOStmtLabel(void);
extern  void            FCUnaryMul(void);
extern  void            FCIChar(void);
extern  void            FCModulus(void);
extern  void            FCCharLen(void);
extern  void            FCImag(void);
extern  void            FCMax(void);
extern  void            FCMin(void);
extern  void            FCConjg(void);
extern  void            FCDProd(void);
extern  void            FCXProd(void);
extern  void            FCSign(void);
extern  void            FCBitTest(void);
extern  void            FCBitSet(void);
extern  void            FCBitClear(void);
extern  void            FCBitOr(void);
extern  void            FCBitAnd(void);
extern  void            FCBitNot(void);
extern  void            FCBitEquiv(void);
extern  void            FCBitExclOr(void);
extern  void            FCBitLShift(void);
extern  void            FCBitRShift(void);
extern  void            FCChar1Move(void);
extern  void            FCCharNMove(void);
extern  void            FCFieldOp(void);
extern  void            FCInpStruct(void);
extern  void            FCOutStruct(void);
extern  void            FCFieldSubscript(void);
extern  void            FCPrtStructArray(void);
extern  void            FCInpStructArray(void);
extern  void            FCSetNml(void);
extern  void            FCSetBlockSize(void);
extern  void            FCInqBlockSize(void);
extern  void            FCSetShare(void);
extern  void            FCFieldSubstring(void);
extern  void            FCAllocated(void);
extern  void            FCMakeSCB(void);
extern  void            FCAllocate(void);
extern  void            FCDeAllocate(void);
extern  void            FCMod(void);
extern  void            FCAbs(void);
extern  void            FCASin(void);
extern  void            FCACos(void);
extern  void            FCATan(void);
extern  void            FCATan2(void);
extern  void            FCLog(void);
extern  void            FCLog10(void);
extern  void            FCCos(void);
extern  void            FCSin(void);
extern  void            FCTan(void);
extern  void            FCSinh(void);
extern  void            FCCosh(void);
extern  void            FCTanh(void);
extern  void            FCSqrt(void);
extern  void            FCExp(void);
extern  void            FCLoc(void);
extern  void            FCBitChange(void);
extern  void            FCSetLine(void);
extern  void            FCDoneParenExpr(void);
extern  void            FCStmtDone(void);
extern  void            FCVolatile(void);
extern  void            FCSetSCBLen(void);

extern  void            DtInpLOG1(void);
extern  void            DtInpLOG4(void);
extern  void            DtInpINT1(void);
extern  void            DtInpINT2(void);
extern  void            DtInpINT4(void);
extern  void            DtInpREAL(void);
extern  void            DtInpDBLE(void);
extern  void            DtInpXTND(void);
extern  void            DtInpCPLX(void);
extern  void            DtInpDBCX(void);
extern  void            DtInpXTCX(void);
extern  void            DtInpCHAR(void);
extern  void            DtEndDataStmt(void);
extern  void            DtEndVarSet(void);
extern  void            DtPushConst(void);
extern  void            DtSubscript(void);
extern  void            DtSubstring(void);
extern  void            DtInpArray(void);
extern  void            DtAdd(void);
extern  void            DtSub(void);
extern  void            DtMul(void);
extern  void            DtDiv(void);
extern  void            DtExp(void);
extern  void            DtUMinus(void);
extern  void            DtPush(void);
extern  void            DtDataDoLoop(void);
extern  void            DtFlip(void);
extern  void            DtUnaryMul(void);
extern  void            DtFieldOp(void);
extern  void            DtFieldSubscript(void);
extern  void            DtInpStruct(void);
extern  void            DtInpStructArray(void);
extern  void            DtFieldSubstring(void);
extern  void            DtPushSCBLen(void);


void    (* __FAR FCJmpTab[])(void) = {
    #define pick(id,code_proc,data_proc) code_proc,
    #include "fcdefn.h"
    #undef pick
};

void    (* __FAR DataJmpTab[])(void) = {
    #define pick(id,code_proc,data_proc) data_proc,
    #include "fcdefn.h"
    #undef pick
};

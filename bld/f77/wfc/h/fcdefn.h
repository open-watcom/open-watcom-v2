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
* Description:  Define data for compiler F-Codes
*
****************************************************************************/


//       id                     code_proc                  data_proc
pick( FC_PAUSE,                 &FCPause,                  NULL )
pick( FC_STOP,                  &FCStop,                   NULL )
pick( FC_SET_IOCB,              &FCSetIOCB,                NULL )
pick( FC_SET_FMT,               &FCSetFmt,                 NULL )
pick( FC_SET_UNIT,              &FCSetUnit,                NULL )
pick( FC_SET_INTL,              &FCSetIntl,                NULL )
pick( FC_SET_ERR,               &FCSetErr,                 NULL )
pick( FC_SET_END,               &FCSetEnd,                 NULL )
pick( FC_SET_REC,               &FCSetRec,                 NULL )
pick( FC_SET_IOS,               &FCSetIOS,                 NULL )
pick( FC_SET_ACC,               &FCSetAcc,                 NULL )
pick( FC_SET_BLNK,              &FCSetBlnk,                NULL )
pick( FC_SET_FILE,              &FCSetFile,                NULL )
pick( FC_SET_FORM,              &FCSetForm,                NULL )
pick( FC_SET_LEN,               &FCSetLen,                 NULL )
pick( FC_SET_STAT,              &FCSetStat,                NULL )
pick( FC_SET_DIR,               &FCSetDir,                 NULL )
pick( FC_SET_CCTRL,             &FCSetCCtrl,               NULL )
pick( FC_SET_FMTD,              &FCSetFmtd,                NULL )
pick( FC_SET_NAME,              &FCSetName,                NULL )
pick( FC_SET_SEQ,               &FCSetSeq,                 NULL )
pick( FC_SET_UFMTD,             &FCSetUFmtd,               NULL )
pick( FC_SET_EXST,              &FCSetExst,                NULL )
pick( FC_SET_NMD,               &FCSetNmd,                 NULL )
pick( FC_SET_NREC,              &FCSetNRec,                NULL )
pick( FC_SET_NUMB,              &FCSetNumb,                NULL )
pick( FC_SET_OPEN,              &FCSetOpen,                NULL )
pick( FC_SET_RECL,              &FCSetRecl,                NULL )
pick( FC_SET_NOFMT,             &FCSetNoFmt,               NULL )
pick( FC_SET_RECTYPE,           &FCSetRecType,             NULL )
pick( FC_SET_ACTION,            &FCSetAction,              NULL )
pick( FC_SET_NML,               &FCSetNml,                 NULL )
pick( FC_SET_BLOCKSIZE,         &FCSetBlockSize,           NULL )
pick( FC_INQ_BLOCKSIZE,         &FCInqBlockSize,           NULL )
pick( FC_SET_SHARE,             &FCSetShare,               NULL )
pick( FC_EX_READ,               &FCExRead,                 NULL )
pick( FC_EX_WRITE,              &FCExWrite,                NULL )
pick( FC_EX_OPEN,               &FCExOpen,                 NULL )
pick( FC_EX_CLOSE,              &FCExClose,                NULL )
pick( FC_EX_BACK,               &FCExBack,                 NULL )
pick( FC_EX_ENDF,               &FCExEndf,                 NULL )
pick( FC_EX_REW,                &FCExRew,                  NULL )
pick( FC_EX_INQ,                &FCExInq,                  NULL )
pick( FC_OUT_LOG1,              &FCOutLOG1,                NULL )
pick( FC_OUT_LOG4,              &FCOutLOG4,                NULL )
pick( FC_OUT_INT1,              &FCOutINT1,                NULL )
pick( FC_OUT_INT2,              &FCOutINT2,                NULL )
pick( FC_OUT_INT4,              &FCOutINT4,                NULL )
pick( FC_OUT_REAL,              &FCOutREAL,                NULL )
pick( FC_OUT_DBLE,              &FCOutDBLE,                NULL )
pick( FC_OUT_XTND,              &FCOutXTND,                NULL )
pick( FC_OUT_CPLX,              &FCOutCPLX,                NULL )
pick( FC_OUT_DBCX,              &FCOutDBCX,                NULL )
pick( FC_OUT_XTCX,              &FCOutXTCX,                NULL )
pick( FC_OUT_CHAR,              &FCOutCHAR,                NULL )
pick( FC_OUT_STR,               NULL,                      NULL )
pick( FC_INP_LOG1,              &FCInpLOG1,                &DtInpLOG1 )
pick( FC_INP_LOG4,              &FCInpLOG4,                &DtInpLOG4 )
pick( FC_INP_INT1,              &FCInpINT1,                &DtInpINT1 )
pick( FC_INP_INT2,              &FCInpINT2,                &DtInpINT2 )
pick( FC_INP_INT4,              &FCInpINT4,                &DtInpINT4 )
pick( FC_INP_REAL,              &FCInpREAL,                &DtInpREAL )
pick( FC_INP_DBLE,              &FCInpDBLE,                &DtInpDBLE )
pick( FC_INP_XTND,              &FCInpXTND,                &DtInpXTND )
pick( FC_INP_CPLX,              &FCInpCPLX,                &DtInpCPLX )
pick( FC_INP_DBCX,              &FCInpDBCX,                &DtInpDBCX )
pick( FC_INP_XTCX,              &FCInpXTCX,                &DtInpXTCX )
pick( FC_INP_CHAR,              &FCInpCHAR,                &DtInpCHAR )
pick( FC_INP_STR,               NULL,                      NULL )
pick( FC_ENDIO,                 &FCEndIO,                  &DtEndDataStmt )
pick( FC_ALLOCATE,              &FCAllocate,               NULL )
pick( FC_DEALLOCATE,            &FCDeAllocate,             NULL )
pick( FC_FMT_SCAN,              &FCFmtScan,                NULL )
pick( FC_FMT_ARR_SCAN,          &FCFmtArrScan,             NULL )
pick( FC_CAT,                   &FCCat,                    NULL )
pick( FC_SUBSTRING,             &FCSubString,              &DtSubstring )
pick( FC_SUBSCRIPT,             &FCSubscript,              &DtSubscript )
pick( FC_PROLOGUE,              &FCPrologue,               NULL )
pick( FC_EPILOGUE,              &FCEpilogue,               NULL )
pick( FC_FMT_ASSIGN,            &FCFmtAssign,              NULL )
pick( FC_SET_LINE,              &FCSetLine,                NULL )
pick( FC_SET_MODULE,            NULL,                      NULL )

// Define "end of F-Code sequence" F-Code:

pick( FC_END_OF_SEQUENCE,       NULL,                      NULL )

// Define binary operators:     +,-,*,/,**

pick( FC_ADD,                   &FCAdd,                    &DtAdd )
pick( FC_SUB,                   &FCSub,                    &DtSub )
pick( FC_MUL,                   &FCMul,                    &DtMul )
pick( FC_DIV,                   &FCDiv,                    &DtDiv )
pick( FC_EXP,                   &FCPow,                    &DtExp )
pick( FC_FLIP,                  &FCFlip,                   &DtFlip )

// Define unary minus.

pick( FC_UMINUS,                &FCUMinus,                 &DtUMinus )

// Define F-Code to push operands.

pick( FC_PUSH,                  &FCPush,                   &DtPush )

// Define F-Code to pop operands.

pick( FC_POP,                   &FCPop,                    NULL )

// Define F-Code to terminate an expression.

pick( FC_EXPR_DONE,             &FCDone,                   NULL )

// Define F-Codes for subprogram processing.

pick( FC_SELECT,                &FCSelect,                 NULL )
pick( FC_PUSH_LIT,              &FCPushLit,                NULL )

// Define logical operators:     .EQV.,.NEQV.,.OR.,.AND.,.NOT.

pick( FC_EQV,                   &FCEqv,                    NULL )
pick( FC_NEQV,                  &FCNEqv,                   NULL )
pick( FC_OR,                    &FCOr,                     NULL )
pick( FC_AND,                   &FCAnd,                    NULL )
pick( FC_NOT,                   &FCNot,                    NULL )

// Define F-Code to push constants.

pick( FC_PUSH_CONST,            &FCPushConst,              &DtPushConst )

// Define comparison operators:  .EQ.,.NE.,.LT.,.GE.,.LE.,.GT.

pick( FC_CMP_EQ,                &FCCmpEQ,                  NULL )
pick( FC_CMP_NE,                &FCCmpNE,                  NULL )
pick( FC_CMP_LT,                &FCCmpLT,                  NULL )
pick( FC_CMP_GE,                &FCCmpGE,                  NULL )
pick( FC_CMP_LE,                &FCCmpLE,                  NULL )
pick( FC_CMP_GT,                &FCCmpGT,                  NULL )

// Define control flow F-Codes:

pick( FC_WARP_RETURN,           &FCWarpReturn,             NULL )
pick( FC_JMP_ALWAYS,            &FCJmpAlways,              NULL )
pick( FC_JMP_FALSE,             &FCJmpFalse,               NULL )
pick( FC_DEFINE_LABEL,          &FCDefineLabel,            NULL )
pick( FC_FCODE_SEEK,            &FCSeek,                   NULL )
pick( FC_ASSIGN,                &FCAssign,                 NULL )
pick( FC_IF_ARITH,              &FCIfArith,                NULL )
pick( FC_EXECUTE,               &FCExecute,                NULL )
pick( FC_END_RB,                &FCEndRB,                  NULL )
pick( FC_SF_CALL,               &FCSFCall,                 NULL )

// Define "initialize dummy argument" F-Code:

pick( FC_DARG_INIT,             &FCDArgInit,               NULL )

// Define debugging information F-Codes:

pick( FC_DBG_LINE,              &FCDbgLine,                NULL )

// Define "call" F-Code:

pick( FC_CALL,                  &FCCall,                   NULL )

// Define "define statment number" F-Code

pick( FC_STMT_DEFINE_LABEL,     &FCStmtDefineLabel,        NULL )

// Define "convert" F-Codes:

pick( FC_CONVERT,               &FCConvert,                NULL )
pick( FC_MAKE_COMPLEX,          &FCMakeComplex,            NULL )
pick( FC_MAKE_DCOMPLEX,         &FCMakeDComplex,           NULL )
pick( FC_MAKE_XCOMPLEX,         &FCMakeXComplex,           NULL )
pick( FC_START_SF,              &FCStartSF,                NULL )
pick( FC_END_SF,                &FCEndSF,                  NULL )

// Define "ADV fill" F-Codes:

pick( FC_ADV_FILL_LO,           &FCAdvFillLo,              NULL )
pick( FC_ADV_FILL_HI,           &FCAdvFillHi,              NULL )
pick( FC_ADV_FILL_HI_LO_1,      &FCAdvFillHiLo1,           NULL )

// Define "trash cg-name" F-Code:

pick( FC_TRASH,                 &FCTrash,                  NULL )

// Define "alternate return" F-Code:

pick( FC_ALT_RET,               &FCAltReturn,              NULL )

// Define "push length in SCB" F-Code:

pick( FC_PUSH_SCB_LEN,          &FCPushSCBLen,             &DtPushSCBLen )

// Define complex binary operators:

// a binary operators to get the complex/complex F-Code.

pick( FC_CCADD,                 &FCAddCmplx,               NULL )
pick( FC_CCSUB,                 &FCSubCmplx,               NULL )
pick( FC_CCMUL,                 &FCMulCmplx,               NULL )
pick( FC_CCDIV,                 &FCDivCmplx,               NULL )
pick( FC_CCEXP,                 &FCExpCmplx,               NULL )
pick( FC_CMPLX_FLIP,            &FCCmplxFlip,              NULL )

// a binary operators to get the complex/scalar F-Code.

pick( FC_CXADD,                 &FCAddMixCX,               NULL )
pick( FC_CXSUB,                 &FCSubMixCX,               NULL )
pick( FC_CXMUL,                 &FCMulMixCX,               NULL )
pick( FC_CXDIV,                 &FCDivMixCX,               NULL )
pick( FC_CXEXP,                 &FCExpMixCX,               NULL )
pick( FC_CX_FLIP,               &FCCXFlip,                 NULL )

//  a binary operators to get the scalar/complex F-Code.

pick( FC_XCADD,                 &FCAddMixXC,               NULL )
pick( FC_XCSUB,                 &FCSubMixXC,               NULL )
pick( FC_XCMUL,                 &FCMulMixXC,               NULL )
pick( FC_XCDIV,                 &FCDivMixXC,               NULL )
pick( FC_XCEXP,                 &FCExpMixXC,               NULL )
pick( FC_XC_FLIP,               &FCXCFlip,                 NULL )

// Define complex unary minus:

pick( FC_CUMINUS,               &FCUMinusCmplx,            NULL )

// Define F-Code to terminate a complex expression.

pick( FC_CMPLX_EXPR_DONE,       &FCCmplxDone,              NULL )

// Define F-Code to indicate a statement function was referenced.

pick( FC_SF_REFERENCED,         &FCSFReferenced,           NULL )

// Define F-Code to pass a label to a run-time routine.

pick( FC_PASS_LABEL,            &FCPassLabel,              NULL )

// Define DATA statement F-Codes:

pick( FC_START_DATA_STMT,       &FCStartDataStmt,          NULL )
pick( FC_END_VAR_SET,           NULL,                      &DtEndVarSet )

// Define "pass character array" F-Code:

pick( FC_PASS_CHAR_ARRAY,       &FCPassCharArray,          NULL )

// Define array i/o F-Codes:

pick( FC_PRT_ARRAY,             &FCPrtArray,               NULL )
pick( FC_INP_ARRAY,             &FCInpArray,               &DtInpArray )

// Define "branch to statement number" F-Code.

pick( FC_STMT_JMP_ALWAYS,       &FCStmtJmpAlways,          NULL )

// Define "set internal file to character array" F-Code:

pick( FC_ARR_SET_INTL,          &FCIntlArrSet,             NULL )

// Define "null" F-Code:

pick( FC_NULL_FCODE,            &FCNull,                   NULL )

// Define "implied-DO for DATA statements" F-Code:

pick( FC_DATA_DO_LOOP,          NULL,                      &DtDataDoLoop )

// Define "assign alternate return index" F-Code:

pick( FC_ASSIGN_ALT_RET,        &FCAssignAltRet,           NULL )

// Define "complex comparison" F-Codes:

pick( FC_CC_CMP_EQ,             &FCCCCmpEQ,                NULL )
pick( FC_CC_CMP_NE,             &FCCCCmpNE,                NULL )
pick( FC_XC_CMP_EQ,             &FCXCCmpEQ,                NULL )
pick( FC_XC_CMP_NE,             &FCXCCmpNE,                NULL )
pick( FC_CX_CMP_EQ,             &FCCXCmpEQ,                NULL )
pick( FC_CX_CMP_NE,             &FCCXCmpNE,                NULL )

// Define character comparison operators:  .EQ. .NE. .LT. .GE. .LE. .GT.

pick( FC_CHAR_CMP_EQ,           &FCCharCmpEQ,              NULL )
pick( FC_CHAR_CMP_NE,           &FCCharCmpNE,              NULL )
pick( FC_CHAR_CMP_LT,           &FCCharCmpLT,              NULL )
pick( FC_CHAR_CMP_GE,           &FCCharCmpGE,              NULL )
pick( FC_CHAR_CMP_LE,           &FCCharCmpLE,              NULL )
pick( FC_CHAR_CMP_GT,           &FCCharCmpGT,              NULL )

// Define "done with label" F-Code:

pick( FC_FREE_LABEL,            &FCFreeLabel,              NULL )

// Define "DO loop" F-Codes:

pick( FC_DO_BEGIN,              &FCDoBegin,                NULL )
pick( FC_DO_END,                &FCDoEnd,                  NULL )
pick( FC_COMPUTED_GOTO,         &FCComputedGOTO,           NULL )
pick( FC_START_RB,              &FCStartRB,                NULL )
pick( FC_WARP,                  &FCWarp,                   NULL )
pick( FC_SET_ATEND,             &FCSetAtEnd,               NULL )
pick( FC_ASSIGNED_GOTO_LIST,    &FCAssignedGOTOList,       NULL )
pick( FC_CHK_IO_STMT_LABEL,     &FCChkIOStmtLabel,         NULL )
pick( FC_UNARY_MUL,             &FCUnaryMul,               &DtUnaryMul )

// In-line intrinsic functions:

pick( FC_ICHAR,                 &FCIChar,                  NULL )
pick( FC_MODULUS,               &FCModulus,                NULL )
pick( FC_CHAR_LEN,              &FCCharLen,                NULL )
pick( FC_IMAG,                  &FCImag,                   NULL )
pick( FC_MAX,                   &FCMax,                    NULL )
pick( FC_MIN,                   &FCMin,                    NULL )
pick( FC_CONJG,                 &FCConjg,                  NULL )
pick( FC_DPROD,                 &FCDProd,                  NULL )
pick( FC_XPROD,                 &FCXProd,                  NULL )
pick( FC_SIGN,                  &FCSign,                   NULL )
pick( FC_BIT_TEST,              &FCBitTest,                NULL )
pick( FC_BIT_SET,               &FCBitSet,                 NULL )
pick( FC_BIT_CLEAR,             &FCBitClear,               NULL )
pick( FC_BIT_EQUIV,             &FCBitEquiv,               NULL )
pick( FC_BIT_EXCL_OR,           &FCBitExclOr,              NULL )
pick( FC_BIT_OR,                &FCBitOr,                  NULL )
pick( FC_BIT_AND,               &FCBitAnd,                 NULL )
pick( FC_BIT_NOT,               &FCBitNot,                 NULL )

// Define character*1 comparison operators:  .EQ. .NE. .LT. .GE. .LE. .GT.

pick( FC_CHAR_1_CMP_EQ,         &FCChar1CmpEQ,             NULL )
pick( FC_CHAR_1_CMP_NE,         &FCChar1CmpNE,             NULL )
pick( FC_CHAR_1_CMP_LT,         &FCChar1CmpLT,             NULL )
pick( FC_CHAR_1_CMP_GE,         &FCChar1CmpGE,             NULL )
pick( FC_CHAR_1_CMP_LE,         &FCChar1CmpLE,             NULL )
pick( FC_CHAR_1_CMP_GT,         &FCChar1CmpGT,             NULL )

// Define single character move:

pick( FC_CHAR_1_MOVE,           &FCChar1Move,              NULL )

// Field Selections operator:

pick( FC_FIELD_OP,              &FCFieldOp,                &DtFieldOp )

// STRUCTURE i/o F-Codes:

pick( FC_INPUT_STRUCT,          &FCInpStruct,              &DtInpStruct )
pick( FC_OUTPUT_STRUCT,         &FCOutStruct,              NULL )

// Field subcript operator:

pick( FC_FIELD_SUBSCRIPT,       &FCFieldSubscript,         &DtFieldSubscript )

// STRUCTURE array i/o F-Codes:

pick( FC_STRUCT_PRT_ARRAY,      &FCPrtStructArray,         NULL )
pick( FC_STRUCT_INP_ARRAY,      &FCInpStructArray,         &DtInpStructArray )

// Field substring operator:

pick( FC_FIELD_SUBSTRING,       &FCFieldSubstring,         &DtFieldSubstring )
pick( FC_MAKE_SCB,              &FCMakeSCB,                NULL )

// ALLOCATED intrinsic function:

pick( FC_ALLOCATED,             &FCAllocated,              NULL )

// Math intrinsic functions:

pick( FC_MATH_MOD,              &FCMod,                    NULL )
pick( FC_MATH_ABS,              &FCAbs,                    NULL )
pick( FC_MATH_ASIN,             &FCASin,                   NULL )
pick( FC_MATH_ACOS,             &FCACos,                   NULL )
pick( FC_MATH_ATAN,             &FCATan,                   NULL )
pick( FC_MATH_ATAN2,            &FCATan2,                  NULL )
pick( FC_MATH_LOG,              &FCLog,                    NULL )
pick( FC_MATH_LOG10,            &FCLog10,                  NULL )
pick( FC_MATH_COS,              &FCCos,                    NULL )
pick( FC_MATH_SIN,              &FCSin,                    NULL )
pick( FC_MATH_TAN,              &FCTan,                    NULL )
pick( FC_MATH_SINH,             &FCSinh,                   NULL )
pick( FC_MATH_COSH,             &FCCosh,                   NULL )
pick( FC_MATH_TANH,             &FCTanh,                   NULL )
pick( FC_MATH_SQRT,             &FCSqrt,                   NULL )
pick( FC_MATH_EXP,              &FCExp,                    NULL )

// LOC intrinsic function:

pick( FC_LOC,                   &FCLoc,                    NULL )

// Change-Bit intrinsic function:

pick( FC_BIT_CHANGE,            &FCBitChange,              NULL )

// Done parenthesized expression:

pick( FC_DONE_PAREN_EXPR,       &FCDoneParenExpr,          NULL )

// Done compiling statement:

pick( FC_STMT_DONE,             &FCStmtDone,               NULL )

// Make reference to symbol volatile:

pick( FC_VOLATILE,              &FCVolatile,               NULL )

// Pass character array that is a field within a structure as an argument:

pick( FC_PASS_FIELD_CHAR_ARRAY, &FCPassFieldCharArray,     NULL )

// Set SCB for dynamic allocation

pick( FC_SET_SCB_LEN,           &FCSetSCBLen,              NULL )

// LSHIFT and RSHIFT opcodes,

pick( FC_LEFT_SHIFT,            &FCBitLShift,              NULL )
pick( FC_RIGHT_SHIFT,           &FCBitRShift,              NULL )

// Define an equal character move of non optimal length strings:

pick( FC_CHAR_N_MOVE,           &FCCharNMove,              NULL )

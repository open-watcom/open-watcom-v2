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
* Description:  Instruction opcode/type matrix to select generate tables.
*
****************************************************************************/


#include "standard.h"
#include "typclass.h"
#include "opcodes.h"
#include "tables.h"

#define         _____   NO
#define         __X__   BAD


table_def OpTable[(XX+1)*(LAST_OP-FIRST_OP+1)] = {
/*************************************************
    for each opcode/type, indicate which generate table to use.
    See i86table.c
*/
/*U1   I1    U2    I2    U4    I4    U8    I8    CP    PT    FS     FD    FL    XX*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/* OP_NOP*/
ADD1 ,ADD1 ,ADD2 ,ADD2 ,ADD4 ,ADD4 ,ADD8 ,ADD8 ,ADDCP,ADDPT,RTN4FC,RTN8C,RTN8C,__X__,/* OP_ADD*/
__X__,__X__,EADD ,EADD ,__X__,__X__,__X__,__X__,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_EXT_ADD*/
SUB1, SUB1 ,SUB2 ,SUB2 ,SUB4 ,SUB4 ,SUB8 ,SUB8 ,SUBCP,SUBPT,RTN4F ,RTN8 ,RTN8 ,__X__,/* OP_SUB*/
__X__,__X__,ESUB ,ESUB ,__X__,__X__,__X__,__X__,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_EXT_SUB*/
MUL1 ,MUL1 ,MUL2 ,MUL2 ,RTN4C,RTN4C,SUPP8,SUPP8,__X__,__X__,RTN4FC,RTN8C,RTN8C,__X__,/* OP_MUL*/
__X__,__X__,EMUL ,EMUL ,EMUL ,EMUL ,__X__,__X__,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_EXT_MUL*/
DIV1 ,DIV1 ,DIV2 ,DIV2 ,RTN4 ,RTN4 ,SUPP8,SUPP8,__X__,__X__,RTN4F ,RTN8 ,RTN8 ,__X__,/* OP_DIV*/
MOD1 ,MOD1 ,MOD2 ,MOD2 ,RTN4 ,RTN4 ,SUPP8,SUPP8,__X__,__X__,RTN4F ,RTN8 ,RTN8 ,__X__,/* OP_MOD*/
AND1 ,AND1 ,AND2 ,AND2 ,AND4 ,AND4 ,LOG8 ,LOG8 ,BITCP,__X__,__X__ ,__X__,__X__,__X__,/* OP_AND*/
OR1  ,OR1  ,OR2  ,OR2  ,OR4  ,OR4  ,LOG8 ,LOG8 ,BITCP,__X__,__X__ ,__X__,__X__,__X__,/* OP_OR*/
OR1  ,OR1  ,OR2  ,OR2  ,OR4  ,OR4  ,LOG8 ,LOG8 ,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_XOR*/
SHFT1,SHFT1,SHFT2,SHFT2,SHFT4,SHFT4,SUPP8,SUPP8,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_RSHIFT*/
SHFT1,SHFT1,SHFT2,SHFT2,SHFT4,SHFT4,SUPP8,SUPP8,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_LSHIFT*/
__X__,__X__,__X__,__X__,__X__,RTN4 ,__X__,SUPP8,__X__,__X__,BFUNS ,BFUND,BFUND,__X__,/* OP_POW*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,BFUNS ,BFUND,BFUND,__X__,/* OP_P5DIV*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,BFUNS ,BFUND,BFUND,__X__,/* OP_ATAN2*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,BFUNS ,BFUND,BFUND,__X__,/* OP_FMOD*/
NEG1 ,NEG1 ,NEG2 ,NEG2 ,NEG4 ,NEG4 ,NEG8 ,NEG8 ,__X__,__X__,NEGF  ,NEGF ,NEGF ,__X__,/* OP_NEGATE*/
NOT1 ,NOT1 ,NOT2 ,NOT2 ,NOT4 ,NOT4 ,NOT8 ,NOT8 ,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_COMPLEMEN*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_LOG*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_COS*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_SIN*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_TAN*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_SQRT*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_FABS*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_ACOS */
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_ASIN */
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_ATAN */
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_COSH */
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_EXP */
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_LOG10 */
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_SINH */
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,UFUNS ,UFUND,UFUND,__X__,/* OP_TANH */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/* OP_PTR_TO_NATIVE */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/* OP_PTR_TO_FOREIGN */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/* OP_SLACK*/
CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT   ,CVT  ,CVT  ,__X__,/* OP_CONVERT*/
__X__,__X__,LA   ,LA   ,LA4  ,LA4  ,LA4  ,LA4  ,LA4  ,LA4  ,__X__ ,__X__,__X__,__X__,/* OP_LA*/
__X__,__X__,LA   ,LA   ,LA4  ,LA4  ,LA4  ,LA4  ,LA4  ,LA4  ,__X__ ,__X__,__X__,__X__,/* OP_CAREFUL_LA*/
CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,__X__,__X__,CVT   ,CVT  ,CVT  ,__X__,/* OP_ROUND*/
MOV1 ,MOV1 ,MOV2 ,MOV2 ,MOV4 ,MOV4 ,MOV8 ,MOV8 ,MOV4 ,MOV4 ,MOVFS ,MOVFD,MOVFD,MOVX ,/* OP_MOV*/
CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI ,CALLI,CALLI,CALLI,/* OP_CALL_INDI*/
PUSH1,PUSH1,PUSH2,PUSH2,PUSH4,PUSH4,PUSH8,PUSH8,PUSH4,PUSH4,PSHFS ,PSHFD,PSHFD,PUSHX,/* OP_PUSH*/
__X__,__X__,POP2, POP2,__X__,__X__,__X__,__X__, __X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_POP*/
DPARM,DPARM,DPARM,DPARM,DPARM,DPARM,DPARM,DPARM,DPARM,DPARM,DPARM ,DPARM,DPARM,DPARM,/* OP_PARM_DEF*/
SJUMP,SJUMP,SJUMP,SJUMP,SJUMP,SJUMP,SJUMP,SJUMP,SJUMP,SJUMP,__X__ ,__X__,__X__,__X__,/* OP_SELECT*/
TEST1,TEST1,TEST2,TEST2,TEST4,TEST4,TEST8,TEST8,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_BIT_TEST_*/
TEST1,TEST1,TEST2,TEST2,TEST4,TEST4,TEST8,TEST8,__X__,__X__,__X__ ,__X__,__X__,__X__,/* OP_BIT_TEST_*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,CMPPT,CMPF  ,CMPD ,CMPD ,__X__,/* OP_CMP_EQUAL*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,CMPPT,CMPF  ,CMPD ,CMPD ,__X__,/* OP_CMP_NOT_E*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,CMPPT,CMPF  ,CMPD ,CMPD ,__X__,/* OP_CMP_GREAT*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,CMPPT,CMPF  ,CMPD ,CMPD ,__X__,/* OP_CMP_LESS_*/
CMP1 ,CMP1 ,CMP2C,CMP2C,CMP4C,CMP4C,CMP8 ,CMP8 ,CMPCP,CMPPT,CMPF  ,CMPD ,CMPD ,__X__,/* OP_CMP_LESS*/
CMP1 ,CMP1 ,CMP2C,CMP2C,CMP4C,CMP4C,CMP8 ,CMP8 ,CMPCP,CMPPT,CMPF  ,CMPD ,CMPD ,__X__,/* OP_CMP_GREAT*/
CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL  ,CALL ,CALL ,CALL ,/* OP_CALL*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,__X__,CMPF ,CMPD , CMPL,   __X__,/*OP_SET_EQ*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,__X__,CMPF ,CMPD , CMPL,   __X__,/*OP_SET_NE*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,__X__,CMPF ,CMPD , CMPL,   __X__,/*OP_SET_GT*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,__X__,CMPF ,CMPD , CMPL,   __X__,/*OP_SET_LT*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,__X__,CMPF ,CMPD , CMPL,   __X__,/*OP_SET_LE*/
CMP1 ,CMP1 ,CMP2 ,CMP2 ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,CMPCP,__X__,CMPF ,CMPD , CMPL,   __X__,/*OP_SET_GE*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____, _____,  _____,/*OP_DEBUG_INFO*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____, _____,  _____,/*OP_CHEAP_NOP*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____ ,_____,_____,_____
};

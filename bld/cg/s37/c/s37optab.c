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


#include "standard.h"
#include "typclass.h"
#include "opcodes.h"
#include "tables.h"
#include "far.h"

#define         _____   NO
#define         __X__   BAD


table_def _FAR OpTable[(XX+1)*(LAST_OP-FIRST_OP+1)] = {
/******************************************************
    for each opcode/type, indicate which generate table to use.
    See S37TABLE.C
*/

/*U1   I1    U2    I2    U4    I4    CP    PT    FS    FD    XX*/
_____,_____,_____,_____,_____,_____,__X__,__X__,_____,_____,_____,/*OP_NOP*/
ADD1 ,ADD1 ,ADD2 ,ADD2 ,ADDU4,ADDI4,__X__,ADDPT,ADDFS,ADDFD,__X__,/*OP_ADD*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,/*OP_EXT_ADD*/
SUB1 ,SUB1 ,SUB2 ,SUB2 ,SUBU4,SUBI4,__X__,SUBPT,SUBFS,SUBFD,__X__,/*OP_SUB*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,/*OP_EXT_SUB*/
MUL1 ,MUL1 ,MUL2 ,MUL2 ,MUL4 ,MUL4 ,__X__,__X__,MULFS,MULFD,__X__,/*OP_MUL*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,/*OP_EXT_MUL*/
DIV1 ,DIV1 ,DIV2 ,DIV2 ,DIV4 ,DIV4 ,__X__,__X__,DIVFS,DIVFD,__X__,/*OP_DIV*/
MOD1 ,MOD1 ,MOD2 ,MOD2 ,MOD4 ,MOD4 ,__X__,__X__,__X__,__X__,__X__,/*OP_MOD*/
AND1 ,AND1 ,AND2 ,AND2 ,AND4 ,AND4 ,__X__,__X__,__X__,__X__,__X__,/*OP_AND*/
OR1  ,OR1  ,OR2  ,OR2  ,OR4  ,OR4  ,__X__,__X__,__X__,__X__,__X__,/*OP_OR*/
XOR1 ,XOR1 ,XOR2 ,XOR2 ,XOR4 ,XOR4 ,__X__,__X__,__X__,__X__,__X__,/*OP_XOR*/
SHFT1,SHFT1,SHFT2,SHFT2,RSHU4,RSHI4,__X__,__X__,__X__,__X__,__X__,/*OP_RSHIFT*/
SHFT1,SHFT1,SHFT2,SHFT2,LSHU4,LSHI4,__X__,__X__,__X__,__X__,__X__,/*OP_LSHIFT*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_POW*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_ATAN2*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_FMOD*/
NEG1 ,NEG1 ,NEG2 ,NEG2 ,NEG4 ,NEG4 ,__X__,__X__,NEGFS,NEGFD,__X__,/*OP_NEGATE*/
NOT1 ,NOT1 ,NOT2 ,NOT2 ,NOT4 ,NOT4 ,__X__,__X__,__X__,__X__,__X__,/*OP_COMPL*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_LOG*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_COS*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SIN*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_TAN*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SQRT*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_FABS*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_ACOS */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_ASIN */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_ATAN */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_COSH */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_EXP */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_LOG10 */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SINH */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_TANH */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_PTR_TO_NATIVE */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_PTR_TO_FORIEGN */
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,__X__,CVT  ,CVT  ,CVT  ,CVT  ,/*OP_CONVERT*/
CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,CVT  ,__X__,CVT  ,CVT  ,CVT  ,__X__,/*OP_ROUND*/
MOV1 ,MOV1 ,MOV2 ,MOV2 ,MOV4 ,MOV4 ,__X__,MOVPT,MOVFS,MOVFD,MOVX ,/*OP_MOV*/
__X__,__X__,__X__,__X__,LA   ,LA   ,__X__,LAPT ,__X__,__X__,__X__,/*P_CARFL_LA*/
__X__,__X__,__X__,__X__,LA   ,LA   ,__X__,LAPT ,__X__,__X__,__X__,/*OP_LA*/
CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,__X__,CALLI,CALLI,CALLI,CALLI,/*OP_CALL_I*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,/*OP_PUSH*/
__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,__X__,/*OP_POP*/
DPARM,DPARM,DPARM,DPARM,DPARM,DPARM,__X__,DPARM,DPARM,DPARM,DPARM,/*OP_PRM_DEF*/
SJUMP,SJUMP,SJUMP,SJUMP,SJUMP,SJUMP,__X__,__X__,__X__,__X__,__X__,/*OP_SELECT*/
TEST1,TEST1,TEST2,TEST2,TEST4,TEST4,__X__,TSTPT,__X__,__X__,__X__,/*OP_B_TST_*/
TEST1,TEST1,TEST2,TEST2,TEST4,TEST4,__X__,TSTPT,__X__,__X__,__X__,/*OP_B_TST_*/
CMPU1,CMPI1,CMPU2,CMPI2,CMPU4,CMPI4,__X__,CMPPT,CMPFS,CMPFD,__X__,/*OP_CMP_EQ*/
CMPU1,CMPI1,CMPU2,CMPI2,CMPU4,CMPI4,__X__,CMPPT,CMPFS,CMPFD,__X__,/*OP_CMP_NE*/
CMPU1,CMPI1,CMPU2,CMPI2,CMPU4,CMPI4,__X__,CMPPT,CMPFS,CMPFD,__X__,/*OP_CMP_GT*/
CMPU1,CMPI1,CMPU2,CMPI2,CMPU4,CMPI4,__X__,CMPPT,CMPFS,CMPFD,__X__,/*OP_CMP_LT*/
CMPU1,CMPI1,CMPU2,CMPI2,CMPU4,CMPI4,__X__,CMPPT,CMPFS,CMPFD,__X__,/*OP_CMP_LE*/
CMPU1,CMPI1,CMPU2,CMPI2,CMPU4,CMPI4,__X__,CMPPT,CMPFS,CMPFD,__X__,/*OP_CMP_GE*/
CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,__X__,CALL ,CALL ,CALL ,CALL ,/*OP_CALL*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____
};

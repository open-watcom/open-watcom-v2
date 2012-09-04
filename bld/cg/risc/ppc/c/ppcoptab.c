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
#include "tables.h"

#define         _____   NO
#define         __X__   BAD
#define         _NYI_   NYI


table_def OpTable[] = {
/**********************
    for each opcode/type, indicate which generate table to use.
    See ppctable.c
*/
/*U1   I1    U2    I2    U4    I4    U8    I8    CP    PT    FS    FD    FL    XX*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,__X__,_____,_____,_____,_____,/*OP_NOP*/
BIN1 ,BIN1 ,BIN2 ,BIN2 ,BIN4 ,BIN4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,FBINS,FBIND,FBIND,_NYI_,/*OP_ADD*/
BIN1 ,BIN1 ,BIN2 ,BIN2 ,BIN4 ,BIN4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,FBINS,FBIND,FBIND,_NYI_,/*OP_EXT_ADD*/
BIN1 ,BIN1 ,BIN2 ,BIN2 ,BIN4 ,BIN4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,FBINS,FBIND,FBIND,_NYI_,/*OP_SUB*/
BIN1 ,BIN1 ,BIN2 ,BIN2 ,BIN4 ,BIN4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,FBINS,FBIND,FBIND,_NYI_,/*OP_EXT_SUB*/
BIN1 ,BIN1 ,BIN2 ,BIN2 ,BIN4 ,BIN4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,FBINS,FBIND,FBIND,_NYI_,/*OP_MUL*/
BIN1 ,BIN1 ,BIN2 ,BIN2 ,BIN4 ,BIN4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,FBINS,FBIND,FBIND,_NYI_,/*OP_EXT_MUL*/
PROM ,PROM ,PROM ,PROM ,NBIN4,NBIN4,BIN8 ,BIN8 ,_NYI_,_NYI_,FBINS,FBIND,FBIND,_NYI_,/*OP_DIV*/
PROM ,PROM ,PROM ,PROM ,MOD4 ,MOD4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_MOD*/
UBIN1,UBIN1,UBIN2,UBIN2,UBIN4,UBIN4,BIN8 ,BIN8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_AND*/
UBIN1,UBIN1,UBIN2,UBIN2,UBIN4,UBIN4,BIN8 ,BIN8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_OR*/
UBIN1,UBIN1,UBIN2,UBIN2,UBIN4,UBIN4,BIN8 ,BIN8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_XOR*/
PROM ,PROM ,PROM ,PROM ,BIN4 ,BIN4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_RSHIFT*/
BIN1 ,BIN1 ,BIN2 ,BIN2 ,BIN4 ,BIN4 ,BIN8 ,BIN8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_LSHIFT*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_POW*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_P5DIV*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_ATAN2*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_FMOD*/
UN1  ,UN1  ,UN2  ,UN2  ,UN4  ,UN4  ,UN8  ,UN8  ,_NYI_,_NYI_,NEGF ,NEGF ,NEGF ,_NYI_,/*OP_NEGATE*/
UN1  ,UN1  ,UN2  ,UN2  ,UN4  ,UN4  ,UN8  ,UN8  ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_COMPL*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_LOG*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_COS*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SIN*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_TAN*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SQRT*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_FABS*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_ACOS */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_ASIN */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_ATAN */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_COSH */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_EXP */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_LOG10 */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SINH */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_TANH*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_PTR_TO_NATIVE */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_PTR_TO_FOREIGN */
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SLACK*/
CONV ,CONV ,CONV ,CONV ,CONV ,CONV ,CONV ,CONV ,CONV ,CONV ,FCONV,FCONV,FCONV,FCONV,/*OP_CONVERT*/
_NYI_,_NYI_,LA2  ,LA2  ,LA4  ,LA4  ,LA8  ,LA8  ,LA4  ,LA4  ,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_LA*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_CARFL_LA*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_ROUND*/
MOV1 ,MOV1 ,MOV2 ,MOV2 ,MOV4 ,MOV4 ,MOV8 ,MOV8 ,MOV4 ,MOV4 ,MOVFS,MOVFD,MOVFD,MOVXX,/*OP_MOV*/
CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,CALLI,/*OP_CALL_I*/
PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,PUSH ,/*OP_PUSH*/
POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,POP  ,/*OP_POP*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_PRM_DEF*/
_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SELECT*/
PROM ,PROM ,PROM ,PROM ,TEST4,TEST4,TEST8,TEST8,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_B_TST_T*/
PROM ,PROM ,PROM ,PROM ,TEST4,TEST4,TEST8,TEST8,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_B_TST_F*/
PROM ,PROM ,PROM ,PROM ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,_NYI_,_NYI_,CMPFS,CMPFD,CMPFD,_NYI_,/*OP_CMP_EQ*/
PROM ,PROM ,PROM ,PROM ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,_NYI_,_NYI_,CMPFS,CMPFD,CMPFD,_NYI_,/*OP_CMP_NE*/
PROM ,PROM ,PROM ,PROM ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,_NYI_,_NYI_,CMPFS,CMPFD,CMPFD,_NYI_,/*OP_CMP_GT*/
PROM ,PROM ,PROM ,PROM ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,_NYI_,_NYI_,CMPFS,CMPFD,CMPFD,_NYI_,/*OP_CMP_LT*/
PROM ,PROM ,PROM ,PROM ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,_NYI_,_NYI_,CMPFS,CMPFD,CMPFD,_NYI_,/*OP_CMP_LE*/
PROM ,PROM ,PROM ,PROM ,CMP4 ,CMP4 ,CMP8 ,CMP8 ,_NYI_,_NYI_,CMPFS,CMPFD,CMPFD,_NYI_,/*OP_CMP_GE*/
CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,CALL ,/*OP_CALL*/
PROM ,PROM ,PROM ,PROM ,SET4 ,SET4 ,SET8 ,SET8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SET_EQ*/
PROM ,PROM ,PROM ,PROM ,SET4 ,SET4 ,SET8 ,SET8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SET_NE*/
PROM ,PROM ,PROM ,PROM ,SET4 ,SET4 ,SET8 ,SET8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SET_GT*/
PROM ,PROM ,PROM ,PROM ,SET4 ,SET4 ,SET8 ,SET8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SET_LT*/
PROM ,PROM ,PROM ,PROM ,SET4 ,SET4 ,SET8 ,SET8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SET_LE*/
PROM ,PROM ,PROM ,PROM ,SET4 ,SET4 ,SET8 ,SET8 ,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,_NYI_,/*OP_SET_GE*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_DEBUG_INFO*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_CHEAP_NOP*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_LOAD_UNALIGNED*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_STORE_UNALIGNED*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_EXTRACT_LOW*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_EXTRACT_HIGH*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_INSERT_LOW*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_INSERT_HIGH*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_MASK_LOW*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_MASK_HIGH*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_ZAP*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_ZAP_NOT*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_STK_ALLOC*/
_____,_____,_____,_____,VASTR,VASTR,_____,_____,VASTR,VASTR,_____,_____,_____,_____,/*OP_VA_START*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_SLACK*/
_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,_____,/*OP_BLOCK*/
};

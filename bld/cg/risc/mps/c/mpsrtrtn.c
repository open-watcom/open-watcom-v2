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
* Description:  MIPS runtime support routine calls.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "regset.h"
#include "rtclass.h"
#include "rttable.h"
#include "zoiks.h"

extern    int   RoutineNum;

#define RTSIZE (RT_NOP - BEG_RTNS + 1)           /* includes NOP*/

#define OP_CMP    OP_CMP_EQUAL

rtn_info RTInfo[] = {
/* name                 op              class   left      right  result*/
"_WtcI8D",              OP_DIV,         I8,     RL_PARM_8,RL_PARM2_8,RL_RET_8,
"_WtcU8D",              OP_DIV,         U8,     RL_PARM_8,RL_PARM2_8,RL_RET_8,
"_WtcI8R",              OP_MOD,         I8,     RL_PARM_8,RL_PARM2_8,RL_RET_8,
"_WtcU8R",              OP_MOD,         U8,     RL_PARM_8,RL_PARM2_8,RL_RET_8,
"_WtcMemCopy",          OP_MOV,         XX,     RL_PARM_4,RL_PARM2_4,RL_RET_4,
"_WtcStkCrawl",         OP_STK_ALLOC,   XX,     RL_,RL_,RL_,
"_WtcStkCrawlSize",     OP_STK_ALLOC,   XX,     RL_,RL_,RL_,
"_WtcStkScribble",      OP_CALL,        XX,     RL_,RL_,RL_,
"_WtcStkCopy",          OP_CALL,        XX,     RL_,RL_,RL_,
"__ExceptionFilter",    OP_CALL,        XX,     RL_,RL_,RL_,
"_IF_dpow",             OP_POW,         FD,     RL_PARM_F,RL_PARM2_F,RL_RET_F,
"_IF_pow",              OP_POW,         FS,     RL_PARM_F,RL_PARM2_F,RL_RET_F,
"_IF_powii",            OP_POW,         I4,     RL_PARM_4,RL_PARM2_4,RL_RET_4,
"_IF_datan2",           OP_ATAN2,       FD,     RL_PARM_F,RL_PARM2_F,RL_RET_F,
"_IF_atan2",            OP_ATAN2,       FS,     RL_PARM_F,RL_PARM2_F,RL_RET_F,
"_IF_dfmod",            OP_FMOD,        FD,     RL_PARM_F,RL_PARM2_F,RL_RET_F,
"_IF_fmod",             OP_FMOD,        FS,     RL_PARM_F,RL_PARM2_F,RL_RET_F,
"_IF_dlog",             OP_LOG,         FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_log",              OP_LOG,         FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dcos",             OP_COS,         FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_cos",              OP_COS,         FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dsin",             OP_SIN,         FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_sin",              OP_SIN,         FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dtan",             OP_TAN,         FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_tan",              OP_TAN,         FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dsqrt",            OP_SQRT,        FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_sqrt",             OP_SQRT,        FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dfabs",            OP_FABS,        FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_fabs",             OP_FABS,        FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dacos",            OP_ACOS,        FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_acos",             OP_ACOS,        FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dasin",            OP_ASIN,        FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_asin",             OP_ASIN,        FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_datan",            OP_ATAN,        FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_atan",             OP_ATAN,        FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dcosh",            OP_COSH,        FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_cosh",             OP_COSH,        FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dexp",             OP_EXP,         FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_exp",              OP_EXP,         FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dlog10",           OP_LOG10,       FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_log10",            OP_LOG10,       FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dsinh",            OP_SINH,        FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_sinh",             OP_SINH,        FS,     RL_PARM_F,RL_,RL_RET_F,
"_IF_dtanh",            OP_TANH,        FD,     RL_PARM_F,RL_,RL_RET_F,
"_IF_tanh",             OP_TANH,        FS,     RL_PARM_F,RL_,RL_RET_F,
"_tls_index",           OP_NOP,         WD,     RL_,RL_,RL_,
"_tls_array",           OP_NOP,         WD,     RL_,RL_,RL_,
"__NOP", OP_NOP
};

extern  char *AskRTName( int rtindex )
/************************************/
{
    return( RTInfo[rtindex].nam );
}

extern  bool RTLeaveOp2( instruction *ins )
/*****************************************/
{
    ins = ins;
    return( FALSE );
}

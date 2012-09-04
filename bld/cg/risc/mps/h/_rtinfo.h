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
* Description:  Runtime support routines list.
*
****************************************************************************/


/* NB: Must be in sync with mipsrtrtn.c */

/*  beginning of runtime calls */

/*    emum               name                 op              class left       right       result */
PICK( RT_DIV_64S,        "_WtcI8D",           OP_DIV,         I8,   RL_PARM_8, RL_PARM2_8, RL_RET_8 )
PICK( RT_DIV_64U,        "_WtcU8D",           OP_DIV,         U8,   RL_PARM_8, RL_PARM2_8, RL_RET_8 )
PICK( RT_MOD_64S,        "_WtcI8R",           OP_MOD,         I8,   RL_PARM_8, RL_PARM2_8, RL_RET_8 )
PICK( RT_MOD_64U,        "_WtcU8R",           OP_MOD,         U8,   RL_PARM_8, RL_PARM2_8, RL_RET_8 )
PICK( RT_MEMCPY,         "_WtcMemCopy",       OP_MOV,         XX,   RL_PARM_4, RL_PARM2_4, RL_RET_4 )
PICK( RT_STK_CRAWL,      "_WtcStkCrawl",      OP_STACK_ALLOC, XX,   RL_,       RL_,        RL_ )
PICK( RT_STK_CRAWL_SIZE, "_WtcStkCrawlSize",  OP_STACK_ALLOC, XX,   RL_,       RL_,        RL_ )
PICK( RT_STK_STOMP,      "_WtcStkScribble",   OP_CALL,        XX,   RL_,       RL_,        RL_ )
PICK( RT_STK_COPY,       "_WtcStkCopy",       OP_CALL,        XX,   RL_,       RL_,        RL_ )
PICK( RT_EXCEPT_RTN,     "__ExceptionFilter", OP_CALL,        XX,   RL_,       RL_,        RL_ )
PICK( RT_POW,            "_IF_dpow",          OP_POW,         FD,   RL_PARM_F, RL_PARM2_F, RL_RET_F )
PICK( RT_POW_S,          "_IF_pow",           OP_POW,         FS,   RL_PARM_F, RL_PARM2_F, RL_RET_F )
PICK( RT_POW_I,          "_IF_powii",         OP_POW,         I4,   RL_PARM_4, RL_PARM2_4, RL_RET_4 )
PICK( RT_ATAN2,          "_IF_datan2",        OP_ATAN2,       FD,   RL_PARM_F, RL_PARM2_F, RL_RET_F )
PICK( RT_ATAN2_S,        "_IF_atan2",         OP_ATAN2,       FS,   RL_PARM_F, RL_PARM2_F, RL_RET_F )
PICK( RT_FMOD,           "_IF_dfmod",         OP_FMOD,        FD,   RL_PARM_F, RL_PARM2_F, RL_RET_F )
PICK( RT_FMOD_S,         "_IF_fmod",          OP_FMOD,        FS,   RL_PARM_F, RL_PARM2_F, RL_RET_F )
PICK( RT_LOG,            "_IF_dlog",          OP_LOG,         FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_LOG_S,          "_IF_log",           OP_LOG,         FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_COS,            "_IF_dcos",          OP_COS,         FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_COS_S,          "_IF_cos",           OP_COS,         FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_SIN,            "_IF_dsin",          OP_SIN,         FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_SIN_S,          "_IF_sin",           OP_SIN,         FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_TAN,            "_IF_dtan",          OP_TAN,         FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_TAN_S,          "_IF_tan",           OP_TAN,         FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_SQRT,           "_IF_dsqrt",         OP_SQRT,        FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_SQRT_S,         "_IF_sqrt",          OP_SQRT,        FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_FABS,           "_IF_dfabs",         OP_FABS,        FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_FABS_S,         "_IF_fabs",          OP_FABS,        FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_ACOS,           "_IF_dacos",         OP_ACOS,        FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_ACOS_S,         "_IF_acos",          OP_ACOS,        FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_ASIN,           "_IF_dasin",         OP_ASIN,        FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_ASIN_S,         "_IF_asin",          OP_ASIN,        FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_ATAN,           "_IF_datan",         OP_ATAN,        FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_ATAN_S,         "_IF_atan",          OP_ATAN,        FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_COSH,           "_IF_dcosh",         OP_COSH,        FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_COSH_S,         "_IF_cosh",          OP_COSH,        FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_EXP,            "_IF_dexp",          OP_EXP,         FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_EXP_S,          "_IF_exp",           OP_EXP,         FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_LOG10,          "_IF_dlog10",        OP_LOG10,       FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_LOG10_S,        "_IF_log10",         OP_LOG10,       FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_SINH,           "_IF_dsinh",         OP_SINH,        FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_SINH_S,         "_IF_sinh",          OP_SINH,        FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_TANH,           "_IF_dtanh",         OP_TANH,        FD,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_TANH_S,         "_IF_tanh",          OP_TANH,        FS,   RL_PARM_F, RL_,        RL_RET_F )
PICK( RT_TLS_INDEX,      "_tls_index",        OP_NOP,         WD,   RL_,       RL_,        RL_ )
PICK( RT_TLS_ARRAY,      "_tls_array",        OP_NOP,         WD,   RL_,       RL_,        RL_ )
PICK( RT_NOP,            "__NOP",             OP_NOP,         0,    RL_,       RL_,        RL_ )


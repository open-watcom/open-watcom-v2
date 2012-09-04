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


/*        NB entries up until OK must match typclass.wif */

/*  beginning of runtime calls */

/*    enum                name                  op              class   left        right       result */
PICK( RT_MEMCPY,          "_WtcMemCopy",        OP_MOV,         XX,     RL_PARM_4,  RL_PARM2_4, RL_RET_4 )
PICK( RT_TOC_NAME,        ".toc",               OP_MOV,         XX,     RL_,        RL_,        RL_ )
PICK( RT_STK_CRAWL,       "_WtcStkCrawl",       OP_STACK_ALLOC, XX,     RL_,        RL_,        RL_ )
PICK( RT_STK_CRAWL_SIZE,  "_WtcStkCrawlSize",   OP_STACK_ALLOC, XX,     RL_,        RL_,        RL_ )
PICK( RT_STK_STOMP,       "_WtcStkScribble",    OP_CALL,        XX,     RL_,        RL_,        RL_ )
PICK( RT_EXCEPT_RTN,      "__ExceptionFilter",  OP_CALL,        XX,     RL_,        RL_,        RL_ )
PICK( RT_I4TOD,           "_WtcI4TOD",          OP_CONVERT,     FD,     RL_PARM_4,  RL_,        RL_RET_F )
PICK( RT_U4TOD,           "_WtcU4TOD",          OP_CONVERT,     FD,     RL_PARM_4,  RL_,        RL_RET_F )
PICK( RT_DTOI4,           "_WtcDTOI4",          OP_CONVERT,     I4,     RL_PARM_F,  RL_,        RL_RET_4 )
PICK( RT_DTOU4,           "_WtcDTOU4",          OP_CONVERT,     U4,     RL_PARM_F,  RL_,        RL_RET_4 )
PICK( RT_NOP,             "__NOP",              OP_NOP,         0,      RL_,        RL_,        RL_ )


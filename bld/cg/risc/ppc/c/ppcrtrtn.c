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
"_WtcMemCopy",          OP_MOV,         XX,     RL_PARM_4,RL_PARM2_4,RL_RET_4,
".toc",                 OP_MOV,         XX,     RL_,RL_,RL_,
"_WtcStkCrawl",         OP_STK_ALLOC,   XX,     RL_,RL_,RL_,
"_WtcStkCrawlSize",     OP_STK_ALLOC,   XX,     RL_,RL_,RL_,
"_WtcStkScribble",      OP_CALL,        XX,     RL_,RL_,RL_,
"__ExceptionFilter",    OP_CALL,        XX,     RL_,RL_,RL_,
"_WtcI4TOD",            OP_CONVERT,     FD,     RL_PARM_4,RL_,RL_RET_F,
"_WtcU4TOD",            OP_CONVERT,     FD,     RL_PARM_4,RL_,RL_RET_F,
"_WtcDTOI4",            OP_CONVERT,     I4,     RL_PARM_F,RL_,RL_RET_4,
"_WtcDTOU4",            OP_CONVERT,     U4,     RL_PARM_F,RL_,RL_RET_4,
"__NOP", OP_NOP };

extern  char    *AskRTName( int rtindex ) {
/*****************************************/
    return( RTInfo[  rtindex  ].nam );
}

extern  bool    RTLeaveOp2( instruction *ins ) {
/**********************************************/

    return( FALSE );
}

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
* Description:  Defines debugger global variables. The variables are 
*               instantiated in dbgdata.c
*
****************************************************************************/

#include <string.h>
#include "dbgtoken.h"
#include "dbgregd.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgbreak.h"
#include "dbginp.h"
#include "trpcore.h"

#if !defined( global )
#define global  extern
#endif

global char             *InitCmdList;
global char             *TrapParms;

global char             *InvokeFile;
global char             *DipFiles[10];

global char_ring        *SrcSpec;
global debug_level      DbgLevel;
global debug_level      ActiveWindowLevel;

global address          NilAddr;

global machine_state    *DbgRegs;
global machine_state    *PrevRegs;

global tokens           CurrToken;
global dbg_switches     DbgSwitches;
global screen_state     ScrnState;

global unsigned         ExprAddrDepth;
global unsigned         NestedCallLevel;
global int              PgmStackUsage[MAX_NESTED_CALL];

global unsigned char    DefRadix;
global unsigned char    CurrRadix;

global unsigned         InvCount;

global update_list      UpdateFlags;

global unsigned long    MemSize;

global location_context Context;
global mod_handle       ContextMod;

global mod_handle       CodeAddrMod;
global unsigned         TaskId;

global brkp             UserTmpBrk; // brk changed
global brkp             DbgTmpBrk;  // brk changed
global brkp             *BrkList;   // brk changed

global void             *CmdHistory;
global void             *SrchHistory;

global unsigned         SkipCount;
global input_stack      *InpStack;
global system_config    SysConfig;
global unsigned         CheckSize;
global unsigned         OvlSize;

global const char       *CmdStart;
global thread_state     *HeadThd;
global thread_state     *ExecThd;
global image_entry      *DbgImageList;
global dip_status       DIPStatus;

global char             *TxtBuff;
global char             *NameBuff;

global char             DbgBuffers[(TXT_LEN + 1) + (NAM_LEN + 1)]; // big!

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
* Description:  Defines debugger global variables. Currently mainly referenced
*               as externs by other compilation units :-(
*
****************************************************************************/

#include "dbgdefn.h"
#include "dbgtoken.h"
#include "dbgreg.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgbreak.h"
#include "dbginp.h"
#include "trpcore.h"

char                    *InitCmdList;
char                    *TrpFile;
#ifdef ENABLE_TRAP_LOGGING
char                    *TrpDebugFile;
bool                    TrpDebugFileFlush = FALSE;
#endif

/*
 *  Queried from trap file supplemental services
 */
bool                    Supports8ByteBreakpoints = FALSE;
bool                    SupportsExactBreakpoints = FALSE;

char                    *InvokeFile;
char                    *DipFiles[10];

char_ring               *SrcSpec;
debug_level             DbgLevel;
debug_level             ActiveWindowLevel;
char                    *Language = NULL;

address                 NilAddr;

machine_state           *DbgRegs;
machine_state           *PrevRegs;

tokens                  CurrToken;
dbg_switches            DbgSwitches;
screen_state            ScrnState;

unsigned                ExprAddrDepth;
unsigned                NestedCallLevel = 0;
int                     PgmStackUsage[ MAX_NESTED_CALL ];

unsigned char           DefRadix;
unsigned char           CurrRadix;

unsigned                InvCount;

update_list             WndFlags = 0;

unsigned long           MemSize;

struct location_context Context;
mod_handle              ContextMod;

mod_handle              CodeAddrMod;
unsigned                TaskId;

brkp                    UserTmpBrk; // brk changed
brkp                    DbgTmpBrk;  // brk changed
brkp                    *BrkList;   // brk changed

void                    *CmdHistory;
void                    *SrchHistory;

unsigned                SkipCount;
input_stack             *InpStack;
system_config           SysConfig;
unsigned                CheckSize;
unsigned                OvlSize;

char                    *CmdStart;
thread_state            *HeadThd;
thread_state            *ExecThd;
image_entry             *DbgImageList;
dip_status              DIPStatus;

char                    *TxtBuff;
char                    *NameBuff;

char DbgBuffers[TXT_LEN+1 + NAM_LEN+1] = " "; // big!
char                    OnOffNameTab[] = { "ON\0OFf\0" };

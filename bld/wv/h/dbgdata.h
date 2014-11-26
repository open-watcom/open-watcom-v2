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

#include "dbgtoken.h"
#include "dbgregd.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgbreak.h"
#include "dbginp.h"
#include "trpcore.h"

extern char             *InitCmdList;
extern char             *TrapParms;

/*
 *  Queried from trap file supplemental services
 */
extern bool             Supports8ByteBreakpoints;
extern bool             SupportsExactBreakpoints;

extern char             *InvokeFile;
extern char             *DipFiles[10];

extern char_ring        *SrcSpec;
extern debug_level      DbgLevel;
extern debug_level      ActiveWindowLevel;
extern char             *Language;

extern address          NilAddr;

extern machine_state    *DbgRegs;
extern machine_state    *PrevRegs;

extern tokens           CurrToken;
extern dbg_switches     DbgSwitches;
extern screen_state     ScrnState;

extern unsigned         ExprAddrDepth;
extern unsigned         NestedCallLevel;
extern int              PgmStackUsage[ MAX_NESTED_CALL ];

extern unsigned char    DefRadix;
extern unsigned char    CurrRadix;

extern unsigned         InvCount;

extern update_list      WndFlags;

extern unsigned long    MemSize;

extern struct location_context Context;
extern mod_handle       ContextMod;

extern mod_handle       CodeAddrMod;
extern unsigned         TaskId;

extern brkp             UserTmpBrk; // brk changed
extern brkp             DbgTmpBrk;  // brk changed
extern brkp             *BrkList;   // brk changed

extern void             *CmdHistory;
extern void             *SrchHistory;

extern unsigned         SkipCount;
extern input_stack      *InpStack;
extern system_config    SysConfig;
extern unsigned         CheckSize;
extern unsigned         OvlSize;

extern char             *CmdStart;
extern thread_state     *HeadThd;
extern thread_state     *ExecThd;
extern image_entry      *DbgImageList;
extern dip_status       DIPStatus;

extern char             *TxtBuff;
extern char             *NameBuff;

extern char DbgBuffers[TXT_LEN+1 + NAM_LEN+1]; // big!
extern char             OnOffNameTab[];

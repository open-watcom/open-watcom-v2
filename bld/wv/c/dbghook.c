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


#include "dbgdefn.h"
#include "dip.h"
#include "dbgerr.h"
#include "dbglit.h"
#include "dbginp.h"
#include "dbghook.h"
#include "dbgtoggl.h"
#include "trpcore.h"
#include "spawn.h"
#include "dui.h"

extern void             FreeCmdList(cmd_list *);
extern void             PushCmdList(cmd_list *);
extern cmd_list         *AllocCmdList(char *,unsigned int );
extern unsigned         ScanCmd( char * );
extern bool             ScanEOC(void);
extern void             ReqEOC(void);
extern bool             ScanItem( bool, char **,unsigned int *);
extern char             *GetCmdEntry(char *,int ,char *);
extern input_type       SetInpStack( input_type );
extern char             *Format(char *,char *,... );
extern char             *StrCopy( char *, char * );
extern char             *GetCmdName( int );
extern void             ConfigCmdList( char *cmds, int indent );
extern void             TypeInpStack( input_type set );
extern void             DoProcPending(void);


extern char             *TxtBuff;
extern mod_handle       ContextMod;
extern system_config    SysConfig;

static unsigned long    HookPendingBits;
static cmd_list         *HookCmdLists[HOOK_NUM];
static bool             HadSrcInfo;


void InitHook()
{
    HookPendingBits = 0;
    /* this is so we run the src/asm stuff after first load */
    HadSrcInfo = 2;
}

void FiniHook()
{
    int         i;

    for( i = 0; i < HOOK_NUM; ++i ) {
        FreeCmdList( HookCmdLists[i] );
        HookCmdLists[i] = NULL;
    }
}

static char HookNames[] = {
    "PROGStart\0"
    "PROGEnd\0"
    "DLLStart\0"
    "DLLEnd\0"
    "EXECStart\0"
    "EXECEnd\0"
    "Sourceinfo\0"
    "Assemblyinfo\0"
    "Modulechange\0"
    "Quit\0"
};

void ProcHook()
{
    char        *start;
    unsigned    len;
    unsigned    idx;
    cmd_list    *list;

    idx = ScanCmd( HookNames );
    if( idx == 0 ) {
        Error( ERR_NONE, LIT( ERR_BAD_HOOK ) );
    }
    idx -= 1;
    if( ScanEOC() ) {
        list = NULL;
    } else if( ScanItem( FALSE, &start, &len ) ) {
        ReqEOC();
        list = AllocCmdList( start, len );
    }
    FreeCmdList( HookCmdLists[idx] );
    HookCmdLists[idx] = list;
}

void ConfigHook()
{
    unsigned    idx;
    char        *p;

    ReqEOC();
    for( idx = 0; idx < HOOK_NUM; ++idx ) {
        p = StrCopy( GetCmdName( CMD_HOOK ), TxtBuff );
        *p++ = ' ';
        p = GetCmdEntry( HookNames, idx+1, p );
        *p++ = ' ';
        *p++ = '{';
        *p++ = '\0';
        DUIDlgTxt( TxtBuff );
        if( HookCmdLists[idx] != NULL ) {
            ConfigCmdList( HookCmdLists[ idx ]->buff, 0 );
        }
        DUIDlgTxt( "}" );
    }
}

void HookNotify( bool immediate, hook_type hook )
{
    cmd_list    *list;
    input_type  save;

    hook -= HOOK_FIRST;
    if( immediate || _IsOn( SW_RUNNING_PROFILE ) ) {
        list = HookCmdLists[hook];
        if( list != NULL ) {
            save = SetInpStack( INP_STOP_PURGE );
            PushCmdList( list );
            TypeInpStack( INP_HOOK );
            Spawn( DoProcPending );
            SetInpStack( save );
        }
    } else {
        HookPendingBits |= 1UL << hook;
    }
}


bool HookPendingPush()
{
    unsigned long       test;
    cmd_list            **list;
    bool                have_src_info;

    if( HookPendingBits == 0 ) return( FALSE );
    test = 1;
    list = HookCmdLists;
    while( !(test & HookPendingBits) ) {
        ++list;
        test <<= 1;
    }
    HookPendingBits &= ~test;
    if( *list != NULL ) {
        PushCmdList( *list );
        TypeInpStack( INP_HOOK );
    }
    if( !(test & (1UL << (HOOK_NEW_MODULE-HOOK_FIRST))) ) return( TRUE );
    if( _IsOff( SW_HAVE_TASK ) && _IsOff( SW_PROC_ALREADY_STARTED ) ) return( TRUE );
    /*
       If the module has changed, we have to see if we've changed
       from a region with no source information to one with or
       vis-versa.
    */
    have_src_info = (ModHasInfo( ContextMod, HK_CUE ) == DS_OK);
    if( have_src_info != HadSrcInfo ) {
        HadSrcInfo = have_src_info;
        if( have_src_info ) {
            HookPendingBits |= 1UL << (HOOK_SRC_START-HOOK_FIRST);
        } else {
            HookPendingBits |= 1UL << (HOOK_SRC_END-HOOK_FIRST);
        }
    }
    return( TRUE );
}

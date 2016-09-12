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


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dip.h"
#include "dbgerr.h"
#include "dbglit.h"
#include "dbghook.h"
#include "spawn.h"
#include "dui.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgmain.h"
#include "dbgshow.h"
#include "dbgpend.h"

#define SET_HOOK_BIT(x)     HookPendingBits |= ((hook_bits)1 << x)
#define RESET_HOOK_BIT(x)   HookPendingBits &= ~((hook_bits)1 << x)
#define TEST_HOOK_BIT(x)    ((HookPendingBits & ((hook_bits)1 << x)) != 0)

typedef unsigned long   hook_bits;

static hook_bits        HookPendingBits;
static cmd_list         *HookCmdLists[HOOK_NUM];
static bool             HadSrcInfo;

static const char HookNames[] = {
    #define pick(e,name)    name
    #include "_dbghook.h"
    #undef pick
};


void InitHook( void )
{
    HookPendingBits = 0;
    /* this is so we run the src/asm stuff after first load */
    HadSrcInfo = true;
}

void FiniHook( void )
{
    hook_type   hook;

    for( hook = 0; hook < HOOK_NUM; ++hook ) {
        FreeCmdList( HookCmdLists[hook] );
        HookCmdLists[hook] = NULL;
    }
}

void ProcHook( void )
{
    const char  *start;
    size_t      len;
    int         idx;
    cmd_list    *list;

    idx = ScanCmd( HookNames );
    if( idx < 0 ) {
        Error( ERR_NONE, LIT_ENG( ERR_BAD_HOOK ) );
    } else {
        list = NULL;
        if( ScanEOC() ) {
        } else if( ScanItem( false, &start, &len ) ) {
            ReqEOC();
            while( len > 0 && *start == '\r' ) {
                ++start;
                --len;
            }
            if( len > 0 ) {
                list = AllocCmdList( start, len );
            }
        }
        FreeCmdList( HookCmdLists[idx] );
        HookCmdLists[idx] = list;
    }
}

void ConfigHook( void )
{
    hook_type   hook;
    char        *p;

    ReqEOC();
    for( hook = 0; hook < HOOK_NUM; ++hook ) {
        p = StrCopy( GetCmdName( CMD_HOOK ), TxtBuff );
        *p++ = ' ';
        p = GetCmdEntry( HookNames, hook, p );
        *p++ = ' ';
        *p++ = '{';
        *p++ = NULLCHAR;
        DUIDlgTxt( TxtBuff );
        if( HookCmdLists[hook] != NULL ) {
            ConfigCmdList( HookCmdLists[hook]->buff, 0 );
        }
        DUIDlgTxt( "}" );
    }
}

void HookNotify( bool immediate, hook_type hook )
{
    cmd_list    *list;
    input_type  save;

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
        SET_HOOK_BIT( hook );
    }
}


bool HookPendingPush( void )
{
    hook_type       test;
    cmd_list        **list;
    bool            have_src_info;

    if( HookPendingBits == 0 )
        return( false );
    test = 0;
    list = HookCmdLists;
    while( !TEST_HOOK_BIT( test ) ) {
        ++list;
        ++test;
    }
    RESET_HOOK_BIT( test );
    if( *list != NULL ) {
        PushCmdList( *list );
        TypeInpStack( INP_HOOK );
    }
    if( test != HOOK_NEW_MODULE )
        return( true );
    if( _IsOff( SW_HAVE_TASK ) && _IsOff( SW_PROC_ALREADY_STARTED ) )
        return( true );
    /*
       If the module has changed, we have to see if we've changed
       from a region with no source information to one with or
       vis-versa.
    */
    have_src_info = ( DIPModHasInfo( ContextMod, HK_CUE ) == DS_OK );
    if( have_src_info != HadSrcInfo ) {
        HadSrcInfo = have_src_info;
        if( have_src_info ) {
            SET_HOOK_BIT( HOOK_SRC_START );
        } else {
            SET_HOOK_BIT( HOOK_SRC_END );
        }
    }
    return( true );
}

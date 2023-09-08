/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Debuggee address mapping (using Toolhelp).
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include "stdwin.h"
#include "dbgrmsg.h"


#define MAX_MODULE      256

typedef struct {
    WORD        sig[2];
    WORD        dataseg_off;
    WORD        codeinfo_off;
    WORD        stacksize_off;
    BYTE        call_cleanup[3];
    BYTE        mov_ax_4c00[3];
    BYTE        int_21[2];
    WORD        new_sig[2];
} winext_data;

int        ModuleTop=0;
int        CurrentModule = 0;

static int      mod32Top;

static HMODULE moduleIDs[MAX_MODULE];
static HMODULE modules32[MAX_MODULE];
static bool moduleIsDLL[MAX_MODULE];

/*
 * AddAllCurrentModules - add all currently running modules to
 *                        the module list
 */
void AddAllCurrentModules( void )
{
    MODULEENTRY me;

    me.dwSize = sizeof( MODULEENTRY );
    if( !ModuleFirst( &me ) )
        return;
    do {
        if( me.hModule != DebugeeModule ) {
            AddModuleLoaded( me.hModule, false );
        }
        me.dwSize = sizeof( MODULEENTRY );
    } while( ModuleNext( &me ) );

} /* AddAllCurrentModules */

/*
 * CheckIsModuleWin32App - check if a given module handle is a win32 app
 */
static bool CheckIsModuleWin32App( HMODULE hmod, WORD *win32ds, WORD *win32cs, DWORD *win32initialeip )
{
    GLOBALENTRY ge;
    winext_data wedata;
    int         segnum;
    addr48_ptr  addr;

    *win32cs = *win32ds = 0;
    ge.dwSize = sizeof( GLOBALENTRY );
    if( !GlobalEntryModule( &ge, hmod, 1 ) ) {
        return( false );
    }
    addr.segment = (WORD)ge.hBlock;
    addr.offset = 0;
    ReadMemory( &addr, &wedata, sizeof( wedata ) );
    if( memcmp( wedata.sig, win386sig, SIG_SIZE ) == 0 || memcmp( wedata.sig, win386sig2, SIG_SIZE ) == 0 ) {
        if( memcmp( wedata.new_sig, win386sig2, SIG_SIZE ) == 0 ) {
            segnum = 2;
        } else {
            segnum = 3;
        }
        if( !GlobalEntryModule( &ge, hmod, segnum ) ) {
            return( false );
        }
        addr.segment = (WORD)ge.hBlock;
        addr.offset = wedata.dataseg_off;
        ReadMemory( &addr, win32ds, sizeof( WORD ) );
        addr.offset = wedata.stacksize_off;
        ReadMemory( &addr, win32initialeip, sizeof( DWORD ) );
        addr.offset = wedata.codeinfo_off + 4;
        ReadMemory( &addr, win32cs, sizeof( WORD ) );
        return( true );
    }
    return( false );

} /* CheckIsModuleWin32App */

/*
 * try32:
 *
 * see if a module is a Win386 app.  If it is, then we add it to a list
 * of 32-bit apps, so that we can give the segment aliases for them later.
 */
static void try32( HANDLE mod )
{
    DWORD       off;
    WORD        ds;
    WORD        cs;

    if( CheckIsModuleWin32App( mod, &ds, &cs, &off ) ) {
        modules32[mod32Top] = mod;
        mod32Top++;
    }

} /* try32 */

/*
 * HasSegAliases - report if any modules are Win386 maps
 */
bool HasSegAliases( void )
{
    return( (mod32Top > 0) );

} /* HasSegAliases */

/*
 * AddDebugeeModule - put debugee module in table first, since thats what
 *                    the debugger expects.
 */
void AddDebugeeModule( void )
{
    int i;
    for( i = ModuleTop; i > 0; i-- ) {
        moduleIDs[i] = moduleIDs[i - 1];
    }
    ModuleTop++;
    Out((OUT_MAP,"AddDebugeeModule - ModuleTop=%d",ModuleTop));
    moduleIDs[0] = DebugeeModule;
    moduleIsDLL[0] = false;
    try32( DebugeeModule );

} /* AddDebugeeModule */

/*
 * AddModuleLoaded - add a loaded module (dll or task) to the list
 */
void AddModuleLoaded( HANDLE mod, bool is_dll )
{
    int         i;

    if( ModuleTop >= MAX_MODULE ) {
        return;
    }

    if( ModuleTop == 0 ) {
        Out((OUT_MAP,"AddModuleLoaded ModuleTop=%d mod=%d dll=%d",ModuleTop,mod,is_dll));
    }
    for( i = 0; i < ModuleTop; i++ ) {
        if( moduleIDs[i] == mod ) {
            Out((OUT_MAP,"Already there"));
            return;
        }
    }

    try32( mod );

    moduleIsDLL[ModuleTop] = is_dll;
    moduleIDs[ModuleTop] = mod;
    ModuleTop++;

} /* AddModuleLoaded */

/*
 * AccGetLibName - get lib name of current module
 */
trap_retval TRAP_CORE( Get_lib_name )( void )
{
    MODULEENTRY         me;
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    size_t              name_maxlen;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    if( acc->mod_handle != 0 ) {
        CurrentModule = acc->mod_handle + 1;
    }
    Out(( OUT_MAP,"acc->handle = %ld", acc->mod_handle ));
    if( CurrentModule >= ModuleTop ) {
        ret->mod_handle = 0;
        Out(( OUT_MAP,"Past end of list" ));
        return( sizeof( *ret ) );
    }
    Out(( OUT_MAP,"ModuleTop=%d CurrentModule=%d id=%d", ModuleTop, CurrentModule, moduleIDs[CurrentModule] ));
    name = GetOutPtr( sizeof( *ret ) );
    *name = '\0';
    me.dwSize = sizeof( me );
    if( ModuleFindHandle( &me, moduleIDs[CurrentModule] ) ) {
        name_maxlen = GetTotalSizeOut() - sizeof( *ret ) - 1;
        strncpy( name, me.szExePath, name_maxlen );
        name[name_maxlen] = '\0';
    }
    ret->mod_handle = CurrentModule;
    Out(( OUT_MAP,"handle=%ld, name=\"%s\"", ret->mod_handle, name ));
    return( sizeof( *ret ) + strlen( name ) + 1 );
}

/*
 * accessSegment:
 *
 * Access a segment to cause the windows loader to load it. We find
 * the segment by looking through the module entry (which is a lot like
 * an NE header, except that where segment numbers would be, the selector
 * value is). We then have the app we are debugging execute an instruction that
 * references the seqment in question.
 */
static void accessSegment( GLOBALHANDLE gh, WORD segment )
{
    WORD                i;
    WORD                sel;
    WORD                offset;
    addr48_ptr          addr;

    addr.segment = (WORD)gh;
    addr.offset = 0x22;
    ReadMemory( &addr, &offset, sizeof( offset ) );
    for( i = 0; i < segment; i++ ) {
        addr.offset = offset + 8;
        ReadMemory( &addr, &sel, sizeof( sel ) );
        offset += 10;
    }
    SegmentToAccess = sel;
    DebuggerWaitForMessage( RUNNING_DEBUGEE, TaskAtFault, ACCESS_SEGMENT );

} /* accessSegment */

/*
 * horkyFindSegment:
 *
 * runs and tries to find a segment.  It does this by finding the module
 * entry in the global heap for this task.  The module entry is a lot like
 * an NE header, except that instead of segment numbers, it has the selector
 * values themselves.  We pass the module entry to accessSegment, who looks
 * up the selector and causes it to load.
 *
 * The reason: load on call segments are not identified by toolhelp,
 * so mapaddrs for those segments would fail without this putrid code.
 * As well, the segments themselves are not always loaded (loadoncall
 * segments), so if you were to look at the memory there, you would see
 * nothing.
 *
 * Special note:  this only works for the task that you are debugging.
 * Windows knows what the current task is, and when you access a not present
 * segment, it tries to load the segment based on the current task.  Thus,
 * you cannot try to have the debugee fault in a segment belonging to some
 * other task. Why DLL's work at all (since they are not a "task") is beyond
 * me.
 */
static bool horkyFindSegment( int module, WORD segment )
{
    static GLOBALENTRY  ge;
    static HMODULE      lastmodid;
    HMODULE             modid;

    modid = moduleIDs[module];
    if( !moduleIsDLL[module] ) {
        return( false );
    }

    if( lastmodid == modid ) {
        accessSegment( ge.hBlock, segment );
        return( true );
    }
    lastmodid = modid;
    ge.dwSize = sizeof( ge );
    if( !GlobalFirst( &ge, GLOBAL_ALL ) ) {
        lastmodid = NULL;
        return( false );
    }
    do {
        if( ge.hOwner == modid && ge.wType == GT_MODULE ) {
            accessSegment( ge.hBlock, segment );
            return( true );
        }
        ge.dwSize = sizeof( ge );
    } while( GlobalNext( &ge, GLOBAL_ALL ) );
    lastmodid = NULL;
    return( false );

} /* horkyFindSegment */

/*
 * TRAP_CORE( Map_addr )
 *
 * Access request to map a segment number to a selector.
 * Possibilites:
 *  1) We are mapping segments for a 32-bit extended app.  In this case,
 *     we return the segment:offset returned by CheckIsModuleWin32App
 *  2) We are mapping a segment for a 16-bit app that is NOT a load
 *     on call segment.  In this case, GlobalEntryModule works and
 *     we return the value we obtain from it
 *  3) We are mapping a segment for a 16-bit app that IS a load
 *     on call segment.  In this case, GlobalEntryModule FAILS (stupid
 *     f*cking Windows) and so we have to go find it ourselves using
 *     horkyFindSegment.  Once horkyFindSegment is done, GlobalEntryModule
 *     will give the right answer, and we return the value we obtain from it.
 */
trap_retval TRAP_CORE( Map_addr )( void )
{
    GLOBALENTRY ge;
    LPVOID      ptr;
    WORD        sel;
    WORD        cs,ds;
    DWORD       off;
    int         module;
    addr_seg    in_seg;
    map_addr_req        *acc;
    map_addr_ret        *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->out_addr = acc->in_addr;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    module = acc->mod_handle;
    in_seg = acc->in_addr.segment;
    if( CheckIsModuleWin32App( moduleIDs[module], &ds, &cs, &off ) ) {
        Out((OUT_MAP,"is 32 bit module"));
        if( in_seg == MAP_FLAT_DATA_SELECTOR ) {
            ret->out_addr.segment = ds;
        } else {
            ret->out_addr.segment = cs;
        }
        ret->out_addr.offset = off;
    } else {
        switch( in_seg ) {
        case MAP_FLAT_CODE_SELECTOR:
        case MAP_FLAT_DATA_SELECTOR:
            in_seg = 1;
            break;
        }
        ge.dwSize = sizeof( ge );
        if( !GlobalEntryModule( &ge, moduleIDs[module], in_seg ) ) {
            if( horkyFindSegment( module, in_seg ) ) {
                if( !GlobalEntryModule( &ge, moduleIDs[module], in_seg ) ) {
                    Out((OUT_MAP,"GlobalEntry 2nd time failed" ));
                    return( sizeof( *ret ) );
                }
            } else {
                Out(( OUT_MAP,"Well I'll be! horkyFindSegment failed" ));
                return( sizeof( *ret ) );
            }
        }
        ptr = GlobalLock( ge.hBlock );
        GlobalUnlock( ge.hBlock );
        sel = _FP_SEG( ptr );
        if( sel == NULL ) {
            sel = (WORD)ge.hBlock + 1;
        }
        ret->out_addr.segment = sel;
        ret->out_addr.offset = 0;
    }
    ret->out_addr.offset += acc->in_addr.offset;
    if( module == 0 ) {
        Out(( OUT_MAP,"in=%4.4x:%8.8lx out=%4.4x:%8.8lx", acc->in_addr.segment, acc->in_addr.offset, ret->out_addr.segment, ret->out_addr.offset ));
    }
    return( sizeof( *ret ) );
}

/*
 * TRAP_CORE( Get_next_alias )
 *
 * Get next alias, for 32-bit extender apps only
 * We maintain a list of all apps that are 32-bit, and return the
 * aliases for each of them.
 */
trap_retval TRAP_CORE( Get_next_alias )( void )
{
    get_next_alias_req  *acc;
    get_next_alias_ret  *ret;
    WORD                ds,cs;
    DWORD               off;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    if( mod32Top > 0 ) {
        mod32Top--;
        CheckIsModuleWin32App( modules32[mod32Top], &ds, &cs, &off );
        ret->seg = cs;
        ret->alias = ds;
    }
    return( sizeof( *ret ) );
}

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
* Description:  Debuggee address mapping (using Toolhelp).
*
****************************************************************************/


#ifdef __WINDOWS__
#pragma library("toolhelp.lib");
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include "stdwin.h"
#include "ismod32.h"

#define MAX_MODULE      256

int        ModuleTop=0;
int        CurrentModule = 0;

static int      mod32Top;

static HMODULE moduleIDs[ MAX_MODULE ];
static HMODULE modules32[ MAX_MODULE ];
static BOOL moduleIsDLL[ MAX_MODULE ];

/*
 * AddAllCurrentModules - add all currently running modules to
 *                        the module list
 */
void AddAllCurrentModules( void )
{
    MODULEENTRY me;

    me.dwSize = sizeof( MODULEENTRY );
    if( !ModuleFirst( &me ) ) return;
    do {
        if( me.hModule != DebugeeModule ) {
            AddModuleLoaded( me.hModule, FALSE );
        }
        me.dwSize = sizeof( MODULEENTRY );
    } while( ModuleNext( &me ) );

} /* AddAllCurrentModules */

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
        modules32[ mod32Top ] = mod;
        mod32Top++;
    }

} /* try32 */

/*
 * HasSegAliases - report if any modules are Win386 maps
 */
BOOL HasSegAliases( void )
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
        moduleIDs[i] = moduleIDs[i-1];
    }
    ModuleTop++;
    Out((OUT_MAP,"AddDebugeeModule - ModuleTop=%d",ModuleTop));
    moduleIDs[0] = DebugeeModule;
    moduleIsDLL[0] = FALSE;
    try32( DebugeeModule );

} /* AddDebugeeModule */

/*
 * AddModuleLoaded - add a loaded module (dll or task) to the list
 */
void AddModuleLoaded( HANDLE mod, BOOL is_dll )
{
    int         i;

    if( ModuleTop >= MAX_MODULE ) {
        return;
    }

    if( ModuleTop == 0 ) {
        Out((OUT_MAP,"AddModuleLoaded ModuleTop=%d mod=%d dll=%d",ModuleTop,mod,is_dll));
    }
    for( i=0;i<ModuleTop;i++ ) {
        if( moduleIDs[i] == mod ) {
            Out((OUT_MAP,"Already there"));
            return;
        }
    }

    try32( mod );

    moduleIsDLL[ ModuleTop ] = is_dll;
    moduleIDs[ ModuleTop ] = mod;
    ModuleTop++;

} /* AddModuleLoaded */

/*
 * AccGetLibName - get lib name of current module
 */
trap_elen ReqGet_lib_name( void )
{
    MODULEENTRY         me;
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    name = GetOutPtr( sizeof( *ret ) );
    name[0] = '\0';

    ret->handle = 0;

    if( acc->handle != 0 ) {
        CurrentModule = acc->handle + 1;
    }
    Out(( OUT_MAP,"acc->handle = %ld", acc->handle ));
    if( CurrentModule >= ModuleTop ) {
        Out(( OUT_MAP,"Past end of list" ));
        return( sizeof( *ret ) + 1 );
    }
    me.dwSize = sizeof( me );
    Out(( OUT_MAP,"ModuleTop=%d CurrentModule=%d id=%d", ModuleTop, CurrentModule, moduleIDs[ CurrentModule ] ));
    if( ModuleFindHandle( &me, moduleIDs[ CurrentModule ] ) ) {
        strcpy( name, me.szExePath );
    }
    ret->handle = CurrentModule;
    Out(( OUT_MAP,"handle=%ld, name=\"%s\"", ret->handle, name ));
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

    ReadMem( (WORD)gh, 0x22, &offset, sizeof( offset ) );
    i = 0;
    while( i < segment ) {
        ReadMem( (WORD)gh, offset+8, &sel, sizeof( sel ) );
        offset += 10;
        i++;
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
static BOOL horkyFindSegment( int module, WORD segment )
{
    static GLOBALENTRY  ge;
    static HMODULE      lastmodid;
    HMODULE             modid;

    modid = moduleIDs[ module ];
    if( !moduleIsDLL[ module ] ) {
        return( FALSE );
    }

    if( lastmodid == modid ) {
        accessSegment( ge.hBlock, segment );
        return( TRUE );
    }
    lastmodid = modid;
    ge.dwSize = sizeof( ge );
    if( !GlobalFirst( &ge, GLOBAL_ALL ) ) {
        lastmodid = NULL;
        return( FALSE );
    }
    do {
        if( ge.hOwner == modid && ge.wType == GT_MODULE ) {
            accessSegment( ge.hBlock, segment );
            return( TRUE );
        }
        ge.dwSize = sizeof( ge );
    } while( GlobalNext( &ge, GLOBAL_ALL ) );
    lastmodid = NULL;
    return( FALSE );

} /* horkyFindSegment */

/*
 * ReqMap_addr
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
trap_elen ReqMap_addr( void )
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
    module = acc->handle;
    in_seg = acc->in_addr.segment;
    if( CheckIsModuleWin32App( moduleIDs[ module ], &ds, &cs, &off ) ) {
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
        if( !GlobalEntryModule( &ge, moduleIDs[ module ], in_seg ) ) {
            if( horkyFindSegment( module, in_seg ) ) {
                if( !GlobalEntryModule( &ge, moduleIDs[ module ], in_seg ) ) {
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
        sel = FP_SEG( ptr );
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
 * ReqGet_next_alias
 *
 * Get next alias, for 32-bit extender apps only
 * We maintain a list of all apps that are 32-bit, and return the
 * aliases for each of them.
 */
trap_elen ReqGet_next_alias( void )
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
        CheckIsModuleWin32App( modules32[ mod32Top ], &ds, &cs, &off );
        ret->seg = cs;
        ret->alias = ds;
    }
    return( sizeof( *ret ) );
}

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
* Description:  Routines to keep track of loaded modules and address maps.
*               Very similar to the Linux version.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "trpimp.h"
#include "mad.h"
#include "ntocomm.h"


typedef struct lli {
    addr_off    base;
    addr_off    offset;
    addr_off    dbg_dyn_sect;
    addr_off    code_size;          
    char        newly_loaded : 1;   /* Library loaded but debugger not yet told */
    char        newly_unloaded : 1; /* Library unloaded but debugger not yet told */
    char        filename[257]; // TODO: These should really be dynamic!
} lib_load_info;

static lib_load_info    *moduleInfo;
static int              ModuleTop;

/*
 * FindLib - find a shared lib entry in the list
 */
lib_load_info *FindLib( addr_off dynsection )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].dbg_dyn_sect == dynsection )
            return( &moduleInfo[i] );
    }
    return( NULL );
}

addr_off FindLibInLinkMap( pid_handle pid, addr_off first_lmap, addr_off dyn_base )
{
    struct link_map     lmap;
    addr_off            dbg_lmap;

    dbg_lmap = first_lmap;
    while( dbg_lmap ) {
        if( !GetLinkMap( pid, dbg_lmap, &lmap ) ) break;
        if( (addr_off)lmap.l_ld == dyn_base )
            return( dbg_lmap );
        dbg_lmap = (addr_off)lmap.l_next;
    }
    return( 0 );
}

/*
 * AddProcess - a new process has been created
 */
void AddProcess( const char *exe_name, addr_off dynsection )
{
    lib_load_info       *lli;

    moduleInfo = malloc( sizeof( lib_load_info ) );
    memset( moduleInfo, 0, sizeof( lib_load_info ) );
    ModuleTop = 1;

    lli = &moduleInfo[0];

    lli->offset = 0;    /* Assume that main executable was not relocated */
    lli->dbg_dyn_sect = dynsection;
    strcpy( lli->filename, exe_name );
    dbg_print(( "Added process: ofs/dyn = %08x/%08x '%s'\n",
                (unsigned)lli->offset, (unsigned)lli->dbg_dyn_sect, lli->filename ));    
}

/*
 * AddLib - a new library has loaded
 */
void AddLib( pid_handle pid, struct link_map *lmap )
{
    lib_load_info       *lli;
    addr_off            map_addr;

    /* This code is not terribly efficient */
    ModuleTop++;
    lli = malloc( ModuleTop * sizeof( lib_load_info ) );
    memset( lli, 0, ModuleTop * sizeof( lib_load_info ) );
    memcpy( lli, moduleInfo, (ModuleTop - 1) * sizeof( lib_load_info ) );
    free( moduleInfo );
    moduleInfo = lli;
    lli = &moduleInfo[ModuleTop - 1];

    lli->dbg_dyn_sect = (addr_off)lmap->l_ld;
    dbg_strcpy( pid, lli->filename, lmap->l_path );
    lli->newly_loaded   = TRUE;
    lli->newly_unloaded = FALSE;
    lli->base = lmap->l_addr;

    /* Ask the OS to translate the base address into link-time VA, so that
     * we can perform the mappings later.
     */
    if( MapAddrToLinkVA( pid, lmap->l_addr, &map_addr ) ) {
        lli->offset = lmap->l_addr - map_addr;
    } else {
        lli->offset = 0;    /* Error - hope for the best */
    }

    dbg_print(( "Added library: ofs/dyn = %08x/%08x '%s'\n",
                (unsigned)lli->offset, (unsigned)lli->dbg_dyn_sect, lli->filename ));    
}

void DelLib( addr_off dynsection )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].dbg_dyn_sect == dynsection ) {
            dbg_print(( "Deleting library '%s'\n", moduleInfo[i].filename ));
            moduleInfo[i].newly_unloaded = TRUE;
            moduleInfo[i].newly_loaded   = FALSE;
            moduleInfo[i].offset = 0;
            moduleInfo[i].dbg_dyn_sect = 0;
            moduleInfo[i].code_size = 0;
            break;
        }
    }
}

void DelProcess( void )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        moduleInfo[i].offset = 0;
        moduleInfo[i].dbg_dyn_sect = 0;
        moduleInfo[i].code_size = 0;
    }
}


/*
 * AddLibs - called when dynamic linker is adding a library, or after loading
 * or attaching to a process. We zip through the list and add whichever libs 
 * we don't know about yet.
 */
int AddLibs( pid_handle pid, addr_off first_lmap )
{
    struct link_map     lmap;
    addr_off            dbg_lmap;
    int                 count = 0;
    lib_load_info       *lli;

    dbg_lmap = first_lmap;
    while( dbg_lmap ) {
        if( !GetLinkMap( pid, dbg_lmap, &lmap ) ) break;
        lli = FindLib( (addr_off)lmap.l_ld );
        if( lli == NULL ) {
            AddLib( pid, &lmap );
            ++count;
        }
        dbg_lmap = (addr_off)lmap.l_next;
    }
    return( count );
}

/*
 * DelLibs - called when dynamic linker is deleting a library. Unfortunately
 * we don't get told which library, so we just have to zip through our list
 * and remove whichever lib is suddenly missing.
 */
int DelLibs( pid_handle pid, addr_off first_lmap )
{
    int                 count = 0;
    int                 i;
    addr_off            dyn_base;

    for( i = 0; i < ModuleTop; ++i ) {
        dyn_base = moduleInfo[i].dbg_dyn_sect;
        if( dyn_base != 0 ) {
            if( FindLibInLinkMap( pid, first_lmap, dyn_base ) == 0 ) {
                DelLib( dyn_base );
            }
        }
    }
    return( count );
}


/* The dynamic linker breakpoint was hit, meaning that
 * libraries are being loaded or unloaded. */
void ProcessLdBreakpoint( pid_handle pid, addr_off rdebug_va )
{
    static int      ld_state;
    struct r_debug  rdebug;

    ReadMem( pid, &rdebug, rdebug_va, sizeof( rdebug ) );

#if 0
    dbg_print(( "  r_map     = %p", rdebug.r_map ));
    dbg_print(( "  r_brk     = %x", rdebug.r_brk ));
    dbg_print(( "  r_state   = %d", rdebug.r_state ));
    dbg_print(( "  r_ldbase  = %x\n", rdebug.r_ldbase));
    dbg_print(( "  r_ldsomap = %p", rdebug.r_ldsomap ));
    dbg_print(( "  r_rdevent = %d", rdebug.r_rdevent ));
    dbg_print(( "  r_flags = %d\n", rdebug.r_flags ));
#endif

    dbg_print(( "ld breakpoint hit, state is " ));
    switch( rdebug.r_state ) {
    case RT_ADD:
        dbg_print(( "RT_ADD\n" ));
        ld_state = RT_ADD;
        AddLibs( pid, (addr_off)rdebug.r_map );
        break;
    case RT_DELETE:
        dbg_print(( "RT_DELETE\n" ));
        ld_state = RT_DELETE;
        break;
    case RT_CONSISTENT:
        dbg_print(( "RT_CONSISTENT\n" ));
#if 1
        // QNX bug? We never seem to get RT_ADD or RT_DELETE
        AddLibs( pid, (addr_off)rdebug.r_map );
        DelLibs( pid, (addr_off)rdebug.r_map );
#else
        if( ld_state == RT_DELETE )
            DelLibs( pid, (addr_off)rdebug.r_map );
#endif
        ld_state = RT_CONSISTENT;
        break;
    default:
        dbg_print(( "error!\n" ));
        break;
    }
}

/*
 * Map address in image from link-time virtual address to actual linear address as 
 * loaded in memory. For executables, this will in effect return the address unchanged
 * (image base 0x08048100 equals linear 0x08048100), for shared libs this will typically 
 * add the offset from zero (if that is the link time VA) to actual load base.
 */
unsigned ReqMap_addr( void )
{
    map_addr_req    *acc;
    map_addr_ret    *ret;
    lib_load_info   *lli;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->in_addr.offset );
    CONV_LE_16( acc->in_addr.segment );
    CONV_LE_32( acc->handle );
    ret = GetOutPtr( 0 );
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr_off)0;

    if( acc->handle > ModuleTop ) {
        dbg_print(( "ReqMap_addr: Invalid handle passed!\n" ));
        return( sizeof( *ret ) );
    } else {
        lli = &moduleInfo[acc->handle];
    }

    dbg_print(( "ReqMap_addr: addr %0x:%08x in module %d\n", acc->in_addr.segment, 
                (unsigned)acc->in_addr.offset, (unsigned)acc->handle ));
    ret->out_addr.offset = acc->in_addr.offset + lli->offset;
    dbg_print(( "to %08x\n", (unsigned)ret->out_addr.offset ));
    CONV_LE_32( ret->out_addr.offset );
    CONV_LE_16( ret->out_addr.segment );
    CONV_LE_32( ret->lo_bound );
    CONV_LE_32( ret->hi_bound );
    return( sizeof( *ret ) );
}


/*
 * AccGetLibName - get lib name of current module
 */
unsigned ReqGet_lib_name( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    unsigned            i;
    unsigned            ret_len = sizeof( *ret );

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->handle );
    ret  = GetOutPtr( 0 );
    name = GetOutPtr( sizeof( *ret ) );

    ret->handle = 0;    /* debugger won't look for more if handle is zero */

    *name = '\0';

    /* The first slot is reserved for the main executable; also, we always
     * look for the next module, not the one whose handle the debugger
     * passed in (because the debugger passes in the handle we returned
     * in the previous call).
     */
    for( i = acc->handle + 1; i < ModuleTop; ++i ) {
        if( moduleInfo[i].newly_unloaded ) {
            /* Indicate that lib is gone */
            dbg_print(( "(lib unloaded, '%s')\n", moduleInfo[i].filename ));
            moduleInfo[i].newly_unloaded = FALSE;
            ret->handle = i;
            ret_len += 1;
            break;
        } else if( moduleInfo[i].newly_loaded ) {
            strcpy( name, moduleInfo[i].filename );
            dbg_print(( "(lib loaded, '%s')\n", name ));
            moduleInfo[i].newly_loaded = FALSE;
            ret->handle = i;
            ret_len += strlen( name ) + 1;
            break;
        }
    }
    dbg_print(( "ReqGet_lib_name: in handle %ld, out handle %ld, name '%s'\n",
               acc->handle, ret->handle, name ));
    CONV_LE_32( ret->handle );
    return( ret_len );
}

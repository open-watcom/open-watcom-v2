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
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include "trpimp.h"
#include "trpcomm.h"
#include "mad.h"
#include "lnxcomm.h"

typedef struct lli {
    addr_off    offset;
    addr_off    dbg_dyn_sect;
    addr_off    code_size;
    char        newly_unloaded : 1;
    char        newly_loaded : 1;
    char        filename[257]; // TODO: These should really be dynamic!
} lib_load_info;

static lib_load_info    *moduleInfo;
static int              ModuleTop;

/*
 * FindLib - find a shared lib entry in the list
 */
static lib_load_info *FindLib( addr_off dynsection )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].dbg_dyn_sect == dynsection )
            return( &moduleInfo[i] );
    }
    return( NULL );
}

static struct link_map *FindLibInLinkMap( struct link_map *first_lmap, addr_off dyn_base )
{
    struct link_map     lmap;
    struct link_map     *dbg_lmap;

    dbg_lmap = first_lmap;
    while( dbg_lmap != NULL ) {
        if( !GetLinkMap( pid, dbg_lmap, &lmap ) ) break;
        if( (addr_off)lmap.l_ld == dyn_base )
            return( dbg_lmap );
        dbg_lmap = lmap.l_next;
    }
    return( NULL );
}

/*
 * AddProcess - a new process has been created
 */
void AddProcess( void )
{
    lib_load_info       *lli;

    moduleInfo = malloc( sizeof( lib_load_info ) );
    memset( moduleInfo, 0, sizeof( lib_load_info ) );
    ModuleTop = 1;

    lli = &moduleInfo[0];

    lli->offset = 0;    /* Assume that main executable was not relocated */
    lli->filename[0] = '\0';
}

/*
 * AddLib - a new library has loaded
 */
static void AddLib( struct link_map *lmap )
{
    lib_load_info       *lli;

    /* This code is not terribly efficient */
    ModuleTop++;
    lli = malloc( ModuleTop * sizeof( lib_load_info ) );
    memset( lli, 0, ModuleTop * sizeof( lib_load_info ) );
    memcpy( lli, moduleInfo, (ModuleTop - 1) * sizeof( lib_load_info ) );
    free( moduleInfo );
    moduleInfo = lli;
    lli = &moduleInfo[ModuleTop - 1];

    lli->offset = lmap->l_addr;
    lli->dbg_dyn_sect = (addr_off)lmap->l_ld;
    dbg_strcpy( pid, lli->filename, lmap->l_name );
    lli->newly_loaded = true;
    lli->newly_unloaded = false;
    lli->offset = lmap->l_addr;

    Out( "Added library: ofs/dyn = " );
    OutNum( lmap->l_addr );
    Out( "/" );
    OutNum( (addr_off)lmap->l_ld );
    Out( " " );
    Out( lli->filename );
    Out( "\n" );
}

static void DelLib( addr_off dynsection )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].dbg_dyn_sect == dynsection ) {
            Out( "Deleting library " );
            Out( moduleInfo[i].filename );
            Out( "\n" );
            moduleInfo[i].newly_unloaded = true;
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
 * AddInitialLibs - called the first time we can get information
 * about loaded shared libs.
 */
int AddInitialLibs( struct link_map *first_lmap )
{
    struct link_map     lmap;
    struct link_map     *dbg_lmap;
    int                 count = 0;

    dbg_lmap = first_lmap;
    while( dbg_lmap != NULL ) {
        if( !GetLinkMap( pid, dbg_lmap, &lmap ) ) break;
        AddLib( &lmap );
        ++count;
        dbg_lmap = lmap.l_next;
    }
    return( count );
}

/*
 * AddOneLib - called when dynamic linker is adding a library. Unfortunately
 * we don't get told which library, so we just have to zip through the list
 * until we find one we don't know about yet.
 */
int AddOneLib( struct link_map *first_lmap )
{
    struct link_map     lmap;
    struct link_map     *dbg_lmap;
    int                 count = 0;
    lib_load_info       *lli;

    dbg_lmap = first_lmap;
    while( dbg_lmap != NULL ) {
        if( !GetLinkMap( pid, dbg_lmap, &lmap ) ) break;
        lli = FindLib( (addr_off)lmap.l_ld );
        if( lli == NULL ) {
            AddLib( &lmap );
            ++count;
        }
        dbg_lmap = lmap.l_next;
    }
    return( count );
}

/*
 * DelOneLib - called when dynamic linker is deleting a library. Unfortunately
 * we don't get told which library, so we just have to zip through our list
 * until we find out which one is suddenly missing.
 */
int DelOneLib( struct link_map *first_lmap )
{
    int                 count = 0;
    int                 i;
    addr_off            dyn_base;

    for( i = 0; i < ModuleTop; ++i ) {
        dyn_base = moduleInfo[i].dbg_dyn_sect;
        if( dyn_base != 0 ) {
            if( FindLibInLinkMap( first_lmap, dyn_base ) == NULL ) {
                DelLib( dyn_base );
            }
        }
    }
    return( count );
}

/*
 * AccMapAddr - map address in image from link-time virtual address to
 * actual linear address as loaded in memory. For executables, this will
 * in effect return the address unchanged (image base 0x08048100 equals
 * linear 0x08048100), for shared libs this will typically add the offset
 * from zero (link time VA) to actual load base.
 */
trap_retval ReqMap_addr( void )
{
    map_addr_req    *acc;
    map_addr_ret    *ret;
    unsigned long   val;
    lib_load_info   *lli;

    // Note: Info about the process address space is stored in the user register
    //       for GDB, so we can use that to find out what we need to convert these
    //       values in here...
    acc = GetInPtr( 0 );
    CONV_LE_32( acc->in_addr.offset );
    CONV_LE_16( acc->in_addr.segment );
    CONV_LE_32( acc->handle );
    ret = GetOutPtr( 0 );
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr_off)0;
    errno = 0;
    if( (val = ptrace( PTRACE_PEEKUSER, pid, (void *)(offsetof( user_struct, start_code )), 0 )) == -1 ) {
        if( errno ) {
            Out( "ReqMap_addr: first PTRACE_PEEKUSER failed!\n" );
            val = 0;
        }
    }
#ifdef __MIPS__
    // Hack for MIPS - the above call seems to be failing but isn't returning
    // an error; it's possible that the call just isn't valid.
    val = 0;
#endif
    ret->out_addr.offset = acc->in_addr.offset + val;

    if( acc->handle > ModuleTop ) {
        Out( "ReqMap_addr: Invalid handle passed!\n" );
        return( sizeof( *ret ) );
    } else {
        lli = &moduleInfo[acc->handle];
    }

    Out( "ReqMap_addr: addr " );
    OutNum( acc->in_addr.segment );
    Out( ":" );
    OutNum( acc->in_addr.offset );
    Out( " in module " );
    OutNum( acc->handle );
    if( acc->in_addr.segment == MAP_FLAT_DATA_SELECTOR ||
        acc->in_addr.segment == flatDS ) {
        if( (val = ptrace( PTRACE_PEEKUSER, pid, (void *)offsetof( user_struct, u_tsize ), 0 )) == -1 ) {
            if( errno ) {
                Out( "ReqMap_addr: second PTRACE_PEEKUSER failed!\n" );
                val = 0;
            }
        }
        ret->out_addr.offset += val;
        ret->out_addr.segment = flatDS;
    } else {
        ret->out_addr.segment = flatCS;
    }
    ret->out_addr.offset += lli->offset;
    Out( " to " );
    OutNum( ret->out_addr.offset );
    Out( "\n" );
    CONV_LE_32( ret->out_addr.offset );
    CONV_LE_16( ret->out_addr.segment );
    CONV_LE_32( ret->lo_bound );
    CONV_LE_32( ret->hi_bound );
    return( sizeof( *ret ) );
}

/*
 * AccGetLibName - get lib name of current module
 */
trap_retval ReqGet_lib_name( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    unsigned            i;
    trap_elen           ret_len;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->handle );
    ret = GetOutPtr( 0 );
    name = GetOutPtr( sizeof( *ret ) );

    ret->handle = 0;
    ret_len = sizeof( *ret );

    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].newly_unloaded ) {
            Out( "(newly unloaded) " );
            ret->handle = i;
            *name = '\0';
            moduleInfo[i].newly_unloaded = false;
            ++ret_len;
            break;
        } else if( moduleInfo[i].newly_loaded ) {
            Out( "(newly loaded) " );
            ret->handle = i;
            strcpy( name, moduleInfo[i].filename );
            moduleInfo[i].newly_loaded = false;
            ret_len += strlen( name ) + 1;
            break;
        }
    }
    Out( "ReqGet_lib_name: in handle " );
    OutNum( acc->handle );
    Out( " out handle " );
    OutNum( ret->handle );
    Out( "\n" );
    CONV_LE_32( ret->handle );
    return( ret_len );
}

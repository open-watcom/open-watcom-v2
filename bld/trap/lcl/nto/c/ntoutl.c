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
* Description:  QNX Neutrino debugger utility functions.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "mad.h"
#include "bool.h"
#include "ntocomm.h"


#ifndef ELF_SIGNATURE
    #define ELF_SIGNATURE   "\177ELF"
#endif

#ifdef DEBUG_OUT
void Out( const char *str )
{
    write( 1, (char *)str, strlen( str ) );
}

void OutNum( unsigned long i )
{
    char    numbuff[16];
    char    *ptr;

    ptr = numbuff+10;
    *--ptr = '\0';
    do {
        *--ptr = ( i % 16 ) + '0';
        if( *ptr > '9' )
            *ptr += 'A' - '9' - 1;
        i /= 16;
    } while( i != 0 );
    Out( ptr );
}
#endif

/* Read a block memory in debuggee's address space */
unsigned ReadMem( pid_handle procfs_fd, void *ptr, addr_off offv, unsigned size )
{
    int     length = 0;
    
    if( lseek( procfs_fd, (off_t)offv, SEEK_SET ) == (off_t)offv ) {
        length = read( procfs_fd, ptr, size );
        if( length < 0 ) {
            dbg_print(( "ReadMem failed\n" ));
            length = 0;
        }
    }
    return( length );
}

/* Write a block of memory in debuggee's address space */
unsigned WriteMem( pid_handle procfs_fd, void *ptr, addr_off offv, unsigned size )
{
    int     length = 0;

    if( lseek( procfs_fd, (off_t)offv, SEEK_SET ) == (off_t)offv ) {
        length = write( procfs_fd, ptr, size );
        if( length < 0 ) {
            dbg_print(( "WriteMem failed\n" ));
            length = 0;
        }
    }
    return( length );
}


/* Obtain address of the dynamic section from the program
 * header. If anything unexpected happens, give up. Note that the
 * address is unrelocated, but usually it will match the actual VA.
 */
addr_off GetDynSection( const char *exe_name )
{
    addr_off    result = 0;
    Elf32_Ehdr  ehdr;
    Elf32_Phdr  phdr;
    int         fd;
    size_t      i;

    fd = open( exe_name, O_RDONLY );
    if( fd < 0 ) {
        return( result );
    }
    if( read( fd, &ehdr, sizeof( ehdr ) ) >= sizeof( ehdr ) &&
        memcmp( ehdr.e_ident, ELF_SIGNATURE, 4 ) == 0 &&
        ehdr.e_phoff != 0 &&
        ehdr.e_phentsize >= sizeof( phdr ) &&
        lseek( fd, ehdr.e_phoff, SEEK_SET ) == ehdr.e_phoff ) {
        for( i = 0; i < ehdr.e_phnum; i++ ) {
            if( read( fd, &phdr, sizeof phdr ) < sizeof( phdr ) )
                break;
            if( phdr.p_type == PT_DYNAMIC ) {
                result = phdr.p_vaddr;
                break;
            }
            if( lseek( fd, ehdr.e_phentsize - sizeof( phdr ), SEEK_CUR ) < 0 )
                break;
        }
    }
    close( fd );
    dbg_print(( "DynSection at: %08x\n", (unsigned)result ));
    return( result );
}

/* Read dynamic linker rendezvous structure from debuggee's address space
 * and return its offset, as well as offset of the dynamic linker breakpoint.
 * Note that it is perfectly valid for this function to fail - that will 
 * happen if the debuggee is statically linked.
 */
int GetLdInfo( pid_handle pid, addr_off dynsec_off, addr_off *rdebug_off, addr_off *ld_bp_off )
{
    Elf32_Dyn       loc_dyn;
    struct r_debug  rdebug;
    addr_off        rdebug_ptr = 0;
    unsigned        read_len;

    if( dynsec_off == 0 ) {
        dbg_print(( "GetLdInfo: dynamic section not available\n" ));
        return( FALSE );
    }
    read_len = sizeof( loc_dyn );
    if( ReadMem( pid, &loc_dyn, dynsec_off, read_len ) != read_len ) {
        dbg_print(( "GetLdInfo: failed to copy first dynamic entry\n" ));
        return( FALSE );
    }
    while( loc_dyn.d_tag != DT_NULL ) {
        if( loc_dyn.d_tag == DT_DEBUG ) {
            rdebug_ptr = loc_dyn.d_un.d_ptr;
            dbg_print(( "GetLdInfo: DT_DEBUG entry found (%08lx)\n", rdebug_ptr ));
            break;
        }
        dynsec_off += sizeof( Elf32_Dyn );
        if( ReadMem( pid, &loc_dyn, dynsec_off, read_len ) != read_len ) {
            dbg_print(( "GetLdInfo: failed to copy dynamic entry\n" ));
            return( FALSE );
        }
    }
    if( rdebug_ptr == 0 ) {
        dbg_print(( "GetLdInfo: DT_DEBUG entry not found or not set\n" ));
        return( FALSE );
    }
    read_len = sizeof( rdebug );
    if( ReadMem( pid, &rdebug, rdebug_ptr, read_len ) != read_len ) {
        dbg_print(( "GetLdInfo: failed to copy r_debug struct\n" ));
        return( FALSE );
    }
    *rdebug_off = rdebug_ptr;
    *ld_bp_off  = rdebug.r_brk;
    dbg_print(( "GetLdInfo: dynamic linker rendezvous structure found\n" ));
    return( TRUE );
}

/* Like strcpy() but source string is in debuggee's address space. Not
 * very efficient, use sparingly!
 */
char *dbg_strcpy( pid_handle pid, char *s1, const char *s2 )
{
    char    *dst = s1;
    char    c;

    do {
        if( ReadMem( pid, &c, (addr48_off)s2, 1 ) != 1 ) {
            dbg_print(( "dbg_strcpy: failed at %08x\n", (unsigned)s2 ));
            return( NULL );
        }
        *dst++ = c;
        ++s2;
    } while( c );

    return( s1 );
}

/* Copy a link map struct from debuggee address space to memory
 * provided by caller.
 */
int GetLinkMap( pid_handle pid, addr_off dbg_lmap_off, struct link_map *local_lmap )
{
    unsigned    read_len;

    read_len = sizeof( *local_lmap );
    if( ReadMem( pid, local_lmap, dbg_lmap_off, read_len ) != read_len ) {
        dbg_print(( "GetLinkMap: failed to copy link_map struct at %08x\n", (unsigned)dbg_lmap_off ));
        return( FALSE );
    }
    return( TRUE );
}


/* Map an address to a link-time VA (map offset) in the module that
 * it belongs to. Preferably used on a base address of a module.
 */
int MapAddrToLinkVA( pid_handle pid, addr_off addr, addr_off *map_addr )
{
    procfs_debuginfo    dbg_info;

    dbg_info.vaddr = addr;
    if( devctl( pid, DCMD_PROC_MAPDEBUG, &dbg_info, sizeof( dbg_info ), 0 ) != EOK ) {
        dbg_print(( "MapAddrToLinkVA: failed to map address\n" ));
        return( FALSE );
    }
    *map_addr = dbg_info.vaddr;
    return( TRUE );
}


/* Break up program arguments passed in as a single string into
 * individual components. Useful for passing argv style array to
 * exec().
 */
int SplitParms( char *p, char *args[], unsigned len )
{
    int     i;
    char    endc;

    i = 0;
    if( len == 1 ) goto done;
    for( ;; ) {
        for( ;; ) {
            if( len == 0 ) goto done;
            if( *p != ' ' && *p != '\t' ) break;
            ++p;
            --len;
        }
        if( len == 0 ) goto done;
        if( *p == '"' ) {
            --len;
            ++p;
            endc = '"';
        } else {
            endc = ' ';
        }
        if( args != NULL ) {
            args[i] = p;
        }
        ++i;
        for( ;; ) {
            if( len == 0 ) goto done;
            if( *p == endc || *p == '\0' || (endc == ' ' && *p == '\t') ) {
                if( args != NULL ) {
                    *p = '\0';  //NYI: not a good idea, should make a copy
                }
                ++p;
                --len;
                if( len == 0 ) goto done;
                break;
            }
            ++p;
            --len;
        }
    }
done:
    return( i );
}

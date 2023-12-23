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
* Description:  Linux debugger utility functions.
*
****************************************************************************/

#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include "bool.h"
#include "digtypes.h"
#include "lnxcomm.h"

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

size_t WriteMemory( pid_t pid, addr_off offv, void *data, size_t size )
{
    size_t  count;

    /* Write the process memory 32-bits at a time. Kind of silly that
     * Linux does not have an extended ptrace call to read and write
     * blocks of data from the debuggee process, but this is what we
     * need to do for now.
     * ... but reads can be done from /proc/pid/mem: see below.
     */
    for( count = size; count >= sizeof( u_long ); count -= sizeof( u_long ) ) {
        if( ptrace( PTRACE_POKETEXT, pid, (void *)offv, (void *)(*(u_long *)data) ) != 0 )
            return( size - count );
        data = (char *)data + sizeof( u_long );
        offv += sizeof( u_long );
    }

    /* Now handle last partial write if neccesary. Note that we first
     * must read the full 32-bit value, then just change the section
     * we want to update.
     */
    if( count ) {
        u_long      val;
        int         i;

        errno = 0;
        if( (val = ptrace( PTRACE_PEEKTEXT, pid, (void *)offv, &val )) == -1 ) {
            if( errno ) {
                return( size - count );
            }
        }
#if DEBUG_WRITEMEM
        Out( "WriteMemory:" );
        OutNum( val );
        Out( "\n" );
#endif
        /* we have to maintain byte order here! */
        for( i = 0; i < count; ++i ) {
            ((char *)&val)[i] = ((char *)data)[i];
        }
#if DEBUG_WRITEMEM
        Out( "WriteMemory:" );
        OutNum( val );
        Out( "\n" );
#endif
        if( ptrace( PTRACE_POKETEXT, pid, (void *)offv, (void *)val ) != 0 ) {
            return( size - count );
        }
    }

    return( size );
}

size_t ReadMemory( pid_t pid, addr_off offv, void *data, size_t size )
{
    size_t  count;

    if( size > 16 ) {
        char    procpidmem[6+20+4+1];
        int     fd;
        loff_t  res;

        snprintf( procpidmem, sizeof( procpidmem ), "/proc/%d/mem", pid );
        fd = open( procpidmem, O_RDONLY );
        if( fd != -1 ) {
#ifdef __WATCOMC__
            _llseek( fd, 0, offv, &res, SEEK_SET );
#else
            res = lseek64( fd, offv, SEEK_SET );
#endif
            count = -1;
            if( res != -1 )
                count = read( fd, data, size );
            close( fd );
            if( count != -1 ) {
                return( count );
            }
        }
    }

    /* Read the process memory 32-bits at a time */
    for( count = size; count >= sizeof( u_long ); count -= sizeof( u_long ) ) {
        u_long  val;

        errno = 0;
        if( (val = ptrace( PTRACE_PEEKTEXT, pid, (void *)offv, &val )) == -1 ) {
            if( errno ) {
                return( size - count );
            }
        }
        *(u_long *)data = val;
        data = (char *)data + sizeof( u_long );
        offv += sizeof( u_long );
    }

    /* Now handle last partial read if neccesary */
    if( count ) {
        u_long  val;
        int     i;

        errno = 0;
        if( (val = ptrace( PTRACE_PEEKTEXT, pid, (void *)offv, &val )) == -1 ) {
            if( errno ) {
                return( size - count );
            }
        }
        for( i = 0; i < count; ++i ) {
            ((char *)data)[i] = ((char *)&val)[i];
        }
        count = 0;
    }
    return( size - count );
}

Elf32_Dyn *GetDebuggeeDynSection( const char *exe_name )
{
    Elf32_Dyn   *result;
    Elf32_Ehdr  ehdr;
    Elf32_Phdr  phdr;
    int         fd;
    size_t      i;

    result = NULL;
    fd = open( exe_name, O_RDONLY );
    if( fd < 0 )
        return( result );
    /* Obtain the address of the dynamic section from the program
     * header. If anything unexpected happens, give up
     */
    if( read( fd, &ehdr, sizeof( ehdr ) ) >= sizeof( ehdr ) &&
        memcmp( ehdr.e_ident, ELF_SIGNATURE, 4 ) == 0 &&
        ehdr.e_phoff != 0 &&
        ehdr.e_phentsize >= sizeof( phdr ) &&
        lseek( fd, ehdr.e_phoff, SEEK_SET ) == ehdr.e_phoff ) {
        for( i = 0; i < ehdr.e_phnum; i++ ) {
            if( read( fd, &phdr, sizeof phdr ) < sizeof( phdr ) )
                break;
            if( phdr.p_type == PT_DYNAMIC ) {
                result = (Elf32_Dyn *)phdr.p_vaddr;
                break;
            }
            if( lseek( fd, ehdr.e_phentsize - sizeof( phdr ), SEEK_CUR ) == -1L ) {
                break;
            }
        }
    }
    close( fd );
    Out( "DynSection at: " );
    OutNum( (size_t)result );
    Out( "\n" );
    return( result );
}

/* Copy dynamic linker rendezvous structure from debuggee's address space
 * to memory provided by caller. Note that it is perfectly valid for this
 * function to fail - that will happen if the debuggee is statically linked.
 */
int Get_ld_info( pid_t pid, Elf32_Dyn *dbg_dyn, struct r_debug *debug_ptr, struct r_debug **dbg_rdebug_ptr )
{
    Elf32_Dyn       loc_dyn;
    struct r_debug  *rdebug = NULL;

    if( dbg_dyn == NULL ) {
        Out( "Get_ld_info: dynamic section not available\n" );
        return( false );
    }
    if( ReadMemory( pid, (addr_off)dbg_dyn, &loc_dyn, sizeof( loc_dyn ) ) != sizeof( loc_dyn ) ) {
        Out( "Get_ld_info: failed to copy first dynamic entry\n" );
        return( false );
    }
    while( loc_dyn.d_tag != DT_NULL ) {
        if( loc_dyn.d_tag == DT_DEBUG ) {
            rdebug = (struct r_debug *)loc_dyn.d_un.d_ptr;
            Out( "Get_ld_info: DT_DEBUG entry found\n" );
            break;
        }
        dbg_dyn++;
        if( ReadMemory( pid, (addr_off)dbg_dyn, &loc_dyn, sizeof( loc_dyn ) ) != sizeof( loc_dyn ) ) {
            Out( "Get_ld_info: failed to copy dynamic entry\n" );
            return( false );
        }
    }
    if( rdebug == NULL ) {
        Out( "Get_ld_info: DT_DEBUG entry not found or not set\n" );
        return( false );
    }
    if( ReadMemory( pid, (addr_off)rdebug, debug_ptr, sizeof( *debug_ptr ) ) != sizeof( *debug_ptr ) ) {
        Out( "Get_ld_info: failed to copy r_debug struct\n" );
        return( false );
    }
    *dbg_rdebug_ptr = rdebug;
    Out( "Get_ld_info: dynamic linker rendezvous structure found\n" );
    return( true );
}

/* Like strcpy() but source string is in debuggee's address space. Not
 * very efficient, use sparingly!
 */
char *dbg_strcpy( pid_t pid, char *s1, const char *s2 )
{
    char    *dst = s1;
    char    c;

    do {
        if( ReadMemory( pid, (addr_off)s2, &c, 1 ) != 1 ) {
            Out( "dbg_strcpy: failed at " );
            OutNum( (addr_off)s2 );
            Out( "\n" );
            return( NULL );
        }
        *dst++ = c;
        ++s2;
    } while( c != '\0' );

    return( s1 );
}

/* Copy a link map struct from debuggee address space to memory
 * provided by caller.
 */
int GetLinkMap( pid_t pid, struct link_map *dbg_lmap, struct link_map *local_lmap )
{
    if( ReadMemory( pid, (addr_off)dbg_lmap, local_lmap, sizeof( *local_lmap ) ) != sizeof( *local_lmap ) ) {
        Out( "GetLinkMap: failed to copy link_map struct at " );
        OutNum( (addr_off)dbg_lmap );
        Out( "\n" );
        return( false );
    }
    return( true );
}

int SplitParms( char *p, const char **args, size_t len )
/**************************************************************
 * Break up program arguments passed in as a single string into
 * individual components. Useful for passing argv style array to
 * exec().
 */
{
    int     i;
    char    endc;

    i = 0;
    if( len == 1 )
        goto done;
    for( ;; ) {
        for( ;; ) {
            if( len == 0 )
                goto done;
            if( *p != ' ' && *p != '\t' )
                break;
            ++p;
            --len;
        }
        if( len == 0 )
            goto done;
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
            if( len == 0 )
                goto done;
            if( *p == endc || *p == '\0' || ( endc == ' ' && *p == '\t' ) ) {
                /*
                 * if output array is not specified then source string
                 * is not changed and it calculates number of parameters only
                 * as soon as output array is specified then source is modified
                 */
                if( args != NULL ) {
                    *p = '\0';
                }
                ++p;
                --len;
                if( len == 0 )
                    goto done;
                break;
            }
            ++p;
            --len;
        }
    }
done:
    return( i );
}

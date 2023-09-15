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
* Description:  Linux core file debug support.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
//#include <sys/stat.h>
#include "digcpu.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "exeelf.h"
#include "lnxcomm.h"
#include "lnxpath.h"
//#include "miscx87.h"
#include "mad.h"
#include "madregs.h"


#define NO_FILE         (-1)

enum {
    MH_NONE,
    MH_DEBUGGEE,
    MH_SLIB,
    MH_PROC
};

struct user_regs_struct {
    unsigned_32     ebx;
    unsigned_32     ecx;
    unsigned_32     edx;
    unsigned_32     esi;
    unsigned_32     edi;
    unsigned_32     ebp;
    unsigned_32     eax;
    unsigned_16     ds;
    unsigned_16     __ds;
    unsigned_16     es;
    unsigned_16     __es;
    unsigned_16     fs;
    unsigned_16     __fs;
    unsigned_16     gs;
    unsigned_16     __gs;
    unsigned_32     orig_eax;
    unsigned_32     eip;
    unsigned_16     cs;
    unsigned_16     __cs;
    unsigned_32     eflags;
    unsigned_32     esp;
    unsigned_16     ss;
    unsigned_16     __ss;
};


struct {
    unsigned                    loaded                  : 1;
    unsigned                    ignore_timestamp        : 1;
    unsigned                    read_gdts               : 1;
    unsigned                    dbg32                   : 1;
    unsigned                    mapping_shared          : 1;
    unsigned                    swap_bytes              : 1;
    int                         err_no;
    int                         fd;
    Elf32_Ehdr                  *e_hdr;
    Elf32_Phdr                  *e_phdr;
//    struct      _dumper_hdr     hdr;
} core_info;

#if 0
static void Out( char *str )
{
    write( 1, str, strlen( str ) );
}

static void OutNum( unsigned i )
{
    char numbuff[10];
    char *ptr;

    ptr = numbuff+10;
    *--ptr = '\0';
    do {
        *--ptr = ( i % 10 ) + '0';
        i /= 10;
    } while( i != 0 );
    Out( ptr );
}
#endif


/* Initially it was thought that we could use the ORL to deal with ELF core
 * files. Unfortunately ELF core dumps may not have any sections (just segments),
 * and the ORL is entirely unsuitable for dealing with that as it is all based
 * around the concept of sections. Therefore we have simple ELF parsing code
 * right here - we'd only need a very very small subset of ORL anyway.
 */

/* Read ELF header and check if it's roughly what we're expecting */
static int elf_read_hdr( int fd, Elf32_Ehdr *e_hdr )
{
//    Elf32_Phdr  phdr;
//    size_t      i;
    int     result = false;

    lseek( fd, 0, SEEK_SET );
    if( read( fd, e_hdr, sizeof( *e_hdr ) ) >= sizeof( *e_hdr ) &&
        memcmp( e_hdr->e_ident, ELF_SIGNATURE, 4 ) == 0 &&
        e_hdr->e_ident[EI_CLASS] == ELFCLASS32) {
#ifdef __BIG_ENDIAN__
        if( e_hdr->e_ident[EI_DATA] == ELFDATA2LSB )
            core_info.swap_bytes = true;
#else
        if( e_hdr->e_ident[EI_DATA] == ELFDATA2MSB )
            core_info.swap_bytes = true;
#endif
        if( core_info.swap_bytes ) {
            SWAP_16( e_hdr->e_type );
            SWAP_16( e_hdr->e_machine );
            SWAP_32( e_hdr->e_version );
            SWAP_32( e_hdr->e_entry );
            SWAP_32( e_hdr->e_phoff );
            SWAP_32( e_hdr->e_shoff );
            SWAP_32( e_hdr->e_flags );
            SWAP_16( e_hdr->e_ehsize );
            SWAP_16( e_hdr->e_phentsize );
            SWAP_16( e_hdr->e_phnum );
            SWAP_16( e_hdr->e_shentsize );
            SWAP_16( e_hdr->e_shnum );
            SWAP_16( e_hdr->e_shstrndx );
        }
        if( e_hdr->e_phoff != 0 && e_hdr->e_phentsize >= sizeof( Elf32_Phdr ) ) {
            result = true;
        }
    }
    return( result );
}

/* Read ELF program headers */
static int elf_read_phdr( int fd, Elf32_Ehdr *e_hdr, Elf32_Phdr **pp_hdr )
{
    Elf32_Phdr      *e_phdr;
    int             i;
    int             result = false;

    *pp_hdr = malloc( sizeof( *e_phdr ) * e_hdr->e_phnum );
    if( *pp_hdr != NULL ) {
        int         error = false;

        e_phdr = *pp_hdr;
        if( lseek( fd, e_hdr->e_phoff, SEEK_SET ) == e_hdr->e_phoff ) {
            for( i = 0; i < e_hdr->e_phnum; i++ ) {
                if( read( fd, e_phdr, sizeof( *e_phdr ) ) < sizeof( *e_phdr ) ) {
                    error = true;
                    break;
                }
                /* Skip any extra bytes that might be present */
                if( lseek( fd, e_hdr->e_phentsize - sizeof( *e_phdr ), SEEK_CUR ) == -1L ) {
                    error = true;
                    break;
                }
                if( core_info.swap_bytes ) {
                    SWAP_32( e_phdr->p_type );
                    SWAP_32( e_phdr->p_offset );
                    SWAP_32( e_phdr->p_vaddr );
                    SWAP_32( e_phdr->p_paddr );
                    SWAP_32( e_phdr->p_filesz );
                    SWAP_32( e_phdr->p_memsz );
                    SWAP_32( e_phdr->p_flags );
                    SWAP_32( e_phdr->p_align );
                }
                e_phdr++;
            }
        }
        if( !error ) {
            result = true;
        }
    }
    return( result );
}

trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->arch = DIG_ARCH_X86;
    ret->os  = DIG_OS_LINUX;
    ret->osmajor = 1;
    ret->osminor = 0;
    ret->fpu = X86_387;
    ret->cpu = X86_686;
    ret->huge_shift = 3;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Map_addr )( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;
    unsigned            index;
    unsigned            seg;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)9;

    seg = acc->in_addr.segment;
    switch( seg ) {
    case MAP_FLAT_CODE_SELECTOR:
        seg = 0x04;
        break;
    case MAP_FLAT_DATA_SELECTOR:
        seg = 0x0c;
        break;
    }

    ret->out_addr.offset = acc->in_addr.offset;
    switch( acc->mod_handle ) {
    case MH_DEBUGGEE:
        if( acc->in_addr.segment == MAP_FLAT_DATA_SELECTOR ) {
            index = 0;
        } else {
            index = seg >> 3;
        }
#if 0
        if( core_info.loaded ) {
            ret->out_addr.offset += core_info.segs[index].mem_off;
        }
#endif
        break;
    case MH_SLIB:
#if 0
        slib = GetSLibTable( core_info.dbg32 );
        seg += slib.segment - 4;
#endif
        break;
    case MH_PROC:
        seg += 0xE0 - 4;
        break;
    }
    ret->out_addr.segment = 0;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Checksum_mem )( void )
{
    checksum_mem_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->result = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req        *acc;
    void                *ret;
    unsigned            i;
    size_t              len;
    Elf32_Phdr          *e_phdr;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( !core_info.loaded ) {
        return( 0 );
    }
    len = acc->len;
    e_phdr = core_info.e_phdr;
    for( i = 0; i < core_info.e_hdr->e_phnum ; ++i, ++e_phdr ) {
        int             read_len;
        Elf32_Off       rel_ofs;    // Relative offset within segment

        if( e_phdr->p_type != PT_LOAD )
            continue;
        if( (acc->mem_addr.offset < e_phdr->p_vaddr) ||
            (acc->mem_addr.offset > e_phdr->p_vaddr + e_phdr->p_memsz - 1) ) {
            continue;
        }
        rel_ofs  = acc->mem_addr.offset - e_phdr->p_vaddr;
        /* Adjust length if pointing past end of segment */
        if( (acc->mem_addr.offset + len) > (e_phdr->p_vaddr + e_phdr->p_memsz) ) {
            len = rel_ofs + len - e_phdr->p_memsz;
        }
        read_len = len;
        /* Adjust length to read from file if p_memsz > p_filesz */
        if( (acc->mem_addr.offset + len) > (e_phdr->p_vaddr + e_phdr->p_filesz) ) {
            read_len = e_phdr->p_filesz - rel_ofs;
            if( read_len < 0 ) {
                read_len = 0;
            }
        }
        if( len != 0 ) {
            if( read_len != 0 ) {
                lseek( core_info.fd, e_phdr->p_offset + rel_ofs, SEEK_SET );
                read_len = read( core_info.fd, ret, read_len );
                if( read_len == -1 ) {
                    return( 0 );
                }
            }
            if( read_len < len ) {
                memset( (unsigned_8 *)ret + read_len, 0, len - read_len );
            }
        }
        return( len );
    }
    return( 0 );
}


trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Read_io )( void )
{
    return( 0 );
}


trap_retval TRAP_CORE( Write_io )( void )
{
    write_io_ret        *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

static void ReadCPU( struct x86_cpu *r )
{
    struct user_regs_struct     regs;

    memset( r, 0, sizeof( *r ) );
    if( core_info.loaded ) {
// TODO: correctly determine the offset of the register struct in core file
        lseek( core_info.fd, 0x1ec, SEEK_SET );
        if( read( core_info.fd, &regs, sizeof( regs ) ) == sizeof( regs ) ) {
            r->eax = regs.eax;
            r->ebx = regs.ebx;
            r->ecx = regs.ecx;
            r->edx = regs.edx;
            r->esi = regs.esi;
            r->edi = regs.edi;
            r->ebp = regs.ebp;
            r->esp = regs.esp;
            r->eip = regs.eip;
            r->efl = regs.eflags;
            r->cs  = regs.cs;
            r->ds  = regs.ds;
            r->ss  = regs.ss;
            r->es  = regs.es;
            r->fs  = regs.fs;
            r->gs  = regs.gs;
        }
    }
}

static void ReadFPU( struct x86_fpu *r )
{
    memset( r, 0, sizeof( *r ) );
    if( core_info.loaded ) {
#if 0
        memcpy( r, core_info.hdr.x87, sizeof( core_info.hdr.x87 ) );
        if( !core_info.fpu32 ) {
            FPUExpand( r );
        }
#endif
    }
}

trap_retval TRAP_CORE( Read_regs )( void )
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );

    ReadCPU( &mr->x86.cpu );
    ReadFPU( &mr->x86.u.fpu );
    return( sizeof( mr->x86 ) );
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    return( 0 );
}

static int load_core_header( const char *core_name )
{
    int         fd;
    int         result;

    result = false;
    if( core_info.e_hdr == NULL ) {
        core_info.e_hdr = malloc( sizeof( *core_info.e_hdr ) );
        if( core_info.e_hdr == NULL ) {
            return( result );
        }
    }
    fd = open( core_name, O_RDONLY );
    if( fd < 0 )
        return( result );

    core_info.fd = fd;
    if( !elf_read_hdr( fd, core_info.e_hdr ) ) {
        close( fd );
    } else {
        if( elf_read_phdr( fd, core_info.e_hdr, &core_info.e_phdr ) ) {
            result = true;
        }
    }
    return( result );
}

trap_retval TRAP_CORE( Prog_load )( void )
{
    prog_load_req       *acc;
    prog_load_ret       *ret;
    char                *argv;

    core_info.dbg32 = false;
    core_info.mapping_shared = false;
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    argv = GetInPtr( sizeof( *acc ) );
    ret->mod_handle = MH_DEBUGGEE;

    if( argv[0] == '\0' ) {
        ret->task_id = 0;
        ret->err = ENOENT;
        return( sizeof( *ret ) );
    }
    core_info.err_no = 0;
    load_core_header( argv );
    ret->flags = LD_FLAG_IS_STARTED | LD_FLAG_IS_PROT | LD_FLAG_IS_BIG;
    ret->task_id = 123; //core_info.hdr.psdata.pid;
    ret->err = core_info.err_no;
    if( core_info.err_no == 0 ) {
        core_info.loaded = true;
    } else {
        close( core_info.fd );
        core_info.fd = NO_FILE;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;

    if( core_info.loaded ) {
        core_info.loaded = false;
        close( core_info.fd );
        core_info.fd = NO_FILE;
    }
    core_info.mapping_shared = false;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->old = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    set_watch_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;   // OK
    ret->multiplier = USING_DEBUG_REG | 1;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    return( 0 );
}


trap_retval TRAP_CORE( Prog_go )( void )
{
    prog_go_ret *ret;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_TERMINATE;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    return( TRAP_CORE( Prog_go )() );
}


trap_retval TRAP_CORE( Get_message_text )( void )
{
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    err_txt[0] = '\0';
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + 1 );
}

trap_retval TRAP_CORE( Redirect_stdin )( void )
{
    redirect_stdin_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Redirect_stdout )( void )
{
    return( TRAP_CORE( Redirect_stdin )() );
}

trap_retval TRAP_FILE( file_to_fullpath )( void )
{
    unsigned_16                 len;
    char                        *name;
    char                        *fullname;
    unsigned                    save_handle;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    fullname = GetOutPtr( sizeof( *ret ) );
    fullname[0] = '\0';
    len = 0;
    if( acc->file_type != DIG_FILETYPE_EXE || core_info.mapping_shared ) {
        len = FindFilePath( acc->file_type, name, fullname );
    } else {
        save_handle = core_info.fd;
        if( load_core_header( name ) ) {
            // TODO: this should figure out the name of the executable
            // that caused the core dump.
            name = "cdump";
            strcpy( fullname, name );
            len = strlen( fullname );
#if 0
            struct stat     chk;

            name = "/foo/bar"; //core_info.hdr.psdata.un.proc.name;
            if( stat( name, &chk ) != 0 ) {
                /* try it without the node number */
                name += 2;
                while( *name != '/' )
                    ++name;
                if( stat( name, &chk ) != 0 ) {
                    chk.st_mtime = 0;
                }
            }
            if( core_info.ignore_timestamp || chk.st_mtime == core_info.hdr.cmdtime ) {
                len = StrCopyDst( name, fullname ) - fullname;
            }
#endif
            close( core_info.fd );
        }
        core_info.fd = save_handle;
    }
    if( len == 0 ) {
        ret->err = ENOENT;      /* File not found */
    }
    return( sizeof( *ret ) + len + 1 );
}

trap_retval TRAP_CORE( Machine_data )( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    machine_data_spec   *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    if( acc->info_type == X86MD_ADDR_CHARACTERISTICS ) {
        data = GetOutPtr( sizeof( *ret ) );
        data->x86_addr_flags = X86AC_BIG;
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    char                *p;
    size_t              name_maxlen;

    // TODO: we could probably figure out what shared libs were loaded
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    switch( acc->mod_handle ) {
    case MH_NONE:
    case MH_DEBUGGEE:
        ret->mod_handle = MH_SLIB;
        p = "/boot/sys/Slib32";
        break;
    case MH_SLIB:
        ret->mod_handle = MH_PROC;
        p = "/boot/sys/Proc32";
        break;
    default:
        ret->mod_handle = 0;
        return( sizeof( *ret ) );
    }
    name_maxlen = GetTotalSizeOut() - sizeof( *ret ) - 1;
    name = GetOutPtr( sizeof( *ret ) );
    strncpy( name, p, name_maxlen );
    name[name_maxlen] = '\0';
    return( sizeof( *ret ) + strlen( name ) + 1 );
}

#if 0
trap_retval TRAP_THREAD( get_next )( void )
{
    thread_get_next_req *req;
    thread_get_next_ret *ret;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( req->thread == 0 ) {
        ret->thread = core_info.hdr.psdata.pid;
        ret->state = THREAD_THAWED;
    } else {
        ret->thread = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( set )( void )
{
    thread_set_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = core_info.hdr.psdata.pid;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( freeze )( void )
{
    thread_freeze_ret   *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( thaw )( void )
{
    thread_thaw_ret     *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( get_extra )( void )
{
    char                    *ret;

    ret = GetOutPtr( 0 );
    ret[0] = '\0';
    return( strlen( ret ) + 1 );
}
#endif

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version ver;
    char         ch;

    /* unused parameters */ (void)remote;

    core_info.fd = NO_FILE;
    while( (ch = *parms++) != '\0' ) {
        switch( ch ) {
        case 'I':
        case 'i':
            core_info.ignore_timestamp = true;
            break;
        }
    }
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
}

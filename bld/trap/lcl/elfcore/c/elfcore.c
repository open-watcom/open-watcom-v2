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
* Description:  ELF core file debug support.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "trpimp.h"
#include "exeelf.h"
#include "mad.h"
#include "madregs.h"
#include "elfcore.h"


/* This file contains a platform-independent superstructure. All
 * platform and architecture specific details are handled
 * by target-specific "drivers".
 *
 * Note: We do not worry about segments in this trap file since
 * ELF does not support segmented architectures. Makes life easier.
 */

/* Somewhat arbitrary path limit */
#define MAX_FULLPATH_LEN    500

#define ELF_ROUND   (sizeof( Elf32_Word ) - 1)

#define NO_FILE         (-1)

static plat_drv_t   *drivers[] = {
    &Drv_FreeBSD,
    &Drv_Neutrino,
    NULL
};

extern unsigned FindFilePath( int exe, char *name, char *result );

enum {
    MH_NONE,
    MH_DEBUGGEE,
    MH_SLIB,
    MH_PROC
};

struct {
    unsigned            loaded              : 1;
    unsigned            x_loaded            : 1;
    unsigned            ignore_timestamp    : 1;
    unsigned            mapping_shared      : 1;
    unsigned            swap_bytes          : 1;
    int                 err_no;
    int                 fd;         /* core file descriptor */
    Elf32_Ehdr          *c_ehdr;    /* core file ELF header */
    Elf32_Phdr          *c_phdr;    /* core file program header */
    int                 x_fd;       /* executable file descriptor */
    Elf32_Ehdr          *x_ehdr;    /* executable ELF header */
    Elf32_Phdr          *x_phdr;    /* executable program header */
    plat_drv_t          *plat;      /* platform-specific driver */
    void                *ctx;       /* driver-specific context data */
    char                exe_name[MAX_FULLPATH_LEN];
} Core;


/* Read ELF header and check if it's roughly what we're expecting */
int elf_read_hdr( int fd, Elf32_Ehdr *e_hdr )
{
    int     result = FALSE;

    lseek( fd, 0, SEEK_SET );
    if( read( fd, e_hdr, sizeof( *e_hdr ) ) >= sizeof( *e_hdr ) &&
        memcmp( e_hdr->e_ident, ELF_SIGNATURE, 4 ) == 0 &&
        e_hdr->e_ident[EI_CLASS] == ELFCLASS32) {
#ifdef __BIG_ENDIAN__
        if( e_hdr->e_ident[EI_DATA] == ELFDATA2LSB )
            Core.swap_bytes = TRUE;
#else
        if( e_hdr->e_ident[EI_DATA] == ELFDATA2MSB )
            Core.swap_bytes = TRUE;
#endif
        if( Core.swap_bytes ) {
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
            result = TRUE;
        }
    }
    return( result );
}

/* Read ELF program headers */
int elf_read_phdr( int fd, Elf32_Ehdr *e_hdr, Elf32_Phdr **pp_hdr )
{
    Elf32_Phdr      *e_phdr;
    int             i;
    int             result = FALSE;

    *pp_hdr = malloc( sizeof( *e_phdr ) * e_hdr->e_phnum );
    if( *pp_hdr != NULL ) {
        int         error = FALSE;

        e_phdr = *pp_hdr;
        if( lseek( fd, e_hdr->e_phoff, SEEK_SET ) == e_hdr->e_phoff ) {
            for( i = 0; i < e_hdr->e_phnum; i++ ) {
                if( read( fd, e_phdr, sizeof( *e_phdr ) ) < sizeof( *e_phdr ) ) {
                    error = TRUE;
                    break;
                }
                /* Skip any extra bytes that might be present */
                if( lseek( fd, e_hdr->e_phentsize - sizeof( *e_phdr ), SEEK_CUR ) < 0 ) {
                    error = TRUE;
                    break;
                }
                if( Core.swap_bytes ) {
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
            result = TRUE;
        }
    }
    return( result );
}

unsigned ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;
    int                 mad, os, cpu, fpu;

    ret = GetOutPtr( 0 );
    if( Core.loaded && Core.plat->qcfg( Core.ctx, &mad, &os, &cpu, &fpu ) ) {
        ret->sys.mad = mad;
        ret->sys.os  = os;
        ret->sys.cpu = cpu;
        ret->sys.fpu = fpu;
    } else {
        ret->sys.mad = MAD_X86;
        ret->sys.os  = OS_IDUNNO;
        ret->sys.cpu = X86_386;
        ret->sys.fpu = X86_387;
    }
    ret->sys.osmajor = 1;
    ret->sys.osminor = 0;
    ret->sys.huge_shift = 3;    // Not relevant for flat model
    return( sizeof( *ret ) );
}

unsigned ReqMap_addr( void )
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
    switch( acc->handle ) {
    case MH_DEBUGGEE:
        if( acc->in_addr.segment == MAP_FLAT_DATA_SELECTOR ) {
            index = 0;
        } else {
            index = seg >> 3;
        }
#if 0
        if( Core.loaded ) {
            ret->out_addr.offset += Core.segs[ index ].mem_off;
        }
#endif
        break;
    case MH_SLIB:
        break;
    case MH_PROC:
        seg += 0xE0 - 4;
        break;
    }
    ret->out_addr.segment = 0;
    return( sizeof( *ret ) );
}

unsigned ReqChecksum_mem( void )
{
    checksum_mem_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->result = 0;    // Dead programs do not tend to change much
    return( sizeof( *ret ) );
}

/* Read data with given virtual address from an ELF executable */

size_t read_from_elf( int fd, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr,
                      void *buf, addr_off va, size_t len )
{
    int         i;

    for( i = 0; i < ehdr->e_phnum ; ++i, ++phdr ) {
        int             read_len;
        Elf32_Off       rel_ofs;    // Relative offset within segment

        if( phdr->p_type != PT_LOAD ) continue;
        if( (va < phdr->p_vaddr) ||
            (va > phdr->p_vaddr + phdr->p_memsz - 1) ) {
            continue;
        }
        rel_ofs = va - phdr->p_vaddr;
        /* Adjust length if pointing past end of segment */
        if( (va + len) > (phdr->p_vaddr + phdr->p_memsz) ) {
            len = rel_ofs + len - phdr->p_memsz;
        }
        read_len = len;
        /* Adjust length to read from file if p_memsz > p_filesz */
        if( (va + len) > (phdr->p_vaddr + phdr->p_filesz) ) {
            read_len = phdr->p_filesz - rel_ofs;
            if( read_len < 0 )
                read_len = 0;
        }
        if( len != 0 ) {
            if( read_len != 0 ) {
                lseek( fd, phdr->p_offset + rel_ofs, SEEK_SET );
                read_len = read( fd, buf, read_len );
                if( read_len == -1 ) {
                    return( 0 );
                }
            }
            if( read_len < len ) {
                memset( (unsigned_8 *)buf + read_len, 0, len - read_len );
            }
        }
        return( len );
    }
    return( 0 );
}

unsigned ReqRead_mem( void )
{
    read_mem_req        *acc;
    void                *ret;
    unsigned            len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( !Core.loaded ) {
        return( 0 );
    }
    len = read_from_elf( Core.fd, Core.c_ehdr, Core.c_phdr, ret,
                         acc->mem_addr.offset, acc->len );
    /* If we couldn't read the memory from core, try the executable.
     * This is required for FreeBSD core files.
     */
    if( len == 0 && (Core.x_fd != NO_FILE) ) {
        len = read_from_elf( Core.x_fd, Core.x_ehdr, Core.x_phdr, ret,
                             acc->mem_addr.offset, acc->len );
    }
    return( len );
}


/* Functions to write memory and do port I/O don't do anything */

unsigned ReqWrite_mem( void )
{
    write_mem_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}


unsigned ReqRead_io( void )
{
    return( 0 );
}


unsigned ReqWrite_io( void )
{
    write_io_ret        *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

/* Functions to read machine registers must read from core file */

static size_t ReadCPU( mad_registers *r )
{
    size_t      size = 0;

    if( Core.loaded ) {
        size = Core.plat->regs( Core.ctx, r, 0 );
    }
    return( size );
}


static size_t ReadFPU( mad_registers *r )
{
    size_t      size = 0;

    if( Core.loaded ) {
        size = Core.plat->regs( Core.ctx, r, 0 );
    }
    return( size );
}


unsigned ReqRead_cpu( void )
{
    return( ReadCPU( GetOutPtr( 0 ) ) );
}


unsigned ReqRead_fpu( void )
{
    return( ReadFPU( GetOutPtr( 0 ) ) );
}


unsigned ReqRead_regs( void )
{
    mad_registers       *mr;
    int                 size;

    mr = GetOutPtr( 0 );

    size  = ReadCPU( mr );
    size += ReadFPU( mr );
    return( size );
}


/* Functions to write machine registers don't do anything */

unsigned ReqWrite_cpu( void )
{
    return( 0 );
}


unsigned ReqWrite_fpu( void )
{
    return( 0 );
}


unsigned ReqWrite_regs( void )
{
    return( 0 );
}


/* Utility routine to look for a note of given type. If found, will
 * return a pointer to its name in memory that caller must free. The
 * file pointer will be positioned at the beginning of note data.
 */
char *find_note( int fd, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr,
                 bool swap, Elf_Note *note )
{
    int             i;
    int             ntype = note->n_type;
    char            *name = NULL;

    /* Loop over program headers */
    for( i = 0; i < ehdr->e_phnum ; ++i, ++phdr ) {
        off_t           read_len;
        off_t           skip;

        if( phdr->p_type != PT_NOTE ) continue;

        /* We found a note segment, loop over the notes */
        read_len = 0;
        while( read_len < phdr->p_filesz ) {
            lseek( fd, phdr->p_offset + read_len, SEEK_SET );
            if( read( fd, note, sizeof( Elf_Note ) ) != sizeof( Elf_Note ) ) {
                break;
            }
            read_len += sizeof( Elf_Note );
            if( swap ) {
                SWAP_32( note->n_namesz );
                SWAP_32( note->n_descsz );
                SWAP_32( note->n_type );
            }
            if( note->n_type == ntype ) {
                /* Found our note, allocate memory for name and read it */
                name = malloc( note->n_namesz );
                if( name ) {
                    read( fd, name, note->n_namesz );
                    /* Skip over padding so that caller can read note data */
                    skip = ((note->n_namesz + ELF_ROUND) & ~ELF_ROUND) - note->n_namesz;
                    lseek( fd, skip, SEEK_CUR );
                }
                break;
            }
            /* Skip over note contents */
            read_len += (note->n_namesz + ELF_ROUND) & ~ELF_ROUND;
            read_len += (note->n_descsz + ELF_ROUND) & ~ELF_ROUND;
        }
    }
    return( name );
}


static int load_elf_header( const char *core_name, Elf32_Ehdr *ehdr, Elf32_Phdr **pphdr )
{
    int         fd;

    fd = open( core_name, O_RDONLY | O_BINARY );
    if( fd < 0 ) {
        return( NO_FILE );
    }
    if( !elf_read_hdr( fd, ehdr ) ) {
        close( fd );
        fd = NO_FILE;
    } else {
        if( !elf_read_phdr( fd, ehdr, pphdr ) ) {
            close( fd );
            fd = NO_FILE;
        }
    }
    return( fd );
}


static int init_platform_driver( int fd, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr )
{
    void        *ctx;
    int         result = FALSE;
    plat_drv_t  **drv;

    for( drv = drivers; *drv; ++drv ) {
        ctx = (*drv)->init( fd, ehdr, phdr );
        if( ctx ) {
            Core.plat = *drv;
            Core.ctx = ctx;
            result = TRUE;
            break;
        }
    }
    return( result );
}


static void close_platform_driver( void )
{
    if( Core.plat ) {
        Core.plat->done( Core.ctx );
        Core.ctx = NULL;
        Core.plat = NULL;
    }
}


unsigned ReqProg_load( void )
{
    prog_load_req       *acc;
    prog_load_ret       *ret;
    char                *argv;

    Core.mapping_shared = FALSE;
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    argv = GetInPtr( sizeof( *acc ) );
    ret->mod_handle = MH_DEBUGGEE;
    ret->task_id = 0;

    if( argv[0] == '\0' ) {
        ret->err = ENOENT;
        return( sizeof( *ret ) );
    }
    Core.err_no = 0;
    Core.fd = load_elf_header( argv, Core.c_ehdr, &Core.c_phdr );
    if( (Core.fd == NO_FILE) || !init_platform_driver( Core.fd, Core.c_ehdr, Core.c_phdr ) ) {
        ret->err = ENOENT;
        return( sizeof( *ret ) );
    }
    ret->flags = LD_FLAG_IS_STARTED | LD_FLAG_IS_PROT;
    /* Tell debugger to ignore segment values when comparing pointers */
    ret->flags |= LD_FLAG_IS_BIG | LD_FLAG_IGNORE_SEGMENTS;
    ret->err = Core.err_no;
    ret->task_id = Core.plat->qpid( Core.ctx );
    if( Core.err_no == 0 ) {
        size_t      len;

        Core.loaded = TRUE;
        /* If we have core, try setting up the executable file */
        len = Core.plat->name( Core.ctx, Core.exe_name, MAX_FULLPATH_LEN );
        if( len ) {
            Core.x_fd = load_elf_header( Core.exe_name, Core.x_ehdr, &Core.x_phdr );
        }
    } else {
        close( Core.fd );
        Core.fd = NO_FILE;
    }
    return( sizeof( *ret ) );
}

unsigned ReqProg_kill( void )
{
    prog_kill_ret       *ret;

    if( Core.loaded ) {
        Core.loaded = FALSE;
        close_platform_driver();
        close( Core.fd );
        if( Core.x_fd != NO_FILE ) {
            close( Core.x_fd );
            Core.x_fd = NO_FILE;
        }
        if( Core.c_phdr ) free( Core.c_phdr );
        if( Core.x_phdr ) free( Core.x_phdr );
        Core.fd = NO_FILE;
    }
    Core.mapping_shared = FALSE;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

unsigned ReqSet_break( void )
{
    set_break_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->old = 0;
    return( sizeof( *ret ) );
}

unsigned ReqClear_break( void )
{
    return( 0 );
}

unsigned ReqSet_watch( void )
{
    set_watch_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->multiplier = USING_DEBUG_REG | 1;
    return( sizeof( *ret ) );
}

unsigned ReqClear_watch( void )
{
    return( 0 );
}


unsigned ReqProg_go( void )
{
    prog_go_ret     *ret;

    ret = GetOutPtr( 0 );
    /* Say the process is terminated, which will prevent
     * the debugger from trying anything funny.
     */
    ret->conditions = COND_TERMINATE;
    return( sizeof( *ret ) );
}

unsigned ReqProg_step( void )
{
    return( ReqProg_go() );
}


unsigned ReqGet_message_text( void )
{
    get_message_text_ret    *ret;
    char                    *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof( *ret ) );
    err_txt[0] = '\0';
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + 1 );
}

unsigned ReqRedirect_stdin( void )
{
    redirect_stdin_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = 1;
    return( sizeof( *ret ) );
}

unsigned ReqRedirect_stdout( void )
{
    return( ReqRedirect_stdin() );
}

unsigned ReqFile_string_to_fullpath( void )
{
    unsigned_16                 len;
    char                        *name;
    char                        *fullname;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;
    int                         fd;
    Elf32_Ehdr                  ehdr;
    Elf32_Phdr                  *phdr;


    acc  = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret  = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    fullname[0] = '\0';
    len = 0;
    if( acc->file_type != TF_TYPE_EXE ) {
        len = FindFilePath( FALSE, name, fullname );
    } else if( Core.mapping_shared ) {
        len = FindFilePath( TRUE, name, fullname );
    } else {
        fd = load_elf_header( name, &ehdr, &phdr );
        if( (fd != NO_FILE) && init_platform_driver( fd, &ehdr, phdr ) ) {
            len = Core.plat->name( Core.ctx, fullname, MAX_FULLPATH_LEN );
            if( !len ) {
                strcpy( fullname, "unknown" );
                len = strlen( fullname );
            } else {
#if 0
                struct stat     chk;

                name = "/foo/bar"; //Core.hdr.psdata.un.proc.name;
                if( stat( name, &chk ) != 0 ) {
                    chk.st_mtime = 0;
                }
                if( Core.ignore_timestamp || chk.st_mtime == Core.hdr.cmdtime ) {
                    len = StrCopy( name, fullname ) - fullname;
                } else {
                    /* Executable and core file timestaps don't match */
                    len = 0;
                }
#endif
            }
            close( fd );
        }
    }
    if( len == 0 ) {
        ret->err = ENOENT;      /* File not found */
    } else {
        ret->err = 0;
    }
    return( sizeof( *ret ) + len + 1 );
}

unsigned ReqAddr_info( void )
{
    addr_info_req       *acc;
    addr_info_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->is_big = TRUE;
    return( sizeof( *ret ) );
}

unsigned ReqMachine_data( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    unsigned_8          *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    *data = X86AC_BIG;
    return( sizeof( *ret ) + sizeof( *data ) );
}

unsigned ReqGet_lib_name( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;

    // TODO: we ought to figure out what shared libs were loaded
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    name = GetOutPtr( sizeof( *ret ) );
    switch( acc->handle ) {
    case MH_NONE:
    case MH_DEBUGGEE:
        ret->handle = MH_SLIB;
        strcpy( name, "/boot/sys/Slib32" );
        break;
    case MH_SLIB:
        ret->handle = MH_PROC;
        strcpy( name, "/boot/sys/Proc32" );
        break;
    default:
        ret->handle = MH_NONE;
        name[0] = '\0';
        break;
    }
    return( sizeof( *ret ) + 1 + strlen( name ) );
}

#if 0
unsigned ReqThread_get_next( void )
{
    thread_get_next_req     *req;
    thread_get_next_ret     *ret;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( req->thread == 0 ) {
        ret->thread = Core.hdr.psdata.pid;
        ret->state = THREAD_THAWED;
    } else {
        ret->thread = 0;
    }
    return( sizeof( *ret ) );
}

unsigned ReqThread_set( void )
{
    thread_set_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = Core.hdr.psdata.pid;
    return( sizeof( *ret ) );
}

unsigned ReqThread_freeze( void )
{
    thread_freeze_ret   *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

unsigned ReqThread_thaw( void )
{
    thread_thaw_ret     *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

unsigned ReqThread_get_extra( void )
{
    char                 *ret;

    ret = GetOutPtr( 0 );
    ret[0] = '\0';
    return( strlen( ret ) + 1 );
}
#endif


unsigned ReqGet_err_text( void )
{
    get_err_text_req    *acc;
    char                *err_txt;

    // TODO: get platform specific messages for signals
    err_txt = GetOutPtr( 0 );
    acc = GetInPtr( 0 );
    strcpy( err_txt, "Unknown error" );
    return( strlen( err_txt ) + 1 );
}


trap_version TRAPENTRY TrapInit( char *parm, char *err, bool remote )
{
    trap_version    ver;

    remote = remote;
    Core.fd   = NO_FILE;
    Core.x_fd = NO_FILE;
    Core.c_ehdr = malloc( sizeof( Elf32_Ehdr ) );
    Core.x_ehdr = malloc( sizeof( Elf32_Ehdr ) );
    if( parm != NULL ) {
        while( *parm != '\0' ) {
            switch( *parm ) {
            case 'I':
            case 'i':
                Core.ignore_timestamp = TRUE;
                break;
            }
            ++parm;
        }
    }
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    return( ver );
}


void TRAPENTRY TrapFini( void )
{
    if( Core.c_ehdr ) free( Core.c_ehdr );
    if( Core.x_ehdr ) free( Core.x_ehdr );
}

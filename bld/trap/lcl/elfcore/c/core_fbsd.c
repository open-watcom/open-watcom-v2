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
* Description:  FreeBSD ELF core file driver.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "exeelf.h"
#include "mad.h"
#include "madregs.h"
#include "elfcore.h"
#include "core_fbsd.h"


typedef struct {
    int         fd;
    bool        swap;
    Elf32_Ehdr  *e_hdr;
    Elf32_Phdr  *p_hdr;
} ctx_freebsd;


/* Initializes the platform-specific driver.
 *
 * Parameters:
 *  fd      - core file descriptor
 *  ehdr    - core file ELF header
 *  phdr    - core file ELF program header
 *
 * Returns:
 *  pointer to driver context or NULL if core file not recognized
 */
static void *freebsd_init( int fd, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr )
{
    ctx_freebsd     *ctx;

    /* See if the core file looks like something we understand */
    if( ehdr->e_ident[EI_OSABI] != ELFOSABI_FREEBSD ) {
        return( NULL );
    }

    ctx = malloc( sizeof( ctx_freebsd ) );
    if( ctx ) {
        ctx->fd    = fd;
        ctx->e_hdr = ehdr;
        ctx->p_hdr = phdr;
        ctx->swap  = FALSE;
        if( ehdr->e_ident[EI_DATA] != NATIVE_ELF_ORDER ) {
            ctx->swap = TRUE;
        }
    }
    return( ctx );
}


/* Attempts to determine the name of the executable from the core
 * file. The executable is likely to be required for determining
 * code segment data.
 *
 * Parameters:
 *  ctx     - driver context
 *  name    - pointer to output buffer
 *  len     - size of provided buffer
 *
 * Returns:
 *  length of name string (sans terminating null) or zero in case of failure
 */
static size_t freebsd_name( void *_ctx, char *name, size_t len )
{
    ctx_freebsd     *ctx = _ctx;
    Elf_Note        note;
    prpsinfo_t      info;
    char            *note_name;

    note.n_type = NT_PRPSINFO;
    *name = '\0';
    note_name = find_note( ctx->fd, ctx->e_hdr, ctx->p_hdr, ctx->swap, &note );
    if( note_name ) {
        /* should we check the name string? */
        free( note_name );
        if( read( ctx->fd, &info, sizeof( info ) ) == sizeof( info ) ) {
            strlcpy( name, info.pr_fname, len );
        }
    }
    return( strlen( name ) );
}


/* Fill in MAD configuration data
 *
 * Parameters:
 *  ctx     - driver context
 *  mad     - MAD architecture
 *  os      - operating system
 *  cpu     - CPU model
 *  fpu     - FPU model
 *
 * Returns:
 *  zero if configuration could not be determined, non-zero otherwise
 */
static int freebsd_qcfg( void *_ctx, int *mad, int *os, int *cpu, int *fpu )
{
    ctx_freebsd     *ctx = _ctx;
    long            rc = FALSE;

    // TODO: add support for non-x86 architectures
    if( ctx->e_hdr->e_machine == EM_386 ) {
        /* There doesn't appear to be a way to determine CPU type etc.
         * from FreeBSD core files. Just say it's a 486 and be done with it.
         */
        *os  = OS_FREEBSD;
        *mad = MAD_X86;
        *cpu = X86_486;
        *fpu = X86_487;
        rc = TRUE;
    }
    return( rc );
}


/* Determine process id of the dead process
 *
 * Parameters:
 *  ctx     - driver context
 *
 * Returns:
 *  process id of dead process or 1 if not available
 */
static long freebsd_qpid( void *_ctx )
{
    ctx_freebsd     *ctx = _ctx;
    Elf_Note        note;
    prstatus_t      status;
    char            *note_name;
    long            pid = 1;

    note.n_type = NT_PRSTATUS;
    note_name = find_note( ctx->fd, ctx->e_hdr, ctx->p_hdr, ctx->swap, &note );
    if( note_name ) {
        /* should we check the name string? */
        free( note_name );
        if( read( ctx->fd, &status, sizeof( status ) ) == sizeof( status ) ) {
            pid = status.pr_pid;
        }
    }
    return( pid );
}


/* Determine signal that killed process
 *
 * Parameters:
 *  ctx     - driver context
 *
 * Returns:
 *  signal which killed dead process or zero if not available
 */
static long freebsd_qsig( void *_ctx )
{
    ctx_freebsd     *ctx = _ctx;
    Elf_Note        note;
    prstatus_t      status;
    char            *note_name;
    long            sig = 0;

    note.n_type = NT_PRSTATUS;
    note_name = find_note( ctx->fd, ctx->e_hdr, ctx->p_hdr, ctx->swap, &note );
    if( note_name ) {
        /* should we check the name string? */
        free( note_name );
        if( read( ctx->fd, &status, sizeof( status ) ) == sizeof( status ) ) {
            sig = status.pr_cursig;
        }
    }
    return( sig );
}


/* Fills in the MAD register structure with CPU register
 * data read from core file.
 *
 * Parameters:
 *  ctx     - driver context
 *  r       - MAD register structure
 *  tid     - thread to return CPU registers for
 *
 * Returns:
 *  size of CPU registers or zero in case of failure
 */
static size_t freebsd_regs( void *_ctx, mad_registers *r, int tid )
{
    ctx_freebsd     *ctx = _ctx;
    Elf_Note        note;
    prstatus_t      status;
    char            *note_name;

    /* Look for a NT_PRSTATUS note */
    note.n_type = NT_PRSTATUS;
    note_name = find_note( ctx->fd, ctx->e_hdr, ctx->p_hdr, ctx->swap, &note );
    if( note_name ) {
        free( note_name );
        /* Read the program status (note descriptor data) */
        if( read( ctx->fd, &status, sizeof( status ) ) == sizeof( status ) ) {
            r->x86.cpu.eax = status.pr_reg.r_eax;
            r->x86.cpu.ebx = status.pr_reg.r_ebx;
            r->x86.cpu.ecx = status.pr_reg.r_ecx;
            r->x86.cpu.edx = status.pr_reg.r_edx;
            r->x86.cpu.esi = status.pr_reg.r_esi;
            r->x86.cpu.edi = status.pr_reg.r_edi;
            r->x86.cpu.ebp = status.pr_reg.r_ebp;
            r->x86.cpu.esp = status.pr_reg.r_esp;
            r->x86.cpu.eip = status.pr_reg.r_eip;
            r->x86.cpu.efl = status.pr_reg.r_eflags;
            r->x86.cpu.cs  = status.pr_reg.r_cs;
            r->x86.cpu.ds  = status.pr_reg.r_ds;
            r->x86.cpu.ss  = status.pr_reg.r_ss;
            r->x86.cpu.es  = status.pr_reg.r_es;
            r->x86.cpu.fs  = status.pr_reg.r_fs;
            r->x86.cpu.gs  = status.pr_reg.r_gs;
            return( sizeof( struct x86_mad_registers ) );
        }
    }
    return( 0 );
}


/* Fills in the MAD register structure with FPU register
 * data read from core file.
 *
 * Parameters:
 *  ctx     - driver context
 *  r       - MAD register structure
 *  tid     - thread to return FPU registers for
 *
 * Returns:
 *  size of FPU registers or zero in case of failure
 */
static size_t freebsd_freg( void *ctx, mad_registers *r, int tid )
{
    return( 0 );
}


/* Frees any resources allocated by driver and generally undoes
 * whatever the init function did.
 */
static void freebsd_done( void *ctx )
{
    free( ctx );
}

plat_drv_t  Drv_FreeBSD = {
    freebsd_init,
    freebsd_name,
    freebsd_qcfg,
    freebsd_qpid,
    freebsd_qsig,
    freebsd_regs,
    freebsd_freg,
    freebsd_done
};

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
* Description:  ELF core file internals and platform specific interface.
*
****************************************************************************/


#ifndef ELFCORE_H_INCLUDED
#define ELFCORE_H_INCLUDED

#include <stddef.h>
#include <sys/stat.h>
#include "machtype.h"
#include "bool.h"


/* Utility macro to easily determine if byte swapping is needed */
#ifdef __BIG_ENDIAN__
    #define NATIVE_ELF_ORDER    ELFDATA2MSB
#else
    #define NATIVE_ELF_ORDER    ELFDATA2LSB
#endif

/* Core dump driver interface */

typedef void  *(* plat_init_fn)( int fd, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr );
typedef size_t (* plat_name_fn)( void *ctx, char *name, size_t len );
typedef int    (* plat_qcfg_fn)( void *ctx, int *mad, int *os, int *cpu, int *fpu );
typedef long   (* plat_qpid_fn)( void *ctx );
typedef long   (* plat_qsig_fn)( void *ctx );
typedef size_t (* plat_regs_fn)( void *ctx, mad_registers *r, int tid );
typedef size_t (* plat_freg_fn)( void *ctx, mad_registers *r, int tid );
typedef void   (* plat_done_fn)( void *ctx );

typedef struct plat_drv {
    plat_init_fn    init;       /* Initialize driver */
    plat_name_fn    name;       /* Query process (executable) name */
    plat_qcfg_fn    qcfg;       /* Query MAD configuration (OS, CPU type etc.) */
    plat_qpid_fn    qpid;       /* Query process id (pid) */
    plat_qsig_fn    qsig;       /* Query killer signal */
    plat_regs_fn    regs;       /* Read CPU registers */
    plat_freg_fn    freg;       /* Read FPU registers */
    plat_done_fn    done;       /* Free any driver resources */
} plat_drv_t;

extern  plat_drv_t  Drv_FreeBSD;
extern  plat_drv_t  Drv_Neutrino;

extern  char        *find_note( int fd, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr, bool swap, Elf_Note *note );

#endif  /* ELFCORE_H_INCLUDED */

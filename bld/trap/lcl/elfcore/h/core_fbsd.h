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
* Description:  FreeBSD core file data.
*
****************************************************************************/


#ifndef CORE_FBSD_INCLUDED
#define CORE_FBSD_INCLUDED

#include <stddef.h>
#include <sys/stat.h>
#include "machtype.h"

#if defined( MD_x86 )

/* From FreeBSD's <machine/reg.h> */

typedef struct reg {
    uint32_t    r_fs;
    uint32_t    r_es;
    uint32_t    r_ds;
    uint32_t    r_edi;
    uint32_t    r_esi;
    uint32_t    r_ebp;
    uint32_t    r_isp;
    uint32_t    r_ebx;
    uint32_t    r_edx;
    uint32_t    r_ecx;
    uint32_t    r_eax;
    uint32_t    r_trapno;
    uint32_t    r_err;
    uint32_t    r_eip;
    uint32_t    r_cs;
    uint32_t    r_eflags;
    uint32_t    r_esp;
    uint32_t    r_ss;
    uint32_t    r_gs;
} gregset_t;

typedef struct fpreg {
    uint32_t        fpr_env[7];
    unsigned char   fpr_acc[8][10];
    uint32_t        fpr_ex_sw;
    unsigned char   fpr_pad[64];
} fpregset_t;

#endif

/* From FreeBSD's <sys/param.h> */

#define MAXCOMLEN       16              /* max command name remembered */

/* Parenthsized numbers like (1) indicate minimum version number
 * which defines the structure element.
 */

#define PRSTATUS_VERSION        1       /* Current version of prstatus_t */

typedef struct prstatus {
    int32_t     pr_version;     /* Version number of struct (1) */
    uint32_t    pr_statussz;    /* sizeof(prstatus_t) (1) */
    uint32_t    pr_gregsetsz;   /* sizeof(gregset_t) (1) */
    uint32_t    pr_fpregsetsz;  /* sizeof(fpregset_t) (1) */
    int32_t     pr_osreldate;   /* Kernel version (1) */
    int32_t     pr_cursig;      /* Current signal (1) */
    int32_t     pr_pid;         /* Process ID (1) */
    gregset_t   pr_reg;         /* General purpose registers (1) */
} prstatus_t;

#define PRARGSZ         80      /* Maximum argument bytes saved */

#define PRPSINFO_VERSION        1       /* Current version of prpsinfo_t */

typedef struct prpsinfo {
    int32_t     pr_version;             /* Version number of struct (1) */
    uint32_t    pr_psinfosz;            /* sizeof(prpsinfo_t) (1) */
    char        pr_fname[MAXCOMLEN+1];  /* Command name, null terminated (1) */
    char        pr_psargs[PRARGSZ+1];   /* Arguments, null terminated (1) */
} prpsinfo_t;


/* Rendezvous structure for communication between the dynamic linker and
 * the debugger. If executable's .dynamic section contains a DT_DEBUG element,
 * the dynamic linker sets it to point to this structure.
 */

struct r_debug {
    int                 r_version;    /* Protocol version */
    struct link_map     *r_map;       /* Start of loaded objects list */
    Elf32_Addr          r_brk;        /* Address to set a breakpoint at */
    enum {
        RT_CONSISTENT,                /* Mapping change is complete */
        RT_ADD,                       /* Loading new shared object */
        RT_DELETE                     /* Removing a shared object */
    }                   r_state;      /* Current state of dynamic linker */
    Elf32_Addr          r_ldbase;     /* Dynamic linker's load base */
};

/* Structure describing a loaded shared object - part of a doubly linked list */
struct link_map {
    Elf32_Addr          l_addr;       /* Offset from object's va to load base */
    char                *l_name;      /* Name (absolute path) of shared object */
    Elf32_Dyn           *l_ld;        /* Shared object's dynamic section */
    struct link_map     *l_next;      /* Next entry in chain */
    struct link_map     *l_prev;      /* Previous entry in chain */
};

#endif  /* CORE_FBSD_INCLUDED */

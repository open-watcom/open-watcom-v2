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
* Description:  QNX Neutrino core file data.
*
****************************************************************************/


#ifndef CORE_NTO_INCLUDED
#define CORE_NTO_INCLUDED

#include <stddef.h>
#include <sys/stat.h>
#include "machtype.h"

/* Misc. Neutrino types */
typedef struct {
    long    bits[2];
} qsigset_t;

#if defined( MD_x86 )

/* From Neutrino's <x86/context.h> */

typedef struct x86_cpu_registers {
#ifdef __SEGMENTS__
    uint32_t        gs, fs;
    uint32_t        es, ds;
#endif
    uint32_t        edi, esi, ebp, exx, ebx, edx, ecx, eax;
    uint32_t        eip, cs, efl;
    uint32_t        esp, ss;
} X86_CPU_REGISTERS;

typedef struct fsave_area {
    uint32_t        fpu_control_word;
    uint32_t        fpu_status_word;
    uint32_t        fpu_tag_word;
    uint32_t        fpu_ip;
    uint32_t        fpu_cs;
    uint32_t        fpu_op;
    uint32_t        fpu_ds;
    uint8_t         st_regs[80];
} X86_FSAVE_REGISTERS;

typedef struct fxsave_area {
    uint16_t        fpu_control_word;
    uint16_t        fpu_status_word;
    uint16_t        fpu_tag_word;
    uint16_t        fpu_operand;
    uint32_t        fpu_ip;
    uint32_t        fpu_cs;
    uint32_t        fpu_op;
    uint32_t        fpu_ds;
    uint32_t        mxcsr;
    uint32_t        reserved;
    uint8_t         st_regs[128];
    uint8_t         xmm_regs[128];
    uint8_t         reserved2[224];
} X86_FXSAVE_REGISTERS;

typedef union x86_fpu_registers {
    X86_FSAVE_REGISTERS     fsave_area;
    X86_FXSAVE_REGISTERS    fxsave_area;
    uint8_t                 data[512];
} X86_FPU_REGISTERS;

#endif

/* From Neutrino's <sys/elf_notes.h> */

#define QNX_NOTE_NAME   "QNX"

enum Elf_qnx_note_types {
    QNT_NULL = 0,
    QNT_DEBUG_FULLPATH,
    QNT_DEBUG_RELOC,
    QNT_STACK,
    QNT_GENERATOR,
    QNT_DEFAULT_LIB,
    QNT_CORE_SYSINFO,
    QNT_CORE_INFO,
    QNT_CORE_STATUS,
    QNT_CORE_GREG,
    QNT_CORE_FPREG,
    QNT_NUM
};

/* From Neutrino's <sys/debug.h> */

typedef struct _debug_process_info {
    int32_t         pid;
    int32_t         parent;
    uint32_t        flags;
    uint32_t        umask;
    int32_t         child;
    int32_t         sibling;
    int32_t         pgrp;
    int32_t         sid;
    uint64_t        base_address;
    uint64_t        initial_stack;
    int32_t         uid;
    int32_t         gid;
    int32_t         euid;
    int32_t         egid;
    int32_t         suid;
    int32_t         sgid;
    qsigset_t       sig_ignore;
    qsigset_t       sig_queue;
    qsigset_t       sig_pending;
    uint32_t        num_chancons;
    uint32_t        num_fdcons;
    uint32_t        num_threads;
    uint32_t        num_timers;
    uint64_t        start_time;     /* Start time in nsec */
    uint64_t        utime;          /* User running time in nsec */
    uint64_t        stime;          /* System running time in nsec */
    uint64_t        cutime;         /* terminated children user time in nsec */
    uint64_t        cstime;         /* terminated children user time in nsec */
    uint8_t         priority;       /* process base priority */
    uint8_t         reserved2[7];
    uint64_t        reserved[14];
} debug_process_t;

typedef struct _debug_thread_info {
    int32_t         pid;
    int32_t         tid;
    uint32_t        flags;
    uint16_t        why;
    uint16_t        what;
    uint64_t        ip;
    uint64_t        sp;
    uint64_t        stkbase;
    uint64_t        tls;
    uint32_t        stksize;
    uint32_t        tid_flags;
    uint8_t         priority;
    uint8_t         real_priority;
    uint8_t         policy;
    uint8_t         state;
    int16_t         syscall;
    uint16_t        last_cpu;
    uint32_t        timeout;
    int32_t         last_chid;
    qsigset_t       sig_blocked;
    qsigset_t       sig_pending;
//  siginfo_t       info;
    union {
        struct {
            int32_t         tid;
        }               join;
        struct {
            int32_t         id;
//          _Uintptrt       sync;
        }               sync;
        struct {
            uint32_t        nd;
            int32_t         pid;
            int32_t         coid;
            int32_t         chid;
            int32_t         scoid;
        }               connect;
        struct {
            int32_t         chid;
        }               channel;
        struct {
            int32_t         pid;
//          _Uintptrt       vaddr;
            uint32_t        flags;
        }               waitpage;
        struct {
            uint32_t        size;
        }               stack;
        uint64_t            filler[4];
    }                   blocked;
    uint64_t            start_time;     /* thread start time in nsec */
    uint64_t            sutime;         /* thread system + user running time in nsec */
    uint64_t            reserved2[6];
} debug_thread_t;

#endif  /* CORE_NTO_INCLUDED */

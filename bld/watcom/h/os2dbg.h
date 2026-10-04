/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Structures for use with OS/2 1.x DOSPTrace API.
*
****************************************************************************/


struct trace_regs {
    USHORT  AX;
    USHORT  BX;
    USHORT  CX;
    USHORT  DX;
    USHORT  SI;
    USHORT  DI;
    USHORT  BP;
    USHORT  DS;
    USHORT  ES;
    USHORT  IP;
    USHORT  CS;
    USHORT  FL;
    USHORT  SP;
    USHORT  SS;
};

struct trace_memory {
    USHORT      offb;
    USHORT      segb;
};

/*
 * For OS/2 1.0, IBM documented commands up to and including
 * PT_CMD_GET_LIB_NAME (16). So did Microsoft for OS/2 1.1.
 * For OS/2 1.2, IBM documented commands up to and including
 * PT_CMD_WRITE_MEM_B (19). Commands 18 and 19 were documented
 * as TRC_C_ReadMem_B and TRC_C_WriteMem_B, read/write memory
 * block. The IBM documentation is wrong and commands 18-20
 * are in fact segment alias commands, analogous to DosDebug
 * on OS/2 2.0.
 * For OS/2 1.2, Microsoft documented commands up to and
 * including PT_CMD_THREAD_STAT (17).
 * Microsoft KB article Q57856 lists commands up to 20, with
 * the last three listed as TRC_C_MapROAlias, TRC_C_MapRWAlias,
 * and TRC_C_UnMapAlias.
 */
typedef struct {
    USHORT  pid;
    USHORT  tid;
    USHORT  cmd;
    USHORT  value;
    USHORT  offv;
    USHORT  segv;
    USHORT  mte;
    union {
        struct trace_regs   r;
        struct trace_memory m;
    }       u;
} TRACEBUF;

typedef enum {                  /* values for .cmd field */
    PT_CMD_READ_MEM_I = 1,  /* read I-space word */
    PT_CMD_READ_MEM_D,      /* read D-space word */
    PT_CMD_READ_REGS,       /* read thread registers */
    PT_WRITE_CMD_MEM_I,     /* write I-space word */
    PT_CMD_WRITE_MEM_D,     /* write D-space word */
    PT_CMD_WRITE_REGS,      /* write thread registers */
    PT_CMD_GO,              /* go (with signal) */
    PT_CMD_TERMINATE,       /* terminate child process */
    PT_CMD_SINGLE_STEP,     /* single step */
    PT_CMD_STOP,            /* stop child process */
    PT_CMD_FREEZE,          /* suspend thread */
    PT_CMD_RESUME,          /* resume thread */
    PT_CMD_SEG_TO_SEL,      /* segment number to selector */
    PT_CMD_READ_8087,       /* read npx */
    PT_CMD_WRITE_8087,      /* write npx */
    PT_CMD_GET_LIB_NAME,    /* get library module name */
    PT_CMD_THREAD_STAT,     /* get thread status */
#if 0   /* depends on which documentation you believe */
    PT_CMD_READ_MEM_B,      /* read memory block */
    PT_CMD_WRITE_MEM_B,     /* write memory block */
#else
    PT_CMD_MAP_RO_ALIAS,    /* create a read only segment alias */
    PT_CMD_MAP_WR_ALIAS,    /* create a read/write segment alias */
    PT_CMD_UNMAP_ALIAS      /* unmap a segment alias */
#endif
} trace_codes;

/*
 * For OS/2 1.0, IBM documented return codes up to and including
 * PT_RET_NO_NPX_YET (-9). So did Microsoft for OS/2 1.1.
 * For OS/2 1.2, IBM documented return codes up to and including
 * PT_RET_STOPPED (-11). So did Microsoft for OS/2 1.2.
 * Microsoft KB article Q57856 lists return codes up to and
 * including -13 (PT_RET_ALIAS_FREE).
 */
                                /* returned in .cmd field */
#define PT_RET_SUCCESS          ((USHORT)(0))     /* 0x0000 success */
#define PT_RET_ERROR            ((USHORT)(-1))    /* 0xFFFF error */
#define PT_RET_SIGNAL           ((USHORT)(-2))    /* 0xFFFE about to receive signal */
#define PT_RET_STEP             ((USHORT)(-3))    /* 0xFFFD single step interrupt */
#define PT_RET_BREAK            ((USHORT)(-4))    /* 0xFFFC hit break point */
#define PT_RET_PARITY           ((USHORT)(-5))    /* 0xFFFB parity error */
#define PT_RET_FUNERAL          ((USHORT)(-6))    /* 0xFFFA process dying */
#define PT_RET_FAULT            ((USHORT)(-7))    /* 0xFFF9 general protection fault */
#define PT_RET_LIB_LOADED       ((USHORT)(-8))    /* 0xFFF8 library has just been loaded */
#define PT_RET_NO_NPX_YET       ((USHORT)(-9))    /* 0xFFF7 task hasn't yet used 8087 */
#define PT_RET_TRD_TERMINATE    ((USHORT)(-10))   /* 0xFFF6 thread ending */
#define PT_RET_STOPPED          ((USHORT)(-11))   /* 0xFFF5 async stop */
#define PT_RET_NEW_PROC         ((USHORT)(-12))   /* 0xFFF4 new process started */
#define PT_RET_ALIAS_FREE       ((USHORT)(-13))   /* 0xFFF3 segment alias freed */
#define PT_RET_WATCH            ((USHORT)(-100))  /* 0xFF9C THIS is a fake. It is never returned */

USHORT APIENTRY DosPTrace( TRACEBUF __far * );

struct thd_state {
    unsigned char   dbg_state;
    unsigned char   thread_state;
    unsigned short  priority;
};

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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
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
        PT_CMD_READ_MEM_I = 1,  /* read I-space */
        PT_CMD_READ_MEM_D,      /* read D-space */
        PT_CMD_READ_REGS,       /* read registers */
        PT_WRITE_CMD_MEM_I,     /* write I-space */
        PT_CMD_WRITE_MEM_D,     /* write D-space */
        PT_CMD_WRITE_REGS,      /* write registers */
        PT_CMD_GO,              /* go (with signal) */
        PT_CMD_TERMINATE,       /* terminate child process */
        PT_CMD_SINGLE_STEP,     /* single step */
        PT_CMD_STOP,            /* stop child process */
        PT_CMD_FREEZE,          /* freeze child process */
        PT_CMD_RESUME,          /* resume child process */
        PT_CMD_SEG_TO_SEL,      /* resume child process */
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
                                /* returned in .cmd field */
#define PT_RET_SUCCESS          0x0000  /* success */
#define PT_RET_ERROR            ((USHORT)(-1))    /* error */
#define PT_RET_SIGNAL           ((USHORT)(-2))    /* about to receive signal */
#define PT_RET_STEP             ((USHORT)(-3))    /* single step interrupt */
#define PT_RET_BREAK            ((USHORT)(-4))    /* hit break point */
#define PT_RET_PARITY           ((USHORT)(-5))    /* parity error */
#define PT_RET_FUNERAL          ((USHORT)(-6))    /* process dying */
#define PT_RET_FAULT            ((USHORT)(-7))    /* general protection fault */
#define PT_RET_LIB_LOADED       ((USHORT)(-8))    /* library has just been loaded */
#define PT_RET_NO_NPX_YET       ((USHORT)(-9))    /* task hasn't yet used 8087 */
#define PT_RET_TRD_TERMINATE    ((USHORT)(-10))   /* undocumented ... */
#define PT_RET_STOPPED          ((USHORT)(-11))   /* ... thanks micro$oft! */
#define PT_RET_NEW_PROC         ((USHORT)(-12))   /* new process started */
#define PT_RET_ALIAS_FREE       ((USHORT)(-13))   /* segment alias freed */
#define PT_RET_WATCH            ((USHORT)(-100))  /* THIS is a fake. It is never returned */

USHORT APIENTRY DosPTrace( TRACEBUF __far * );

struct thd_state {
    unsigned char   dbg_state;
    unsigned char   thread_state;
    unsigned short  priority;
};

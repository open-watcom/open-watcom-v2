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


#include "extender.h"
#include "machtype.h"

typedef union {
    struct {
        addr32_off      offset;
        addr_seg        segment;
    }           s;
    unsigned_32 a;
} memptr;

typedef union {
    struct {
        addr_seg        rm;
        addr_seg        pm;
    }           segm;
    unsigned_32 dpmi_adr;
} dos_memory;

#define         NUM_VECTS       256
#define         MAX_MSG_SIZE    0x400
#define         PARM_SIZE       (MAX_MSG_SIZE+0x40)
#define         STACK_SIZE      4096

/* has to match STATICDATA structure in DSXHDLR.ASM */
#include "pushpck1.h"
typedef struct {
    memptr          orig_vects[NUM_VECTS];
    memptr          vecttable1[NUM_VECTS];
    memptr          vecttable2[NUM_VECTS];
    addr32_off      rm_func;
    memptr          initfunc;
    memptr          reqfunc;
    memptr          finifunc;
    dos_memory      envseg;
    memptr          switchaddr;
    memptr          saveaddr;
    dos_memory      saveseg;
    unsigned_16     savesize;
    addr_seg        pmode_ds;
    addr_seg        pmode_es;
    addr_seg        pmode_cs;
    addr48_off      pmode_eip;
    addr_seg        pmode_ss;
    addr48_off      pmode_esp;
    addr_seg        save_env;
    addr_seg        save_ss;
    addr32_off      save_sp;
    memptr          oldint10;
    unsigned_8      pending;
    unsigned_8      fail;
    unsigned_8      act_font_tbls;
    unsigned_8      othersaved;
    unsigned_8      parmarea[PARM_SIZE];
    unsigned_8      stack[STACK_SIZE];
} rm_data;
#include "poppck.h"

extern rm_data          __far *PMData;
extern dos_memory       RMData;

extern unsigned_8       RMSegStart[];
extern unsigned_8       RMDataStart[];
extern void             RMTrapInit();
extern void             RMTrapAccess();
extern void             RMTrapFini();
extern void             RMTrapHandler();
extern void             RawSwitchHandler();
extern void             Interrupt10();
extern void             Interrupt15();
extern void             Interrupt1b_23();
extern void             Interrupt24();
extern unsigned_32      MyGetRMVector( unsigned );
extern void             MySetRMVector( unsigned, unsigned, unsigned );
extern unsigned_8       RMSegEnd[];

#define _NBPARAS( bytes )       ( ( (bytes) + 15UL ) / 16 )
#define RM_OFF( sym )           ((unsigned)(sym)-(unsigned)RMSegStart)
#define MK_PM( s, o )           MK_FP( _ExtenderRealModeSelector, ((s) << 4) + (o) )

extern void Boom( unsigned );
#pragma aux Boom = "db 0xf, 0xff" parm [eax]

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
* Description:  Overlay manager data structure definitions
*               shared between linker and overlay loader library.
*
****************************************************************************/


//   !!!!!!!!!   must correspond to declarations in ovltab.inc  !!!!!!!!!

#ifndef _OVLTAB_H_
#define _OVLTAB_H_

#define OVL_MAJOR_VERSION 3
#define OVL_MINOR_VERSION 0

/* Next is for compilers which don't like segmented architecture
 * GCC is too dumb to understand far pointers; fortunately this define
 * is only needed for building 16-bit overlay loader code.
 */
#if defined( __WATCOMC__ )
#define _CODE_BASED __based( __segname("_CODE") )
#else
#define _CODE_BASED
#define far
#define near
#endif

#include "pushpck1.h"               /* make sure no structures are padded. */

typedef struct dos_addr {
    unsigned_16     off;
    unsigned_16     seg;
} dos_addr;

typedef struct ovltab_entry {
    unsigned_16         flags_anc;  /* flags & number of ancestor */
    unsigned_16         relocs;     /* # of segment relocs at end of overlay */
    unsigned_16         start_para; /* starting para. of section as generated */
    unsigned_16         code_handle;/* start. para. of section in memory */
    unsigned_16         num_paras;  /* number of paragraphs in section. */
    unsigned_16         fname;      /* offset from OVLTAB to filename */
    unsigned_32         disk_addr;  /* location of overlay in file */
} ovltab_entry;

typedef ovltab_entry _CODE_BASED *ovltab_entry_ptr;

// flags_anc
#define OVE_FLAG_PRELOAD    0x8000  /* load overlay at init time */
#define OVE_FLAG_ANC_MASK   0x07ff  /* mask to get ancestor */
// fname
#define OVE_EXE_FILENAME    0x8000  /* flag indicating the .EXE file. */

typedef struct ovltab_prolog {
    unsigned_8      major;      // version numbers
    unsigned_8      minor;
    dos_addr        start;      /* start address for program */
    unsigned_16     delta;      /* paragraph offset of beginning of module */
    unsigned_16     ovl_size;   /* size of overlay area (used in dynamic only)*/
} ovltab_prolog;

typedef struct ovl_null_table {
    ovltab_prolog       prolog;
    unsigned_16         finish;
} ovl_null_table;

typedef struct ovl_table {
    ovltab_prolog       prolog;
    ovltab_entry        entries[ 1 ];
} ovl_table;

#define OVLTAB_TERMINATOR   0xffff

typedef struct svector {            /* short overlay vector */
    unsigned_8  call_op;
    unsigned_16 ldr_addr;
    unsigned_16 sec_num;
    unsigned_8  jmp_op;
    unsigned_16 target;
} svector;

typedef svector _CODE_BASED *svector_ptr;

typedef struct {
    unsigned_16 big_nop;
    unsigned_8  test_op;
    unsigned_16 sec_num;
} mungedvector;

// big_nop
#define OVV_MOV_AX_AX   0xC089      /* opcode for MOV AX,AX */
// test_op
#define OVV_TEST_OPCODE 0xA9        /* opcode for TEST AX,word */
                                    /* used for hiding the section number */
typedef struct {
    unsigned_8  call_op;
    unsigned_16 ldr_addr;
    unsigned_16 sec_num;
} plainvector;

typedef struct {
    unsigned_8  cs_over;
    unsigned_16 inc_op;
    unsigned_16 tab_addr;
} incvector;

// cs_over
#define OVV_CS_OVERRIDE 0x2E
// inc_op
#define OVV_INC_OPCODE 0x6FE

typedef struct lvector {            /* long overlay vector */
    union {
        plainvector     v;
        mungedvector    m;
        incvector       i;
    } u;
    unsigned_8          jmp_op;
    dos_addr            target;
} lvector;

typedef lvector _CODE_BASED *lvector_ptr;

#include "poppck.h"

#endif

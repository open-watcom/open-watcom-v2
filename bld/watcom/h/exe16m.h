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
* Description:  DOS/16M executable format structures and constants.
*
****************************************************************************/


#ifndef _EXE16M_H

typedef struct gdt_info {
    unsigned_16     gdtlen;
    unsigned_16     gdtaddr;
    unsigned_8      gdtaddr_hi;
    unsigned_8      gdtaccess;
    unsigned_16     gdtreserved;
} gdt_info;

typedef struct dos16m_exe_header {
    unsigned_16 signature;          /* BW signature to mark valid file  */
    unsigned_16 last_page_bytes;    /* length of image mod 512          */
    unsigned_16 pages_in_file;      /* number of 512 byte pages         */
    unsigned_16 reserved1;
    unsigned_16 reserved2;
    unsigned_16 min_alloc;          /* required memory, in KB           */
    unsigned_16 max_alloc;          /* max KB (private allocation)      */
    unsigned_16 stack_seg;          /* segment of stack                 */
    unsigned_16 stack_ptr;          /* initial SP value                 */
    unsigned_16 first_reloc_sel;    /* huge reloc list selector         */
    unsigned_16 init_ip;            /* initial IP value                 */
    unsigned_16 code_seg;           /* segment of code                  */
    unsigned_16 runtime_gdt_size;   /* runtime GDT size in bytes        */
    unsigned_16 MAKEPM_version;     /* ver * 100, GLU = (ver+10)*100    */
    /* end of DOS style EXE header */
    unsigned_32 next_header_pos;    /* file pos of next spliced .EXP    */
    unsigned_32 cv_info_offset;     /* offset to start of debug info    */
    unsigned_16 last_sel_used;      /* last selector value used         */
    unsigned_16 pmem_alloc;         /* private xm amount KB if nonzero  */
    unsigned_16 alloc_incr;         /* auto ExtReserve amount, in KB    */
    unsigned_8  reserved4[6];
    /* the following used to be referenced as gdtimage[0..1] */
    unsigned_16 options;            /* runtime options                  */
    unsigned_16 trans_stack_sel;    /* sel of transparent stack         */
    unsigned_16 exp_flags;          /* see ef_ constants below          */
    unsigned_16 program_size;       /* size of program in paras         */
    unsigned_16 gdtimage_size;      /* size of gdt in file (bytes)      */
    unsigned_16 first_selector;     /* gdt[first_sel] = gdtimage[0], 0 => 0x80 */
    unsigned_8  default_mem_strategy;
    unsigned_8  reserved5;
    unsigned_16 transfer_buffer_size;   /* default in bytes, 0 => 8KB   */
    /* the following used to be referenced as gdtimage[2..15] */
    unsigned_8  reserved6[48];
    char        EXP_path[64];       /* original .EXP file name  */
    /* gdtimage[16..gdtimage_size] follows immediately, then program image follows */
} dos16m_exe_header;

/* the values for the exp_flags field. */
enum {
    ef_auto			    = 0x0001,   /* automatic selectors (=> relocatable) */
    ef_package		    = 0x0002,   /* module is a package                  */
    ef_nobigfoot	    = 0x0004,   /* don't place stack low DPMI memory    */
    ef_sharedata	    = 0x2000,   /* data is global, not per-instance     */
    ef_shareable	    = 0x4000,   /* component may be shared              */
    ef_dos4g		    = 0x8000    /* module is or requires DOS/4G         */
};

/* the values for the options field */

enum {
    OPT_NOVCPI          = 0x0002,   /* don't test for VCPI at startup   */
    OPT_KEYBOARD        = 0x0004,   /* don't inhibit keyboard polling   */
    OPT_OVERLOAD        = 0x0008,   /* allow overloading                */
    OPT_INT10           = 0x0010,
    OPT_INIT00          = 0x0020,   /* init memory allocs to 0x00       */
    OPT_INITFF          = 0x0040,   /* init memory allocs to 0xFF       */
    OPT_ROTATE          = 0x0080,   /* rotate selector assignment       */
    OPT_AUTO            = 0x1000,   /* reloc table is present           */
};

#define TRANSPARENT 0x8000

#define NUM_RESERVED_SELS    16         // number of reserved selectors.

/* memory strategy constants */
#define MPreferExt      0       /* prefer, but not force, extended */
#define MPreferLow      1       /* prefer, but not force, conventional memory */
#define MForceExt       2       /* force extended */
#define MForceLow       3       /* force conventional */
#define MNoStrategy     0       /* no strategy specified */

#define D16M_ACC_CODE   0x9A    /* present, DPL 0, code, read, not accessed */
#define D16M_ACC_DATA   0x92    /* present, DPL 0, data, write, not accessed */

#define D16M_USER_SEL   0x80    /* first user selector (user_gdt << 3) */

#define DOS16M_SIGNATURE    0x5742      /* 'BW' */

#define _EXE16M_H
#endif

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



typedef struct ovltab_entry {
    unsigned_16         flags_anc;  /* flags & number of ancestor */
    unsigned_16         relocs;     /* # of segment relocs at end of overlay */
    unsigned_16         start_para; /* starting para. of section as generated */
    unsigned_16         code_handle;/* start. para. of section in memory */
    unsigned_16         num_paras;  /* number of paragraphs in section. */
    unsigned_16         fname;      /* offset from OVLTAB to filename */
    unsigned_32         disk_addr;  /* location of overlay in file */
} ovltab_entry;

#define FLAG_PRELOAD    0x8000  /* load overlay at init time */
#define FLAG_ANC_MASK   0x07ff  /* mask to get ancestor */
#define EXE_FILENAME    0x8000  // flag indicating the .EXE file.

typedef struct ovltab_prolog {
    unsigned_8      major;      // version numbers
    unsigned_8      minor;
    dos_addr        start;      /* start address for program */
    unsigned_16     delta;      /* paragraph offset of beginning of module */
    unsigned_16     ovl_size;   /* size of overlay area (used in dynamic only)*/
} ovltab_prolog;

#define OVL_MAJOR_VERSION 3
#define OVL_MINOR_VERSION 0

typedef struct ovl_null_table {
    ovltab_prolog       prolog;
    unsigned_16         finish;
} ovl_null_table;

typedef struct ovl_table {
    ovltab_prolog       prolog;
    ovltab_entry        entries[ 1 ];
} ovl_table;

#define OVLTAB_TERMINATOR   0xffff

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
* Description:  IBM/Microsoft .sym file format (produced by MAPSYM).
*
****************************************************************************/


#include "watcom.h"

/* Note: The "pointers" within a .sym file are stored in units of 16 bytes
 * (and consequently structures need to be 16-byte aligned in the file).
 * This is a holdover from 8086 days, but it does allow the .sym files to
 * be up to 1MB in size even though they're essentially 16-bit.
 */

#define SYM_PTR_TO_OFS( a )     ( a * 16 )

/* MAPSYM version 5.1 - all IBM releases of MAPSYM at least since 1991 */
#define SYM_VERSION_MAJOR       5
#define SYM_VERSION_MINOR       1

/* MAPSYM version 4.0 - Microsoft release from 1985 */
#define SYM_VERSION_MAJOR_OLD   4
#define SYM_VERSION_MINOR_OLD   0

/* MAPSYM version 6.0 - Microsoft release from circa 1990 */
#define SYM_VERSION_MAJOR_M32   6

#include "pushpck1.h"

typedef struct {
    unsigned_16     next_ptr;           /* pointer to next mapdef */
    unsigned_8      abs_type;           /* type of abs symbols (16/32bit) */
    unsigned_8      pad0;               /* pad byte */
    unsigned_16     entry_seg;          /* segment of entry point */
    unsigned_16     abs_sym_count;      /* number of abs symbols */
    unsigned_16     abs_tab_ofs;        /* offset of abs sym table from mapdef */
    unsigned_16     num_segs;           /* number of segments in map */
    unsigned_16     seg_ptr;            /* pointer to first segment in chain */
    unsigned_8      max_sym_len;        /* maximum length of symbol name */
    unsigned_8      name_len;           /* length of map name */
    char            name[1];            /* map name */
} sym_mapdef;

#define SYM_MAPDEF_FIXSIZE      offsetof( sym_mapdef, name_len )

typedef struct {
    unsigned_16     zero;               /* next map, must be 0 */
    unsigned_8      minor_ver;          /* minor version number */
    unsigned_8      major_ver;          /* major version number */
} sym_endmap;

typedef struct {
    unsigned_16     next_ptr;           /* next segdef, 0 if last; may be circular */
    unsigned_16     num_syms;           /* number of symbols in segment */
    unsigned_16     sym_tab_ofs;        /* offset of symbol table from segdef */
    unsigned_16     load_addr;          /* segment load address */
    unsigned_16     phys_0;             /* physical address 0 */
    unsigned_16     phys_1;             /* physical address 1 */
    unsigned_16     phys_2;             /* physical address 2 */
    unsigned_8      sym_type;           /* type of symbols (16/32bit) */
    unsigned_8      pad0;               /* pad byte */
    unsigned_16     linnum_ptr;         /* pointer to line numbers */
    unsigned_8      is_loaded;          /* segment loaded flag */
    unsigned_8      curr_inst;          /* current instance */
    unsigned_8      name_len;           /* length of symbol name */
    char            name[1];            /* segment name */
} sym_segdef;

#define SYM_SEGDEF_FIXSIZE      offsetof( sym_segdef, name_len )

/* NB: 32-bit segments do not imply 32-bit symbol records; MAPSYM will emit
 * 16-bit symbols as long as all offsets in a segment fit within 16 bits.
 */

typedef struct {
    unsigned_16     offset;             /* offset of symbol within segment */
    unsigned_8      name_len;           /* length of symbol name */
    char            name[1];            /* symbol name */
} sym_symdef;

#define SYM_SYMDEF_FIXSIZE      offsetof( sym_symdef, name_len )

typedef struct {
    unsigned_32     offset;             /* offset of symbol within segment */
    unsigned_8      name_len;           /* length of symbol name */
    char            name[1];            /* symbol name */
} sym_symdef_32;

#define SYM_SYMDEF_32_FIXSIZE   offsetof( sym_symdef_32, name_len )

typedef struct {
    unsigned_16     next_ptr;           /* next linedef, 0 if last */
    unsigned_16     seg_ptr;            /* pointer to corresponding segdef */
    unsigned_16     lines_ofs;          /* offset of line table from linedef */
    unsigned_16     file;               /* external file handle */
    unsigned_16     lines_num;          /* number of linerecs */
    unsigned_8      name_len;           /* symbol name length */
    char            name[1];            /* symbol name */
} sym_linedef;

#define SYM_LINEDEF_FIXSIZE     offsetof( sym_linedef, name_len )

typedef struct {
    unsigned_16     code_offset;        /* offset within segment */
    unsigned_32     line_offset;        /* offset within file; may be -1 */
} sym_linerec;

typedef struct {
    unsigned_16     code_offset;        /* offset within segment */
    unsigned_16     line_offset;        /* offset within file; may be -1 */
} sym_linerec_16;

enum {
    SYM_FLAG_32BIT = 1,                 /* symbols are 32-bit */
    SYM_FLAG_ALPHA = 2                  /* alphasorted symbol table included */
};

#include "poppck.h"

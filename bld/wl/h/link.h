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


#ifdef _M_I86
#define ENUMU32(x)      x ## UL
#else
#define ENUMU32(x)      x ## U
#endif

typedef unsigned long linkflag;

#define DWARF_DBI_FLAG  0x00000001UL    // set if making DWARF debug info
#define STK_SIZE_FLAG   0x00000002UL
#define GOT_CHGD_FILES  0x00000004UL
#define TRACE_FLAG      0x00000008UL
#define __UNUSED_FLAG_0 0x00000010UL
#define CV_DBI_FLAG     0x00000020UL
#define UNDEFS_ARE_OK   0x00000040UL
#define REDEFS_OK       0x00000080UL
#define QUIET_FLAG      0x00000100UL
#define PACKCODE_FLAG   0x00000200UL    // set if packcode specified
#define VF_REMOVAL      0x00000400UL
#define NOVELL_DBI_FLAG 0x00000800UL    // set if generating novell debug info
#define PACKDATA_FLAG   0x00001000UL
#define CASE_FLAG       0x00002000UL    // set for case sensitive operation
#define OLD_DBI_FLAG    0x00004000UL    // set if making WATCOM debug info
#define SHOW_DEAD       0x00008000UL
#define STRIP_CODE      0x00010000UL    // strip dead code.
#define CVPACK_FLAG     0x00020000UL
#define MAX_ERRORS_FLAG 0x00040000UL    // there is a max. number of errors.
#define DONT_UNMANGLE   0x00080000UL    // don't unmangle the names
#define INC_LINK_FLAG   0x00100000UL
#define NOCACHE_FLAG    0x00200000UL
#define CACHE_FLAG      0x00400000UL
#define FAR_CALLS_FLAG  0x00800000UL    // optimize far calls
#define __UNUSED_FLAG_8 0x01000000UL
#define __UNUSED_FLAG_7 0x02000000UL
#define __UNUSED_FLAG_6 0x04000000UL
#define __UNUSED_FLAG_5 0x08000000UL
#define __UNUSED_FLAG_4 0x10000000UL
#define __UNUSED_FLAG_3 0x20000000UL
#define __UNUSED_FLAG_2 0x40000000UL
#define __UNUSED_FLAG_1 0x80000000UL

// Novell treated differently, as it can be generated at the same time as the
// others.

#define ANY_DBI_FLAG    (DWARF_DBI_FLAG | CV_DBI_FLAG | OLD_DBI_FLAG)

typedef enum mapflag {
    MAP_FLAG            = 0x0001,
    MAP_STATICS         = 0x0002,       // put statics in the map file
    MAP_ARTIFICIAL      = 0x0004,       // put artificial syms in the map file
    MAP_SORT            = 0x0008,       // sort symbols in the map file
    MAP_ALPHA           = 0x0010,       // sort symbols in alphabetical order
    MAP_GLOBAL          = 0x0020,       // sort symbols globally.
    MAP_VERBOSE         = 0x0040,       // verbose mode
    MAP_LINES           = 0x0080        // put line numbers in map file
} mapflag;


typedef enum {
    MAKE_RELOCS             ENUMU32( 0x00000001 ),
    SEARCHING_LIBRARIES     ENUMU32( 0x00000002 ),
    LIBRARIES_ADDED         ENUMU32( 0x00000004 ),
    LINK_ERROR              ENUMU32( 0x00000008 ),
    FMT_SPECIFIED           ENUMU32( 0x00000010 ),
    FMT_DECIDED             ENUMU32( 0x00000020 ),
    FMT_SEEN_32_BIT         ENUMU32( 0x00000040 ),
    FMT_SEEN_IMPORT_CMT     ENUMU32( 0x00000080 ),
    PROC_LIBS_ADDED         ENUMU32( 0x00000100 ),
    FMT_INITIALIZED         ENUMU32( 0x00000200 ),
    UNDEFED_SYM_ERROR       ENUMU32( 0x00000400 ),
    GENERATE_LIB_LIST       ENUMU32( 0x00000800 ),
    HAVE_16BIT_CODE         ENUMU32( 0x00001000 ),  // true if we have 16 bit code.
    HAVE_ALPHA_CODE         ENUMU32( 0x00002000 ),
    HAVE_PPC_CODE           ENUMU32( 0x00004000 ),
    HAVE_I86_CODE           ENUMU32( 0x00008000 ),
    HAVE_MIPS_CODE          ENUMU32( 0x00010000 ),
    HAVE_X64_CODE           ENUMU32( 0x00020000 ),
    CAN_REMOVE_SEGMENTS     ENUMU32( 0x00040000 ),
    STOP_WORKING            ENUMU32( 0x00080000 ),  // IDE wants us to stop now
    INTERNAL_DEBUG          ENUMU32( 0x00100000 ),
    GOT_PREV_STRUCTS        ENUMU32( 0x00200000 ),
    DOSSEG_FLAG             ENUMU32( 0x00400000 ),
    SPEC_ORDER_FLAG         ENUMU32( 0x00800000 ),
    FMT_SEEN_64_BIT         ENUMU32( 0x01000000 ),

    HAVE_MACHTYPE_MASK      = (HAVE_I86_CODE | HAVE_X64_CODE | HAVE_ALPHA_CODE | HAVE_PPC_CODE | HAVE_MIPS_CODE),
    CLEAR_ON_INC            = (STOP_WORKING | INTERNAL_DEBUG | GOT_PREV_STRUCTS | MAKE_RELOCS | FMT_SPECIFIED | FMT_DECIDED | FMT_INITIALIZED),
    ORDER_FLAG_MASK         = (DOSSEG_FLAG | SPEC_ORDER_FLAG)
} stateflag;

// this used for ID splits.

#define CODE_SEGMENT    1
#define DATA_SEGMENT    2

/*  Object file formats & flags */
typedef enum obj_format {
    FMT_EASY_OMF        = 0x0001,
    FMT_MS_386          = 0x0002,
    FMT_UNUSED_3        = 0x0004,
    FMT_IS_LIDATA       = 0x0008,   // true if last data was lidata
    FMT_TOLD_XXBIT      = 0x0010,   // already said object was xx-bit
    FMT_IGNORE_FIXUPP   = 0x0020,   // don't process a fixup record
    FMT_UNSAFE_FIXUPP   = 0x0040,   // don't optimize calls in current fixupp
    FMT_UNUSED_1        = 0x0080,
    FMT_PE_XFER         = 0x0000,   // .obj is PE xfer code segment(see note)
    FMT_OMF             = 0x0100,   // .obj is an OMF object file (see note)
    FMT_COFF            = 0x0200,   // .obj is a COFF object file (see note)
    FMT_ELF             = 0x0300,   // .obj is an ELF object file (see note)
    FMT_INCREMENTAL     = 0x0400,   // .obj is saved inc. linking info
    FMT_OBJ_FMT_MASK    = 0x0700,
    FMT_UNUSED_2        = 0x0800,
    FMT_DEBUG_COMENT    = 0x1000,   // saw an object debug coment.
} obj_format;

/* NOTE:  these are also stored into the
 * mod_entry->modinfo field, so if new object file formats are added, make
 * sure there isn't a conflict there!
*/

#define FMT_32BIT_REC           (FMT_EASY_OMF | FMT_MS_386)
#define FMT_IDX_SHIFT           8
#define FMT_IDX_BITS            (FMT_OBJ_FMT_MASK >> FMT_IDX_SHIFT)
#define GET_FMT_IDX(x)          (((x) >> FMT_IDX_SHIFT) & FMT_IDX_BITS)
#define IS_FMT_ORL(x)           (((x) & FMT_OBJ_FMT_MASK) >= FMT_COFF)
#define IS_FMT_OMF(x)           (((x) & FMT_OBJ_FMT_MASK) == FMT_OMF)
#define IS_FMT_ELF(x)           (((x) & FMT_OBJ_FMT_MASK) == FMT_ELF)
#define IS_FMT_COFF(x)          (((x) & FMT_OBJ_FMT_MASK) == FMT_COFF)
#define IS_FMT_INCREMENTAL(x)   (((x) & FMT_OBJ_FMT_MASK) == FMT_INCREMENTAL)
#define SET_FMT_TYPE(x,type)    (((x) = (x) & ~FMT_OBJ_FMT_MASK) | (type))

/* Default File Extension Enumeration, see ldefext.h */

typedef enum file_defext {
    #define pick1(enum,text) enum,
    #include "ldefext.h"
    #undef pick1
} file_defext;


/*  Generic constants */
#define MAX_REC             1024
#define UNDEFINED           ((segment)0xffff)           /* undefined segment */

#define SET_ADDR_UNDEFINED(a)   (a).seg=UNDEFINED;(a).off=0
#define IS_ADDR_UNDEFINED(a)    ((a).seg==UNDEFINED)

/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


typedef enum {
    LF_DWARF_DBI_FLAG       = 0x00000001,   // set if making DWARF debug info
    LF_STK_SIZE_FLAG        = 0x00000002,
    LF_GOT_CHGD_FILES       = 0x00000004,
    LF_TRACE_FLAG           = 0x00000008,
    LF_CV_DBI_FLAG          = 0x00000010,
    LF_UNDEFS_ARE_OK        = 0x00000020,
    LF_REDEFS_OK            = 0x00000040,
    LF_QUIET_FLAG           = 0x00000080,
    LF_PACKCODE_FLAG        = 0x00000100,   // set if packcode specified
    LF_VF_REMOVAL           = 0x00000200,
    LF_NOVELL_DBI_FLAG      = 0x00000400,   // set if generating novell debug info
    LF_PACKDATA_FLAG        = 0x00000800,
    LF_CASE_FLAG            = 0x00001000,   // set for case sensitive operation
    LF_OLD_DBI_FLAG         = 0x00002000,   // set if making WATCOM debug info
    LF_SHOW_DEAD            = 0x00004000,
    LF_STRIP_CODE           = 0x00008000,   // strip dead code.
    LF_CVPACK_FLAG          = 0x00010000,
    LF_MAX_ERRORS_FLAG      = 0x00020000,   // there is a max. number of errors.
    LF_DONT_UNMANGLE        = 0x00040000,   // don't unmangle the names
    LF_INC_LINK_FLAG        = 0x00080000,
    LF_NOCACHE_FLAG         = 0x00100000,
    LF_CACHE_FLAG           = 0x00200000,
    LF_FAR_CALLS_FLAG       = 0x00400000,   // optimize far calls
} linkflag;

// Novell treated differently, as it can be generated at the same time as the others.

#define LF_ANY_DBI_FLAG     (LF_DWARF_DBI_FLAG | LF_CV_DBI_FLAG | LF_OLD_DBI_FLAG)

typedef enum {
    MAP_FLAG                = 0x0001,
    MAP_STATICS             = 0x0002,       // put statics in the map file
    MAP_ARTIFICIAL          = 0x0004,       // put artificial syms in the map file
    MAP_SORT                = 0x0008,       // sort symbols in the map file
    MAP_ALPHA               = 0x0010,       // sort symbols in alphabetical order
    MAP_GLOBAL              = 0x0020,       // sort symbols globally.
    MAP_VERBOSE             = 0x0040,       // verbose mode
    MAP_LINES               = 0x0080        // put line numbers in map file
} mapflag;


typedef enum {
    LS_MAKE_RELOCS          = 0x00000001,
    LS_SEARCHING_LIBRARIES  = 0x00000002,
    LS_LIBRARIES_ADDED      = 0x00000004,
    LS_LINK_ERROR           = 0x00000008,
    LS_FMT_SPECIFIED        = 0x00000010,
    LS_FMT_DECIDED          = 0x00000020,
    LS_FMT_SEEN_32BIT       = 0x00000040,
    LS_FMT_SEEN_IMPORT_CMT  = 0x00000080,
    LS_PROC_LIBS_ADDED      = 0x00000100,
    LS_FMT_INITIALIZED      = 0x00000200,
    LS_UNDEFED_SYM_ERROR    = 0x00000400,
    LS_GENERATE_LIB_LIST    = 0x00000800,
    LS_HAVE_16BIT_CODE      = 0x00001000,   // true if we have 16 bit code.
    LS_HAVE_ALPHA_CODE      = 0x00002000,
    LS_HAVE_PPC_CODE        = 0x00004000,
    LS_HAVE_X86_CODE        = 0x00008000,
    LS_HAVE_MIPS_CODE       = 0x00010000,
    LS_HAVE_X64_CODE        = 0x00020000,
    LS_CAN_REMOVE_SEGMENTS  = 0x00040000,
    LS_STOP_WORKING         = 0x00080000,   // IDE wants us to stop now
    LS_INTERNAL_DEBUG       = 0x00100000,
    LS_GOT_PREV_STRUCTS     = 0x00200000,
    LS_DOSSEG_FLAG          = 0x00400000,
    LS_SPEC_ORDER_FLAG      = 0x00800000,
    LS_FMT_SEEN_64BIT       = 0x01000000,
} stateflag;

#define LS_HAVE_MACHTYPE_MASK   (LS_HAVE_X86_CODE | LS_HAVE_X64_CODE | LS_HAVE_ALPHA_CODE | LS_HAVE_PPC_CODE | LS_HAVE_MIPS_CODE)
#define LS_ORDER_FLAG_MASK      (LS_DOSSEG_FLAG | LS_SPEC_ORDER_FLAG)

#define LS_CLEAR_ON_INC     /* flags to clear when incremental linking. */ \
    (LS_STOP_WORKING | LS_INTERNAL_DEBUG | LS_GOT_PREV_STRUCTS | LS_MAKE_RELOCS | LS_FMT_SPECIFIED | LS_FMT_DECIDED | LS_FMT_INITIALIZED)

// this used for ID splits.

#define CODE_SEGMENT        1
#define DATA_SEGMENT        2

/*  Object file formats & flags */
typedef enum {
    /* bits 0..4 available (bits 0..4 reserved for DBI_xxxx symbols, not used here) */
    OBJ_FMT_EASY_OMF            = 0x0001,
    OBJ_FMT_MS_386              = 0x0002,
    OBJ_FMT_IS_LIDATA           = 0x0004,   // true if last data was lidata
    OBJ_FMT_TOLD_BITNESS        = 0x0008,   // already said object was 16/32/64-bit
    OBJ_FMT_DEBUG_COMENT        = 0x0010,   // saw an object debug coment.
    /* bits 5..7 reserved for FMT_xxxx symbols (for deciding .obj format) */
    /* bits 8..max available */
    OBJ_FMT_IGNORE_FIXUPP       = 0x0100,   // don't process a fixup record
    OBJ_FMT_UNSAFE_FIXUPP       = 0x0200,   // don't optimize calls in current fixupp
} obj_format;

#define OBJ_FMT_32BIT_REC       (OBJ_FMT_EASY_OMF | OBJ_FMT_MS_386)

/* Default File Extension Enumeration, see ldefext.h */

typedef enum {
    #define pick(enum,text) enum,
    #include "ldefext.h"
    #undef pick
} file_defext;


/*  Generic constants */
#define MAX_REC                 _1K
#define UNDEFINED               ((segment)0xffff)           /* undefined segment */

#define SET_ADDR_UNDEFINED(a)   (a).seg=UNDEFINED;(a).off=0
#define IS_ADDR_UNDEFINED(a)    ((a).seg==UNDEFINED)

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
* Description:  IBM HLL and pre-CV4 CodeView debugging information.
*
****************************************************************************/


/* Note: The following is conceptually similar to cv4.h, but significantly
 * different in detail. Why lump together old CodeView with IBM HLL instead
 * of keeping all CodeView information in one place? Firstly, the IBM HLL
 * format is an outgrowth of old CodeView style debug data, and therefore old
 * CodeView format is much closer to HLL than it is to CV4. Secondly, and more
 * importantly, it is possible to have HLL and old CodeView debug information
 * mixed in a single executable image, hence the need to support HLL and old
 * CodeView in a single package.
 */

#define HLL_FIRST_USER_TYPE 0x200

#include "watcom.h"
typedef unsigned_8      u1;     /* short forms for read/written types */
typedef unsigned_16     u2;
typedef unsigned_32     u4;
typedef unsigned_64     u8;
typedef signed_8        i1;
typedef signed_16       i2;
typedef signed_32       i4;
typedef signed_64       i8;

/*
 * here are structures used in the executables.
 */


#define HLL_SIG_SIZE    4
#define HLL_NB00        "NB00"
#define HLL_NB02        "NB02"
#define HLL_NB04        "NB04"

typedef enum  hll_lf_values {   /* type enumerations */
#define _LFMAC( n, N, c )    HLF_##N = c,
    #include "hlltypes.h"
#undef _LFMAC
} hll_lf_values;


#pragma pack( 1 )
typedef enum {
    hll_sstModule = 0x101,
    hll_sstModules = hll_sstModule, /* misnomer. */
    hll_sstPublics,
    hll_sstTypes,
    hll_sstSymbols,
    hll_sstSrcLines,
    hll_sstLibraries,
    hll_sstImports,
    hll_sstCompacted,
    hll_sstSrcLnSeg,
    hll_sstHLLSrc = 0x10B
} hll_sst;

typedef struct {
    char        sig[HLL_SIG_SIZE];
    signed_32   offset;
} hll_trailer;

/* HLL debug directory info. */
typedef struct {
    unsigned_16 cbDirHeader;
    unsigned_16 cbDirEntry;
    unsigned_32 cDir;
} hll_dirinfo;

/* CV3 debug directory info. */
typedef struct {
    unsigned_16 cDir;
} cv3_dirinfo;

/* HLL debug directory entry. */
typedef struct {
    unsigned_16 subsection;             /* The subsection type, hll_sst. */
    unsigned_16 iMod;                   /* The module index. (1 based) */
    unsigned_32 lfo;                    /* The offset of the subsection (NBxx relative). */
    unsigned_32 cb;                     /* The size of the subsection. */
} hll_dir_entry;

/* CV3 debug directory entry. */
typedef struct {
    unsigned_16 subsection;             /* The subsection type, hll_sst. */
    unsigned_16 iMod;                   /* The module index. (1 based) */
    unsigned_32 lfo;                    /* The offset of the subsection (NBxx relative). */
    unsigned_16 cb;                     /* The size of the subsection. */
} cv3_dir_entry;

/* HLL segment info */
typedef struct {
    unsigned_16 Seg;
    unsigned_32 offset;
    unsigned_32 cbSeg;
} hll_seginfo;

/* CV3 16-bit segment info. */
typedef struct {
    unsigned_16 Seg;
    unsigned_16 offset;
    unsigned_16 cbSeg;
} cv3_seginfo_16;

/* CV3 32-bit segment info. */
typedef struct {
    unsigned_16 Seg;
    unsigned_32 offset;
    unsigned_32 cbSeg;
} cv3_seginfo_32;

#define HLL_DEBUG_STYLE_CV ('V' << 8 | 'C')
#define HLL_DEBUG_STYLE_HL ('L' << 8 | 'H')

typedef struct {
    hll_seginfo         SegInfo;        /* The info for the first [code] segment. */
    unsigned_16         ovlNumber;      /* The overlay number. */
    unsigned_16         iLib;           /* The index of the library to which we belong. */
    unsigned_16         cSeg;           /* The number of segment info pieces (includes SegInfo). */
    unsigned_16         Style;          /* The debug info style. */
    unsigned_16         Version;        /* The style version. */
    unsigned_8          name_len;       /* The name length. */
    char                name[];
    // hll_seginfo         segs[];
} hll_module;

typedef struct {
    cv3_seginfo_16      SegInfo;        /* The segment info for the first [code] segment. */
    unsigned_16         ovlNumber;      /* The overlay number. */
    unsigned_16         iLib;           /* The index of the library to which we belong. */
    unsigned_8          cSeg;           /* The number of segment info pieces (includes SegInfo). */
    unsigned_8          reserved;
    unsigned_8          name_len;       /* The name length. */
//    char                name[1];
//  cv3_seginfo_16 arnsg[];
} cv3_module_16;

typedef struct {
    cv3_seginfo_32      SegInfo;        /* The segment info for the first [code] segment. */
    unsigned_16         ovlNumber;      /* The overlay number. */
    unsigned_16         iLib;           /* The index of the library to which we belong. */
    unsigned_8          cSeg;           /* The number of segment info pieces (includes SegInfo). */
    unsigned_8          reserved;
    unsigned_8          name_len;       /* The name length. */
//    char                name[1];
//  cv3_seginfo_32 arnsg[];
} cv3_module_32;

/* HLL public symbol record. */
typedef struct {
    unsigned_32 offset;
    unsigned_16 seg;
    unsigned_16 type;
    unsigned_8  name_len;
//    char        name[];
} hll_public;
typedef hll_public hll_public_32; /* remove me */

/* CV3 16-bit public symbol record. */
typedef struct {
    unsigned_16 offset;
    unsigned_16 seg;
    unsigned_16 type;
    unsigned_8  name_len;
//    char        name[1];
} cv3_public_16;

/* CV3 32-bit public symbol record. (same as HLL) */
typedef hll_public cv3_public_32;

/* Union of the different public records. */
typedef union {
    hll_public      hll;                /* HLL and 32-bit CV3 */
    cv3_public_16   cv3;                /* 16-bit CV3 */
} hll_public_all;

typedef struct {
//    char      name[1];
    unsigned_16 seg;
    unsigned_16 cPair;
//      line_offset_parms[1];
} cv_linnum_seg;

/* obsolete */
typedef struct {
    unsigned_16     line;
    unsigned_16     offset;
} cv_srcln_off_16;

/* CV3 16-bit line number entry. (hll_sstSrcLnSeg & hll_sstSrcLines) */
typedef struct {
    unsigned_16 line;
    unsigned_16 offset;
} cv3_linnum_entry_16;

/* CV3 32-bit line number entry. (hll_sstSrcLnSeg) */
typedef struct {
    unsigned_16 line;
    unsigned_32 offset;
} cv3_linnum_entry_32;

/* HL1 & HL2 line number entry. (hll_sstHLLSrc) */
typedef struct {
    unsigned_16 line;
    unsigned_8  sfi;                    /* source file index */
    unsigned_8  flags;
    unsigned_32 offset;
} hl1_linnum_entry;

/* HL3+ line number entry. (hll_sstHLLSrc) */
typedef struct {
    unsigned_16 line;
    unsigned_16 sfi;                    /* source file index */
    unsigned_32 offset;
} hl3_linnum_entry;


/* HL1+ file table header. */
typedef struct {
    unsigned_32 first_sfi;              /* The index of the first file. */
    unsigned_32 num_src_files;          /* The number of primary source files. */
    unsigned_32 num_files;              /* The number of source and listing files.  */
} hl1_filetab_hdr;


/* HL1+ path table entry. */
typedef struct {
    unsigned_32 offset;
    unsigned_16 code;
} hl1_pathtab_entry;


/* HLL line number entry type. */
typedef enum {
    HLL_LNE_TYPE_SOURCE = 0,            /* Source file line number table. */
    HLL_LNE_TYPE_LISTING,               /* Listing file line number table. */
    HLL_LNE_TYPE_SOURCES_AND_LISTING,   /* Source and Listing files line number table. */
    HLL_LNE_TYPE_FILE_TABLE,            /* File table. (HLL V4) */
    HLL_LNE_TYPE_PATH_TABLE             /* Path talbe. (HLL V4) */
} hll_linnum_entry_type;

/* HL1+ first line number entry. */
typedef struct {
    unsigned_16 line;                   /* Ignored, 0. */
    unsigned_8  entry_type;             /* The entry type. */
    unsigned_8  reserved;               /* Reserved */
    unsigned_16 num_line_entries;       /* Number of line numbers following this struct. */
    unsigned_16 num_path_entries;       /* Number of path table entries following the line numbers. */
                                        /* Following the path table is a hl1_filetab_hdr. */
} hl1_linnum_first;

/* HL2 & HL3 first line number entry.
 * Seems there is no path table for HL2 & HL3. */
typedef struct {
    unsigned_16 line;                   /* Ignored, 0. */
    unsigned_8  entry_type;             /* The entry type. */
    unsigned_8  reserved;               /* Reserved */
    unsigned_16 num_line_entries;       /* Number of line numbers following this struct. */
    unsigned_16 seg;                    /* The segment number. Reserved for HL4  */
    unsigned_32 file_tab_size;          /* The size of the file table following the line numbers. */
                                        /* The file table starts with a hl1_filetab_hdr just like HL1. */
} hl2_linnum_first;


/* HL4+ first core entry. */
typedef struct {
    unsigned_16 line;                   /* Ignored, 0. */
    unsigned_8  entry_type;             /* The entry type. */
    unsigned_8  reserved;               /* Reserved */
} hl4_linnum_first_core;

/* HL4+ first entry for line numbers (entry types 0, 1 and 2). */
typedef struct {
    hl4_linnum_first_core core;         /* The core record. */
    unsigned_16 num_line_entries;       /* Number of line numbers following this struct. */
    unsigned_16 seg;                    /* The segment number. Reserved for HL4  */
    unsigned_32 base_offset;            /* The base offset of the line numbers.  */
} hl4_linnum_first_lines;


/* HL4+ first entry for line numbers (entry type 3). */
typedef struct {
    hl4_linnum_first_core core;         /* The core record. */
    unsigned_16 num_line_entries;       /* Number of line numbers following this struct. */
    unsigned_16 reserved;               /* Reserved (0). */
    unsigned_32 file_tab_size;          /* The size of the file table. */
} hl4_linnum_first_files;


/* HL4+ first line number entry for path table (entry type 4). */
typedef struct {
    unsigned_16 line;                   /* Ignored, 0. */
    unsigned_8  entry_type;             /* The entry type. */
    unsigned_8  reserved;               /* Reserved */
    unsigned_16 path_tab_entries;       /* Number of path table entries following this struct. */
    unsigned_16 reserved2;              /* Reserved */
} hl4_linnum_first_path;


#if 0
typedef struct {
    unsigned_32 firstChar;
    unsigned_32 numChars;
    unsigned_32 numFiles;
} hl4_filetab_entry;


/* HLL version 3 (HL03) specific line number information */


typedef struct {
    unsigned_32 srcStart;
    unsigned_32 numRecords;
    unsigned_32 numFiles;
} hl3_filetab_entry;
#endif


/* Symbol Scope Table Sub-Record Codes. */
typedef enum SSR {
    HLL_SSR_BEGIN = 0,          /* Begin block ({). */
    HLL_SSR_PROC,               /* Procedure. */
    HLL_SSR_END,                /* End block (}). */
    HLL_SSR_AUTO = 4,           /* Automatic (stack) variable. */
    HLL_SSR_STATIC,             /* Static variable. */
    HLL_SSR_TLS,                /* Thread Local Storage variable. */
    HLL_SSR_CODE_LABEL = 0xb,   /* Code Label. */
    HLL_SSR_WITH,               /* With Start Symbol (PASCAL with?). */
    HLL_SSR_REG,                /* Register variable. */
    HLL_SSR_CONSTANT,           /* Constant symbol. (PL/X-86) */
    HLL_SSR_ENTRY,              /* Secondary entry. (PL/X-86) */
    HLL_SSR_SKIP,               /* Skip record (incremental link). */
    HLL_SSR_CHANGE_SEG,         /* Change default segment. */
    HLL_SSR_TYPEDEF,            /* Typedef. */
    HLL_SSR_PUBLIC,             /* Global (see PUBDEF in OMF). */
    HLL_SSR_MEMBER,             /* Structure member. */
    HLL_SSR_BASED,              /* Based variables. */
    HLL_SSR_TAG,                /* Tag (structure/union/++) */
    HLL_SSR_TABLE,              /* Table or similar. */
    HLL_SSR_MAP,                /* Map external varable (C). */
    HLL_SSR_TAG2,               /* Extended SSR_TAG (long name). */
    HLL_SSR_MEM_FUNC = 0x1a,    /* Member function (method). */
    HLL_SSR_AUTO_SCOPED,        /* Scoped automatic variable (C++). */
    HLL_SSR_STATIC_SCOPED,      /* Scoped static variable (C++). */
    HLL_SSR_PROC2,              /* Extended SSR_PROC (long name). */
    HLL_SSR_STATIC2,            /* Extended SSR_STATIC (long name). */
    HLL_SSR_REG_RELATIVE = 0x20,/* Register relative */
    HLL_SSR_BASED_MEMBER,       /* Based with offset variable. */
    HLL_SSR_ARRAY_SYM,          /* Array symbol record. */
    HLL_SSR_CU_INFO = 0x40,     /* Compiler unit information. */
    HLL_SSR_CU_FUNC_NUM         /* Compiler unit function name list. */
} hll_ssr;


/* HLL language types (for hll_cuinfo::language). */
typedef enum {
    HLL_LANG_C = 1,
    HLL_LANG_CPP,
    HLL_LANG_PLX86,
    HLL_LANG_PL1,
    HLL_LANG_RPG,
    HLL_LANG_COBOL,
    HLL_LANG_ALP,
    HLL_LANG_JAVA
} hll_lang;


/* common to all symbol scope table records. */
typedef struct {
    unsigned_8      code;               /* hll_ssr */
} hll_ssr_common;

/* HLL begin block ({). */
typedef struct {
    unsigned_32     offset;             /* Offset into the current segment. */
    unsigned_32     len;                /* Length of the block. */
    unsigned_8      name_len;           /* Length of the block name (usually 0). */
    char            name[];             /* Block name. Usually not present. */
} hll_ssr_begin;

/* HLL near/far/16/32 flags used by hll_ssr_proc, hll_ssr_proc2,
 * hll_ssr_mem_func, and hll_ssr_code_label. */
typedef enum {
    HLL_SSR_PROC_FAR = 4,               /* Far (set) / near (clear). */
    HLL_SSR_PROC_32BIT = 8,             /* 32-bit (set) / 16-bit (clear). */
} hll_ssr_near_far;

/* HLL procedure, procedure 2 and member function. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Offset into the current segment. */
    unsigned_16     type;               /* The type index of the procedure. */
    unsigned_32     len;                /* Length of the procedure. */
    unsigned_16     prologue_len;       /* Length of the prologue. */
    unsigned_32     prologue_body_len;  /* Length of the prologue and body. */
    unsigned_16     class_type;         /* The type index of the class. */
    unsigned_8      flags;              /* Near/far 16/32-bit flags (hll_ssr_near_far). */
    unsigned_8      name_len;           /* Length of the procedure name.
                                           For PROC2 and MEM_FUNC this can
                                           span two bytes. */
    char            name[];             /* Procedure name. */
} hll_ssr_proc, hll_ssr_proc2, hll_ssr_mem_func;

/* HLL end block (}). */
typedef struct {
    hll_ssr_common  common;
} hll_ssr_end;

/* HLL auto (stack) variable. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Offset into the stack frame. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_auto;

/* HLL static and static 2. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Segment offset. */
    unsigned_16     seg;                /* Segment index. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. This can span
                                           two bytes for STATIC2. */
    char            name[];
} hll_ssr_static, hll_ssr_static2;

/* HLL TLS. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Offset into the thread TLS. */
    unsigned_16     reserved;           /* Reserved. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. This can span
                                           two bytes. */
    char            name[];
} hll_ssr_tls;

/* HLL code label. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Offset into the current segment. */
    unsigned_8      flags;              /* Near/far 16/32-bit flags (hll_ssr_near_far). */
    unsigned_8      name_len;           /* The name length.*/
    char            name[];             /* Procedure name. */
} hll_ssr_code_label;

/* HLL x86 register value. */
typedef enum {
    /* 8-bit */
    HLL_REG_AL = 0,
    HLL_REG_CL,
    HLL_REG_DL,
    HLL_REG_BL,
    HLL_REG_AH,
    HLL_REG_CH,
    HLL_REG_DH,
    HLL_REG_BH,
    /* 16-bit */
    HLL_REG_AX = 8,
    HLL_REG_CX,
    HLL_REG_DX,
    HLL_REG_BX,
    HLL_REG_SP,
    HLL_REG_BP,
    HLL_REG_SI,
    HLL_REG_DI,
    /* 32-bit */
    HLL_REG_EAX = 0x10,
    HLL_REG_ECX,
    HLL_REG_EDX,
    HLL_REG_EBX,
    HLL_REG_ESP,
    HLL_REG_EBP,
    HLL_REG_ESI,
    HLL_REG_EDI,
    /* seg */
    HLL_REG_ES = 0x18,
    HLL_REG_CS,
    HLL_REG_SS,
    HLL_REG_DS,
    HLL_REG_FS,
    HLL_REG_GS,
    /* others/specials */
    HLL_REG_DX_AX = 0x20,
    HLL_REG_ES_BX,
    HLL_REG_IP,
    HLL_REG_FLAGS,
    HLL_REG_EFLAGS,
    /* x87 */
    HLL_REG_ST0 = 0x80,
    HLL_REG_ST1,
    HLL_REG_ST2,
    HLL_REG_ST3,
    HLL_REG_ST4,
    HLL_REG_ST5,
    HLL_REG_ST6,
    HLL_REG_ST7
} hll_reg;

/* HLL register variable. */
typedef struct {
    hll_ssr_common  common;
    unsigned_16     type;               /* The type index. */
    unsigned_8      reg;                /* Register number (hll_reg) */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_reg;

/* HLL register relative variable. */
typedef struct {
    hll_ssr_common  common;
    unsigned_8      reg;                /* Register number (hll_reg) */
    unsigned_32     offset;             /* Offset relative to the register. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_reg_relative;

/* HLL constant. */
typedef struct {
    hll_ssr_common  common;
    unsigned_16     type;               /* The type index. */
    unsigned_8      val_len;            /* The constant value length. */
    unsigned_8      val[1];             /* The constant value. */
    //unsigned_8      name_len;           /* The name length. */
    //char            name[];
} hll_ssr_constant;

/* HLL change segment. */
typedef struct {
    hll_ssr_common  common;
    unsigned_16     seg;                /* The new default segment index. */
    //unsigned_8      reserved;         /* FIXME: don't know the size! */
} hll_ssr_change_seg;

/* HLL typedef. */
typedef struct {
    hll_ssr_common  common;
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_typedef;

/* HLL public symbol, duplication of PUBDEF. (never seen used) */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Segment offset. */
    unsigned_16     seg;                /* Segment index. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_public;

/* HLL member. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     off_sub_rec;        /* Offset to sub-record. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_member;

/* HLL based. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     off_sub_rec;        /* Offset to sub-record. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_based;

/* HLL based with member. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     off_sub_rec;        /* Offset to sub-record. */
    unsigned_16     type;               /* The type index. */
    unsigned_32     off_based;          /* Offset from based. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_based_member;


/* HLL array. */
typedef struct {
    hll_ssr_common  common;
    unsigned_16     type;               /* The type index. */
    unsigned_32     off_odo;            /* Offset to symbol of ODO object. */
} hll_ssr_array;

/* HLL tag and tag 2. */
typedef struct {
    hll_ssr_common  common;
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. This can span
                                           two bytes for TAG2. */
    char            name[];
} hll_ssr_tag, hll_ssr_tag2;

/* HLL map. */
typedef struct {
    hll_ssr_common  common;
    unsigned_8      name_len;           /* The name length. */
    char            name[];
    //unsigned_8      known_name_len;   /* The known name length. */
    //char            known_name[];     /* The known name. */
} hll_ssr_map;

/* HLL table. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Segment offset. */
    unsigned_16     seg;                /* Segment index. */
    unsigned_16     type;               /* The type index. */
    unsigned_32     idx_offset;         /* Index offset to subrecord. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_table;

/* HLL Compiled unit information record. */
typedef struct {
    hll_ssr_common  common;
    unsigned_8      language;       /* hll_lang */
    unsigned_8      options_len;
    char            options[1];
    // unsigned_8      compiler_date_len;
    // char            compiler_date[1];
    // DATETIME        timestamp; /* DosGetDateTime() */
} hll_ssr_cuinfo;

/* HLL auto scoped. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Offset into stackframe. */
    unsigned_16     sfi;                /* Source file index. */
    unsigned_16     line;               /* Source line number. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_auto_scoped;

/* HLL static scoped. */
typedef struct {
    hll_ssr_common  common;
    unsigned_32     offset;             /* Segment offset. */
    unsigned_16     seg;                /* Segment index. */
    unsigned_16     sfi;                /* Source file index. */
    unsigned_16     line;               /* Source line number. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} hll_ssr_static_scoped;

/* union of all the HLL symbol scope table records. */
typedef union {
    hll_ssr_common          common;
    hll_ssr_begin           begin;
    hll_ssr_proc            proc;
    hll_ssr_proc2           proc2;
    hll_ssr_mem_func        mem_func;
    hll_ssr_end             end;
    hll_ssr_auto            auto_;
    hll_ssr_static          static_;
    hll_ssr_static2         static2;
    hll_ssr_tls             tls;
    hll_ssr_code_label      code_lable;
    hll_ssr_reg             reg;
    hll_ssr_reg_relative    reg_relative;
    hll_ssr_constant        constant;
    hll_ssr_change_seg      change_seg;
    hll_ssr_typedef         typedef_;
    hll_ssr_public          public_;
    hll_ssr_member          member;
    hll_ssr_based           based;
    hll_ssr_based_member    based_member;
    hll_ssr_array           array;
    hll_ssr_tag             tag;
    hll_ssr_tag2            tag2;
    hll_ssr_map             map;
    hll_ssr_table           table;
    hll_ssr_cuinfo          cuinfo;
    hll_ssr_auto_scoped     auto_scoped;
    hll_ssr_static_scoped   static_scoped;
} hll_ssr_all;


/* CodeView Symbol Scope Table Data. */


/* Symbol Scope Table Sub-Record Codes are identical to HLL. */

/* common to all symbol scope table records. */
typedef struct {
    unsigned_8      length;
    unsigned_8      code;               /* hll_ssr */
} cv3_ssr_common;

/* CV3 16-bit begin block ({). */
typedef struct {
    cv3_ssr_common  common;
    unsigned_16     offset;             /* Offset into the current segment. */
    unsigned_16     len;                /* Length of the block. */
} cv3_ssr_begin;

/* CV3 16-bit procedure. */
typedef struct {
    cv3_ssr_common  common;
    unsigned_16     offset;             /* Offset into the current segment. */
    unsigned_16     type;               /* The type index of the procedure. */
    unsigned_16     len;                /* Length of the procedure. */
    unsigned_16     prologue_len;       /* Length of the prologue. */
    unsigned_16     prologue_body_len;  /* Length of the prologue and body. */
    unsigned_16     reserved;           /* Not used? */
    unsigned_8      flags;              /* Near/far flag. */
    unsigned_8      name_len;           /* Length of the procedure name. */
    char            name[];             /* Procedure name. */
} cv3_ssr_proc;

/* CV3 end block (}). */
typedef struct {
    cv3_ssr_common  common;
} cv3_ssr_end;

/* CV3 16-bit auto (stack) variable. */
typedef struct {
    cv3_ssr_common  common;
    unsigned_16     offset;             /* Offset into the stack frame. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} cv3_ssr_auto;

/* CV3 16-bit static variable. */
typedef struct {
    cv3_ssr_common  common;
    unsigned_16     offset;             /* Segment offset. */
    unsigned_16     seg;                /* Segment index. */
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} cv3_ssr_static;

/* CV3 16-bit register variable. */
typedef struct {
    cv3_ssr_common  common;
    unsigned_16     type;               /* The type index. */
    unsigned_8      reg;                /* Register number (hll_reg) */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} cv3_ssr_reg;

/* CV3 change segment. */
typedef struct {
    cv3_ssr_common  common;
    unsigned_16     seg;                /* The new default segment index. */
    unsigned_16     reserved;           /* Reserved. */
} cv3_ssr_change_seg;

/* CV3 typedef. */
typedef struct {
    cv3_ssr_common  common;
    unsigned_16     type;               /* The type index. */
    unsigned_8      name_len;           /* The name length. */
    char            name[];
} cv3_ssr_typedef;

/* union of all the CV3 symbol scope table records. */
typedef union {
    cv3_ssr_common          common;
    cv3_ssr_begin           begin;
    cv3_ssr_proc            proc;
    cv3_ssr_end             end;
    cv3_ssr_auto            auto_;
    cv3_ssr_static          static_;
    cv3_ssr_reg             reg;
    cv3_ssr_change_seg      change_seg;
    cv3_ssr_typedef         typedef_;
} cv3_ssr_all;


/* CodeView Type Table Data. */

/* common to all type table records. */
typedef struct {
    unsigned_8      rec_id;             /* Record id, always 0x01. */
    unsigned_16     length;             /* The length of type record. */
    unsigned_8      type;               /* The kind of type record. */
} cv3_lf_common;

/* CV3 bitfield type. */
typedef struct {
    unsigned_8      length;             /* Bitfield length (in bits). */
    unsigned_8      type;               /* Base type of bitfield. */
    unsigned_8      position;           /* Bitfield offset (in bits). */
} cv3_lf_bitfield;

/* union of all the CV3 symbol type leaf records. */
typedef union {
    cv3_lf_common           common;
    cv3_lf_bitfield         bitfield;
} cv3_lf_all;


#pragma pack()

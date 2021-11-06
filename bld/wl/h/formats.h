/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal data structures for various executable formats.
*
****************************************************************************/



typedef enum {          // there is a corresp. table in MSG.C
    MK_OS2_NE           = CONSTU32( 0x00000001 ),
    MK_OS2_LE           = CONSTU32( 0x00000002 ),
    MK_OS2_LX           = CONSTU32( 0x00000004 ),
    MK_WINDOWS          = CONSTU32( 0x00000008 ),
    MK_PE               = CONSTU32( 0x00000010 ),
    MK_DOS_EXE          = CONSTU32( 0x00000020 ),
    MK_COM              = CONSTU32( 0x00000040 ),
    MK_OVERLAYS         = CONSTU32( 0x00000080 ),
    MK_NOVELL           = CONSTU32( 0x00000100 ),
    MK_QNX_16           = CONSTU32( 0x00000200 ),   // POSIX QNX, not ICON QNX
    MK_PHAR_SIMPLE      = CONSTU32( 0x00000400 ),
    MK_PHAR_FLAT        = CONSTU32( 0x00000800 ),
    MK_PHAR_REX         = CONSTU32( 0x00001000 ),
    MK_PHAR_MULTISEG    = CONSTU32( 0x00002000 ),
    MK_QNX_FLAT         = CONSTU32( 0x00004000 ),
    MK_ELF              = CONSTU32( 0x00008000 ),
    MK_WIN_VXD          = CONSTU32( 0x00010000 ),
    MK_DOS16M           = CONSTU32( 0x00020000 ),
    MK_ZDOS             = CONSTU32( 0x00040000 ),
    MK_RAW              = CONSTU32( 0x00080000 ),
    MK_RDOS_32          = CONSTU32( 0x00100000 ),
    MK_RDOS_16          = CONSTU32( 0x00200000 ),
    MK_ALL              = CONSTU32( 0x003FFFFF ),
} exe_format;

#define MK_DOS          (MK_OVERLAYS | MK_DOS_EXE | MK_COM)
#define MK_ONLY_OS2_16  MK_OS2_NE
#define MK_OS2_FLAT     (MK_OS2_LE | MK_OS2_LX | MK_WIN_VXD)
#define MK_ONLY_OS2     (MK_ONLY_OS2_16 | MK_OS2_LE | MK_OS2_LX)
#define MK_OS2_16BIT    (MK_ONLY_OS2_16 | MK_WINDOWS)
/* MK_WIN_VXD is not included into MK_OS2 */
#define MK_OS2          (MK_OS2_16BIT | MK_OS2_LE | MK_OS2_LX)
#define MK_PHAR_LAP     (MK_PHAR_SIMPLE | MK_PHAR_FLAT | MK_PHAR_REX | MK_PHAR_MULTISEG)
#define MK_QNX          (MK_QNX_16 | MK_QNX_FLAT)
#define MK_RDOS         (MK_RDOS_16 | MK_RDOS_32)
#define MK_64BIT        (MK_PE | MK_ELF)
#define MK_32BIT        (MK_PHAR_LAP | MK_NOVELL | MK_QNX | MK_OS2_LE | MK_OS2_LX | MK_PE | MK_ELF | MK_WIN_VXD | MK_ZDOS | MK_RAW | MK_RDOS_32)
#define MK_16BIT        (MK_DOS | MK_OS2_16BIT | MK_DOS16M | MK_RDOS_16)
/* MK_OS2_LE, MK_OS2_LX, MK_WIN_VXD and MK_PE are not treated as FLAT internally */
#define MK_FLAT_OFFS    (MK_PHAR_SIMPLE | MK_PHAR_FLAT | MK_PHAR_REX | MK_ZDOS | MK_RAW)
#define MK_ALLOW_64     (MK_64BIT)
#define MK_ALLOW_32     (MK_32BIT)
#define MK_ALLOW_16     (MK_16BIT | MK_PHAR_FLAT | MK_OS2 | MK_QNX | MK_PE | MK_WIN_VXD | MK_RAW)
#define MK_ID_SPLIT     (MK_NOVELL)
#define MK_REAL_MODE    (MK_DOS)
#define MK_PROT_MODE    (~MK_REAL_MODE)
#define MK_SEGMENTED    (MK_16BIT | MK_OS2 | MK_PHAR_MULTISEG | MK_RDOS_16)
#define MK_IMPORTS      (MK_NOVELL | MK_OS2 | MK_PE | MK_ELF)
#define MK_SPLIT_DATA   (MK_ELF | MK_PE)
#define MK_LINEARIZE    (MK_ELF | MK_PE)
#define MK_END_PAD      (MK_DOS)

// linker specific phar lap data

struct fmt_dos_data {
    boolbit             distribute      : 1;
    boolbit             noindirect      : 1;
    boolbit             dynamic         : 1;
    boolbit             ovl_short       : 1;
    boolbit             pad_sections    : 1;
    boolbit             full_mz_hdr     : 1;
};

struct fmt_phar_data {
    unsigned_32         mindata;
    unsigned_32         maxdata;
    // run-time parameter block data
    char                *breaksym;              // name of realbreak symbol
    char                *stub;                  // name of stub file.
    unsigned_32         realbreak;
    unsigned_16         minreal;
    unsigned_16         maxreal;
    unsigned_16         nistack;
    unsigned_16         extender_flags;
    unsigned_8          minibuf;
    unsigned_8          maxibuf;
    unsigned_8          callbufs;
    unsigned_8          istksize;
    boolbit             unpriv      : 1;
//    boolbit             pack        : 1;      // needed if/when .exp packing implemented
};

// linker specific OS/2 formats family data

struct fmt_os2fam_data {
    struct entry_export     *exports;
    obj_name_list           *mod_ref_list;
    obj_name_list           *imp_tab_list;
    char                    *stub_file_name;
    char                    *module_name;
    char                    *old_lib_name;
    struct os2_seg_flags    *seg_flags;
    unsigned_32             heapsize;
    unsigned                segment_shift;
    unsigned                flags;              // in LOADOS2.H
    boolbit                 chk_seg_relocs : 1;
    boolbit                 toggle_relocs  : 1;
    boolbit                 gen_int_relocs : 1;
    boolbit                 gen_rel_relocs : 1;
    boolbit                 is_private_dll : 1;
    boolbit                 no_stub        : 1;
    boolbit                 mixed1632      : 1;
};

// linker specific OS/2 data

struct fmt_os2_data {
    struct fmt_os2fam_data  os2fam;         /* must be first field */
};

// linker specific PE data

struct fmt_pe_data {
    struct fmt_os2fam_data  os2fam;         /* must be first field */
    list_of_names           *resources;
    unsigned_32             heapcommit;
    unsigned_32             stackcommit;
    unsigned                subsystem;
    unsigned_16             submajor;
    unsigned_16             subminor;
    unsigned_16             osmajor;        /*  OS major version    */
    unsigned_16             osminor;        /*  OS minor version    */
    unsigned_8              linkmajor;      /*  link major version  */
    unsigned_8              linkminor;      /*  link minor version  */
    boolbit                 tnt                 : 1;
    boolbit                 sub_specd           : 1;
    boolbit                 no_stdcall          : 1;
    boolbit                 osv_specd           : 1;    /* OS version specified? */
    boolbit                 lnk_specd           : 1;    /* Link version specified */
    boolbit                 checksumfile        : 1;    /* Create checksum for file? */
    boolbit                 largeaddressaware   : 1;
    boolbit                 nolargeaddressaware : 1;
};

// structures used in processing DOS/16M load files.

struct fmt_d16m_data {
    unsigned_16             options;
    unsigned_8              flags;              // in load16m.h
    unsigned_8              strategy;
    unsigned_16             buffer;
    unsigned_16             gdtsize;
    unsigned_16             selstart;
    unsigned_16             extended;
    unsigned_16             datasize;
    char                    *exp_name;          // original EXP file name.
    char                    *stub;              // name of stub file.
};

// stuff common to some file formats which have the concept of an export

struct exp_common {
    obj_name_list           *export;
    obj_name_list           *module;
};

// linker specific Novell NLM data

struct fmt_nov_data {
    struct exp_common       exp;                // must be at the start
    char                    *screenname;        // actually a length byte then a string
    char                    *checkfn;           // check function name;
    char                    *exitfn;            // exit function name;
    char                    *customdata;        // custom data file name;
    char                    *threadname;
    char                    *copyright;
    char                    *messages;
    char                    *help;
    char                    *rpcdata;
    char                    *sharednlm;
    unsigned                moduletype;
    unsigned                flags;
    unsigned_32             exeflags;
};

// so we don't have to allocate any memory for imports which are not
// referenced, make them initially point to some other aribtrary non-zero place

#define DUMMY_IMPORT_PTR    (void *)&FmtData.u.nov

// linker specific QNX 4.0 data

struct fmt_qnx_data {
    struct qnx_seg_flags    *seg_flags;
    unsigned_32             heapsize;
    unsigned                flags;
    unsigned                priv_level;
    boolbit                 gen_seg_relocs      : 1;
    boolbit                 gen_linear_relocs   : 1;
    boolbit                 seen_mismatch       : 1;
};

// linker specific ELF data

struct fmt_elf_data {
    struct exp_common       exp;                // must be at the start
    unsigned long           alignment;
    unsigned_32             extrasects;
    unsigned_8              abitype;            // EI_OSABI contents
    unsigned_8              abiversion;         // EI_ABIVERSION contents
    boolbit                 exportallsyms : 1;
};

// linker specific RDOS device driver data

struct fmt_rdos_data {
    unsigned_32             code_sel;
    unsigned_32             data_sel;
    segment                 code_seg;
    segment                 data_seg;
    boolbit                 driver  : 1;
    boolbit                 mboot   : 1;
};

#define NO_BASE_SPEC    ((offset)-1L)

struct fmt_data {
    union   fmt_spec_data {
        struct  fmt_dos_data    dos;
        struct  fmt_os2fam_data os2fam;
        struct  fmt_os2_data    os2;
        struct  fmt_pe_data     pe;
        struct  fmt_d16m_data   d16m;
        struct  fmt_phar_data   phar;
        struct  fmt_nov_data    nov;
        struct  fmt_qnx_data    qnx;
        struct  fmt_elf_data    elf;
        struct  fmt_rdos_data   rdos;
    }               u;
    seg_leader      *dgroupsplitseg;
    offset          bsspad;
    offset          base;
    offset          objalign;
    char            *implibname;
    char            *osname;
    char            *resource;
    char            *description;
    int             cpu_type;       /* '0' => 86, '1' => 186, etc */
    exe_format      type;
    unsigned        major;
    unsigned        minor;
    unsigned        revision;
    unsigned        def_seg_flags;
    unsigned        Hshift;         // Corresponds to huge shift variable used by libr
    unsigned        SegShift;       // 16 - HShift, used to convert a segment to an address
    unsigned_32     SegMask;        // used to extract remainder for segment normalization
    unsigned        HexSegShift;    // shift to convert Intel Hex record segments to address
    unsigned_32     output_offset;
    char            FillChar;
    boolbit         dll             : 1;
    boolbit         ver_specified   : 1;
    boolbit         make_implib     : 1;
    boolbit         make_impfile    : 1;
    boolbit         res_name_only   : 1;
    boolbit         toc_initialized : 1;
    boolbit         output_raw      : 1;
    boolbit         output_hex      : 1;
    boolbit         output_hshift   : 1;    // Hexout uses HexSegShift (else uses SegShift)
    boolbit         output_start    : 1;    // If Hexout should provide a start record
    boolbit         raw_hex_output  : 1;
};

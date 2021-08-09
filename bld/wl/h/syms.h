/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linker symbol table structures.
*
****************************************************************************/


typedef enum {

// These constants define action to take with the symbol table

    ST_STATIC                = 0x01,         // this is a static symbol
    ST_CREATE                = 0x02,         // create the symbol if not found
    ST_NOALIAS               = 0x04,         // don't expand aliases
    ST_FIND                  = 0x08,         // just looking for the symbol
    ST_REFERENCE             = 0x10,         // mark symbol as referenced
    ST_DEFINE                = 0x20,         // mark symbol as defined
    ST_NONUNIQUE             = 0x40,         // duplicate names allowed
    ST_CREATE_DEFINE_NOALIAS = ST_CREATE | ST_DEFINE | ST_NOALIAS,
    ST_CREATE_REFERENCE      = ST_CREATE | ST_REFERENCE
} sym_flags;

typedef enum {

// These constants define the meaning of the bits in the symbol "info" field
// The values 0-15 are used for the type of the symbol
// SYM_OLDHAT is only used in pass 1, and sym_checked is only used in pass 1.
// SYM_MAP_GLOBAL is also used inbetween pass 1 and pass 2 to keep track of
// symbols which are going in the map file (for sort global command)
// SYM_KILL & SYM_RELOC_REFD only used before pass 1

// Here is what is put in the first four bits of the info field.

    SYM_COMMUNAL_16     = 0,    // symbol is a 16-bit communal symbol
    SYM_COMMUNAL_32     = 1,    // symbol is a 32-bit communal symbol
    SYM_COMDAT          = 2,    // symbol is an initialized communal.
    SYM_REGULAR         = 3,    // a good, old-fashioned linker symbol
    SYM_ALIAS           = 4,    // this is an alias for another symbol.
    SYM_IMPORTED        = 5,    // symbol is imported (OS/2 & Novell).
    SYM_GROUP           = 6,    // symbol is attached to a group
    SYM_LAZY_REF        = 7,    // symbol is a "lazy" reference.
    SYM_WEAK_REF        = 8,    // symbol is a "weak" reference.
    SYM_LINK_WEAK_REF   = 9,    // symbol is a linker-generated weak reference
    SYM_VF_REF          = 10,   // symbol is a virtual function table reference
    SYM_PURE_REF        = 11,   // symbol is a pure virt. func. table reference

// Here is all other values except the first four bits of the info field.

    SYM_DEAD            = CONSTU32( 0x00000010 ),   // symbol has been eliminated.
    SYM_FREE_ALIAS      = CONSTU32( 0x00000010 ),   // used for aliases only.
    SYM_OLDHAT          = CONSTU32( 0x00000020 ),   // symbol referenced in a previous obj
    SYM_REFERENCED      = CONSTU32( 0x00000040 ),   // used to mark unref'd symbols in map
    SYM_CHECKED         = CONSTU32( 0x00000080 ),   // symbol has been checked by FindUndefined
    SYM_MAP_GLOBAL      = CONSTU32( 0x00000080 ),   // put sym in global map symbol list
    SYM_STATIC          = CONSTU32( 0x00000100 ),   // non-global symbol
    SYM_LOCAL_REF       = CONSTU32( 0x00000200 ),   // symbol referenced locally.
    SYM_TRACE           = CONSTU32( 0x00000400 ),   // keep track of references to this symbol
    SYM_IN_CURRENT      = CONSTU32( 0x00000800 ),   // symbol seen in current module
    SYM_WAS_LAZY        = CONSTU32( 0x00000800 ),   // used for aliases only
    SYM_DEFINED         = CONSTU32( 0x00001000 ),   // symbol defined.
    SYM_ABSOLUTE        = CONSTU32( 0x00002000 ),   // symbol is absolute
    SYM_EXPORTED        = CONSTU32( 0x00008000 ),   // symbol has been exported
    SYM_CDAT_SEL_NODUP  = CONSTU32( 0x00000000 ),   // do not allow duplicates
    SYM_CDAT_SEL_ANY    = CONSTU32( 0x00010000 ),
    SYM_CDAT_SEL_SIZE   = CONSTU32( 0x00020000 ),
    SYM_CDAT_SEL_EXACT  = CONSTU32( 0x00030000 ),
    SYM_RELOC_REFD      = CONSTU32( 0x00040000 ),   // symbol referenced by a stored reloc
    SYM_KILL            = CONSTU32( 0x00080000 ),   // remove symbol
    SYM_IS_ALTDEF       = CONSTU32( 0x00100000 ),   // symbol is an altdef symbol.
    SYM_FAR_COMMUNAL    = CONSTU32( 0x00200000 ),   // is a far communal symbol
    SYM_LINK_GEN        = CONSTU32( 0x00400000 ),   // is a linker generated symbol
    SYM_HAS_DATA        = CONSTU32( 0x00800000 ),   // only used for altdefs
    SYM_NAME_XLATED     = CONSTU32( 0x01000000 ),   // only used during permdata writing
    SYM_IS_FREE         = CONSTU32( 0x10000000 ),   // only used during permdata writing.

// the top three bits are used for the floating point patch type during pass 2
// SYM_DCE_REF, SYM_VF_REFS_DONE and SYM_VF_MARKED are only needed during
// pass 1 and before the syms are checked for.
// SYM_VF_MARKED can be used at the same time as SYM_DISTRIB since
// SYM_DISTRIB is only set when SYM_DEFINED is on, and SYM_VF_MARKED is only
// needed when the symbol is not defined.

    SYM_DISTRIB         = CONSTU32( 0x20000000 ),   // DOS ONLY: symbol is in a distrib. lib.
    SYM_VF_MARKED       = CONSTU32( 0x20000000 ),   // vf reference record seen for this sym.
    SYM_DCE_REF         = CONSTU32( 0x40000000 ),   // referenced for the purposes of dead code
    SYM_VF_REFS_DONE    = CONSTU32( 0x80000000 ),   // ALL: vf refs added to call graph
} sym_info;

// values used to keep track of the special floating point patch symbols.
typedef enum {
    FPP_NONE = 0,
    // value 1...6 are encoded FPP patch system values, see bld/watcom/h/fppatche.h
    FPP_IGNORE = 7
} fix_fpp_type;

#define FPP_MASK            7           // floating point patch value is 3 bits wide

#define SYM_FPP_SHIFT       (32 - 3)    // floating point patch value is top 3 bits
#define SYM_FPP_MASK        (FPP_MASK << SYM_FPP_SHIFT)
#define SYM_FPP_NONE        (FPP_NONE << SYM_FPP_SHIFT)

// some handy macros for checking and setting symbol type bits

#define SYM_CDAT_SEL_MASK   (SYM_CDAT_SEL_NODUP | SYM_CDAT_SEL_ANY | SYM_CDAT_SEL_SIZE | SYM_CDAT_SEL_EXACT)
#define SYM_CDAT_SEL_SHIFT  16

#define SYM_CLEAR_ON_P2  /* 0xE00000A0 flags to clear before pass 2 starts. */ \
    (SYM_OLDHAT | SYM_CHECKED | SYM_MAP_GLOBAL | SYM_FPP_MASK)
#define SYM_CLEAR_ON_INC /* 0x010404F0 flags to clear when incremental linking. */ \
    (SYM_DEAD | SYM_FREE_ALIAS | SYM_OLDHAT | SYM_REFERENCED | SYM_CHECKED | SYM_MAP_GLOBAL | SYM_TRACE | SYM_RELOC_REFD | SYM_NAME_XLATED)
#define SYM_CLEAR_ON_ALT /* 0x00980010 */ \
    (SYM_DEAD | SYM_FREE_ALIAS | SYM_KILL | SYM_IS_ALTDEF | SYM_HAS_DATA)

#define SYM_TYPE_MASK       0xF

#define IS_SYM_COMMUNAL(sym)    (((sym)->info & SYM_TYPE_MASK) <= SYM_COMDAT)
#define IS_SYM_NICOMDEF(sym)    (((sym)->info & SYM_TYPE_MASK) <= SYM_COMMUNAL_32)
#define IS_SYM_COMDAT(sym)      (((sym)->info & SYM_TYPE_MASK) == SYM_COMDAT)
#define IS_SYM_ALIAS(sym)       (((sym)->info & SYM_TYPE_MASK) == SYM_ALIAS)
#define IS_SYM_IMPORTED(sym)    (((sym)->info & SYM_TYPE_MASK) == SYM_IMPORTED)
#define IS_SYM_GROUP(sym)       (((sym)->info & SYM_TYPE_MASK) == SYM_GROUP)
#define IS_SYM_REGULAR(sym)     (((sym)->info & SYM_TYPE_MASK) == SYM_REGULAR)
#define IS_SYM_VF_REF(sym)      (((sym)->info & SYM_TYPE_MASK) >= SYM_VF_REF)
#define IS_SYM_PURE_REF(sym)    (((sym)->info & SYM_TYPE_MASK) == SYM_PURE_REF)
#define IS_SYM_LAZY_REF(sym)    (((sym)->info & SYM_TYPE_MASK) == SYM_LAZY_REF)
#define IS_SYM_A_REF(sym)       (((sym)->info & SYM_TYPE_MASK) >= SYM_LAZY_REF)
#define IS_SYM_WEAK_REF(sym)    (((sym)->info & SYM_TYPE_MASK) >= SYM_WEAK_REF)
#define IS_SYM_LINK_WEAK(sym)   (((sym)->info & SYM_TYPE_MASK) == SYM_LINK_WEAK_REF)
#define IS_SYM_COMM32(sym)      (((sym)->info & SYM_TYPE_MASK) == SYM_COMMUNAL_32)

#define SET_SYM_TYPE(sym,type)  ((sym)->info = ((sym)->info & ~SYM_TYPE_MASK)|(type))
#define SET_SYM_FPP(sym,value)  ((sym)->info = ((sym)->info & ~SYM_FPP_MASK)|((value) << SYM_FPP_SHIFT))

#define GET_SYM_FPP(sym)        (((sym)->info >> SYM_FPP_SHIFT) & FPP_MASK)

#define SET_SYM_ADDR(sym,o,s)   (sym)->addr.seg=s;(sym)->addr.off=o

/* note that OVL_VECTOR && OVL_FORCE can be thought of as a two-bit field.
 * OVL_NO_VECTOR == 0 && OVL_FORCE == 0 means undecided.
 * OVL_NO_VECTOR == 1 && OVL_FORCE == 0 means tenatively no vector generated.
 * OVL_NO_VECTOR == 0 && OVL_FORCE == 1 means generate a vector
 * OVL_NO_VECTOR == 1 && OVL_FORCE == 1 means do not generate a vector
*/

typedef enum overlay_info {
    OVL_UNDECIDED       = 0x0,
    OVL_NO_VECTOR       = 0x1,          // symbol has a vector
    OVL_FORCE           = 0x2,          // force symbol to have a vector
    OVL_ALWAYS          = 0x4,          // always use vector
    OVL_REF             = 0x8,          // reference number assigned
    OVL_MAKE_VECTOR     = OVL_FORCE,
    OVL_VEC_MASK        = (OVL_NO_VECTOR | OVL_FORCE)
} overlay_info;

typedef struct {
    union {
        char        *ptr;
        size_t      offs;
    } u;
} name_strtab;

typedef struct {
    unsigned_16     modnum;         // DOS: idx of module which defines this sym
    unsigned_16     ovlref;         // DOS: overlay vector #
    overlay_info    ovlstate;       // DOS: overlay vector state
} dos_sym_data;

typedef struct symbol {
    struct symbol       *hash;
    struct symbol       *publink;
    struct symbol       *link;
    targ_addr           addr;
    unsigned_16         namelen_cmp;
    sym_info            info;       // flags & floating point patch type.
    struct mod_entry    *mod;
    union {
        void            *edges;     // for dead code elim. when sym undefd
        struct segdata  *seg;       // seg symbol is in.
        name_strtab     alias;      // for aliased syms.
        void            *import;    // NOVELL & OS/2 only: imported symbol data.
        offset          cdefsize;   // altdef comdefs: size of comdef
    } p;
    union {
        dos_sym_data    d;
        struct symbol   *altdefs;   // for keeping track of comdat & comdef defs
        struct symbol   *datasym;   // altdef comdats: sym which has data def
        size_t          aliaslen;   // for aliases - length of name.
    } u;
    union {
        struct symbol   *mainsym;   // altdefs: main symbol definition
        struct symbol   *def;       // for lazy externs
        struct symbol   **vfdata;   // for virtual function lazy externs.
        void            *export;    // OS/2 & PE only: exported sym info.
    } e;
    name_strtab         name;
    char                *prefix;    // primarily for netware, though could be
                                    // subverted for other use. gives symbol
                                    // namespace qualification
} symbol;

/* function prototypes */

extern void             FiniSym( void );
extern void             CleanSym( void );
extern void             InitSym( void );
extern void             WriteHashPointers( void * );
extern void             ReadHashPointers( void * );
extern void             ClearHashPointers( void );
extern void             SetSymCase( void );
extern void             SymModStart( void );
extern void             SymModEnd( void );
extern void             ClearSymUnion( symbol * );
extern void             ClearRefInfo( symbol * );
extern void             TraceSymList( symbol * );
extern void             MakeSymAlias( const char *, size_t, const char *, size_t );
extern symbol           *MakeWeakExtdef( const char *, symbol * );
extern void             ConvertVFSym( symbol * );
extern void             WeldSyms( symbol *, symbol * );
extern symbol           *UnaliasSym( sym_flags, symbol * );
extern void             ConvertLazyRefs( void );
extern symbol           *RefISymbol( const char * );
extern symbol           *DefISymbol( const char * );
extern symbol           *FindISymbol( const char * );
extern symbol           *SymOpNWPfx( sym_flags, const char *, size_t, const char *, size_t );
extern symbol           *SymOp( sym_flags, const char *, size_t );
extern void             ReportMultiple( symbol *, const char *, size_t );
extern void             ReportUndefined( void );
extern void             WriteCommunals( void );
extern void             XReportSymAddr( symbol * );
extern void             XWriteImports( void );
extern symbol           *AddAltDef( symbol *, sym_info );
extern symbol           *HashReplace( symbol * );
extern void             PurgeSymbols( void );
extern offset           SymbolAbsAddr( symbol * );
extern struct group_entry *SymbolGroup( symbol * );

extern size_t           NameLen;
extern int              (*CmpRtn)( const char *, const char *, size_t );
extern symbol           *LastSym;

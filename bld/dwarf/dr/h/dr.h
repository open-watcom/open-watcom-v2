/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DWARF reader interface definition.
*
****************************************************************************/


#ifndef DR_H_INCLUDED
#define DR_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "watcom.h"
#include "bool.h"
#include "dwarf.h"


#define DRENTRY

#define DRMEM_HDL_VOID  ((drmem_hdl)(pointer_uint)-1)       /* special handle denoting void type */
#define DRMEM_HDL_NULL  ((drmem_hdl)(pointer_uint)0)

#define DWRSetRtns( __read, __seek, __alloc, __realloc, __free, __except ) \
    WDRRoutines DWRRtns = { \
        __read,       \
        __seek,       \
        __alloc,      \
        __realloc,    \
        __free,       \
        __except      \
    };

typedef enum {
    DR_PTR_none,
    DR_PTR_near16,
    DR_PTR_far16,
    DR_PTR_huge16,
    DR_PTR_near32,
    DR_PTR_far32
} dr_ptr;

typedef enum {
    DR_TYPEK_NONE,
    DR_TYPEK_DATA,
    DR_TYPEK_CODE,
    DR_TYPEK_ADDRESS,
    DR_TYPEK_VOID,
    DR_TYPEK_BOOL,
    DR_TYPEK_ENUM,
    DR_TYPEK_CHAR,
    DR_TYPEK_INTEGER,
    DR_TYPEK_REAL,
    DR_TYPEK_COMPLEX,
    DR_TYPEK_STRING,
    DR_TYPEK_POINTER,
    DR_TYPEK_REF,
    DR_TYPEK_STRUCT,
    DR_TYPEK_UNION,
    DR_TYPEK_CLASS,
    DR_TYPEK_ARRAY,
    DR_TYPEK_FUNCTION,
    DR_TYPEK_LAST
} dr_typek;

typedef enum {
    DR_STORE_NONE     = 0,
    DR_STORE_CONST    = 1<<0,
    DR_STORE_VOLATILE = 1<<1,
    DR_STORE_PACKED   = 1<<2,
} dr_store;

typedef enum {
    DR_MOD_NONE,
    DR_MOD_ADDR,
    DR_MOD_BASE
} dr_mod;

enum {
    DR_WLKBLK_STRUCT = 5,
    DR_WLKBLK_ARRSIB = 3,
    DR_WLKBLK_ENUMS  = 2,
};

typedef enum {
    DR_ARRAY_NONE          = 0,
    DR_ARRAY_ORDERING      = 1<<0,
    DR_ARRAY_BYTE_SIZE     = 1<<1,
    DR_ARRAY_STRIDE_SIZE   = 1<<2,
    DR_ARRAY_COUNT         = 1<<3,
} dr_array_stat;

typedef enum {
    DR_VAL_NOT,
    DR_VAL_INT,
    DR_VAL_REF,
} dr_val_class;

typedef enum {      // code in drsearch depends on the order */
    DR_SYM_FUNCTION,
    DR_SYM_CLASS,
    DR_SYM_ENUM,
    DR_SYM_TYPEDEF,
    DR_SYM_VARIABLE,
    DR_SYM_MACRO,
    DR_SYM_LABEL,
    DR_SYM_NOT_SYM      /* not a symbol - must be last */
} dr_sym_type;

typedef enum {
    DR_DEPTH_FUNCTIONS  = 1,
    DR_DEPTH_CLASSES    = 2,
} dr_depth;

typedef enum {
    DR_SEARCH_ALL,
    DR_SEARCH_FUNCTIONS,
    DR_SEARCH_CLASSES,
    DR_SEARCH_TYPES,
    DR_SEARCH_VARIABLES,
    DR_SEARCH_FRIENDS,
    DR_SEARCH_BASE,
    DR_SEARCH_MACROS,
    DR_SEARCH_NOT_SYM   /* should always be last */
} dr_search;

typedef enum {
    DR_TAG_FUNCTION,
    DR_TAG_CLASS,
    DR_TAG_ENUM,
    DR_TAG_TYPEDEF,
    DR_TAG_VARIABLE,
    DR_TAG_LABEL,
    DR_TAG_NAMESPACE,
    DR_TAG_NONE         /* not any of the above */
} dr_tag_type;

// NOTE: these have to correspond to equivalent #defines in dwarf.h!

typedef enum {
    DR_ACCESS_PUBLIC = 1,
    DR_ACCESS_PROTECTED = 2,
    DR_ACCESS_PRIVATE = 3
} dr_access;

typedef enum {
    DR_LANG_UNKNOWN,
    DR_LANG_CPLUSPLUS,
    DR_LANG_FORTRAN,
    DR_LANG_C
} dr_language;

typedef enum {
    DR_MODEL_NONE    = 0,
    DR_MODEL_FLAT    = 1,
    DR_MODEL_SMALL   = 2,
    DR_MODEL_MEDIUM  = 3,
    DR_MODEL_COMPACT = 4,
    DR_MODEL_LARGE   = 5,
    DR_MODEL_HUGE    = 6,
} dr_model;

typedef enum {
    DR_SRCH_func_var,
    DR_SRCH_func,
    DR_SRCH_class,
    DR_SRCH_enum,
    DR_SRCH_typedef,
    DR_SRCH_ctypes,
    DR_SRCH_cpptypes,
    DR_SRCH_var,
    DR_SRCH_parm,
    DR_SRCH_label,
    DR_SRCH_LAST,
} dr_srch;

typedef enum {
    DR_VIRTUALITY_NONE = 0,
    DR_VIRTUALITY_VIRTUAL = 1
} dr_virtuality;

typedef enum {
    DR_DEBUG_INFO,
    DR_DEBUG_PUBNAMES,
    DR_DEBUG_ARANGES,
    DR_DEBUG_LINE,
    DR_DEBUG_LOC,
    DR_DEBUG_ABBREV,
    DR_DEBUG_MACINFO,
    DR_DEBUG_STR,
    DR_DEBUG_REF,
    DR_DEBUG_NUM_SECTS          // the number of debug info sections.
} dr_section;

typedef enum {
    DREXCEP_OUT_OF_VM,
    DREXCEP_BAD_DBG_VERSION,
    DREXCEP_BAD_DBG_INFO,
    DREXCEP_OUT_OF_MMEM,        // DWRMALLOC or DWRREALLOC failed
    DREXCEP_DWARF_LIB_FAIL      // bug in the dwarf library
} dr_except;

typedef enum {
    DR_LOC_NONE,
    DR_LOC_REG,
    DR_LOC_ADDR
} dr_loc_kind;

#if defined( USE_VIRTMEM )
typedef unsigned long   drmem_hdl;
#else
typedef char            *drmem_hdl;
#endif

typedef unsigned_16     filetab_idx;
typedef unsigned        dr_fileidx;

typedef struct dr_dbg_info  *dr_dbg_handle;

typedef struct {
    dr_typek kind;
    uint_32  size;
    dr_store acc;
    dr_mod   mclass;
    union {
        dr_ptr   ptr;
        bool     sign;
        uint_32  beg_scope;
    } modifier;
} dr_typeinfo;

typedef struct {
    uint_32    byte_size;
    uint_32    stride_size;
    uint_32    count;
    dw_ord     ordering;
    drmem_hdl  child;
} dr_array_info;

typedef struct {
    dr_val_class val_class;
    union{
        drmem_hdl ref;
        uint_32   s;
    } val;
} dr_val32;

typedef struct {
    dr_val32 low;
    dr_val32 high;
    dr_val32 count;
} dr_subinfo;

typedef struct {
    dr_val32 byte_size;
    dr_val32 bit_offset;
    dr_val32 bit_size;
} dr_bitfield;

typedef struct {
    drmem_hdl   dbg_cu;
    drmem_hdl   dbg_handle;
    size_t      len;
    char        *name;
    bool        is_start;
} dr_pubname_data;

typedef struct COMPUNIT_INFO    compunit_info;
typedef compunit_info   *dr_cu_handle;

typedef struct {
    uint_32     addr;
    uint_32     len;
    drmem_hdl   dbg;
    uint_16     seg;
    uint_8      addr_size;
    uint_8      seg_size;
    bool        is_start;
} dr_arange_data;

typedef struct { /* current state of stmt prog */
    uint_32 offset;
    uint_32 line;
    uint_16 column;
    uint_16 file;
    uint_16 seg;
    uint_8  is_stmt   :1;
    uint_8  basic_blk :1;
    uint_8  end_seq   :1;
    uint_8  addr_set  :1;
} dr_line_data;

typedef struct {
    uint_16     index;
    char        *name;
} dr_line_dir;

/* walk directory and  file names */
typedef struct {
    uint_16     index;
    char        *name;
    uint_16     dir;
    uint_32     time;
    uint_32     len;
} dr_line_file;

typedef struct {
    int         size;   /* available room */
    int         free;   /* next free entry */
    drmem_hdl   *stack; /* values */
} dr_scope_stack;

typedef struct {
    dr_scope_stack  scope;
    drmem_hdl       dependent;
    char            *file;
    unsigned_32     line;
    unsigned_16     column;
} dr_ref_info;

typedef struct dr_context_stack {
    int         size;   /* available room */
    int         free;   /* next free entry */
    drmem_hdl   *stack; /* values */
} dr_context_stack;

typedef struct dr_search_context {
    dr_cu_handle        compunit;       /* current compile unit */
    drmem_hdl           start;          /* die to start from */
    drmem_hdl           end;            /* end of compile unit */
    drmem_hdl           functionhdl;    /* containing function */
    drmem_hdl           classhdl;       /* containing class */
    dr_context_stack    stack;          /* stack of classes / functions */
} dr_search_context;

typedef struct dr_sym_context {
    drmem_hdl               handle;         /* symbol's handle */
    dr_sym_type             type;           /* symbol's type */
    char                    *name;          /* symbol's name */
    dr_search_context       *context;       /* context to resume search */
} dr_sym_context;

typedef struct {
    dr_loc_kind (*init)( void *, uint_32 * );                            //Init Stk
    bool (*ref)( void *d, uint_32 ref, uint_32 size, dr_loc_kind kind ); //grab ref, size of kind
    bool (*dref)( void *d, uint_32 *, uint_32, uint_32 );                //dref addr
    bool (*drefx)( void *d, uint_32 *, uint_32, uint_32, uint_16 );      //dref addr seg
    bool (*frame)( void *d, uint_32 * );                                 //get frame val
    bool (*reg)( void *d, uint_32 *, uint_16 );                          //get reg val
    bool (*acon)( void *d, uint_32 *, bool );                            //address constant
    bool (*live)( void *d, uint_32 * );                                  //get pc line range
} dr_loc_callbck_def;

typedef dr_loc_callbck_def const dr_loc_callbck;

typedef struct {                                                /* defaults */
/* I/O routines */
    void   (*cli_read)( void *, dr_section, void *, size_t );   // read
    void   (*cli_seek)( void *, dr_section, long );             // lseek
/* memory routines */
    void * (*cli_alloc)( size_t );                              // malloc
    void * (*cli_realloc)( void *, size_t );                    // realloc
    void   (*cli_free)( void * );                               // free
    void   (*cli_except)( dr_except );                          // fatal error handler
} WDRRoutines;

typedef bool    (*DRWLKBLK)( drmem_hdl, int index, void * );
typedef bool    (*DRPUBWLK)( void *, dr_pubname_data * );
typedef bool    (*DRWLKMODF)( drmem_hdl, void *, dr_search_context * );
typedef bool    (*DRARNGWLK)( void *, dr_arange_data * );
typedef bool    (*DRITERCUCB)( void *, drmem_hdl );
typedef bool    (*DRCUEWLK)( void *, dr_line_data * );
typedef bool    (*DRLFILEWLK)( void *, dr_line_file * );
typedef bool    (*DRLDIRWLK)( void *, dr_line_dir * );
typedef bool    (*DRCLSSRCH)( dr_sym_type, drmem_hdl, char *, drmem_hdl, void * );
typedef bool    (*DRFNAMECB)( char *, void * );
typedef bool    (*DRSYMREF)( drmem_hdl, dr_ref_info *, char *, void * );
typedef bool    (*DRENUMCB)( char *, unsigned_32, void * );
typedef bool    (*DRSYMSRCH)( dr_sym_context *, void * );
typedef void    (*DRDECORCB)( void *, char *, int, drmem_hdl, dr_sym_type );

/* drinit.c */
extern dr_dbg_handle    DRENTRY DRDbgInit( void *, unsigned long *, bool );
extern dr_dbg_handle    DRENTRY DRDbgInitNFT( void *, unsigned long *, bool ); /* no file table */
extern void             DRENTRY DRDbgFini( dr_dbg_handle );
extern dr_dbg_handle    DRENTRY DRSetDebug( dr_dbg_handle );
extern dr_dbg_handle    DRENTRY DRGetDebug( void );
extern bool             DRENTRY DRDbgClear( dr_dbg_handle dbg );
extern void             DRENTRY DRDbgDone( dr_dbg_handle dbg );
extern void             DRENTRY DRInit( void );
extern void             DRENTRY DRFini( void );
extern void             DRENTRY DRDbgWatProducerVer( dr_dbg_handle dbg, df_ver wat_producer_ver );

/* drtype.c */
extern bool             DRENTRY DRGetTypeInfo( drmem_hdl entry, dr_typeinfo *info );
extern drmem_hdl        DRENTRY DRGetTypeAT( drmem_hdl entry );
extern drmem_hdl        DRENTRY DRSkipTypeChain( drmem_hdl entry );
extern bool             DRENTRY DRWalkStruct( drmem_hdl mod, const DRWLKBLK *wlks, void *d );
extern bool             DRENTRY DRWalkArraySibs( drmem_hdl mod, const DRWLKBLK *wlks, void *d );
extern bool             DRENTRY DRWalkEnum( drmem_hdl mod,  DRWLKBLK wlk, void *d );
extern bool             DRENTRY DRConstValAT( drmem_hdl var, uint_32 *ret );
extern dr_array_stat    DRENTRY DRGetArrayInfo( drmem_hdl array, dr_array_info *info );
extern void             DRENTRY DRGetSubrangeInfo( drmem_hdl sub, dr_subinfo *info );
extern int              DRENTRY DRGetBitFieldInfo( drmem_hdl mem, dr_bitfield *info );
extern dr_ptr           DRENTRY DRGetAddrClass( drmem_hdl entry );

/* drpubnam.c */
extern void             DRENTRY DRWalkPubName( DRPUBWLK, void * );

/* drinfo.c */
extern dr_language      DRENTRY DRGetLanguageAT( drmem_hdl );
extern char             * DRENTRY DRGetProducer( drmem_hdl );
extern dr_model         DRENTRY DRGetMemModelAT( drmem_hdl );
extern dr_language      DRENTRY DRGetLanguage( void );
extern char             * DRENTRY DRGetName( drmem_hdl );
extern size_t           DRENTRY DRGetNameBuff( drmem_hdl entry, char *buff, size_t length );
extern size_t           DRENTRY DRGetScopedNameBuff( drmem_hdl entry, char *buff, size_t max );
extern size_t           DRENTRY DRGetCompDirBuff( drmem_hdl entry, char *buff, size_t length );
extern long             DRENTRY DRGetColumn( drmem_hdl );
extern long             DRENTRY DRGetLine( drmem_hdl );
extern char             * DRENTRY DRGetFileName( drmem_hdl );
extern void             DRENTRY DRGetFileNameList( DRFNAMECB, void * );
extern char             * DRENTRY DRIndexFileName( drmem_hdl mod, dr_fileidx fileidx );
extern dr_access        DRENTRY DRGetAccess( drmem_hdl );
extern bool             DRENTRY DRIsMemberStatic( drmem_hdl );
extern bool             DRENTRY DRIsFunc( drmem_hdl );
extern bool             DRENTRY DRIsParm( drmem_hdl entry );
extern bool             DRENTRY DRIsStatic( drmem_hdl );
extern bool             DRENTRY DRIsArtificial( drmem_hdl );
extern bool             DRENTRY DRIsSymDefined( drmem_hdl );
extern dr_virtuality    DRENTRY DRGetVirtuality( drmem_hdl );
extern unsigned         DRENTRY DRGetByteSize( drmem_hdl );
extern bool             DRENTRY DRGetLowPc( drmem_hdl, uint_32 * );
extern bool             DRENTRY DRGetHighPc( drmem_hdl, uint_32 * );
extern drmem_hdl        DRENTRY DRGetContaining( drmem_hdl );
extern drmem_hdl        DRENTRY DRDebugPCHDef( drmem_hdl );
extern bool             DRENTRY DRWalkModFunc( drmem_hdl, bool, DRWLKMODF, void * );
extern bool             DRENTRY DRWalkBlock( drmem_hdl, dr_srch, DRWLKBLK, void * );
extern drmem_hdl        DRENTRY DRWalkParent( dr_search_context * );
extern bool             DRENTRY DRStartScopeAT( drmem_hdl, uint_32 * );
extern unsigned         DRENTRY DRGetAddrSize( drmem_hdl );
extern bool             DRENTRY DRWalkScope( drmem_hdl, DRWLKBLK, void * );
extern dr_tag_type      DRENTRY DRGetTagType( drmem_hdl );
extern bool             DRENTRY DRWalkModTypes( drmem_hdl, DRWLKMODF, void * );

/* drarange.c */
extern void             DRENTRY DRWalkARange( DRARNGWLK, void * );

/* drutils.c */
extern drmem_hdl        DRENTRY DRGetCompileUnitTag( drmem_hdl comp_unit );
extern void             DRENTRY DRIterateCompileUnits( void *, DRITERCUCB );

/* drline.c */
extern drmem_hdl        DRENTRY DRGetStmtList( drmem_hdl ccu );
extern bool             DRENTRY DRWalkLines( drmem_hdl, uint_16, DRCUEWLK, void * );
extern bool             DRENTRY DRWalkLFiles( drmem_hdl, DRLFILEWLK, void *, DRLDIRWLK, void * );

/* drloc.c */
extern drmem_hdl        DRENTRY DRStringLengthAT( drmem_hdl str );
extern bool             DRENTRY DRLocationAT( drmem_hdl var, dr_loc_callbck *callbck, void *d );
extern bool             DRENTRY DRParmEntryAT( drmem_hdl var, dr_loc_callbck *callbck, void *d );
extern bool             DRENTRY DRLocBasedAT( drmem_hdl var, dr_loc_callbck *callbck, void *d );
extern bool             DRENTRY DRRetAddrLocation( drmem_hdl var, dr_loc_callbck *callbck, void *d );
extern bool             DRENTRY DRSegLocation( drmem_hdl var, dr_loc_callbck *callbck, void *d );

/* drdecnam.c */
extern char             * DRENTRY DRDecoratedName( drmem_hdl entry, drmem_hdl parent );

/* drfdkids.c */
extern void             DRENTRY DRBaseSearch( drmem_hdl, void *, DRCLSSRCH );
extern void             DRENTRY DRDerivedSearch( drmem_hdl, void *, DRCLSSRCH );
extern void             DRENTRY DRKidsSearch( drmem_hdl, dr_search, void *, DRCLSSRCH );
extern void             DRENTRY DRFriendsSearch( drmem_hdl, void *, DRCLSSRCH );

/* drenum.c */
extern void             DRENTRY DRLoadEnum( drmem_hdl, void *, DRENUMCB );

/* drgetref.c */
extern void             DRENTRY DRRefersTo( drmem_hdl, void *, DRSYMREF callback );
extern void             DRENTRY DRReferencedSymbols( dr_sym_type, void *, DRSYMREF callback );
extern void             DRENTRY DRReferredToBy( drmem_hdl, void *, DRSYMREF callback );

/* drsearch.c */
extern bool             DRENTRY DRResumeSymSearch( dr_search_context *, dr_search, dr_depth, void *, void *, DRSYMSRCH );
extern bool             DRENTRY DRSymSearch( dr_search, dr_depth, void *, void *, DRSYMSRCH );

/* drdecnam.c */
extern void             DRENTRY DRDecoratedNameList( void *obj, drmem_hdl die, drmem_hdl parent, DRDECORCB );

/* virtmem.c, virtstub.c */
extern bool             DRENTRY DRSwap( void );

extern dr_sym_type      DRENTRY DRGetSymType( drmem_hdl );

#ifdef __cplusplus
};
#endif

#endif

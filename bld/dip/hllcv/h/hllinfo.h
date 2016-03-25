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
* Description:  Internal interfaces for HLL/CV DIP.
*
****************************************************************************/


#ifndef DIP_HLL
#define DIP_HLL

#include <stdlib.h>
#include <string.h>
#include "dip.h"
#include "dipimp.h"
#include "bool.h"
#include "hll.h"

typedef unsigned long   virt_mem;

#define BLOCK_FACTOR( i, n )    (((i)+((n)-1))/(n))

typedef enum {
    HLL_SYM_TYPE_HLL_SSR,               /* HLL Symbol scope record. */
    HLL_SYM_TYPE_CV3_SSR,               /* CV3 Symbol scope record. */
    HLL_SYM_TYPE_PUB                    /* Public symbol record. */
} hll_sym_type;

struct imp_sym_handle {
    hll_sym_type        type;           /* The symbol type. */
    virt_mem            handle;         /* The record position. */
    unsigned_16         len;            /* The record length. */
    unsigned_16         segment;        /* The current segment. */
    imp_mod_handle      im;             /* The module handle. */
    unsigned short      mfunc_idx;
    virt_mem            containing_type;
    virt_mem            adjustor_type;
    unsigned long       adjustor_offset;

};

struct imp_type_handle {
    virt_mem            handle;
    unsigned short      idx;
    unsigned short      array_dim;
};

typedef enum {
    HLL_LINE_STYLE_SRC_LINES,
    HLL_LINE_STYLE_SRC_LINES_SEG_16,
    HLL_LINE_STYLE_SRC_LINES_SEG_32,
    HLL_LINE_STYLE_HLL_01,
    HLL_LINE_STYLE_HLL_02,
    HLL_LINE_STYLE_HLL_03,
    HLL_LINE_STYLE_HLL_04
} hll_line_style;

struct imp_cue_handle {

    virt_mem            file;           /* The current file header. */
    virt_mem            lines;          /* The position of the line number table. */
    unsigned_16         num_lines;      /* The number of lines. */
    unsigned_16         cur_line;       /* The current table index. */
    unsigned_16         segment;        /* The current segment. */
    imp_mod_handle      im;             /* The module handle. */
    hll_line_style      style;          /* The line number style. */
    union {
        struct {
            unsigned_16 first;          /* The first index for this file. */
            unsigned_16 last;           /* The last index for this file. */
            unsigned_32 base_offset;    /* The base segment offset. */
        }               hll;
    }                   u;

};

/*
 * Segment mapping information.
 * (Segments are sometimes refered to as sections or objects.)
 *
 * This structure is used to re-map seg:offset address at the wish of
 * the DBI user. It also contains some segment attributes.
 * (This information is either derived from the executable image or by
 *  scanning the module table.)
 */
typedef struct {
    addr48_ptr          map;        /* The segment:offset remapping. */
    unsigned_32         address;    /* The segment load address (hl4_linnum_first_lines::base_offset). */
    unsigned_16         ovl;        /* The segment overlay. */
    unsigned_32         size;       /* The segment size. */
    unsigned    is_executable : 1;  /* Set if this is a code segment. */
    unsigned         is_16bit : 1;  /* Set if this is a 16-bit segment. (not 100%?) */
} hllinfo_seg;

/* The NB level. */
typedef enum {
    HLL_LVL_NB00 = 1,               /* 16-bit codeview. */
    HLL_LVL_NB00_32BIT,             /* 32-bit codeview (cl386). */
    HLL_LVL_NB02,                   /* 16-bit codeview. */
    HLL_LVL_NB04                    /* 32-bit HLL */
} hllinfo_level;

/* Checks if the format level is 32-bit or 16-bit. */
#define HLL_IS_LVL_32BIT( format_lvl )  (   (format_lvl) == HLL_LVL_NB04 \
                                         || (format_lvl) == HLL_LVL_NB00_32BIT )

#define HLLINFO_NUM_SORTED              4
#define DIRECTORY_BLOCK_ENTRIES         256

struct imp_image_handle {
    imp_image_handle   *next_image; /* Pointer to the next HLL/CV image. */
    unsigned long       bias;       /* The the offset of the NBxx signature. */
    unsigned long       size;       /* the size of the debug info. */
    dig_fhandle         sym_file;   /* The file handle. */
    struct virt_page ***virt;       /* ? */
    unsigned            vm_dir_num; /* ? */
    hll_dir_entry     **directory;  /* The subsection directory, 2 levels. */
    unsigned            dir_count;  /* Number of entries in the directory. */
    virt_mem            types_base; /* HLLPack types, NULL if per-module. */
    mad_handle          mad;        /* MAD_X86 */
    hllinfo_seg        *segments;   /* Segment mappings and attribs. */
    unsigned            seg_count;  /* Number of segments. */
    hllinfo_level       format_lvl; /* The format level. */
    unsigned            is_32bit : 1; /* 32-bit (set) or 16-bit (clear) image. */
    void               *sorted_linnum_blocks[HLLINFO_NUM_SORTED];
                                    /* Linnum blocks which have been sorted. */
};

typedef struct {
    type_kind           k;
    unsigned            size;
    const void          *valp;
    unsigned long       int_val; /* only if k == TK_INTEGER */
} numeric_leaf;

typedef enum {
    HLL_STYLE_CV00 = 1,
    HLL_STYLE_CV02,
    HLL_STYLE_HL01,
    HLL_STYLE_HL02,
    HLL_STYLE_HL03,
    HLL_STYLE_HL04,
    HLL_STYLE_HL05,
    HLL_STYLE_HL06,
} hll_style;


typedef walk_result (DIR_WALKER)( imp_image_handle *, hll_dir_entry *, void * );

extern address  NilAddr;

extern imp_image_handle     *ImageList;

extern dip_status           VMInit( imp_image_handle *, unsigned long );
extern void                 VMFini( imp_image_handle * );
extern dip_status           VMAddDtor( imp_image_handle *ii, virt_mem start,
                                       void ( *dtor )( imp_image_handle *ii, void *user ),
                                       void *user );
extern void                 *VMBlock( imp_image_handle *, virt_mem, unsigned );
extern bool                 VMGetU8( imp_image_handle *ii, virt_mem start, unsigned_8 *valp );
extern bool                 VMGetU16( imp_image_handle *ii, virt_mem start, unsigned_16 *valp );
extern bool                 VMGetU32( imp_image_handle *ii, virt_mem start, unsigned_32 *valp );
extern void                 *VMRecord( imp_image_handle *, virt_mem, virt_mem *, unsigned_16 * );

extern void                 *VMSsBlock( imp_image_handle *, hll_dir_entry *, unsigned_32, unsigned );
extern bool                 VMSsGetU8( imp_image_handle *, hll_dir_entry *, unsigned_32, unsigned_8 * );
extern bool                 VMSsGetU16( imp_image_handle *, hll_dir_entry *, virt_mem, unsigned_16 * );
extern bool                 VMSsGetU32( imp_image_handle *, hll_dir_entry *, virt_mem, unsigned_32 * );
extern void                *VMSsRecord( imp_image_handle *ii, hll_dir_entry *hde, unsigned_32 rec_off,
                                         unsigned_32 *next_rec, unsigned_16 *sizep );
extern unsigned             VMShrink(void);

extern walk_result          hllWalkDirList( imp_image_handle *, hll_sst, DIR_WALKER *, void * );
extern hll_dir_entry       *hllFindDirEntry( imp_image_handle *, imp_mod_handle, hll_sst );

extern void                 hllLocationCreate( location_list *, location_type, void * );
extern void                 hllLocationAdd( location_list *, long );
extern void                 hllLocationTrunc( location_list *, unsigned );
extern dip_status           hllLocationOneReg( imp_image_handle *, unsigned, location_context *, location_list * );
extern dip_status           hllLocationManyReg( imp_image_handle *, unsigned, const unsigned_8 *, location_context *, location_list * );

extern size_t               hllNameCopy( char *, const char *, size_t, size_t );
extern void                 hllMapLogical( imp_image_handle *, address * );
extern void *               hllGetNumLeaf( void *, numeric_leaf * );
extern bool                 hllIsSegExecutable( imp_image_handle *, unsigned );

extern search_result        hllAddrMod( imp_image_handle *, address, imp_mod_handle * );

extern dip_status           hllTypeSymGetName( imp_image_handle *, imp_sym_handle *, const char **, unsigned * );
extern dip_status           hllTypeSymGetType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
extern dip_status           hllTypeSymGetAddr( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
extern dip_status           hllTypeSymGetValue( imp_image_handle *, imp_sym_handle *, location_context *, void * );
extern dip_status           hllTypeSymGetInfo( imp_image_handle *, imp_sym_handle *, location_context *, sym_info * );
#ifdef _CVREG  /* FIXME */
extern dip_status           hllTypeCallInfo( imp_image_handle *, unsigned, cv_calls *, unsigned * );
#endif
extern walk_result          hllTypeSymWalkList( imp_image_handle *ii, imp_type_handle *it, IMP_SYM_WKR* wk, imp_sym_handle *is, void *d );
extern dip_status           hllTypeIndexFillIn( imp_image_handle *, unsigned, imp_type_handle * );
extern search_result        hllTypeSearchTagName( imp_image_handle *, lookup_item *, void * );
extern search_result        hllTypeSearchNestedSym( imp_image_handle *, imp_type_handle *, lookup_item *, void * );
extern dip_status           hllTypeInfo( imp_image_handle *, imp_type_handle *, location_context *, dip_type_info * );
extern dip_status           hllTypeBase( imp_image_handle *, imp_type_handle *, imp_type_handle * );
extern dip_status           hllTypeMemberFuncInfo( imp_image_handle *, imp_type_handle *, imp_type_handle *, imp_type_handle *, unsigned long * );

extern dip_status           hllSymFillIn( imp_image_handle *, imp_sym_handle *, unsigned_16 seg, virt_mem, unsigned len );
extern dip_status           hllSymFindMatchingSym( imp_image_handle *, const char *, unsigned, unsigned, imp_sym_handle * );
extern dip_status           hllSymLocation( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
extern dip_status           hllSymValue( imp_image_handle *, imp_sym_handle *, location_context *, void * );
extern dip_status           hllSymType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );

extern void                 hllConfused(void);
extern dip_status           hllDoIndirection( imp_image_handle *, dip_type_info *, location_context *, location_list * );
extern hll_ssr_cuinfo      *hllGetCompInfo( imp_image_handle *, imp_mod_handle );
extern hll_style            hllGetModStyle( imp_image_handle *, imp_mod_handle );

#define T_CODE_LBL16    0x00f01
#define T_CODE_LBL32    0x00f02
#define T_DATA_LBL16    0x00f09
#define T_DATA_LBL32    0x00f0a

#define SCOPE_TOKEN "::"
#define SCOPE_TOKEN_LEN (sizeof( SCOPE_TOKEN ) - 1)

#if 0
# define HLL_LOG_ENABLED
extern void hllLog(const char *fmt, ...);
# define HLL_LOG(a) hllLog a
#else
# define HLL_LOG(a) do { } while (0)
#endif

#endif


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
* Description:  DIP (Debugging Information Processor) type definitions.
*
****************************************************************************/


#ifndef DIPTYPES_H_INCLUDED
#define DIPTYPES_H_INCLUDED

#include "digtypes.h"


#define NO_MOD              ((mod_handle)0)
#define ILL_MOD             ((mod_handle)1)

#define DIP_PRIOR_MIN       0
#define DIP_PRIOR_HIGHEST   (DIP_PRIOR_DEFAULT-1)
#define DIP_PRIOR_DEFAULT   (DIP_PRIOR_NORMAL-1)
#define DIP_PRIOR_NORMAL    50
#define DIP_PRIOR_EXPORTS   75
#define DIP_PRIOR_MAX       100

#include "digpck.h"

struct image_handle;
struct type_handle;
struct cue_handle;
struct sym_handle;

typedef struct image_handle     image_handle;
typedef struct type_handle      type_handle;
typedef struct cue_handle       cue_handle;
typedef struct sym_handle       sym_handle;

struct process_info;
struct dip_imp_routines;
struct location_context;

typedef struct process_info     process_info;
typedef struct dip_imp_routines dip_imp_routines;
typedef struct location_context location_context;

typedef unsigned_32             mod_handle;
typedef unsigned_32             cue_fileid;

typedef enum {
    #define pick(e,h,ih,wih)    e,
    #include "diphndls.h"
    #undef pick
    MAX_HK
} handle_kind;

typedef unsigned_8 symbol_source; enum {
    SS_MODULE,
    SS_SCOPED,
    SS_TYPE,
    SS_BLOCK,
    SS_SCOPESYM,
    SS_LAST
};

typedef unsigned_8 sym_walk_info; enum {
    SWI_SYMBOL,
    SWI_INHERIT_START,
    SWI_INHERIT_END,
    SWI_LAST
};

typedef unsigned_8 symbol_name; enum {
    SN_EXPRESSION,
    SN_SOURCE,
    SN_OBJECT,
    SN_DEMANGLED,
    SN_SCOPED,
    SN_LAST
};

typedef enum {
    DK_INT,
    DK_DATA_PTR,
    DK_CODE_PTR,
    DK_LAST
} default_kind;

typedef unsigned_16 dip_status; enum {
    DS_OK,
    DS_FAIL,
    DS_TOO_MANY_DIPS,
    DS_INVALID_DIP_VERSION,
    DS_NO_MEM,
    DS_NO_PROCESS,
    DS_TOO_MANY_IMAGES,
    DS_TOO_MANY_POINTERS,
    DS_IMPROPER_TYPE,
    DS_FOPEN_FAILED,
    DS_FREAD_FAILED,
    DS_FWRITE_FAILED,
    DS_FSEEK_FAILED,
    DS_INVALID_DIP,
    DS_BAD_PARM,
    DS_WRAPPED,
    DS_INFO_INVALID,
    DS_INFO_BAD_VERSION,
    DS_NO_PARM,
    DS_BAD_LOCATION,
    DS_NO_CONTEXT,
    DS_CONTEXT_ITEM_INVALID,
    DS_NO_READ_MEM,
    DS_NO_WRITE_MEM,
    DS_INVALID_OPERATOR,
    DS_LAST,
    DS_ERR=0x4000
};


typedef struct {
    address             start;
    addr_off            len;
    unsigned long       unique;
} scope_block;

typedef struct {
    long                low_bound;
    unsigned long       num_elts;
    unsigned long       stride;
    unsigned            num_dims;
    unsigned            column_major    : 1;
} array_info;

typedef unsigned_8 sym_kind; enum {
    SK_NONE,
    SK_CODE,
    SK_DATA,
    SK_CONST,
    SK_TYPE,
    SK_PROCEDURE,
    SK_NAMESPACE,
    SK_LAST
};

typedef struct {
    sym_kind            kind;
    unsigned            is_global               : 1;
    unsigned            is_member               : 1;
    unsigned            compiler                : 1;
    unsigned            is_private              : 1;
    unsigned            is_protected            : 1;
    unsigned            is_public               : 1;
    /* only valid for SK_PROCEDURE */
    unsigned            rtn_far                 : 1;
    unsigned            rtn_calloc              : 1;
    unsigned            ret_modifier            : 3;
    unsigned            ret_size                : 4;
    /* is_static is valid for all symbols */
    unsigned            is_static               : 1;
    unsigned short      num_parms;
    addr_off            ret_addr_offset;
    addr_off            prolog_size;
    addr_off            epilog_size;
    addr_off            rtn_size;
} sym_info;

typedef unsigned_8 location_type; enum { LT_ADDR, LT_INTERNAL };

typedef struct {
    unsigned    type            : 1;
    unsigned    bit_start       : 15;
    unsigned    bit_length      : 16;
    union {
        address                 addr;
        void                    *p;
    }           u;
} location_entry;

#define MAX_LOC_ENTRIES 16

typedef struct {
    unsigned_8          num;
    unsigned_8          flags;                  /* user defined flags */
    location_entry      e[MAX_LOC_ENTRIES];
} location_list;

typedef struct {
    const char  *start;
    size_t      len;
} lookup_token;

typedef unsigned_8 symbol_type; enum {
    ST_NONE,
    ST_OPERATOR,
    ST_DESTRUCTOR,
    ST_TYPE,
    ST_STRUCT_TAG,
    ST_CLASS_TAG,
    ST_UNION_TAG,
    ST_ENUM_TAG,
    ST_NAMESPACE,
    ST_LAST
};

typedef struct {
    mod_handle          mod;
    lookup_token        name;
    lookup_token        scope;
    lookup_token        source;
    unsigned            file_scope      : 1;
    unsigned            case_sensitive  : 1;
    unsigned            type            : 4; /* symbol_type values */
} lookup_item;

#include "digunpck.h"

#endif

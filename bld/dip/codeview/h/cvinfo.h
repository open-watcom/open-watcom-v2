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


#ifndef DIP_CV
#define DIP_CV

#include <string.h>
#include <stdlib.h>
#include "bool.h"
#include "dip.h"
#include "dipimp.h"
#include "cv4w.h"

enum {
#define _CVREG( name, num )     CV_X86_##name = num,
#include "cv4intl.h"
#undef _CVREG
#define _CVREG( name, num )     CV_AXP_##name = num,
#include "cv4axp.h"
CV_LAST_REG
};

#define IMH_FORTRAN         IMH_BASE

typedef unsigned long   virt_mem;

#define BLOCK_FACTOR( i, n )    (((i)+((n)-1))/(n))

struct imp_sym_handle {
    virt_mem            handle;
    unsigned short      len;
    imp_mod_handle      im;
    virt_mem            containing_type;
    virt_mem            adjustor_type;
    unsigned long       adjustor_offset;
    unsigned short      mfunc_idx;
};

struct imp_type_handle {
    virt_mem            handle;
    unsigned short      idx;
    unsigned short      array_dim;
};

struct imp_cue_handle {
    virt_mem            file;
    virt_mem            line;
    unsigned_16         pair;
    imp_mod_handle      im;
};

#define DIRECTORY_BLOCK_ENTRIES         256

struct imp_image_handle {
    imp_image_handle            *next_image;
    struct virt_page            ***virt;
    unsigned                    vm_dir_num;
    dig_fhandle                 sym_file;
    unsigned long               bias;
    cv_directory_entry          **directory;
    unsigned                    dir_count;
    unsigned                    map_count;
    seg_desc                    *mapping;
    virt_mem                    types_base;
    dig_mad                     mad;
};

typedef struct {
    type_kind           k;
    unsigned            size;
    const void          *valp;
    unsigned long       int_val; /* only if k == TK_INTEGER */
} numeric_leaf;

extern address  NilAddr;

extern imp_image_handle         *ImageList;

extern dip_status               VMInit( imp_image_handle *, unsigned long );
extern void                     VMFini( imp_image_handle * );
extern void                     *VMBlock( imp_image_handle *, virt_mem, size_t );
extern void                     *VMRecord( imp_image_handle *, virt_mem );
extern unsigned                 VMShrink(void);

typedef                         walk_result (DIP_DIR_WALKER)( imp_image_handle *, cv_directory_entry *, void * );
extern walk_result              WalkDirList( imp_image_handle *, DIP_DIR_WALKER *, void * );
extern cv_directory_entry       *FindDirEntry( imp_image_handle *, imp_mod_handle, unsigned );

extern void                     LocationCreate( location_list *, location_type, void * );
extern void                     LocationAdd( location_list *, long );
extern void                     LocationTrunc( location_list *, unsigned );
extern dip_status               LocationOneReg( imp_image_handle *, unsigned, location_context *, location_list * );
extern dip_status               LocationManyReg( imp_image_handle *, unsigned, const unsigned_8 *, location_context *, location_list * );

extern size_t                   NameCopy( char *, const char *, size_t, size_t );
extern void                     MapLogical( imp_image_handle *, address * );
extern const void               *GetNumLeaf( const void *, numeric_leaf * );
extern dip_status               SegIsExecutable( imp_image_handle *, unsigned );

extern search_result            ImpAddrMod( imp_image_handle *, address, imp_mod_handle * );

extern dip_status               TypeSymGetName( imp_image_handle *, imp_sym_handle *, const char **, size_t * );
extern dip_status               TypeSymGetType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
extern dip_status               TypeSymGetAddr( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
extern dip_status               TypeSymGetValue( imp_image_handle *, imp_sym_handle *, location_context *, void * );
extern dip_status               TypeSymGetInfo( imp_image_handle *, imp_sym_handle *, location_context *, sym_info * );
extern dip_status               TypeCallInfo( imp_image_handle *, unsigned, cv_calls *, unsigned * );
extern walk_result              TypeSymWalkList( imp_image_handle *ii, imp_type_handle *it, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *is, void *d );
extern dip_status               TypeIndexFillIn( imp_image_handle *, unsigned, imp_type_handle * );
extern search_result            TypeSearchTagName( imp_image_handle *, lookup_item *, void * );
extern search_result            TypeSearchNestedSym( imp_image_handle *, imp_type_handle *, lookup_item *, void * );
extern dip_status               ImpTypeInfo( imp_image_handle *, imp_type_handle *, location_context *, dip_type_info * );
extern dip_status               ImpTypeBase( imp_image_handle *, imp_type_handle *, imp_type_handle * );
extern dip_status               TypeMemberFuncInfo( imp_image_handle *, imp_type_handle *, imp_type_handle *, imp_type_handle *, unsigned long * );

extern dip_status               SymFillIn( imp_image_handle *, imp_sym_handle *, virt_mem );
extern dip_status               SymFindMatchingSym( imp_image_handle *, const char *, size_t, unsigned, imp_sym_handle * );
extern dip_status               ImpSymLocation( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
extern dip_status               ImpSymValue( imp_image_handle *, imp_sym_handle *, location_context *, void * );
extern dip_status               ImpSymType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );

extern void                     Confused(void);
extern dip_status               DoIndirection( imp_image_handle *, dip_type_info *, location_context *, location_list * );
extern cs_compile               *GetCompInfo( imp_image_handle *, imp_mod_handle );

#define T_CODE_LBL16    0x00f01
#define T_CODE_LBL32    0x00f02
#define T_DATA_LBL16    0x00f09
#define T_DATA_LBL32    0x00f0a

#define SCOPE_TOKEN "::"
#define SCOPE_TOKEN_LEN (sizeof(SCOPE_TOKEN)-1)

#endif

//temp
extern void NYI(void);

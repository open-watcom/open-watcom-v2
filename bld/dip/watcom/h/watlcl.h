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


#include "watloc.h"

typedef struct {
    unsigned char       class;
    unsigned char       namelen;
    unsigned            type_index;
    const char          *name;
    /* stuff that we don't have to parse until we know we want the lcl */
    const char          *unparsed;
} lcl_info;

/* variable, class = 1x */

typedef struct {
    lcl_info            i;
} lcl_variable;

/* code,     class = 2x */

typedef struct {
    lcl_info            i;
    unsigned int        parent_block;
    unsigned long       size;
    address             start;
} lcl_block;

typedef struct {
    lcl_block           b;
    unsigned char       pro_size;
    unsigned char       epi_size;
    unsigned long       ret_addr_offset;
} lcl_routine;

typedef struct {
    lcl_info    i;
    void        *obj_type;
    void        *obj_member;
} lcl_member;

typedef union {
    lcl_info            i;
    lcl_variable        v;
    lcl_block           b;
    lcl_routine         r;
    lcl_member          m;
} lcl_defn;


#define CLASS_MASK      0xf0
#define SUBCLASS_MASK   0x0f


/* variables */

#define VAR_SYMBOL      0x10
#define VAR_MODULE      0x00
#define VAR_LOCAL       0x01
#define VAR_MODULE386   0x02
#define VAR_MODULE_LOC  0x03

#define CODE_SYMBOL         0x20
#define CODE_BLOCK          0x00
#define CODE_NEAR_ROUT      0x01
#define CODE_FAR_ROUT       0x02
#define CODE_BLOCK386       0x03
#define CODE_NEAR_ROUT386   0x04
#define CODE_FAR_ROUT386    0x05
#define CODE_MEMBER_SCOPE   0x06

#define NEW_BASE        0x30
#define ADD_PREV_SEG    0x00
#define SET_BASE        0x01
#define SET_BASE386     0x02


extern unsigned         InfoSize( imp_image_handle *ii, imp_mod_handle im,
                                demand_kind dk, unsigned entry );
extern void             *InfoLoad( imp_image_handle *ii, imp_mod_handle im,
                                demand_kind dk, unsigned entry,
                                void (*clear)(void *, void *) );
extern void             InfoClear( imp_image_handle *ii );
extern void             InfoUnlock( void );
extern dip_status       InfoRelease( void );
extern void             InfoSpecUnlock(const char *);
extern void             InfoSpecLock(const char *);
extern dip_status       InitDemand( imp_image_handle *ii );
extern void             FiniDemand( void );
extern const char       *GetIndex( const char *ptr, unsigned *value );
extern const char       *GetAddress( imp_image_handle *ii, const char *ptr,
                                address *addr, int is32 );
extern void             KillLclLoadStack( void );
extern search_result    SearchLclMod( imp_image_handle *ii, imp_mod_handle im,
                                lookup_item *li, void *d );
extern search_result    SearchLclScope( imp_image_handle *ii, imp_mod_handle im,
                                address *addr, lookup_item *li, void *d );
extern search_result    LookupLclAddr( imp_image_handle *ii, address addr,
                                imp_sym_handle *is );
extern unsigned         SymHdl2LclName( imp_image_handle *ii, imp_sym_handle *is,
                                char *buff, unsigned buff_size );
extern dip_status       SymHdl2LclLoc( imp_image_handle *ii, imp_sym_handle *is,
                                location_context *lc, location_list *ll );
extern dip_status       SymHdl2LclType( imp_image_handle *ii, imp_sym_handle *is,
                                imp_type_handle *it );
extern void             SetGblLink( imp_sym_handle *is, void *link );
extern dip_status       Lcl2GblHdl( imp_image_handle *ii, imp_sym_handle *lcl_is,
                                imp_sym_handle *gbl_is );
extern dip_status       SymHdl2LclInfo( imp_image_handle *ii, imp_sym_handle *is,
                                sym_info *si );
extern dip_status       SymHdl2LclParmLoc( imp_image_handle *ii, imp_sym_handle *is,
                                location_context *lc, location_list *ll, unsigned parm );
extern walk_result      WalkScopedSymList( imp_image_handle *ii, address *addr,
                                IMP_SYM_WKR *wk, imp_sym_handle *is, void *d );
extern walk_result      WalkBlockSymList( imp_image_handle *ii, scope_block *scope,
                                IMP_SYM_WKR *wk, imp_sym_handle *is, void *d );
extern dip_status       WalkLclModSymList( imp_image_handle *ii, imp_mod_handle im,
                                IMP_SYM_WKR *wk, imp_sym_handle *is, void *d,
                                walk_result *last );

extern dip_status DIGENTRY      DIPImpSymObjType( imp_image_handle *ii, imp_sym_handle *is,
                                        imp_type_handle *it, dip_type_info *ti );
extern dip_status DIGENTRY      DIPImpSymObjLocation( imp_image_handle *ii, imp_sym_handle *is,
                                        location_context *lc, location_list *ll );
extern search_result DIGENTRY   DIPImpAddrScope( imp_image_handle *ii, imp_mod_handle im,
                                        address addr, scope_block *scope );
extern search_result DIGENTRY   DIPImpScopeOuter( imp_image_handle *ii, imp_mod_handle im,
                                        scope_block *in, scope_block *out );

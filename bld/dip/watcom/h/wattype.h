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

/* classes */
#define CLASS_MASK      0xf0
#define SUBCLASS_MASK   0x0f

#define NO_TYPE         0x00    /* just used internally */

#define NAME_TYPE       0x10
#define TYPE_SCALAR     0x00
#define TYPE_SCOPE      0x01
#define TYPE_NAME       0x02
#define TYPE_CUE_TABLE  0x03
#define TYPE_EOF        0x04

#define ARRAY_TYPE       0x20
#define ARRAY_BYTE_INDEX 0x00
#define ARRAY_WORD_INDEX 0x01
#define ARRAY_LONG_INDEX 0x02
#define ARRAY_TYPE_INDEX 0x03
#define ARRAY_DESC_INDEX 0x04
#define ARRAY_DESC386_INDEX 0x05

#define SUBRANGE_TYPE   0x30
#define SUBRANGE_BYTE   0x00
#define SUBRANGE_WORD   0x01
#define SUBRANGE_LONG   0x02

#define POINTER_TYPE            0x40
#define POINTER_NEAR            0x00
#define POINTER_FAR             0x01
#define POINTER_HUGE            0x02
#define POINTER_NEAR_DEREF      0x03
#define POINTER_FAR_DEREF       0x04
#define POINTER_HUGE_DEREF      0x05
#define POINTER_NEAR386         0x06
#define POINTER_FAR386          0x07
#define POINTER_NEAR386_DEREF   0x08
#define POINTER_FAR386_DEREF    0x09

#define ENUM_TYPE       0x50
#define ENUM_LIST       0x00
#define ENUM_CONST_BYTE 0x01
#define ENUM_CONST_WORD 0x02
#define ENUM_CONST_LONG 0x03
#define ENUM_CONST_I64  0x04

#define STRUCT_TYPE     0x60
#define STRUCT_LIST     0x00
#define ST_FIELD_BYTE   0x01
#define ST_FIELD_WORD   0x02
#define ST_FIELD_LONG   0x03
#define ST_BIT_BYTE     0x04
#define ST_BIT_WORD     0x05
#define ST_BIT_LONG     0x06
#define ST_FIELD_LOC    0x07
#define ST_BIT_LOC      0x08
#define ST_INHERIT      0x09

#define ST_ATTR_INTERNAL        0x01
#define ST_ATTR_PUBLIC          0x02
#define ST_ATTR_PROTECTED       0x04
#define ST_ATTR_PRIVATE         0x08


#define PROC_TYPE       0x70
#define PROC_NEAR       0x00
#define PROC_FAR        0x01
#define PROC_NEAR386    0x02
#define PROC_FAR386     0x03
#define PROC_EXT_PARMS  0x04

#define CHAR_TYPE        0x80
#define CHAR_BYTE_LEN    0x00
#define CHAR_WORD_LEN    0x01
#define CHAR_LONG_LEN    0x02
#define CHAR_DESC_LEN    0x03
#define CHAR_DESC386_LEN 0x04
#define CHAR_DESC_LOC    0x05

/* definition for 'scalar_type_byte' */
#define SCLR_LEN_MASK   0x0f

#define SCLR_CLASS_MASK 0x70
#define SCLR_INTEGER    0x00
#define SCLR_UNSIGNED   0x10
#define SCLR_FLOAT      0x20
#define SCLR_VOID       0x30
#define SCLR_COMPLEX    0x40

extern void             KillTypeLoadStack( void );
extern void             ClearTypeCache( imp_image_handle *ii );
extern dip_status       FindTypeHandle( imp_image_handle *ii, imp_mod_handle im,
                                unsigned index, imp_type_handle *it );
extern search_result    LookupTypeName( imp_image_handle *ii, imp_mod_handle im,
                                lookup_item *li, imp_type_handle *it );
extern search_result    SearchEnumName( imp_image_handle *ii, imp_mod_handle im,
                                lookup_item *li, void *d );
extern search_result    SearchTypeName( imp_image_handle *ii, imp_mod_handle im,
                                lookup_item *li, void *d );
extern unsigned         SymHdl2CstName( imp_image_handle *ii, imp_sym_handle *is,
                                char *buff, unsigned buff_size );
extern unsigned         SymHdl2TypName( imp_image_handle *ii, imp_sym_handle *is,
                                char *buff, unsigned buff_size );
extern unsigned         SymHdl2MbrName( imp_image_handle *ii, imp_sym_handle *is,
                                char *buff, unsigned buff_size );
extern dip_status       SymHdl2CstValue( imp_image_handle *ii, imp_sym_handle *is,
                                void *d );
extern dip_status       SymHdl2CstType( imp_image_handle *ii, imp_sym_handle *is,
                                imp_type_handle *it );
extern dip_status       SymHdl2TypType( imp_image_handle *ii, imp_sym_handle *is,
                                imp_type_handle *it );
extern dip_status       SymHdl2MbrType( imp_image_handle *ii, imp_sym_handle *is,
                                imp_type_handle *it );
extern dip_status       SymHdl2MbrLoc( imp_image_handle *ii, imp_sym_handle *is,
                                location_context *lc, location_list *ll );
extern dip_status       SymHdl2MbrInfo( imp_image_handle *ii, imp_sym_handle *is,
                                sym_info *si, location_context *lc );
extern search_result    SearchMbr( imp_image_handle *ii, imp_type_handle *it,
                                lookup_item *li, void *d );
extern walk_result      WalkTypeSymList( imp_image_handle *ii, imp_type_handle *it,
                                IMP_SYM_WKR *wk, imp_sym_handle *is, void *d );
extern const char       *FindSpecCueTable( imp_image_handle *ii, imp_mod_handle im,
                                const char **base );


extern walk_result DIGENTRY     DIPImpWalkTypeList( imp_image_handle *ii,
                                    imp_mod_handle im, IMP_TYPE_WKR *wk,
                                    imp_type_handle *it, void *d );
extern dip_status DIGENTRY      DIPImpTypeInfo(imp_image_handle *ii,
                                    imp_type_handle *it, location_context *lc,
                                    dip_type_info *ti );
extern dip_status DIGENTRY      DIPImpTypeBase(imp_image_handle *ii,
                                    imp_type_handle *it, imp_type_handle *base,
                                    location_context *lc, location_list *ll );
extern dip_status DIGENTRY      DIPImpTypeArrayInfo(imp_image_handle *ii,
                                    imp_type_handle *it, location_context *lc,
                                    array_info *ai, imp_type_handle *index );
extern dip_status DIGENTRY      DIPImpTypeProcInfo(imp_image_handle *ii,
                                    imp_type_handle *it, imp_type_handle *parm,
                                    unsigned num );
extern dip_status DIGENTRY      DIPImpTypePtrAddrSpace( imp_image_handle *ii,
                                    imp_type_handle *it, location_context *lc,
                                    address *addr );
extern dip_status DIGENTRY      DIPImpTypeThunkAdjust( imp_image_handle *ii,
                                    imp_type_handle *oit, imp_type_handle *mit,
                                    location_context *lc, address *addr );
extern imp_mod_handle DIGENTRY  DIPImpTypeMod( imp_image_handle *ii,
                                    imp_type_handle *it );
extern int DIGENTRY             DIPImpTypeCmp( imp_image_handle *ii,
                                    imp_type_handle *it1, imp_type_handle *it2 );
extern size_t DIGENTRY          DIPImpTypeName( imp_image_handle *ii,
                                    imp_type_handle *it, unsigned num,
                                    symbol_type *tag, char *buff,
                                    size_t buff_size );
extern dip_status DIGENTRY      DIPImpTypeAddRef( imp_image_handle *ii,
                                    imp_type_handle *it );
extern dip_status DIGENTRY      DIPImpTypeRelease( imp_image_handle *ii,
                                    imp_type_handle *it );
extern dip_status DIGENTRY      DIPImpTypeFreeAll( imp_image_handle *ii );

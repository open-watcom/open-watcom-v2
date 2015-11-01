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


#define GETU8(x)        *(unsigned_8 *)(x)
#define GETU16(x)       *(unsigned_16 *)(x)
#define GETU32(x)       *(unsigned_32 *)(x)

#define GETS8(x)        *(signed_8 *)(x)
#define GETS16(x)       *(signed_16 *)(x)
#define GETS32(x)       *(signed_32 *)(x)

extern void             GblSymFini( section_info *inf );
extern void             AdjustSyms( section_info *inf );
extern dip_status       MakeGblLst( section_info *inf );
extern unsigned         GblSymSplit( info_block *gbl, section_info *inf );
extern dip_status       SymHdl2GblLoc( imp_image_handle *ii, imp_sym_handle *is,
                                location_list *ll );
extern dip_status       SymHdl2GblInfo( imp_image_handle *ii, imp_sym_handle *is,
                                sym_info *si );
extern unsigned         SymHdl2GblName( imp_image_handle *ii, imp_sym_handle *is,
                                char *buff, unsigned buff_size );
extern unsigned         SymHdl2ObjGblName( imp_image_handle *ii, imp_sym_handle *is,
                                char *buff, unsigned buff_size );
extern dip_status       SymHdl2GblType( imp_image_handle *ii, imp_sym_handle *is,
                                imp_type_handle *it );
extern type_kind        GblTypeClassify( unsigned kind );
extern walk_result      WalkGblModSymList( imp_image_handle *ii, imp_mod_handle im,
                                IMP_SYM_WKR *wk, imp_sym_handle *is, void *d );
extern dip_status       Link2GblHdl( imp_image_handle *ii, void *gbl,
                                imp_sym_handle *is );
extern search_result    SearchGbl( imp_image_handle *ii, imp_mod_handle cim,
                                imp_mod_handle im, lookup_item *li, void *d );
extern search_result    LookupGblAddr( imp_image_handle *, address,
                                imp_sym_handle * );

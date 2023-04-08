/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


extern void             GblSymFini( section_info *inf );
extern void             AdjustSyms( imp_image_handle *, unsigned sectno );
extern dip_status       MakeGblLst( imp_image_handle *, section_info *inf );
extern unsigned         GblSymSplit( imp_image_handle *, info_block *gbl, section_info *inf );
extern dip_status       SymHdl2GblLoc( imp_image_handle *, imp_sym_handle *, location_list *ll );
extern dip_status       SymHdl2GblInfo( imp_image_handle *, imp_sym_handle *, sym_info *si );
extern unsigned         SymHdl2GblName( imp_image_handle *, imp_sym_handle *, char *buff, unsigned buff_size );
extern unsigned         SymHdl2ObjGblName( imp_image_handle *, imp_sym_handle *, char *buff, unsigned buff_size );
extern dip_status       SymHdl2GblType( imp_image_handle *, imp_sym_handle *, imp_type_handle *ith );
extern type_kind        GblTypeClassify( unsigned kind );
extern walk_result      WalkGblModSymList( imp_image_handle *, imp_mod_handle, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *, void *d );
extern dip_status       Link2GblHdl( imp_image_handle *, gbl_info *gbl, imp_sym_handle * );
extern search_result    SearchGbl( imp_image_handle *, imp_mod_handle, imp_mod_handle, lookup_item *li, void *d );
extern search_result    LookupGblAddr( imp_image_handle *, address, imp_sym_handle * );

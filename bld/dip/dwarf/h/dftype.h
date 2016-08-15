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


extern walk_result WalkTypeSymList( imp_image_handle *ii, imp_type_handle *it,
                 DIP_IMP_SYM_WALKER *wk, imp_sym_handle *is, void *d );
extern search_result SearchMbr( imp_image_handle *ii, imp_type_handle *it,
                 lookup_item *li, void *d );
extern drmem_hdl GetParmN(  imp_image_handle *ii, drmem_hdl proc, int count );
extern int GetParmCount(  imp_image_handle *ii, drmem_hdl proc );
extern void MapImpTypeInfo( dr_typeinfo *typeinfo, dip_type_info *ti );
extern dip_status  DFBaseAdjust( imp_image_handle *ii,
                                drmem_hdl base, drmem_hdl derived,
                                location_context *lc, address *addr );

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


extern section_info     *FindInfo( imp_image_handle *ii, imp_mod_handle im );
extern mod_info         *ModPointer( imp_image_handle *ii, imp_mod_handle im );
extern dip_status       AdjustMods( section_info *inf, unsigned long adjust );
extern void             SetModBase( imp_image_handle *ii );
extern void             ModInfoFini( section_info *inf );
extern unsigned         ModInfoSplit( info_block *blk, section_info *inf );
extern unsigned         ModOff2Idx( section_info *inf, unsigned off );
extern walk_result      MyWalkModList( imp_image_handle *ii, INT_MOD_WKR *wk,
                                void *d );
extern size_t           PrimaryCueFile( imp_image_handle *ii, imp_cue_handle *ic,
                                char *buff, size_t buff_size );
extern walk_result DIGENTRY DIPImpWalkModList( imp_image_handle *ii, IMP_MOD_WKR *wk,
                                void *d );
extern char *DIGENTRY       DIPImpModSrcLang( imp_image_handle *ii, imp_mod_handle im );
extern dip_status DIGENTRY  DIPImpModInfo( imp_image_handle *ii, imp_mod_handle im,
                                handle_kind hk );
extern size_t DIGENTRY      DIPImpModName( imp_image_handle *ii, imp_mod_handle im,
                                char *buff, size_t buff_size );

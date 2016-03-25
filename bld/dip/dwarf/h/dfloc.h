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


extern void LocationCreate( location_list *ll, location_type lt, void *d );
extern void LocationTrunc( location_list *ll, unsigned bits );
extern void LocationAdd( location_list *ll, long sbits );
extern dip_status EvalLocation( imp_image_handle *ii,
                                location_context *lc,
                                dr_handle         sym,
                                word              seg,
                                location_list    *ll );
extern dip_status EvalParmLocation( imp_image_handle *ii,
                                    location_context *lc,
                                    dr_handle         sym,
                                    location_list    *ll );
extern dip_status EvalRetLocation( imp_image_handle *ii,
                                    location_context *lc,
                                    dr_handle         sym,
                                    location_list    *ll );
extern dip_status EvalLocAdj( imp_image_handle *ii,
                               location_context *lc,
                               dr_handle         sym,
                               address          *addr );
extern bool EvalOffset( imp_image_handle *ii, dr_handle sym, uint_32 *ret );
extern bool EvalSeg( imp_image_handle *ii, dr_handle sym, addr_seg *ret );
extern dip_status EvalBasedPtr( imp_image_handle *ii,
                                location_context *lc,
                                dr_handle         sym,
                                address          *addr );
extern bool EvalSymOffset( imp_image_handle *ii,
                           dr_handle         sym,
                           uint_32          *val );
dip_status SafeDCItemLocation( location_context *,
                               context_item ci,
                               location_list *ll );

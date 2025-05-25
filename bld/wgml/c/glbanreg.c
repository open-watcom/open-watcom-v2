/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description: WGML implement :BANREGION :eBANREGION  tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


static  region_lay_tag      wk;         // temp for input values
static  region_lay_tag  *   prev_reg;


/***************************************************************************/
/*   :BANREGION attributes                                                 */
/***************************************************************************/
const lay_att       banregion_att[] = {
    e_indent, e_hoffset, e_width, e_voffset, e_depth, e_font, e_refnum,
    e_region_position, e_pouring, e_script_format, e_contents
};

static bool         count[TABLE_SIZE( banregion_att )];
static int          sum_count;

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
* Description:  Source line cue (non-)support.
*
****************************************************************************/


#include "msym.h"

/* Note: MAPSYM .sym files do suppport rudimentary line number information.
 * However we do not support this for two reasons: One, most .sym files do not
 * include line number information, and two, the .sym format is so pathetic
 * that anyone who actually has source code will definitely want to use some
 * better debugging information. It is expected that users typically will
 * have .sym files for executables for which no source code is available.
 */

walk_result DIPIMPENTRY( WalkFileList )( imp_image_handle *ii,
                imp_mod_handle im, IMP_CUE_WKR *wk, imp_cue_handle *ic, void *d )
{
    ii = ii; im = im; wk = wk; ic = ic; d = d;
    return( WR_CONTINUE );
}

imp_mod_handle DIPIMPENTRY( CueMod )( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    ii = ii; ic = ic;
    return( IMH_NOMOD );
}

size_t DIPIMPENTRY( CueFile )( imp_image_handle *ii,
                        imp_cue_handle *ic, char *buff, size_t buff_size )
{
    ii = ii; ic = ic; buff = buff; buff_size = buff_size;
    return( 0 );
}

cue_fileid DIPIMPENTRY( CueFileId )( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    ii = ii; ic = ic;
    return( 0 );
}

dip_status DIPIMPENTRY( CueAdjust )( imp_image_handle *ii,
                imp_cue_handle *src, int adj, imp_cue_handle *dst )
{
    ii = ii; src = src; adj = adj; dst = dst;
    return( DS_ERR|DS_FAIL );
}

unsigned long DIPIMPENTRY( CueLine )( imp_image_handle *ii, imp_cue_handle *ic )
{
    ii = ii; ic = ic;
    return( 0 );
}

unsigned DIPIMPENTRY( CueColumn )( imp_image_handle *ii, imp_cue_handle *ic )
{
    ii = ii; ic = ic;
    return( 0 );
}

address DIPIMPENTRY( CueAddr )( imp_image_handle *ii, imp_cue_handle *ic )
{
    ii = ii; ic = ic;
    return( NilAddr );
}

search_result DIPIMPENTRY( LineCue )( imp_image_handle *ii,
                imp_mod_handle im, cue_fileid file, unsigned long line,
                unsigned column, imp_cue_handle *ic )
{
    ii = ii; im = im; file = file; line = line; column = column; ic = ic;
    return( SR_NONE );
}

search_result DIPIMPENTRY( AddrCue )( imp_image_handle *ii,
                imp_mod_handle im, address addr, imp_cue_handle *ic )
{
    ii = ii; im = im; addr = addr; ic = ic;
    return( SR_NONE );
}

int DIPIMPENTRY( CueCmp )( imp_image_handle *ii, imp_cue_handle *ic1,
                                imp_cue_handle *ic2 )
{
    ii = ii; ic1 = ic1; ic2 = ic2;
    return( 0 );
}

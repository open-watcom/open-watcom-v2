/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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

walk_result DIPIMPENTRY( WalkFileList )( imp_image_handle *iih,
                imp_mod_handle imh, DIP_IMP_CUE_WALKER *wk, imp_cue_handle *icueh, void *d )
{
    /* unused parameters */ (void)iih; (void)imh; (void)wk; (void)icueh; (void)d;

    return( WR_CONTINUE );
}

imp_mod_handle DIPIMPENTRY( CueMod )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    return( IMH_NOMOD );
}

size_t DIPIMPENTRY( CueFile )( imp_image_handle *iih,
                        imp_cue_handle *icueh, char *buff, size_t buff_size )
{
    /* unused parameters */ (void)iih; (void)icueh; (void)buff; (void)buff_size;

    return( 0 );
}

cue_fileid DIPIMPENTRY( CueFileId )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    return( 0 );
}

dip_status DIPIMPENTRY( CueAdjust )( imp_image_handle *iih, imp_cue_handle *src, int adj, imp_cue_handle *dst )
{
    /* unused parameters */ (void)iih; (void)src; (void)adj; (void)dst;

    return( DS_ERR | DS_FAIL );
}

unsigned long DIPIMPENTRY( CueLine )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    return( 0 );
}

unsigned DIPIMPENTRY( CueColumn )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    return( 0 );
}

address DIPIMPENTRY( CueAddr )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    return( NilAddr );
}

search_result DIPIMPENTRY( LineCue )( imp_image_handle *iih,
                imp_mod_handle imh, cue_fileid file, unsigned long line,
                unsigned column, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)imh; (void)file; (void)line; (void)column; (void)icueh;

    return( SR_NONE );
}

search_result DIPIMPENTRY( AddrCue )( imp_image_handle *iih,
                imp_mod_handle imh, address addr, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)imh; (void)addr; (void)icueh;

    return( SR_NONE );
}

int DIPIMPENTRY( CueCmp )( imp_image_handle *iih, imp_cue_handle *icueh1, imp_cue_handle *icueh2 )
{
    /* unused parameters */ (void)iih; (void)icueh1; (void)icueh2;

    return( 0 );
}

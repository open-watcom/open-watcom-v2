/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#include "skel.h"


/*
 * Stuff dealing with module handles
 */

walk_result DIPIMPENTRY( WalkModList )( imp_image_handle *iih, DIP_IMP_MOD_WALKER *wk, void *d )
/***********************************************************************************************
 * TODO:
 *
 * PSEUDO-CODE:
 *
 * for( each module in 'iih' ) {
 *     im = implementation module handle for module;
 *     wr = wk( iih, im, d );
 *     if( wr != WR_CONTINUE ) {
 *         return( wr );
 *     }
 * }
 */
{
   return( WR_CONTINUE );
}

size_t DIPIMPENTRY( ModName )( imp_image_handle *iih,
                        imp_mod_handle imh, char *buff, size_t buff_size )
/*************************************************************************
 * TODO:
 *
 * Given the imp_mod_handle, copy the name of the module into 'buff'.
 * Do not copy more than 'buff_size' - 1 characters into the buffer and
 * append a trailing '\0' character. Return the real length
 * of the module name (not including the trailing '\0' character) even
 * if you had to truncate it to fit it into the buffer. If something
 * went wrong and you can't get the module name, call DCStatus and
 * return zero.
 *
 * NOTE: the client might pass in zero for 'buff_size'. In that
 * case, just return the length of the module name and do not attempt
 * to put anything into the buffer.
 */
{
    return( 0 );
}

char *DIPIMPENTRY( ModSrcLang )( imp_image_handle *iih, imp_mod_handle imh )
/***************************************************************************
 * TODO:
 *
 * Return a pointer to a string naming the source language of the
 * module. Currently allowed names are "c", "cpp", and "fortran".
 */
{
    return( "c" );
}

dip_status DIPIMPENTRY( ModInfo )( imp_image_handle *iih,
                                imp_mod_handle imh, handle_kind hk )
/*******************************************************************
 * TODO:
 *
 * Return DS_OK if the module has the kind of information indicated
 * by 'hk', DS_FAIL if it does not.
 */
{
    return( DS_FAIL );
}

search_result DIPIMPENTRY( AddrMod )( imp_image_handle *iih, address a,
                imp_mod_handle *imh )
/**********************************************************************
 * TODO:
 *
 * Given an address, find the module which defines that address and
 * assign the implemenation module handle to '*imh'. Return SR_EXACT
 * or SR_CLOSEST. If no module for the image defines the address,
 * return SR_NONE.
 */
{
    return( SR_NONE );
}

address DIPIMPENTRY( ModAddr )( imp_image_handle *iih, imp_mod_handle imh )
/**************************************************************************
 * TODO:
 *
 * Given a module, return the first address off one of it's code segments.
 * Practically speaking, we're looking for the address of the first line
 * source cue in the file.
 */
{
    return( NilAddr );
}

dip_status DIPIMPENTRY( ModDefault )( imp_image_handle *iih,
                imp_mod_handle imh, default_kind dk, dig_type_info *ti )
/***********************************************************************
 * TODO:
 *
 * Return the default type information for indicated type. The
 * client uses this to figure out how big a default 'int', code pointer,
 * and data pointer should be. The information depends on whether
 * the 16 or 32-bit compiler was used, and what memory model the
 * source file was compiled with.
 */
{
     return( DS_FAIL );
}

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


#include "skel.h"


/*
        Stuff dealing with module handles
*/
walk_result     DIGENTRY DIPImpWalkModList( imp_image_handle *ii,
                        IMP_MOD_WKR *wk, void *d )
{
    //TODO:
    /* PSEUDO-CODE:

       for( each module in 'ii' ) {
           im = implementation module handle for module;
           wr = wk( ii, im, d );
           if( wr != WR_CONTINUE ) return( wr );
       }
   */
   return( WR_CONTINUE );
}

unsigned        DIGENTRY DIPImpModName( imp_image_handle *ii,
                        imp_mod_handle im, char *buff, unsigned max )
{
    //TODO:
    /*
        Given the imp_mod_handle, copy the name of the module into 'buff'.
        Do not copy more than 'max' - 1 characters into the buffer and
        append a trailing '\0' character. Return the real length
        of the module name (not including the trailing '\0' character) even
        if you had to truncate it to fit it into the buffer. If something
        went wrong and you can't get the module name, call DCStatus and
        return zero. NOTE: the client might pass in zero for 'max'. In that
        case, just return the length of the module name and do not attempt
        to put anything into the buffer.
    */
    return( 0 );
}

char            *DIGENTRY DIPImpModSrcLang( imp_image_handle *ii, imp_mod_handle im )
{
    //TODO:
    /*
        Return a pointer to a string naming the source language of the
        module. Currently allowed names are "c", "cpp", and "fortran".
    */
    return( "c" );
}

dip_status      DIGENTRY DIPImpModInfo( imp_image_handle *ii,
                                imp_mod_handle im, handle_kind hk )
{
    //TODO:
    /*
        Return DS_OK if the module has the kind of information indicated
        by 'hk', DS_FAIL if it does not.
    */
    return( DS_FAIL );
}

search_result   DIGENTRY DIPImpAddrMod( imp_image_handle *ii, address a,
                imp_mod_handle *im )
{
    //TODO:
    /*
        Given an address, find the module which defines that address and
        assign the implemenation module handle to '*im'. Return SR_EXACT
        or SR_CLOSEST. If no module for the image defines the address,
        return SR_NONE.
    */
    return( SR_NONE );
}

address         DIGENTRY DIPImpModAddr( imp_image_handle *ii,
                                imp_mod_handle im )
{
    //TODO:
    /*
        Given a module, return the first address off one of it's code segments.
        Practically speaking, we're looking for the address of the first line
        source cue in the file.
    */
    return( NilAddr );
}

dip_status      DIGENTRY DIPImpModDefault( imp_image_handle *ii,
                imp_mod_handle im, default_kind dk, dip_type_info *ti )
{
    //TODO:
    /*
        Return the default type information for indicated type. The
        client uses this to figure out how big a default 'int', code pointer,
        and data pointer should be. The information depends on whether
        the 16 or 32-bit compiler was used, and what memory model the
        source file was compiled with.
     */
     return( DS_FAIL );
}

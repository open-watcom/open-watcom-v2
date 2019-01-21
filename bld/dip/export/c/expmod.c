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


#include "exp.h"

/*
        Stuff dealing with module handles
*/
walk_result DIPIMPENTRY( WalkModList )( imp_image_handle *iih,
                        DIP_IMP_MOD_WALKER *wk, void *d )
{
    return( wk( iih, IMH_EXPORT, d ) );
}

size_t DIPIMPENTRY( ModName )( imp_image_handle *iih,
                        imp_mod_handle imh, char *buff, size_t buff_size )
{

    /* unused parameters */ (void)imh;

    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > iih->len )
            buff_size = iih->len;
        memcpy( buff, iih->name, buff_size );
        buff[buff_size] = '\0';
    }
    return( iih->len );
}

char *DIPIMPENTRY( ModSrcLang )( imp_image_handle *iih, imp_mod_handle imh )
{
    /* unused parameters */ (void)iih; (void)imh;

    return( "c" );
}

dip_status DIPIMPENTRY( ModInfo )( imp_image_handle *iih, imp_mod_handle imh, handle_kind hk )
{
    /* unused parameters */ (void)iih; (void)imh; (void)hk;

    return( DS_FAIL );
}

search_result DIPIMPENTRY( AddrMod )( imp_image_handle *iih, address a, imp_mod_handle *imh )
{
    /* unused parameters */ (void)iih; (void)a;

    if( FindAddrBlock( iih, a.mach ) != NULL ) {
        *imh = IMH_EXPORT;
        return( SR_CLOSEST );
    }
    return( SR_NONE );
}

address DIPIMPENTRY( ModAddr )( imp_image_handle *iih, imp_mod_handle imh )
{
    address     addr;
    exp_block   *exp;

    /* unused parameters */ (void)imh;

    for( exp = iih->addr; exp != NULL; exp = exp->next ) {
        if( exp->code ) {
            addr.mach = exp->start;
            DCAddrSection( &addr );
            return( addr );
        }
    }
    return( NilAddr );
}

dip_status DIPIMPENTRY( ModDefault )( imp_image_handle *iih,
                imp_mod_handle imh, default_kind dk, dig_type_info *ti )
{
    /* unused parameters */ (void)iih; (void)imh; (void)dk; (void)ti;

    return( DS_FAIL );
}

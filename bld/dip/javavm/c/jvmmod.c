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


#include "jvmdip.h"


/*
 * This guy needs to be *FAST*. He's called from all over.
 */

search_result FindMBIndex( imp_image_handle *iih, addr_off off, unsigned *idx )
{
    ji_ptr              code_start;
    unsigned            i;
    struct methodblock  *mb;

    if( iih->num_methods > 0 ) {
        i = iih->last_method;
        mb = &iih->methods[i];
        for( ;; ) {
            if( !(mb->fb.access & ACC_NATIVE) ) {
                code_start = (ji_ptr)mb->code;
                if( off == code_start ) {
                    *idx = i;
                    iih->last_method = i;
                    return( SR_EXACT );
                }
                if( off > code_start && off < (code_start + mb->code_length) ) {
                    *idx = i;
                    iih->last_method = i;
                    return( SR_CLOSEST );
                }
            }
            ++mb;
            ++i;
            if( i >= iih->num_methods ) {
                i = 0;
                mb = iih->methods;
            }
            if( i == iih->last_method ) {
                break;
            }
        }
    }
    *idx = (unsigned)-1;
    return( SR_NONE );
}

walk_result DIPIMPENTRY( WalkModList )( imp_image_handle *iih,
                        DIP_IMP_MOD_WALKER *wk, void *d )
{
    return( wk( iih, IMH_JAVA, d ) );
}

size_t DIPIMPENTRY( ModName )( imp_image_handle *iih,
                    imp_mod_handle imh, char *buff, size_t buff_size )
{
    ji_ptr      name;
    size_t      len;

    name = GetPointer( iih->cc + offsetof( ClassClass, name ) );
    len = GetString( name, NameBuff, sizeof( NameBuff ) );
    NormalizeClassName( NameBuff, len );
    return( NameCopy( buff, NameBuff, buff_size, len ) );
}

char *DIPIMPENTRY( ModSrcLang )( imp_image_handle *iih, imp_mod_handle imh )
{
    return( "java" );
}

dip_status DIPIMPENTRY( ModInfo )( imp_image_handle *iih,
                                imp_mod_handle imh, handle_kind hk )
{
    return( DS_OK );
}

search_result DIPIMPENTRY( AddrMod )( imp_image_handle *iih, address a,
                imp_mod_handle *imh )
{
    search_result       sr;
    unsigned            i;

    sr = FindMBIndex( iih, a.mach.offset, &i );
    switch( sr ) {
    case SR_EXACT:
    case SR_CLOSEST:
        *imh = IMH_JAVA;
        break;
    }
    return( sr );
}

address DIPIMPENTRY( ModAddr )( imp_image_handle *iih,  imp_mod_handle imh )
{
    address     a;
    unsigned    i;

    for( i = 0; i < iih->num_methods; ++i ) {
        if( !(iih->methods[i].fb.access & ACC_NATIVE) ) {
            a = DefCodeAddr;
            a.mach.offset = (ji_ptr)iih->methods[i].code;
            return( a );
        }
    }
    return( NilAddr );
}

dip_status DIPIMPENTRY( ModDefault )( imp_image_handle *iih,
                imp_mod_handle imh, default_kind dk, dig_type_info *ti )
{
     return( DS_FAIL );
}

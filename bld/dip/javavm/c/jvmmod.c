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


#include "jvmdip.h"


/*
        This guy needs to be *FAST*. He's called from all over.
*/
search_result FindMBIndex( imp_image_handle *ii, addr_off off, unsigned *idx )
{
    ji_ptr              code_start;
    unsigned            i;
    struct methodblock  *mb;

    if( ii->num_methods > 0 ) {
        i = ii->last_method;
        mb = &ii->methods[i];
        for( ;; ) {
            if( !(mb->fb.access & ACC_NATIVE) ) {
                code_start = (ji_ptr)mb->code;
                if( off == code_start ) {
                    *idx = i;
                    ii->last_method = i;
                    return( SR_EXACT );
                }
                if( off > code_start && off < (code_start + mb->code_length) ) {
                    *idx = i;
                    ii->last_method = i;
                    return( SR_CLOSEST );
                }
            }
            ++mb;
            ++i;
            if( i >= ii->num_methods ) {
                i = 0;
                mb = ii->methods;
            }
            if( i == ii->last_method ) break;
        }
    }
    *idx = (unsigned)-1;
    return( SR_NONE );
}

walk_result     DIGENTRY DIPImpWalkModList( imp_image_handle *ii,
                        IMP_MOD_WKR *wk, void *d )
{
    return( wk( ii, IMH_JAVA, d ) );
}

unsigned        DIGENTRY DIPImpModName( imp_image_handle *ii,
                        imp_mod_handle im, char *buff, unsigned buff_size )
{
    ji_ptr      name;
    unsigned    len;

    name = GetPointer( ii->cc + offsetof( ClassClass, name ) );
    len = GetString( name, NameBuff, sizeof( NameBuff ) );
    NormalizeClassName( NameBuff, len );
    return( NameCopy( buff, NameBuff, buff_size, len ) );
}

char            *DIGENTRY DIPImpModSrcLang( imp_image_handle *ii, imp_mod_handle im )
{
    return( "java" );
}

dip_status      DIGENTRY DIPImpModInfo( imp_image_handle *ii,
                                imp_mod_handle im, handle_kind hk )
{
    return( DS_OK );
}

search_result   DIGENTRY DIPImpAddrMod( imp_image_handle *ii, address a,
                imp_mod_handle *im )
{
    search_result       sr;
    unsigned            i;

    sr = FindMBIndex( ii, a.mach.offset, &i );
    switch( sr ) {
    case SR_EXACT:
    case SR_CLOSEST:
        *im = IMH_JAVA;
        break;
    }
    return( sr );
}

address         DIGENTRY DIPImpModAddr( imp_image_handle *ii,
                                imp_mod_handle im )
{
    address     a;
    unsigned    i;

    for( i = 0; i < ii->num_methods; ++i ) {
        if( !(ii->methods[i].fb.access & ACC_NATIVE) ) {
            a = DefCodeAddr;
            a.mach.offset = (ji_ptr)ii->methods[i].code;
            return( a );
        }
    }
    return( NilAddr );
}

dip_status      DIGENTRY DIPImpModDefault( imp_image_handle *ii,
                imp_mod_handle im, default_kind dk, dip_type_info *ti )
{
     return( DS_FAIL );
}

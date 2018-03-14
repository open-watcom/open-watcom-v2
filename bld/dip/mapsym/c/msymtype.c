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
* Description:  Stuff dealing with type handles.
*
****************************************************************************/


#include "msym.h"


/* Note: Not much to do here. Could possibly try to guess data vs. code by
 * looking at segment names, but results would not necessarily be accurate.
 */

walk_result DIPIMPENTRY( WalkTypeList )( imp_image_handle *ii,
                imp_mod_handle im, DIP_IMP_TYPE_WALKER *wk, imp_type_handle *it, void *d )
{
    /* unused parameters */ (void)ii; (void)im; (void)wk; (void)it; (void)d;

    return( WR_CONTINUE );
}

imp_mod_handle DIPIMPENTRY( TypeMod )( imp_image_handle *ii, imp_type_handle *it )
{
    /* unused parameters */ (void)ii; (void)it;

    return( IMH_MAP );
}

dip_status DIPIMPENTRY( TypeInfo )( imp_image_handle *ii,
                imp_type_handle *it, location_context *lc, dip_type_info *ti )
{
    /* unused parameters */ (void)ii; (void)lc;

    ti->modifier = TM_NONE;
    ti->size = 0;
    if( it->code ) {
        ti->kind = TK_CODE;
    } else {
        ti->kind = TK_DATA;
    }
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeBase )( imp_image_handle *ii,
                        imp_type_handle *it, imp_type_handle *base,
                        location_context *lc, location_list *ll )
{
    /* unused parameters */ (void)ii; (void)lc; (void)ll;

    *base = *it;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeArrayInfo )( imp_image_handle *ii,
                        imp_type_handle *array, location_context *lc,
                        array_info *ai, imp_type_handle *index )
{
    /* unused parameters */ (void)ii; (void)lc;

    ai->low_bound = 0;
    ai->num_elts = 0;
    ai->stride = 0;
    ai->num_dims = 0;
    ai->column_major = 0;
    if( index != NULL ) *index = *array;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeProcInfo )( imp_image_handle *ii,
                imp_type_handle *proc, imp_type_handle *parm, unsigned n )
{
    /* unused parameters */ (void)ii; (void)n;

    *parm = *proc;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypePtrAddrSpace )( imp_image_handle *ii,
                    imp_type_handle *it, location_context *lc, address *a )
{
    /* unused parameters */ (void)ii; (void)it; (void)lc; (void)a;

    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeThunkAdjust )( imp_image_handle *ii,
                        imp_type_handle *base, imp_type_handle *derived,
                        location_context *lc, address *addr )
{
    /* unused parameters */ (void)ii; (void)base; (void)derived; (void)lc; (void)addr;

    return( DS_FAIL );
}

int DIPIMPENTRY( TypeCmp )( imp_image_handle *ii, imp_type_handle *it1, imp_type_handle *it2 )
{
    /* unused parameters */ (void)ii;

    if( it1 < it2 )
        return( -1 );
    if( it1 > it2 )
        return( 1 );
    return( 0 );
}

size_t DIPIMPENTRY( TypeName )( imp_image_handle *ii, imp_type_handle *it,
                unsigned num, symbol_type *tag, char *buff, size_t buff_size )
{
    /* unused parameters */ (void)ii; (void)it; (void)num; (void)tag; (void)buff; (void)buff_size;

    return( 0 );
}

dip_status DIPIMPENTRY( TypeAddRef )( imp_image_handle *ii, imp_type_handle *it )
{
    /* unused parameters */ (void)ii; (void)it;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeRelease )( imp_image_handle *ii, imp_type_handle *it )
{
    /* unused parameters */ (void)ii; (void)it;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeFreeAll )( imp_image_handle *ii )
{
    /* unused parameters */ (void)ii;

    return( DS_OK );
}

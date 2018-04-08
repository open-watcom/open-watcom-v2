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
    Stuff dealing with type handles.
*/


walk_result DIPIMPENTRY( WalkTypeList )( imp_image_handle *iih,
                    imp_mod_handle imh, DIP_IMP_TYPE_WALKER *wk, imp_type_handle *ith,
                    void *d )
{
    /* unused parameters */ (void)iih; (void)imh; (void)wk; (void)ith; (void)d;

    return( WR_CONTINUE );
}

imp_mod_handle DIPIMPENTRY( TypeMod )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( IMH_EXPORT );
}

dip_status DIPIMPENTRY( TypeInfo )( imp_image_handle *iih, imp_type_handle *ith, location_context *lc, dig_type_info *ti )
{
    /* unused parameters */ (void)iih; (void)lc;

    if( ith->code ) {
        ti->kind = TK_CODE;
    } else {
        ti->kind = TK_DATA;
    }
    ti->size = 0;
    ti->modifier = TM_NONE;
    ti->deref = false;
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeBase )( imp_image_handle *iih,
                        imp_type_handle *ith, imp_type_handle *base_ith,
                        location_context *lc, location_list *ll )
{
    /* unused parameters */ (void)iih; (void)lc; (void)ll;

    *base_ith = *ith;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeArrayInfo )( imp_image_handle *iih,
                        imp_type_handle *array_ith, location_context *lc,
                        array_info *ai, imp_type_handle *index_ith )
{
    /* unused parameters */ (void)iih; (void)lc;

    ai->low_bound = 0;
    ai->num_elts = 0;
    ai->stride = 0;
    ai->num_dims = 0;
    ai->column_major = 0;
    if( index_ith != NULL )
        *index_ith = *array_ith;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeProcInfo )( imp_image_handle *iih,
                imp_type_handle *proc_ith, imp_type_handle *parm_ith, unsigned n )
{
    /* unused parameters */ (void)iih; (void)n;

    *parm_ith = *proc_ith;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypePtrAddrSpace )( imp_image_handle *iih,
                    imp_type_handle *ith, location_context *lc, address *a )
{
    /* unused parameters */ (void)iih; (void)ith; (void)lc; (void)a;

    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeThunkAdjust )( imp_image_handle *iih,
                        imp_type_handle *base_ith, imp_type_handle *derived_ith,
                        location_context *lc, address *addr )
{
    /* unused parameters */ (void)iih; (void)base_ith; (void)derived_ith; (void)lc; (void)addr;

    return( DS_FAIL );
}

int DIPIMPENTRY( TypeCmp )( imp_image_handle *iih, imp_type_handle *ith1, imp_type_handle *ith2 )
{
    /* unused parameters */ (void)iih;

    if( ith1 < ith2 )
        return( -1 );
    if( ith1 > ith2 )
        return( 1 );
    return( 0 );
}

size_t DIPIMPENTRY( TypeName )( imp_image_handle *iih, imp_type_handle *ith,
                unsigned num, symbol_type *tag, char *buff, size_t buff_size )
{
    /* unused parameters */ (void)iih; (void)ith; (void)num; (void)tag; (void)buff; (void)buff_size;

    return( 0 );
}

dip_status DIPIMPENTRY( TypeAddRef )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeRelease )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeFreeAll )( imp_image_handle *iih )
{
    /* unused parameters */ (void)iih;

    return( DS_OK );
}

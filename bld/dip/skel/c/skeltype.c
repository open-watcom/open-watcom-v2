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
 * Stuff dealing with type handles.
 */

walk_result DIPIMPENTRY( WalkTypeList )( imp_image_handle *iih, imp_mod_handle imh,
                            DIP_IMP_TYPE_WALKER *wk, imp_type_handle *ith, void *d )
/***********************************************************************************
 * TODO:
 *
 * PSEUDO-CODE:
 *
 * for( each type in the module ) {
 *     *ith = fill in the implemenation type handle;
 *     wr = wk( iih, ith, d );
 *     if( wr != WR_CONTINUE ) {
 *         return( wr );
 *     }
 * }
 */
{
    return( WR_CONTINUE );
}

imp_mod_handle DIPIMPENTRY( TypeMod )( imp_image_handle *iih, imp_type_handle *ith )
/***********************************************************************************
 * TODO:
 *
 * Return the module that the type handle comes from.
 */
{
    return( IMH_NOMOD );
}

dip_status DIPIMPENTRY( TypeInfo )( imp_image_handle *iih, imp_type_handle *ith,
                                        location_context *lc, dig_type_info *ti )
/********************************************************************************
 * TODO:
 *
 * Fill in the type information for the type handle. The location
 * context is being passed in because it might be needed to calculate
 * the size of the type (variable dimensioned arrays and the like).
 */
{
    ti->kind = TK_NONE;
    ti->size = 0;
    ti->modifier = TM_NONE;
    ti->deref = false;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeBase )( imp_image_handle *iih, imp_type_handle *ith,
            imp_type_handle *base_ith, location_context *lc, location_list *ll )
/*******************************************************************************
 * TODO:
 *
 * Given an implementation type handle, fill in 'base' with the
 * base type of the handle.
 * If 'll' is non-NULL, it points to a location list describing the
 * object that TypeBase is being performed on. If doing the TypeBase
 * operation implies some special magic operation on the object that
 * is not described by the typing information (following pointers, etc)
 * then the passed in location list should be adjusted to point at the
 * start of the data for the object described by the result of the
 * TypeBase operation.
 */
{
     *base_ith = *ith;
     return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeArrayInfo )( imp_image_handle *iih,
                        imp_type_handle *array_ith, location_context *lc,
                        array_info *ai, imp_type_handle *index_ith )
/************************************************************************
 * TODO:
 *
 * Given an implemenation type handle that represents an array type,
 * get information about the array shape and index type. The location
 * context is for variable dimensioned arrays again. The 'index'
 * parameter is filled in with the type of variable used to subscript
 * the array. It may be NULL, in which case no information is returned.
 */
{
    ai->low_bound = 0;
    ai->num_elts = 0;
    ai->stride = 0;
    ai->num_dims = 0;
    ai->column_major = 0; /* 1 for fortran */
    if( index_ith != NULL )
        *index_ith = *array_ith;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeProcInfo )( imp_image_handle *iih, imp_type_handle *proc_ith,
                                            imp_type_handle *parm_ith, unsigned n )
/****************************************************************************************
 * TODO:
 *
 * Given an implementation type handle that represents a procedure type,
 * get information about the return and parameter types. If the 'n'
 * parameter is zero, store the return type handle into the 'parm'
 * variable. Otherwise store the handle for the n'th parameter in
 * 'parm'.
 */
{
    *parm_ith = *proc_ith;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypePtrAddrSpace )( imp_image_handle *iih,
                    imp_type_handle *ith, location_context *lc, address *a )
/***************************************************************************
 * Given an implementation type handle that represents a pointer type,
 * get information about any implied address space for that pointer
 * (based pointer cruft). If there is an implied address space for
 * the pointer, fill in *a with the information and return DS_OK.
 * Otherwise return DS_FAIL.
 */
{
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeThunkAdjust )( imp_image_handle *iih, imp_type_handle *base_ith,
                        imp_type_handle *derived_ith, location_context *lc, address *addr )
/*******************************************************************************************
 * TODO:
 *
 * When you convert a pointer to a C++ class to a pointer at one
 * of its derived classes you have to adjust the pointer so that
 * it points at the start of the derived class. The 'derived' type
 * may not actually be a derived type of 'base'. In that case, return
 * DS_FAIL and add nothing to 'addr'. If it is a derived type, let 'disp'
 * be the displacement between the 'base' type and the 'derived' type.
 * You need to do the following. "addr->mach.offset += disp;".
 */
{
    return( DS_FAIL );
}

int DIPIMPENTRY( TypeCmp )( imp_image_handle *iih, imp_type_handle *ith1, imp_type_handle *ith2 )
/************************************************************************************************
 * TODO:
 *
 * Compare two type handles and return 0 if they refer to the same
 * information. If they refer to differnt things return either a
 * positive or negative value to impose an 'order' on the information.
 * The value should obey the following constraints.
 * Given three handles H1, H2, H3:
 *         - if H1 < H2 then H1 is always < H2
 *         - if H1 < H2 and H2 < H3 then H1 is < H3
 * The reason for the constraints is so that a client can sort a
 * list of handles and binary search them.
 */
{
    return( 0 );
}

size_t DIPIMPENTRY( TypeName )( imp_image_handle *iih, imp_type_handle *ith,
                unsigned num, symbol_type *tag, char *buff, size_t buff_size )
/*****************************************************************************
 * TODO:
 *
 * Given the imp_type_handle, copy the name of the type into 'buff'.
 * Do not copy more than 'buff_size' - 1 characters into the buffer and
 * append a trailing '\0' character. Return the real length
 * of the type name (not including the trailing '\0' character) even
 * if you had to truncate it to fit it into the buffer. If something
 * went wrong and you can't get the type name, call DCStatus and
 * return zero. NOTE: the client might pass in zero for 'buff_size'. In that
 * case, just return the length of the module name and do not attempt
 * to put anything into the buffer.
 *
 * Since there can be a "string" of typedef names associated with
 * a type_handle, the 'num' parm indicates which one of the names
 * the client wants returned. Zero is the first type name, one is
 * the second, etc. Fill in '*tag' with ST_ENUM_TAG, ST_UNION_TAG,
 * ST_STRUCT_TAG, ST_CLASS_TAG if the name is a enum, union, struct,
 * or class tag name respectively. If not, set '*tag' to ST_NONE.
 *
 * If the type does not have a name, return zero.
 */
{
    return( 0 );
}

/*
 * The following 3 routines are for resource management of type handles.
 * They should be implemented if the implementation wants to store pointers
 * to resources that must be freed in an imp_type_handle.  Here is an
 * example of how they could be used.
 *
 * typedef struct type_ref {
 *     int     signature;
 *     struct type_ref *link;
 *     int     refCount;
 *     void    *otherValuableResource;
 * }
 *
 * typedef struct {
 *     type_ref        *r;
 * } imp_type_handle;
 *
 * type_ref *TypesList;
 *
 * Note: if you do implement these routines, it is a good idea to have
 * a signature to validate type handles IN EVERY ENTRY POINT that takes
 * an imp_type_handle*.  ie:
 *
 * #define ValidType( t ) assert( t->r->signature == 0xC0DEAB1E );
 *
 *
 * Finally, you may assume that DIPImpTypeFreeAll and DIPImpSymFreeAll
 * are called at the same time
 *
 */
dip_status DIPIMPENTRY( TypeAddRef )( imp_image_handle *iih, imp_type_handle *ith )
/*********************************************************************************/
{
//    ith->r->refCount++;
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeRelease )( imp_image_handle *iih, imp_type_handle *ith )
/**********************************************************************************/
{
//    ith->r->refCount--;
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeFreeAll )( imp_image_handle *iih )
/************************************************************/
{
#if 0
    for( type = TypesList; type != NULL; type = type->link ) {
        if( type->r->refCount == 0 ) {
            free type->r->otherValuableResource;
            type->r->signature = 0xDEADBEEF;
            free type->r
        }
    }
#endif
    return( DS_OK );
}

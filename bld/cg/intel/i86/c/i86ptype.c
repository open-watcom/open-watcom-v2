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
* Description:  Type mappings.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "procdef.h"


extern type_class_def MapIntReturn( cg_type type )
/************************************************/
{
    switch( type ) {
    case TY_INT_1: return( I1 );
    case TY_INT_2: return( I2 );
    case TY_INT_4: return( I4 );
    case TY_INT_8: return( I8 );
    case TY_UINT_1: return( U1 );
    case TY_UINT_2: return( U2 );
    case TY_UINT_4: return( U4 );
    case TY_UINT_8: return( U8 );
    }
    return( XX );
}


extern type_class_def MapPointer( cg_type type )
/***********************************************
    return the internal type associated with
    pointer type given. This varies depending upon
    the archtecture
*/
{
    switch( type ) {
    case TY_NEAR_POINTER:
    case TY_NEAR_CODE_PTR:
        return( U2 );
    case TY_HUGE_POINTER:
        return( PT );
    case TY_LONG_CODE_PTR:
    case TY_LONG_POINTER:
        return( CP );
    }
    return( XX );
}


extern  type_class_def  MapFloat( cg_type type, call_attributes attr )
/*********************************************************************
    called by the return value generator to decide whether to treat
    floating point return values as floats or structs.
*/
{
    if( attr & ROUTINE_NO_FLOAT_REG_RETURNS ) return( XX );
    if( type == TY_SINGLE ) return( FS );
    if( type == TY_LONG_DOUBLE ) return( FL );
    return( FD );
}


extern  type_class_def  MapStruct( type_length length, call_attributes attr )
/****************************************************************************
    called by the return value generator to decide whether to treat
    1/2/4 byte struct return values as ints or structs.
*/
{
    if( attr & ROUTINE_NO_STRUCT_REG_RETURNS ) return( XX );
    if( length == 1 ) return( U1 );
    if( length == 2 ) return( U2 );
    if( length == 4 ) return( U4 );
    // if( length == 8 ) return( U8 );
    return( XX );
}

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


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "procdef.h"

#include "s37ptype.def"

extern type_class_def MapIntReturn( cg_type type ) {
/***************************************************

*/
    type=type;
    return( WD );
}


extern type_class_def MapPointer( cg_type type ) {
/*************************************************
    return the internal type associated with
    pointer type given. This varies depending upon
    the archtecture
*/

    type = type;
    return( PT );
}


extern  type_class_def  MapFloat( cg_type type, call_attributes attr ) {
/***********************************************************************
    called by the return value generator to decide whether to treat
    floating point return values as floats or structs.
*/

    attr = attr;
    if( type == TY_SINGLE ) return( FS );
    return( FD );
}


extern  type_class_def  MapStruct( type_length length, call_attributes attr ) {
/******************************************************************************
    called by the return value generator to decide whether to treat
    1/2/4 byte struct return values as ints or structs.
*/

    length = length; attr = attr;
    return( XX );
}

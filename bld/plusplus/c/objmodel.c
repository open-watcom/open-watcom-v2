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


#include "plusplus.h"
#include "cgfront.h"
#include "errdefns.h"
#include "class.h"
#include "objmodel.h"



OMR ObjModelFunctionReturn(     // DETERMINE TYPE OF RETURN FOR OBJECT MODEL
    TYPE fun_type )             // - a function type
{
    OMR retn;                   // - kind of return (OMR_...)

    fun_type = FunctionDeclarationType( fun_type );
    switch( TypedefModifierRemoveOnly( fun_type->of )->id ) {
      case TYP_VOID :
        retn = OMR_VOID;
        break;
      case TYP_CLASS :
        if( fun_type->flag & TF1_PLUSPLUS
         && ClassParmIsRef( StructType( fun_type->of ) ) ) {
            retn = OMR_CLASS_REF;
        } else {
            retn = OMR_CLASS_VAL;
        }
        break;
      default :
        retn = OMR_SCALAR;
        break;
    }
    return retn;
}


OMR ObjModelArgument(           // DETERMINE HOW TYPE PASSED IN OBJECT MODEL
    TYPE atype )                // - argument type
{
    OMR retn;                   // - kind of return (OMR_...)

    atype = StructType( atype );
    if( atype == NULL ) {
        retn = OMR_SCALAR;
    } else if( ClassParmIsRef( atype ) ) {
        retn = OMR_CLASS_REF;
    } else {
        retn = OMR_CLASS_VAL;
    }
    return retn;
}

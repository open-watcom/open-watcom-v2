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


#include "cpplib.h"
#include "rtexcept.h"


extern "C"
size_t CPPLIB( ts_size )(       // GET SIZE OF ELEMENT FROM TYPE SIGNATURE
    RT_TYPE_SIG sig )           // - type signature
{
    size_t size;                // - size of an element

    sig = CPPLIB( ts_refed )( sig );
    switch( sig->hdr.type ) {
    case THROBJ_SCALAR :
    case THROBJ_PTR_FUN :
        size = sig->scalar.size;
        break;
    case THROBJ_CLASS :
    case THROBJ_CLASS_VIRT :
        size = sig->clss.size;
        break;
    case THROBJ_VOID_STAR :
    case THROBJ_PTR_SCALAR :
    case THROBJ_PTR_CLASS :
        size = sizeof( void* );
        break;
    default:
        GOOF_EXC( "unexpected type signature" );
    }
    return size;
}

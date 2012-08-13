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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "cpplib.h"
#include "rt_rtti.h"

#include <typeinfo.h>
// works around an EH problem
#pragma inline_depth(0)

static void foundNoTypeid( void )
{
#ifdef RT_EXC_ENABLED
    throw bad_typeid( );
#endif
}

extern "C"
_WPRTLINK
void const * CPPLIB(gettid)( void *p, unsigned delta, type_info const *from ) {
    auto rtti_leap leap_storage;
    auto rtti_exec data;

    if( p == NULL ) {
        foundNoTypeid();
        /* may return */
        return( NULL );
    }
    _INIT_EXEC( data, ((char*)p) + delta, from, NULL, NULL );
    if( __CalcMostDerived( &data ) != NULL ) {
        if( __MakeSureTidIsPresent( &data, data.from_tid, &leap_storage ) != NULL ) {
            return( data.md_class->id );
        }
    }
    foundNoTypeid();
    /* may return */
    return( NULL );
}

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


#include <stddef.h>
#include <stdio.h>
/*#include <string.h>*/

#include "cpplib.h"
#include "rt_rtti.h"


extern "C"
void *__DoDynamicCast( rtti_exec *data ) {
    unsigned **vbptr;
    rtti_leap const *base_leap;
    rtti_class const *md_class;
    type_info const *to_tid;
    rtti_bad throw_except;
    char *return_value;
    auto rtti_leap leap_storage;

    data->ok = 0;
    if( __CalcMostDerived( data ) != NULL ) {
        if( __MakeSureTidIsPresent( data, data->from_tid, &leap_storage ) != NULL ) {
            to_tid = data->to_tid;
            if( to_tid != NULL ) {
                base_leap = __MakeSureTidIsPresent( data, to_tid, &leap_storage );
                if( base_leap != NULL ) {
                    if(( base_leap->control & RL_BAD_BASE ) == 0 ) {
                        return_value = (char *) data->md_addr;
                        if( base_leap->control & RL_VIRTUAL ) {
                            md_class = data->md_class;
                            return_value += md_class->vbptr;
                            vbptr = (unsigned **) return_value;
                            return_value += (*vbptr)[ base_leap->vb_index ];
                        }
                        return_value += base_leap->offset;
                        data->ok = 1;
                    }
                }
            } else {
                /* cast to 'void cv *' */
                return_value = (char *) data->md_addr;
                data->ok = 1;
            }
        }
    }
    if( ! data->ok ) {
        throw_except = data->throw_except;
        if( throw_except != NULL ) {
            throw_except();
            /* may return */
        }
        return_value = NULL;
    }
    return( return_value );
}

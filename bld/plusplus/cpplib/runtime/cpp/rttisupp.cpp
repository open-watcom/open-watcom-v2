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
void *__CalcMostDerived( rtti_exec *data ) {
    char *pc;
    void *vftable;
    unsigned from_here;
    rtti_adjust *adjust;

    vftable = *((void **) data->vfptr );
    adjust = ((rtti_adjust **) vftable )[-1];
    if( adjust == NULL ) {
        /* vftable has no RTTI descriptor */
        return( NULL );
    }
    pc = (char *) data->vfptr;
    pc -= adjust->delta;
    from_here = adjust->from_here;
    if( from_here & RA_CDISP ) {
        pc = pc - ((unsigned*)pc)[-1];
    }
    from_here &= ~RA_RUNTIME;
    data->md_class = (rtti_class *) ( from_here + (char*) adjust );
    data->md_addr = pc;
    return( pc );
}

extern "C"
rtti_leap const *__MakeSureTidIsPresent( rtti_exec *data, type_info const *find, rtti_leap *pleap ) {
    rtti_class const *rt_class;
    rtti_leap const *leap;

    rt_class = data->md_class;
    if( *find == *(rt_class->id) ) {
        pleap->vb_index = 0;
        pleap->offset = 0;
        pleap->control = RL_NULL;
        pleap->id = find;
        return( pleap );
    }
    if( rt_class->vbptr != -1 ) {
        leap = rt_class->leaps;
        for(;;) {
            if( *find == *(leap->id) ) {
                return( leap );
            }
            if( leap->control & RL_LAST ) break;
            ++leap;
        }
    }
    return( NULL );
}

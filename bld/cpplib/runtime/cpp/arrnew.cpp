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


// ARRNEW.CPP -- support to call constructor for each element in a newly
//               allocated array
//            -- and set count field
//
// 91/11/07 -- J.W.Welch        -- defined
// 92/12/02 -- J.W.Welch        -- adapt to type signatures
// 94/12/12 -- J.W.Welch        -- moved to CPP

#include "cpplib.h"


extern "C"
void * CPPLIB( new_array )(         // CALL CONSTRUCTORS FOR NEW ARRAY ELEMENTS
    ARRAY_STORAGE *new_alloc,       // - what was allocated
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - type signature for array type
{
    void *retn;                     // - return: NULL or first element

    if( new_alloc == NULL ) {
        retn = NULL;
    } else {
        new_alloc->element_count = count;
        retn = new_alloc->apparent_address;
        retn = CPPLIB( ctor_array )( retn, count, sig );
    }
    return( retn );
}


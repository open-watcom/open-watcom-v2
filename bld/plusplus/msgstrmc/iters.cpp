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


// ITERS -- implementation for Iter, IterOver
//
// 92/12/30 -- J.W.Welch        -- defined


#include "iters.h"


void IterOver::iterateOverElements( // DO ITERATIONS
    Iter& iterator,             // - iterator
    void* control_info )        // - control information
{
    start( control_info );
    for( ; iterator.next(); ) {
        process( iterator.element(), control_info );
    }
    complete( control_info );
}


void IterOver::start(           // START ITERATING
    void* control_info )        // - control information
{
    control_info = control_info;
}


void IterOver::complete(        // COMPLETE ITERATING
    void* control_info )        // - control information
{
    control_info = control_info;
}

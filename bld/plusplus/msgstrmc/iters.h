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


#ifndef __ITERS_H__
#define __ITERS_H__

#include "truefals.h"

// Iters -- iteration classes
//
// 92/12/30 -- J.W.Welch        -- defined


// Iter -- basic iterator

class Iter
{

public:                         // public functions
                                // ----------------
    virtual Boolean next        // NEXT ITERATION
        ( void ) = 0;
    virtual void* element       // GET ELEMENT POINTER
        ( void ) const = 0;

};


// IterOver -- do iteration

class IterOver
{
public:                         // public functions
                                // ----------------
    virtual void start          // PROCESSING AT START
        ( void* control_info ); // - control information

    virtual void complete       // PROCESSING AT END
        ( void* control_info ); // - control information

    virtual void process        // PROCESS EACH ITERATION
        ( void* element         // - element
        , void* control_info )  // - control information
        = 0;
    void iterateOverElements    // ITERATE OVER THE ELEMENTS
        ( Iter& iterator        // - iterator
        , void* control_info ); // - control information
};


#endif

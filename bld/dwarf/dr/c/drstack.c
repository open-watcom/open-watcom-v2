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
* Description:  Stack helper functions.
*
****************************************************************************/


#include <string.h>

#include "drstack.h"
#include "drpriv.h"

extern void DWRStackCreate(                 // INITIALIZE A STACK
    dr_stack *stk,                          // -- stack to initialize
    uint start_size )                       // -- initial size guess
/*************************/
{
    stk->stack = DWRALLOC( start_size * sizeof( uint_32 ) );
    stk->free = 0;
    stk->size = start_size;
}

extern void DWRStackCopy(                   // COPY A STACK FROM ANOTHER
    dr_stack *dest,                         // -- destination of copy
    const dr_stack *src )                   // -- source of copy
/************************/
{
    dest->free = src->free;
    dest->size = src->size;
    dest->stack = DWRALLOC( dest->size * sizeof( uint_32 ) );
    memcpy( dest->stack, src->stack, dest->size * sizeof( uint_32 ) );
}

extern void DWRStackFree(                   // DESTRUCT A STACK
    dr_stack *stk )                         // -- stack to trash
/***********************/
{
    DWRFREE( stk->stack );
    stk->stack = NULL;
    stk->size = 0;
    stk->free = 0;
}

extern void DWRStackPush(                   // PUSH ITEM ON THE STACK
    dr_stack *stk,                          // -- stack to push on
    uint_32 val )                           // -- value to push
/***********************/
{
    if( stk->free >= stk->size ) {
        stk->size *= 2;
        stk->stack = DWRREALLOC( stk->stack, stk->size * sizeof( uint_32 ) );
    }

    stk->stack[ stk->free ] = val;
    stk->free += 1;
}

extern uint_32 DWRStackPop(                 // POP ITEM OFF THE STACK
    dr_stack *stk )                         // -- stack to pop off of
/*************************/
{
    if( stk->free == 0 ) {
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }

    stk->free -= 1;
    return stk->stack[ stk->free ];
}

extern uint_32 DWRStackTop(                 // RETURN TOP ELEMENT OF STACK
    dr_stack *stk )                         // -- stack to use
/*************************/
{
    if( stk->free == 0 ) {
        return 0;
    } else {
        return stk->stack[ stk->free - 1 ];
    }
}

extern bool DWRStackEmpty(                  // IS A STACK EMPTY?
    dr_stack *stk )                         // -- stack to check
/*************************/
{
    return( stk->free == 0 );
}

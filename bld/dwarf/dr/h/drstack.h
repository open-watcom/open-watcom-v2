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


#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include "watcom.h"
#include "bool.h"

/*
 * This header file defines two stacks.
 *
 * The first is dr_stack, a safe, reallocatable stack that can
 * be retained and passed around, but is a bit slow.  It throws
 * a DWREXCEP if you pop an empty stack.
 *
 * The second is an interface to the program stack with push and
 * pop operations.  It is faster, but "a scary low-level hack" as
 * it makes a lot of assumptions about code generation, and assumes
 * that the user _really_ knows whats going on. Be _very_, _very_
 * careful when using these functions, and make _sure_ that you do
 * not blow the stack when using them!!!!
 */

typedef struct {                            // PROVIDE A RESIZABLE STACK
    uint        size;                       // -- allocated space
    uint        free;                       // -- next free entry
    uint_32 *   stack;                      // -- reallocatable array
} dr_stack;

extern void DWRStackCreate(                 // INITIALIZE A STACK
    dr_stack * stk,                         // -- stack to initialize
    uint start_size );                      // -- initial size guess

extern void DWRStackCopy(                   // COPY A STACK FROM ANOTHER
    dr_stack * dest,                        // -- destination of copy
    const dr_stack * src );                 // -- source of copy

extern void DWRStackFree(                   // DESTRUCT A STACK
    dr_stack * stk );                       // -- stack to trash

extern void DWRStackPush(                   // PUSH ITEM ON THE STACK
    dr_stack * stk,                         // -- stack to push on
    uint_32 val );                          // -- value to push

extern uint_32 DWRStackPop(                 // POP ITEM OFF THE STACK
    dr_stack * stk );                       // -- stack to pop off of

extern uint_32 DWRStackTop(                 // RETURN TOP ELEMENT OF STACK
    dr_stack * stk );                       // -- stack to use

extern bool DWRStackEmpty(                  // IS A STACK EMPTY?
    dr_stack * stk );                       // -- stack to check


#if 0
extern uint_32 _stkpop( void );
extern void    _stkpush( uint_32 );

#if defined(__386__)                        // 32-bit land

#pragma aux _stkpop =   \
    "pop eax"           \
    parm value [eax] modify nomemory [esp];

#pragma aux _stkpush =  \
    "push eax"          \
    parm caller [eax] modify nomemory [esp];

#else                                       // 16-bit land

#pragma aux _stkpop =   \
    "pop ax"            \
    "pop dx"            \
    parm value [ax dx] modify nomemory [sp];

#pragma aux _stkpush =  \
    "push dx"           \
    "push ax"           \
    parm caller [ax dx] modify nomemory [sp];

#endif  // __386__
#endif

#endif // STACK_H_INCLUDED

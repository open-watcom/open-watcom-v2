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


#define SPACE_LIMIT             25      // I got this by trial and error
#define SPACE_MIN               1500000

typedef struct stack_element {
    HBITMAP                     xorbitmap;
    HBITMAP                     andbitmap;
    struct stack_element        *next;
    struct stack_element        *previous;
} an_undo_node;

typedef struct a_stack {
    HWND                        hwnd;
    int                         opcount;
    int                         redocount;
    int                         current_icon;           // for icons only
    HBITMAP                     original_xor;
    HBITMAP                     original_and;
    HBITMAP                     firstxor;
    HBITMAP                     firstand;
    BOOL                        modified;
    an_undo_node                *top_undo;
    an_undo_node                *bottom_undo;
    an_undo_node                *top_redo;
    struct a_stack              *next;
    struct a_stack              *previous;
    struct a_stack              *nexticon;              // for icons
} an_undo_stack;

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


#include "vi.h"

/*
 * dropUndoStackTop - clear top of undo stack
 */
static void dropUndoStackTop( undo_stack *stack )
{
    int i;

    if( stack->current < 0 ) {
        return;
    }
    UndoFree( stack->stack[0], true );
    for( i = 1; i <= stack->current; i++ ) {
        stack->stack[i - 1] = stack->stack[i];
    }
    stack->rolled = true;

} /* dropUndoStackTop */

/*
 * shrinkUndoStack - reduce size of undo stack by one
 */
static void shrinkUndoStack( undo_stack *stack )
{
    stack->current--;
    if( stack->current < 0 ) {
        MemFreePtr( (void **)&stack->stack );
    } else {
        stack->stack = MemReAlloc( stack->stack, (stack->current + 1) * sizeof( undo * ) );
    }

} /* shrinkUndoStack */

/*
 * UndoAlloc - allocate memory for an undo
 */
undo *UndoAlloc( undo_stack *stack, int type )
{
    undo        *tmp;
    int         size;

    if( EditFlags.UndoLost ) {
        return( NULL );
    }

    size = UNDO_SIZE - sizeof( undo_data );
    switch( type ) {
    case START_UNDO_GROUP:
        size += sizeof( undo_start );
        break;
    case UNDO_DELETE_FCBS:
        size += sizeof( undo_delete );
        break;
    case UNDO_INSERT_LINES:
        size += sizeof( undo_insert );
        break;
    case END_UNDO_GROUP:
        break;
    }

    for( ;; ) {
        tmp = MemAllocUnsafe( size );
        if( tmp != NULL ) {
            memset( tmp, 0, size );
            break;
        }
        if( stack->current > 0 ) {
            dropUndoStackTop( stack );
            shrinkUndoStack( stack );
        } else {
            if( stack->current == 0 ) {
                dropUndoStackTop( stack );
                shrinkUndoStack( stack );
                EditFlags.UndoLost = true;
            }
            return( NULL );
        }
    }
    tmp->type = (char) type;

    return( tmp );

} /* UndoAlloc */

/*
 * UndoFree - release an undo entry
 */
void UndoFree( undo *cundo, bool freefcbs )
{
    undo    *tundo;

    while( cundo != NULL ) {
        /*
         * release any fcbs
         */
        if( freefcbs && cundo->type == UNDO_DELETE_FCBS ) {
            FreeFcbList( cundo->data.fcbs.head );
        }
        tundo = cundo->next;
        MemFree( cundo );
        cundo = tundo;
    }

} /* UndoFree */

/*
 * PushUndoStack - push entry onto top of undo stack
 */
void PushUndoStack( undo *item, undo_stack *stack )
{
    void        *tmp;

    tmp = MemReAllocUnsafe( stack->stack, (stack->current + 2) * sizeof( undo * ) );
    if( tmp == NULL ) {
        dropUndoStackTop( stack );
    } else {
        stack->current++;
        stack->stack = tmp;
    }
    stack->stack[stack->current] = item;

} /* PushUndoStack */

/*
 * PopUndoStack - return top entry on undo stack (don't free it)
 */
undo *PopUndoStack( undo_stack *stack )
{
    undo        *tmp;

    if( stack->current < 0 ) {
        return( NULL );
    }

    tmp = stack->stack[stack->current];
    shrinkUndoStack( stack );

    return( tmp );

} /* PopUndoStack */

/*
 * PurgeUndoStack - do just that
 */
void PurgeUndoStack( undo_stack *stack )
{
    int i;

    if( stack == NULL ) {
        return;
    }

    for( i = stack->current; i >= 0; i-- ) {
        UndoFree( stack->stack[i], true );
    }
    MemFreePtr( (void **)&(stack->stack) );
    stack->current = -1;

} /* PurgeUndoStack */

/*
 * AddUndoToCurrent - tack an undo to the end of the current stack entry
 */
void AddUndoToCurrent( undo *item, undo_stack *stack )
{
    undo        *cundo;

    cundo = stack->stack[stack->current];
    stack->stack[stack->current] = item;
    item->next = cundo;

} /* AddUndoToCurrent */

/*
 * AllocateUndoStacks - allocate current undo stacks
 */
void AllocateUndoStacks( void )
{
    UndoStack = MemAlloc( sizeof( undo_stack ) );
    UndoUndoStack = MemAlloc( sizeof( undo_stack ) );
    UndoStack->stack = NULL;
    UndoUndoStack->stack = NULL;
    UndoStack->current = -1;
    UndoUndoStack->current = -1;

} /* AllocateUndoStacks */

/*
 * FreeUndoStacks - do just that
 */
void FreeUndoStacks( void )
{
    FreeAllUndos();
    MemFree( UndoStack->stack );
    MemFreePtr( (void **)&UndoStack );
    MemFree( UndoUndoStack->stack );
    MemFreePtr( (void **)&UndoUndoStack );

} /* FreeUndoStacks */

/*
 * doTossUndo - toss the top undo
 */
static bool doTossUndo( undo_stack *stack )
{
    if( stack->current >= 0 ) {
        if( stack->current == 0 ) {
            if( stack->OpenUndo ) {
                EditFlags.UndoLost = true;
            }
        }
        dropUndoStackTop( stack );
        shrinkUndoStack( stack );
        return( true );
    }
    return( false );

} /* doTossUndo */

/*
 * TossUndos - remove LRU undo memory
 */
bool TossUndos( void )
{
    info        *cinfo;
    info        *least;
    undo_stack  *stack;

    least = NULL;
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        stack = cinfo->UndoStack;
        if( stack->current >=0 ) {
            if( least == NULL ) {
                least = cinfo;
            } else {
                if( least->UndoStack->stack[0]->data.sdata.time_stamp <
                    stack->stack[0]->data.sdata.time_stamp ) {
                    least = cinfo;
                }
            }
        }
        stack = cinfo->UndoUndoStack;
        if( stack->current >= 0 ) {
            if( least == NULL ) {
                least = cinfo;
            } else {
                if( least->UndoUndoStack->stack[0]->data.sdata.time_stamp <
                    stack->stack[0]->data.sdata.time_stamp ) {
                    least = cinfo;
                }
            }
        }
    }
    if( least != NULL ) {
        if( doTossUndo( least->UndoUndoStack ) ) {
            return( true );
        }
        return( doTossUndo( least->UndoStack ) );
    }
    return( false );

} /* TossUndos */

/*
 * FreeAllUndos - release contents of all undo stacks
 */
void FreeAllUndos( void )
{
    PurgeUndoStack( UndoStack );
    PurgeUndoStack( UndoUndoStack );

} /* FreeAllUndos */

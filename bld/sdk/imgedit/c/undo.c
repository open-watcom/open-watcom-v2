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


#include "imgedit.h"
#include "undo.h"
#include "iemem.h"

static an_undo_stack    *firstStack;
static an_undo_stack    *lastStack;

/*
 * setBaseImage - set the base image (for when no undos are left)
 */
static void setBaseImage( img_node *node, an_undo_stack *stack )
{
    stack->firstxor = DuplicateBitmap( node->hxorbitmap );
    stack->firstand = DuplicateBitmap( node->handbitmap );

} /* setBaseImage */

/*
 * moveToUndo - move the top of the redo stack to the top of the undo stack
 */
static void moveToUndo( an_undo_stack *stack )
{
    an_undo_node        *new_redo_top;

    if( stack->top_redo == NULL ) {
        return;
    }

    new_redo_top = stack->top_redo->next;
    if( new_redo_top != NULL ) {
        new_redo_top->previous = NULL;
    }
    stack->top_redo->next = stack->top_undo;

    if( stack->top_undo != NULL ) {
        stack->top_undo->previous = stack->top_redo;
    } else {
        stack->bottom_undo = stack->top_redo;
    }
    stack->top_undo = stack->top_redo;
    stack->top_redo = new_redo_top;

} /* moveToUndo */

/*
 * copyBitmaps - copy the XOR and the AND bitmaps
 */
static void copyBitmaps( img_node *node, HBITMAP xorbitmap, HBITMAP andbitmap )
{
    WPI_PRES            pres;
    HDC                 destdc;
    WPI_PRES            destpres;
    HDC                 srcdc;
    WPI_PRES            srcpres;
    HBITMAP             oldsrc;
    HBITMAP             olddest;

    pres = _wpi_getpres( node->viewhwnd );
    destpres = _wpi_createcompatiblepres( pres, Instance, &destdc );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    _wpi_releasepres( node->viewhwnd, pres );

    oldsrc = _wpi_selectbitmap( srcpres, xorbitmap );
    olddest = _wpi_selectbitmap( destpres, node->hxorbitmap );
    _wpi_bitblt( destpres, 0, 0, node->width, node->height, srcpres, 0, 0, SRCCOPY );
    _wpi_getoldbitmap( srcpres, oldsrc );
    _wpi_getoldbitmap( destpres, olddest );

    oldsrc = _wpi_selectbitmap( srcpres, andbitmap );
    olddest = _wpi_selectbitmap( destpres, node->handbitmap );
    _wpi_bitblt( destpres, 0, 0, node->width, node->height, srcpres, 0, 0, SRCCOPY );
    _wpi_getoldbitmap( srcpres, oldsrc );
    _wpi_getoldbitmap( destpres, olddest );
    _wpi_deletecompatiblepres( srcpres, oldsrc );
    _wpi_deletecompatiblepres( destpres, olddest );

} /* copyBitmaps */

/*
 * deleteRedoStack - delete the redo stack
 */
static void deleteRedoStack( an_undo_stack *stack )
{
    an_undo_node        *new_top;

    while( stack->top_redo != NULL ) {
        new_top = stack->top_redo->next;
        stack->top_redo->next = NULL;
        _wpi_deletebitmap( stack->top_redo->xorbitmap );
        _wpi_deletebitmap( stack->top_redo->andbitmap );
        MemFree( stack->top_redo );
        stack->top_redo = new_top;
        stack->redocount = 0;
    }

} /* deleteRedoStack */

/*
 * addToRedo - add a node to the redo stack
 */
static void addToRedo( an_undo_stack *stack )
{
    if( stack == NULL ) {
        return;
    }
    if( stack->top_redo != NULL ) {
        stack->top_undo->next = stack->top_redo;
        stack->top_redo->previous = stack->top_undo;
        stack->top_redo = stack->top_undo;
    } else {
        stack->top_redo = stack->top_undo;
        stack->top_redo->next = NULL;
    }

} /* addToRedo */

/*
 * getTopStack - retrieve the top stack (of potentially many icons) from
 *               the icon list
 */
static an_undo_stack *getTopStack( HWND hwnd )
{
    an_undo_stack       *stack;

    stack = firstStack;
    while( stack != NULL ) {
        if( stack->hwnd == hwnd ) {
            return( stack );
        }
        stack = stack->next;
    }
    return( NULL );

} /* getTopStack */

/*
 * getStack - return the stack corresponding to the window handle
 */
static an_undo_stack *getStack( HWND hwnd )
{
    an_undo_stack       *stack;
    int                 i;

    stack = getTopStack( hwnd );
    if( stack == NULL ) {
        return( NULL );
    }

    for( i = 0; i < stack->current_icon; i++ ) {
        if( stack == NULL ) {
            break;
        }
        stack = stack->nexticon;
    }
    return( stack );

} /* getStack */

/*
 * checkIfSaved - check if the image is saved or not (by the number of
 *                undo's on the undo stack).
 */
static void checkIfSaved( img_node *node )
{
    BOOL                issaved_flag;
    an_undo_stack       *stack;

    stack = getTopStack( node->hwnd );
    if( stack == NULL ) {
        return;
    }

    /*
     * If it is currently saved, and we undo an operation, then it becomes
     * unsaved.  From then on, it can only become saved by explicitly saving
     * it.
     */
    if( node->issaved ) {
        SetIsSaved( node->hwnd, FALSE );
        while( stack != NULL ) {
            stack->modified = TRUE;
            stack = stack->nexticon;
        }
        return;
    }

    issaved_flag = TRUE;
    while( stack != NULL ) {
        if( stack->opcount == 0 && !stack->modified ) {
            issaved_flag = TRUE & issaved_flag;
        } else {
            issaved_flag = FALSE;
            if( node->issaved ) {
                SetIsSaved( node->hwnd, FALSE );
            }
            break;
        }
        stack = stack->nexticon;
    }
    SetIsSaved( node->hwnd, issaved_flag );

} /* checkIfSaved */

/*
 * deleteFromBottom - delete an element from the bottom of the stack (since
 *                    the stack is a fixed size)
 */
static void deleteFromBottom( an_undo_stack *stack )
{
    an_undo_node        *temp;

    if( stack == NULL ) {
        return;
    }
    if( stack->bottom_undo == NULL ) {
        return;
    }

    temp = stack->bottom_undo->previous;
    _wpi_deletebitmap( stack->firstxor );
    _wpi_deletebitmap( stack->firstand );
    stack->firstxor = stack->bottom_undo->xorbitmap;
    stack->firstand = stack->bottom_undo->andbitmap;
    stack->bottom_undo->previous = NULL;
    MemFree( stack->bottom_undo );

    stack->bottom_undo = temp;
    if( temp != NULL ) {
        stack->bottom_undo->next = NULL;
    }

} /* deleteFromBottom */

/*
 * moveFromTop - delete from the top of the stack (when undo is selected)
 *             - if this call is a result of an UndoOp, the XOR and AND
 *               bitmaps are the new bitmaps for the active image, so we
 *               don't want to delete them
 */
static void moveFromTop( an_undo_stack *stack, BOOL add_to_redo )
{
    an_undo_node        *new_undo;

    if( stack->top_undo == NULL ) {
        return;
    }

    new_undo = stack->top_undo->next;
    if( new_undo != NULL ) {
        new_undo->previous = NULL;
    } else {
        stack->bottom_undo = NULL;
    }

    if( !add_to_redo ) {
        _wpi_deletebitmap( stack->top_undo->xorbitmap );
        _wpi_deletebitmap( stack->top_undo->andbitmap );
        stack->top_undo->next = NULL;
        MemFree( stack->top_undo );
    } else {
        addToRedo( stack );
    }

    stack->top_undo = new_undo;

} /* moveFromTop */

/*
 * RecordImage - after something is drawn, the operation is recorded so
 *               that it can be undone if desired
 */
void RecordImage( HWND hwnd )
{
    an_undo_node        *undo_node;
    an_undo_stack       *stack;
    img_node            *image_node;
    long                bytes;
    DWORD               free_space;
    DWORD               space_needed;

    stack = getStack( hwnd );
    if( stack == NULL ) {
        return;
    }

    deleteRedoStack( stack );

#ifndef __OS2_PM__
    free_space = GetFreeSpace( 0 );
#else
    free_space = 0;
#endif
    image_node = SelectImage( hwnd );
    if( image_node == NULL ) {
        return;
    }

    bytes = (image_node->width * image_node->bitcount + 31) / 32;
    bytes = bytes * 4 * image_node->height;
    /*
     * Here's a bit of a problem:  I'm not sure how much Windows is going
     * to need to keep running relatively happily!
     */
    space_needed = SPACE_LIMIT * bytes;
    if( space_needed < SPACE_MIN )
        space_needed = SPACE_MIN;

#ifndef __OS2_PM__
#ifndef __NT__
    if( free_space < space_needed ) {
        if( !RelieveUndos() ) {
            /*
             * No image is recorded if undo's could not be relieved.
             */
            return;
        }
    }
#endif
#endif

    undo_node = MemAlloc( sizeof( an_undo_node ) );
    if( undo_node == NULL ) {
        RelieveUndos();
        undo_node = MemAlloc( sizeof( an_undo_node ) );
        if( undo_node == NULL ) {
            return;
        }
    }
    undo_node->previous = NULL;
    undo_node->xorbitmap = DuplicateBitmap( image_node->hxorbitmap );
    undo_node->andbitmap = DuplicateBitmap( image_node->handbitmap );

    if( stack->opcount == 0 ) {
        undo_node->next = NULL;
        stack->top_undo = undo_node;
        stack->bottom_undo = undo_node;
    } else {
        undo_node->next = stack->top_undo;
        stack->top_undo->previous = undo_node;
        stack->top_undo = undo_node;
    }

    stack->opcount++;

    if( image_node->issaved ) {
        SetIsSaved( image_node->hwnd, FALSE );
    }

} /* RecordImage */

/*
 * UndoOp - undo an operation
 */
void UndoOp( void )
{
    img_node            *node;
    an_undo_stack       *stack;

    node = GetCurrentNode();

    if( node == NULL ) {
        return;
    }

    stack = getStack( node->hwnd );
    if( stack == NULL ) {
        return;
    }

    if( stack->opcount <= 0 ) {
        PrintHintTextByID( WIE_UNDOSTACKEMPTY, NULL );
        return;
    }

    moveFromTop( stack, TRUE );
    if( stack->top_undo == NULL ) {
        copyBitmaps( node, stack->firstxor, stack->firstand );
    } else {
        copyBitmaps( node, stack->top_undo->xorbitmap, stack->top_undo->andbitmap );
    }
    stack->opcount--;
    stack->redocount++;

    IEPrintAmtText( WIE_UNDOITEMSLEFT, stack->opcount );

    InvalidateRect( node->viewhwnd, NULL, TRUE );
    BlowupImage( node->hwnd, NULL );

    checkIfSaved( node );

} /* UndoOp */

/*
 * CheckForUndo - check if the undo menu options should be enabled or not
 */
void CheckForUndo( img_node *node )
{
    an_undo_stack       *stack;
    HMENU               hmenu;

    if( HMainWindow != NULL ) {
        hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );
    } else {
        return;
    }

    if( node == NULL ) {
        _wpi_enablemenuitem( hmenu, IMGED_UNDO, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_REDO, FALSE, FALSE );
        return;
    }

    stack = getStack( node->hwnd );
    if( stack == NULL ) {
        _wpi_enablemenuitem( hmenu, IMGED_UNDO, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_REDO, FALSE, FALSE );
        return;
    }

    if( stack->opcount <= 0 ) {
        _wpi_enablemenuitem( hmenu, IMGED_UNDO, FALSE, FALSE );
    } else {
        _wpi_enablemenuitem( hmenu, IMGED_UNDO, TRUE, FALSE );
    }

    if( stack->top_redo != NULL ) {
        _wpi_enablemenuitem( hmenu, IMGED_REDO, TRUE, FALSE );
    } else {
        _wpi_enablemenuitem( hmenu, IMGED_REDO, FALSE, FALSE );
    }

    if( stack->original_xor != NULL ) {
        _wpi_enablemenuitem( hmenu, IMGED_REST, TRUE, FALSE );
    } else {
        _wpi_enablemenuitem( hmenu, IMGED_REST, FALSE, FALSE );
    }

} /* CheckForUndo */

/*
 * CreateUndoStack - create a new undo stack (when a new image is opened or begun)
 */
void CreateUndoStack( img_node *node )
{
    an_undo_stack       *new_stack;
    an_undo_stack       *next_stack;
    an_undo_stack       *prev_stack;
    img_node            *nexticon;
    int                 icon_count;
    int                 i;

    new_stack = MemAlloc( sizeof( an_undo_stack ) );
    new_stack->hwnd = node->hwnd;
    new_stack->opcount = 0;
    new_stack->redocount = 0;
    new_stack->current_icon = 0;
    new_stack->top_undo = NULL;
    new_stack->bottom_undo = NULL;
    new_stack->top_redo = NULL;
    new_stack->modified = FALSE;
    new_stack->nexticon = NULL;
    new_stack->next = NULL;
    new_stack->previous = NULL;

    if( node->issaved ) {
        new_stack->original_xor = DuplicateBitmap( node->hxorbitmap );
        new_stack->original_and = DuplicateBitmap( node->handbitmap );
    } else {
        new_stack->original_xor = NULL;
        new_stack->original_and = NULL;
    }
    setBaseImage( node, new_stack );

    icon_count = node->num_of_images;
    nexticon = node->nexticon;
    prev_stack = new_stack;
    for( i = 1; i < icon_count; i++ ) {
        if( nexticon == NULL ) {
            break;
        }
        next_stack = MemAlloc( sizeof( an_undo_stack ) );
        memcpy( next_stack, prev_stack, sizeof( an_undo_stack ) );
        /*
         * If node->issaved is true then this was an opened image and we store
         * the original bitmaps.  Otherwise, the 'Restore' option is grayed.
         */
        if( nexticon->issaved ) {
            next_stack->original_xor = DuplicateBitmap( nexticon->hxorbitmap );
            next_stack->original_and = DuplicateBitmap( nexticon->handbitmap );
        } else {
            next_stack->original_xor = NULL;
            next_stack->original_and = NULL;
        }
        setBaseImage( nexticon, next_stack );

        nexticon = nexticon->nexticon;
        prev_stack->nexticon = next_stack;
        prev_stack = next_stack;
    }

    if( lastStack == NULL ) {
        new_stack->next = NULL;
        new_stack->previous = NULL;
        firstStack = new_stack;
        lastStack = new_stack;
    } else {
        new_stack->next = NULL;
        new_stack->previous = lastStack;
        lastStack->next = new_stack;
        lastStack = new_stack;
    }

} /* CreateUndoStack */

/*
 * DeleteUndoStack - delete an undo stack when we close an image
 */
void DeleteUndoStack( HWND hwnd )
{
    an_undo_stack       *stack;
    an_undo_stack       *previous_stack;
    an_undo_stack       *next_stack;
    an_undo_stack       *nexticon_stack;
    HCURSOR             prevcursor;
    int                 i;
    int                 max_ops;
    int                 percent_complete;

    stack = getTopStack( hwnd );
    if( stack == NULL ) {
        return;
    }

    prevcursor = _wpi_setcursor( _wpi_getsyscursor( IDC_WAIT ) );
    previous_stack = stack->previous;
    next_stack = stack->next;

    while( stack != NULL ) {
        deleteRedoStack( stack );

        max_ops = stack->opcount;
        i = 0;
        while( stack->top_undo != NULL ) {
            moveFromTop( stack, FALSE );
            stack->opcount--;
            percent_complete = (i * 100) / max_ops;
            IEPrintAmtText( WIE_CLOSINGIMAGEPERCENT, percent_complete );
            i++;
        }

        _wpi_deletebitmap( stack->firstxor );
        _wpi_deletebitmap( stack->firstand );
        if( stack->original_xor != NULL ) {
            _wpi_deletebitmap( stack->original_xor );
            _wpi_deletebitmap( stack->original_and );
        }
        nexticon_stack = stack->nexticon;
        MemFree( stack );
        stack = nexticon_stack;
    }

    if( previous_stack != NULL ) {
        previous_stack->next = next_stack;
    } else {
        firstStack = next_stack;
    }

    if( next_stack != NULL ) {
        next_stack->previous = previous_stack;
    } else {
        lastStack = previous_stack;
    }
    _wpi_setcursor( prevcursor );

} /* DeleteUndoStack */

/*
 * RedoOp - redo an operation
 */
void RedoOp( void )
{
    img_node            *node;
    an_undo_stack       *stack;

    node = GetCurrentNode();

    if( node == NULL ) {
        return;
    }

    stack = getStack( node->hwnd );
    if( stack == NULL ) {
        return;
    }

    if( stack->top_redo == NULL ) {
        PrintHintTextByID( WIE_REDOSTACKEMPTY, NULL );
        stack->redocount = 0;
        return;
    }

    copyBitmaps( node, stack->top_redo->xorbitmap, stack->top_redo->andbitmap );
    moveToUndo( stack );
    stack->opcount++;
    stack->redocount--;

    IEPrintAmtText( WIE_REDOITEMSLEFT, stack->redocount );

    InvalidateRect( node->viewhwnd, NULL, TRUE );
    BlowupImage( node->hwnd, NULL );

    if( stack->original_xor != NULL && node->issaved ) {
        SetIsSaved( node->hwnd, FALSE );
    }

} /* RedoOp */

/*
 * ResetUndoStack - resets the undo stack when the image size is changed
 */
void ResetUndoStack( img_node *node )
{
    an_undo_stack       *stack;

    stack = getStack( node->hwnd );

    deleteRedoStack( stack );

    while( stack->top_undo != NULL ) {
        moveFromTop( stack, FALSE );
        stack->opcount--;
    }

    _wpi_deletebitmap( stack->firstxor );
    _wpi_deletebitmap( stack->firstand );

    stack->firstxor = DuplicateBitmap( node->hxorbitmap );
    stack->firstand = DuplicateBitmap( node->handbitmap );

} /* ResetUndoStack */

/*
 * RestoreImage - restore current image (set to how it was when it was opened)
 *              - not a valid operation for a new bitmap
 */
void RestoreImage( void )
{
    img_node            *node;
    an_undo_stack       *stack;

    node = GetCurrentNode();

    if( node == NULL ) {
        return;
    }

    stack = getStack( node->hwnd );
    if( stack == NULL ) {
        return;
    }

    if( stack->original_xor == NULL ) {
        return;
    }

    copyBitmaps( node, stack->original_xor, stack->original_and );

    PrintHintTextByID( WIE_IMAGERESTORED, NULL );

    InvalidateRect( node->viewhwnd, NULL, TRUE );
    BlowupImage( node->hwnd, NULL );

    RecordImage( node->hwnd );
    if( node->imgtype == BITMAP_IMG || node->imgtype == CURSOR_IMG ) {
        SetIsSaved( node->hwnd, TRUE );
    } else {
        checkIfSaved( node );
    }

} /* RestoreImage */

/*
 * AllowRestoreOption - if a file is originally new, then the user saves the
 *                      file, we now allow the user to select the restore
 *                      option if they want
 */
void AllowRestoreOption( img_node *node )
{
    an_undo_stack       *stack;

    if( node == NULL ) {
        return;
    }

    stack = getStack( node->hwnd );
    if( stack == NULL ) {
        return;
    }

    if( stack->original_xor != NULL ) {
        _wpi_deletebitmap( stack->original_xor );
        _wpi_deletebitmap( stack->original_and );
    }
    stack->original_xor = DuplicateBitmap( node->hxorbitmap );
    stack->original_and = DuplicateBitmap( node->handbitmap );

} /* AllowRestoreOption */

/*
 * SelIconUndoStack - selecting a new icon means we must change the undo stack
 *                  - do this by just setting the current_icon field to the given index
 */
void SelIconUndoStack( HWND hwnd, short index )
{
    an_undo_stack       *stack;

    stack = getTopStack( hwnd );

    while( stack != NULL ) {
        stack->current_icon = index;
        stack = stack->nexticon;
    }

} /* SelIconUndoStack */

/*
 * AddIconUndoStack - when we add an icon to the current icon image, we also
 *                    have to add an undo stack for that icon
 *                  - set the issaved flag to false
 */
void AddIconUndoStack( img_node *node )
{
    an_undo_stack       *stack;
    an_undo_stack       *new_stack;

    stack = getTopStack( node->hwnd );
    if( stack == NULL ) {
        return;
    }

    while( stack->nexticon != NULL ) {
        stack = stack->nexticon;
    }

    new_stack = MemAlloc( sizeof( an_undo_stack ) );
    new_stack->hwnd = node->hwnd;
    new_stack->opcount = 0;
    new_stack->redocount = 0;

    /*
     * I don't change the current icon because select icon is called right
     * after this so that will set the new current_icon value.
     */
    new_stack->current_icon = 0;
    new_stack->top_undo = NULL;
    new_stack->bottom_undo = NULL;
    new_stack->top_redo = NULL;
    new_stack->modified = FALSE;
    new_stack->nexticon = NULL;
    new_stack->original_xor = NULL;
    new_stack->original_and = NULL;
    setBaseImage( node, new_stack );

    stack->nexticon = new_stack;

    /*
     * We use the modified field to indicate when the image has FOR SURE
     * been modified.  Once this is set, the issaved flag cannot be set to
     * true. (as in the case when an icon is deleted from the current image)
     */
    SetIsSaved( node->hwnd, FALSE );
    stack = getTopStack( node->hwnd );
    while( stack != NULL ) {
        stack->modified = TRUE;
        stack = stack->nexticon;
    }

} /* AddIconUndoStack */

/*
 * DelIconUndoStack - delete the undo stack associated with icon image being deleted
 */
void DelIconUndoStack( img_node *node, int index )
{
    an_undo_stack       *stack;
    an_undo_stack       *prev_icon;
    an_undo_stack       *next_icon;
    an_undo_stack       *prev_stack;
    an_undo_stack       *next_stack;
    int                 i;

    stack = getTopStack( node->hwnd );
    if( stack == NULL ) {
        return;
    }

    prev_stack = stack->previous;
    next_stack = stack->next;

    prev_icon = NULL;
    next_icon = stack->nexticon;

    for( i = 0; i < index; i++ ) {
        if( stack == NULL ) {
            break;
        }
        prev_icon = stack;
        stack = stack->nexticon;
        next_icon = stack->nexticon;
    }

    if( prev_icon == NULL ) {
        if( prev_stack != NULL ) {
            prev_stack->next = next_icon;
        }
        if( next_icon != NULL ) {
            next_icon->next = next_stack;
            next_icon->previous = prev_stack;
        }
    } else {
        prev_icon->nexticon = next_icon;
    }

    stack->nexticon = NULL;
    stack->previous = NULL;
    stack->next = NULL;
    deleteRedoStack( stack );

    while( stack->top_undo != NULL ) {
        moveFromTop( stack, FALSE );
        stack->opcount--;
    }

    _wpi_deletebitmap( stack->firstxor );
    _wpi_deletebitmap( stack->firstand );
    if( stack->original_xor != NULL ) {
        _wpi_deletebitmap( stack->original_xor );
        _wpi_deletebitmap( stack->original_and );
    }
    MemFree( stack );

    /*
     * We use the modified field to indicate when the image has FOR SURE
     * been modified.  Once this is set, the issaved flag cannot be set to
     * true (as in the case when an icon is deleted from the current image).
     */
    SetIsSaved( node->hwnd, FALSE );
    stack = getTopStack( node->hwnd );
    while( stack != NULL ) {
        stack->modified = TRUE;
        stack = stack->nexticon;
    }

} /* DelIconUndoStack */

/*
 * RelieveUndos - called when the main window (i.e. client window) gets the
 *                WM_COMPACTING message, indicating that memory is getting low
 *              - remove elements from the largest undo stack
 *              - also called when the record image routine realizes that memory
 *                is getting low
 */
BOOL RelieveUndos( void )
{
    an_undo_stack       *stack;
    an_undo_stack       *iconstack;
    an_undo_stack       *delstack;
    int                 max_ops = 0;
    HCURSOR             prevcursor;
    int                 i;

    PrintHintTextByID( WIE_DISCARDINGUNDOS, NULL );
    prevcursor = _wpi_setcursor( _wpi_getsyscursor( IDC_WAIT ) );
    stack = firstStack;
    while( stack != NULL ) {
        iconstack = stack;
        while( iconstack != NULL ) {
            if( iconstack->opcount > max_ops ) {
                max_ops = iconstack->opcount;
                delstack = iconstack;
            }
            iconstack = iconstack->nexticon;
        }
        stack = stack->next;
    }

    if( max_ops == 0 ) {
        PrintHintTextByID( WIE_NOUNDOSRECORDED, NULL );
        _wpi_setcursor( prevcursor );
        return( FALSE );
    }
    if( max_ops > 5 ) {
        for( i = 0; i < 5; i++ ) {
            deleteFromBottom( delstack );
            delstack->opcount--;
        }
    } else {
        deleteFromBottom( delstack );
        delstack->opcount--;
    }
    _wpi_setcursor( prevcursor );
    return( TRUE );

} /* RelieveUndos */

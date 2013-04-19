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
#include "iconinfo.h"
#include "iemem.h"

typedef struct list_el {
    int                 index;
    struct list_el      *next;
} index_table;

static img_node         *imgHead = NULL;
static img_node         *imgTail = NULL;
static index_table      *indexHead = NULL;
static index_table      *indexTail = NULL;

/*
 * deleteNodeData - delete the bitmaps, check for multiple icons, and free memory.
 */
static void deleteNodeData( img_node *node )
{
    img_node    *next_icon;

    node->next = NULL;
    next_icon = node->nexticon;

    while( node != NULL ) {
        _wpi_deletebitmap( node->hxorbitmap );
        _wpi_deletebitmap( node->handbitmap );
        node->nexticon = NULL;
        node->hwnd = NULL;
        node->viewhwnd = NULL;
        node->num_of_images = 0;
        if( node->wrinfo != NULL ) {
            WRFreeWRInfo( node->wrinfo );
        }
        MemFree( node );

        node = next_icon;
        if( node != NULL ) {
            next_icon = node->nexticon;
        }
    }

} /* deleteNodeData */

/*
 * removeIcon - remove the index(th) icon from the list
 */
static img_node *removeIcon( img_node *node, short index )
{
    img_node    *delnode;
    img_node    *newnode;
    img_node    *prevnode;
    short       i;

    if( index == 0 ) {
        delnode = node;
        prevnode = NULL;
        newnode = delnode->nexticon;
    } else {
        prevnode = node;
        for( i = 0; i < index - 1; i++ ) {
            prevnode = prevnode->nexticon;
        }
        delnode = prevnode->nexticon;
        prevnode->nexticon = delnode->nexticon;
        newnode = node;
    }

    delnode->nexticon = NULL;
    deleteNodeData( delnode );
    delnode = NULL;

    return( newnode );

} /* removeIcon */

/*
 * findPreviousNode - return a pointer to the node whose 'next' field is
 *                    is a pointer to the given node
 */
static img_node *findPreviousNode( img_node *node )
{
    img_node    *prevnode;

    if( imgHead == node ) {
        return( NULL );
    }

    prevnode = imgHead;

    while( prevnode->next != node ) {
        prevnode = prevnode->next;
    }

    return( prevnode );

} /* findPreviousNode */

/*
 * AddImageNode - add a node to the linked list
 */
void AddImageNode( img_node *node )
{
    img_node    *new_node;
    img_node    *prevnode;
    img_node    *next_icon;
    img_node    *next_in_src;
    index_table *new_index;
    int         imagecount;
    int         i;

    imagecount = node->num_of_images;

    new_node = MemAlloc( sizeof( img_node ) );
    memcpy( new_node, node, sizeof( img_node ) );
    new_node->next = NULL;
    prevnode = new_node;
    next_in_src = node->nexticon;

    for( i = 1; i < imagecount; i++ ) {
        next_icon = MemAlloc( sizeof( img_node ) );
        memcpy( next_icon, next_in_src, sizeof( img_node ) );
        prevnode->nexticon = next_icon;
        prevnode = next_icon;
        next_in_src = next_in_src->nexticon;
    }
    prevnode->nexticon = NULL;

    new_index = MemAlloc( sizeof( index_table ) );

    new_index->index = 0;
    new_index->next = NULL;

    if( imgHead == NULL ) {
        /*
         * Perform first time stuff ...
         */
        imgHead = new_node;
        imgTail = new_node;
        indexHead = new_index;
        indexTail = new_index;
    } else {
        imgTail->next = new_node;
        imgTail = new_node;

        indexTail->next = new_index;
        indexTail = new_index;
    }

} /* AddImageNode */

/*
 * SelectImage - this just uses a linear search - it's easy and since we
 *               aren't working with really large amounts of data, we can
 *               afford the time pretty easily
 *             - for icons, return the current icon being edited (not the root icon
 *               in the icon file)
 */
img_node *SelectImage( HWND hwnd )
{
    img_node    *current_node;
    index_table *tableptr;
    short       i;

    current_node = imgHead;
    tableptr = indexHead;

    while( current_node != NULL ) {
        if( current_node->hwnd == hwnd ) {
            for( i = 0; i < tableptr->index; i++ ) {
                current_node = current_node->nexticon;
            }
            return( current_node );
        }
        current_node = current_node->next;
        tableptr = tableptr->next;
    }

    return( NULL );

} /* SelectImage */

/*
 * DeleteNode - delete the node (frees memory) corresponding to the hwnd given
 */
BOOL DeleteNode( HWND hwnd )
{
    img_node    *node;
    img_node    *delnode;
    index_table *delindex;
    index_table *index;

    node = imgHead;
    index = indexHead;

    /*
     * First check the head and from then on check node->next ...
     */
    if( imgHead->hwnd == hwnd ) {
        if( imgHead == imgTail ) {
            imgTail = NULL;
            imgHead = NULL;
        } else {
            imgHead = imgHead->next;
        }
        deleteNodeData( node );
        node = NULL;

        if( indexHead == indexTail ) {
            indexTail = NULL;
            indexTail = NULL;
        } else {
            indexHead = indexHead->next;
        }
        MemFree( index );
        return( TRUE );
    }

    while( node->next != NULL ) {
        if( node->next->hwnd == hwnd ) {
            delnode = node->next;
            if( delnode == imgTail ) {
                imgTail = node;
            }
            node->next = delnode->next;
            deleteNodeData( delnode );
            delnode = NULL;

            delindex = index->next;
            if( delindex == indexTail ) {
                indexTail = index;
            }
            index->next = delindex->next;
            MemFree( delindex );

            return( TRUE );
        }
        node = node->next;
        index = index->next;
    }

    return( FALSE );

} /* DeleteNode */

/*
 * DeleteList - delete both the index table and the linked list of image data
 */
void DeleteList( void )
{
    img_node    *node;
    index_table *index;

    if( !DoImagesExist ) {
        return;
    }

    while( imgHead != NULL ) {
        node = imgHead;
        imgHead = node->next;
        deleteNodeData( node );
        node = NULL;

        index = indexHead;
        indexHead = index->next;
        MemFree( index );
    }

    imgTail = NULL;

} /* DeleteList */

/*
 * DoImagesExist - check to see if there are any opened/new images in the
 *                 linked list (checks to see if list is empty)
 *               - return the number of images in the list
 */
int DoImagesExist( void )
{
    int         img_count = 1;
    img_node    *node;

    if( imgHead == NULL ) {
        return( FALSE );
    }

    node = imgHead;
    while( node != imgTail ) {
        img_count++;
        node = node->next;
    }
    return( img_count );

} /* DoImagesExist */

/*
 * GetHeadNode - return the head of the linked list
 */
img_node *GetHeadNode( void )
{
    return( imgHead );

} /* GetHeadNode */

/*
 * GetNthIcon - get the nth (index th) icon from the linked list
 */
img_node *GetNthIcon( HWND hwnd, short index )
{
    img_node            *node;
    index_table         *tableptr;
    int                 i;

    node = imgHead;
    tableptr = indexHead;

    while( node != NULL ) {
        if( node->hwnd == hwnd ) {
            break;
        }
        node = node->next;
        tableptr = tableptr->next;
    }

    if( node == NULL ) {
        WImgEditError( WIE_ERR_BAD_HWND, WIE_INTERNAL_006 );
        return( NULL );
    }

    if( index > node->num_of_images ) {
        WImgEditError( WIE_ERR_BAD_ICONINDEX, WIE_INTERNAL_007 );
        return( NULL );
    }

    for( i = 1; i <= index; i++ ) {
        if( node->nexticon != NULL ) {
            node = node->nexticon;
        } else {
            break;
        }
    }

    tableptr->index = min( i, index );
    return( node );

} /* GetNthIcon */

/*
 * GetImageNode - get the first node (i.e. the root of the list of icons) in
 *                the linked list corresponding the the given window handle
 */
img_node *GetImageNode( HWND hwnd )
{
    img_node            *node;

    node = imgHead;
    while( node != NULL ) {
        if( node->hwnd == hwnd ) {
            return( node );
        }
        node = node->next;
    }

    return( NULL );

} /* GetImageNode */

/*
 * AddIconToList - an icon can be a number of different images in one
 *               - add an icon image to the current icon in the linked list
 */
void AddIconToList( img_node *icon, img_node *current_node )
{
    img_node            *temp;
    img_node            *new_icon;

    temp = current_node;

    new_icon = MemAlloc( sizeof( img_node ) );
    memcpy( new_icon, icon, sizeof( img_node ) );

    while( temp != NULL ) {
        if( temp->nexticon == NULL ) {
            temp->nexticon = new_icon;
            break;
        }
        temp = temp->nexticon;
    }

} /* AddIconToList */

/*
 * RemoveIconFromList - remove an icon image from the current icon file
 *                    - after calling this routine, the 'node' variable
 *                      should no longer be used by the calling routine
 */
img_node *RemoveIconFromList( img_node *node, int index )
{
    img_node            *prevnode;
    img_node            *nextimage;
    img_node            *newnodelist;

    nextimage = node->next;
    prevnode = findPreviousNode( node );
    newnodelist = removeIcon( node, index );

    if( prevnode == NULL ) {
        imgHead = newnodelist;
    } else {
        prevnode->next = newnodelist;
    }
    newnodelist->next = nextimage;

    if( nextimage == NULL ) {
        imgTail = newnodelist;
    }
    return( newnodelist );

} /* RemoveIconFromList */

/*
 * SelectFromViewHwnd - given the handle to the view window, this returns
 *                      the node corresponding to it
 */
img_node *SelectFromViewHwnd( HWND viewhwnd )
{
    img_node    *current_node;
    index_table *tableptr;
    short       i;

    current_node = imgHead;
    tableptr = indexHead;

    while( current_node != NULL ) {
        if( current_node->viewhwnd == viewhwnd ) {
            for( i = 0; i < tableptr->index; i++ ) {
                current_node = current_node->nexticon;
            }
            return( current_node );
        }
        current_node = current_node->next;
        tableptr = tableptr->next;
    }
    return( NULL );

} /* SelectFromViewHwnd */

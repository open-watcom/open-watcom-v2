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
* Description:  Implements the functions declared in heapchk.h:
*                   end_heapcheck()
*                   display_heap()
*                   start_heapcheck()
*                   null_buffer()
*               and this local function:
*                   do_heapcheck()
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__ 1

#include <malloc.h>
#include <stdio.h>

#include "heapchk.h"

/* Local data definition. */

static _HEAPINFO save_node;

/* Local function definitions. */

static _HEAPINFO do_heapcheck( char * location )
{
    char    *   block_status    = NULL;
    int         heap_status;
    _HEAPINFO   heap_node;
    _HEAPINFO   return_node;

    printf_s( "Walking the Heap: location: %s\n", location );
    heap_node._pentry = NULL;
    for(;;)
    {
        heap_status = _heapwalk( &heap_node );
        if( heap_status != _HEAPOK ) break;

        if( heap_node._useflag == _USEDENTRY ) block_status = "USED";
        else block_status = "FREE";

        printf_s( "%s block at 0x%llx of size 0x%x\n", block_status, \
                                            heap_node._pentry, heap_node._size );
        return_node._pentry = heap_node._pentry;
        return_node._size = heap_node._size;
        return_node._useflag = heap_node._useflag;
    }

    puts( "Final heap status:" );

    switch(heap_status)
    {
      case _HEAPEND :
        puts( "  _HEAPEND: OK - end of heap" );
        break;
      case _HEAPEMPTY :
        puts( "  _HEAPEMPTY: OK - heap is empty" );
        break;
      case _HEAPBADBEGIN :
        puts( "  _HEAPBADBEGIN: heap is damaged" );
        break;
      case _HEAPBADPTR :
        puts( "  _HEAPBADPTR: bad pointer to heap" );
        break;
      case _HEAPBADNODE :
        puts( "  _HEAPBADNODE: bad node in heap" );
        break;
      default :
        puts( "  Unknown heap status value" );
        break;
    }

    return ( return_node );
};


/* Extern function definitions. */

void    end_heapcheck( char * location )
{
    _HEAPINFO   heap_node;

    heap_node = do_heapcheck( location );

    if( heap_node._pentry != save_node._pentry ) \
                                    puts( "NOTE: Heap changed from saved value");
    return;
};

void    display_heap( char * location )
{
    do_heapcheck( location );
    return;
};

void    start_heapcheck( char * location )
{
    save_node = do_heapcheck( location );
    return;
};

void null_buffer( void )
{
    FILE * temp = NULL;

    fopen_s( &temp, ".", "rb" );
    return;
}

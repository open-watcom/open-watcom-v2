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
* Description:  Declares functions used to check the heap.
*               Does not require special compilation switches or provide
*               endless undocumented options.
*               These are the primary functions:
*                   end_heapcheck()
*                   display_heap()
*                   start_heapcheck()
*               and this is the auxiliary function:
*                   null_buffer()
*
****************************************************************************/

#ifndef HEAPCHK_H_INCLUDED
#define HEAPCHK_H_INCLUDED

/* How to Use These Functions.
 *
 * The three primary functions each have a single parameter, a character string.
 * This string will be displayed at the start of the heap dump. It is not used
 * in any other way. In particular, it is not used to associate calls to
 * end_heapcheck() with calls to start_heapcheck().
 *
 * display_heap() simply produces a heap dump. No attempt to determine if the
 * heap has changed from the last heap dump is made.
 *
 * Each invocation of start_heapcheck() preserves information about the last
 * block on the heap. Only one set of this information exists: each invocation
 * of start_heapcheck() will overwrite any prior values.
 *
 * Each invocation of end_heapcheck() compares the information describing the
 * last block on the heap with the saved information. If it is different, then
 * a message to that effect will appear after the heap dump. The saved information
 * is not changed.
 *
 * Because only one set of saved information exists, start_heapcheck() and
 * end_heapcheck() pairs will not work properly if they overlap or are nested.
 *
 * The function start_heapcheck() should be used to establish a starting
 * condition for the heap. Because the run-time uses the heap, this will usually
 * not be an empty heap even at the start of the main() function. This function
 * can be placed anywhere in the code; the location should be chosen to show the
 * heap before the heap actions to be examined take place.
 *
 * The function end_heapcheck() should be used to determine whether or not there
 * has been a net change in the heap since the start_heapcheck() was invoked. The
 * theory here is that, if there is no net change, then anything allocated on the
 * heap in the intermediate code has also been freed. A program showing no net
 * change from the start of main() to the end of main() is not leaking memory. 
 *
 * The function display_heap() should be used at any point in the program where
 * the state of the heap needs to be determined.
 *
 * Taken together, these functions can:
 *      identify regions in a program over which the heap is the same at the end
 *          as it is at the start.
 *      identify regions in a program over which the heap is not the same at the
 *          end as it is at the start.
 *      map heap usage at any number of points in the program. 
 *
 * The auxiliary function null_buffer() compensates for a minor quirk observed in
 * the Open Watcom runtime: when a file cannot be found, so that a NULL pointer is
 * returned as the value of FILE *, a buffer is allocated which cannot be
 * deallocated by the user program. In particular, fclose(), although documented to
 * close any automatically-allocated buffer associated with the FILE *, will not
 * close it when the value of the FiLE * is a NULL pointer. Although the run-time
 * allocates several buffers before main() starts, this is not one of them.
 * null_buffer() attempts to open a file (".") that is guaranteed not to exist,
 * thus causing the buffer to be allocated. If called before the first call to
 * start_heapcheck(), it ensures that this unfreeable buffer is allocated before
 * the testing begins.
 */
 
/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern void end_heapcheck( char * location );
extern void display_heap( char * location );
extern void start_heapcheck( char * location );

extern void null_buffer( void );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* HEAPCHK_H_INCLUDED */


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
* Description:  FCB allocation routines.
*
****************************************************************************/


#include "vi.h"
#include "fcbmem.h"

#if defined( USE_EMS ) || defined( USE_XMS )

#include <i86.h>

static unsigned short   *XSize;
static long             *XHandle;
static int              Xcount;
static int              XcurrMem;
#endif

/*
 * FcbAlloc - allocate an fcb and initialize it
 */
fcb *FcbAlloc( file *f )
{
    fcb *cfcb;

    /*
     * allocate fcb
     */
    cfcb = MemAlloc( FCB_SIZE );

    /*
     * thread fcb with all the others
     */
    if( FcbThreadHead == NULL ) {
        FcbThreadHead = FcbThreadTail = cfcb;
    } else {
        cfcb->thread_prev = FcbThreadTail;
        FcbThreadTail->thread_next = cfcb;
        FcbThreadTail = cfcb;
    }

    /*
     * initialize the data
     */
    cfcb->f = f;
    cfcb->non_swappable = true;
    cfcb->in_memory = true;
    cfcb->swapped = false;
    cfcb->on_display = false;
    cfcb->was_on_display = false;
#if defined( USE_XTD )
    cfcb->in_extended_memory = false;
#endif
#if defined( USE_XMS )
    cfcb->in_xms_memory = false;
#endif
#if defined( USE_EMS )
    cfcb->in_ems_memory = false;
#endif
    cfcb->nullfcb = false;
    cfcb->dead = false;
    cfcb->offset = -1L;
    FcbBlocksInUse++;

    return( cfcb );

} /* FcbAlloc */

/*
 * FcbFree - release an fcb
 */
void FcbFree( fcb *cfcb )
{
    /*
     * release swap data
     */
    if( cfcb->offset >= 0 ) {
        GiveBackSwapBlock( cfcb->offset );
    }
    if( cfcb->xmemaddr > 0 ) {
#if defined( USE_EMS )
        if( cfcb->in_ems_memory ) {
            GiveBackEMSBlock( cfcb->xmemaddr );
        }
#endif
#if defined( USE_XTD )
        if( cfcb->in_extended_memory ) {
            GiveBackXMemBlock( cfcb->xmemaddr );
        }
#endif
#if defined( USE_XMS )
        if( cfcb->in_xms_memory ) {
            GiveBackXMSBlock( cfcb->xmemaddr );
        }
#endif
    }

    if( cfcb == FcbThreadHead ) {
        FcbThreadHead = cfcb->thread_next;
    }
    if( cfcb == FcbThreadTail ) {
        FcbThreadTail = cfcb->thread_prev;
    }
    if( cfcb->thread_prev != NULL ) {
        cfcb->thread_prev->thread_next = cfcb->thread_next;
    }
    if( cfcb->thread_next != NULL ) {
        cfcb->thread_next->thread_prev = cfcb->thread_prev;
    }

    /*
     * release fcb data
     */
    MemFree( cfcb );
    FcbBlocksInUse--;

} /* FcbFree */

/*
 * FreeEntireFcb - free all data in an fcb
 */
void FreeEntireFcb( fcb *cfcb )
{
    line        *cline, *tline;

    if( cfcb->in_memory ) {
        for( cline = cfcb->lines.head; cline != NULL; cline = tline ) {
            tline = cline->next;
            MemFree( cline );
        }
    }
    FcbFree( cfcb );

} /* FreeEntireFcb */

/*
 * FreeFcbList - free up a list of fcbs
 */
void FreeFcbList( fcb *cfcb )
{
    fcb     *tfcb;

    for( ; cfcb != NULL; cfcb = tfcb ) {
        tfcb = cfcb->next;
        FreeEntireFcb( cfcb );
    }

} /* FreeFcbList */

#if defined( USE_EMS ) || defined( USE_XMS )

void XmemGiveBack( void (*rtn)( long ) )
{
    int i;

    for( i = 0; i < Xcount; i++ ) {
        rtn( XHandle[i] );
    }

} /* memGiveBack */

void XmemBlockWrite( void (*rtn)(long, void*, unsigned), __segment buff, unsigned *size )
{
    unsigned    bytes;

    if( *size >= 0x0200 ) {
        *size = 0x0200;
    }
    bytes = *size << 4;
    rtn( XHandle[XcurrMem], MK_FP( buff, 0 ), bytes );
    XSize[XcurrMem] = bytes;
    XcurrMem++;

} /* memBlockWrite */

bool XmemBlockRead( void (*rtn)(long, void*, unsigned), __segment *buff )
{
    rtn( XHandle[XcurrMem], MK_FP( *buff, 0 ), XSize[XcurrMem] );
    *buff += 0x200;
    if( XSize[XcurrMem] < MAX_IO_BUFFER ) {
        return( false );
    }
    XcurrMem++;
    return( true );

} /* memBlockRead */

void XSwapInit( int count, long *xHandle, unsigned short *xSize )
{
    Xcount = count;
    XHandle = xHandle;
    XSize = xSize;
}

void Xopen( void )
{
    XcurrMem = 0;
}

#endif

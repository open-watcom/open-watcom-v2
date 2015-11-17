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
* Description:  FCB swapping to disk/memory.
*
****************************************************************************/


#include "vi.h"
#include "fcbmem.h"

/*
 * FetchFcb - get an fcb
 */
void FetchFcb( fcb *fb )
{
    vi_rc       rc;

    if( fb == NULL ) {
        rc = ERR_INTERNAL_NULL_PTR;
    } else {
        if( fb->in_memory ) {
            return;
        }
        rc = ERR_NO_ERR;
        if( fb->swapped ) {
            rc = SwapToMemoryFromDisk( fb );
#if defined( USE_XTD )
        } else if( fb->in_extended_memory ) {
            rc = SwapToMemoryFromExtendedMemory( fb );
#endif
#if defined( USE_EMS )
        } else if( fb->in_ems_memory ) {
            rc = SwapToMemoryFromEMSMemory( fb );
#endif
#if defined( USE_XMS )
        } else if( fb->in_xms_memory ) {
            rc = SwapToMemoryFromXMSMemory( fb );
#endif
        }
    }

    if( rc != ERR_NO_ERR ) {
        AbandonHopeAllYeWhoEnterHere( rc );
    }

} /* FetchFcb */

/*
 * SwapFcb - exchange fcb from memory to disk
 */
void SwapFcb( fcb *fb )
{
    vi_rc   rc;

#if defined( USE_XTD )
    rc = SwapToExtendedMemory( fb );
    if( rc == ERR_NO_EXTENDED_MEMORY ) {
#endif
#if defined( USE_EMS )
        rc = SwapToEMSMemory( fb );
        if( rc == ERR_NO_EMS_MEMORY ) {
#endif
#if defined( USE_XMS )
            rc = SwapToXMSMemory( fb );
            if( rc == ERR_NO_XMS_MEMORY ) {
#endif
                rc = SwapToDisk( fb );
#if defined( USE_XMS )
            }
#endif
#if defined( USE_EMS )
        }
#endif
#if defined( USE_XTD )
    }
#endif
    if( rc == ERR_NO_ERR ) {
        fb->lines.head = fb->lines.tail = NULL;
        fb->in_memory = false;
    }

    if( rc != ERR_NO_ERR ) {
        AbandonHopeAllYeWhoEnterHere( rc );
    }

} /* SwapFcb */

/*
 * RestoreToNormalMemory - restore swapped data to normal memory
 */
vi_rc RestoreToNormalMemory( fcb *fb, int len )
{
    int         used, linecnt;
    char        *buff;
    line        *cline;
    char        savech;

    /*
     * remove line data from buffer that is restored
     */
    len -= (int) sizeof( linedata_t ) * (fb->end_line - fb->start_line + 1);
    buff = &ReadBuffer[len];
    savech = *buff;

    /*
     * restore buffer
     */
    CreateLinesFromBuffer( len, &fb->lines, &used, &linecnt, &(fb->byte_cnt) );

    /*
     * restore line data
     */
    *buff = savech;
    for( cline = fb->lines.head; cline != NULL; cline = cline->next ) {
        cline->u.ld_word = *(linedata_t *)buff;
        buff += sizeof( linedata_t );
    }

    /*
     * reset flags
     */
    fb->xmemaddr = 0L;
#if defined( USE_XTD )
    fb->in_extended_memory = false;
#endif
#if defined( USE_EMS )
    fb->in_ems_memory = false;
#endif
#if defined( USE_XMS )
    fb->in_xms_memory = false;
#endif
    fb->swapped = false;
    fb->in_memory = true;
    fb->last_swap = ClockTicks;

    return( ERR_NO_ERR );

} /* RestoreToNormalMemory */

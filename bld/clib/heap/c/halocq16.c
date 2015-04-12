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


#include "variety.h"
#include <malloc.h>
#include <sys/types.h>
#include <sys/seginfo.h>
#include <i86.h>
#include "crwd.h"

extern  pid_t           _my_pid;

static int only_one_bit( size_t x )
{
    if( x == 0 ) {
        return 0;
    }
    /* turns off lowest 1 bit and leaves all other bits on */
    if(( x & ( x - 1 )) != 0 ) {
        return 0;
    }
    /* only one bit was on! */
    return 1;
}

_WCRTLINK void _WCHUGE * (halloc)( long n, size_t size )
{
    short seg;
    unsigned long len;

    len = (unsigned long)n * size;
    if( len == 0 ) return( 0 );
    if( len > 65536 && ! only_one_bit( size ) ) return( 0 );
    seg = qnx_segment_huge( len );
    if( seg == -1 ) seg = 0;
    return( (void _WCHUGE *)MK_FP( seg , 0 ) );
}

_WCRTLINK void (hfree)( void _WCHUGE *ptr )
{
    unsigned            seg;
    unsigned            incr;
    struct _seginfo     info;

    if( ptr != NULL ) {
        incr = 1 << _HShift;
        seg = FP_SEG( ptr );
        for( ;; ) {
            if( qnx_segment_info( 0, _my_pid, seg, &info ) != seg ) break;
            qnx_segment_free( seg );
            if( !(info.flags & _PMF_HUGE) ) break;
            seg += incr;
        }
    }
}

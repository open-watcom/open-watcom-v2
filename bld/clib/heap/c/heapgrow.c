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
* Description:  Routines to grow heap (allocate memory from OS).
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#if defined(__QNX__)
    #include <sys/seginfo.h>
#elif defined(__LINUX__)
#elif defined(__OS2__)
    #define INCL_DOSMEMMGR
    #include <wos2.h>
    #include "tinyos2.h"
#elif defined(_M_IX86)
    #include "tinyio.h"
#endif
#include "heap.h"
#include "heapacc.h"

#if defined( _M_I86 )

_WCRTLINK void _fheapgrow( void )
{
    /* multiple heaps are used so growing the far heaps is not necessary */
}

#endif


#if defined(__SMALL_DATA__)

_WCRTLINK void _heapgrow( void )
    {
        _nheapgrow();
    }

#else

_WCRTLINK void _heapgrow( void )
    {
        _fheapgrow();
    }

#endif


_WCRTLINK void _nheapgrow( void )
    {
#if defined(__WINDOWS_286__) || defined(__386__) || defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
        _nfree( _nmalloc( 1 ) );        /* get something into the heap */
#else
        unsigned max_paras;
        unsigned curr_paras;
        unsigned diff_paras;
        unsigned expand;

        _AccessNHeap();
        /* calculate # pages which always has extra slack space (ie. 0x10) */
        curr_paras = (( _curbrk + 0x10 ) & ~0x0f ) >> 4;
        if( curr_paras == 0 ) {
            /* we're already at 64k */
            _ReleaseNHeap();
            return;
        }
#if defined(__QNX__)
        if( qnx_segment_realloc( _DGroup(), 65536L ) == -1 ) {
            _ReleaseNHeap();
            return;
        }
        max_paras = PARAS_IN_64K;
#elif defined(__OS2__)
        if( DosReallocSeg( 0, _DGroup() ) )  {
            _ReleaseNHeap();
            return;
        }
        max_paras = PARAS_IN_64K;
#else
        if( _osmode != DOS_MODE ) {                     /* 23-apr-91 */
            max_paras = PARAS_IN_64K;
        } else {
            max_paras = TinyMaxSet( _psp );
            /* subtract off code size */
            max_paras -= _DGroup() - _psp;
            if( max_paras > PARAS_IN_64K ) {
                max_paras = PARAS_IN_64K;
            }
        }
#endif
        if( max_paras <= curr_paras ) {
            /* '<' -> something is wrong, '==' -> can't change size */
            _ReleaseNHeap();
            return;
        }
        diff_paras = max_paras - curr_paras;
        expand = (( diff_paras + 1 ) << 4 ) - ( _curbrk & 0x0f );
        expand += __LastFree(); /* compensate for _expand's adjustment */
        _ReleaseNHeap();
        _nfree( _nmalloc( expand - ( sizeof( size_t ) + sizeof(frl) ) ) );
#endif
    }

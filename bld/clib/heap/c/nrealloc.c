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
* Description:  Near heap realloc routines.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "heap.h"
#include "heapacc.h"


#if defined(__SMALL_DATA__)

_WCRTLINK void *realloc( void *stg, size_t amount )
    {
        return( _nrealloc( stg, amount ) );
    }

#endif

#pragma intrinsic(memcpy)

_WCRTLINK void _WCNEAR *_nrealloc( void _WCI86NEAR *stg, size_t req_size )
    {
        void _WCNEAR *p;
        size_t     old_size;

        if( stg == NULL ) {
            return( _nmalloc( req_size ) );
        }
        if( req_size == 0 ) {
            _nfree( stg );
            return( (void _WCNEAR *) NULL );
        }
        old_size = _nmsize( stg );
        p = _nexpand( stg, req_size );  /* try to expand it in place */
        if( p == NULL ) {               /* if couldn't be expanded in place */
#if defined(__DOS_EXT__)
            if( _IsRational() ) {
                frlptr  flp, newflp;

                flp = (frlptr) ((PTR)stg - TAG_SIZE);
                newflp = __ReAllocDPMIBlock( flp, req_size + TAG_SIZE );
                if( newflp ) {
                    return( (void _WCNEAR *)((PTR)newflp + TAG_SIZE) );
                }
            }
#endif
#if defined(__WARP__)
            // If block in upper memory (i.e. above 512MB), try to keep it there
            if ( (unsigned int)stg >= 0x20000000 ) {
                int prior;
                _AccessNHeap();
                prior = _os2_use_obj_any;
                _os2_use_obj_any = 1;
                p = _nmalloc( req_size );   /* - allocate a new block */
                _os2_use_obj_any = prior;
                _ReleaseNHeap();
            } else {
                p = _nmalloc( req_size );   /* - allocate a new block */
            }
#else // !__WARP__
            p = _nmalloc( req_size );   /* - allocate a new block */
#endif
            if( p != NULL ) {           /* - if we got one */
                memcpy( p, stg, old_size );  /* copy it */
                _nfree( stg );                  /* and free old one */
            } else {
                _nexpand( stg, old_size );      /* reset back to old size */
            }
        }
        return( p );
    }

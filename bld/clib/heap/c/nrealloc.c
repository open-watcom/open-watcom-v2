/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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

_WCRTLINK void *realloc( void *cstg, size_t amount )
{
    return( _nrealloc( cstg, amount ) );
}

#endif

_WCRTLINK void_nptr _nrealloc( void_nptr cstg_old, size_t req_size )
{
    void_nptr   cstg_new;
    size_t      old_size;

    if( cstg_old == NULL ) {
        return( _nmalloc( req_size ) );
    }
    if( req_size == 0 ) {
        _nfree( cstg_old );
        return( NULL );
    }
    old_size = _nmsize( cstg_old );
    cstg_new = _nexpand( cstg_old, req_size );  /* try to expand it in place */
    if( cstg_new == NULL ) {                    /* if couldn't be expanded in place */
#if defined(__DOS_EXT__)
        if( _IsRational() ) {
            freelist_nptr  flp, newflp;

            flp = (freelist_nptr)CSTG2BLK( cstg_old );
            newflp = __ReAllocDPMIBlock( flp, req_size + TAG_SIZE );
            if( newflp ) {
                return( (void_nptr)BLK2CSTG( newflp ) );
            }
        }
#endif
#if defined(__OS2_32BIT__)
        // If block in upper memory (i.e. above 512MB), try to keep it there
        if( (unsigned int)cstg_old >= 0x20000000 ) {
            int prior;
            _AccessNHeap();
            prior = _os2_use_obj_any;
            _os2_use_obj_any = 1;
            cstg_new = _nmalloc( req_size );    /* - allocate a new block */
            _os2_use_obj_any = prior;
            _ReleaseNHeap();
        } else {
            cstg_new = _nmalloc( req_size );    /* - allocate a new block */
        }
#else /* !defined(__OS2_32BIT__) */
        cstg_new = _nmalloc( req_size );        /* - allocate a new block */
#endif
        if( cstg_new != NULL ) {                /* - if we got one */
            memcpy( cstg_new, cstg_old, old_size );  /* copy it */
            _nfree( cstg_old );                 /* and free old one */
        } else {
            _nexpand( cstg_old, old_size );     /* reset back to old size */
        }
    }
    return( cstg_new );
}

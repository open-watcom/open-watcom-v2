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
* Description:  Near heap expansion routines.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"


#if defined(__SMALL_DATA__)

_WCRTLINK void *_expand( void *cstg, size_t amount )
{
    return( _nexpand( cstg, amount ) );
}

#endif

_WCRTLINK void_nptr _nexpand( void_nptr cstg, size_t req_size )
{
    size_t  growth_size;

    _AccessNHeap();
#if defined( _M_I86 )
  #if defined( __WINDOWS__ )
    if( __HeapManager_expand( _DGroup(), cstg, req_size, &growth_size ) != __HM_SUCCESS ) {
        cstg = NULL;
    }
  #else
    {
        int         retval;
        unsigned    expanded = 0;

        for( ;; ) {
            retval = __HeapManager_expand( _DGroup(), cstg, req_size, &growth_size );
            if( retval == __HM_SUCCESS ) {
                break;
            }
            if( retval == __HM_FAIL ) {
                cstg = NULL;
                break;
            }
            if( retval == __HM_TRYGROW ) {
                if( expanded || __ExpandDGROUP( growth_size ) == 0 ) {
                    cstg = NULL;
                    break;
                }
                expanded = 1;
            }
        }
    }
  #endif
#elif defined( __DOS_EXT__ )
    {
        int         retval;
        unsigned    expanded = 0;

        for( ;; ) {
            retval = __HeapManager_expand( cstg, req_size, &growth_size );
            if( retval == __HM_SUCCESS ) {
                break;
            }
            if( retval == __HM_FAIL || _IsRationalZeroBase() || _IsCodeBuilder() ) {
                cstg = NULL;
                break;
            }
            if( retval == __HM_TRYGROW ) {
                if( expanded || __ExpandDGROUP( growth_size ) == 0 ) {
                    cstg = NULL;
                    break;
                }
                expanded = 1;
            }
        }
    }
#else
    if( __HeapManager_expand( cstg, req_size, &growth_size ) != __HM_SUCCESS ) {
        cstg = NULL;
    }
#endif
    _ReleaseNHeap();
    return( cstg );
}

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
* Description:  Near heap set routines.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "heap.h"
#include "heapacc.h"


#if defined( _M_I86 )
// 16-bit Intel all models
extern  void    _mymemset(void _WCFAR *,unsigned,unsigned);
#pragma aux     _mymemset =     \
        "shr cx,1"              \
        "rep stosw"             \
        "adc cx,cx"             \
        "rep stosb"             \
    parm caller [es di] [ax] [cx] modify exact [di cx]
#elif defined( _M_IX86 )
// 32-bit Intel
#if defined( __NT__ ) ||  defined( __OS2__ ) || defined( __LINUX__ ) || defined( __WINDOWS__ ) || defined( __RDOS__ )
// flat model
extern  void    _mymemset(void *,unsigned,unsigned);
#pragma aux     _mymemset =     \
        "shr ecx,1"             \
        "rep stosw"             \
        "adc ecx,ecx"           \
        "rep stosb"             \
    parm caller [edi] [eax] [ecx] modify exact [edi ecx]
#else
// all segmented models
extern  void    _mymemset(void _WCFAR *,unsigned,unsigned);
#pragma aux     _mymemset =     \
        "shr ecx,1"             \
        "rep stosw"             \
        "adc ecx,ecx"           \
        "rep stosb"             \
    parm caller [es edi] [eax] [ecx] modify exact [edi ecx]
#endif
#else
// 32-bit non-Intel targets
#define _mymemset   memset
#endif

#if defined(__SMALL_DATA__)
_WCRTLINK int _heapset( unsigned int fill )
{
    return( _nheapset( fill ) );
}
#endif

_WCRTLINK int _nheapset( unsigned int fill )
{
    mheapptr    mhp;
    frlptr      curr_frl;
    int         heap_status;

    heap_status = _heapchk();
    if( heap_status != _HEAPOK ) {
        return( heap_status );
    }
    fill |= fill << 8;
    _AccessNHeap();

    for( mhp = __nheapbeg; mhp != NULL; mhp = mhp->next ) {
        for( curr_frl = mhp->freehead.next; curr_frl != (frlptr)&mhp->freehead; curr_frl = curr_frl->next ) {
            _mymemset( curr_frl + 1, fill, curr_frl->len - sizeof( freelistp ) );
        }
    }
    _ReleaseNHeap();
    return( _HEAPOK );
}

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


// OPNEW.CPP -- ::operator new default definition
//
// 93/12/16  -- J.W.Welch        -- use CPPLIB( new_allocator )
// 94/02/27  -- Greg Bentz       -- augment for MFC support
// 95/08/12  -- Greg Bentz       -- add _WPRTLINK
// 96/04/10  -- Greg Bentz      -- add layer of indirection for DLL library
//
// Note: _WPRTLINK is present here so that the entry point is
//      exported, but it is not present in new.h so that
//      the entry point can be replaced in the user code
// 0004

#include "cpplib.h"
#include "lock.h"
#include <malloc>
#include <stddef.h>
#if defined(__MAKE_DLL_CPPLIB)
  #define __SW_BR
#endif
#include <new.h>
#if defined(__MAKE_DLL_CPPLIB)
  #undef __SW_BR
#endif


#if defined(__MAKE_DLL_CPPLIB)
static void* __do_new( unsigned size )
#else
_WPRTLINK void* operator new(    // ALLOCATE STORAGE FOR NEW
    size_t size )               // - size required
#endif
{
    void *p;
    PFU  _handler;
    PFV  handler;

    if( size == 0 ) {
        ++size;
    }
    for(;;) {
        p = malloc( size );
        if( p != NULL ) break;

        // first try Microsoft Style handler
        _handler = _RWD_ThreadData._new_handler;
        if( NULL != _handler ) {
            if( (*_handler)( size ) ) continue;
        }

        // now try ANSI Style handler
        handler = _RWD_ThreadData.new_handler;
        if( NULL == handler ) break;
        (*handler)();
    }
    return p;
}

#if defined(__MAKE_DLL_CPPLIB)
static _PUP __pfn_new = &__do_new;

_WPRTLINK extern _PUP _set_op_new( _PUP on ) {
    _PUP old;
    _RWD_StaticInitSema.p();
    old = __pfn_new;
    __pfn_new = on;
    _RWD_StaticInitSema.v();
    return( old );
}

_WPRTLINK void* operator new(    // ALLOCATE STORAGE FOR NEW
    size_t size )               // - size required
{
    return (*__pfn_new)( size );
}
#endif

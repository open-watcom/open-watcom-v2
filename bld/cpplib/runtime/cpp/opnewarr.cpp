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


// OPNEWARR.CPP -- ::operator new[] default definition
//
// 93/12/16  -- J.W.Welch        -- defined
// 95/08/12  -- Greg Bentz       -- add _WPRTLINK
// 96/04/10  -- Greg Bentz      -- add layer of indirection for DLL library
//
// Note: _WPRTLINK is present here so that the entry point is
//      exported, but it is not present in new.h so that
//      the entry point can be replaced in the user code


#include "cpplib.h"
#include "lock.h"
#include <stddef.h>
#if defined(__MAKE_DLL_CPPLIB)
  #define __SW_BR
#endif
#include <new.h>
#if defined(__MAKE_DLL_CPPLIB)
  #undef __SW_BR
#endif


#if defined(__MAKE_DLL_CPPLIB)
static void* __do_new_array( unsigned size )
#else
_WPRTLINK void* operator new[](  // ALLOCATE STORAGE FOR NEW[]
    size_t size )               // - size required
#endif
{
    return ::operator new( size );
}

#if defined(__MAKE_DLL_CPPLIB)
static _PUP __pfn_new_array = &__do_new_array;

_WPRTLINK extern _PUP _set_op_new_array( _PUP ona ) {
    _PUP old;
    _RWD_StaticInitSema.p();
    old = __pfn_new_array;
    __pfn_new_array = ona;
    _RWD_StaticInitSema.v();
    return( old );
}

_WPRTLINK void* operator new[](  // ALLOCATE STORAGE FOR NEW[]
    size_t size )               // - size required
{
    return (*__pfn_new_array)( size );
}
#endif

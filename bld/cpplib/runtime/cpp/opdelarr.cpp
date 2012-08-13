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


// OPDELARR.CPP - default ::delete[] operator definition
//
// 93/12/16  -- J.W.Welch       -- Define delete[]
// 95/08/12  -- Greg Bentz      -- add _WPRTLINK
// 96/04/10  -- Greg Bentz      -- add layer of indirection for DLL library
//
// Note: _WPRTLINK is present here so that the entry point is
//      exported, but it is not present in new.h so that
//      the entry point can be replaced in the user code

#include "cpplib.h"
#include "lock.h"
#include <malloc.h>
#if defined(__MAKE_DLL_CPPLIB)
  #define __SW_BR
#endif
#include <new.h>
#if defined(__MAKE_DLL_CPPLIB)
  #undef __SW_BR
#endif

#if defined(__MAKE_DLL_CPPLIB)
static void __do_delete_array( void *p )
#else
_WPRTLINK void operator delete[](// RELEASE STORAGE FOR NEW[]
    void *p )                   // -- storage to release
#endif
{
    ::delete( (char *) p );
}

#if defined(__MAKE_DLL_CPPLIB)
static _PVV __pfn_delete_array = &__do_delete_array;

_WPRTLINK extern _PVV _set_op_delete_array( _PVV oda ) {
    _PVV old;
    _RWD_StaticInitSema.p();
    old = __pfn_delete_array;
    __pfn_delete_array = oda;
    _RWD_StaticInitSema.v();
    return( old );
}

_WPRTLINK void operator delete[](// RELEASE STORAGE FOR NEW[]
    void *p )                   // -- storage to release
{
    (*__pfn_delete_array)( p );
}
#endif

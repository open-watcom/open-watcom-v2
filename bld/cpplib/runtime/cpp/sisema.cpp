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


// SISEMA.CPP -- static initialization semaphore for multi-threading
//
// 92/10/27  -- G.R.Bentz       -- defined
// 93/03/95  -- Greg Bentz      -- integrated with front end
// 95/06/14  -- Greg Bentz      -- dll linkage

#ifdef __MT__

#include <cpplib.h>
#include <new>

extern "C"
_WPRTLINK
int CPPLIB(static_init)(            // CHECK STATIC INIT ONCE ONLY BIT
    unsigned char *bits,            // - bit field to check
    int mask )                      // - mask we are interested in
{
    int retn = 1;

    if( (*bits & mask) == 0 ) {
        _RWD_StaticInitSema.p();
        if( (*bits & mask) == 0 ) {
            (*bits) |= (unsigned char)mask;
            retn = 0;
        }
        _RWD_StaticInitSema.v();
    }
    return( retn );
}

#endif

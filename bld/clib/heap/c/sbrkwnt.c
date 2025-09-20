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
* Description:  sbrk() implementation for WIN32.
*
****************************************************************************/


#include "variety.h"
#include "seterrno.h"
#include <stddef.h>
#include <stdlib.h>
#include <dos.h>
#include <windows.h>
#include "roundmac.h"
#include "rtdata.h"
#include "thread.h"
#include "heap.h"


_WCRTLINK void_nptr sbrk( int increment )
{
    if( increment > 0 ) {
        LPVOID      cstg;

        increment = __ROUND_UP_SIZE_4K( increment );
        //cstg = LocalAlloc( LMEM_FIXED, increment );
        cstg = VirtualAlloc( NULL, increment, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
        if( cstg != NULL )
            return( cstg );
        lib_set_errno( ENOMEM );
    } else {
        lib_set_errno( EINVAL );
    }
    return( (void_nptr)-1 );
}

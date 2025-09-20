/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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


#include "variety.h"
#include "seterrno.h"
#include <stdlib.h>
#include <dos.h>
#include <windows.h>
#include "rtdata.h"


_WCRTLINK unsigned _dos_allocmem( unsigned num_of_paras, LPVOID *p_mem )
{
    LPVOID      ptr;

    ptr = GlobalAlloc( GMEM_FIXED, num_of_paras << 4 );
    if( ptr == NULL ) {
        return( __set_errno_nt_reterr() );
    }
    *p_mem = ptr;
    return( 0 );
}

_WCRTLINK unsigned _dos_freemem( LPVOID mem )
{
    if( GlobalFree( mem ) != NULL ) {
        return( __set_errno_nt_reterr() );
    }
    return( 0 );
}

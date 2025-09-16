/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include <io.h>
#include <windows.h>
#include "seterrno.h"
#include "iomode.h"
#include "rtcheck.h"
#include "thread.h"


/*
    BOOL UnlockFile(
      HANDLE hFile,                    // handle to file
      DWORD dwFileOffsetLow,           // low-order word of start
      DWORD dwFileOffsetHigh,          // high-order word of start
      DWORD nNumberOfBytesToUnlockLow, // low-order word of length
      DWORD nNumberOfBytesToUnlockHigh // high-order word of length
    );
 */

_WCRTLINK int unlock( int handle, unsigned long offset, unsigned long nbytes )
{
    __handle_check( handle, -1 );

    if( UnlockFile( __getOSHandle( handle ), offset, 0L, nbytes, 0L ) == 0 ) {
        return( __set_errno_nt() );
    }
    return( 0 );
}

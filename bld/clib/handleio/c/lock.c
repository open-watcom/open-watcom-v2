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


#include "variety.h"
#include <stddef.h>
#include <io.h>

#ifdef __OS2__
#include <wos2.h>
#else
#include "tinyio.h"
#endif
#include "rtcheck.h"
#include "seterrno.h"


_WCRTLINK int lock( int handle, unsigned long offset, unsigned long nbytes )
    {
#if defined(__WARP__)
        APIRET          rc;
        FILELOCK        lock_block;

        __handle_check( handle, -1 );
        lock_block.lOffset = offset;
        lock_block.lRange = nbytes;
        /* last 2 arguments are not documented */
        rc = DosSetFileLocks( handle, NULL, &lock_block, 0, 0 );
        if( rc != 0 ) {
            return( __set_errno_dos( rc ) );
        }
        return( 0 );
#elif defined(__OS2_286__)
        APIRET          rc;
        LONG lock_block[2];

        __handle_check( handle, -1 );
        lock_block[0] = offset;
        lock_block[1] = nbytes;
        rc = DosFileLocks( handle, NULL, &lock_block );
        if( rc != 0 ) {
            return( __set_errno_dos( rc ) );
        }
        return( 0 );
#else
        tiny_ret_t rc;

        __handle_check( handle, -1 );
        rc = TinyLock( handle, offset, nbytes );
        if( TINY_ERROR(rc) ) {
            return( __set_errno_dos( TINY_INFO(rc) ) );
        }
        return( 0 );
#endif
    }

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
* Description:  Implementation of isatty() for DOS/Win16.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include "rtdata.h"
#include "tinyio.h"
#include "defwin.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"

/* determine if file is a terminal */

_WCRTLINK int isatty( int handle )
{
    tiny_ret_t      rc;

    __handle_check( handle, 0 );
#ifdef DEFAULT_WINDOWING
    if( _WindowsIsWindowedHandle != NULL
      && _WindowsIsWindowedHandle( handle ) != NULL ) {
        return( true );
    }
#endif
    rc = TinyGetDeviceInfo( handle );
    return( TINY_OK( rc ) && (TINY_INFO( rc ) & TIO_CTL_DEVICE) );
}

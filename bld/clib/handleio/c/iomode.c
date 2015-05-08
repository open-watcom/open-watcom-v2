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
* Description:  io_mode handle information array manipulation
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#if defined(__OS2__)
 #define INCL_DOSFILEMGR
 #include <wos2.h>
#endif
#include "rtdata.h"
#include "liballoc.h"
#include "fileacc.h"
#include "rtinit.h"
#include "seterrno.h"
#include "iomode.h"

unsigned __NFiles   = _NFILES;          /* maximum # of files we can open */

#if defined(__NETWARE__)
#error NO IO MODE MANAGER UNDER NETWARE
#endif

#if !defined(__UNIX__)

unsigned _HUGEDATA __init_mode[_NFILES] = { /* file mode information (flags) */
        _READ,          /* stdin */
        _WRITE,         /* stdout */
        _WRITE,         /* stderr */
        _READ|_WRITE,   /* stdaux */
        _WRITE          /* stdprn */
};

unsigned *__io_mode = __init_mode;      /* initially points to static array */

unsigned __GetIOMode( int handle )
{
    if( handle >= __NFiles ) {
        return( 0 );
    }
    return( __io_mode[handle] );
}

void __SetIOMode_nogrow( int handle, unsigned value )
{
    if( handle < __NFiles ) {
        __io_mode[handle] = value;    /* we're closing it; smite _INITIALIZED */
    }
}

#endif


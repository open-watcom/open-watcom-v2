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
#include <unistd.h>
#if defined(__DOS__) || defined(__WINDOWS__)
    #include <dos.h>
#elif defined(__NETWARE__)
    #if defined (_NETWARE_CLIB)
        #include <owfileng.h>
    #endif
#endif
#include "rterrno.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "thread.h"

/*
//  take fsync from LIBC import file
*/
#if !defined (_NETWARE_LIBC)

_WCRTLINK int fsync( int handle )
/*******************************/
{
    int                 ret = 0;

    __handle_check( handle, -1 );

    #if defined(__DOS__) || defined(__WINDOWS__)
    ret = _dos_commit( handle );
    #elif defined(__NT__)
    if( !FlushFileBuffers( __getOSHandle( handle ) ) ) 
    {
        __set_errno_nt();
        ret = -1;
    }
    #elif defined(__OS2__)
    if( DosBufReset( handle ) != 0 ) 
    {
        _RWD_errno = EBADF;
        ret = -1;
    }
    #elif defined(__NETWARE__)

    if( FEFlushWrite( handle ) != 0 ) 
    {
        _RWD_errno = EBADF;
        ret = -1;
    }
    #else
        #error Unknown target system
    #endif

    return( ret );
}

#endif /* !defined (_NETWARE_LIBC) */

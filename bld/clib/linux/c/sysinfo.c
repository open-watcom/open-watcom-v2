/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom contributors. 
*    All Rights Reserved.
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
* Description:  Retrieves a system info structure
*
* Author: J. Armstrong
****************************************************************************/

#include "variety.h"
#include <sys/sysinfo.h>
#include "rtdata.h"
#include "linuxsys.h"
#include "errorno.h"

_WCRTLINK int sysinfo ( struct sysinfo *__info )
{
u_long ures;
    
    if( __info == 0 ) {
        _RWD_errno = EINVAL;
        return -1;
    }
    
    sys_call1( SYS_sysinfo, (u_long)__info );
       
    __syscall_return( int, ures );
}

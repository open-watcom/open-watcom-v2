/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2014 Open Watcom contributors. 
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
* Description:  Linux pclose() implementation.
*
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include <sys/wait.h>
#include "rtdata.h"
#include "linuxsys.h"

_WCRTLINK int pclose( FILE *fp )
{
    int status;
    pid_t pret;

    /*** Get the return code of the process ***/
    if( _FP_PIPEDATA(fp).isPipe == 0 ) {
        return( -1 ); /* must be a pipe */
    }
    
    if( fclose( fp ) ) {
        return( -1 ); /* must close pipe before waitpid */
    }

    /* close stream and wait for process termination */
    do {
        pret = waitpid(_FP_PIPEDATA(fp).pid, &status, 0);
    } while (pret == -1 && _RWD_errno == EINTR);


    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    
    _RWD_errno = ECHILD;
    return -1;
}

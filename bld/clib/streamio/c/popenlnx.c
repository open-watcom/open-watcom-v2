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
* Description:  Linux popen() implementation.
*
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include "rtdata.h"
#include "linuxsys.h"

#define _PIPE_READ_END      0
#define _PIPE_WRITE_END     1

_WCRTLINK FILE* popen(const char *command, const char *mode)
{
    int fd[2];
    pid_t pid;
    FILE *fp;

    if ((mode[0] != 'r' && mode[0] != 'w') || mode[1] != '\0') {
        /* Note: by checking mode[1], we are basically ruling out
         * glibc's extra designators, as we should.
         */
        _RWD_errno = EINVAL;
        return NULL;
    }
    
    if (pipe(fd) < 0) {
        /* errno is set appropriately in pipe */
        return NULL;
    }

    pid = fork();
    
    switch (pid) {
        
        case -1:
            /* Fork failed... */
            close(fd[0]);
            close(fd[1]);

            /* errno should be set by failing fork call */
            return NULL;
        
        case 0:
            /* Duplicate the file handle we want, close both originals */
            if (mode[0] == 'r') {
                dup2(fd[_PIPE_WRITE_END], STDOUT_FILENO);
            } else {
                dup2(fd[_PIPE_READ_END], STDIN_FILENO);
            }
            close(fd[0]);
            close(fd[1]);
        
            execl("/bin/sh", "sh", "-c", command, (char *) NULL);
            _exit(1);

        default:
            /* This is the parent process.  Close the proper pipe end. */
            if (mode[0] == 'r') {
                close(fd[_PIPE_WRITE_END]);
                fp = fdopen(fd[_PIPE_READ_END], mode);
                if(fp == NULL) {
                    close(fd[_PIPE_READ_END]);
                }
            } else {
                close(fd[_PIPE_READ_END]);
                fp = fdopen(fd[_PIPE_WRITE_END], mode);
                if(fp == NULL) {
                    close(fd[_PIPE_WRITE_END]);
                }
            }
            
            _FP_PIPEDATA(fp).isPipe = 1;
            _FP_PIPEDATA(fp).pid = pid;       
    }

    return fp;
}

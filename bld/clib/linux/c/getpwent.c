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
* Description:  Retrieves entries from the Linux passwd database
*
* Author: J. Armstrong
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

static FILE *__pwfp;

_WCRTLINK void setpwent()
{
    if(__pwfp != NULL)
        fclose(__pwfp);
    __pwfp = NULL;
}

_WCRTLINK struct passwd *getpwent()
{
    static char *line;
    static size_t linesize;
    static struct passwd ret;

    ssize_t line_length;

    char *ptr;
    char *numptr;
    char *lignore;

    if(!__pwfp)
        __pwfp = fopen("/etc/passwd", "r");
    if(!__pwfp)
        goto getpwent_fail;
    
    while(1) {
        line_length = getline(&line, &linesize, __pwfp);
        if(line_length < 0)
            goto getpwent_fail;
            
        ret.pw_name = line;
        ptr = strchr(line, ':');    
        if(ptr == NULL) continue;
        *ptr++ = '\0';
        
        ret.pw_passwd = ptr;
        ptr = strchr(ptr, ':');    
        if(ptr == NULL) continue;
        *ptr++ = '\0';
        
        numptr = ptr;
        ptr = strchr(ptr, ':');    
        if(ptr == NULL) continue;
        *ptr++ = '\0';
        ret.pw_uid = (uid_t)strtol(numptr, &lignore, 10);
        
        numptr = ptr;
        ptr = strchr(ptr, ':');    
        if(ptr == NULL) continue;
        *ptr++ = '\0';
        ret.pw_gid = (gid_t)strtol(numptr, &lignore, 10);
        
        ret.pw_gecos = ptr;
        ptr = strchr(ptr, ':');    
        if(ptr == NULL) continue;
        *ptr++ = '\0';
        
        ret.pw_dir = ptr;
        ptr = strchr(ptr, ':');    
        if(ptr == NULL) continue;
        *ptr++ = '\0';
        
        ret.pw_shell = ptr;
        ptr = strchr(line, '\n');
        if(ptr != NULL)
            *ptr = '\0';
        
        break;
    }
    
    return &ret;
    
getpwent_fail:
    if(line != NULL) 
        free(line);
        
    linesize = 0;
    
    return NULL;
}

/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom Contributors.
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
* Description:  Implementations of gethostent/sethostent/endhostent
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rtdata.h"
#include "variety.h"

#define MAX_HOST_ALIASES  16

static const char *__hostFname = "/etc/hosts";
static FILE *__hostFile;
static int __hostClose = 1;

/*
 * Return the next (non-commented) line from the host-file
 * Format is:
 *  ip-address [=] host-name [alias..] {\n | # ..}
 */
_WCRTLINK struct hostent *gethostent (void)
{
    static struct hostent ret;
    char *ip, *alias;
    
    /* For loading a line from the hosts file */
    static char     *buf = NULL;
    static size_t    buflen = 0;
    
    /* More static data */
    static in_addr_t primary;
    static char *addr_list[2];
    
    /* For line parsing */
    int i;
    
    if(__hostFile == NULL)
        sethostent(0);
 
    /* First pass */
    if(buf == NULL) {
        buflen = 64;
        buf = (char *)malloc(buflen);
        if(buf)
            *buf = '\0';
        else
            buflen = 0;
    }

    do
    {
        if(getline(&buf, &buflen, __hostFile) < 0)
            return NULL;
    } while (buf[0] == '\0' || buf[0] == '#' || buf[0] == ';');

    /* Store the IP address (a bit complicated) */
    /* ... and we only support one IP address */
    ip = strtok(buf, " \t");
    primary = inet_addr(ip);
    
    /* If no address for this line, try again (could be an IPv6 value) */
    if(primary == INADDR_NONE)
        return gethostent();
    
    if(__hostClose)
        endhostent();
    
    addr_list[0] = (char *)&primary;
    addr_list[1] = NULL;
    ret.h_addr_list = addr_list;
    ret.h_length = 4;
    
    /* Everything here is assumed to be a Internet address */
    ret.h_addrtype  = AF_INET;
    
    /* The host name */
    ret.h_name = strtok(NULL, " \t\n");

    /* Load in any aliases */
    ret.h_aliases = NULL;
    alias = strtok(NULL, " \t\n");

    if (alias && *alias != '#' && *alias != ';')
    {
        ret.h_aliases = calloc ((1+MAX_HOST_ALIASES), sizeof(char *));
        i = -1;
        do
        {
            i++;
            if (*alias == '#' || *alias == ';')
                break;
        
            ret.h_aliases[i] = strtok(NULL, " \t\n");
            
            if(ret.h_aliases[i] == NULL)
                break;
            else if(ret.h_aliases[i][0] == '\0') {
                ret.h_aliases[i] = NULL;
                break;
            }
            
        } while (i < MAX_HOST_ALIASES);
        
    }
    return &ret;
}

_WCRTLINK void sethostent (int stayopen)
{
  __hostClose = (stayopen == 0);
  
  if (!__hostFile)    
      __hostFile = fopen (__hostFname, "rt");
  else 
      rewind (__hostFile);
}

_WCRTLINK void endhostent (void)
{
  if (!__hostFile)
     return;   

  fclose (__hostFile);
  __hostFile  = NULL;
  __hostClose = 1;
} 

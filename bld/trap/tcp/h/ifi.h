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
* Description:  Structures and function prototypes for querying IP
*               interfaces in a machine.
*
****************************************************************************/

#ifndef __IFI_H__
#define __IFI_H__

#define IFI_NAME    16          /* same as IFNAMSIZ in <net/if.h> */

struct ifi_info {
    char    ifi_name[IFI_NAME]; /* interface name, null terminated */
    short   ifi_flags;          /* IFF_xxx constants from <net/if.h> */
    short   flags;              /* our own IFI_xxx flags */
    struct sockaddr  *ifi_addr; /* primary address */
    struct ifi_info  *ifi_next; /* next of these structures */
};

#define IFI_ALIAS   1           /* ifi_addr is an alias */
#define IFI_LOOP    2           /* corresponds to IFF_LOOPBACK */

//struct ifi_info *get_ifi_info( int, int );
//void             free_ifi_info( struct ifi_info * );

#endif

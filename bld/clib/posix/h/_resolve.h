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
* Description:  Header for internal network name resolution helpers
*
* Author: J. Armstrong
*
****************************************************************************/


#include <netinet/in.h>
#include <netdb.h>

/* Query types for DNS queries */
#define DNSQ_TYPE_A     1   /* IPv4 Address */
#define DNSQ_TYPE_NS    2   /* Nameserver */
#define DNSQ_TYPE_CNAME 5   /* Canonical Name */
#define DNSQ_TYPE_SOA   6   /* Start of Authority Zone */
#define DNSQ_TYPE_PTR   12  /* Domain Name Pointer */
#define DNSQ_TYPE_MX    15  /* Mail eXchange */

/* Attempts to request server information from DNS.  Upon success,
 * routine will return 1.  Upon failure, routine should return the
 * negative error code.
 */
extern int _dns_query( const char *__name, int __query_type, in_addr_t __dnsaddr,
                        struct hostent *__res );

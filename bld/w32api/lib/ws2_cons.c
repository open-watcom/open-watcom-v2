/***************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2014 The Open Watcom Contributors. All Rights Reserved.
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
*  Description: Implementation of constants in ws2_32.lib.
*
*  Note: Some IPv4 values taken from -
*        http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xhtml
*
*  Author: J. Armstrong
*
**************************************************************************/

/* Necessary to define so we can access IPv6 constants */
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif

#define NTDDI_VERSION 0x05000100
#include <winsock2.h>
#include <ws2tcpip.h>

const IN_ADDR __cdecl in4addr_any = {{ INADDR_ANY }};
const IN_ADDR __cdecl in4addr_loopback = {{ { 0x7F, 0, 0, 0x01 } }};
const IN_ADDR __cdecl in4addr_broadcast = {{ INADDR_BROADCAST }};
const IN_ADDR __cdecl in4addr_allnodesonlink = {{ { 0xE0, 0, 0, 0x01 } }};
const IN_ADDR __cdecl in4addr_allroutersonlink = {{ { 0xE0, 0, 0, 0x02 } }};
const IN_ADDR __cdecl in4addr_alligmpv3routersonlink = {{ { 0xE0, 0, 0, 0x16 } }};
const IN_ADDR __cdecl in4addr_allteredohostsonlink = {{ { 0xE0, 0, 0, 0xFD } }};
const IN_ADDR __cdecl in4addr_linklocalprefix = {{ { 0xA9, 0xFE } }};
const IN_ADDR __cdecl in4addr_multicastprefix = {{ { 0xE0 } }};

const IN6_ADDR __cdecl in6addr_any = {{ IN6ADDR_ANY_INIT }};
const IN6_ADDR __cdecl in6addr_loopback = {{ IN6ADDR_LOOPBACK_INIT }};
const IN6_ADDR __cdecl in6addr_allnodesonnode = {{ IN6ADDR_ALLNODESONNODE_INIT }};
const IN6_ADDR __cdecl in6addr_allnodesonlink = {{ IN6ADDR_ALLNODESONLINK_INIT }};
const IN6_ADDR __cdecl in6addr_allroutersonlink = {{ IN6ADDR_ALLROUTERSONLINK_INIT }};
const IN6_ADDR __cdecl in6addr_allmldv2routersonlink = {{ IN6ADDR_ALLMLDV2ROUTERSONLINK_INIT }};
const IN6_ADDR __cdecl in6addr_teredoinitiallinklocaladdress = {{ IN6ADDR_TEREDOINITIALLINKLOCALADDRESS_INIT }};
const IN6_ADDR __cdecl in6addr_linklocalprefix = {{ IN6ADDR_LINKLOCALPREFIX_INIT }};
const IN6_ADDR __cdecl in6addr_multicastprefix = {{ IN6ADDR_MULTICASTPREFIX_INIT }};
const IN6_ADDR __cdecl in6addr_solicitednodemulticastprefix = {{ IN6ADDR_SOLICITEDNODEMULTICASTPREFIX_INIT }};
const IN6_ADDR __cdecl in6addr_v4mappedprefix = {{ IN6ADDR_V4MAPPEDPREFIX_INIT }};
const IN6_ADDR __cdecl in6addr_6to4prefix = {{ IN6ADDR_6TO4PREFIX_INIT }};
const IN6_ADDR __cdecl in6addr_teredoprefix = {{ IN6ADDR_TEREDOPREFIX_INIT }};
const IN6_ADDR __cdecl in6addr_teredoprefix_old = {{ IN6ADDR_TEREDOPREFIX_INIT_OLD }};

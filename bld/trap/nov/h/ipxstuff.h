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


//#define       MAX_DATA_SIZE   20 /* for testing */
#define MAX_DATA_SIZE   512
#define END_OF_MSG      0x01

/* be careful of wrap-around state */
#define LOWER_SEQ( a, b ) (((signed_16)(WORD)((a)-(b))) < 0)

#define _SWAPINT(var)   ((((unsigned)(var)<<8)&0xff00)|(((var)>>8)&0xff))

#define SAP_SOCKET_VALUE        0x0452
#define SAP_SOCKET      _SWAPINT( SAP_SOCKET_VALUE )
#define DBG_SERVER_TYPE _SWAPINT( 0x0093 )

#define TICKS_PER_SEC           18
#define MAX_CONNECT_WAIT        (4*TICKS_PER_SEC)
#define MAX_PARTNER_WAIT        (2*TICKS_PER_SEC)
#define MAX_ACKNOWLEDGE_WAIT    (1*TICKS_PER_SEC)

#define IPX_PACKET_TYPE         4
#define SPX_PACKET_TYPE         5

#ifndef __OS2__

#define _INITECB( ecb, head, count, tipe )                      \
    {                                                           \
        memset( &ecb, 0, sizeof( ecb ) );                       \
        ecb.socketNumber = tipe##Socket;                        \
        ecb.fragmentCount = count;                              \
        ecb.ESRAddress = NULL;                                  \
        ecb.fragmentDescriptor[0].address = &head;              \
        ecb.fragmentDescriptor[0].size = sizeof( head );        \
        head.packetType = tipe##_PACKET_TYPE;                   \
        head.length = _SWAPINT( sizeof( head ) );               \
    }

#define _INITIPXECB( name )                                     \
    _INITECB( name##ECB, name##Head, 1, IPX )

#define _INITSPXECB( name, count, addr, len )                   \
    {                                                           \
        _INITECB( name##ECB, name##Head, count, SPX )           \
        name##ECB.fragmentDescriptor[1].address = addr;         \
        name##ECB.fragmentDescriptor[1].size = len;             \
    }

#else

#define _INITECB( ecb, head, count, tipe )                      \
    {                                                           \
        memset( &ecb, 0, sizeof( ecb ) );                       \
        ecb.fragCount = count;                                  \
        ecb.fragList[0].fragAddress = &head;                    \
        ecb.fragList[0].fragSize = sizeof( head );              \
        head.packetType = tipe##_PACKET_TYPE;                   \
        head.packetLen = _SWAPINT( sizeof( head ) );            \
    }

#define _INITIPXECB( name )                                     \
    _INITECB( name##ECB, name##Head, 1, IPX )

#define _INITSPXECB( name, count, addr, len )                   \
    {                                                           \
        _INITECB( name##ECB, name##Head, count, SPX )           \
        name##ECB.fragList[1].fragAddress = addr;               \
        name##ECB.fragList[1].fragSize = len;                   \
    }

#endif

#define AssignArray( a,b ) memcpy( &(a), &(b), sizeof( a ) );
#define FillArray( a,b )   memset( &(a), b, sizeof( a ) );
#define ZeroArray( a )     memset( &(a), 0, sizeof( a ) );

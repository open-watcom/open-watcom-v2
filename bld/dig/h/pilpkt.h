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


#ifndef PILPKT_H

#include "piltypes.h"

typedef struct {
    link_buffer *buff;
    pil_header  *h;
    unsigned    curr;
} link_packet;

/*
        NO      -> Natural Order
        LO      -> Link Order (low byte first)
*/

#if defined(__X86__) || defined(__AXP__)
    #define PPU8LO( data )      (data)
    #define PPU16LO( data )     (data)
    #define PPU32LO( data )     (data)
    #define PKT_LITTLE_ENDIAN
//#elif ...big endian processors
//    extern unsigned_16 PPSwapU16( unsigned_16 );
//    extern unsigned_32 PPSwapU32( unsigned_32 );
//    #define PPU8LO( data )    (data)
//    #define PPU16LO( data )   (PPSwapU16(data))
//    #define PPU32LO( data )   (PPSwapU32(data))
//    #define PKT_BIG_ENDIAN
#else
    #error Byte order not set for processor
#endif



/*
        PIL Packet Putting.
*/
void            PPPutInit( link_packet *pkt, link_buffer *buff, unsigned req, unsigned service, unsigned number );
unsigned        PPPutOffset( link_packet *pkt, unsigned new_point );
void            *PPPutPointer( link_packet *pkt );
unsigned        PPPutData( link_packet *pkt, unsigned align_size, const void *data );
unsigned        PPPutString( link_packet *pkt, unsigned header, const void *string );
unsigned        PPPutU8NO( link_packet *pkt, unsigned_8 data );
unsigned        PPPutU16NO( link_packet *pkt, unsigned_16 data );
unsigned        PPPutU32NO( link_packet *pkt, unsigned_32 data );

#define         PPPutU8LO( pkt, data )  PPPutU8NO( pkt, PPU8LO( data ) )
#define         PPPutU16LO( pkt, data ) PPPutU16NO( pkt, PPU16LO( data ) )
#define         PPPutU32LO( pkt, data ) PPPutU32NO( pkt, PPU32LO( data ) )

/*
        PIL Packet Getting.
*/
void            PPGetInit( link_packet *pkt, link_buffer *buff );
unsigned        PPGetOffset( link_packet *pkt, unsigned new_point );
unsigned        PPGetLeft( link_packet *pkt );
void            *PPGetData( link_packet *pkt, unsigned align_size );
unsigned        PPGetStringLen( link_packet *pkt );
unsigned        PPGetString( link_packet *pkt, void *buff );
unsigned_8      PPGetU8NO( link_packet *pkt );
unsigned_16     PPGetU16NO( link_packet *pkt );
unsigned_32     PPGetU32NO( link_packet *pkt );

#define         PPGetU8LO( pkt )        PPU8LO( PPGetU8NO( pkt ) )
#define         PPGetU16LO( pkt )       PPU16LO( PPGetU16NO( pkt ) )
#define         PPGetU32LO( pkt )       PPU32LO( PPGetU32NO( pkt ) )

#define PILPKT_H
#endif

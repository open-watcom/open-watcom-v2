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


#include <string.h>
#include "pilpkt.h"

/* assumes aligment is always power of two */
#define SETTOALIGN( o, a )      (((o) + ((a)-1) & ~((a)-1)))

#ifdef PKT_BIG_ENDIAN
unsigned_16 PPSwapU16( unsigned_16 data )
{
    return( (data >> 8) | ((data & 0xff) << 8) );
}

unsigned_32 PPSwapU32( unsigned_32 data )
{
    return( (data >> 24)
          | (data & 0x00ff0000) >> 8)
          | (data & 0x0000ff00) << 8)
          | (data & 0x000000ff) << 24) );
}
#endif
/*
        PIL Packet Putting.
*/
void            PPPutInit( link_packet *pkt, link_buffer *buff, unsigned req, unsigned service, unsigned number )
{
    buff->len = sizeof( pil_header );
    pkt->buff = buff;
    pkt->curr = 0;
    pkt->h = (pil_header *)&buff->data[0];
    memset( pkt->h, 0, sizeof( pil_header ) );
    pkt->h->r_service = service;
    pkt->h->r_num = number;
    pkt->h->req_handle = req;
}

unsigned        PPPutOffset( link_packet *pkt, unsigned new_point )
{
    unsigned    old;

    old = pkt->buff->len;
    if( new_point != 0 ) pkt->buff->len = new_point;
    return( old );
}

void            *PPPutPointer( link_packet *pkt )
{
    return( &pkt->buff->data[pkt->buff->len] );
}

unsigned        PPPutData( link_packet *pkt, unsigned align_size, const void *data )
{
    unsigned    align;
    unsigned    new;

    align = AS_ALIGNGET( align_size );
    new = SETTOALIGN( pkt->buff->len, align );
    pkt->buff->len = new + AS_SIZEGET( align_size );
    memcpy( &pkt->buff->data[new], data, AS_SIZEGET( align_size ) );
    return( new );
}

unsigned        PPPutString( link_packet *pkt, unsigned header, const void *string )
{
    unsigned    len;
    const unsigned_16   *p;
    unsigned    new;

    len = PS_SIZEGET( header );
    if( len != 0 && string != NULL ) {
        switch( PS_ENCODEGET( header ) ) {
        case PCE_UNICODE:
            for( p = string; *p != 0; ++p ) {
                /* nothing to do */
            }
            len = (unsigned_8 *)p - (unsigned_8 *)string;
        default:
            len = strlen( string );
            break;
        }
        header |= len;
    }
    new = PPPutU16LO( pkt, header );
    if( len > 0 ) {
        #ifdef PKT_BIG_ENDIAN
            #error Unicode on big endian system not supported yet
        #else
            PPPutData( pkt, MKALIGNSIZE( 1, len ), string );
        #endif
    }
    return( new );
}

unsigned        PPPutU8NO( link_packet *pkt, unsigned_8 data )
{
    return( PPPutData( pkt, ALIGNSIZE_1, &data ) );
}

unsigned        PPPutU16NO( link_packet *pkt, unsigned_16 data )
{
    return( PPPutData( pkt, ALIGNSIZE_2, &data ) );
}

unsigned        PPPutU32NO( link_packet *pkt, unsigned_32 data )
{
    return( PPPutData( pkt, ALIGNSIZE_4, &data ) );
}


/*
        PIL Packet Getting.
*/
void            PPGetInit( link_packet *pkt, link_buffer *buff )
{
    pkt->curr = sizeof( pil_header );
    pkt->buff = buff;
    pkt->h = (pil_header *)&buff->data[0];
}

unsigned        PPGetOffset( link_packet *pkt, unsigned new_point )
{
    unsigned    old;

    old = pkt->curr;
    if( new_point != 0 ) pkt->curr = new_point;
    return( old );
}

unsigned        PPGetLeft( link_packet *pkt )
{
    return( pkt->buff->len - pkt->curr );
}

void            *PPGetData( link_packet *pkt, unsigned align_size )
{
    unsigned    align;
    unsigned    new;

    align = AS_ALIGNGET( align_size );
    new = SETTOALIGN( pkt->curr, align );
    pkt->curr = new + AS_SIZEGET( align_size );
    return( &pkt->buff->data[new] );
}

unsigned        PPGetStringLen( link_packet *pkt )
{
    unsigned    old;
    unsigned    header;

    old = pkt->curr;

    header = PPGetU16LO( pkt );
    pkt->curr = old;
    return( PS_SIZEGET( header ) );
}

unsigned        PPGetString( link_packet *pkt, void *buff )
{
    unsigned    header;
    unsigned    len;
    void        *end;

    header = PPGetU16LO( pkt );
    len = PS_SIZEGET( header );
    if( buff != NULL ) {
        #ifdef PKT_BIG_ENDIAN
            #error Unicode not supported on big endian machines yet
        #else
            memcpy( buff, &pkt->buff->data[pkt->curr], len );
        #endif
        end = (unsigned_8 *)buff + len;
        switch( PS_ENCODEGET( header ) ) {
        case PCE_UNICODE:
            *(unsigned_16 *)end = 0;
            break;
        default:
            *(unsigned_8 *)end = 0;
            break;
        }
    }
    pkt->curr += len;
    return( header );
}

unsigned_8      PPGetU8NO( link_packet *ptk )
{
    return( *(unsigned_8 *)PPGetData( ptk, ALIGNSIZE_1 ) );
}

unsigned_16     PPGetU16NO( link_packet *ptk )
{
    return( *(unsigned_16 *)PPGetData( ptk, ALIGNSIZE_2 ) );
}

unsigned_32     PPGetU32NO( link_packet *ptk )
{
    return( *(unsigned_32 *)PPGetData( ptk, ALIGNSIZE_4 ) );
}

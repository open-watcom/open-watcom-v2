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


#include "jvmdip.h"
#include "madjvm.h"


char    NameBuff[MAX_NAME];
address DefCodeAddr;
address DefDataAddr;

void LocationCreate( location_list *ll, location_type lt, void *d )
{
    ll->num = 1;
    ll->flags = 0;
    ll->e[0].bit_start = 0;
    ll->e[0].bit_length = 0;
    ll->e[0].type = lt;
    if( lt == LT_ADDR ) {
        ll->e[0].u.addr = *(address *)d;
    } else {
        ll->e[0].u.p = d;
    }
}

void LocationAdd( location_list *ll, long sbits )
{
    location_entry      *le;
    unsigned long       add;
    byte                num;
    unsigned long       bits;

    bits = sbits;
    if( sbits < 0 ) {
        bits = -bits;
        add = (bits + 7) / 8;
        if( ll->e[0].type == LT_ADDR ) {
            ll->e[0].u.addr.mach.offset -= add;
        } else {
            ll->e[0].u.p = (byte *)ll->e[0].u.p - add;
        }
        bits = 8 - (bits % 8);
        bits %= 8;
    }
    num = 0;
    le = &ll->e[0];
    for( ;; ) {
        if( le->bit_length == 0 ) break;
        if( le->bit_length > bits ) break;
        bits -= le->bit_length;
        ++num;
    }
    if( num != 0 ) {
        ll->num -= num;
        memcpy( &ll->e[0], le, ll->num * sizeof( ll->e[0] ) );
    }
    add = bits / 8;
    bits = bits % 8;
    ll->e[0].bit_start += bits;
    if( ll->e[0].bit_length != 0 ) ll->e[0].bit_length -= bits;
    if( ll->e[0].type == LT_ADDR ) {
        ll->e[0].u.addr.mach.offset += add;
    } else {
        ll->e[0].u.p = (byte *)ll->e[0].u.p + add;
    }
}

void LocationTrunc( location_list *ll, unsigned bits )
{
    byte    i;

    if( bits != 0 ) {
        for( i = 0; i < ll->num; ++i ) {
            if( ll->e[i].bit_length == 0 || ll->e[i].bit_length > bits ) {
                ll->e[i].bit_length = (word)bits;
                break;
            }
            bits -= ll->e[i].bit_length;
        }
    }
}

dip_status GetData( ji_ptr off, void *p, unsigned len )
{
    location_list       src;
    location_list       dst;
    address             src_addr;
    dip_status          ds;

    if( off < 0x1000 ) {
        ds = DS_ERR | DS_NO_READ_MEM;
    } else {
        memset( &src_addr, 0, sizeof( src_addr ) );
        src_addr.mach.offset = off;
        src_addr.mach.segment = MAD_JVM_DIP_MEM_SELECTOR;
        LocationCreate( &src, LT_ADDR, &src_addr );
        LocationCreate( &dst, LT_INTERNAL, p );
        ds = DCAssignLocation( &dst, &src, len );
    }
    if( ds != DS_OK ) memset( p, 0, len );
    return( ds );
}

#define MAX_CHUNK       32

unsigned GetString( ji_ptr off, char *buff, unsigned buff_size )
{
    unsigned    len;
    char        *end;
    unsigned    get;

    len = 0;
    if( buff_size > 0 ) {
        --buff_size;
        buff[buff_size] = '\0';
        for( ; buff_size > 0; ) {
            get = buff_size;
            if( get > MAX_CHUNK )
                get = MAX_CHUNK;
            if( GetData( off, buff, get ) != DS_OK ) {
                *buff = '\0';
                return( 0 );
            }
            end = memchr( buff, '\0', get );
            if( end != NULL ) {
                len += end - buff;
                break;
            }
            len += get;
            buff += get;
            buff_size -= get;
        }
    }
    return( len );
}

ji_ptr  GetPointer( ji_ptr off )
{
    ji_ptr      p;

    GetData( off, &p , sizeof( p ) );
    return( p );
}

unsigned        GetU8( ji_ptr off )
{
    unsigned_8  p;

    GetData( off, &p , sizeof( p ) );
    return( p );
}

unsigned        GetU16( ji_ptr off )
{
    unsigned_16 p;

    GetData( off, &p , sizeof( p ) );
    return( p );
}

unsigned long   GetU32( ji_ptr off )
{
    unsigned_32 p;

    GetData( off, &p , sizeof( p ) );
    return( p );
}

/*
    Tell the trap file to do a FindClass on the given class name
*/
ji_ptr GetClass( ji_ptr off )
{
    location_list       src;
    location_list       dst;
    address             src_addr;
    ji_ptr              clazz;
    dip_status          ds;

    memset( &src_addr, 0, sizeof( src_addr ) );
    src_addr.mach.offset = off;
    src_addr.mach.segment = MAD_JVM_FINDCLASS_SELECTOR;
    LocationCreate( &src, LT_ADDR, &src_addr );
    LocationCreate( &dst, LT_INTERNAL, &clazz );
    ds = DCAssignLocation( &dst, &src, sizeof( clazz ) );
    if( ds != DS_OK ) return( 0 );
    return( clazz );
}

dip_status GetLineCue( struct mad_jvm_findlinecue_acc *acc, struct mad_jvm_findline_ret *ret )
{
    location_list       src;
    location_list       dst;
    address             addr;
    dip_status          ds;

    memset( &addr, 0, sizeof( addr ) );
    addr.mach.segment = MAD_JVM_FINDLINECUE_SELECTOR;
    LocationCreate( &dst, LT_ADDR, &addr );
    LocationCreate( &src, LT_INTERNAL, acc );
    ds = DCAssignLocation( &dst, &src, sizeof( *acc ) );
    if( ds != DS_OK ) return( ds );
    LocationCreate( &src, LT_ADDR, &addr );
    LocationCreate( &dst, LT_INTERNAL, ret );
    return( DCAssignLocation( &dst, &src, sizeof( *ret ) ) );
}

dip_status      GetAddrCue( struct mad_jvm_findaddrcue_acc *acc, struct mad_jvm_findline_ret *ret )
{
    location_list       src;
    location_list       dst;
    address             addr;
    dip_status          ds;

    memset( &addr, 0, sizeof( addr ) );
    addr.mach.segment = MAD_JVM_FINDADDRCUE_SELECTOR;
    LocationCreate( &dst, LT_ADDR, &addr );
    LocationCreate( &src, LT_INTERNAL, acc );
    ds = DCAssignLocation( &dst, &src, sizeof( *acc ) );
    if( ds != DS_OK ) return( ds );
    LocationCreate( &src, LT_ADDR, &addr );
    LocationCreate( &dst, LT_INTERNAL, ret );
    return( DCAssignLocation( &dst, &src, sizeof( *ret ) ) );
}

unsigned NameCopy( char *buff, const char *src, unsigned buff_size, unsigned len )
{
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, src, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

void NormalizeClassName( char *name, unsigned len )
{
    unsigned    i;

    for( i = 0; i < len; ++i ) {
        switch( name[i] ) {
        case '/':
        case '\\':
            name[i] = '.';
            break;
        }
    }
}

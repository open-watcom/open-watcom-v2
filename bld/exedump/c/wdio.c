/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Executable dumper I/O routines.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <string.h>
#include <limits.h>
#include "wio.h"
#include "wdglb.h"
#include "wdfunc.h"

#include "clibext.h"


bool Weof( void )
/***************/
{
    long    cur_posn;
    long    eof_posn;

    cur_posn = lseek( Handle, 0, SEEK_CUR );
    if( cur_posn == -1L )
        return( true );
    eof_posn = lseek( Handle, 0, SEEK_END );
    if( lseek( Handle, cur_posn, SEEK_SET ) == -1L || eof_posn == -1L )
        return( true );
    return( cur_posn == eof_posn );
}

/*
 * read from the image
 */
static void chkread( unsigned_32 amount )
/***************************************/
{
    if( Num_read < amount ) {
        Wdputs( "Error! Couldn't read from executable: " );
        if( errno == 0 ) {
            Wdputs( "reached EOF" );
        } else {
            Wdputs( strerror( errno ) );
        }
        Wdputslc( ".\n" );
        longjmp( Se_env, 1 );
    }
}

/*
 * read from the image
 */
void Wread( void *buf, unsigned_32 amount )
/*****************************************/
{
    char    *buffer = buf;

    errno = 0;
    if( amount > Sizeleft ) {
        if( Sizeleft ) {
            memcpy( buffer, &Read_buff[Num_read - Sizeleft], Sizeleft );
            amount -= Sizeleft;
        }
        if( amount/BSIZE >= 1 ) {
            Num_read = read( Handle, &buffer[Sizeleft], amount );
            chkread( amount );
            Sizeleft = 0;
            Num_buff = FORCE_SEEK;  // must read in new buffer on next seek
        } else {
            Num_read = read( Handle, Read_buff, BSIZE );
            chkread( amount );
            memcpy( &buffer[Sizeleft], Read_buff, amount );
            Sizeleft = (unsigned_16)( Num_read - amount );
            Num_buff++;
        }
    } else {
        memcpy( buffer, &Read_buff[Num_read - Sizeleft], amount );
        Sizeleft -= (unsigned_16)amount;
    }
}

/*
 * lseek in the image
 */
void Wlseek( unsigned long offset )
/*********************************/
{
    unsigned long   nbuff;

    errno = 0;
    nbuff = offset / BSIZE;
    if( nbuff != Num_buff ) {
        if( lseek( Handle, nbuff * BSIZE, SEEK_SET ) == -1L ) {
            Wdputs( "Error! Couldn't seek in the executable: " );
            Wdputs( strerror( errno ) );
            Wdputslc( ".\n" );
            longjmp( Se_env, 1 );
        }
        Num_buff = (unsigned_16)nbuff;
        Num_read = read( Handle, Read_buff, BSIZE );
        chkread( offset % BSIZE );
    }
    Sizeleft = (unsigned_16)( Num_read - ( offset % BSIZE ) );
}

unsigned long WFileSize( void )
/*****************************/
{
    long    cur_posn;
    long    size;

    size = 0;
    cur_posn = lseek( Handle, 0, SEEK_CUR );
    if( cur_posn != -1L ) {
        size = lseek( Handle, 0, SEEK_END );
        lseek( Handle, cur_posn, SEEK_SET );
    }
    return( size );
}

/*
 * writeout a char
 */
void Wdputc( int c )
/******************/
{
    putchar( c );
}

/*
 * writeout a msg - part of a line
 */
void Wdputs( const char *buf )
/****************************/
{
    fputs( buf, stdout );
}

/*
 * writeout a msg - includes newline
 */
void Wdputslc( const char *buf )
/******************************/
{
    fputs( buf, stdout );
}

void Wdputname( const char *str )
/*******************************/
{
    char            name[256];
    unsigned_8      len;

    len = *str++;
    if( len ) {
        memcpy( name, str, len );
        name[len] = '\0';
        Wdputs( name );
    }
}

void Dump_namel( unsigned_8 len )
/*******************************/
{
    char            name[256];

    if( len ) {
        Wread( name, len );
        name[len] = '\0';
        Wdputs( name );
    }
}

unsigned_8 Dump_name( void )
/**************************/
{
    unsigned_8      len;

    Wread( &len, sizeof( len ) );
    Dump_namel( len );
    return( len );
}

unsigned Align_name( unsigned_8 len, unsigned_8 align )
/*****************************************************/
{
    unsigned_8  pad = 0;

    if( align && (len & ( align - 1 )) ) {
        pad = align - (len & ( align - 1 ));
        if( pad ) {
            lseek( Handle, pad, SEEK_CUR );
        }
    }
    return( len + pad );
}


void Dump_header( void *data_ptr, const_string_table *msg, int max_width )
/************************************************************************/
{
    unsigned_8  *data = (unsigned_8 *)data_ptr;
    int         skip;
    bool        print_h;
    static const char *spaces = "                ";

    for( ; *msg != NULL; ++msg ) {
        print_h = true;
        switch( msg[0][0] ) {
        case '1':
            Wdputs( &msg[0][1] );
            Wdputs( spaces + 2 * ( 8 - max_width + 1 ) );
            Puthex( *(unsigned_8 *)data, 2 );
            data += sizeof( unsigned_8 );
            break;
        case '2':
            Wdputs( &msg[0][1] );
            Wdputs( spaces + 2 * ( 8 - max_width + 2 ) );
            Puthex( *(unsigned_16 *)data, 4 );
            data += sizeof( unsigned_16 );
            break;
        case '4':
            Wdputs( &msg[0][1] );
            Wdputs( spaces + 2 * ( 8 - max_width + 4 ) );
            Puthex( *(unsigned_32 *)data, 8 );
            data += sizeof( unsigned_32 );
            break;
        case '8':
            Wdputs( &msg[0][1] );
            Puthex64( *(long long *)data, 16 );
            data += sizeof( long long );
            break;
        case '0':       // fixed size ASCIIZ string
            skip = msg[0][1] - '0';
            if( isdigit( msg[0][2] ) ) {
                skip = skip * 10 + msg[0][2] - '0';
                Wdputs( &msg[0][3] );
            } else {
                Wdputs( &msg[0][2] );
            }
            Wdputs( (char *)data );
            data += skip;
            print_h = false;
            break;
        default:
            Wdputs( &msg[0][1] );
            Wdputs( "*** invalid size" );
            break;
        }
        if( print_h ) {
            Wdputs( "H" );
        }
        Wdputslc( "\n" );
    }
}

#define MAX_FLAG_SIZE   256

void DumpFlags( unsigned_32 flags, unsigned_32 ignore, const_string_table *msg, const char *prefix )
/**************************************************************************************************/
{
    char            name[MAX_FLAG_SIZE];
    char            *currpos;
    unsigned_32     mask;
    size_t          len;

    Wdputs( "          flags = " );
    Puthex( flags, 8 );
    Wdputs( " :  " );
    flags &= ~ignore;
    mask = 1;
    currpos = name;
    len = strlen( prefix );
    if( len > 0 ) {
        memcpy( currpos, prefix, len );
        currpos += len;
        *currpos = '|';
        currpos++;
    }
    while( flags != 0 ) {
        if( flags & 1 ) {
            if( *msg == NULL ) {
                if( currpos - name + 9 > MAX_FLAG_SIZE )
                    break;
                currpos += sprintf( currpos, "?%x", mask );
            } else {
                len = strlen( *msg );
                if( currpos - name + len + 1 > MAX_FLAG_SIZE )
                    break;
                memcpy( currpos, *msg, len );
                currpos += len;
            }
            *currpos = '|';
            currpos++;
        }
        msg++;
        flags >>= 1;
        mask <<= 1;
    }
    if( currpos != name ) {
        currpos--;
    }
    *currpos = '\0';
    Wdputs( name );
    Wdputslc( "\n" );
}

#define min_len(a, b) ((a) < (b) ? (a) : (b))

/*
 * dump an arbitrarily long ASCIIZ string starting at specified offset
 */
void Dump_asciiz( unsigned long offset )
/**************************************/
{
    char            buf[65];
    unsigned long   fsize;
    unsigned_32     amount;


    Wlseek( offset );
    fsize = WFileSize() - offset;

    /* We must handle both arbitrarily long strings and strings that
     * are stored right at the end of the image.
     */
    amount = sizeof( buf ) - 1;
    do {
        if( amount > fsize )
            amount = fsize;
        Wread( buf, amount );
        buf[amount] = '\0';
        Wdputs( buf );
        /* Check if we read in a null terminator. */
        if( strlen( buf ) < amount )
            break;
        fsize -= amount;
    } while( fsize > 0 );
}

/*
 * allocate memory
 */
void * Wmalloc( size_t size )
/***************************/
{
    void    *start;

    start = malloc( size );
    if( start == NULL ) {
        Wdputslc( "Error! Dynamic memory exhausted.\n" );
        longjmp( Se_env, 1 );
    }
    return( start );
}

/*
 * print out a banner
 */
void Banner( const char *title )
/******************************/
{
    char    line[LINE_LEN + 1];
    size_t  title_len;

    line[LINE_LEN] = '\0';

    title_len = strlen( title );
    if( title_len > LINE_LEN ) {
        title_len = LINE_LEN;
    }

    memset( line, ' ', LINE_LEN );
    memcpy( line + ( LINE_LEN - title_len ) / 2, title, title_len );

    Wdputs( line );
    Wdputslc( "\n" );
    memset( line, '=', LINE_LEN );
    Wdputs( line );
    Wdputslc( "\n" );
}

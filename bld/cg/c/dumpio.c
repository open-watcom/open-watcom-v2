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


#include "cgstd.h"
#include <stdio.h>
#include "dumpio.h"

static  char    *dumpFileName = "cgdump.tmp";
static  FILE    *dumpFile;

void    DumpRedirect( void )
/**************************/
{
    if( dumpFile != NULL )
        return;
    dumpFile = fopen( dumpFileName, "wt" );
}

void    DumpUnredirect( void )
/****************************/
{
    if( dumpFile == NULL )
        return;
    fclose( dumpFile );
    dumpFile = NULL;
}

void    DumpChar( char c )
/************************/
{
    if( dumpFile != NULL ) {
        fputc( c, dumpFile );
    } else {
        fputc( c, stdout );
    }
}

void    DumpNL( void )
/********************/
{
    FILE        *fp;

    fp = (dumpFile != NULL) ? dumpFile : stdout;

    fputc( '\n', fp );
    fflush( fp );
}

void    DumpPadString( const char *s, int i )
/*******************************************/
{
    while( *s != '\0' ) {
        DumpChar( *s );
        ++s;
        --i;
    }
    while( --i >= 0 ) {
        DumpChar( ' ' );
    }
}


void    DumpString( const char *s )
/*********************************/
{
    while( *s != '\0' ) {
        DumpChar( *s );
        s++;
    }
}


void    DumpXString( char const *s )
/**********************************/
{
    DumpString( s );
}


void    DumpByte( byte n )
/************************/
{
    char        c;

    c = (n>>4) & 0x0f;
    if( c > 9 ) {
        c -= 10;
        c += 'A';
    } else {
        c += '0';
    }
    DumpChar( c );
    c = n & 0x0f;
    if( c > 9 ) {
        c -= 10;
        c += 'A';
    } else {
        c += '0';
    }
    DumpChar( c );
}

void    Dump8h( unsigned_32 n )
/*****************************/
{
    DumpByte( n >> 24 );
    DumpByte( n >> 16 );
    DumpByte( n >> 8  );
    DumpByte( n >> 0  );
}


static void _DumpLongLen( unsigned_32 n, int len, bool sign )
/***********************************************************/
{
    char        b[30];
    char        *bp;
    bool        neg;

    bp = b;
    bp += 20;
    *--bp = NULLCHAR;
    if( sign && (signed_32)n < 0 ) {
        neg = true;
        n = -(signed_32)n;
    } else {
        neg = false;
    }
    if( n != 0 ) {
        for(;;) {
            *--bp = n % 10 + '0';
            --len;
            n /= 10;
            if( n == 0 ) break;
        }
    } else {
        *--bp = '0';
        --len;
    }
    if( sign && neg ) {
        *--bp = '-';
        --len;
    }
    while( --len >= 0 ) {
        *--bp = ' ';
    }
    DumpXString( bp );
}

void    DumpLongLen( signed_32 n, int len )
/*****************************************/
{
    _DumpLongLen( n, len, true );
}

void    DumpLong( signed_32 n )
/*****************************/
{
    _DumpLongLen( n, 0, true );
}

void    DumpInt( int n )
/**********************/
{
    DumpLong( n );
}

void    DumpULongLen( unsigned_32 n, int len )
/********************************************/
{
    _DumpLongLen( n, len, false );
}

void    DumpULong( unsigned_32 n )
/********************************/
{
    _DumpLongLen( n, 0, false );
}

void    DumpUInt( unsigned n )
/****************************/
{
    DumpULong( n );
}

void    DumpId( unsigned id )
/***************************/
{
    DumpChar( '(' );
    DumpLongLen( id, 8 );
    DumpChar( ')' );
}

void    DumpPtr( void *ptr )
/**************************/
{
    Dump8h( (unsigned_32)(pointer_int)ptr );
}

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
* Description:  List module names defined in an object file.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "watcom.h"
#include "pcobj.h"
#include "misc.h"

typedef unsigned char byte;
typedef byte *data_ptr;

static data_ptr    NamePtr;
static byte        NameLen;
static unsigned_16 RecLen;
static data_ptr    RecBuff;
static data_ptr    RecPtr;
static unsigned_16 RecMaxLen;

static void usage( void )
/***********************/
{
    printf( "Usage: objlist <list of object or library files>\n" );
}

static byte GetByte( void )
/*************************/
{
    byte        ret;

    ret = *RecPtr;
    RecPtr++;
    return( ret );
}

static byte *GetName( void )
/**************************/
{
    NameLen = GetByte();
    NamePtr = RecPtr;
    RecPtr += NameLen;
    return( NamePtr );
}

static void ResizeBuff( unsigned_16 reqd_len )
/********************************************/
{
    if( reqd_len > RecMaxLen ) {
        RecMaxLen = reqd_len;
        if( RecBuff != NULL ) {
            free( RecBuff );
        }
        RecBuff = malloc( RecMaxLen );
        if( RecBuff == NULL ) {
            printf( "**FATAL** Out of memory!\n" );
            exit( -1 );
        }
    }
}

static void ProcFile( FILE *fp )
/******************************/
{
    byte        hdr[ 3 ];
    unsigned_16 page_len;
    unsigned_32 offset;

    page_len = 0;
    RecBuff = NULL;
    RecMaxLen = 0;
    for(;;) {
        offset = ftell( fp );
        if( fread( hdr, 1, 3, fp ) != 3 )
            break;
        RecLen = hdr[ 1 ] | ( hdr[ 2 ] << 8 );
        ResizeBuff( RecLen );
        RecPtr = RecBuff;
        if( fread( RecBuff, RecLen, 1, fp ) == 0 )
            break;
        switch( hdr[ 0 ] & ~1 ) {
        case CMD_THEADR:
            GetName();
            *RecPtr = 0;
            printf( "%s\n", NamePtr );
            break;
        case CMD_MODEND:
            if( page_len != 0 ) {
                offset = ftell( fp );
                offset = page_len - offset % page_len;
                if( offset != page_len ) {
                    fseek( fp, offset, SEEK_CUR );
                }
            }
            break;
        case LIB_HEADER_REC:
            if( hdr[ 0 ] & 1 ) {
                fseek( fp, 0L, SEEK_END );
                page_len = 0;
            } else {
                page_len = RecLen + 3;
            }
            break;
        default:
            break;
        }
    }
    free( RecBuff );
}

static int process_file_mod( char *filename )
/*******************************************/
{
    FILE    *fp;

    fp = fopen( filename, "rb" );
    if( fp == NULL ) {
        printf( "Cannot open input file: %s.\n", filename );
        return( 0 );
    }
    ProcFile( fp );
    fclose( fp );
    return( 1 );
}

int main( int argc, char *argv[] )
/********************************/
{
    int     i;
    char    *fn;

    if( argc == 1 ) {
        usage();
        return( 1 );
    }
    for( i = 1; i < argc; ++i ) {
        fn = DoWildCard( argv[i] );
        while( fn != NULL ) {
            process_file_mod( fn );
            fn = DoWildCard( NULL );
        }
        DoWildCardClose();
    }
    return( 0 );
}

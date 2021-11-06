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
* Description:  file i/o routines for the microsoft linker file translator
*
****************************************************************************/


#include <errno.h>
#include <stdio.h>      /* for SEEK_SET/SEEK_END */
#include <string.h>
#include "wio.h"
#include "ms2wlink.h"

#include "clibext.h"


// file io routines

static void IOError( char *msgstart, const char *name )
/*****************************************************/
{
    ErrorOut( msgstart );
    ErrorOut( name );
    ErrorOut( ": " );
    ErrorExit( strerror( errno ) );
}

FILE *QOpenR( const char *name )
/******************************/
{
    FILE    *fp;

    fp = fopen( name, "rb" );
    if( fp != NULL ) {
        return( fp );
    }
    IOError( "can't open ", name );
    return( NULL );
}

size_t QRead( FILE *fp, void *buffer, size_t len, const char *name )
/***********************************************************************/
{
    size_t  ret;

    ret = fread( buffer, 1, len, fp );
    if( ret == IOERROR ) {
        IOError( "io error processing ", name );
    }
    return( ret );
}

size_t QWrite( FILE *fp, const void *buffer, size_t len, const char *name )
/*************************************************************************/
/* write from far memory */
{
    size_t  ret;

    if( len == 0 )
        return( 0 );

    ret = fwrite( buffer, 1, len, fp );
    if( ret == IOERROR ) {
        IOError( "io error processing ", name );
    }
    return( ret );
}

void QWriteNL( FILE *fp, const char *name )
/**********************************************/
{
    QWrite( fp, "\n", 1, name );
}

void QClose( FILE *fp, const char *name )
/***************************************/
/* file close */
{
    if( fclose( fp ) ) {
        IOError( "io error processing ", name );
    }
}

unsigned long QFileSize( FILE *fp )
/*********************************/
{
    long            curpos;
    unsigned long   size;

    curpos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size = ftell( fp );
    fseek( fp, curpos, SEEK_SET );
    return( size );
}

bool QReadStr( FILE *fp, char *dest, size_t size, const char *name )
/***********************************************************************/
/* quick read string (for reading directive file) */
{
    bool            eof;
    char            ch;
    size_t          len;

    eof = false;
    while( --size > 0 ) {
        len = QRead( fp, &ch, 1, name );
        if( len == 0 || size == IOERROR ) {
            eof = true;
            break;
        } else if( ch != '\r' ) {
            *dest++ = ch;
        }
        if( ch == '\n' ) {
            break;
        }
    }
    *dest = '\0';
    return( eof );
}

bool QIsConIn( FILE *fp )
/***********************/
{
    return( isatty( fileno( fp ) ) );
}

// routines based on the "quick" file i/o routines.

void ErrorOut( const char *msg )
/******************************/
{
    QWrite( stderr, msg, strlen( msg ), "console" );
}

void ErrorExit( const char *msg )
/*******************************/
{
    QWrite( stderr, msg, strlen( msg ), "console" );
    QWriteNL( stderr, "console" );
    Suicide();
}

void CommandOut( const char *command )
/************************************/
{
    printf( "%s\n", command );
}

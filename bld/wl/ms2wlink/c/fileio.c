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


/*
 *  FILEIO : file i/o routines for the microsoft linker file translator
 *
*/

#include <errno.h>
#include <stdio.h>      /* for SEEK_SET/SEEK_END */
#include <string.h>
#include "wio.h"
#include "ms2wlink.h"

static bool     DeleteMsg = FALSE;

extern char *       Msg3Splice( char *, char *, char * );
extern char *       Msg2Splice( char *, char * );
extern void         Suicide( void );
extern void         MemFree( void * );

/* forward prototype */
extern void Error( char * msg );

// file io routines

static void IOError( char *msgstart, char *name )
/***********************************************/
{
    char *  tempmsg;
    char *  realmsg;

    DeleteMsg = TRUE;
    tempmsg = Msg3Splice( msgstart, name, ": " );
    realmsg = Msg2Splice( tempmsg, strerror( errno ) );
    MemFree( tempmsg );
    Error( realmsg );
}

extern f_handle QOpenR( char *name )
/**********************************/
{
    f_handle h;

    h = open( name, O_RDONLY | O_BINARY );
    if( h >= 0 ) {
        return( h );
    }
    IOError( "can't open ", name );
    return( NIL_HANDLE );
}

extern unsigned QRead( f_handle file, void *buffer, unsigned len, char *name )
/****************************************************************************/
{
    int ret;

    ret = read( file, buffer, len );
    if( ret == -1 ) {
        IOError( "io error processing ", name );
    }
    return( ret );
}

extern unsigned QWrite( f_handle file, void *buffer, unsigned len, char *name )
/*****************************************************************************/
/* write from far memory */
{
    int ret;

    if( len == 0 ) return( 0 );

    ret = write( file, buffer, len );
    if( ret == -1 ) {
        IOError( "io error processing ", name );
    }
    return( ret );
}

extern void QWriteNL( f_handle file, char *name )
/***********************************************/
{
    QWrite( file, "\n", 1, name );
}

extern void QClose( f_handle file, char *name )
/*********************************************/
/* file close */
{
    int ret;

    ret = close( file );
    if( ret != -1 ) return;
    IOError( "io error processing ", name );
}

static unsigned long QPos( f_handle file )
/****************************************/
{
    return( tell( file ) );
}

extern unsigned long QFileSize( f_handle file )
/*********************************************/
{
    unsigned long   curpos;
    unsigned long   size;

    curpos = QPos( file );
    size = lseek( file, 0L, SEEK_END );
    lseek( file, curpos, SEEK_SET );
    return( size );
}

extern bool QReadStr( f_handle file, char *dest, unsigned size, char *name )
/**************************************************************************/
/* quick read string (for reading directive file) */
{
    bool            eof;
    char            ch;

    eof = FALSE;
    while( --size > 0 ) {
        if( QRead( file, &ch, 1, name ) == 0 ) {
            eof = TRUE;
            break;
        } else if( ch != '\r' ) {
            *dest++ = ch;
        }
        if( ch == '\n' ) break;
    }
    *dest = '\0';
    return( eof );
}

extern bool QIsConIn( f_handle file )
/***********************************/
{
    return( isatty( file ) );
}

// routines based on the "quick" file i/o routines.

extern void Error( char * msg )
/*****************************/
{
    QWrite( STDERR_HANDLE, msg, strlen( msg ), "console" );
    QWriteNL( STDERR_HANDLE, "console" );
    if( DeleteMsg ) {
        MemFree( msg );
    }
    Suicide();
}

extern void CommandOut( char *command )
/*************************************/
{
    QWrite( STDOUT_HANDLE, command, strlen( command ), "console" );
    QWriteNL( STDOUT_HANDLE, "console" );
}

extern void QSetBinary( f_handle file )
/*************************************/
{
    setmode( file, O_BINARY );
}

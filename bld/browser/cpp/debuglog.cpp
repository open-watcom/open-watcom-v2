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


#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <wclist.h>
#include <wcstack.h>

#include "debuglog.h"

static FILE * OpenFile( const char * name, bool append );
static char * AppendSlash( char * fileName );
static void   PrintHeader( const char * name, FILE * fp );

DebuggingLog::DebuggingLog( const char * name, bool append )
                : _fp( stderr )
                , _times( NULL )
//----------------------------------------------------------
// open an output file using name. append is a boolean, true if
// the file is to be opened in append mode
//
// since this class is often used as a static member which might
// be instantiated before the memory tracker is initialized,
// avoid doing any 'new's in the ctor
{
    if( name != NULL ) {
        _fp = OpenFile( name, append );     // override from stderr
    }

    if( _fp == NULL ) {
        fprintf( stderr, "%s: %s (error %d)\n\n", name, strerror( errno ), errno );
        _fp = stderr;
    }

    PrintHeader( name, _fp );
}

DebuggingLog::~DebuggingLog()
//---------------------------
{
    fprintf( _fp, "\n\n<log end> %s\n", DebuggingLog::timeStamp() );

    if( _times ) {
        _times->clear();
        delete _times;
        _times = NULL;
    }
}

int DebuggingLog::printf( const char * format, ... )
//--------------------------------------------------
// normal printf to the file
{
    va_list arglist;
    int     rc;

    va_start( arglist, format );
    rc = vfprintf( _fp, format, arglist );
    va_end( arglist );
    fflush( _fp );

    if( rc < 0 ) {
        throw WriteError;
    }

    return rc;
}

void DebuggingLog::write( const char * buffer, int len )
//------------------------------------------------------
// write a straight buffer out.  I don't know a good
// use for this, but trmem uses it, so...
{
    int numWritten;

    numWritten = fwrite( buffer, len, 1, _fp );
    fflush( _fp );

    if( numWritten != 1 ) {
        throw WriteError;
    }
}

void DebuggingLog::puts( const char * buffer )
//--------------------------------------------
// normal fputs to the file
{
    int rc;

    rc = fputs( buffer, _fp );
    fflush( _fp );

    if( rc ) {
        throw WriteError;
    }
}

static char * DebuggingLog::timeStamp()
//-------------------------------------
// writes a time / date stamp then a newline
{
    time_t  timeOfDay;

    timeOfDay = time( NULL );
    return ctime( &timeOfDay );
}

static char * AppendSlash( char * fileName )
//------------------------------------------
// append slash to end of fileName if there isn't one there.
// assumes there is room to appen a character.
{
    int len;

    len = strlen( fileName );
    if( fileName[ len - 1 ] != '\\' && fileName[ len - 1 ] != '/' ) {
        fileName[ len ] = '\\';
        fileName[ len + 1 ] = '\0';
    }

    return fileName;
}

/*
 * openFile first searches for the given name as an environment
 * variable.  If it is not found as an environment variable, it searches
 * for a %log_dir% environment variable, and opens a file called "name"
 * under that directory.  If there is no log_dir variable, it uses %tmp%,
 * failing that, the current directory
 */

const char * LogDirEnvVar = "log_dir";
const char * TmpEnvVar = "tmp";

FILE * OpenFile( const char * name, bool append )
//-----------------------------------------------
{
    char    fileName[ _MAX_PATH ];
    char *  foundName;
    FILE *  fp;

    foundName = getenv( name );

    if( foundName != NULL ) {
        strcpy( fileName, foundName );
    } else {
        foundName = getenv( LogDirEnvVar );
        if( foundName != NULL ) {
            strcpy( fileName, foundName );
            AppendSlash( fileName );
            strncat( fileName, name, _MAX_FNAME - 1 );
            strcat( fileName, ".log" );
        } else {
            foundName = getenv( TmpEnvVar );
            if( foundName != NULL ) {
                strcpy( fileName, foundName );
                AppendSlash( fileName );
                strncat( fileName, name, _MAX_FNAME );
                strcat( fileName, ".log" );
            } else {
                strcpy( fileName, name );           // give up, use name
            }
        }
    }

    if( append ) {
        fp = fopen( fileName, "at" );               // append to end
    } else {
        fp = fopen( fileName, "wt" );
    }

    return fp;
}

static void PrintHeader( const char * name, FILE * fp )
//-----------------------------------------------------
{
    int     nChars;
    int     i;

    nChars = fprintf( fp, "%s:  %s", name, DebuggingLog::timeStamp() );

    for( i = 0; i < nChars; i += 1 ) {
        fputc( '-', fp );
    }

    fputs( "\n", fp );
}

void DebuggingLog::startTiming()
//------------------------------
{
    clock_t  timeOfDay;

    timeOfDay = clock();


    if( _times == NULL ) {
        _times = new WCStack< uint_32, WCValSList< uint_32 > >;
    }

    _times->push( timeOfDay );
}

double DebuggingLog::endTiming()
//------------------------------
{
    clock_t  oldTime;
    clock_t  newTime;

    if( _times->isEmpty() ) {
        return 0.0;
    } else {
        oldTime = _times->pop();
        newTime = clock();

        return (double)(newTime - oldTime) / CLOCKS_PER_SEC;
    }
}

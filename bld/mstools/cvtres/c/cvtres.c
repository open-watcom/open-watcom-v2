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
* Description:  Microsoft CVTRES clone tool.
*
****************************************************************************/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"
#include "error.h"
#include "file.h"
#include "memory.h"
#include "message.h"
#include "optparse.h"
#include "parse.h"
#include "pathconv.h"


#define CVTRES_SUCCESS          0
#define CVTRES_NOACTION         (-1)
#define CVTRES_ERROR            (-2)


/*
 * Top-level parsing routine.  Returns the number of items parsed.
 */
static int do_parsing( OPT_STORAGE *cmdOpts )
/*******************************************/
{
    int                 itemsParsed = 0;

    /*** Process the WATCOM_CLONE_OPTIONS environment variable ***/
    if( OpenEnvironContext( "WATCOM_CLONE_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the CVTRES_OPTIONS environment variable ***/
    if( OpenEnvironContext( "CVTRES_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the CVTRES environment variable ***/
    if( OpenEnvironContext( "CVTRES" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the command line ***/
    OpenCmdLineContext();
    CmdStringParse( cmdOpts, &itemsParsed );

    return( itemsParsed );
}


/*
 * Convert the resource file.
 */
static int res_convert( const OPT_STORAGE *cmdOpts )
/**************************************************/
{
    const size_t        bufsize = 32768;
    void *              buf;
    char *              infilename;
    char                outfilename[_MAX_PATH];
    char                drive[_MAX_DRIVE];
    char                dir[_MAX_DIR];
    char                fname[_MAX_FNAME];
    FILE *              in;
    FILE *              out;
    long                bytes;
    size_t              amount;
    size_t              rc;
    char *              p;

    /*** Get the name of the .res file to compile ***/
    infilename = GetNextFile( NULL, TYPE_RES_FILE, TYPE_INVALID_FILE );
    if( infilename == NULL )  return( CVTRES_NOACTION );
    infilename = PathConvert( infilename, '\"' );
    if( GetNextFile( NULL, TYPE_RES_FILE, TYPE_INVALID_FILE )  !=  NULL ) {
        FatalError( "Can only convert one file at a time" );
    }

    /*** Determine the name of the output file ***/
    strcpy( outfilename, "" );
    if( cmdOpts->o_value != NULL ) {            /* /O used */
        p = PathConvert( cmdOpts->o_value->data, '\"' );
        strcpy( outfilename, p );
        FreeMem( p );
    }
    if( cmdOpts->out_value != NULL ) {          /* /OUT used (overrides /O) */
        p = PathConvert( cmdOpts->out_value->data, '\"' );
        strcpy( outfilename, p );
        FreeMem( p );
    }
    if( !strcmp( outfilename, "" ) ) {          /* based on input filename */
        _splitpath( infilename, drive, dir, fname, NULL );
        _makepath( outfilename, drive, dir, fname, ".obj" );
    }


    if( cmdOpts->showwopts ) {
        fprintf( stderr, "copy %s %s\n", infilename, outfilename );
    }

    if( !cmdOpts->noinvoke ) {
        /*** Prepare to convert (copy) the file ***/
        buf = AllocMem( bufsize );
        in = fopen( infilename, "rb" );
        if( in == NULL )  FatalError( "Cannot open '%s'", infilename );
        out = fopen( outfilename, "wb" );
        if( out == NULL )  FatalError( "Cannot create '%s'", outfilename );
        bytes = filelength( fileno( in ) );
        if( bytes == -1L )  FatalError( "Cannot get size of '%s'", infilename );

        /*** Convert (copy) the file ***/
        while( bytes > 0L ) {
            amount = ( bytes >= bufsize )  ?  bufsize  :  (size_t)bytes;
            rc = fread( buf, amount, 1, in );
            if( rc == 0 )  FatalError( "Cannot read from '%s'", infilename );
            rc = fwrite( buf, amount, 1, out );
            if( rc == 0 )  FatalError( "Cannot write to '%s'", outfilename );
            bytes -= amount;
        }
        fclose( in );
        fclose( out );
    }

    return( CVTRES_SUCCESS );
}


/*
 * Program entry point.
 */
void main( int argc, char *argv[] )
/*********************************/
{
    OPT_STORAGE         cmdOpts;
    int                 itemsParsed;
    int                 rc = CVTRES_NOACTION;

    /*** Initialize ***/
    SetBannerFuncError( BannerMessage );
    SetDefaultFile( TYPE_RES_FILE, "res" );
    AllowTypeFile( TYPE_RES_FILE, TYPE_INVALID_FILE );

    /*** Parse the command line ***/
    InitParse( &cmdOpts );
    itemsParsed = do_parsing( &cmdOpts );
    if( itemsParsed==0 || cmdOpts.help ) {
        PrintHelpMessage();
        exit( EXIT_SUCCESS );
    }

    /*** Do the conversion ***/
    rc = res_convert( &cmdOpts );
    switch( rc ) {
      case CVTRES_ERROR:
        exit( EXIT_FAILURE );
        break;
      case CVTRES_NOACTION:
        FatalError( "Nothing to do!" );
        break;
      case CVTRES_SUCCESS:
        FiniParse( &cmdOpts );
        exit( EXIT_SUCCESS );
        break;
      default:
        Zoinks();
    }
}

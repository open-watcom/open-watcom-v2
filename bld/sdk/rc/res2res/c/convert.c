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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "param.h"
#include "wresall.h"
#include "m2wres.h"
#include "w2mres.h"
#include "convert.h"

#define TMP_FILENAME "__TMP__.RES"

static int ConvertFileMResToWRes( WResFileID infile )
/***************************************************/
{
    WResFileID      tempfile;
    int             error;

    tempfile = WResOpenNewFile( TMP_FILENAME );
    if (tempfile == -1) {
        perror( "Error (temp file): " );
        ResCloseFile( infile );
        return( TRUE );
    }
    /* put a message out if quiet option not selected */
    if (!CmdLineParms.Quiet) {
        puts( "Converting Microsoft .RES to Open Watcom .RES" );
    }
    error = ConvertMResToWRes( infile, tempfile );

    ResCloseFile( tempfile );

    return( error );
} /* ConvertFileMResToWRes */

static int ConvertFileWResToMRes( WResFileID infile )
/***************************************************/
{
    WResFileID      tempfile;
    int             error;

    tempfile = MResOpenNewFile( TMP_FILENAME );
    if (tempfile == -1) {
        perror( "Error (temp file): " );
        ResCloseFile( infile );
        return( TRUE );
    }
    /* put a message out if quiet option not selected */
    if (!CmdLineParms.Quiet) {
        puts( "Converting Open Watcom .RES to Microsoft .RES" );
    }
    error = ConvertWResToMRes( infile, tempfile );

    ResCloseFile( tempfile );
    return( error );
} /* ConvertFileWResToMRes */

static int ChangeTmpToOutFile( void )
/***********************************/
{
    int             fileerror;      /* error while deleting or renaming */

    /* remove the old copy of the output file */
    fileerror = remove( CmdLineParms.OutFileName );
    if (fileerror) {
        if (errno == ENOENT) {
            /* ignore the error if it says that the file doesn't exist */
            errno = 0;
        } else {
            perror( NULL );
            return( TRUE );
        }
    }
    /* rename the temp file to the output file */
    fileerror = rename( TMP_FILENAME, CmdLineParms.OutFileName );
    if (fileerror) {
        /* probably should check if errno is EXDEV at this point and copy */
        /* file if it is */
        perror( NULL );
        return( TRUE );
    }

    return( FALSE );
} /* ChangeTmpToOutFile */


int ConvertFiles( void )
/**********************/
{
    WResFileID      infile;
    int             error;
    int             fileerror;      /* error while deleting or renaming */

    infile = ResOpenFileRO( CmdLineParms.InFileName );
    if (infile == -1) {
        perror( "Error (input file): " );
        return( TRUE );
    }

    if (WResIsWResFile( infile )) {
        /* the input file is in Open Watcom format so convert to MS format */
        error = ConvertFileWResToMRes( infile );
    } else {
        /* the input file is in MS format so convert to Open Watcom format */
        error = ConvertFileMResToWRes( infile );
    }

    ResCloseFile( infile );

    if (error) {
        puts("Error writing to output file");
        fileerror = remove( TMP_FILENAME );
        if (fileerror) {
            perror( NULL );
        }
    } else {
        error = ChangeTmpToOutFile();
    }

    return( error );
} /* ConvertFiles */

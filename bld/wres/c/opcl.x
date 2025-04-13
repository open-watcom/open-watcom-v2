/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Routines to open and close WatCOM  .RES  files.
*
****************************************************************************/


#include <fcntl.h>
#include <sys/stat.h>
#include "layer0.h"
#include "filefmt.h"
#include "resfmt.h"
#include "mresfmt.h"
#include "read.h"
#include "write.h"
#include "opcl.h"


WResFileID WResOpenNewFile( const char * filename )
/*************************************************/
{
    WResFileID      newhandle;

    newhandle = WRESOPEN( filename, O_CREAT | O_WRONLY | O_TRUNC
                        | O_BINARY, S_IWRITE | S_IREAD );
    if (newhandle != -1) {
        WResFileInit( newhandle );
    }

    return( newhandle );
}

WResFileID MResOpenNewFile( const char * filename )
/*************************************************/
{
    return( WRESOPEN( filename, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD ) );
}

int WResFileInit( WResFileID handle )
/************************************
 * Writes the initial file header out to the file. Later, when WResWriteDir
 * is called the real header will be written out
 */
{
    WResHeader  head;
    int         error;

    head.Magic[0] = WRESMAGIC0;
    head.Magic[1] = WRESMAGIC1;
    head.DirOffset = 0;
    head.NumResources = 0;
    head.NumTypes = 0;
    head.WResVer = WRESVERSION;

    /* write the empty record out at the begining of the file */
    error = WRESSEEK( handle, 0, SEEK_SET );
    if (!error) {
        error = WResWriteHeaderRecord( &head, handle );
    }

    return( error );
} /* WResInitFile */

WResFileID  ResOpenFileRO( const char * filename )
/*************************************************
 * use this function to open Microsoft .RES files also
 */
{
    return( WRESOPEN( filename, O_RDONLY | O_BINARY ) );
}

WResFileID  ResOpenFileRW( const char * filename )
/************************************************/
{
    return( WRESOPEN( filename, O_CREAT | O_RDWR | O_BINARY, S_IWRITE | S_IREAD ) );
}

int ResCloseFile( WResFileID handle )
/***********************************/
{
    return( WRESCLOSE( handle ) );
}

int WResCheckWResFile( const char * filename )
/********************************************/
{
    int         iswresfile;
    WResFileID  handle;

    handle = ResOpenFileRO( filename );

    if (handle == -1) {
        return( FALSE );        /* assume it's not a WRES file */
    }

    iswresfile = WResIsWResFile( handle );

    ResCloseFile( handle );

    return( iswresfile );
}

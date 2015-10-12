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


#include <sys/types.h>
#include <sys/stat.h>
#include "bdiff.h"
#include "wpatchio.h"
#include "wpatch.h"
#include "patchio.h"
#include "msg.h"

FILE        *PatchF;
const char  *PatchName;

/************************************************************************
The following functions are used by the wpatch utility to create patches,
and then to read in the created patches.
*************************************************************************/

void PatchWriteOpen( const char *patch_name )
{
    PatchName = patch_name;
    PatchF = fopen( patch_name, "wb" );
    if( PatchF == NULL ) {
        FilePatchError( ERR_CANT_OPEN, patch_name );
    }
}


void PatchReadOpen( const char *patch_name )
{
    PatchName = patch_name;
    PatchF = fopen ( patch_name, "rb" );
    if ( PatchF == NULL ) {
        FilePatchError( ERR_CANT_OPEN, patch_name );
    }
}

void PatchWriteClose( void )
{
    fclose( PatchF );
}

void PatchReadClose( void )
{
    fclose( PatchF );
}

void PatchWriteFile( short flag, const char *RelPath )
{
    fwrite( &flag, sizeof(short), 1, PatchF );
    fputs( RelPath, PatchF );
    fputc( '\n', PatchF );
}

void PatchReadFile( short *Pflag, char *RelPath )
{
    fread( Pflag, sizeof(short), 1, PatchF );
    if (feof( PatchF)) {
        *Pflag = PATCH_EOF;
        return;
    }
    fgets( RelPath, PATCH_MAX_PATH_SIZE, PatchF );
    RelPath[strlen( RelPath ) - 1] = '\0'; /* remove newline char. */
}


void PatchAddFile( const char *path )
{
    FILE *inF;
    struct stat filestats;
    char filechar;

    inF = fopen( path, "rb" );
    if ( fstat( fileno( inF ), &filestats ) == -1 ) {
        printf( "Error opening file %s.\n", path );
        exit( -1 );
    }
    fwrite( &filestats.st_size, sizeof(off_t), 1, PatchF );
    for( ;; ) {
        filechar = fgetc( inF );
        if (feof( inF )) break;
        fputc( filechar, PatchF );
    }
    fclose( inF );
}

void PatchGetFile( const char *path )
{
    FILE *outF;
    char filechar;
    off_t filesize;
    off_t count;

    outF = fopen( path, "rb" );
    if ( outF == NULL ) {
        printf( "Error opening file %s.\n", path );
        exit( -1 );
    }
    fread( &filesize, sizeof(off_t), 1, PatchF );
    count = filesize;
    while ( count > 0 ) {
        filechar = fgetc( PatchF );
        fputc( filechar, outF );
        count -= 1;
    }
    fclose( outF );
}


void PatchWrite( void *patch, int size )
{
    fwrite( patch, size, 1, PatchF );
}

PATCH_RET_CODE OpenPatch()
{
    return( PATCH_RET_OKAY );
}

void ClosePatch()
{
}

PATCH_RET_CODE InputPatch( byte *tmp, size_t len )
{
    if( fread( tmp, len, 1, PatchF ) != 1 ) {
        FilePatchError( ERR_CANT_READ, PatchName );
        return( PATCH_CANT_READ );
    }
    return( PATCH_RET_OKAY );
}

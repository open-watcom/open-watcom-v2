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


// -- file merging code -----------------------------------------------------
// Copyright (c) WATCOM Systems Inc., 1992. All rights reserved.

// wbrfile.cpp - implementation of function createBrowseFile

//  Modified    By              Reason
//  ========    ==              ======
//  92/09/16    Paul Harapiak   Initial implementation.

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <process.h>

int main( int argc, char * argv[] )
{
    FILE * in;

    if( argc != 2 ) {
        fprintf( stderr, "Usage: %s <filename>", argv[0] );
        return( 1 );
    }

    in = fopen( argv[1], "wb" );
    assert( in != NULL );

    createBrowseFile( in );
    return( 0 );
}

void CFatal( char * msg )
{
    fprintf( stderr, msg );
    exit( 1 );
}

/*
    createBrowseFile merges the component files along with a header
    into the target browseFile.
    The resulting file will be of a format readable by the WATCOM Browser.
    The return value is the value of errorno if no errors are encountered.

    The input files are assumed open for read - they will be rewound and
        left open, positioned at the end of the file

    The output files is assumed open for write and empty - it will be
        left open, positioned at the end of the file
*/

/* output DWARF sections in following order
 * .debug_info
 * .WATCOM_reference
 * .debug_abbrev
 * .debug_line
 * .debug_macinfo
 */
static const char *inFileNames[] = { "info.dw", "ref.dw", "abbrev.dw", "line.dw", "macinfo.dw" };
#define SECTION_COUNT   (sizeof( inFileNames ) / sizeof( inFileNames[0] ))

int createBrowseFile( FILE *browseFile )
{
    const unsigned char wbrHeaderSignature[] = { 0xcb, 0xcb };
    const char          *wbrHeaderString = "WBROWSE";
    const char          wbrHeaderVersion = '1';
    const long          bufSize = 1024;
    char                buf[1024];
    size_t              trySize,wroteSize,readSize;
    int                 fileNum, i;
    long                startSectionDesc;
    FILE                *inFiles[SECTION_COUNT];
    unsigned long       sectionSize[SECTION_COUNT];
    unsigned long       sectionOffset[SECTION_COUNT];

    rewind( browseFile );

    // insert signature (lsb first)
    if( fputc( wbrHeaderSignature[0], browseFile ) == EOF ) {
        return( errno );
    }
    if( fputc( wbrHeaderSignature[1], browseFile ) == EOF ) {
        return( errno );
    }

    // insert readable string and version
    sprintf( buf, "%s%c", wbrHeaderString, wbrHeaderVersion );
    trySize = strlen( buf );
    wroteSize = fwrite( buf, 1, trySize, browseFile );
    if( wroteSize < trySize )
        return( errno );

    startSectionDesc = ftell( browseFile );

    // leave space for adding section offsets and sizes later
    sprintf( buf, "1111aaaa2222bbbb3333cccc4444dddd5555eeee" );
    trySize = strlen( buf );
    wroteSize = fwrite( buf, 1, trySize, browseFile );
    if( wroteSize < trySize )
        return( errno );

    // write each of the 5 sections, tracking size and offset
    for( fileNum = 0; fileNum < SECTION_COUNT; fileNum++ ) {
        inFiles[fileNum] = fopen( inFileNames[fileNum], "rb" );
        if( inFiles[fileNum] == NULL ) {
            puts( strerror( errno ) );
            puts( inFileNames[fileNum] );
            CFatal( "dwarf: unable to open file for concatenate" );
        }
        sectionSize[fileNum] = 0;
        sectionOffset[fileNum] = ftell( browseFile );
        rewind(inFiles[fileNum]);

        do {
            readSize = fread( buf, 1, bufSize, inFiles[fileNum] );
            sectionSize[fileNum] += readSize;
            wroteSize = fwrite( buf, 1, readSize, browseFile );
        } while( readSize == bufSize && wroteSize == readSize );

        if( readSize > wroteSize )
            return( errno );
        if( fclose( inFiles[fileNum] ) ) {
            puts( strerror( errno ) );
            puts( inFileNames[fileNum] );
            CFatal( "dwarf: unable to close file after concatenate" );
        }
    }

    // go back and fill in section offsets and sizes
    fseek( browseFile, startSectionDesc, SEEK_SET /*from start of file*/);
    for( fileNum = 0; fileNum < SECTION_COUNT; fileNum++ ) {
        unsigned char   *p;

        p = (unsigned char *)&sectionOffset[fileNum];
        for( i = 0; i < 4; i++ ) {
            if( fputc( *p++, browseFile ) == EOF ) {
                return( errno );
            }
        }
        p = (unsigned char *)&sectionSize[fileNum];
        for( i = 0; i < 4; i++ ) {
            if( fputc( *p++, browseFile ) == EOF ) {
                return( errno );
            }
        }
    }
    return( 0 );
}
//---------------------------------------------------------------------------

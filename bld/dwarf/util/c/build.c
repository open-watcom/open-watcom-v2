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
        fprintf( stderr, "Usage: %s <filename>", argv[ 0 ] );
        exit( 1 );
    }

    in = fopen( argv[ 1 ], "wb" );
    assert( in != NULL );

    createBrowseFile( in, "info.dw", "ref.dw", "abbrev.dw", "line.dw",
                      "macinfo.dw" );
    return 0;
}

void CFatal( char * msg ) {
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

int createBrowseFile(FILE* browseFile,      /* target file */
                      char* debugFile,      /* .debug_info section */
                      char* referenceFile,   /* .WATCOM_reference section */
                      char* abbrevFile,     /* .debug_abbrev section */
                      char* lineFile,       /* .debug_line section */
                      char* macroFile       /* .debug_macinfo section */
                      )
{
    const unsigned int wbrHeaderSignature = 0xcbcb;
    const char* wbrHeaderString = "WBROWSE";
    const char wbrHeaderVersion = '1';
    const long bufSize = 1024;
    char buf[1024];
    size_t trySize,wroteSize,readSize;
    int fileNum, i;
    long startSectionDesc;
    char* inFileNames[5];
    FILE* inFiles[5];
    unsigned long sectionSize[5];
    unsigned long sectionOffset[5];

    rewind(browseFile);

    // insert signature (lsb first)
    if (fputc((unsigned char)wbrHeaderSignature,browseFile) == EOF) {
        return errno;
    }
    if (fputc((unsigned char)(wbrHeaderSignature>>8),browseFile) == EOF) {
        return errno;
    }

    // insert readable string and version
    sprintf(buf,"%s%c" ,wbrHeaderString ,wbrHeaderVersion );
    trySize = strlen(buf);
    wroteSize = fwrite(buf,1,trySize,browseFile);
    if (wroteSize < trySize) return errno;

    startSectionDesc = ftell(browseFile);

    // leave space for adding section offsets and sizes later
    sprintf(buf,"1111aaaa2222bbbb3333cccc4444dddd5555eeee");
    trySize = strlen(buf);
    wroteSize = fwrite(buf,1,trySize,browseFile);
    if (wroteSize < trySize) return errno;

    // write each of the 5 sections, tracking size and offset
    inFileNames[0] = debugFile;
    inFileNames[1] = referenceFile;
    inFileNames[2] = abbrevFile;
    inFileNames[3] = lineFile;
    inFileNames[4] = macroFile;

    for (fileNum=0;fileNum<5;fileNum++) {
        inFiles[fileNum] = fopen( inFileNames[fileNum], "rb" );
        if( inFiles[fileNum] == NULL ) {
            puts( strerror( errno ) );
            puts( inFileNames[fileNum] );
            CFatal( "dwarf: unable to open file for concatenate" );
        }
        sectionSize[fileNum] = 0;
        sectionOffset[fileNum] = ftell(browseFile);
        rewind(inFiles[fileNum]);

        do {
            readSize = fread(buf, 1, bufSize, inFiles[fileNum]);
            sectionSize[fileNum] += readSize;
            wroteSize = fwrite(buf,1,readSize,browseFile);
        } while (readSize == bufSize && wroteSize == readSize);

        if (readSize > wroteSize) return errno;
        if( fclose( inFiles[fileNum] ) ) {
            puts( strerror( errno ) );
            puts( inFileNames[fileNum] );
            CFatal( "dwarf: unable to close file after concatenate" );
        }
    }

    // go back and fill in section offsets and sizes
    fseek(browseFile, startSectionDesc, SEEK_SET /*from start of file*/);
    for (fileNum=0;fileNum<5;fileNum++) {
        for (i=0; i<4; i++) {
            if (fputc((unsigned char)sectionOffset[fileNum],browseFile) == EOF) {
                return errno;
            } else {
                sectionOffset[fileNum] >>= 8;
            }
        }
        for (i=0; i<4; i++) {
            if (fputc((unsigned char)sectionSize[fileNum],browseFile) == EOF) {
                return errno;
            } else {
                sectionSize[fileNum] >>= 8;
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------

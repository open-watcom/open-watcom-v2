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


#include <setjmp.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

#include <dw.h>
#include <dr.h>
#include "client.h"

#define TRUE 1
#define FALSE 0

void SetSects( int handle );
int TestIntegrity( int handle );

struct section_data Sections[DR_DEBUG_NUM_SECTS];

int main( int argc, char * argv[] ) {
    int handle;

    if( argc != 2 ) {
        fprintf( stderr, "Usage: main <filename>" );
        exit( 1 );
    }

    handle = open( argv[1], O_RDONLY | O_BINARY );
    assert( handle > 0 );

    if( !TestIntegrity( handle ) ) {
        fprintf( stderr, "%s is not a valid browser file.", argv[1] );
    }

    SetSects( handle );
    close( handle );

    DumpSections();

    return 0;
}

void SetSects( int handle )
/*************************/
{
    unsigned long sectsizes[DR_DEBUG_NUM_SECTS];
    unsigned long sections[DR_DEBUG_NUM_SECTS];
    int           i;

    memset( sections, 0, DR_DEBUG_NUM_SECTS * sizeof(unsigned_32) );
    memset( sectsizes, 0, DR_DEBUG_NUM_SECTS * sizeof(unsigned_32) );

    read( handle, &sections[DW_DEBUG_INFO], sizeof(unsigned_32) );
    read( handle, &sectsizes[DW_DEBUG_INFO], sizeof(unsigned_32) );
    read( handle, &sections[DW_DEBUG_REF], sizeof(unsigned_32) );
    read( handle, &sectsizes[DW_DEBUG_REF], sizeof(unsigned_32) );
    read( handle, &sections[DW_DEBUG_ABBREV], sizeof(unsigned_32) );
    read( handle, &sectsizes[DW_DEBUG_ABBREV], sizeof(unsigned_32) );
    read( handle, &sections[DW_DEBUG_LINE], sizeof(unsigned_32) );
    read( handle, &sectsizes[DW_DEBUG_LINE], sizeof(unsigned_32) );
    read( handle, &sections[DW_DEBUG_MACINFO], sizeof(unsigned_32) );
    read( handle, &sectsizes[DW_DEBUG_MACINFO], sizeof(unsigned_32) );

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        Sections[i].cur_offset = 0;
        Sections[i].max_offset = sectsizes[i];

        if( sectsizes[i] != 0 ) {
            if( lseek( handle, sections[i], SEEK_SET ) < 0 ) {
                perror( "" );
            }
            Sections[i].data = malloc( sectsizes[i] );
            if( Sections[i].data == NULL ) {
                fprintf( stderr, "Not enough memory\n" );
                exit(1);
            }
            read( handle, Sections[i].data, sectsizes[i] );
        }
    }
}

int TestIntegrity( int handle ) {
/*******************************/
    char *      mbrHeaderString = "WBROWSE";
    int         mbrHeaderStringLen = 7;

    unsigned    mbrHeaderSignature = 0xcbcb;

    char        mbrHeaderVersion = '1';

    unsigned_16 signature;
    char        buf[7];

    // verify browser identification bytes
    read( handle, &signature, sizeof(unsigned_16) );
    if( signature != mbrHeaderSignature ){
        return FALSE;
    }
    read( handle, buf, mbrHeaderStringLen );
    if( memcmp( buf, mbrHeaderString, mbrHeaderStringLen ) != 0 ) {
        return FALSE;
    }
    read( handle, buf, sizeof(char) );
    if( buf[0] != mbrHeaderVersion ) {
        return FALSE;
    }
    return TRUE;
}

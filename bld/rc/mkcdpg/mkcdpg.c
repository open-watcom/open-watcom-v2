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
* Description:  WRC code page generator.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "wio.h"
#include "watcom.h"
#include "banner.h"
#include "rctypes.h"
#include "dbtable.h"

typedef struct {
    DBTableHeader       head;
    uint_8              leads[256];
    DBIndexEntry        *index;
    uint_16             *table;
}DBInformation;

static void freeInfo( DBInformation *info ) {

    if( info != NULL ) {
        if( info->index != NULL ) free( info->index );
        if( info->table != NULL ) free( info->table );
        free( info );
    }
}

static BOOL writeInfo( char *fname, DBInformation *info ) {
    int         fp;
    int         rc;
    int         len;

    fp = open( fname, O_WRONLY | O_BINARY | O_CREAT, PMODE_RWX );
    if( fp == -1 ) {
        printf( "Error - could not open %s", fname );
        return( TRUE );
    }
    len = sizeof( DBTableHeader );
    rc = write( fp, &(info->head), len );
    if( rc != len ) goto WRITE_ERROR;

    len = 256;
    rc = write( fp, &(info->leads), len );
    if( rc != len ) goto WRITE_ERROR;

    len = info->head.num_indices * sizeof( DBIndexEntry );
    rc = write( fp, info->index, len );
    if( rc != len ) goto WRITE_ERROR;

    len = info->head.num_entries * sizeof( uint_16 );
    rc = write( fp, info->table, len );
    if( rc != len ) goto WRITE_ERROR;

    close( fp );
    return( FALSE );

WRITE_ERROR:
    printf( "Error writting to file %s\n", fname );
    close( fp );
    remove( fname );
    return( TRUE );
}

static DBInformation *buildInfo( char *page ) {
    DWORD               cp;
    DBInformation       *ret;
    char                ch[3];
    int                 i;
    unsigned            j;
    unsigned char       k;
    unsigned            offset;
    unsigned_16         base;
    int                 rc;
    CPINFO              theCPInfo;

    cp = strtoul( page, NULL, 0 );
    ret = malloc( sizeof( DBInformation ) );
    memset( ret, 0, sizeof( DBInformation ) );

    /* set up header */
    ret->head.sig[0] = DB_TABLE_SIG_1;
    ret->head.sig[1] = DB_TABLE_SIG_2;
    ret->head.ver = DB_TABLE_VER;

    /* getting the code page info */
    memset( &theCPInfo, 0, sizeof( CPINFO ) );
    rc = GetCPInfo( cp, &theCPInfo );
    if( rc != TRUE ) {
        printf( "Error - unable to get info for code page %d\n", cp );
        printf( "Error code - %d\n", GetLastError() );
        freeInfo( ret );
        ret = NULL;
        goto FINISHED;
    }

    /* iterate through the array of lead byte ranges */
    for( i = 0; i < MAX_LEADBYTES; i += 2 ) {
        /* if we're not at the end  then */
        if( theCPInfo.LeadByte[i] != 0 && theCPInfo.LeadByte[i + 1] != 0 ) {
            /* iterate through the range specified */
            for( j = theCPInfo.LeadByte[i];
                    j <= theCPInfo.LeadByte[i + 1];  j++ ) {
                /* set the lead char in the leads[] */
                ret->leads[j] = DB_BEG_CHAR;
                /* for every lead character, add 256 entries. */
                ret->head.num_entries += 256;
            }
            /* increment the number of indices for every range */
            ret->head.num_indices++;
        } else {
            /* we're done */
            break;
        }
    }

    /* because there is always at least one index and 256 entries */
    /* (single byte case) */
    ret->head.num_indices++;
    ret->head.num_entries += 256;

    /* allocate memory for index */
    ret->index = malloc( sizeof( DBIndexEntry ) * ret->head.num_indices );
    memset( ret->index, 0, sizeof( DBIndexEntry ) * ret->head.num_indices );

    /* allocate the memory necessary to contain each character mapping. */
    ret->table = malloc( ret->head.num_entries * sizeof( uint_16 ) );
    memset( ret->table, 0, sizeof( uint_16 ) * ret->head.num_entries );

    /* this is for the first index which always exists */
    ret->index[0].min = 0;
    ret->index[0].max = 255;
    ret->index[0].base = 0;

    /* go through the GetCPInfo() list again, this time copying     */
    /* the values into the newly allocated ret->index block       */
    base = 256;
    j = 1;
    for( i = 0; i < MAX_LEADBYTES; i += 2 ) {
        if ( theCPInfo.LeadByte[i] != 0 && theCPInfo.LeadByte[i + 1] != 0 ) {
            ret->index[j].min = 0   + ( theCPInfo.LeadByte[i] << 8 );
            ret->index[j].max = 255 + ( theCPInfo.LeadByte[i + 1] << 8 );
            ret->index[j].base = base;
            j++;
            base = base + ( ( theCPInfo.LeadByte[i + 1]
                               - theCPInfo.LeadByte[i] + 1 ) <<  8 );
        } else {
            break;
        }
    }

    /* handle the special case of no lead char */
    ch[1] = '\0';
    for( i=0; i < 256; i++ ) {
        ch[0] = (char)i;
        rc = MultiByteToWideChar( cp, MB_PRECOMPOSED, ch, 1,
                                  ret->table + i, 1 );
        if( rc == 0 ) {
            printf( "Error - unable to convert characters for code page %d\n", cp );
            printf( "Error code - %d\n", GetLastError() );
            freeInfo( ret );
            ret = NULL;
            goto FINISHED;
        }
    }

    /* iterate through the indexes, placing the wide char versions */
    /* into the corrosponding array entries                        */
    ch[2] = '\0';
    /* iterate through the indexes */
    for( j = 1; j < ret->head.num_indices; j++ ) {
        /* iterate through the min -> max of the indexes */
        for( k = ret->index[j].min >> 8 ; k <= ret->index[j].max >> 8; k++ ) {
            ch[0] = k;
            offset = ret->index[j].base +
                        ( k - ( ret->index[j].min >> 8 ) ) * 256;
            /* iterate through the 256 characters for each of the lead char */
            for( i = 0; i < 256; i++ ) {
                ch[1] = (char)i;
                /* translate the DBCS to Unicode */
                rc = MultiByteToWideChar( cp,
                                  MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                                  ch, 2,  ret->table + offset + i,  1 );
                if( rc == 0 ) {
                    rc = GetLastError();
                    /* if the error is that there's NO_UNICODE equiv, */
                    /* then ignore it, and place NULL in the array    */
                    /* otherwise we're in trouble and we should fail. */
                    if( rc != ERROR_NO_UNICODE_TRANSLATION ) {
                        printf( "Error - unable to convert characters for code page %d\n", cp );
                        printf( "Error code - %d\n", rc );
                        freeInfo( ret );
                        ret = NULL;
                        goto FINISHED;
                    } else {
                        ret->table[ offset + i ] = 0;
                    }
                }
            }
        }
    }
    FINISHED:
    return( ret );
}

int main( int argc, char *argv[] ) {

    DBInformation       *info;
    BOOL                error;

    error = FALSE;
    printf( banner1( "WRC Code Page Generator", "1.0" ) "\n" );
    printf( banner2 "\n" );
    printf( banner2a( "1984" ) "\n" );
    printf( banner3 "\n" );
    printf( banner3a "\n\n" );
    if( argc != 3 ) {
        printf( "This program creates data files used by the resource compiler to convert text\n" );
        printf( "from a given code page into UNICODE. It can only create data files for code\n" );
        printf( "pages that are currently installed and available in the system. It will \n" );
        printf( "correctly create conversion files for code pages that contain multi-byte\n" );
        printf( "characters.\n" );
        printf( "\n" );
        printf( "Usage:\n" );
        printf( "    %s <codepage> <filename>\n", argv[0] );
        return( 1 );
    }
    info = buildInfo( argv[1] );
    if( info == NULL ) error = TRUE;
    if( !error ) error = writeInfo( argv[2], info );
    freeInfo( info );
    return( error );
}

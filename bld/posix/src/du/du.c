/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX du utility
*               Displays directory disk space usage
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <sys/stat.h>
#include "bool.h"
#include "misc.h"
#include "getopt.h"
#include "fnutils.h"
#include "argvenv.h"
#include "pathgrp2.h"

#include "clibext.h"


char *OptEnvVar = "du";

char    numbuff[14];
long    clsize;
int     filecnt, linecnt;
char    aflag = false;
char    bflag = false;
char    fflag = false;
char    sflag = false;

void    DoDU( char *, unsigned long *, unsigned long * );
void    fmtPrint( unsigned long );

static const char *usageMsg[] = {
    "Usage: du [-?abs] [@env] [dirs]",
    "\tenv         : environment variable to expand",
    "\tdirs        : directories to calculate usage on",
    "\tOptions: -? : display this message",
    "\t\t -a : display usage for each file",
    "\t\t -b : display usage in bytes",
    "\t\t -f : format the output of the numbers",
    "\t\t -s : display usage based on true file size",
    NULL
};


/*
 * start of mainline
 */
int main( int argc, char *argv[] )
{
    int         i,ch,j;
    unsigned long   csum=0,ssum=0;

    argv = ExpandEnv( &argc, argv );    // Expand env. variables

    /*
     * process options
     */

    for( ;; ) {
        ch = GetOpt( &argc, argv, "abfs", usageMsg );
        if( ch == -1 ) {
            break;
        }
        if( ch == 'a' ) {
            aflag = true;
        } else if( ch == 'b' ) {
            bflag = true;
        } else if( ch == 'f' ) {
            fflag = true;
        } else if( ch == 's' ) {
            sflag = true;
        }
    }

    if( argc == 1 ) {
        clsize = GetClusterSize( 0 );
        DoDU( ".", &csum, &ssum );
        if( sflag ) {
            fmtPrint( ssum );
        }
        fmtPrint( csum );
        printf( " .\n" );
        exit(0);
    }


    /*
     * run through all specified files
     */
    for(i=1;i<argc;i++) {
        j = 0;
        if(argv[i][1] == ':') {
            j = tolower(argv[i][0]) - 'a' + 1;
        }
        clsize = GetClusterSize( j );
        csum = 0;
        ssum = 0;
        DoDU( argv[i], &csum, &ssum );
        if( sflag ) {
            fmtPrint( ssum );
        }
        fmtPrint( csum );
        printf( " %s\n", argv[i] );
    }
    return( 0 );
} /* main */

typedef struct ack {
    struct dirent df;
    struct ack *next;
} dfs;

/*
 * fmtPrint - format a number for output
 */
void fmtPrint( unsigned long val )
{
    if( !fflag ) {
        printf( "%-10lu ", val );
        return;
    }
    memset( numbuff, ' ', 13 );
    sprintf( numbuff, "%13lu", val );
    if( numbuff[9] != ' ' ) {
        memmove( numbuff+2, numbuff+3, 7 );
        numbuff[9] = ',';
        if( numbuff[5] != ' ' ) {
            memmove( numbuff+1, numbuff+2, 4 );
            numbuff[5] = ',';
            if( numbuff[1] != ' ' ) {
                numbuff[0] = numbuff[1];
                numbuff[1] = ',';
            }
        }
    }
    printf( "%s ", numbuff );
}

static int skip_entry( struct dirent *dire )
{
    return( (dire->d_attr & _A_SUBDIR) && IsDotOrDotDot( dire->d_name ) );
}

/*
 * DoDU - perform DU on a specified directory
 */
void DoDU( char *dir, unsigned long * tcsum, unsigned long * tssum )
{
    dfs                 *head;
    dfs                 *tail;
    dfs                 *temp;
    dfs                 *next;
    long                ii;
    long                jj;
    long                kk;
    unsigned long       csum;
    unsigned long       ssum;
    char                fname[_MAX_PATH];
    char                filename[_MAX_PATH];
    char                path[_MAX_PATH];
    size_t              path_len;
    DIR                 *dirp;
    struct dirent       *dire;
    size_t              len;
    struct stat         sb;
    pgroup2             pg;

    /*
     * initialize for file scan
     */
    _splitpath2( dir, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( stat( dir, &sb ) != -1 && S_ISDIR( sb.st_mode ) ) {
        _makepath( path, NULL, pg.dir, pg.fname, pg.ext );
        pg.fname = pg.ext = "";
    } else {
        _makepath( path, NULL, pg.dir, NULL, NULL );
    }

    if( pg.fname[0] == 0 ) {
        pg.fname = "*";
    }
    if( pg.ext[0] == 0 ) {
        pg.ext = ".*";
    } else if( pg.ext[0] == '.' && pg.ext[1] == 0 ) {
        pg.ext = ".*";
    }
    _makepath( filename, pg.drive, path, pg.fname, pg.ext );

    dirp = opendir( filename );
    if( dirp == NULL )
        return;

    /*
     * run until carry is set (no more matches)
     */
    tail = head = NULL;
    while( (dire = readdir( dirp )) != NULL ) {
        if( skip_entry( dire ) )
            continue;               // skip special directory entries
        if( (temp = malloc( sizeof( dfs ) )) == NULL )
            break;
        temp->df = *dire;
        temp->next = NULL;
        FNameLower( temp->df.d_name );
        if( head == NULL ) {
            head = tail = temp;
        } else {
            tail->next = temp;
            tail = temp;
        }
    }
    closedir( dirp );

    path_len = strlen( path );
    if( path[path_len - 1] != '\\' ) {
        path[path_len++] = '\\';
        path[path_len] = '\0';
    }
    if( strcmp( pg.fname, "*" ) == 0 && strcmp( pg.ext, ".*" ) == 0 ) {
        pg.fname = NULL;
        pg.ext = NULL;
    }
    /*
     * descend the subdirectories
     */
    for( temp = head; temp != NULL; temp = temp->next ) {
        if( temp->df.d_attr & _A_SUBDIR ) {
            strcpy( filename, path );
            strcpy( filename + path_len, temp->df.d_name );
            _makepath( fname, pg.drive, filename, pg.fname, pg.ext );
            len = strlen( fname ) - 1;
            if( fname[len] == '\\' ) {
                fname[len] = 0;
            }
            csum = 0;
            ssum = 0;
            DoDU( fname, &csum, &ssum );
            *tcsum += csum;
            *tssum += ssum;
            if( sflag ) {
                fmtPrint( ssum );
            }
            fmtPrint( csum );
            printf( " %s\n", fname );
        }
    }

    /*
     * get the total for this directory
     */
    for( temp = head; temp != NULL; temp = next ) {
        next = temp->next;
        ii = (long)temp->df.d_size;
        jj = ii / clsize;
        if( temp->df.d_size % clsize != 0 )
            jj++;
        kk = jj * clsize;
        if( !bflag ) {
            ii = (ii + 1023) / 1024;
            kk = (kk + 1023) / 1024;
        }
        *tssum += ii;
        *tcsum += kk;
        if( aflag ) {
            if( sflag ) {
                fmtPrint( ii );
            }
            fmtPrint( kk );
            printf( " %s\\%s\n", dir, temp->df.d_name );
        }
        free( temp );
    }

} /* DoDU */

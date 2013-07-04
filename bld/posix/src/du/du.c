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
#include "misc.h"
#include "getopt.h"
#include "fnutils.h"
#include "argvenv.h"
#include "clibext.h"

#define TRUE    1
#define FALSE   0

char *OptEnvVar = "du";

char    filename[_MAX_PATH];
char    numbuff[14];
long    clsize;
int     filecnt,linecnt;
char    aflag=FALSE;
char    bflag=FALSE;
char    fflag=FALSE;
char    sflag=FALSE;

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

    while( 1 ) {
        ch = GetOpt( &argc, argv, "abfs", usageMsg );
        if( ch == -1 ) {
            break;
        }
        if( ch == 'a' ) {
            aflag=TRUE;
        } else if( ch == 'b' ) {
            bflag=TRUE;
        } else if( ch == 'f' ) {
            fflag=TRUE;
        } else if( ch == 's' ) {
            sflag=TRUE;
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
        clsize = GetClusterSize(j);
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

/*
 * DoDU - perform DU on a specified directory
 */
void DoDU( char *dir, unsigned long * tcsum, unsigned long * tssum )
{
    dfs                 *head=NULL,*tail=NULL,*temp,*tmp;
    long                ii,jj,kk;
    unsigned long       csum,ssum;
    char                fname[_MAX_PATH];
    DIR                 *d;
    struct dirent       *nd;
    int                 len;
    struct stat         sb;
    char                drive[_MAX_DRIVE],directory[_MAX_DIR];
    char                extin[_MAX_EXT],name[_MAX_FNAME];

    /*
     * initialize for file scan
     */
    _splitpath( dir, drive, directory, name, extin );
    if( stat( dir, &sb ) != -1 ) {
        if( S_ISDIR( sb.st_mode ) ) {
            strcat( directory, name );
            strcat( directory, extin );
            name[0] = 0;
            extin[0] = 0;
        }
    }

    if( name[0] == 0 ) {
        strcpy( name,"*" );
    }
    if( extin[0] == 0 ) {
        strcpy( extin,".*" );
    } else if( extin[0] == '.' && extin[1] == 0 ) {
        strcpy( extin,".*" );
    }
    _makepath( filename, drive, directory, name, extin );

    d = opendir( filename );
    if( d == NULL ) return;

    /*
     * run until carry is set (no more matches)
     */
    while( (nd = readdir( d )) != NULL ) {

        if( !((nd->d_attr & _A_SUBDIR ) && IsDotOrDotDot( nd->d_name ) ) ) {
            if( (temp = malloc( sizeof( dfs ) )) == NULL ) break;
            temp->df = *nd;
            temp->next = NULL;
            FNameLower( temp->df.d_name );
            if( head == NULL ) {
                head = tail = temp;
            } else {
                tail->next = temp;
                tail = temp;
            }
        }

    }
    closedir( d );

    /*
     * descend the subdirectories
     */
    temp = head;
    while( temp != NULL ) {
        if( temp->df.d_attr & _A_SUBDIR ) {
            strcpy( filename,directory );
            if( directory[ strlen( directory ) -1 ] != '\\' ) {
                strcat( filename,"\\" );
            }
            strcat( filename,temp->df.d_name );
            if( !strcmp( name,"*" ) && !strcmp( extin,".*" ) ) {
                _makepath( fname,drive, filename, NULL, NULL );
                len = strlen( fname ) - 1;
                if( fname[ len ] == '\\' ) fname[ len ] = 0;
            } else {
                _makepath( fname,drive, filename, name, extin );
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
        temp = temp->next;
    }

    /*
     * get the total for this directory
     */
    temp = head;
    while( temp != NULL ) {
        ii = (long) temp->df.d_size;
        jj = ii / clsize;
        if( temp->df.d_size % clsize != 0 ) jj++;
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
        tmp = temp;
        temp = temp->next;
        free( tmp );
    }

} /* DoDU */

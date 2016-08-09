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


/*
  LC.C - perform unix-like lc function.
         must be compiled with large data.

   Date         By              Reason
   ====         ==              ======
   26-jun-89
    ...
   24-nov-91    Craig Eisler    original development
   08-jan-92    Craig Eisler    qnx
   21-jan-92    G.R.Bentz       change \*.* to check for \ first
   22-jan-92    Craig Eisler    unlimited files
   18-jun-92    D.J.Gaudet      added -d, -f, and -x options
   19-jun-92    Craig Eisler    use GetOpt
   21-jun-92    Craig Eisler    use -r instead of -x (-x is for regexps)
   07-jul-92    D.J.Gaudet      handle HPFS filenames
   30-oct-93    Brad Brisco     detect console dimensions

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __QNX__
#include <dos.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif
#include "bool.h"
#include "getopt.h"
#include "misc.h"
#include "fnutils.h"
#include "console.h"

#include "clibext.h"


#define LINE_WIDTH      80      /* FIXME: should really determine screen width*/
#define COLUMN_WIDTH    16
#define GUTTER_WIDTH    2

char *OptEnvVar="lc";

#define DIR_PASS        1
#define FILE_PASS       2
#define READ_ONLY_PASS  3

struct dirent **files;
char filename[_MAX_PATH];
int  filecnt,linecnt,fileflag,dirflag,read_only_flag,pass;
int maxwidth;

int     directories_only;
int     files_only;
int     separate_read_only;

static const char *usageMsg[] = {
    "Usage: lc [-?dfr] [files]",
    "\tfiles       : directories to list",
    "\tOptions: -? : display this message",
    "\t\t -d : show directories only",
    "\t\t -f : show files only",
    "\t\t -r : show read-only files separately",
    NULL
};

/* Forward declarations */
void DoLC( char *dir );
void PrintFile( struct dirent *file );


/*
 * start of mainline
 */
int main( int argc, char *argv[] )
{
    int i, ch;

#ifndef __QNX__
    maxwidth = GetConsoleWidth();
#else
    maxwidth = LINE_WIDTH;              /* for now */
#endif
    maxwidth /= COLUMN_WIDTH;

    for( ;; ) {
        ch = GetOpt( &argc, argv, "dfr", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case 'd':
            directories_only = 1;
            break;
        case 'f':
            files_only = 1;
            break;
        case 'r':
            separate_read_only = 1;
            break;
        }
    }

    if( directories_only && files_only ) {
        directories_only = files_only = 0;
    }
    if( argc == 1 ) {
        DoLC( "." );
    } else {
        for( i = 1; i < argc; i++ ) {
            printf( "%s:\n", argv[i] );
            DoLC( argv[i] );
        }
    }
    return( 0 );
} /* main */

static int Compare( const void *_p1, const void *_p2 )
{
    const struct dirent **p1 = (const struct dirent **)_p1;
    const struct dirent **p2 = (const struct dirent **)_p2;

    return( strcmp( (*p1)->d_name,(*p2)->d_name ) );
} /* Compare */

/*
 * DoLC - perform LC on a specified directory
 */
void DoLC( char *dir )
{
    int                 i;
    DIR                 *d;
    struct dirent       *nd;
#ifdef __QNX__
    char                tmpname[ _MAX_PATH ];
    char                drive[_MAX_DRIVE],directory[_MAX_DIR];
    char                name[_MAX_FNAME],ext[_MAX_EXT];
#endif

    /*
     * initialize for file scan
     */
    filecnt = 0;
    strcpy(filename,dir);
#ifndef __QNX__
    if( !FNameCompare(dir,"..") ) {
        strcat(filename,"\\*.*");
    } else if( dir[ strlen(dir)-1 ] == '.' ) {
        filename[ strlen(dir)-1 ] = 0;
        strcat(filename,"*.*");
    } else if( dir[ strlen(dir)-1 ] == '\\' ) {
        strcat(filename,"*.*");
    } else {
        strcat(filename,"\\*.*");
    }
#else
    if( filename[0] == 0 ) {
        filename[0] = '.';
        filename[1] = 0;
    }
    _splitpath( filename, drive, directory, name, ext );
#endif
    d = opendir( filename );
    if( d == NULL ) {
        printf( "Directory (%s) not found.\n",filename );
        return;
    }


    /*
     * find all files (except for . and ..)
     */
    while( (nd = readdir( d )) != NULL ) {

#ifndef __QNX__
        if( files_only && nd->d_attr & _A_SUBDIR ) {
            continue;
        }
        if( directories_only && !( nd->d_attr & _A_SUBDIR ) ) {
            continue;
        }
        if( !((nd->d_attr & _A_SUBDIR ) && IsDotOrDotDot( nd->d_name ))) {
#else
        if( files_only && S_ISDIR( nd->d_stat.st_mode ) ) {
            continue;
        }
        if( directories_only && !S_ISDIR( nd->d_stat.st_mode ) ) {
            continue;
        }
        if( !((S_ISDIR( nd->d_stat.st_mode) && IsDotOrDotDot( nd->d_name ) ))) {
#endif
            files = realloc( files, ( filecnt+1 )*sizeof( struct dirent * ) );
            if( files == NULL ) {
                printf( "Out of memory!\n" );
                exit( 1 );
            }
            files[ filecnt ] = malloc( sizeof( struct dirent ) );
            if( files[ filecnt ] == NULL ) {
                break;
            }
#ifndef __QNX__
            FNameLower( nd->d_name );
#else
            if( !(nd->d_stat.st_status & _FILE_USED ) ) {
                _splitpath( nd->d_name, NULL, NULL, name, ext );
                _makepath( tmpname, drive, directory, name, ext );
                stat( tmpname, &nd->d_stat );
            }
#endif
            memcpy( files[filecnt++],nd,sizeof( struct dirent ) );
        }

    }
    closedir( d );
    if( filecnt == 0 ) {
        return;
    }

    /*
     * sort the data.
     */
    qsort( files, filecnt, sizeof(struct dirent *), Compare );

    /*
     * determine if there are files and/or directories
     */
    fileflag = dirflag = read_only_flag = false;
    for( i = 0; i < filecnt; i++ ) {
#ifndef __QNX__
        if( files[i]->d_attr & _A_SUBDIR ) {
            dirflag = true;
        } else if( separate_read_only && files[i]->d_attr & _A_RDONLY ) {
            read_only_flag = true;
        } else {
            fileflag = true;
        }
#else
        if( S_ISDIR( files[i]->d_stat.st_mode ) ) {
            dirflag = true;
        } else {
            fileflag = true;
        }
#endif
        if( fileflag && dirflag && (!separate_read_only || read_only_flag) ) {
            break;
        }
    }

    /*
     * print out results
     */
    if( dirflag ) {
        printf("Directories:\n");
        pass = DIR_PASS;
        linecnt=0;
        for( i = 0; i < filecnt; i++ ) {
            PrintFile( files[i] );
        }
        if( linecnt != 0 ) {
            printf("\n");
        }
    }
    if( fileflag ) {
        if( dirflag ) {
            printf("\n");
        }
        printf("Files:\n");
        pass = FILE_PASS;
        linecnt=0;
        for( i = 0; i < filecnt; i++ ) {
            PrintFile( files[i] );
        }
        if( linecnt != 0 ) {
            printf("\n");
        }
    }
    if( read_only_flag ) {
        if( dirflag || fileflag ) {
            printf("\n");
        }
        printf( "Read-Only Files:\n" );
        pass = READ_ONLY_PASS;
        linecnt = 0;
        for( i = 0; i < filecnt; i++ ) {
            PrintFile( files[i] );
        }
        if( linecnt != 0 ) {
            printf("\n");
        }
    }
} /* DoLC */

/*
 * PrintFile - print a file entry
 */
void PrintFile( struct dirent *file )
{

    size_t      len;
    size_t      num_columns;

#ifndef __QNX__
    if( file->d_attr & _A_SUBDIR ) {
#else
    if( S_ISDIR( file->d_stat.st_mode ) ) {
#endif
        if( pass != DIR_PASS ) {
            return;
        }
    } else {
#ifndef __QNX__
        if( separate_read_only && file->d_attr & _A_RDONLY ) {
            if( pass != READ_ONLY_PASS ) {
                return;
            }
        } else if( pass != FILE_PASS ) {
            return;
        }
#else
        if( pass != FILE_PASS ) {
            return;
        }
#endif
    }

    /* calculate how many columns it will take to print the filename */
    len = strlen( file->d_name ) + ( GUTTER_WIDTH + COLUMN_WIDTH - 1 );
    num_columns = len / COLUMN_WIDTH;
    if( num_columns + linecnt > maxwidth ) {
        printf( "\n" );
        linecnt = 0;
    }
    printf("%-*s", COLUMN_WIDTH * num_columns - GUTTER_WIDTH, file->d_name );
    linecnt += num_columns;
    if( linecnt >= maxwidth ) {
        printf("\n");
        linecnt = 0;
    } else {
        printf( "%*s", GUTTER_WIDTH, "" );
    }

} /* PrintFile */

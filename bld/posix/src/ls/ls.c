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
* Description:  POSIX ls utility.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include <io.h>
#include <dos.h>
#if defined( __OS2__ )
#include <os2.h>
#endif
#include "watcom.h"
#include "misc.h"
#include "getopt.h"
#include "fnutils.h"
#include "console.h"

#include "clibext.h"
#include "filerx.h"


#define LINE_WIDTH      80
#define GUTTER_WIDTH    2

char *OptEnvVar="ls";

typedef struct {
    unsigned    sec:5;
    unsigned    min:6;
    unsigned    hour:5;
} dostime;

typedef struct {
    unsigned    day:5;
    unsigned    month:4;
    unsigned    year:7;
} dosdate;

unsigned line_width;
long    clsize;
unsigned fileperlinecnt;
unsigned maxfileperline;
unsigned columnwidth;
int     N1flag=FALSE,
        Fflag=FALSE,
        pflag=FALSE,
        rxflag=FALSE,
        Rflag=FALSE,
        lflag=FALSE,
        hflag=FALSE,
        rflag=FALSE,
        sflag=FALSE,
        tflag=FALSE;

static const char *usageMsg[] = {
    "Usage: ls [-?1CFRlhprstX] [files]",
    "\tfiles       : directories/files to list",
    "\tOptions: -? : display this message",
    "\t\t -1 : single column display",
    "\t\t -C : multi-column display",
    "\t\t -F : display file type indicator (/ for dir, * for x)",
    "\t\t -R : recursive ls",
    "\t\t -l : print long info about each file",
    "\t\t -h : dont't include hidden and system files",
    "\t\t -p : display relative path for file",
    "\t\t -r : reverse sort",
    "\t\t -s : sort by size, rather than name (largest first)",
    "\t\t -t : sort by time, rather than name (most recent first)",
    "\t\t -X : match files by regular expressions",
    NULL
};

/* forward declarations */
static void DoLS( char *path, char *name );
static int IsSpecialRoot( char * filename );
static void PrintFile( char *drive, char *dir, DIR *file );
static int IsX( char *file );


/*
 * start of mainline
 */
int main( int argc, char *argv[] )
{
    DIR         *d;
    char        filebuff[_MAX_PATH];
    char        **todo = NULL;
    int         todocnt = 0;
    int         i;
    int         ch;


#ifndef __QNX__
    line_width = GetConsoleWidth();
#else
    line_width = LINE_WIDTH;            /* for now */
#endif

    if( !isatty( fileno( stdout ) ) ) {
        N1flag = TRUE;
    }

    /*
     * process options
     */
    while( (ch = GetOpt( &argc, argv, "XRphrtsFl1C", usageMsg )) != -1 ) {
        switch( ch ) {
        case 'R':
            Rflag=TRUE;
            break;
        case 'X':
            rxflag=TRUE;
            break;
        case 'p':
            N1flag=TRUE;
            pflag=TRUE;
            break;
        case 'r':
            rflag=TRUE;
            break;
        case 't':
            tflag=TRUE;
            break;
        case 's':
            sflag=TRUE;
            break;
        case 'F':
            Fflag=TRUE;
            break;
        case 'l':
            lflag=TRUE;
            break;
        case '1':
            N1flag=TRUE;
            break;
        case 'C':
            N1flag=FALSE;
            break;
        case 'h':
            hflag=TRUE;
            break;
        }
    }

    /*
     * if no files specified, list current directory
     */
    if( argc == 1 ) {
        if( rxflag ) {
            DoLS( ".", "*" );
        } else {
            DoLS( ".", "*.*" );
        }
        exit( 0 );
    }

    /*
     * process any directories specified
     */
    for( i = 1 ; i < argc ; i++ ) {
        if( FileNameWild( argv[i], rxflag ) ) {
            d = NULL;
        } else if( IsSpecialRoot( argv[i] ) ) {
            d = opendir( strcat( strcpy( filebuff, argv[i] ), "\\" ) );
        } else {
            d = opendir( argv[i] );
        }
        if( d != NULL && ( d->d_attr & _A_SUBDIR ) ) {
            printf( "%s:\n", argv[i] );
            if( rxflag ) {
                DoLS( argv[i], "*" );
            } else {
                DoLS( argv[i], "*.*" );
            }
            printf( "\n" );
        } else {
            todo = realloc( todo, (todocnt+1)*sizeof( char * ) );
            if( todo == NULL ) {
                printf( "Out of memory!\n" );
                exit( 1 );
            }
            todo[todocnt++] = argv[i];
        }
        if( d != NULL ) {
            closedir( d );
        }
    }

    /*
     * run through all specified files
     */
    for( i = 0 ; i < todocnt ; i++ ) {
        DoLS( NULL, todo[i] );
    }
    free( todo );
    return( 0 );
} /* main */

/*
 * Compare routines follow
 */
int Compare( struct dirent **p1, struct dirent **p2 )
{
    return( strcmp( (*p1)->d_name, (*p2)->d_name ) );
}
int CompareReverse( struct dirent **p1, struct dirent **p2 )
{
    return( strcmp( (*p2)->d_name, (*p1)->d_name ) );
}
int CompareDate( struct dirent **p1, struct dirent **p2 )
{
    if( (*p1)->d_date < (*p2)->d_date ) {
        return( 1 );
    }
    if( (*p1)->d_date > (*p2)->d_date ) {
        return( -1 );
    }
    if( (*p1)->d_time < (*p2)->d_time ) {
        return( 1 );
    }
    if( (*p1)->d_time > (*p2)->d_time ) {
        return( -1 );
    }
    return( Compare( p1, p2 ) );
}
int CompareDateReverse( struct dirent **p1, struct dirent **p2 )
{
    if( (*p1)->d_date > (*p2)->d_date ) {
        return( 1 );
    }
    if( (*p1)->d_date < (*p2)->d_date ) {
        return( -1 );
    }
    if( (*p1)->d_time > (*p2)->d_time ) {
        return( 1 );
    }
    if( (*p1)->d_time < (*p2)->d_time ) {
        return( -1 );
    }
    return( CompareReverse( p1, p2 ) );
}
int CompareSize( struct dirent **p1, struct dirent **p2 )
{
    if( (*p1)->d_size < (*p2)->d_size ) {
        return( 1 );
    }
    if( (*p1)->d_size > (*p2)->d_size ) {
        return( -1 );
    }
    return( Compare( p1, p2 ) );
}
int CompareSizeReverse( struct dirent **p1, struct dirent **p2 )
{
    if( (*p1)->d_size > (*p2)->d_size ) {
        return( 1 );
    }
    if( (*p1)->d_size < (*p2)->d_size ) {
        return( -1 );
    }
    return( CompareReverse( p1, p2 ) );
}

/*
 * DoLS - perform LS on a specified directory
 */
static void DoLS( char *path, char *name )
{
    char                filename[_MAX_PATH];
    char                filebuff[_MAX_PATH2];
    char                *drive;
    char                *dir;
    int                 filecnt = 0;
    DIR                 *directory;
    struct dirent       **files = NULL;
    struct dirent       *nextdirentry;
    struct dirent       *file;
    int                 (*fn)(struct dirent **, struct dirent **);
    int                 i;
    char                wild[_MAX_PATH];
    char                *err;
    void                *crx = NULL;
    unsigned            fname_len;
    unsigned            max_fname_len = 0;

    /*
     * initialize for file scan
     */
    if( path != NULL ) {
        strcpy( filename, path );
        if( !isFILESEP( path[ strlen( path ) - 1 ] ) ) {
            strcat( filename, FILESEPSTR );
        }
        strcat( filename, name );
    } else {
        strcpy( filename, name );
        if( IsSpecialRoot( filename ) ) {
            strcat( filename, "\\" );
        }
    }
    _splitpath2( filename, filebuff, &drive, &dir, NULL, NULL );

    if( lflag ) {
        i = 0;
        if( filename[1] == ':' ) {
            i = tolower( filename[0] ) - 'a' + 1;
        }
        clsize = GetClusterSize( i );
    }

    if( rxflag ) {
        directory = OpenDirAll( filename, wild );
    } else {
        directory = opendir( filename );
    }


    if( directory == NULL ) {
        printf( "File (%s) not found.\n", filename );
        return;
    }

    /*
     * get all directory entries
     */
    if( rxflag ) {
        err = FileMatchInit( &crx, wild );
        if( err != NULL ) {
            Die( "\"%s\": %s\n", err, wild );
        }
    }
    while( ( nextdirentry = readdir( directory ) ) != NULL ) {

        FNameLower( nextdirentry->d_name );
        if( rxflag ) {
            if( !FileMatch( crx, nextdirentry->d_name ) ) {
                continue;
            }
        }
        if( hflag ) {
            if( nextdirentry->d_attr & (_A_HIDDEN|_A_SYSTEM) ) {
                continue;
            }
        }
        if( !( ( nextdirentry->d_attr & _A_SUBDIR )
             && IsDotOrDotDot( nextdirentry->d_name ) ) ) {
            files = realloc( files, ( filecnt+1 )*sizeof( struct dirent * ) );
            if( files == NULL ) {
                printf( "Out of memory!\n" );
                exit( 1 );
            }
            files[ filecnt ] = malloc( sizeof( struct dirent ) );
            if( files[filecnt] == NULL ) {
                break;
            }
            fname_len = strlen( nextdirentry->d_name );
            if( fname_len > max_fname_len ) {
                max_fname_len = fname_len;
            }
            memcpy( files[filecnt++], nextdirentry, sizeof( struct dirent ) );
        }

    }
    closedir( directory );
    if( rxflag ) {
        FileMatchFini( crx );
    }

    /*
     * now display files, if there are any
     */
    if( filecnt ) {

        /*
         * sort the data
         */
        if( rflag ) {
            if( tflag ) {
                fn = CompareDateReverse;
            } else if( sflag ) {
                fn = CompareSizeReverse;
            } else {
                fn = CompareReverse;
            }
        } else {
            if( tflag ) {
                fn = CompareDate;
            } else if( sflag ) {
                fn = CompareSize;
            } else {
                fn = Compare;
            }
        }
        qsort( files, filecnt, sizeof(struct dirent *), (int (*)(const void *, const void * ))fn );

        /*
         * print out results
         */
        if( lflag ) {
            long size = 0L;
            long sum = 0L;
            for( i = 0 ; i < filecnt ; i++ ) {
                file = files[i];
                size = file->d_size / clsize;
                if( (file->d_size % clsize != 0L)
                  ||(file->d_size == 0L) ) {
                    size++;
                }
                sum += (size * clsize) / 1024L;
            }
            printf( "total %ld\n", sum );
        }
        columnwidth = max_fname_len + GUTTER_WIDTH;
        maxfileperline = line_width / columnwidth;
        fileperlinecnt = 0;
        for( i = 0 ; i < filecnt ; i++ ) {
            PrintFile( drive, dir, files[i] );
        }
        if( fileperlinecnt != 0 ) {
            printf( "\n" );
        }

        if( Rflag ) {
            for( i = 0 ; i < filecnt ; i++ ) {
                if( files[i]->d_attr & _A_SUBDIR ) {
                    _makepath( filename, drive, dir, files[i]->d_name, NULL );
                    printf( "\n%s:\n", filename );
                    DoLS( filename, name );
                }
                free( files[i] );
            }
        }
        free( files );

    }

} /* DoLS */

/*
 * PrintFile - print a file entry
 */
static void PrintFile( char *drive, char *dir, DIR *file )
{
    static char months[13][4] = {
        "***", "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug", "Sep",
        "Oct", "Nov", "Dec"
    };
    char name[_MAX_PATH + 1];
    char buff[80 + _MAX_PATH];
    long size;
    unsigned    len;
    int         extra_char;

    strcpy( name, file->d_name );
    len = strlen( name );

    extra_char = 0;
    if( Fflag ) {
        if( file->d_attr & _A_SUBDIR ) {
            name[len] = '/';
            ++len;
            name[len] = 0;
            ++extra_char;
        } else if( IsX( file->d_name ) ) {
            name[len] = '*';
            ++len;
            name[len] = 0;
            ++extra_char;
        }
    }

    if( !lflag ) {

        if( !N1flag ) {
            printf( "%-*s", extra_char + columnwidth - GUTTER_WIDTH, name );
            if( ++fileperlinecnt == maxfileperline ) {
                printf( "\n" );
                fileperlinecnt = 0;
            } else {
                printf( "%-*s", GUTTER_WIDTH - extra_char, "" );
            }
        } else {
            if( pflag ) {
                if( drive != NULL && drive[0] != '\0' ) {
                    printf( "%s",  drive );
                }
                if( dir != NULL && dir[0] != '\0' ) {
                    printf( "%s", dir );
                }
            }
            printf( "%s\n", name );
        }

    } else {

        /*
         * build attributes
         */
        strcpy( buff, "-------" );
        size = file->d_size;
        if( file->d_attr & _A_SUBDIR ) {
            buff[0] = 'd';
            size = 0;
        }
        if( file->d_attr & _A_ARCH ) {
            buff[1] = 'a';
        }
        if( file->d_attr & _A_HIDDEN ) {
            buff[2] = 'h';
        }
        if( file->d_attr & _A_SYSTEM ) {
            buff[3] = 's';
        }
        buff[4] = 'r';
        if( !(file->d_attr & _A_RDONLY ) ) {
            buff[5] = 'w';
        }
        if( IsX( file->d_name ) ) {
            buff[6] = 'x';
        }

        printf( "%s %10ld  %3s %2d %d  %2d:%02d  ", buff, size,
                months[(int)((dosdate *)&file->d_date)->month],
                (int)((dosdate *)&file->d_date)->day,
                (int)((dosdate *)&file->d_date)->year+1980,
                (int)((dostime *)&file->d_time)->hour,
                (int)((dostime *)&file->d_time)->min );

        if( pflag ) {
            if( drive != NULL && drive[0] != '\0' ) {
                printf( "%s", drive );
            }
            if( dir != NULL && dir[0] != '\0' ) {
                printf( "%s", dir );
            }
        }
        printf( "%s\n", name );

    }

} /* PrintFile */

/*
 * IsX - check if a file has .com, .bat, or .exe at the end
 */
static int IsX( char *file )
{
    char        *f;

    f = file + strlen( file ) - 4;
    if( f < file ) return( FALSE );
    if( !FNameCompare( f, ".com" )
      ||!FNameCompare( f, ".bat" )
      ||!FNameCompare( f, ".exe" )
#if defined( __OS2__ )
      ||!FNameCompare( f, ".cmd" )
#endif
       ) {
        return( TRUE );
    }
    return( FALSE );

} /* IsX */

static int IsSpecialRoot( char * filename )
/**********************************/
// Check if 'filename' is of the form 'd:'
{
    char                filebuff[_MAX_PATH2];
    char                *drive;
    char                *dir;

    _splitpath2( filename, filebuff, &drive, &dir, NULL, NULL );
    return( drive[0] != '\0' && dir[0] == '\0' );
} /* IsSpecialRoot */

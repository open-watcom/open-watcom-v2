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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
 * MS-DOS routines to read directories in the BSD format
 * By J. Eric Roskos    1/87  Public domain.
 *
 * Revision History:
 *      3/08/87         Eliminate use of static strings; malloc search string,
 *                              store whichdir in DIR structure.  Make dosdir structure
 *                              a substructure of DIR.
 */

#include "ndir.h"
#include <dos.h>
#include <stdio.h>

extern int  _doserrno;

char    *dos_errlst[] = {
    "",
    "Invalid function number",
    "File not found",
    "Path not found",
    "Too many open files",
    "Access denied",
    "Invalid handle",
    "Memory control blocks destroyed",
    "Insufficient memory",
    "Invalid memory block address",
    "Invalid environment",
    "Invalid format",
    "Invalid access code",
    "Invalid data",
    "?",
    "Invalid drive",
    "Can't remove current directory",
    "Not same device",
    "No more files",
    ""
};

/*
* open a directory.
*/
DIR *opendir( char *name )
{
    DIR     *dirp;
    char    *malloc();

    dirp = (DIR *)malloc( sizeof( DIR ) );
    if( dirp == NULL ) {
        fprintf( stderr, "opendir: malloc failed\n" );
        return( NULL );
    }
    strncpy( dirp->whichdir, name, sizeof( dirp->whichdir ) );
    dirp->dosdir.doshdr[0] = 'E';
    dirp->dosdir.doshdr[1] = 'R';
    return( dirp );
}

/*
* close a directory.
*/
void closedir( DIR *dirp )
{
    free( dirp );
}

/*
* read a DOS directory entry and present it as a new Unix one
*/

/*
* get next entry in a directory.
*/
struct direct *readdir( DIR *dirp )
{
    static struct direct dir;
    union REGS      inregs;
    union REGS      outregs;
    char            *search;

    inregs.h.ah = 0x1a;                 /* set DTA */
    inregs.x.dx = (int)&dirp->dosdir;
    intdos( &inregs, &outregs );
    if( outregs.x.cflag ) {
        fprintf( stderr, "readdir: set DTA: dos error %d\n", _doserrno );
        return( NULL );
    }

    if( dirp->dosdir.doshdr[0] == 'E' && dirp->dosdir.doshdr[1] == 'R' ) {
        inregs.h.ah = 0x4e;             /* find first */
    } else {
        inregs.h.ah = 0x4f;             /* find next  */
    }
    search = malloc( strlen( dirp->whichdir ) + 8 );
    if( search == NULL ) {
        fprintf( stderr, "readdir: malloc failed\n" );
        perror( "malloc" );
        return( NULL );
    }
    sprintf( search, "%s/*.*", dirp->whichdir );
    inregs.x.dx = (int)search;          /* match any name */
    inregs.x.cx = 0x16;                 /* match any type */
    intdos( &inregs, &outregs );
    free( search );
    dir.d_ino = 1;
    if( outregs.x.cflag ) {
        if( _doserrno != 18 )           /* 18 = "end of dir" */
            fprintf( stderr, "readdir: '%s': %s\n", dirp->whichdir, dos_errlst[_doserrno] );
        dir.d_ino = 0;
        dir.d_name[0] = '\0';
        return( NULL );
    } else {
        strncpy( dir.d_name, strlwr( dirp->dosdir.name ), 13 );
    }
    dir.d_namlen = strlen( dir.d_name );
    return( &dir );
}

/*
* Go back to the beginning of a directory.
*/

void rewinddir( DIR *dirp )
{
    dirp->dosdir.doshdr[0] = 'E';
    dirp->dosdir.doshdr[1] = 'R';
}


#ifdef TEST

int main( int argc, char **argv )
{
    DIR            *dirp;
    struct direct  *dire;
    int             rew = 0;

    if( argc > 1 ) {
        dirp = opendir( argv[1] );
    } else {
        dirp = opendir( "." );
    }
again:
    do {
        dire = readdir( dirp );
        printf( "%s\n", dire->d_name );
    } while( dire->d_ino );

    if( rew++ == 0 ) {                      /* test rewinddir once */
        printf( "**** Rewinding ****\n" );
        rewinddir( dirp );
        goto again;
    }
    closedir( dirp );
    return( 0 );
}

#endif

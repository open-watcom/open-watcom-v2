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
* Description:  POSIX rm utility
*               Removes files and directories
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dos.h>
#include <direct.h>
#include <conio.h>
#include "wio.h"
#include "watcom.h"
#include "lineprt.h"
#include "misc.h"
#include "getopt.h"
#include "fnutils.h"
#include "clibext.h"
#include "filerx.h"

char                    *OptEnvVar = "rm";

static const char       *usageMsg[] = {
    "Usage: rm [-?firRsvX] [files]",
    "\tfiles       : files/directories to delete",
    "\tOptions: -? : print this list",
    "\t\t -f : force deletion of read-only files",
    "\t\t      and don't complain about non existant files / dirs",
    "\t\t -i : inquire about each deletion",
    "\t\t -r : recursively delete all directories",
    "\t\t -R : recursively delete all directories",
    "\t\t -s : silent operation (default)",
    "\t\t -v : verbose operation",
    "\t\t -X : match files by regular expressions",
    NULL
};

int                     rflag = FALSE;
int                     iflag = FALSE;
int                     fflag = FALSE;
int                     sflag = TRUE;
int                     rxflag = FALSE;
int                     error_occured = 0;

typedef struct dd {
  struct dd     *next;
  char          attr;
  char          name[1];
}                       iolist;

void DoRM( const char *f );
void RecursiveRM( const char *dir );

/* start of mainline */
int main( int argc, char *argv[] )
{
    int i;
    int ch;
    DIR *d;

    /* process options */
    while( ( ch = GetOpt( &argc, argv, "firRsvX", usageMsg ) ) != -1 ) {
        switch( ch ) {
        case 'f': fflag  = TRUE;  iflag = FALSE; break;
        case 'i': iflag  = TRUE;  fflag = FALSE; break;
        case 'R':
        case 'r': rflag  = TRUE;  break;
        case 's': sflag  = TRUE;  break;
        case 'v': sflag  = FALSE; break;
        case 'X': rxflag = TRUE;  break;
        }
    }
    if( argc < 2 )
        Quit( usageMsg, "No filename/directory specified\n" );

    StartPrint();

    /* process -r option */
    if( rflag ) {
        for( i = 1; i < argc; i++ ) {
            if( strcmp( argv[i], rxflag ? "*" : "*.*" ) == 0 )
                RecursiveRM( "." );
            else {
                d = FileNameWild( argv[i], rxflag ) ? NULL : opendir( argv[i] );
                if( d != NULL ) {
                    if( !( d->d_attr & _A_SUBDIR ) ) {
                        closedir( d );
                        if( fflag )
                            DoRM( argv[i] );
                        else
                            PrintALineThenDrop( "%s is not a directory.", argv[i] );
                    } else {
                        closedir( d );
                        RecursiveRM( argv[i] );
                    }
                } else if( !fflag ) {
                    PrintALineThenDrop( "Directory %s not found.", argv[i] );
                    error_occured = 1;
                }
            }
        }
    } else {
        /* run through all specified files */
        for( i = 1; i < argc; i++ )
            DoRM( argv[i] );
    }
    DropALine();
    return( error_occured );
}

/* DoRM - perform RM on a specified file */
void DoRM( const char *f )
{
    iolist              *fhead = NULL;
    iolist              *ftail = NULL;
    iolist              *tmp;

    iolist              *dhead = NULL;
    iolist              *dtail = NULL;

    char                *bo;
    char                fpath[_MAX_PATH];
    char                tmppath[_MAX_PATH];

    int                 i;
    int                 j;
    int                 k;
    int                 l;

    size_t              len;
    DIR                 *d;
    struct dirent       *nd;
    char                wild[_MAX_PATH];
    char                *err;
    void                *crx = NULL;

    /* get file path prefix */
    fpath[0] = 0;
    for( i = ( int ) strlen( f ); i >= 0; i-- ) {
        if( f[i] == ':' || f[i] == '\\' || f[i] == '/' ) {
            fpath[i + 1] = 0;
            for( j = i; j >= 0; j-- )
                fpath[j] = f[j];
            i = -1;
        }
    }
    d = OpenDirAll( f, wild );
    if( d == NULL ) {
        PrintALineThenDrop( "File (%s) not found.", f );
        if( !fflag ) {
            error_occured = 1;
        }
        return;
    }

    if( rxflag ) {
        err = FileMatchInit( &crx, wild );
        if( err != NULL )
            Die( "\"%s\": %s\n", err, wild );
    }

    k = ( int ) strlen( fpath );
    while( ( nd = readdir( d ) ) != NULL ) {
        FNameLower( nd->d_name );
        if( rxflag ) {
            if( !FileMatch( crx, nd->d_name ) )
                continue;
        } else {
            if( !FileMatchNoRx( nd->d_name, wild ) )
                continue;
        }
        /* set up file name, then try to delete it */
        l = ( int ) strlen( nd->d_name );
        bo = tmppath;
        for( i = 0; i < k; i++ )
            *bo++ = fpath[i];
        for( i = 0; i < l; i++ )
            *bo++ = nd->d_name[i];
        *bo = 0;
        if( nd->d_attr & _A_SUBDIR ) {
            /* process a directory */
            if( !IsDotOrDotDot( nd->d_name ) ) {
                if( rflag ) {
                    /* build directory list */
                    len = strlen( tmppath );
                    tmp = MemAlloc( sizeof( iolist ) + len );
                    if( dtail == NULL )
                        dhead = tmp;
                    else
                        dtail->next = tmp;
                    dtail = tmp;
                    memcpy( tmp->name, tmppath, len + 1 );
                } else {
                    PrintALineThenDrop( "%s is a directory, use -r", tmppath );
                    error_occured = 1;
                }
            }

        } else if( ( nd->d_attr & _A_RDONLY ) && !fflag ) {
            PrintALineThenDrop( "%s is read-only, use -f", tmppath );
            error_occured = 1;
        } else {
            /* build file list */
            len = strlen( tmppath );
            tmp = MemAlloc( sizeof( iolist ) + len );
            if( ftail == NULL )
                fhead = tmp;
            else
                ftail->next = tmp;
            ftail = tmp;
            memcpy( tmp->name, tmppath, len + 1 );
            tmp->attr = nd->d_attr;
        }
    }
    closedir( d );
    if( rxflag )
        FileMatchFini( crx );

    /* process any files found */
    tmp = fhead;
    if( tmp == NULL && !fflag ) {
        PrintALineThenDrop( "File (%s) not found.", f );
        error_occured = 1;
    }
    while( tmp != NULL ) {
        if( tmp->attr & _A_RDONLY )
            chmod( tmp->name, PMODE_RW );

        if( iflag ) {
            PrintALine( "Delete %s (y\\n)", tmp->name );
            while( ( i = tolower( getch() ) ) != 'y' && i != 'n' )
                ;
            DropALine();
            if( i == 'y' )
                remove( tmp->name );
        } else {
            if( !sflag )
                PrintALine( "Deleting file %s", tmp->name );
            remove( tmp->name );
        }

        ftail = tmp;
        tmp = tmp->next;
        MemFree( ftail );
    }

    /* process any directories found */
    if( rflag && ( tmp = dhead ) != NULL ) {
        while( tmp != NULL ) {
            RecursiveRM( tmp->name );
            dtail = tmp;
            tmp = tmp->next;
            MemFree( dtail );
        }
    }
}

/* DoRMdir - perform RM on a specified directory */
void DoRMdir( const char *dir )
{
    unsigned    attribute;
    int         rc;

    if( ( rc = rmdir( dir ) ) == -1 ) {
        _dos_getfileattr( dir, &attribute );
        if( attribute & _A_RDONLY ) {
            if( fflag ) {
                _dos_setfileattr( dir, _A_NORMAL );
                rc = rmdir( dir );
            } else {
                PrintALineThenDrop( "Directory %s is read-only, use -f", dir );
                error_occured = 1;
            }
        }
    }
    if( rc == -1 ) {
        PrintALineThenDrop( "Unable to delete directory %s", dir );
        error_occured = 1;
    } else if( !sflag )
        PrintALine( "Deleting directory %s", dir );
}

/* RecursiveRM - do an RM recursively on all files */
void RecursiveRM( const char *dir )
{
    int         i;
    char        fname[_MAX_PATH];

    /* purge the files */
    strcpy( fname, dir );
    strcat( fname, ( rxflag ) ? "/*" : "/*.*" );
    DoRM( fname );

    /* purge the directory */
    if( iflag ) {
        PrintALine( "Delete directory %s (y\\n)", dir );
        while( ( i = tolower( getch() ) ) != 'y' && i != 'n' )
            ;
        DropALine();
    } else
        i = 'y';
    if( i == 'y' )
        DoRMdir( dir );
}

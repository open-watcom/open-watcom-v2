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
   RM.C - perform unix-like rm function.

   Date         By              Reason
   ====         ==              ======
   06-jul-89
    ...
   24-aug-91    Craig Eisler    original development
   28-jan-92    Craig Eisler    cleanup
   28-jan-92    G.R.Bentz       make -r flag delete files
                                if -f is also specified
   25-mar-92    Craig Eisler    NT port
   19-jun-92    Craig Eisler    use GetOpt
   20-jun-92    Craig Eisler    use FileMatch (regular expressions)
   23-jun-92    Craig Eisler    use FileMatchNoRx
   26-jun-92    D.J.Gaudet      if rxflag use "\*" instead of "\*.*"
   07-jul-92    D.J.Gaudet      HPFS support
   16-dec-92    Brad Brisco     print message if no files are found
   11-jan-95    A.F.Scian       made 16-dec-92 respect silent flag
 */
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dos.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <direct.h>
#include <conio.h>
#include "lineprt.h"
#include "misc.h"
#include "getopt.h"
#include "filerx.h"
#include "fnutils.h"

#define TRUE    1
#define FALSE   0

char *OptEnvVar="rm";

static char * usageMsg[] = {
    "Usage: rm [-?firsX] [files]",
    "\tfiles       : files/directories to delete",
    "\tOptions: -? : print this list",
    "\t\t -f : force deletion of read-only files",
    "\t\t -i : inquire about each deletion",
    "\t\t -r : recursively delete all directories",
    "\t\t -s : silent operation",
    "\t\t -X : match files by regular expressions",
    NULL
};

int  rflag=FALSE,iflag=FALSE,fflag=FALSE,sflag=FALSE,rxflag=FALSE;

typedef struct dd {
  struct dd *next;
  char attr;
  char name[1];
} iolist;

/*
 * start of mainline
 */
main( int argc, char *argv[] )
{
    int i,ch;
    DIR *d;

    /*
     * process options
     */
    while( 1 ) {
        ch = GetOpt( &argc, argv, "frisX", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case 'X':
            rxflag=TRUE;
            break;
        case 'f':
            fflag=TRUE;
            break;
        case 'r':
            rflag=TRUE;
            break;
        case 'i':
            iflag=TRUE;
            break;
        case 's':
            sflag=TRUE;
            break;
        }
    }
    if( argc < 2 ) {
        Quit( usageMsg, "No filename/directory specified\n" );
    }

    StartPrint();

    /*
     * process -r option
     */
    if( rflag ) {
        for(i=1;i<argc;i++) {
            if( (rxflag && !strcmp( argv[i],"*" ) ) ||
                (!rxflag && !strcmp( argv[i], "*.*" ) ) ) {
                RecursiveRM( "." );
            } else {
                if( FileNameWild( argv[i], rxflag ) ) {
                    d = NULL;
                } else {
                    d = opendir( argv[i] );
                }
                if( d != NULL ) {
                    if( !(d->d_attr & _A_SUBDIR) ) {
                        closedir( d );
                        if( fflag ) {
                            DoRM( argv[i] );
                        } else {
                            PrintALineThenDrop( "%s is not a directory.", argv[i] );
                        }
                    } else {
                        closedir( d );
                        RecursiveRM( argv[i] );
                    }
                } else {
                    PrintALineThenDrop( "Directory %s not found.", argv[i] );
                }
            }
        }
    } else {
        /*
         * run through all specified files
         */
        for(i=1;i<argc;i++) {
            DoRM( argv[i] );
        }
    }
    DropALine();
    exit( 0 );

} /* main */

/*
 * DoRM - perform RM on a specified file
 */
void DoRM( char *f )
{
    iolist              *fhead=NULL,*ftail,*tmp;
    iolist              *dhead=NULL,*dtail;
    char                *bo,fpath[_MAX_PATH],tmppath[_MAX_PATH];
    int                 i,j,k,l;
    int                 len;
    DIR                 *d;
    struct dirent       *nd;
    char                wild[_MAX_PATH];
    char                *err;
    void                *crx;

    /*
     * get file path prefix
     */
    fpath[0] = 0;
    for( i=strlen(f);i>=0;i--) {
        if( f[i] == ':' || f[i] == '\\' ) {
            fpath[i+1] = 0;
            for(j=i;j>=0;j--) fpath[j] = f[j];
            i = -1;
        }
    }
    d = OpenDirAll( f, wild );
    if( d == NULL ) {
        PrintALineThenDrop( "File (%s) not found.", f );
        return;
    }

    if( rxflag ) {
        err = FileMatchInit( &crx, wild );
        if( err != NULL ) {
            Die( "\"%s\": %s\n", err, wild );
        }
    }

    k = strlen( fpath );
    while( (nd = readdir( d )) != NULL ) {

        FNameLower( nd->d_name );
        if( rxflag ) {
            if( !FileMatch( crx, nd->d_name ) ) {
                continue;
            }
        } else {
            if( !FileMatchNoRx( nd->d_name, wild ) ) {
                continue;
            }
        }
        /*
         * set up file name, then try to delete it
         */
        l = strlen( nd->d_name );
        bo = tmppath;
        for( i=0;i<k;i++ ) {
            *bo++ = fpath[i];
        }
        for( i=0;i<l;i++) {
            *bo++ = nd->d_name[i];
        }
        *bo = 0;
        if( nd->d_attr & _A_SUBDIR ) {

            /*
             * process a directory
             */
            if( !IsDotOrDotDot( nd->d_name ) ) {
                if( rflag ) {
                    /*
                     * build directory list
                     */
                    len = strlen( tmppath );
                    tmp = MemAlloc( sizeof( iolist ) + len );
                    if( dhead == NULL ) {
                        dhead = dtail = tmp;
                    } else {
                        dtail->next = tmp;
                        dtail = tmp;
                    }
                    memcpy( tmp->name, tmppath, len+1 );
                } else {
                    PrintALineThenDrop( "%s is a directory, use -r", tmppath );
                }
            }

        } else if( ( nd->d_attr & _A_RDONLY ) && !fflag ) {

            PrintALineThenDrop( "%s is read-only, use -f", tmppath );

        } else {

            /*
             * build file list
             */
            len = strlen( tmppath );
            tmp = MemAlloc( sizeof(iolist) + len );
            if( fhead == NULL ) {
                fhead = ftail = tmp;
            } else {
                ftail->next = tmp;
                ftail = tmp;
            }
            memcpy( tmp->name, tmppath, len+1 );
            tmp->attr = nd->d_attr;

        }

    }
    closedir( d );
    if( rxflag ) {
        FileMatchFini( crx );
    }

    /*
     * process any files found
     */
    tmp = fhead;
    if( tmp == NULL ) {
        if( !sflag ) {
            PrintALineThenDrop( "File (%s) not found.", f );
        }
    }
    while( tmp != NULL ) {

        if( tmp->attr & _A_RDONLY ) chmod( tmp->name, S_IWRITE | S_IREAD );

        if( iflag ) {
            PrintALine( "Delete %s (y\\n)", tmp->name );
            i = getch();
            while( i != 'y' && i != 'n' ) i=getch();
            DropALine();
            if( i=='y' ) remove( tmp->name );
        } else {
            if( !sflag ) PrintALine( "Deleting file %s", tmp->name );
            remove( tmp->name );
        }

        ftail = tmp;
        tmp = tmp->next;
        MemFree( ftail );

    }

    /*
     * process any directories found
     */
    if( rflag && (tmp=dhead) != NULL ) {
        while( tmp != NULL ) {
            RecursiveRM( tmp->name );
            dtail = tmp;
            tmp = tmp->next;
            MemFree( dtail );
        }
    }

} /* DoRM */

/*
 * DoRMdir - perform RM on a specified directory
 */
void DoRMdir( char *dir )
{
    unsigned attribute;
    int rc;

    if( (rc = rmdir( dir )) == -1 ) {
        _dos_getfileattr( dir, &attribute );
        if( attribute & _A_RDONLY ) {
            if( fflag ) {
                _dos_setfileattr( dir, _A_NORMAL );
                rc = rmdir( dir );
            } else {
                PrintALineThenDrop( "Directory %s is read-only, use -f", dir );
            }
        }
    }
    if( rc == -1 ) {
        PrintALineThenDrop( "Unable to delete directory %s", dir );
    } else {
        if( !sflag ) PrintALine( "Deleting directory %s", dir );
    }
} /* DoRMdir */

/*
 * RecursiveRM - do an RM recursively on all files
 */
void RecursiveRM( char *dir )
{
    int         i;
    char        fname[ _MAX_PATH ];

    /*
     * purge the files
     */
    strcpy(fname,dir);
    if( rxflag ) {
        strcat( fname, "\\*" );
    } else {
        strcat(fname,"\\*.*");
    }
    DoRM( fname );

    /*
     * purge the directory
     */
    if( iflag ) {
        PrintALine( "Delete directory %s (y\\n)", dir );
        i = getch();
        while( i != 'y' && i != 'n' ) i=getch();
        DropALine();
        if( i=='y' ) DoRMdir( dir );
    } else {
        DoRMdir( dir );
    }

} /* RecursiveRM */

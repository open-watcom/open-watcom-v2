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
* Description:  POSIX cp utility
*               Copies files and directories
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dos.h>
#include <direct.h>
#include "watcom.h"
#include "cp.h"
#include "getopt.h"
#include "filerx.h"
#include "iopath.h"

char *OptEnvVar="cp";

static void parseTD( char *, char, unsigned *, unsigned *, unsigned * );
static void doneCP( void );

static const char * usageMsg[] = {
    "Usage: cp [-?srpnifaX] [-t<ti>] [-T<ti>] [-d<da>] [-D<da>] [files] dest",
    "\t[files]        : source files/directories",
    "\tdest           : destination directory/file",
    "\tOptions: -?    : print this list",
    "\t\t -s    : silent operation",
    "\t\t -r    : recursively copy all subdirectories",
    "\t\t -p    : preserve file attributes",
    "\t\t -n    : do NOT preserve time/date stamp",
    "\t\t -i    : interactive verify of all file overwrites",
    "\t\t -f    : force overwrite of read-only files",
    "\t\t -a    : archival cp - copy files with archive bit on, and",
    "\t\t\t turn off archive bit of source once copied.",
    "\t\t -t<ti>: time copy - copy files with time of or after <ti>",
    "\t\t\t <ti> can be hh:mm:ss or hh:mm or hh",
    "\t\t -d<da>: date copy - copy files with date of or after <da>",
    "\t\t\t<da> can be yy-mm-dd or yy-mm or yy",
    "\t\t -T<ti>: like -t, but copy files with time of or before <ti>",
    "\t\t -D<da>: like -d, but copy files with date of or before <da>",
    "\t\t -X    : match files by regular expressions",
    NULL
};

/*
 * main - handle command line parms
 */
int main( int argc, char *argv[] )
{
    int                 i;
    char                destination[_MAX_PATH];
    char                c;
    timedate            *t_d;
    int                 rc;
    struct find_t       ft;
    int                 ch;

    /*
     * initialization
     */
    MemInit();
    StartPrint();

    /*
     * get options
     */
    while( (ch = GetOpt( &argc, argv, "Xsaifpnrt:T:d:D:", usageMsg )) != -1 ) {
        switch( ch ) {
        case 'X':
            rxflag = TRUE;
            break;
        case 'm':
            break;
        case 'r':
            rflag=TRUE;
            break;
        case 'n':
            npflag=FALSE;
            break;
        case 'p':
            pattrflag=TRUE;
            break;
        case 'f':
            fflag=TRUE;
            break;
        case 'i':
            iflag=TRUE;
            break;
        case 'a':
            aflag=TRUE;
            break;
        case 's':
            sflag=TRUE;
            break;
        case 't':
        case 'T':
            todflag = TRUE;
            if( ch =='T' ) {
                Tflag1=TRUE;
                t_d = &before_t_d;
            } else {
                tflag2=TRUE;
                t_d = &after_t_d;
            }
            parseTD( OptArg,':',&t_d->hr,&t_d->min, &t_d->sec );
            break;
        case 'd':
        case 'D':
            todflag = TRUE;
            if( ch =='D' ) {
                Dflag1=TRUE;
                t_d = &before_t_d;
            } else {
                dflag2=TRUE;
                t_d = &after_t_d;
            }
            parseTD( OptArg,'-',&t_d->yy,&t_d->mm, &t_d->dd );
            t_d->yy += 1900;
            break;
        }
    }

    /*
     * get destination directory/file, and validate it
     */
    if( argc < 2 ) {
        Quit( usageMsg, "No filename specified\n" );
    }
    if( argc == 2 ) {
        if( !strcmp( argv[1], "?" ) ) {
            Quit( usageMsg, NULL );
        }
        strcpy( destination,"." );
        argc++;
    } else {
        strcpy( destination, argv[ argc-1 ] );
    }
    rc = _dos_findfirst( destination, _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_SUBDIR | _A_ARCH, &ft );

    StartTime = clock();

    /*
     * see if destination is a directory
     */
    c = destination[ strlen(destination)-1 ];
    if( ( !rc && (ft.attrib & _A_SUBDIR) ) || c=='.' || IS_PATH_SEP( c ) ) {
        if( !IS_PATH_SEP( c ) ) {
            size_t len = strlen( destination );
            destination[len++] = DIR_SEP;
            destination[len] = '\0';
        }
        for( i=1;i<argc-1;i++ ) {
            DoCP( argv[i] , destination );
        }
        doneCP();
        return( 0 );
    }

    /*
     * destination not a directory, can only copy two files
     */
    if( argc < 3 ) {
        Quit( usageMsg, "Only one filename specified\n" );
    }
    if( argc > 3 ) {
        Quit( usageMsg, "Only one destination file allowed\n" );
    }
    CopyOneFile( argv[2], argv[1] );
    doneCP();
    return( 0 );
} /* main */

/*
 * parseTD - parse a time or a date string
 */
static void parseTD( char *arg, char pc, unsigned *a, unsigned *b, unsigned *c  )
{
    size_t      len;
    char        tmp[3];

    len = strlen( arg );
    if( len != 2 && len != 5 && len != 8 ) {
        Quit( usageMsg, InvalidTDStr );
    }
    tmp[2] = 0;

    if( !isdigit( arg[0] ) || !isdigit( arg[1] )) {
        Quit( usageMsg, InvalidTDStr );
    }
    tmp[0] = arg[0]; tmp[1] = arg[1];
    *a = atoi(tmp);

    if( len > 2 ) {
        if( arg[2] != pc ) {
            Quit( usageMsg, InvalidTDStr );
        }
        if( !isdigit( arg[3] ) || !isdigit( arg[4] )) {
            Quit( usageMsg, InvalidTDStr );
        }
        tmp[0] = arg[3]; tmp[1] = arg[4];
        *b = atoi(tmp);
        if( len == 8 ) {
            if( arg[2] != pc ) {
                Quit( usageMsg, InvalidTDStr );
            }
            if( !isdigit( arg[6] ) || !isdigit( arg[7] )) {
                Quit( usageMsg, InvalidTDStr );
            }
            tmp[0] = arg[6]; tmp[1] = arg[7];
            *c = atoi(tmp);
        }
    }

} /* parseTD */

/*
 * doneCP - as it sounds
 */
static void doneCP( void )
{
    time_t              secs,hunds;

    FlushMemoryBlocks();
    if( !sflag && DumpCnt > 1 ) {
        PrintALineThenDrop( "Total bytes copied:        %ld",TotalBytes );
        PrintALineThenDrop( "Total files copied:        %u",TotalFiles );
        if( rflag ) {
            PrintALineThenDrop( "Total directories created: %u", TotalDirs );
        }
        secs = (TotalTime/CLOCKS_PER_SEC);
        hunds = TotalTime-secs*CLOCKS_PER_SEC;
        PrintALineThenDrop( "Total time taken:          %ld.%02ld seconds",secs,hunds );
    }
    MemFini();

} /* doneCP */

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
* Description:  WBIND for the Win386 extender.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include <sys/types.h>
#include "wio.h"
#include "watcom.h"
#include "banner.h"
#include "exedos.h"
#include "exeos2.h"
#include "exephar.h"
#include "clibext.h"

#undef _WBIND_VERSION_
#define _WBIND_VERSION_ "2.3"

#define IO_BUFF 64000
#define TRUE 1
#define FALSE 0
#define MAGIC_OFFSET    0x38L
#define MAX_DESC        80
#ifdef BOOTSTRAP
#define RC_STR          "bwrc"
#else
#define RC_STR          "wrc"
#endif

static int quietFlag=FALSE;

static void updateNHStuff( int handle, char *modname, char *desc )
{
    dos_exe_header      dh;
    os2_exe_header      nh;
    long                off;
    int                 len;

    lseek( handle, 0, SEEK_SET );
    read( handle, &dh, sizeof( dh ) );
    off = (dh.file_size-1)*512L + dh.mod_size;
    lseek( handle, off, SEEK_SET );
    read( handle, &nh, sizeof( nh ) );
    off += nh.resident_off+1L;
    lseek( handle, off, SEEK_SET );
    write( handle, modname, 8 );

    if( desc == NULL ) {
        desc = modname;
        len = 8;
    } else {
        len = strlen( desc );
        if( len > MAX_DESC ) {
            len = MAX_DESC;
        }
    }
    off = nh.nonres_off+1L;
    lseek( handle, off, SEEK_SET );
    write( handle, desc, len );
}


static void myPrintf( char *str, ... )
{
    va_list     al;

    if( !quietFlag ) {
        va_start( al, str );
        vprintf( str, al );
        va_end( al );
    }

} /* myPrintf */

static void doError( char *str, ... )
{
    va_list     al;

    va_start( al, str );
    vprintf( str, al );
    va_end( al );
    printf("\n");
    exit( 1 );

} /* doError */

static void doBanner( void )
{
    myPrintf( banner1w( "Win386 Bind Utility",_WBIND_VERSION_ ) "\n" );
    myPrintf( banner2 "\n" );
    myPrintf( banner2a( "1991" ) "\n" );
    myPrintf( banner3 "\n" );
    myPrintf( banner3a "\n" );
    myPrintf("\n");
}


static void doUsage( char *str )
{
    quietFlag = FALSE;
    doBanner();
    if( str != NULL ) {
        printf( "Error - %s\n\n", str );
    }

    printf("Usage:  wbind [file] [-udnq] [-D \"<desc>\"] [-s <supervisor>] [-R <rc options>]\n" );
    printf("        [file] is the name of the 32-bit windows exe to bind\n" );
    printf("        -u              : unbind a bound executable\n" );
    printf("        -d              : build a 32-bit dll\n" );
    printf("        -n              : no resource compile required\n" );
    printf("        -q              : quiet mode\n" );
    printf("        -D \"<desc>\"     : specify the description field\n" );
    printf("        -s <supervisor> : specifies the path/name of the windows supervisor\n" );
    printf("        -R <rc options> : all options after -R are passed to the resource\n" );
    printf("                          compiler. Note that ONLY the options after -R are\n" );
    printf("                          given to the resource compiler.\n" );
    exit( 0 );

} /* doUsage */

static void *myAlloc( size_t amount )
{
    void        *tmp;

    tmp = malloc( amount );
    if( tmp == NULL ) {
        doError("Out of memory!");
    }
    return( tmp );
}

static void errPrintf( char *str, ... )
{
    va_list     al;

    va_start( al, str );
    vfprintf( stderr, str, al );
    va_end( al );

} /* errPrintf */

static long CopyFile( int in, int out, char *infile, char *outfile )
{
    unsigned    size;
    unsigned    len;
    unsigned    bufsize;
    long        totalsize;
    void        *buff;

    buff = myAlloc( IO_BUFF );
    bufsize = IO_BUFF;
    totalsize = 0L;
    for( ;; ) {
        size = read( in, buff, bufsize );
        if( size == 0 ) {
            break;
        }

        if( size == -1 ) {
            doError( "Error reading file \"%s\"", infile );
        }
        len = write( out, buff, size );
        if( len != size ) {
            doError( "Error writing file \"%s\"", outfile );
        }
        totalsize += len;
        if( (unsigned) size != bufsize ) {
            break;
        }
    }
    free( buff );
    return( totalsize );
}

void FindExtender( char *extname, char *winext )
{
    char        *watcom;

    _searchenv( extname, "PATH", winext );
    if( winext[0] == '\0' ) {
        watcom = getenv( "WATCOM" );
        if( watcom != NULL ) {
            strcpy( winext, watcom );
#if defined( __UNIX__ )
            strcat( winext, "/binw/" );
#else
            strcat( winext, "\\binw\\" );
#endif
            strcat( winext, extname );
            if( access( winext, R_OK ) == -1 ) {
                winext[0] = '\0';               // indicate file not found
            }
        }
        if( winext[0] == '\0' ) {
            doError( "Could not find \"%s\" in your path", extname );
        }
    }
}

int main( int argc, char *argv[] )
{
    int             in, out, i, rcparm = 0, pcnt;
    int             Rflag = FALSE, nflag = FALSE;
    int             uflag = FALSE;
    int             dllflag = FALSE;
    char            *wext = NULL;
    long            tsize = 0;
    char            drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME];
    char            ext[_MAX_EXT];
    char            rex[_MAX_PATH];
    char            exe[_MAX_PATH];
    char            dll[_MAX_PATH];
    char            res[_MAX_PATH];
    char            winext[_MAX_PATH];
    char            rc[256];
    long            totalsize;
    const char      **arglist;
    char            *path = NULL;
    int             currarg,len;
    simple_header   re;
    long            exelen;
    char            *desc = NULL;

    /*
     * get parms
     */
    if( argc < 2 ) {
        doUsage( NULL );
    }
    currarg=1;
    while( currarg < argc ) {
#ifdef __UNIX__
        if( argv[ currarg ][0] == '-' ) {
#else
        if( argv[ currarg ][0] == '/' || argv[ currarg ][0] == '-' ) {
#endif
            len = strlen( argv[ currarg ] );
            for( i=1; i<len; i++ ) {
                switch( argv[ currarg ][i] ) {
                case '?': doUsage( NULL );
                case 'D':
                    currarg++;
                    desc = argv[ currarg ];
                    break;
                case 's':
                    currarg++;
                    wext = argv[ currarg ];
                    break;
                case 'q':
                    quietFlag = TRUE;
                    break;
                case 'u':
                    uflag = TRUE;
                    break;
                case 'n':
                    nflag = TRUE;
                    break;
                case 'd':
                    dllflag = TRUE;
                    break;
                case 'R': case 'r':
                    Rflag=TRUE;
                    rcparm = currarg+1;
                    if( rcparm == argc ) {
                        doUsage("must specify resource compiler command line" );
                    }
                    break;
                }
            }
            if( Rflag ) {
                break;
            }
        } else {
            if( path != NULL ) {
                doUsage( "Only one executable may be specified" );
            }
            path = argv[ currarg ];
        }
        currarg++;
    }
    if( path == NULL ) {
        doUsage( "No executable to bind" );
    }
    doBanner();

    /*
     * get files to use
     */
    _splitpath( path, drive, dir, fname, ext );
    _makepath( rex, drive, dir, fname, ".rex" );
    if( dllflag ) {
        _makepath( dll, drive, dir, fname, ".dll" );
    }
    _makepath( exe, drive, dir, fname, ".exe" );
    _makepath( res, drive, dir, fname, "" );

    /*
     * do the unbind
     */
    if( uflag ) {
        if( ext[0] == 0 ) {
            path = exe;
        }
        in = open( path, O_RDONLY | O_BINARY );
        if( in < 0 ) {
            doError( "Could not open %s", path );
        }
        out = open( rex, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, PMODE_RWX );
        if( out < 0 ) {
            doError( "Could not open %s", rex );
        }
        lseek( in, MAGIC_OFFSET, SEEK_SET );
        read( in, &exelen, sizeof( unsigned_32 ) );
        lseek( in, exelen, SEEK_SET );
        read( in, &re, sizeof( re ) );
        if( re.signature != ('M' & ('Q' << 8)) ) {
            doError( "Not a bound Open Watcom 32-bit Windows application" );
        }
        lseek( in, exelen, SEEK_SET );
        CopyFile( in, out, path, rex );
        close( in );
        close( out );
        myPrintf( ".rex file %s created", rex );
        exit( 0 );
    }

    if( wext == NULL ) {
        if( dllflag ) {
            FindExtender( "w386dll.ext", winext );
        } else {
            FindExtender( "win386.ext", winext );
        }
    } else {
        strcpy( winext, wext );
    }
    if( dllflag ) {
        myPrintf("Loading 32-bit Windows DLL Supervisor \"%s\"\n",winext );
    } else {
        myPrintf("Loading 32-bit Windows Supervisor \"%s\"\n",winext );
    }

    /*
     * open files
     */
    in = open( winext, O_RDONLY | O_BINARY );
    if( in < 0 )  {
        doError( "Could not open %s", winext );
    }
    out = open( exe, O_CREAT | O_TRUNC|O_WRONLY | O_BINARY, PMODE_RWX );
    if( out < 0 )  {
        doError( "Could not open %s", exe );
    }

    /*
     * copy extender over
     */
    CopyFile( in, out, winext, exe );
    close( in );
    close( out );

    /*
     * run the resource compiler
     */
    if( !nflag ) {
        myPrintf( "Invoking the resource compiler...\n" );
        if( Rflag ) {
            strcpy( rc, RC_STR );
            arglist = myAlloc( sizeof(char *) *(argc-rcparm +3) );
            pcnt = 1;
            for( i=rcparm;i<argc;i++ ) {
                arglist[pcnt++] = argv[i];
                strcat( rc," " );
                strcat( rc, argv[i] );
            }
        } else {
            sprintf( rc, RC_STR " %s", res );
            arglist = myAlloc( sizeof(char *) * 3 );
            arglist[1] = res;
            pcnt = 2;
        }
        arglist[0] = RC_STR;
        arglist[pcnt] = NULL;

        myPrintf( "%s\n",rc );
        i = spawnvp( P_WAIT, RC_STR, arglist );
        if( i == -1 ) {
            remove( exe );
            switch( errno ) {
            case E2BIG:
                doError( "Argument list too big. Resource compiler step failed." );
                break;
            case ENOENT:
                doError( "Could not find " RC_STR ".exe." );
                break;
            case ENOMEM:
                doError( "Not enough memory. Resource compiler step failed." );
                break;
            }
            doError( "Unknown error %d, resource compiler step failed.", errno );
        }
        if( i != 0 ) {
            remove( exe );
            errPrintf( "Resource compiler failed, return code = %d\n", i );
            exit( i );
        }
    }

    /*
     * copy the rex file onto the end
     */
    in = open( rex, O_RDONLY | O_BINARY );
    if( in < 0 )  {
        doError( "Could not open %s", rex );
    }
    out = open( exe, O_RDWR | O_BINARY );
    if( out < 0 )  {
        doError( "Could not open %s", exe );
    }
    lseek( out, 0, SEEK_END );
    tsize = tell( out );

    totalsize = CopyFile( in, out, rex, exe );
    close( in );

    /*
     * noodle the file: change name, and then
     * write the file size into the old exe header (for
     * use by the loader)
     */
    lseek( out, MAGIC_OFFSET, SEEK_SET );
    write( out, &tsize, sizeof( tsize ) );
    len = strlen( fname );
    memset( &fname[len],' ',8-len );
    updateNHStuff( out, fname, desc );
    close( out );
    if( dllflag ) {
        remove( dll );
        rename( exe,dll );
        myPrintf("Created \"%s\" (%ld + %ld = %ld bytes)\n", dll,
                tsize,totalsize, tsize+totalsize );
    } else {
        myPrintf("Created \"%s\" (%ld + %ld = %ld bytes)\n", exe,
                tsize,totalsize, tsize+totalsize );
    }

    return( 0 );
} /* main */

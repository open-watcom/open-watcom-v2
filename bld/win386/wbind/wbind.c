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
#include "bool.h"
#include "watcom.h"
#include "banner.h"
#include "exedos.h"
#include "exeos2.h"
#include "exephar.h"

#include "clibext.h"


#undef _WBIND_VERSION_
#define _WBIND_VERSION_ "2.3"

#define IO_BUFF         64000
#define MAX_DESC        80
#ifdef BOOTSTRAP
#define RC_STR          "bwrc"
#else
#define RC_STR          "wrc"
#endif

static bool quietFlag = false;

static void normalizeFName( char *dst, size_t maxlen, const char *src )
/***********************************************************************
 * Removes doublequote characters from filename and copies other content
 * from src to dst. Only maxlen number of characters are copied to dst
 * including terminating NUL character. Returns value 1 when quotes was
 * removed from orginal filename, 0 otherwise.
 */
{
    char    string_open = 0;
    size_t  pos = 0;
    char    c;

    // leave space for NUL terminator
    maxlen--;

    while( (c = *src++) != '\0' && pos < maxlen ) {
        if( c == '"' ) {
            string_open = !string_open;
            continue;
        }
        if( string_open && c == '\\' ) {
            c = *src++;
            if( c != '"' ) {
                *dst++ = '\\';
                pos++;
                if( pos >= maxlen ) {
                    break;
                }
            }
        }
#ifndef __UNIX__
        if( c == '/' )
            c = '\\';
#endif
        *dst++ = c;
        pos++;
    }
    *dst = '\0';
}

static void updateNHStuff( FILE *fp, const char *modname, const char *desc )
{
    dos_exe_header      dh;
    os2_exe_header      nh;
    long                off;
    size_t              len;

    fseek( fp, 0, SEEK_SET );
    fread( &dh, 1, sizeof( dh ), fp );
    off = dh.file_size * 512L - (-dh.mod_size & 0x1ff);
    fseek( fp, off, SEEK_SET );
    fread( &nh, 1, sizeof( nh ), fp );
    off += nh.resident_off + 1L;
    fseek( fp, off, SEEK_SET );
    fwrite( modname, 1, 8, fp );

    if( desc == NULL ) {
        desc = modname;
        len = 8;
    } else {
        len = strlen( desc );
        if( len > MAX_DESC ) {
            len = MAX_DESC;
        }
    }
    off = nh.nonres_off + 1L;
    fseek( fp, off, SEEK_SET );
    fwrite( desc, 1, len, fp );
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
    myPrintf( banner2a( 1991 ) "\n" );
    myPrintf( banner3 "\n" );
    myPrintf( banner3a "\n" );
    myPrintf("\n");
}


static void doUsage( char *str )
{
    quietFlag = false;
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

static long CopyFile( FILE *in, FILE *out, const char *infile, const char *outfile )
{
    size_t      size;
    size_t      len;
    long        totalsize;
    void        *buff;

    buff = myAlloc( IO_BUFF );
    totalsize = 0L;
    for( ;; ) {
        size = fread( buff, 1, IO_BUFF, in );
        if( size != IO_BUFF ) {
            if( ferror( in ) ) {
                doError( "Error reading file \"%s\"", infile );
            }
        }
        len = fwrite( buff, 1, size, out );
        if( len != size ) {
            doError( "Error writing file \"%s\"", outfile );
        }
        totalsize += (long)len;
        if( size != IO_BUFF ) {
            break;
        }
    }
    free( buff );
    return( totalsize );
}

static void FindExtender( char *extname, char *winext )
{
    char        *watcom;
    FILE        *fp;

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
            if( (fp = fopen( winext, "r" )) != NULL ) {
                fclose( fp );
            } else {
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
    FILE            *in;
    FILE            *out;
    int             i, rcparm = 0, pcnt;
    bool            Rflag = false;
    bool            nflag = false;
    bool            uflag = false;
    bool            dllflag = false;
    char            *wext = NULL;
    unsigned_32     exelen = 0;
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
    int             currarg;
    size_t          len;
    simple_header   re;
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
        if( argv[currarg][0] == '-' ) {
#else
        if( argv[currarg][0] == '/' || argv[currarg][0] == '-' ) {
#endif
            len = strlen( argv[currarg] );
            for( i = 1; i < len; i++ ) {
                switch( argv[currarg][i] ) {
                case '?': doUsage( NULL );
                case 'D':
                    currarg++;
                    desc = argv[currarg];
                    break;
                case 's':
                    currarg++;
                    wext = argv[currarg];
                    break;
                case 'q':
                    quietFlag = true;
                    break;
                case 'u':
                    uflag = true;
                    break;
                case 'n':
                    nflag = true;
                    break;
                case 'd':
                    dllflag = true;
                    break;
                case 'R': case 'r':
                    Rflag = true;
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
            path = argv[currarg];
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
    normalizeFName( path, strlen( path ) + 1, path );
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
        in = fopen( path, "rb" );
        if( in == NULL ) {
            doError( "Could not open %s", path );
        }
        out = fopen( rex, "wb" );
        if( out == NULL ) {
            doError( "Could not open %s", rex );
        }
        fseek( in, NH_MAGIC_REX, SEEK_SET );
        fread( &exelen, 1, sizeof( exelen ), in );
        fseek( in, exelen, SEEK_SET );
        fread( &re, 1, sizeof( re ), in );
        if( re.signature != ('M' & ('Q' << 8)) ) {
            doError( "Not a bound Open Watcom 32-bit Windows application" );
        }
        fseek( in, exelen, SEEK_SET );
        CopyFile( in, out, path, rex );
        fclose( in );
        fclose( out );
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
        normalizeFName( winext, sizeof( winext ), wext );
    }
    if( dllflag ) {
        myPrintf( "Loading 32-bit Windows DLL Supervisor \"%s\"\n", winext );
    } else {
        myPrintf( "Loading 32-bit Windows Supervisor \"%s\"\n", winext );
    }

    /*
     * open files
     */
    in = fopen( winext, "rb" );
    if( in == NULL )  {
        doError( "Could not open %s", winext );
    }
    out = fopen( exe, "wb" );
    if( out == NULL )  {
        doError( "Could not open %s", exe );
    }

    /*
     * copy extender over
     */
    CopyFile( in, out, winext, exe );
    fclose( in );
    fclose( out );

    /*
     * run the resource compiler
     */
    if( !nflag ) {
        myPrintf( "Invoking the resource compiler...\n" );
        if( Rflag ) {
            strcpy( rc, RC_STR );
            arglist = myAlloc( sizeof( char * ) * ( argc - rcparm + 3 ) );
            pcnt = 1;
            for( i=rcparm;i<argc;i++ ) {
                arglist[pcnt++] = argv[i];
                strcat( rc," " );
                strcat( rc, argv[i] );
            }
        } else {
            sprintf( rc, RC_STR " %s", res );
            arglist = myAlloc( sizeof( char * ) * 3 );
            arglist[1] = res;
            pcnt = 2;
        }
        arglist[0] = RC_STR;
        arglist[pcnt] = NULL;

        myPrintf( "%s\n",rc );
        i = (int)spawnvp( P_WAIT, RC_STR, arglist );
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
    in = fopen( rex, "rb" );
    if( in == NULL )  {
        doError( "Could not open %s", rex );
    }
    out = fopen( exe, "rb+" );
    if( out == NULL )  {
        doError( "Could not open %s", exe );
    }
    fseek( out, 0, SEEK_END );
    exelen = ftell( out );

    totalsize = CopyFile( in, out, rex, exe );
    fclose( in );

    /*
     * noodle the file: change name, and then
     * write the file size into the old exe header (for
     * use by the loader)
     */
    fseek( out, NH_MAGIC_REX, SEEK_SET );
    fwrite( &exelen, 1, sizeof( exelen ), out );
    len = strlen( fname );
    if( len < 8 ) {
        memset( &fname[len], ' ', 8 - len );
    }
    updateNHStuff( out, fname, desc );
    fclose( out );
    if( dllflag ) {
        remove( dll );
        rename( exe, dll );
        myPrintf( "Created \"%s\" (%ld + %ld = %ld bytes)\n", dll,
                exelen, totalsize, exelen + totalsize );
    } else {
        myPrintf( "Created \"%s\" (%ld + %ld = %ld bytes)\n", exe,
                exelen, totalsize, exelen + totalsize );
    }

    return( 0 );
} /* main */

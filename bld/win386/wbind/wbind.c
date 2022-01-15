/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "pathgrp2.h"

#include "clibext.h"


#define IO_BUFF         64000
#define MAX_DESC        80
#ifdef BOOTSTRAP
#define WRC_STR         "bwrc"
#else
#define WRC_STR         "wrc"
#endif

typedef struct file_str {
    FILE        *fp;
    const char  *name;
} file_str;

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
    size_t  pos;
    char    c;

    if( maxlen ) {
        // leave space for NUL terminator
        maxlen--;
        for( pos = 0; (c = *src++) != '\0' && pos < maxlen; pos++ ) {
            if( c == '"' ) {
                string_open = !string_open;
                continue;
            }
            if( c == '\\' && string_open ) {
                c = *src++;
                if( c != '"' ) {
                    dst[pos] = '\\';
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
            dst[pos] = c;
        }
        dst[pos] = '\0';
    }
}

static bool updateNHStuff( FILE *fp, const char *basename, const char *desc )
{
    dos_exe_header      dh;
    os2_exe_header      nh;
    long                off;
    size_t              len;
    char                modname[8];

    len = strlen( basename );
    if( len < 8 ) {
        memcpy( modname, basename, len );
        memset( modname + len, ' ', 8 - len );
    } else {
        memcpy( modname, basename, 8 );
    }
    fseek( fp, 0, SEEK_SET );
    if( fread( &dh, 1, sizeof( dh ), fp ) != sizeof( dh ) )
        return( false );
    off = dh.file_size * 512L - (-dh.mod_size & 0x1ff);
    if( fseek( fp, off, SEEK_SET ) )
        return( false );
    if( fread( &nh, 1, sizeof( nh ), fp ) != sizeof( nh ) )
        return( false );
    off += nh.resident_off + 1L;
    if( fseek( fp, off, SEEK_SET ) )
        return( false );
    if( fwrite( modname, 1, 8, fp ) != 8 )
        return( false );
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
    if( fseek( fp, off, SEEK_SET ) )
        return( false );
    return( fwrite( desc, 1, len, fp ) == len );
}


static void myPrintf( const char *str, ... )
{
    va_list     args;

    if( !quietFlag ) {
        va_start( args, str );
        vprintf( str, args );
        va_end( args );
    }

} /* myPrintf */

static void doError( const char *str, ... )
{
    va_list     args;

    va_start( args, str );
    vfprintf( stderr, str, args );
    va_end( args );
    fputs( "\n", stderr );
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


static void doUsage( const char *str )
{
    quietFlag = false;
    doBanner();
    if( str != NULL ) {
        printf( "Error - %s\n\n", str );
    }

    printf("Usage:  wbind [file] [-udnq] [-D \"<desc>\"] [-s <supervisor>] [-R <rc options>]\n" );
    printf("        [file] is the name of the 32-bit windows executable to bind\n" );
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
        doError( "Out of memory!" );
    }
    return( tmp );
}

static void errPrintf( const char *str, ... )
{
    va_list     args;

    va_start( args, str );
    vfprintf( stderr, str, args );
    va_end( args );

} /* errPrintf */

static long copy_file( file_str *in, file_str *out )
{
    size_t      size;
    size_t      len;
    long        totalsize;
    void        *buff;
    bool        ok;

    totalsize = 0L;
    ok = false;
    buff = malloc( IO_BUFF );
    if( buff == NULL ) {
        errPrintf( "Out of memory!\n" );
    } else {
        ok = true;
        for( ;; ) {
            size = fread( buff, 1, IO_BUFF, in->fp );
            if( size != IO_BUFF ) {
                if( ferror( in->fp ) ) {
                    errPrintf( "Error reading file \"%s\"\n", in->name );
                    ok = false;
                    break;
                }
            }
            len = fwrite( buff, 1, size, out->fp );
            if( len != size ) {
                errPrintf( "Error writing file \"%s\"\n", out->name );
                ok = false;
                break;
            }
            totalsize += (long)len;
            if( size != IO_BUFF ) {
                break;
            }
        }
        free( buff );
    }
    if( !ok ) {
        fclose( in->fp );
        fclose( out->fp );
        exit( 1 );
    }
    return( totalsize );
}

#if defined( __UNIX__ )
    #define WATCOM_SUBDIR   "/binw/"
#else
    #define WATCOM_SUBDIR   "\\binw\\"
#endif
#define DLL_NAME            "w386dll.ext"
#define EXE_NAME            "win386.ext"

static void FindExtender( bool dllflag, char *ext_path_name )
{
    char        *watcom;
    FILE        *fp;
    const char  *extender_name;
    size_t      len;

    if( dllflag ) {
        extender_name = DLL_NAME;
    } else {
        extender_name = EXE_NAME;
    }
    _searchenv( extender_name, "PATH", ext_path_name );
    if( ext_path_name[0] == '\0' ) {
        watcom = getenv( "WATCOM" );
        if( watcom != NULL ) {
            strncpy( ext_path_name, watcom, _MAX_PATH - 1 );
            ext_path_name[_MAX_PATH - 1] = '\0';
            len = strlen( ext_path_name );
            if( dllflag ) {
                strncpy( ext_path_name + len, WATCOM_SUBDIR DLL_NAME, _MAX_PATH - 1 - len );
            } else {
                strncpy( ext_path_name + len, WATCOM_SUBDIR EXE_NAME, _MAX_PATH - 1 - len );
            }
            ext_path_name[_MAX_PATH - 1] = '\0';
            if( (fp = fopen( ext_path_name, "r" )) != NULL ) {
                fclose( fp );
            } else {
                watcom = NULL;
            }
        }
        if( watcom == NULL ) {
            doError( "Could not find \"%s\" in your path", extender_name );
        }
    }
}

static FILE *open_file( const char *name, const char *mode )
{
    FILE    *fp;

    fp = fopen( name, mode );
    if( fp == NULL ) {
        doError( "Could not open %s", name );
    }
    return( fp );
}

int main( int argc, char *argv[] )
{
    file_str        in;
    file_str        out;
    bool            Rflag = false;
    bool            nflag = false;
    bool            uflag = false;
    bool            dllflag = false;
    const char      *wext = NULL;
    unsigned_32     u32;
    pgroup2         pg;
    char            rex_name[_MAX_PATH];
    char            exe_name[_MAX_PATH];
    char            res_name[_MAX_PATH];
    char            ext_name[_MAX_PATH];
    char            wrc_cmd[256];
    int             wrc_parm;
    long            totalsize;
    long            exelen;
    const char      **arglist;
    const char      *path = NULL;
    int             currarg;
    simple_header   re;
    char            *desc = NULL;
    int             rc;
    bool            ok;

    /*
     * get parms
     */
    if( argc < 2 ) {
        doUsage( NULL );
    }
    wrc_parm = 0;
    for( currarg = 1; currarg < argc; currarg++ ) {
#ifdef __UNIX__
        if( argv[currarg][0] == '-' ) {
#else
        if( argv[currarg][0] == '/' || argv[currarg][0] == '-' ) {
#endif
            size_t  i;
            size_t  len;

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
                    wrc_parm = currarg + 1;
                    if( wrc_parm == argc ) {
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
    }
    if( path == NULL ) {
        doUsage( "No executable to bind" );
    }
    doBanner();

    /*
     * get files to use
     */
    normalizeFName( ext_name, sizeof( ext_name ), path );
    _splitpath2( ext_name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( uflag ) {
        if( pg.ext[0] == '\0' ) {
            pg.ext = "exe";
        }
    } else if( dllflag ) {
        pg.ext = "dll";
    } else {
        pg.ext = "exe";
    }
    _makepath( exe_name, pg.drive, pg.dir, pg.fname, pg.ext );
    _makepath( rex_name, pg.drive, pg.dir, pg.fname, "rex" );
    _makepath( res_name, pg.drive, pg.dir, pg.fname, NULL );

    if( uflag ) {
        /*
         * do the unbind
         */
        in.name = exe_name;
        in.fp = open_file( in.name, "rb" );
        fseek( in.fp, NH_MAGIC_REX, SEEK_SET );
        fread( &u32, 1, sizeof( u32 ), in.fp );
        exelen = u32;
        fseek( in.fp, exelen, SEEK_SET );
        fread( &re, 1, sizeof( re ), in.fp );
        if( re.signature != ('M' & ('Q' << 8)) ) {
            fclose( in.fp );
            doError( "Not a bound Open Watcom 32-bit Windows application" );
        }
        fseek( in.fp, exelen, SEEK_SET );
        out.name = rex_name;
        out.fp = open_file( out.name, "wb" );
        copy_file( &in, &out );
        fclose( in.fp );
        fclose( out.fp );
        myPrintf( ".rex file %s created", out.name );
        return( 0 );
    }

    if( wext == NULL ) {
        FindExtender( dllflag, ext_name );
    } else {
        normalizeFName( ext_name, sizeof( ext_name ), wext );
    }
    if( dllflag ) {
        myPrintf( "Loading 32-bit Windows DLL Supervisor \"%s\"\n", ext_name );
    } else {
        myPrintf( "Loading 32-bit Windows Supervisor \"%s\"\n", ext_name );
    }

    /*
     * copy extender over
     */
    in.name = ext_name;
    in.fp = open_file( in.name, "rb" );
    out.name = exe_name;
    out.fp = open_file( out.name, "wb" );
    copy_file( &in, &out );
    fclose( in.fp );
    fclose( out.fp );

    /*
     * run the resource compiler
     */
    if( !nflag ) {
        myPrintf( "Invoking the resource compiler...\n" );
        if( Rflag ) {
            int     pcnt;
            int     i;

            strcpy( wrc_cmd, WRC_STR );
            arglist = myAlloc( sizeof( char * ) * ( argc - wrc_parm + 3 ) );
            pcnt = 1;
            for( i = wrc_parm; i < argc; i++ ) {
                arglist[pcnt++] = argv[i];
                strcat( wrc_cmd, " " );
                strcat( wrc_cmd, argv[i] );
            }
            arglist[pcnt] = NULL;
        } else {
            sprintf( wrc_cmd, WRC_STR " %s", res_name );
            arglist = myAlloc( sizeof( char * ) * 3 );
            arglist[1] = res_name;
            arglist[2] = NULL;
        }
        arglist[0] = WRC_STR;

        myPrintf( "%s\n", wrc_cmd );
        rc = (int)spawnvp( P_WAIT, WRC_STR, arglist );
        if( rc == -1 ) {
            remove( exe_name );
            switch( errno ) {
            case E2BIG:
                doError( "Argument list too big. Resource compiler step failed." );
                break;
            case ENOENT:
                doError( "Could not find " WRC_STR ".exe." );
                break;
            case ENOMEM:
                doError( "Not enough memory. Resource compiler step failed." );
                break;
            }
            doError( "Unknown error %d, resource compiler step failed.", errno );
        }
        if( rc != 0 ) {
            remove( exe_name );
            errPrintf( "Resource compiler failed, return code = %d\n", rc );
            exit( rc );
        }
    }

    /*
     * copy the rex file onto the end
     */
    in.name = rex_name;
    in.fp = open_file( in.name, "rb" );
    out.name = exe_name;
    out.fp = open_file( out.name, "rb+" );
    exelen = 0;
    totalsize = 0;
    ok = false;
    if( fseek( out.fp, 0, SEEK_END ) == 0 ) {
        exelen = ftell( out.fp );
        totalsize = copy_file( &in, &out );
        ok = true;
    }
    fclose( in.fp );
    if( ok ) {
        /*
         * noodle the file: change name, and then
         * write the file size into the old exe header (for
         * use by the loader)
         */
        ok = false;
        if( fseek( out.fp, NH_MAGIC_REX, SEEK_SET ) == 0 ) {
            u32 = exelen;
            if( fwrite( &u32, 1, sizeof( u32 ), out.fp ) == sizeof( u32 ) ) {
                if( updateNHStuff( out.fp, pg.fname, desc ) ) {
                    ok = true;
                }
            }
        }
    }
    fclose( out.fp );
    if( ok ) {
        myPrintf( "Created \"%s\" (%ld + %ld = %ld bytes)\n", exe_name,
                exelen, totalsize, exelen + totalsize );
        return( 0 );
    }
    errPrintf( "Error writing executable \"%s\".\n", exe_name );
    return( 1 );
} /* main */

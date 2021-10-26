/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  VI ctags utility.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#if defined( __UNIX__ )
    #include <dirent.h>
    #include <sys/stat.h>
#else
    #include <direct.h>
#endif
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <fnmatch.h>
#endif
#include "pathgrp2.h"
#include "ctags.h"
#include "banner.h"

#include "clibext.h"


#define CMPFEXT(e,c)    (e[0] == '.' && stricmp(e + 1, c) == 0)

static const char       *usageMsg[] = {
    "Usage: ctags [-?adempstqvxy] [-z[a,c,f]] [-f<fname>] [files] [@optfile]",
    "\t[files]\t    : source files (may be C, C++, or FORTRAN)",
    "\t\t      file names may contain wild cards (* and ?)",
    "\t[@optfile]  : specifies an option file",
    "\tOption File Directives:",
    "\t\t option <opts>: any command line options (no dashes).",
    "\t\t\t\tan option line resets the d,m,s and t options.",
    "\t\t\t\tthey must be specified on option line to",
    "\t\t\t\tremain in effect",
    "\t\t file <flist> : a list of files, separated by commas",
    "\tOptions: -?\t   : print this list",
    "\t\t -a\t   : append output to existing tags file",
    "\t\t -c\t   : add classes (C++ files)",
    "\t\t -d\t   : add all #defines (C,C++ files)",
    "\t\t -e\t   : add enumerated constants (C,C++ files)",
    "\t\t -f<fname> : specify alternate tag file (default is \"tags\")",
    "\t\t -m\t   : add #defines (macros only) (C,C++ files)",
    "\t\t -p\t   : add prototypes to tags file",
    "\t\t -s\t   : add structs, enums and unions (C,C++ files)",
    "\t\t -t\t   : add typedefs (C,C++ files)",
    "\t\t -q\t   : quiet operation",
    "\t\t -v\t   : verbose operation",
    "\t\t -x\t   : add all possible tags (same as -cdst)",
    "\t\t -y\t   : add all non-c++ tags (same as -dst)",
    "\t\t -z[a,c,f] : assume files are of type ASM, C/C++, or FORTRAN",
    "\tOptions may be specified in a CTAGS environment variable",
    NULL
};

static char optStr[] = "acdempstqvxyf:z:";

static bool             quietFlag;
static bool             appendFlag;
static char             *fileName = "tags";
static char             tmpFileName[_MAX_PATH];
static file_type        fileType = TYPE_NONE;

#if defined( __UNIX__ )
static int _stat2( const char *path, const char *name, struct stat *st )
{
    char        full_name[_MAX_PATH];

    _makepath( full_name, NULL, path, name, NULL );
    return( stat( full_name, st ) );
}
#endif

static bool skipEntry( const char *path, const char *mask, struct dirent *dire )
{
#ifdef __UNIX__
  #if defined( __QNX__ )
    if( (dire->d_stat.st_status & _FILE_USED) == 0 )
        _stat2( path, dire->d_name, &dire->d_stat );
    return( S_ISDIR( dire->d_stat.st_mode ) || fnmatch( mask, dire->d_name, FNM_NOESCAPE ) != 0 );
  #else
    struct stat     st;

    _stat2( path, dire->d_name, &st );
    return( S_ISDIR( st.st_mode ) || fnmatch( mask, dire->d_name, FNM_NOESCAPE ) != 0 );
  #endif
#else
    /* unused parameters */ (void)path; (void)mask;

    return( (dire->d_attr & (_A_VOLID | _A_SUBDIR)) != 0 );
#endif
}

static void displayBanner( void )
{
    if( quietFlag ) {
        return;
    }
    printf( banner1w( "CTAGS Utility", "1.0" ) "\n" );
    printf( banner2 "\n" );
    printf( banner2a( 1984 ) "\n" );
    printf( banner3 "\n" );
    printf( banner3a "\n" );

} /* displayBanner */

void ErrorMsgExit( const char *str, ... )
{
    va_list     args;
//    int         len;

    va_start( args, str );
//    len = vfprintf( stderr, str, args );
    vfprintf( stderr, str, args );
    va_end( args );
    exit( 1 );
}

/*
 * Quit - print usage messages
 */
static void Quit( const char **usage_msg, const char *str, ... )
{
    va_list     args;
    int         i;
    int         cnt;

    /* unused parameters */ (void)usage_msg;

    if( str != NULL ) {
        va_start( args, str );
        vfprintf( stderr, str, args );
        va_end( args );
        cnt = 1;
    } else {
        cnt = sizeof( usageMsg ) / sizeof( char * );
    }

    for( i = 0; i < cnt; i++ ) {
        fprintf( stderr, "%s\n", usageMsg[i] );
    }
    exit( EXIT_FAILURE );

} /* Quit */

/*
 * doOption - handle a single option
 */
static void doOption( int ch )
{
    switch( ch ) {
    case 'a':
        appendFlag = true;
        break;
    case 'c':
        WantClasses = true;
        break;
    case 'd':
        WantAllDefines = true;
        break;
    case 'e':
        WantEnums = true;
        break;
    case 'f':
        fileName = optarg;
        break;
    case 'm':
        WantMacros = true;
        break;
    case 'p':
        WantProtos = true;
        break;
    case 's':
        WantUSE = true;
        break;
    case 't':
        WantTypedefs = true;
        break;
    case 'q':
        quietFlag = true;
        break;
    case 'x':
        WantClasses = true;
        /* fall through */
    case 'y':
        WantAllDefines = true;
        WantMacros = true;
        WantUSE = true;
        WantTypedefs = true;
        break;
    case 'v':
        VerboseFlag = true;
        break;
    case 'z':
        switch( optarg[0] ) {
        case 'a':
            fileType = TYPE_ASM;
            break;
        case 'c':
            fileType = TYPE_C;
            break;
        case 'f':
            fileType = TYPE_FORTRAN;
            break;
        default:
            Quit( usageMsg, "Invalid file type\n" );
            break;
        }
    }

} /* doOption */

/*
 * processFile - process a specified file
 */
static void processFile( const char *arg )
{
    pgroup2     pg;
    file_type   ftype;
    unsigned    tag_count;

    StartFile( arg );
    _splitpath2( arg, pg.buffer, NULL, NULL, NULL, &pg.ext );
    if( fileType == TYPE_NONE ) {
        ftype = TYPE_C;
        if( CMPFEXT( pg.ext, "for" ) ) {
            ftype = TYPE_FORTRAN;
        } else if( CMPFEXT( pg.ext, "fi" ) ) {
            ftype = TYPE_FORTRAN;
        } else if( CMPFEXT( pg.ext, "pas" ) ) {
            ftype = TYPE_PASCAL;
        } else if( CMPFEXT( pg.ext, "cpp" ) ) {
            ftype = TYPE_CPLUSPLUS;
        } else if( CMPFEXT( pg.ext, "asm" ) ) {
            ftype = TYPE_ASM;
        }
    } else {
        ftype = fileType;
    }
    tag_count = 0;
    if( VerboseFlag ) {
        printf( "Processing %s", arg );
        tag_count = GetTagCount();
        fflush( stdout );
    }
    switch( ftype ) {
    case TYPE_C:
        ScanC();
        break;
    case TYPE_CPLUSPLUS:
        ScanC();
        break;
    case TYPE_FORTRAN:
        ScanFortran();
        break;
    case TYPE_PASCAL:
        ScanFortran();
        break;
    case TYPE_ASM:
        ScanAsm();
        break;
    }
    if( VerboseFlag ) {
        printf( ", %u tags.\n", GetTagCount() - tag_count );
    }
    EndFile();

} /* processFile */

/*
 * processFileList - process a possible file list
 */
static void processFileList( const char *fullmask )
{
    DIR                 *dirp;
    struct dirent       *dire;
    const char          *tmp;
    bool                has_wild;
    char                fullname[_MAX_PATH];
    char                path[_MAX_PATH];
    char                mask[_MAX_PATH];
    pgroup2             pg1;
    pgroup2             pg2;

    has_wild = false;
    for( tmp = fullmask; *tmp != '\0'; tmp++ ) {
        if( *tmp == '*' || *tmp == '?' ) {
            has_wild = true;
            break;
        }
    }

    if( !has_wild ) {
        processFile( fullmask );
        return;
    }

    _splitpath2( fullmask, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, &pg1.ext );
    _makepath( path, pg1.drive, pg1.dir, NULL, NULL );
    _makepath( mask, NULL, NULL, pg1.fname, pg1.ext );
    dirp = opendir( fullmask );
    if( dirp != NULL ) {
        while( (dire = readdir( dirp )) != NULL ) {
            if( skipEntry( path, mask, dire ) )
                continue;
            _splitpath2( dire->d_name, pg2.buffer, NULL, NULL, &pg2.fname, &pg2.ext );
            _makepath( fullname, pg1.drive, pg1.dir, pg2.fname, pg2.ext );
#ifndef __UNIX__
            strlwr( fullname );
#endif
            processFile( fullname );
        }
        closedir( dirp );
    }

} /* processFileList */

/*
 * processOptionFile - process an option file
 */
static void processOptionFile( const char *fname )
{
    FILE        *optfile;
    char        option[MAX_STR];
    char        *ptr;
    char        *cmd, *arg;
    int         ch;

    optfile = fopen( fname, "r" );
    if( optfile == NULL ) {
        printf( "Could not open option file %s\n", fname );
    } else {
        while( (ptr = fgets( option, sizeof( option ), optfile )) != NULL ) {
            SKIP_SPACES( ptr );
            if( *ptr == '#' || *ptr == '\0' ) {
                continue;
            }
            cmd = ptr;
            SKIP_NOSPACES( ptr );
            if( *ptr == '\0' ) {
                continue;
            }
            *ptr = '\0';
            SKIP_CHAR_SPACES( ptr );
            if( *ptr == '\0' ) {
                continue;
            }
            if( stricmp( cmd, "file" ) == 0 ) {
                for( ;; ) {
                    arg = ptr;
                    SKIP_LIST_NOWS( ptr );
                    ch = *ptr;
                    *ptr = '\0';
                    processFileList( arg );
                    if( ch == '\0' ) {
                        break;
                    }
                    ptr++;
                    SKIP_LIST_WS( ptr );
                    if( *ptr == '\0' ) {
                        break;
                    }
                }
            } else if( stricmp( cmd, "option" ) == 0 ) {
                WantTypedefs = false;
                WantMacros = false;
                WantAllDefines = false;
                WantUSE = false;
                for( ; *ptr != '\0'; ptr++ ) {
                    if( *ptr == 'f' ) {
                        SKIP_CHAR_SPACES( ptr );
                        if( *ptr != '\0' ) {
                            strcpy( tmpFileName, ptr );
                            ptr = tmpFileName;
                            SKIP_NOSPACES( ptr );
                            *ptr = '\0';
                            optarg = tmpFileName;
                            doOption( 'f' );
                        }
                        break;
                    }
                    doOption( *ptr );
                }
            }
        }
        fclose( optfile );
    }

} /* processOptionFile */


int main( int argc, char *argv[] )
{
    int         ch, i;

    while( (ch = getopt( argc, argv, optStr )) != -1 ) {
        if( ch == '?' ) {
            Quit( usageMsg, NULL );
        }
        doOption( ch );
    }
    displayBanner();

    if( argc < 2 ) {
        Quit( usageMsg, "No files specified\n" );
    }

    if( quietFlag ) {
        VerboseFlag = false;
    }

    for( i = 1; i < argc; i++ ) {
        if( argv[i][0] == '@' ) {
            processOptionFile( &argv[i][1] );
        } else {
            processFileList( argv[i] );
        }
    }
    if( appendFlag ) {
        if( VerboseFlag ) {
            printf( "Generated %u tags.\n", GetTagCount() );
        }
        ReadExtraTags( fileName );
    }
    GenerateTagsFile( fileName );
    return( 0 );

} /* main */

/*
 * IsTokenChar - determine if a character is part of a token
 */
bool IsTokenChar( int ch )
{
    return( isalnum( ch ) || ch == '_' );

} /* IsTokenChar */

/*
 * MyStricmp - ignore trailing null, advance buf pointer
 */
int MyStricmp( char **buf, char *literal )
{
    int     ret;
    size_t  len;
    char    *bufptr;
    char    save_ch;

    len = strlen( literal );
    bufptr = *buf;
    save_ch = bufptr[len];
    bufptr[len] = '\0';
    ret = stricmp( *buf, literal );
    bufptr[len] = save_ch;
    if( ret == 0 ) {
        (*buf) += len;
    }
    return( ret );

} /* MyStricmp */

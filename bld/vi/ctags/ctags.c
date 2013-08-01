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
#include "clibext.h"
#include "ctags.h"
#include "banner.h"

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

static void displayBanner( void )
{
    if( quietFlag ) {
        return;
    }
    printf( banner1w( "CTAGS Utility", "1.0" ) "\n" );
    printf( banner2 "\n" );
    printf( banner2a( "1984" ) "\n" );
    printf( banner3 "\n" );
    printf( banner3a "\n" );

} /* displayBanner */

void ErrorMsgExit( const char *str, ... )
{
    va_list     al;
//    int         len;

    va_start( al, str );
//    len = vfprintf( stderr, str, al );
    vfprintf( stderr, str, al );
    va_end( al );
    exit( 1 );
}

/*
 * Quit - print usage messages
 */
static void Quit( const char **usage_msg, const char *str, ... )
{
    va_list     al;
    int         i;
    int         cnt;

    usage_msg = usage_msg;
    if( str != NULL ) {
        va_start( al, str );
        vfprintf( stderr, str, al );
        va_end( al );
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
        appendFlag = TRUE;
        break;
    case 'c':
        WantClasses = TRUE;
        break;
    case 'd':
        WantAllDefines = TRUE;
        break;
    case 'e':
        WantEnums = TRUE;
        break;
    case 'f':
        fileName = optarg;
        break;
    case 'm':
        WantMacros = TRUE;
        break;
    case 'p':
        WantProtos = TRUE;
        break;
    case 's':
        WantUSE = TRUE;
        break;
    case 't':
        WantTypedefs = TRUE;
        break;
    case 'q':
        quietFlag = TRUE;
        break;
    case 'x':
        WantClasses = TRUE;
        /* fall through */
    case 'y':
        WantAllDefines = TRUE;
        WantMacros = TRUE;
        WantUSE = TRUE;
        WantTypedefs = TRUE;
        break;
    case 'v':
        VerboseFlag = TRUE;
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
static void processFile( char *arg )
{
    char        buff[_MAX_EXT + 5];
    char        *ext;
    file_type   ftype;
    unsigned    tagcnt;

    StartFile( arg );
    _splitpath2( arg, buff, NULL, NULL, NULL, &ext );
    if( fileType == TYPE_NONE ) {
        ftype = TYPE_C;
        if( !stricmp( ext, ".for" ) ) {
            ftype = TYPE_FORTRAN;
        } else if( !stricmp( ext, ".fi" ) ) {
            ftype = TYPE_FORTRAN;
        } else if( !stricmp( ext, ".pas" ) ) {
            ftype = TYPE_PASCAL;
        } else if( !stricmp( ext, ".cpp" ) ) {
            ftype = TYPE_CPLUSPLUS;
        } else if( !stricmp( ext, ".asm" ) ) {
            ftype = TYPE_ASM;
        }
    } else {
        ftype = fileType;
    }
    tagcnt = 0;
    if( VerboseFlag ) {
        printf( "Processing %s", arg );
        tagcnt = TagCount;
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
        printf( ", %u tags.\n", TagCount - tagcnt );
    }
    EndFile();

} /* processFile */

/*
 * processFileList - process a possible file list
 */
static void processFileList( char *ptr )
{
    DIR                 *dirp;
    struct dirent       *dirent;
    char                *tmp;
    bool                has_wild = FALSE;
    char                buff1[_MAX_PATH2];
    char                buff2[_MAX_PATH2];
    char                *drive;
    char                *dir;
    char                *fname;
    char                *ext;
    char                path[_MAX_PATH];

    tmp = ptr;
    while( *tmp != 0 ) {
        if( *tmp == '*' || *tmp == '?' ) {
            has_wild = TRUE;
            break;
        }
        tmp++;
    }

    if( !has_wild ) {
        processFile( ptr );
        return;
    }

    _splitpath2( ptr, buff1, &drive, &dir, &fname, &ext );
    dirp = opendir( ptr );
    if( dirp == NULL ) {
        return;
    }
    while( (dirent = readdir( dirp )) != NULL ) {

#ifdef __UNIX__
        {
            struct stat buf;
            stat( dirent->d_name, &buf );
            if ( S_ISDIR( buf.st_mode ) )
                continue;
        }
#else
        if( dirent->d_attr & (_A_SUBDIR | _A_VOLID) ) {
            continue;
        }
#endif

        _splitpath2( dirent->d_name, buff2, NULL, NULL, &fname, &ext );
        _makepath( path, drive, dir, fname, ext );
        strlwr( path );
        processFile( path );
    }
    closedir( dirp );

} /* processFileList */

/*
 * processOptionFile - process an option file
 */
static void processOptionFile( char *fname )
{
    FILE        *optfile;
    char        option[MAX_STR];
    char        *ptr, *cmd, *arg;
    int         ch;

    optfile = fopen( fname, "r" );
    if( optfile == NULL ) {
        printf( "Could not open option file %s\n", fname );
        return;
    }
    while( fgets( option, sizeof( option ), optfile ) != NULL ) {
        ptr = option;
        while( isspace( *ptr ) ) {
            ptr++;
        }
        if( *ptr == '#' || *ptr == 0 ) {
            continue;
        }
        cmd = ptr;
        while( !isspace( *ptr ) && *ptr ) {
            ptr++;
        }
        if( *ptr == 0 ) {
            continue;
        }
        *ptr = 0;
        ptr++;
        while( isspace( *ptr ) ) {
            ptr++;
        }
        if( *ptr == 0 ) {
            continue;
        }
        if( !stricmp( cmd, "file" ) ) {
            for( ;; ) {
                arg = ptr;
                while( !isspace( *ptr ) && *ptr != ',' && *ptr != 0 ) {
                    ptr++;
                }
                ch = *ptr;
                *ptr = 0;
                processFileList( arg );
                if( ch == 0 ) {
                    break;
                }
                ptr++;
                while( isspace( *ptr ) || *ptr == ',' ) {
                    ptr++;
                }
                if( *ptr == 0 ) {
                    break;
                }
            }
        } else if( !stricmp( cmd, "option" ) ) {
            WantTypedefs = FALSE;
            WantMacros = FALSE;
            WantAllDefines = FALSE;
            WantUSE = FALSE;
            while( *ptr != 0 ) {
                if( *ptr == 'f' ) {
                    ptr++;
                    while( isspace( *ptr ) ) {
                        ptr++;
                    }
                    if( *ptr == 0 ) {
                        break;
                    }
                    strcpy( tmpFileName, ptr );
                    ptr = tmpFileName;
                    while( !isspace( *ptr ) ) {
                        ptr++;
                    }
                    *ptr = 0;
                    optarg = tmpFileName;
                    doOption( 'f' );
                    break;
                }
                doOption( *ptr );
                ptr++;
            }
        }

    }
    fclose( optfile );

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
        VerboseFlag = FALSE;
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
            printf( "Generated %u tags.\n", TagCount );
        }
        ReadExtraTags( fileName );
    }
    GenerateTagsFile( fileName );
    return( 0 );

} /* main */

/*
 * IsTokenChar - determine if a character is part of a token
 */
bool IsTokenChar( char ch )
{
    if( isalnum( ch ) ) {
        return( TRUE );
    }
    if( ch == '_' ) {
        return( TRUE );
    }
    return( FALSE );

} /* IsTokenChar */

/*
 * MyStricmp - ignore trailing null, advance buf pointer
 */
int MyStricmp( char **buf, char *literal )
{
    int     ret;
    int     len;
    char    *bufptr;
    char    save_ch;

    len = strlen( literal );
    bufptr = *buf;
    save_ch = bufptr[len];
    bufptr[len] = 0;
    ret = stricmp( *buf, literal );
    bufptr[len] = save_ch;
    if( ret == 0 ) {
        (*buf) += len;
    }
    return( ret );

} /* MyStricmp */

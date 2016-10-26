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
* Description:  VI editor bind utility.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "wio.h"
#include "watcom.h"
#include "bool.h"
#include "banner.h"
#include "bnddata.h"

#include "clibext.h"


#define isWSorCtrlZ(x)  (isspace( x ) || (x == 0x1A))

#define MAX_LINE_LEN    1024
#define COPY_SIZE       0x8000 - 512
#define MAX_DATA_FILES  255

char    *dats[MAX_DATA_FILES];

short   FileCount;
bool    sflag = false;
bool    qflag = false;
char    _bf[] = "edbind.dat";
char    *bindfile = _bf;

static void Banner( void )
{
    if( qflag ) {
        return;
    }
    printf( banner1w( "Editor Bind Utility", _EDBIND_VERSION_ ) "\n" );
    printf( banner2 "\n" );
    printf( banner2a( "1984" ) "\n" );
    printf( banner3 "\n" );
    printf( banner3a "\n" );
}

/*
 * Abort - made a boo-boo
 */
static void Abort( char *str, ... )
{
    va_list     al;

    va_start( al, str );
    vprintf( str, al );
    va_end( al );
    printf( "\n" );
    exit( 1 );

} /* Abort */

/*
 * MyPrintf - do a printf
 */
static void MyPrintf( char *str, ... )
{
    va_list     al;

    if( !qflag ) {
        va_start( al, str );
        vprintf( str, al );
        va_end( al );
    }

} /* MyPrintf */

/*
 * AddDataToEXE - tack data to end of an EXE
 */
static void AddDataToEXE( char *exe, char *buffer, unsigned len, unsigned long tocopy )
{
    int                 h, i, newh;
    char                buff[sizeof( MAGIC_COOKIE ) + sizeof( bind_size )];
    long                shift;
    unsigned            taillen;
    char                *copy;
    char                foo[128];
    char                drive[_MAX_DRIVE], dir[_MAX_DIR];

    /*
     * get files
     */
    copy = malloc( COPY_SIZE );
    if( copy == NULL ) {
        Abort( "Out of Memory" );
    }
    h = open( exe, O_RDWR | O_BINARY );
    if( h == -1 ) {
        Abort( "Fatal error opening \"%s\"", exe );
    }
    _splitpath( exe, drive, dir, NULL, NULL );
    _makepath( foo, drive, dir, "__cge__", ".exe" );
    newh = open( foo, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, PMODE_RWX );
    if( newh == -1 ) {
        Abort( "Fatal error opening \"%s\"", foo );
    }

    /*
     * get trailer
     */
    i = lseek( h, -((long)sizeof( buff )), SEEK_END );
    if( i == -1 ) {
        Abort( "Initial seek error on \"%s\"", exe );
    }
    i = read( h, buff, sizeof( buff ) );
    if( i == -1 ) {
        Abort( "Read error on \"%s\"", exe );
    }

    /*
     * if trailer is one of ours, then set back to overwrite data;
     * else just set to write at end of file
     */
    if( strcmp( buff, MAGIC_COOKIE ) ) {
        if( sflag ) {
            Abort( "\"%s\" does not contain configuration data!", exe );
        }
    } else {
        taillen = *((bind_size *)&(buff[sizeof( MAGIC_COOKIE )]));
        shift = (long)-((long)taillen + (long)sizeof( buff ));
        tocopy += shift;
    }
    i = lseek( h, 0, SEEK_SET );
    if( i ) {
        Abort( "Seek error on \"%s\"", exe );
    }

    /*
     * copy crap
     */
    while( tocopy > 0 ) {
        if( tocopy > (unsigned long)COPY_SIZE ) {
            i = read( h, copy, COPY_SIZE );
            if( i != COPY_SIZE ) {
                Abort( "Read error on \"%s\"", exe );
            }
            i = write( newh, copy, COPY_SIZE );
            if( i != COPY_SIZE ) {
                Abort( "Write error on \"%s\"", foo );
            }
            tocopy -= (unsigned long)COPY_SIZE;
        } else {
            i = read( h, copy, (unsigned int)tocopy );
            if( i != (int)tocopy ) {
                Abort( "Read error on \"%s\"", exe );
            }
            i = write( newh, copy, (unsigned int)tocopy );
            if( i != (int)tocopy ) {
                Abort( "Write error on \"%s\"", foo );
            }
            tocopy = 0;
        }
    }
    free( copy );
    close( h );

    /*
     * write out data and new trailer
     */
    if( !sflag ) {
        i = write( newh, buffer, len );
        if( i != (int)len ) {
            Abort( "write 1 error on \"%s\"", exe );
        }
        strcpy( buff, MAGIC_COOKIE );
        *((bind_size *)&(buff[sizeof( MAGIC_COOKIE )])) = len;
        i = write( newh, buff, sizeof( buff ) );
        if( i != sizeof( buff ) ) {
            Abort( "write 2 error on \"%s\"", exe );
        }
    }
    close( newh );
    remove( exe );
    rename( foo, exe );

} /* AddDataToEXE */

/*
 * GetFromEnv - get file name from environment
 */
static void GetFromEnv( char *what, char *path )
{
    _searchenv( what, "EDPATH", path );
    if( path[0] != '\0' ) {
        return;
    }
    _searchenv( what, "PATH", path );

} /* GetFromEnv */

/*
 * GetFromEnvAndOpen - search env and fopen a file
 */
static FILE *GetFromEnvAndOpen( char *inpath )
{
    char tmppath[_MAX_PATH];

    GetFromEnv( inpath, tmppath );
    if( tmppath[0] != '\0' ) {
        MyPrintf( " %s...", tmppath );
        return( fopen( tmppath, "r" ) );
    }
    return( NULL );

} /* GetFromEnvAndOpen */

/*
 * Usage - dump the usage message
 */
#if defined( __WATCOMC__ ) && !defined( __ALPHA__ )
    #pragma aux Usage aborts;
#endif

static void Usage( char *msg )
{
    if( msg != NULL ) {
        printf( "%s\n", msg );
    }
    printf( "Usage: edbind [-?sq] [-d<datfile>] <exename>\n" );
    if( msg == NULL ) {
        printf( "\t<exename>\t     executable to add editor data to\n" );
        printf( "\tOptions -?:\t     display this message\n" );
        printf( "\t\t-s:\t     strip info from executable\n" );
        printf( "\t\t-q:\t     run quietly\n" );
        printf( "\t\t-d<datfile>: specify data file other than edbind.dat\n" );
    }
    exit( 1 );

} /* Usage */

/*
 * SkipLeadingSpaces - skip leading spaces in a string
 */
static char *SkipLeadingSpaces( const char *buff )
{
    while( isspace( *buff ) )
        ++buff;
    return( (char *)buff );

} /* SkipLeadingSpaces */

/*
 * MyAlloc - allocate memory, failing if cannot
 */
static void *MyAlloc( size_t size )
{
    void        *tmp;

    tmp = malloc( size );
    if( tmp == NULL ) {
        Abort( "Out of Memory!" );
    }
    return( tmp );

} /* MyAlloc */


int main( int argc, char *argv[] )
{
    char                *buff = NULL;
    char                *buff2, *buff3;
    char                *buffn, *buffs;
    char                *ptr;
    int                 i, bytes, j, k, sl;
    FILE                *f;
    struct stat         fs;
    char                drive[_MAX_DRIVE], dir[_MAX_DIR];
    char                fname[_MAX_FNAME], ext[_MAX_EXT];
    char                path[_MAX_PATH];
    char                tmppath[_MAX_PATH];
    char                tmpfname[_MAX_FNAME], tmpext[_MAX_EXT];
    unsigned            cnt;
    unsigned            lines;
    bind_size           *index;
    bind_size           *entries;

    for( j = argc - 1; j > 0; --j ) {
        if( argv[j][0] == '/' || argv[j][0] == '-' ) {
            sl = strlen( argv[j] );
            for( i = 1; i < sl; i++ ) {
                switch( argv[j][i] ) {
                case 's': sflag = true; break;
                case 'q': qflag = true; break;
                case 'd':
                    bindfile = &argv[j][i + 1];
                    i = sl;
                    break;
                case '?':
                    Banner();
                    Usage( NULL );
                default:
                    Banner();
                    Usage( "Invalid option" );
                }
            }
            for( i = j; i < argc; i++ ) {
                argv[i]= argv[i + 1];
            }
            argc--;
        }
    }
    Banner();

    /*
     * now, check for null file name
     */
    if( argc < 2 ) {
        Usage( "No executable to bind" );
    }
    _splitpath( argv[1], drive, dir, fname, ext );
    if( ext[0] == '\0' ) {
        _makepath( path, drive, dir, fname, ".exe" );
    } else {
        strcpy( path, argv[1] );
    }
    if( stat( path, &fs ) == -1 ) {
        Abort( "Could not find executable \"%s\"", path );
    }

    cnt = 0;
    if( !sflag ) {

        buff = MyAlloc( 65000 );
        buff2 = MyAlloc( 32000 );
        buff3 = MyAlloc( MAX_LINE_LEN );

        /*
         * read in all data files
         */
        MyPrintf( "Getting data files from" );
        f = GetFromEnvAndOpen( bindfile );
        MyPrintf( "\n" );
        if( f == NULL ) {
            Abort( "Could not open %s", bindfile );
        }
        while( (ptr = fgets( buff3, MAX_LINE_LEN, f )) != NULL ) {
            for( i = strlen( ptr ); i && isWSorCtrlZ( ptr[i - 1] ); --i ) {
                ptr[i - 1] = '\0';
            }
            if( ptr[0] == '\0' ) {
                continue;
            }
            ptr = SkipLeadingSpaces( ptr );
            if( ptr[0] == '#' ) {
                continue;
            }
            dats[FileCount] = MyAlloc( strlen( ptr ) + 1 );
            strcpy( dats[FileCount], ptr );
            FileCount++;
            if( FileCount >= MAX_DATA_FILES ) {
                Abort( "Too many files to bind!" );
            }
        }
        fclose( f );
        index = MyAlloc( FileCount * sizeof( bind_size ) );
        entries = MyAlloc( FileCount * sizeof( bind_size ) );

        buffn = buff;

        *(bind_size *)buffn = FileCount;
        buffn += sizeof( bind_size );
        cnt += sizeof( bind_size );
        buffs = buffn;
        buffn += sizeof( bind_size );
        cnt += sizeof( bind_size );
        k = 0;
        for( i = 0; i < FileCount; i++ ) {
//          j = strlen( dats[i] ) + 1;
//          memcpy( buffn, dats[i], j );
            _splitpath( dats[i], NULL, NULL, tmpfname, tmpext );
            _makepath( tmppath, NULL, NULL, tmpfname, tmpext );
            j = strlen( tmppath ) + 1;
            memcpy( buffn, tmppath, j );
            buffn += j;
            cnt += j;
            k += j;
        }
        *(bind_size *)buffs = k + 1;  /* size of token list */
        *buffn = 0;                             /* trailing zero */
        buffn++;
        cnt++;
        buffs = buffn;
        buffn += FileCount * ( sizeof( bind_size ) + sizeof( bind_size ) );
        cnt += FileCount * ( sizeof( bind_size ) + sizeof( bind_size ) );

        for( j = 0; j < FileCount; j++ ) {
            MyPrintf( "Loading" );
            f = GetFromEnvAndOpen( dats[j] );
            if( f == NULL ) {
                Abort( "\nLoad of %s failed!", dats[j] );
            }
            setvbuf( f, buff2, _IOFBF, 32000 );
            bytes = lines = 0;
            index[j] = cnt;
            while( (ptr = fgets( buff3, MAX_LINE_LEN, f )) != NULL ) {
                unsigned    len;

                for( len = strlen( ptr ); len && isWSorCtrlZ( ptr[len - 1] ); --len )
                    ptr[len - 1] = '\0';
                if( ptr[0] == '\0' ) {
                    continue;
                }
                ptr = SkipLeadingSpaces( ptr );
                if( ptr[0] == '#' ) {
                    continue;
                }
                len = strlen( ptr );
                *buffn = (char)len;
                buffn++;
                memcpy( buffn, ptr, len );
                buffn += len;
                cnt += len + 1;
                lines++;
                bytes += len;
            }
            fclose( f );
            entries[j] = lines;
            MyPrintf( "Added %d lines (%d bytes)\n", lines, bytes );
        }
        memcpy( buffs, index, FileCount * sizeof( bind_size ) );
        buffs += FileCount * sizeof( bind_size );
        memcpy( buffs, entries, FileCount * sizeof( bind_size ) );
    }

    AddDataToEXE( path, buff, cnt, fs.st_size );
    if( !sflag ) {
        MyPrintf( "Added %d bytes to \"%s\"\n", cnt, path );
    } else {
        MyPrintf( "\"%s\" has been stripped of configuration information\n", path );
    }
    return( 0 );

} /* main */

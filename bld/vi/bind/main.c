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


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include "banner.h"

char magic_cookie[]="CGEXXX";

#define MAX_LINE_LEN 1024
#define FALSE 0
#define TRUE 1
#define COPY_SIZE 0x8000-512
#define MAX_FILES 255

char *dats[MAX_FILES];

#define MAGIC_COOKIE_SIZE sizeof( magic_cookie )

short FileCount;
long *index;
short *entries;
short sflag = FALSE;
short qflag = FALSE;
char _bf[] = "edbind.dat";
char *bindfile=_bf;

void Banner( void )
{
    if( qflag ) {
        return;
    }
    printf( "%s\n", banner1w( "Editor Bind Utility",_EDBIND_VERSION_ ) );
    printf( "%s\n", banner2a() );
    printf( "%s\n", banner3 );
}

/*
 * Abort - made a boo-boo
 */
void Abort( char *str, ... )
{
    va_list     al;

    va_start( al, str );
    vprintf( str, al );
    va_end( al );
    printf("\n");
    exit( 1 );

} /* Abort */

/*
 * MyPrintf - do a printf
 */
void MyPrintf( char *str, ... )
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
void AddDataToEXE( char *exe, char *buffer, unsigned short len,
                        unsigned long tocopy )
{
    int                 h,i,newh;
    char                buff[MAGIC_COOKIE_SIZE+3];
    long                shift;
    short               taillen;
    char                *copy;
    char                foo[128];
    char                drive[_MAX_DRIVE],dir[_MAX_DIR];

    /*
     * get files
     */
    copy = malloc( COPY_SIZE );
    if( copy == NULL ) {
        Abort( "Out of Memory" );
    }
    h = open( exe, O_RDWR | O_BINARY );
    if( h == -1 ) {
        Abort("Fatal error opening \"%s\"",exe );
    }
    _splitpath( exe, drive, dir, NULL, NULL );
    _makepath( foo, drive,dir,"__cge__",".exe" );
    newh = open( foo, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD );
    if( newh == -1 ) {
        Abort("Fatal error opening \"%s\"",foo );
    }

    /*
     * get trailer
     */
    i = lseek( h, -((long) MAGIC_COOKIE_SIZE+3L), SEEK_END );
    if( i == -1 ) {
        Abort( "Initial seek error on \"%s\"", exe );
    }
    i = read( h, buff, 3+MAGIC_COOKIE_SIZE );
    if( i == -1) {
        Abort( "Read error on \"%s\"", exe );
    }

    /*
     * if trailer is one of ours, then set back to overwrite data;
     * else just set to write at end of file
     */
    if( strcmp( buff, magic_cookie ) ) {
        if( sflag ) {
            Abort("\"%s\" does not contain configuration data!", exe);
        }
    } else {
        taillen = *( (unsigned short *) &(buff[MAGIC_COOKIE_SIZE+1]) );
        shift = (long) -((long) taillen+(long) MAGIC_COOKIE_SIZE+3);
        tocopy += shift;
    }
    i = lseek( h,0, SEEK_SET );
    if( i ) {
        Abort( "Seek error on \"%s\"",exe );
    }

    /*
     * copy crap
     */
    while( tocopy > 0 ) {
        if( tocopy > (unsigned long) COPY_SIZE ) {
            i = read( h, copy, COPY_SIZE );
            if( i != COPY_SIZE ) {
                Abort( "Read error on \"%s\"", exe );
            }
            i = write( newh, copy, COPY_SIZE );
            if( i != COPY_SIZE ) {
                Abort( "Write error on \"%s\"", foo );
            }
            tocopy -= (unsigned long) COPY_SIZE;
        } else {
            i = read( h, copy, (unsigned int) tocopy );
            if( i != tocopy ) {
                Abort( "Read error on \"%s\"", exe );
            }
            i = write( newh, copy, (unsigned int) tocopy );
            if( i != (int) tocopy ) {
                Abort( "Write error on \"%s\"", foo );
            }
            tocopy = 0;
        }
    }
    close( h );

    /*
     * write out data and new trailer
     */
    if( !sflag ) {
        i = write( newh, buffer, len );
        if( i != len ) {
            Abort( "write 1 error on \"%s\"", exe );
        }
        i = write( newh, magic_cookie, MAGIC_COOKIE_SIZE+1 );
        if( i != MAGIC_COOKIE_SIZE+1 ) {
            Abort( "write 2 error on \"%s\"", exe );
        }
        i = write( newh, &len, sizeof( short ) );
        if( i != sizeof( short ) ) {
            Abort( "write 3 error on \"%s\"", exe );
        }
    }
    close( newh );
    remove( exe );
    rename( foo, exe );

} /* AddDataToEXE */

/*
 * GetFromEnvAndOpen - search env and fopen a file
 */
FILE *GetFromEnvAndOpen( char *inpath )
{
char tmppath[256];

    GetFromEnv( inpath, tmppath );
    strlwr( tmppath );
    if( tmppath[0] != 0 ) {
        MyPrintf(" %s...",tmppath );
        return( fopen( tmppath, "r" ) );
    }
    return( NULL );

} /* GetFromEnvAndOpen */

/*
 * GetFromEnv - get file name from environment
 */
void GetFromEnv( char *what, char *path )
{
    _searchenv(what,"EDPATH",path );
    if( path[0] != 0 ) {
        return;
    }
    _searchenv(what,"PATH",path );

} /* GetFromEnv */

/*
 * Usage - dump the usage message
 */
#ifndef __ALPHA__
#pragma aux Usage aborts;
#endif
void Usage( char *msg )
{
    if( msg != NULL ) {
        printf( "%s\n", msg );
    }
    printf("Usage: edbind [-?sq] [-d<datfile>] <exename>\n");
    if( msg == NULL ) {
        printf("\t<exename>\t     executable to add editor data to\n" );
        printf("\tOptions -?:\t     display this message\n");
        printf("\t\t-s:\t     strip info from executable\n");
        printf("\t\t-q:\t     run quietly\n");
        printf("\t\t-d<datfile>: specify data file other than edbind.dat\n");
    }
    exit( 1 );

} /* Usage */

/*
 * RemoveLeadingSpaces - remove leading spaces from a string
 */
void RemoveLeadingSpaces( char *buff )
{
    short       k=0;

    if( buff[0] == 0 ) {
        return;
    }
    while( isspace( buff[k] ) ) {
        k++;
    }
    if( k==0 ) {
        return;
    }
    EliminateFirstN( buff, k );

} /* RemoveLeadingSpaces */

/*
 * EliminateFirstN - eliminate first n chars from buff
 */
void EliminateFirstN( char *buff, short n  )
{
    char        *buff2;

    buff2 = &buff[n];
    while( *buff2 != 0 ) {
        *buff++ = *buff2++;
    }
    *buff = 0;

} /* EliminateFirstN */

/*
 * MyAlloc - allocate memory, failing if cannot
 */
void *MyAlloc( unsigned size )
{
    void        *tmp;

    tmp = malloc( size );
    if( tmp == NULL ) {
        Abort("Out of Memory!" );
    }
    return( tmp );

} /* MyAlloc */


main( int argc, char *argv[] )
{
    char                *buff=NULL,*buff2,*buff3;
    char                *buffn,*buffs;
    int                 i,cnt,bytes,lines,j,k,sl;
    FILE                *f;
    struct stat         fs;
    char                drive[_MAX_DRIVE],dir[_MAX_DIR];
    char                fname[_MAX_FNAME],ext[_MAX_EXT];
    char                path[_MAX_PATH];
    char                tmppath[_MAX_PATH];
    char                tmpfname[_MAX_FNAME],tmpext[_MAX_EXT];

    j=argc-1;
    while( j > 0 ) {
        if( argv[j][0] == '/' || argv[j][0] == '-' ) {
            sl = strlen( argv[j] );
            for(i=1;i<sl;i++) {
                switch( argv[j][i] ) {
                case 's': sflag = TRUE; break;
                case 'q': qflag = TRUE; break;
                case 'd':
                    bindfile = &argv[j][i+1];
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
            for(i=j;i<(unsigned) argc;i++) {
                argv[i]=argv[i+1];
            }
            argc--;
        }
        j--;
    }
    Banner();
    /*
     * now, check for null file name
     */
    if( argc<2 ) {
        Usage( "No executable to bind" );
    }
    _splitpath( argv[1], drive, dir, fname, ext );
    if( ext[0] == 0 ) {
        _makepath( path, drive, dir, fname, ".exe" );
    } else {
        strcpy( path, argv[1] );
    }
    if( stat( path, &fs ) == -1 ) {
        Abort( "Could not find executable \"%s\"", path );
    }


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
            Abort( "Could not open %s",bindfile );
        }
        while( fgets( buff3,MAX_LINE_LEN-1,f ) != NULL ) {
            if( buff[0] == '#' ) {
                continue;
            }
            i = strlen( buff3 );
            if( i == 0 ) {
                continue;
            }
            i--;
            buff3[i] = 0;
            dats[ FileCount ] = MyAlloc( i+1 );
            strcpy( dats[ FileCount], buff3 );
            FileCount++;
            if( FileCount >= MAX_FILES ) {
                Abort( "Too many files to bind!" );
            }
        }
        fclose( f );
        index = MyAlloc( FileCount * sizeof( long ) );
        entries = MyAlloc( FileCount * sizeof( short ) );

        buffn = buff;
        cnt = 0;

        *(short *) buffn = FileCount;
        buffn += sizeof( short );
        cnt += sizeof( short );
        buffs = buffn;
        buffn += sizeof( short );
        cnt += sizeof( short );
        k = 0;
        for( i=0;i<FileCount;i++ ) {
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
        *(short *) buffs = k+1;     /* size of token list */
        *buffn = 0;                 /* trailing zero */
        buffn++;
        cnt++;
        buffs = buffn;
        buffn += FileCount*(sizeof(short)+sizeof(long));
        cnt += FileCount*(sizeof(short)+sizeof(long));

        for( j=0;j<FileCount;j++ ) {
            MyPrintf( "Loading" );
            f = GetFromEnvAndOpen( dats[j] );
            if( f == NULL ) {
                Abort("\nLoad of %s failed!",dats[j] );
            }
            setvbuf( f, buff2, _IOFBF, 32000 );
            bytes = lines = 0;
            index[j] = (long) cnt;
            while( fgets( buff3,MAX_LINE_LEN-1,f ) != NULL ) {
                if( buff3[0] == '#' ) {
                    continue;
                }
                i = strlen( buff3 );
                i--;
                buff3[i] = 0;
                RemoveLeadingSpaces( buff3 );
                if( buff3[0] == 0 ) {
                    continue;
                }
                *buffn = (char) i;
                buffn++;
                memcpy( buffn, buff3, i );
                buffn += i;
                cnt += i+1;
                lines ++;
                bytes += i;
            }
            fclose( f );
            entries[j] = lines;
            MyPrintf( "Added %d lines (%d bytes)\n",lines,bytes );
        }
        i = FileCount;
        memcpy( buffs, index, FileCount * sizeof( long ));
        buffs += FileCount * sizeof( long );
        memcpy( buffs, entries, FileCount * sizeof( short ));
    }

    AddDataToEXE( path, buff, cnt, fs.st_size );
    if( !sflag ) {
        MyPrintf( "Added %d bytes to \"%s\"\n",cnt, path );
    } else {
        MyPrintf( "\"%s\" has been stripped of configuration information\n", path );
    }
    exit( 0 );

} /* main */

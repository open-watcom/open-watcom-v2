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
* Description:  VI script pre-compiler code
*
****************************************************************************/


#include "vi.h"
#include <stdarg.h>
#include <stdlib.h>
#include "posix.h"
#include "specio.h"

#include "clibext.h"


#define MAX_SRC_LINE    512

#define isWSorCtrlZ( x )    (isspace( x ) || (x == 0x1A))

char _NEAR  SingleBlank[] = " ";
char _NEAR  SingleSlash[] = "/";
char _NEAR  SingleQuote[] = "\"";

int         SourceErrCount = 0;
line        *WorkLine;
key_map     *KeyMaps = NULL;
key_map     *InputKeyMaps = NULL;
int         MaxLine = 512;

vi_rc       LastRC      = ERR_NO_ERR;
vi_rc       LastRetCode = ERR_NO_ERR;

/*
 * MemAlloc - allocate some memory (always works, or editor aborts)
 */
void *MemAlloc( unsigned size )
{
    return( malloc( size ) );
}

/*
 * MemFree - free up memory
 */
void MemFree( void *ptr )
{
    free( ptr );

} /* MemFree */

/*
 * MemFreeList - free up memory
 */
void MemFreeList( int count, char **ptr )
{
    if( ptr != NULL ) {
        int i;
        for( i = 0; i < count; i++ ) {
            free( ptr[i] );
        }
        free( ptr );
    }

} /* MemFreeList */

/*
 * MemReAlloc - reallocate a block, and it will succeed.
 */
void *MemReAlloc( void *ptr, unsigned size )
{
    return( realloc( ptr, size ) );

} /* MemReAlloc */

/*
 * StrMerge - merge a number of strings together
 */
char *StrMerge( int cnt, char *str, ... )
{
    va_list     arg;
    char        *n;

    va_start( arg, str );
    while( cnt > 0 ) {
        n = va_arg( arg, char * );
        if( n != NULL ) {
            strcat( str, n );
        }
        cnt--;
    }
    va_end( arg );
    return( str );

} /* StrMerge */


/*
 * GetFromEnv - get file name from environment
 */
void GetFromEnv( const char *what, char *path )
{
    _searchenv( what, "EDPATH", path );
    if( path[0] != 0 ) {
        return;
    }
    _searchenv( what, "PATH", path );

} /* GetFromEnv */

/*
 * GetFromEnvAndOpen - search env and fopen a file
 */
FILE *GetFromEnvAndOpen( const char *path )
{
    char        tmppath[FILENAME_MAX];

    GetFromEnv( path, tmppath );
    if( tmppath[0] != 0 ) {
        return( fopen( tmppath, "r" ) );
    }
    return( NULL );

} /* GetFromEnvAndOpen */

/*
 * initSource - initialize language variables
 */
static vi_rc initSource( vlist *vl )
{
    VarAddStr( "*", "", vl );
    return( ERR_NO_ERR );

} /* initSource */

/*
 * finiSource - release language variables
 */
static void finiSource( labels *lab, vlist *vl, sfile *sf )
{
    sfile       *curr, *tmp;

    if( lab != NULL ) {
        MemFreeList( lab->cnt, lab->name );
        MemFree( lab->pos );
    }

    VarListDelete( vl );

    for( curr = sf; curr != NULL; curr = tmp ) {
        tmp = curr->next;

        MemFree( curr->data );
        MemFree( curr->arg1 );
        MemFree( curr->arg2 );
        MemFree( curr );
    }

} /* finiSource */

/*
 * writeScript - write a compiled script
 */
static vi_rc writeScript( const char *fn, sfile *sf, vlist *vl, srcline *sline, char *vn )
{
    sfile       *curr;
    FILE        *foo;
    char        drive[_MAX_DRIVE], directory[_MAX_DIR], name[_MAX_FNAME];
    char        path[FILENAME_MAX];
    char        tmp[MAX_SRC_LINE];
    int         token;

    vl = vl;
    /*
     * get compiled file name, and make error file
     */
    if( vn[0] == 0 ) {
        _splitpath( fn, drive, directory, name, NULL );
        _makepath( path, drive, directory, name, "._vi" );
    } else {
        strcpy( path, vn );
    }
    foo = fopen( path, "wb" );
    if( foo == NULL ) {
        return( ERR_FILE_OPEN );
    }
    MyFprintf( foo, "VBJ__\n" );

    /*
     * process all lines
     */
    *sline = 1;
    for( curr = sf->next; curr != NULL; curr = curr->next ) {

        token = curr->token;
        if( curr->data != NULL ) {
            strcpy( tmp, curr->data );
        } else {
            tmp[0] = 0;
        }
        /*
         * spew out line
         */
        MyFprintf( foo, "%c%d %s", ( curr->hasvar ) ? '1' : '0', token, tmp );
        if( token == SRC_T_GOTO ) {
            MyFprintf( foo, " %d", curr->branchcond );
        }
        MyFprintf( foo, "\n" );
        *sline += 1;

    }
    fclose( foo );
    return( ERR_NO_ERR );

} /* writeScript */

/*
 * SpecialOpen - open a file or exe
 */
bool SpecialOpen( const char *fn, GENERIC_FILE *gf )
{
    /*
     * process regular file
     */
    gf->type = GF_FILE;
    gf->gf.a.currline = 0;
    gf->data.f = GetFromEnvAndOpen( fn );
    if( gf->data.f == NULL ) {
        return( false );
    }
    return( true );

} /* SpecialOpen */

/*
 * SpecialFclose - close either file or exe
 */
void SpecialFclose( GENERIC_FILE *gf )
{
    fclose( gf->data.f );

} /* SpecialFclose */

/*
 * SpecialFgets - get from either file or exe
 */
int SpecialFgets( char *buff, int max, GENERIC_FILE *gf )
{
    int         i;

    if( fgets( buff, max, gf->data.f ) == NULL ) {
        return( -1 );
    }
    gf->gf.a.currline++;
    for( i = strlen( buff ); i && isWSorCtrlZ( buff[i - 1] ); --i ) {
        buff[i - 1] = '\0';
    }
    return( i );

} /* SpecialFgets */


static vi_rc Compile( const char *fn, char *data )
{
    labels      *lab, lb;
    vlist       vl;
    sfile       *sf;
    char        sname[FILENAME_MAX];
    vi_rc       rc;
    srcline     sline = 0;

    WorkLine = MemAlloc( sizeof( line ) + MaxLine + 2 );
    WorkLine->len = -1;
    LastRC = LastRetCode;
    vl.head = vl.tail = NULL;
    sname[0] = 0;
    GetNextWord1( data, sname );
    rc = initSource( &vl );
    if( rc == ERR_NO_ERR ) {
        SourceErrCount = 0;
        sf = NULL;
        lab = &lb;
        memset( lab, 0, sizeof( labels ) );
        rc = PreProcess( fn, &sf, lab );
        sline = CurrentSrcLine;
        if( SourceErrCount > 0 ) {
            printf( "Compile of %s finished, %d errors encountered\n", fn, SourceErrCount );
        }
        if( rc == ERR_NO_ERR && SourceErrCount == 0 ) {
            rc = writeScript( fn, sf, &vl, &sline, sname );
            finiSource( lab, &vl, sf );
        }
    }
    MemFree( WorkLine );
    return( rc );
}

int main( int argc, char **argv )
{
    vi_rc   rc;

    argc = argc;
    rc = Compile( argv[1], argv[2] );
    return( (rc == ERR_NO_ERR) ? 0 : -1 );
}

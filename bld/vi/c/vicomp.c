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
* Description:  VI script pre-compiler code
*
****************************************************************************/


#include "vi.h"
#include <stdarg.h>
#include "posix.h"
#include "specio.h"
#include "pathgrp2.h"
#include "myio.h"

#include "clibext.h"


#define MAX_SRC_LINE    512

const char _NEAR  SingleBlank[] = " ";
const char _NEAR  SingleSlash[] = "/";
const char _NEAR  SingleDQuote[] = "\"";

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
void *MemAlloc( size_t size )
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
void MemFreeList( list_linenum count, char **ptr )
{
    if( ptr != NULL ) {
        list_linenum i;
        for( i = 0; i < count; i++ ) {
            free( ptr[i] );
        }
        free( ptr );
    }

} /* MemFreeList */

/*
 * MemRealloc - reallocate a block, and it will succeed.
 */
void *MemRealloc( void *ptr, size_t size )
{
    return( realloc( ptr, size ) );

} /* MemRealloc */

/*
 * StrMerge - merge a number of strings together
 */
char *StrMerge( int cnt, char *str, ... )
{
    va_list     args;
    char        *n;

    va_start( args, str );
    for( ; cnt-- > 0; ) {
        n = va_arg( args, char * );
        if( n != NULL ) {
            strcat( str, n );
        }
    }
    va_end( args );
    return( str );

} /* StrMerge */


/*
 * GetFromEnv - get file name from environment
 */
void GetFromEnv( const char *what, char *path )
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
FILE *GetFromEnvAndOpen( const char *path )
{
    char        tmppath[_MAX_PATH];

    GetFromEnv( path, tmppath );
    if( tmppath[0] != '\0' ) {
        return( fopen( tmppath, "r" ) );
    }
    return( NULL );

} /* GetFromEnvAndOpen */

/*
 * initSource - initialize language variables
 */
static vi_rc initSource( vars_list *vl )
{
    VarAddStr( "*", "", vl );
    return( ERR_NO_ERR );

} /* initSource */

/*
 * finiSource - release language variables
 */
static void finiSource( labels *lab, vars_list *vl, sfile *sf )
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
static vi_rc writeScript( const char *fn, sfile *sf, vars_list *vl, srcline *sline, const char *vn )
{
    sfile       *curr;
    FILE        *foo;
    pgroup2     pg;
    char        path[FILENAME_MAX];
    char        tmp[MAX_SRC_LINE];
    int         token;

    /* unused parameters */ (void)vl;

    /*
     * get compiled file name, and make error file
     */
    if( vn[0] == '\0' ) {
        _splitpath2( fn, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
        _makepath( path, pg.drive, pg.dir, pg.fname, "_vi" );
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
            tmp[0] = '\0';
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
    gf->data.fp = GetFromEnvAndOpen( fn );
    if( gf->data.fp == NULL ) {
        return( false );
    }
    return( true );

} /* SpecialOpen */

/*
 * SpecialFclose - close either file or exe
 */
void SpecialFclose( GENERIC_FILE *gf )
{
    fclose( gf->data.fp );

} /* SpecialFclose */

/*
 * SpecialFgets - get from either file or exe
 */
char *SpecialFgets( char *buff, int max_len, GENERIC_FILE *gf )
{
    if( myfgets( buff, max_len, gf->data.fp ) == NULL ) {
        return( NULL );
    }
    gf->gf.a.currline++;
    return( buff );

} /* SpecialFgets */


static vi_rc Compile( const char *fn, const char *data )
{
    labels      *lab, lb;
    vars_list   vl;
    sfile       *sf;
    char        sname[FILENAME_MAX];
    vi_rc       rc;
    srcline     sline = 0;

    WorkLine = MemAlloc( sizeof( line ) + MaxLine + 2 );
    WorkLine->len = -1;
    LastRC = LastRetCode;
    vl.head = vl.tail = NULL;
    sname[0] = '\0';
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

    /* unused parameters */ (void)argc;

    rc = Compile( argv[1], argv[2] );
    return( (rc == ERR_NO_ERR) ? 0 : -1 );
}

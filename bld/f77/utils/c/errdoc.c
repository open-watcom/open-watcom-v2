/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Make Fortran file for errors messages utility
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "bool.h"

#include "clibext.h"


#define NULLCHAR        '\0'

#define BUFF_LEN        256

static void change_format( char *s )
/**********************************/
{
    char    *p;

    do {
        p = strstr( s, "%u" );
        if( p == NULL ) {
            p = strstr( s, "%i" );
        }
        if( p != NULL ) {
            p++;
            *p++ = 'd';
            s = p;
        }
    } while( p != NULL );
}

static char *rtrims( char *s )
/****************************/
{
    size_t  len;

    if( *s != NULLCHAR ) {
        len = strlen( s );
        while( len-- > 0 && s[len] == ' ' ) {
            s[len] = NULLCHAR;
        }
    }
    return( s );
}

static  int     ReadInFile( char *buff, int max_len, FILE *fi )
/*************************************************************/
{
    size_t      len;

    if( fgets( buff, max_len, fi ) == NULL ) {
        return( 1 );
    }
    for( len = strlen( buff ); len > 0 && ( buff[len - 1] == '\r' || buff[len - 1] == '\n' ); len-- )
        ;
    buff[len] = NULLCHAR;
    return( 0 );
}


int main( int argc, char **argv )
/*******************************/
{
    FILE        *fi;
    FILE        *fo;
    char        rec[BUFF_LEN+1];
    char        group_name[3];
    char        header[81];
    bool        noheader;
    bool        include;
    int         msg_num;
    char        *p;

    if( argc > 1 ) {
        fi = fopen( argv[1], "rt" );
        argc--;
    } else {
        fi = fopen( "error.msg", "rt" );
    }
    if( argc > 1 ) {
        fo = fopen( argv[2], "wt" );
        argc--;
    } else {
        fo = fopen( "ferror.gml", "wt" );
    }

    ReadInFile( rec, sizeof( rec ), fi );
    for( ;; ) {
        if( rec[0] == 'M' && rec[1] == 'S' )
            break;
        group_name[0] = rec[0];
        group_name[1] = rec[1];
        group_name[2] = NULLCHAR;
        strncpy( header, rec + 3, sizeof( header ) - 1 );
        header[sizeof( header ) - 1] = NULLCHAR;
        noheader = true;
        ReadInFile( rec, sizeof( rec ), fi );
        msg_num = 1;
        for( ;; ) {
            if( rec[0] != NULLCHAR && rec[1] != NULLCHAR && rec[2] == ' ' )
                // Group record
                break;
            p = strchr( rec, '[' );
            include = ( p != NULL && p[6] == ']' && p[2] != 'w' );
            if( include ) {
                if( noheader ) {
                    fprintf( fo, ".errhead %s\n", rtrims( header ) );
                    noheader = false;
                }
                change_format( p + 8 );
                fprintf( fo, ".errnote %s-%2.2d %s\n", group_name, msg_num, rtrims( p + 8 ) );
                fprintf( fo, ".pc\n" );
            }
            do {
                // skip all language records
                ReadInFile( rec, sizeof( rec ), fi );
            } while( rec[0] != ' ' );
            do {
                if( include ) {
                    fprintf( fo, "%s\n", rtrims( rec + 1 ) );
                }
                ReadInFile( rec, sizeof( rec ), fi );
            } while( rec[0] == ' ' );
            msg_num++;
        }
        if( !noheader ) {
            fprintf( fo, ".errtail\n" );
        }
    }
    fclose( fi );
    fclose( fo );
    return( 0 );
}

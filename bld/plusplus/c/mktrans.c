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


#define ENUM_PREFIX     "\tY_"
#define ENUM_LEN        1
#define DEFINE_PREFIX   "#define Y_"
#define DEFINE_LEN      8

#define ENUM_ON         "typedef enum yytokentype {"
#define ENUM_OFF        "} yytokentype;"

char ibuff[1024];
char mname[1024];
char mvalue[1024];

static void fail( char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    vfprintf( stderr, msg, args );
    va_end( args );
    exit( EXIT_FAILURE );
}

static int chk_enum( int enumflag )
{
    if( enumflag ) {
        if( memcmp( ibuff, ENUM_OFF, sizeof( ENUM_OFF ) - 1 ) == 0 ) {
            return( 1 );
        }
    } else {
        if( memcmp( ibuff, ENUM_ON, sizeof( ENUM_ON ) - 1 ) == 0 ) {
            return( 1 );
        }
    }
    return( 0 );
}

static int is_item( int enumflag )
{
    if( enumflag ) {
        if( memcmp( ibuff, ENUM_PREFIX, sizeof( ENUM_PREFIX ) - 1 ) == 0 ) {
            return( ENUM_LEN );
        }
    } else {
        if( memcmp( ibuff, DEFINE_PREFIX, sizeof( DEFINE_PREFIX ) - 1 ) == 0 ) {
            return( DEFINE_LEN );
        }
    }
    return( 0 );
}

static void get_mname_mvalue( int prefix_len )
{
    char *p;
    char *q;
    char *r;

    p = ibuff + prefix_len;
    for( ; *p; ++p ) {
        if( ! isspace( *p ) ) {
            break;
        }
    }
    q = mname;
    for( ; *p; ++p ) {
        if( isspace( *p ) )
            break;
        if( *p == '=' )
            break;
        *q++ = *p;
    }
    *q = '\0';
    for( ; *p; ++p ) {
        if( ! isspace( *p ) && *p != '=' ) {
            break;
        }
    }
    r = mvalue;
    for( ; *p; ++p ) {
        if( isspace( *p ) || *p == ',' )
            break;
        *r++ = *p;
    }
    *r = '\0';
}

static int mname_suffix( char *s )
{
    size_t mlen;
    size_t slen;
    size_t i;

    mlen = strlen( mname );
    slen = strlen( s );
    if( slen > mlen ) {
        return 0;
    }
    i = mlen - slen;
    return( strcmp( &mname[i], s ) == 0 );
}

int main( int argc, char **argv )
{
    FILE *ifp;
    FILE *ofp;
    char *check;
    int prefix_len;
    int enumflag;

    if( argc != 3 ) {
        fail( "mktrans <ytab-hfile> <xlat-hfile>" );
    }
    ifp = fopen( argv[1], "r" );
    if( ifp == NULL ) {
        fail( "cannot open input file '%s'", argv[1] );
    }
    ofp = fopen( argv[2], "w" );
    if( ofp == NULL ) {
        fail( "cannot open input file '%s'", argv[2] );
    }
    fprintf( ofp, "static unsigned char toYACC[256];\n" );
    fprintf( ofp, "static void createTable( void )\n{\n" );
    enumflag = 0;
    for(;;) {
        check = fgets( ibuff, sizeof( ibuff ), ifp );
        if( check == NULL )
            break;
        if( chk_enum( enumflag ) ) {
            if( enumflag )
                break;
            enumflag = 1;
            continue;
        }
        prefix_len = is_item( enumflag );
        if( prefix_len == 0 )
            continue;
        get_mname_mvalue( prefix_len );
        if( mname_suffix( "_NAME" ) || mname_suffix( "_SPECIAL" ) )
            continue;
        fprintf( ofp, "    toYACC[T_%s] = %s;\n", &mname[2], mvalue );
    }
    fprintf( ofp, "}\n" );
    fclose( ifp );
    fclose( ofp );
    return( EXIT_SUCCESS );
}

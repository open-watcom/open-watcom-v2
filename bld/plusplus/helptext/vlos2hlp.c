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

#include "awk.h"

unsigned line;
unsigned errors;
int xmp_active = 0;
char *last_code;
char *error_code;

void print_msg( char *msg, char *field )
{
    if( errors == 0 ) {
        fprintf( stderr, "OS/2 Help is invalid!\n" );
    }
    ++errors;
    if( strcmp( last_code, error_code ) != 0 ) {
        fprintf( stderr, "Processing error message: %s\n", error_code );
        free( last_code );
        last_code = strdup( error_code );
    }
    fprintf( stderr, "line %d -- %s: %s\n", line, msg, field );
}

int starts( char *t )
{
    size_t len = strlen( t );

    if( NF < 1 ) {
        return( 0 );
    }
    return memcmp( field[0], t, len ) == 0;
}

int is_err_code()
{
    if( NF < 3 ) {
        return( 0 );
    }
    if( memcmp( field[2], "ERR_", 4 ) == 0 ) {
        return( 1 );
    }
    if( memcmp( field[2], "WARN_", 5 ) == 0 ) {
        return( 1 );
    }
    if( memcmp( field[2], "INF_", 4 ) == 0 ) {
        return( 1 );
    }
    return( 0 );
}

int main( int argc, char **argv )
{
    FILE *ifp;
    char *check;
    char *s;
    unsigned pos;

    if( argc != 2 ) {
        fail( "vlos2hlp <hlp-file>" );
    }
    ifp = fopen( argv[1], "r" );
    if( ifp == NULL ) {
        fail( "cannot open input file '%s'", argv[1] );
    }
    last_code = strdup( "" );
    error_code = strdup( "???" );
    for(;;) {
        check = fgets( ibuff, sizeof(ibuff), ifp );
        if( check == NULL ) break;
        ++line;
        make_fields();
        if( starts( ".*" ) ) {
            if( is_err_code() ) {
                free( error_code );
                error_code = strdup( field[2] );
            }
        } else if( starts( "." ) ) {
            print_msg( "found SCRIPT macro", field[0] );
        } else if( starts( ":xmp" ) ) {
            if( xmp_active ) {
                print_msg( "no :exmp found", ":xmp" );
            }
            xmp_active = 1;
        } else if( starts( ":exmp" ) ) {
            xmp_active = 0;
        } else if( starts( ":h1" ) ) {
            if( xmp_active ) {
                print_msg( "no :exmp found", ":h1" );
                xmp_active = 0;
            }
        } else if( ! xmp_active ) {
            for( s = ibuff; ; ++s ) {
                s = strchr( s, '&' );
                if( s == NULL ) break;
                if( match( s, "&amp", "&colon", NULL ) == 0 ) {
                    print_msg( "found potential substitution", s );
                    break;
                }
            }
        }
    }
    fclose( ifp );
    return( errors != 0 );
}

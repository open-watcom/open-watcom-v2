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
* Description:  This is the C file for 'ctl' input file parser.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "bool.h"
#include "myio.h"

#include "clibext.h"


#define MAX_LINE_LEN    200

#define IS_WS( x )      ((x) == ' ' || (x) == '\t')

static char WS_delims[] = " \t";

static int Line = 1;

/* Local implementations of strupr()/strlwr() for portability */

char *my_strupr( char *string )
/*****************************/
{
    char    *s;

    if( string != NULL ) {
        for( s = string; *s != '\0'; ++s ) {
            *s = (char)toupper( *s );
        }
    }
    return( string );
}

char *my_strlwr( char *string )
/*****************************/
{
    char    *s;

    if( string != NULL ) {
        for( s = string; *s != '\0'; ++s ) {
            *s = (char)tolower( *s );
        }
    }
    return( string );
}

static char *get_line( char *buf, int max_len, FILE *file )
/*********************************************************/
{
    char    *ret;

    while( (ret = myfgets( buf, max_len, file )) != NULL ) {
        ++Line;
        ret += strspn( ret, WS_delims );
        if( ret[0] != '#' && ret[0] != '\0' ) {
            break;
        }
    }
    return( ret );
}

static bool empty_data( char *str )
/*********************************/
{
    char    c;

    if( str == NULL )
        return( true );
    c = *str++;
    if( c == '*' )
        c = *str++;
    while( c != '\0' ) {
        if( !IS_WS( c ) ) {
            return( false );
        }
        c = *str++;
    }
    return( true );
}

int main( int argc, char *argv[] )
/********************************/
{
    FILE                *in;
    FILE                *out;
    char                buf[MAX_LINE_LEN];
    int                 elt;
    char                *end;
    char                *line;
    char                type[50];

    if( argc != 4 ) {
        printf( "FORMAT: parsectl [input file] [output file] [Ctl data name]\n" );
        return( -1 );
    }

    in = fopen( argv[1], "r" );
    if( in == NULL ) {
        printf( "Could not open input file: %s\n", argv[1] );
        return( -1 );
    }

    out = fopen( argv[2], "w" );
    if( out == NULL ) {
        printf( "Could not open output file: %s\n", argv[2] );
        return( -1 );
    }

    fputs( "/**** DO NOT MODIFY THIS FILE BY HAND. CREATED BY PARSECTL ****/\n\n\n",
           out );

    /* Create Data struct definition */
    fputs( "struct {\n", out );
    fputs( "    int            num_ctls;\n", out );

    elt = 0;
    while( (line = get_line( buf, sizeof( buf ), in )) != NULL ) {
        end = strpbrk( line, WS_delims );
        if( end == NULL ) {
            printf( "No control on line %d\n", Line );
            goto error;
        }
        *end = '\0';
        strcpy( type, line );

        line = get_line( buf, sizeof( buf ), in );     // skip over data
        if( line == NULL ) {
            printf( "No data at line %d\n", Line );
            goto error;
        }

        fputs( "    struct {\n", out );
        fputs( "        ctl_type      type;\n", out );
        fputs( "        int           control;\n", out );
        fputs( "        bool          modified;\n", out );
        fputs( "        void (* get)( void *ptr, struct ctl_elt *elt, void *data );\n", out );
        fputs( "        void (* set)( void *ptr, struct ctl_elt *elt, void *data );\n", out );
        fputs( "        union {\n", out );
        line = get_line( buf, sizeof( buf ), in );     // skip over data
        if( line == NULL ) {
            printf( "No data at line %d\n", Line );
            goto error;
        }
        if( !empty_data( line ) ) {
            fprintf( out,  "            %s            d1;\n", my_strlwr( type ) );
        }
        fputs( "            ctl_info      d2;\n", out );
        fputs( "        } d3;\n", out );
        fprintf( out,  "    } d%-d;\n", elt++ );
    }

    fclose( in );

    fprintf( out, "} %s = {\n", argv[3] );
    fprintf( out, "%d,\n", elt );

    in = fopen( argv[1], "r" );
    while( (line = get_line( buf, sizeof( buf ), in )) != NULL ) {
        end = strpbrk( line, WS_delims );
        if( end != NULL ) {
            *end++ = '\0';
            fprintf( out, "{ %s, %s, false,", my_strupr( line ), end );
        }

        line = get_line( buf, sizeof( buf ), in );
        if( line == NULL )
            break;
        fprintf( out, " %s", line );

        line = get_line( buf, sizeof( buf ), in );
        if( empty_data( line ) ) {
            fputs( " },\n", out );
        } else {
            fprintf( out, ", %s },\n", line );
        }
    }
    fputs( "};\n\n", out );

    fclose( in );
    fclose( out );

    return( 0 );

error:
    fclose( in );
    fclose( out );

    return( 1 );
}

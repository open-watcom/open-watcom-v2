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
* Description:  This is the C file for 'dyn' input file parser.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "myio.h"


#define MAX_LINE_LEN    200

#define IS_WS( x )      ((x) == ' ' || (x) == '\t')

static char WS_delims[] = " \t";

static int Line = 1;

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

#if 0
static bool empty_data( char *ret )
/*********************************/
{
    char    *end;

    if( ret != NULL && *ret == '*' ) {
        for( end = ret + 1; *end != '\0'; ++end ) {
            if( !IS_WS( *end ) ) {
                return( false );
            }
        }
    }
    return( true );
}
#endif

int main( int argc, char *argv[] )
/********************************/
{
    FILE                *in;
    FILE                *out;
    char                buf[MAX_LINE_LEN];
    int                 elt;
    int                 items;
    char                *start;
    char                *end;
    char                *line;
    char                type[50];

    if( argc != 4 ) {
        printf( "FORMAT: parsedyn [input file] [output file] [dyn tpl name]\n" );
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

    fputs( "/**** DO NOT MODIFY THIS FILE BY HAND. CREATED BY PARSEDYN ****/\n\n\n",
           out );

    /* Create Data struct definition */
    fputs( "struct {\n", out );
    fputs( "    int                num_tpls;\n", out );

    elt = 1;
    while( (line = get_line( buf, sizeof( buf ), in )) != NULL ) {
        end = strpbrk( line, WS_delims );
        if( end == NULL ) {
            printf( "No template on line %d\n", Line );
            goto error;
        }
        *end = '\0';
        strcpy( type, line );

        if( strcmp( type, "DYN_TPL_DIM" ) == 0 )
            fprintf( out, "    dyn_dim_def        dyn_tpl%-d;\n", elt );
        else if( strcmp( type, "DYN_TPL_STATIC" ) == 0 )
            fprintf( out, "    dyn_static_def        dyn_tpl%-d;\n", elt );
        else {
            printf( "Invalid template type on line %d\n", Line );
            goto error;
        }

        line = get_line( buf, sizeof( buf ), in );     // get data line and count entries
        if( line == NULL ) {
            printf( "No data at line %d\n", Line );
            goto error;
        }

        items = 2;
        while( (line = strchr( line, ',' )) != NULL ) {
            items++;
            line++;
        }

        fprintf( out, "    int                tpl_%-d[%d];\n", elt, items );
        elt++;
    }

    fclose( in );

    fprintf( out, "} %s = {\n", argv[3] );
    fprintf( out, "%d,\n", elt - 1 );

    in = fopen( argv[1], "r" );

    while( (line = get_line( buf, sizeof( buf ), in )) != NULL ) {
        start = strpbrk( line, WS_delims );
        if( start != NULL )
            *start++ = '\0';
        strcpy( type, line );

        if( strcmp( type, "DYN_TPL_DIM" ) == 0 ) {
            fputs( "DYN_TPL_DIM, true, 0, ", out );
        } else {
            fputs( "DYN_TPL_STATIC, true, 0, ", out );
        }

        do {
            end = strpbrk( start, WS_delims );
            if( end != NULL ) {
                *end++ = '\0';
            }
            fprintf( out, "%s, ", start );
            start = end;
        } while( end );

        line = get_line( buf, sizeof( buf ), in );
        if( line == NULL ) {
            break;
        }
        fprintf( out, "\n    %s,\n    -1,\n", line );
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

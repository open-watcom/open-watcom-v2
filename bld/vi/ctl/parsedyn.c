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
* Description:  This is the C file for 'dyn' input file parser.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN    200

#define isWSorCtrlZ(x)  (isspace( x ) || (x == 0x1A))

static char White_space[] = " \t";

static int Line = 1;

static char *get_line( char *buf, FILE *file ) 
/********************************************/
{
    char    *ret;
    int     i;

    for( ; (ret = fgets( buf, MAX_LINE_LEN, file )) != NULL; ) {

        for( i = strlen( buf ); i && isWSorCtrlZ( buf[i - 1] ); --i ) {
            buf[i - 1] = '\0';
        }
        ++Line;

        ret += strspn( ret, White_space );

        if( ret[0] != '#' && ret[0] != '\0' ) {
            break;
        }
    }

    return( ret );
}

int empty_data( char *ret ) 
/*************************/
{
    char    *end;

    if( *ret == '*' ) {
        for( end = ret + 1;; ++end ) {
            if( *end == '\0' ) {
                return( 1 );
            } else if( *end != ' ' && *end != '\t' ) {
                break;
            }
        }
    }

    return( 0 );
}

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

    for( elt = 0;; ++elt ) {
        line = get_line( buf, in );
        if( line == NULL ) {
            break;
        }
        end = strpbrk( line, White_space );
        if( end == NULL ) {
            printf( "No template on line %d\n", Line );
            goto error;
        }
        *end = '\0';
        strcpy( type, line );

        if( !strcmp( type, "DYN_TPL_DIM" ) )
            fprintf( out, "    dyn_dim_def        dyn_tpl%-d;\n", elt + 1 );
        else if( !strcmp( type, "DYN_TPL_STATIC" ) )
            fprintf( out, "    dyn_static_def        dyn_tpl%-d;\n", elt + 1 );
        else {
            printf( "Invalid template type on line %d\n", Line );
            goto error;
        }

        line = get_line( buf, in );     // get data line and count entries

        if( line == NULL ) {
            printf( "No data at line %d\n", Line );
            goto error;
        }

        items = 2;
        while( (line = strchr( line, ',' )) != NULL ) {
            items++;
            line++;
        }

        fprintf( out, "    int                tpl_%-d[%d];\n", elt + 1, items );
    }

    fclose( in );

    fprintf( out, "} %s = {\n", argv[3] );
    fprintf( out, "%d,\n", elt );

    in = fopen( argv[1], "r" );

    for( ;; ) {
        line = get_line( buf, in );
        if( line == NULL ) {
            break;
        }

        start = strpbrk( line, White_space );
        *start++ = '\0';
        strcpy( type, line );

        if( !strcmp( type, "DYN_TPL_DIM" ) ) {
            fputs( "DYN_TPL_DIM, TRUE, 0, ", out );
        } else {
            fputs( "DYN_TPL_STATIC, TRUE, 0, ", out );
        }

        do {
            end = strpbrk( start, White_space );
            if( end ) {
                *end++ = '\0';
            }
            fprintf( out, "%s, ", start );
            start = end;
        } while( end );

        line = get_line( buf, in );
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

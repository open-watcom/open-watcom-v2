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
* Description:  This is the C file for 'ctl' input file parser.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "clibext.h"


#define MAX_LINE_LEN    200

#define isWSorCtrlZ(x)    (isspace( x ) || (x == 0x1A))

static char White_space[] = " \t";

static int Line = 1;

/* Local implementations of strupr()/strlwr() for portability */

char *my_strupr( char *string )
/*****************************/
{
    char    *s;

    if( string != NULL ) {
        for( s = string; *s; ++s ) {
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
        for( s = string; *s; ++s ) {
            *s = (char)tolower( *s );
        }
    }
    return( string );
}

static char *get_line( char *buf, FILE *file ) 
/********************************************/
{
    char    *ret;
    size_t  i;

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

static int empty_data( char *ret ) 
/********************************/
{
    char                *end;

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

    for( elt = 0;; ++elt ) {
        line = get_line( buf, in );
        if( line == NULL ) {
            break;
        }
        end = strpbrk( line, White_space );
        if( end == NULL ) {
            printf( "No control on line %d\n", Line );
            goto error;
        }
        *end = '\0';
        strcpy( type, line );

        line = get_line( buf, in );     // skip over data
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
        line = get_line( buf, in );     // skip over data
        if( line == NULL ) {
            printf( "No data at line %d\n", Line );
            goto error;
        }
        if( !empty_data( line ) ) {
            fprintf( out,  "            %s            d1;\n", my_strlwr( type ) );
        }
        fputs( "            ctl_info      d2;\n", out );
        fputs( "        } d3;\n", out );
        fprintf( out,  "    } d%-d;\n", elt );
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

        end = strpbrk( line, White_space );
        *end = '\0';
        ++end;

        fprintf( out, "{ %s, %s, false,", my_strupr( line ), end );

        line = get_line( buf, in );
        fprintf( out, " %s", line );

        line = get_line( buf, in );
        if( !empty_data( line ) ) {
            fprintf( out, ", %s },\n", line );
        } else {
            fputs( "},\n", out );
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

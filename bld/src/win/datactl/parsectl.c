#include <stdio.h>
#include <string.h>

static char White_space[]=" \t";

static int Line = 1;

static char *get_line( char *buf, FILE *file )
/********************************************/

{
    char                *ret;

    for( ;; ) {
        ret = fgets( buf, 200, file );
        if( ret == NULL ) {
            break;
        }
        *strrchr( ret, '\n' ) = '\0';
        ++Line;

        ret += strspn( ret, White_space );

        if( ret[0] != '#' && ret[0] != '\0' ) {
            break;
        }
    }

    return( ret );
}

int empty_data(
/*************/

    char                *ret
) {
    char                *end;

    if( *ret == '*' ) {
        for( end = ret+1;; ++end ) {
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
    char                buf[200];
    int                 elt;
    char                *end;
    char                *line;
    char                type[50];

    if( argc != 4 ) {
        printf(
            "FORMAT: parsectl [input file] [output file] [Ctl data name]\n" );
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

   fputs(
    "/**** DO NOT MODIFY THIS FILE BY HAND. CREATED BY PARSECTL ****/\n\n\n",
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

        line = get_line( buf, in );     // skip over data_offset
        if( line == NULL ) {
            printf( "No data offset at line %d\n", Line );
            goto error;
        }

        fputs( "    struct {\n", out );
        fputs( "        ctl_type      type;\n", out );
        fputs( "        int           control;\n", out );
        fputs( "        BOOL          modified;\n", out );
        fputs( "        unsigned int  data_offset;\n", out );
        fputs( "        union {\n", out );
        line = get_line( buf, in );     // skip over data
        if( line == NULL ) {
            printf( "No data at line %d\n", Line );
            goto error;
        }
        if( !empty_data( line ) ) {
            fprintf( out,  "            %s            d1;\n", strlwr( type ) );
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

        fprintf( out, "{ %s, %s, FALSE,", strupr( line ), end );

        line = get_line( buf, in );
        fprintf( out, "%s ", line );

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

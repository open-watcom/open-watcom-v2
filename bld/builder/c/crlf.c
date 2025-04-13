/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2024 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  EOL conversion tool
*
****************************************************************************/


#include <stdio.h>
#include "bool.h"


#define CR  13
#define LF  10

int main( int argc, char **argv )
{
    bool    to_crlf;
    FILE    *fi;
    FILE    *fo;
    int     c;
    int     prev;
    char    *out_file;
    int     rc;

    /* unused parameters */ (void)argc;

#ifdef __UNIX__
    to_crlf = false;
#else
    to_crlf = true;
#endif
    ++argv;
    while( *argv != NULL && **argv == '-' ) {
        if( argv[0][1] == 'd' || argv[0][1] == 'D' ) {
            to_crlf = true;
        } else if( argv[0][1] == 'u' || argv[0][1] == 'U' ) {
            to_crlf = false;
        } else {
            printf( "Option '%s' not recognized.\n", *argv );
            return( 1 );
        }
        ++argv;
    }
    if( *argv == NULL ) {
        printf( "Missing input file name.\n" );
        return( 2 );
    }
    fi = fopen( *argv, "rb" );
    if( fi == NULL ) {
        printf( "Cannot open input file '%s'.\n", *argv );
        return( 3 );
    }
    /*
     * if defined output file name then use it
     * otherwise temporary file is used for processing
     * and input file name is used also as output file name
     * after processing temporary file is copied to input file
     */
    rc = 0;
    out_file = NULL;
    if( *( argv + 1 ) == NULL ) {
        out_file = *argv;
        fo = tmpfile();
        if( fo == NULL ) {
            printf( "Cannot open output temporary file.\n" );
            rc = 4;
        }
    } else {
        argv++;
        fo = fopen( *argv, "wb" );
        if( fo == NULL ) {
            printf( "Cannot open output file '%s'.\n", *argv );
            rc = 5;
        }
    }
    if( fo != NULL ) {
        prev = '\0';
        while( (c = fgetc( fi )) != EOF ) {
            if( c == CR ) {
                if( prev != CR ) {
                    prev = c;
                    continue;
                }
            } else if( c == LF ) {
                if( to_crlf ) {
                    fputc( CR, fo );
                }
            }
            fputc( c, fo );
            prev = c;
        }
        if( prev == CR ) {
            fputc( CR, fo );
        }
        if( out_file != NULL ) {
            fclose( fi );
            /*
             * flush and rewind temporary file
             */
            fflush( fo );
            rewind( fo );
            fi = fo;
            fo = fopen( out_file, "wb" );
            if( fo == NULL ) {
                printf( "Cannot open output file '%s'.\n", *argv );
                rc = 5;
            } else {
                size_t  numread;
                char    buffer[0x8000];

                while( (numread = fread( buffer, 1, sizeof( buffer ), fi )) != 0 ) {
                    if( numread != sizeof( buffer ) && ferror( fi ) ) {
                        printf( "File read error.\n" );
                        rc = 6;
                        break;
                    }
                    if( fwrite( buffer, 1, numread, fo ) != numread ) {
                        printf( "File write error.\n" );
                        rc = 7;
                        break;
                    }
                }
                fclose( fo );
            }
        } else {
            fclose( fo );
        }
    }
    fclose( fi );
    return( rc );
}

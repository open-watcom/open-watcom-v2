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
#include <string.h>
#include <ctype.h>

char *prettyToken[1024] = {
    "<end-of-file>",
    "<impossible>",
    "<error>",
};
unsigned max = 2;

char literal[1024];
unsigned value;

char buff[1024];

char *skipWhite( char *p ) {
    while( *p && isspace( *p ) ) {
        ++p;
    }
    return( p );
}

char *skipText( char *p, char *text ) {
    unsigned len = strlen( text );
    if( memcmp( p, text, len ) == 0 ) {
        p += len;
    }
    return( p );
}

char *skipLiteral( char *p ) {
    char *q = literal;
    while( *p && ! isspace( *p ) ) {
        *q = *p;
        ++q;
        ++p;
    }
    *q = '\0';
    return( p );
}

char *skipValue( char *p ) {
    value = 0;
    for(;;) {
        int c = *p;
        switch( c ) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            value <<= 4;
            value += c - '0';
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            value <<= 4;
            value += ( c - 'a' ) + 10;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            value <<= 4;
            value += ( c - 'A' ) + 10;
            break;
        default:
            return( p );
        }
        ++p;
    }
    return( p );
}

int parseLine( void ) {
    char *p = buff;

    p = skipWhite( p );
    if( *p != '#' ) {
        return( 0 );
    }
    ++p;
    p = skipText( p, "define" );
    p = skipWhite( p );
    p = skipLiteral( p );
    p = skipWhite( p );
    if( p[0] != '0' || p[1] != 'x' ) {
        return( 0 );
    }
    p += 2;
    p = skipValue( p );
    p = skipWhite( p );
    if( *p != '\0' ) {
        return( 0 );
    }
    return( 1 );
}

enum {
    C_UPPER     = 0x01,
    C_LOWER     = 0x02,
    C_UNDERSCORE= 0x04,
    C_UNKNOWN   = 0x08,
    C_NULL      = 0x00
};

int category( void ) {
    char *p;
    int c;

    c = C_NULL;
    for( p = literal; *p; ++p ) {
        if( isupper( *p ) ) {
            c |= C_UPPER;
        } else if( islower( *p ) ) {
            c |= C_LOWER;
        } else if( *p == '_' ) {
            c |= C_UNDERSCORE;
        } else {
            c |= C_UNKNOWN;
        }
    }
    return( c );
}

char *whatChar( char *p, char *w ) {
    char *q;

    q = skipText( p, "AND" );
    if( q != p ) {
        *w = '&';
        return( q );
    }
    q = skipText( p, "EQUAL" );
    if( q != p ) {
        *w = '=';
        return( q );
    }
    q = skipText( p, "COLON" );
    if( q != p ) {
        *w = ':';
        return( q );
    }
    q = skipText( p, "COMMA" );
    if( q != p ) {
        *w = ',';
        return( q );
    }
    q = skipText( p, "DIVIDE" );
    if( q != p ) {
        *w = '/';
        return( q );
    }
    q = skipText( p, "DOT" );
    if( q != p ) {
        *w = '.';
        return( q );
    }
    q = skipText( p, "EXCLAMATION" );
    if( q != p ) {
        *w = '!';
        return( q );
    }
    q = skipText( p, "GT" );
    if( q != p ) {
        *w = '>';
        return( q );
    }
    q = skipText( p, "LT" );
    if( q != p ) {
        *w = '<';
        return( q );
    }
    q = skipText( p, "LBRACE" );
    if( q != p ) {
        *w = '{';
            return( q );
    }
    q = skipText( p, "LPAREN" );
    if( q != p ) {
        *w = '(';
        return( q );
    }
    q = skipText( p, "LSQUARE" );
    if( q != p ) {
        *w = '[';
        return( q );
    }
    q = skipText( p, "RBRACE" );
    if( q != p ) {
        *w = '}';
        return( q );
    }
    q = skipText( p, "RPAREN" );
    if( q != p ) {
        *w = ')';
        return( q );
    }
    q = skipText( p, "RSQUARE" );
    if( q != p ) {
        *w = ']';
        return( q );
    }
    q = skipText( p, "MINUS" );
    if( q != p ) {
        *w = '-';
        return( q );
    }
    q = skipText( p, "OR" );
    if( q != p ) {
        *w = '|';
        return( q );
    }
    q = skipText( p, "PERCENT" );
    if( q != p ) {
        *w = '%';
        return( q );
    }
    q = skipText( p, "PLUS" );
    if( q != p ) {
        *w = '+';
        return( q );
    }
    q = skipText( p, "QUESTION" );
    if( q != p ) {
        *w = '?';
        return( q );
    }
    q = skipText( p, "SEMICOLON" );
    if( q != p ) {
        *w = ';';
        return( q );
    }
    q = skipText( p, "TILDE" );
    if( q != p ) {
        *w = '~';
        return( q );
    }
    q = skipText( p, "TIMES" );
    if( q != p ) {
        *w = '*';
        return( q );
    }
    q = skipText( p, "XOR" );
    if( q != p ) {
        *w = '^';
        return( q );
    }
    *w = '\0';
    return( p );
}

void makeDelimiter() {
    char *p = literal;
    char *q;
    char *r = buff;
    char w;

    for(;;) {
        if( *p == '\0' ) break;
        q = whatChar( p, &w );
        if( w == '\0' ) {
            printf( "could not translate '%s' into a character\n", p );
            break;
        }
        *r++ = w;
        p = q;
        if( *p == '_' ) {
            ++p;
        }
    }
    *r = '\0';
}

void makeLiteral() {
    char *p = literal;
    char *q = buff;
    char prev = *p;

    *q++ = '<';
    while( *p ) {
        if( isupper( *p ) ) {
            if( islower( prev ) ) {
                *q++ = '-';
            }
            *q++ = tolower( *p );
        } else {
            *q++ = *p;
        }
        prev = *p;
        ++p;
    }
    *q++ = '>';
    *q = '\0';
}

char *makePretty( void ) {
    int c;

    if( literal[0] == 'Y' && literal[1] == '_' ) {
        strcpy( &literal[0], &literal[2] );
    }
    c = category();
    if( c & C_UNKNOWN ) {
        printf( "literal '%s' contains an unknown character\n", literal );
        return( NULL );
    }
    switch( c ) {
    case C_UPPER | C_UNDERSCORE:        // delimiter
    case C_UPPER:
        makeDelimiter();
        break;
    case C_LOWER:                       // keyword
        strcpy( buff, literal );
        break;
    case C_UPPER | C_LOWER:             // literal
        makeLiteral();
        break;
    default:
        printf( "literal '%s' contains unknown combination of chars\n", literal );
        return( NULL );
    }
    return( buff );
}

int main( int argc, char **argv )
{
    int errors;
    FILE *fi, *fo;
    char *check;
    int i;

    if( argc != 3 ) {
        puts( "usage: ytabprty <ytab-gh> <ytab-pick-output-gh>" );
        return( EXIT_FAILURE );
    }
    fi = fopen( argv[1], "r" );
    if( !fi ) {
        printf( "cannot open '%s' for input\n", argv[1] );
        return( EXIT_FAILURE );
    }
    fo = fopen( argv[2], "w" );
    if( !fo ) {
        printf( "cannot open '%s' for output\n", argv[2] );
        return( EXIT_FAILURE );
    }
    errors = 0;
    for(;;) {
        check = fgets( buff, sizeof( buff ), fi );
        if( check == NULL ) break;
        if( parseLine() ) {
            if( value > max ) {
                max = value;
            }
            prettyToken[ value ] = strdup( makePretty() );
            if( prettyToken[ value ] == NULL ) {
                printf( "couldn't produce pretty representation of '%s'\n", literal );
                ++errors;
            }
        }
    }
    for( i = 0; i <= max; ++i ) {
        if( prettyToken[i] == NULL ) {
            printf( "token value %u has no string\n", i );
            ++errors;
        }
    }
    if( errors ) {
        return( EXIT_FAILURE );
    }
    fprintf( fo, "char const * const yytokenname[] = {\n" );
    for( i = 0; i <= max; ++i ) {
        fprintf( fo, "\"%s\",\n", prettyToken[ i ] );
    }
    fprintf( fo, "};\n" );
    fclose( fi );
    fclose( fo );
    return( EXIT_SUCCESS );
}

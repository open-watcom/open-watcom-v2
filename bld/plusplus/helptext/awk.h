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


#define BUFF_SIZE       (2048)

char ibuff[BUFF_SIZE];
char fbuff[BUFF_SIZE];
char tbuff[BUFF_SIZE];
char *field[32];
unsigned NF;

void fail( char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    vfprintf( stderr, msg, args );
    va_end( args );
    exit( EXIT_FAILURE );
}

int match( char *text, ... )
{
    char *test;
    va_list args;

    if( text == NULL ) {
        return( 0 );
    }
    va_start( args, text );
    for(;;) {
        test = va_arg( args, char * );
        if( test == NULL ) break;
        if( strstr( text, test ) != NULL ) {
            va_end( args );
            return( 1 );
        }
    }
    va_end( args );
    return( 0 );
}

unsigned make_fields( void )
{
    char *f;
    char *p;
    char *s;
    unsigned nf;

    nf = 0;
    f = fbuff;
    p = ibuff;
    for(;;) {
        while( *p && isspace( *p ) ) {
            ++p;
        }
        if( *p == '\0' ) break;
        field[nf++] = f;
        while( *p && ! isspace( *p ) ) {
            *f++ = *p++;
        }
        *f++ = '\0';
    }
    field[nf] = NULL;
    NF = nf;
    return( nf );
}

void unmake_fields()
{
    char *p;
    unsigned i;

    p = ibuff;
    for( i = 0; field[i] != NULL; ++i ) {
        strcpy( p, field[i] );
        p += strlen( p );
        if( field[i+1] != NULL ) {
            *p++ = ' ';
        }
    }
    *p = '\0';
}

void print_fields( FILE *ofp )
{
    unsigned i;

    for( i = 0; field[i] != NULL; ++i ) {
        fputs( field[i], ofp );
        if( field[i+1] != NULL ) {
            fputc( ' ', ofp );
        }
    }
}

typedef struct gsub gsub;
struct gsub {
    char *find;
    char *replace;
};

void substitutions( gsub *subs )
{
    char *n;
    char *p;
    gsub *c;
    size_t len;
    size_t rlen;

    n = tbuff;
    for( p = ibuff; *p; ++p ) {
        *n = *p;
        for( c = subs; c->find != NULL; ++c ) {
            len = strlen( c->find );
            if( memcmp( p, c->find, len ) == 0 ) {
                rlen = strlen( c->replace );
                memcpy( n, c->replace, rlen );
                n += rlen - 1;
                p += len - 1;
                break;
            }
        }
        ++n;
    }
    *n = '\0';
    strcpy( ibuff, tbuff );
    make_fields();
}

int a_tag( char *f )
{
    char *s;

    if( *f++ != ':' ) {
        return( 0 );
    }
    s = f;
    while( *f ) {
        if( isupper( *f ) ) {
            ++f;
            continue;
        }
        if( *f == '_' ) {
            ++f;
            continue;
        }
        break;
    }
    if( *f != '.' || f == s ) {
        return( 0 );
    }
    return( 1 );
}

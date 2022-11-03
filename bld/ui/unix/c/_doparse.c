/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Parse the .tix terminal description file.
*
****************************************************************************/


#define TC_ERROR    ((unsigned)-1)

#define CODE2BUFF( __b, __c )   __b[0] = __c & 0xff; __b[1] = (__c >> 8) & 0xff;
#define BUFF2CODE( __b )        (*(unsigned char *)__b + (*(unsigned char *)(__b + 1) << 8 ))

static tix_token get_tix_token( char *buff, FILE *in_file )
{
    int         c;
    char        *p;
    char        *end;
    unsigned    num;
    int         endc;

    for( ;; ) {
        c = getc( in_file );
        if( c == EOF )
            return( TT_EOF );
        if( c == '#' ) {
            /* eat a comment */
            for( ;; ) {
                c = getc( in_file );
                if( c == EOF )
                    return( TT_EOF );
                if( c == '\n' ) {
                    break;
                }
            }
        }
        if( !isspace( c ) ) {
            break;
        }
    }
    p = buff;
    if( c == '\'' || c == '\"' ) {
        /* collect a string */
        endc = c;
        for( ;; ) {
            c = getc( in_file );
            if( c == EOF )
                break;
            if( c == '\r' )
                break;
            if( c == '\n' )
                break;
            if( c == endc )
                break;
            if( c == '\\' ) {
                c = getc( in_file );
                if( c == EOF )
                    break;
                switch( c ) {
                case 'a':
                    c = '\a';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'e':
                    c = '\x1B';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'v':
                    c = '\b';
                    break;
                case 'x':
                    num = 0;
                    for( ;; ) {
                        c = getc( in_file );
                        if( c == EOF )
                            break;
                        if( isdigit( c ) ) {
                            c = c - '0';
                        } else if( c >= 'A' && c <= 'F' ) {
                            c = c - 'A' + 10;
                        } else if( c >= 'a' && c <= 'f' ) {
                            c = c - 'a' + 10;
                        } else {
                            ungetc( c, in_file );
                        }
                        num = (num << 8) + c;
                    }
                    c = num;
                    break;
                }
            }
            *p++ = (char)c;
        }
        *p = '\0';
    } else {
        /* collect a string or number */
        for( ;; ) {
            *p++ = (char)c;
            c = getc( in_file );
            if( c == EOF )
                break;
            if( isspace( c ) )
                break;
            if( c == '#' ) {
                ungetc( c, in_file );
                break;
            }
        }
        *p = '\0';
        num = strtoul( buff, &end, 0 );
        if( end != p )
            return( TT_STRING );
        CODE2BUFF( buff, num );
    }
    return( TT_CODE );
}

static unsigned get_tix_code( char *buff, FILE *in_file )
{
    if( get_tix_token( buff, in_file ) != TT_CODE ) {
        tix_error( "expecting code" );
        return( TC_ERROR );
    }
    return( BUFF2CODE( buff ) );
}

bool do_parse( FILE *in_file )
{
    char        buff[80];
    char        input[80];
    tix_token   tok;
    unsigned    code;
    bool        alt_map;

    tok = get_tix_token( buff, in_file );
    while( tok == TT_STRING ) {
        if( stricmp( buff, "display" ) == 0 ) {
            code = get_tix_code( buff, in_file );
            if( code == TC_ERROR )
                return( false );
            alt_map = false;
            tok = get_tix_token( buff, in_file );
            if( tok == TT_STRING ) {
                if( stricmp( buff, "alt" ) != 0 ) {
                    tix_error( "expecting alt" );
                    return( false );
                }
                tok = get_tix_token( buff, in_file );
                if( tok == TT_STRING ) {
                    tix_error( "expecting display code" );
                    return( false );
                }
                alt_map = true;
            }
            if( tok == TT_EOF )
                break;
            if( tok == TT_CODE ) {
                ti_map_display_code( buff[0], alt_map );
            }
            tok = get_tix_token( buff, in_file );
        } else if( stricmp( buff, "key" ) == 0 ) {
            code = get_tix_code( buff, in_file );
            if( code == TC_ERROR )
                return( false );
            input[0] = '\0';
            while( (tok = get_tix_token( buff, in_file )) == TT_CODE ) {
                strcat( input, buff );
            }
            TrieAdd( code, input );
        } else {
            tix_error( "unknown directive" );
            return( false );
        }
    }
    if( tok == TT_CODE ) {
        tix_error( "expecting directive" );
        return( false );
    }
    return( true );
}

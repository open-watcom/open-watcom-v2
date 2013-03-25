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


#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

typedef enum { FALSE = 0, TRUE = 1 } boolean;
typedef enum {
    LETTER_0 = 1,
    LETTER_1,
    LETTER_2,
    LETTER_3,
    LETTER_4,
    LETTER_5,
    LETTER_6,
    LETTER_7,
    LETTER_8,
    LETTER_9,
    LETTER_a,
    LETTER_b,
    LETTER_c,
    LETTER_d,
    LETTER_e,
    LETTER_f,
    LETTER_g,
    LETTER_h,
    LETTER_i,
    LETTER_j,
    LETTER_k,
    LETTER_l,
    LETTER_m,
    LETTER_n,
    LETTER_o,
    LETTER_p,
    LETTER_q,
    LETTER_r,
    LETTER_s,
    LETTER_t,
    LETTER_u,
    LETTER_v,
    LETTER_w,
    LETTER_x,
    LETTER_y,
    LETTER_z,
    LETTER_A,
    LETTER_B,
    LETTER_C,
    LETTER_D,
    LETTER_E,
    LETTER_F,
    LETTER_G,
    LETTER_H,
    LETTER_I,
    LETTER_J,
    LETTER_K,
    LETTER_L,
    LETTER_M,
    LETTER_N,
    LETTER_O,
    LETTER_P,
    LETTER_Q,
    LETTER_R,
    LETTER_S,
    LETTER_T,
    LETTER_U,
    LETTER_V,
    LETTER_W,
    LETTER_X,
    LETTER_Y,
    LETTER_Z,
    LETTER_NULLCHAR,
    LETTER_DOLLAR,
    LETTER_BACK_QUOTE,
    LETTER__,
    LETTER_UNKNOWN,

    LETTER_MIN = LETTER_0,
    LETTER_MAX = LETTER_UNKNOWN,
} letter_t;
typedef unsigned keyword_t;

#define CHAR_UNKNOWN    '#'
#define NULL_KEYWORD    0

#define MAX_KEYWORDS    200
#define MAX_HASHSIZE    (2*MAX_KEYWORDS)
#define MAX_WEIGHTS     256
#define BAD_THRESHOLD   5

char *tokens[ MAX_KEYWORDS+1 ];
char *token_class[ MAX_KEYWORDS+1 ];
unsigned position[ MAX_KEYWORDS+1 ];
letter_t first[ MAX_KEYWORDS+1 ];
letter_t last[ MAX_KEYWORDS+1 ];
unsigned len[ MAX_KEYWORDS+1 ];
unsigned init_hash[ MAX_KEYWORDS+1 ];
unsigned done[ MAX_KEYWORDS+1 ];
unsigned hash[ MAX_KEYWORDS+1 ];
unsigned ordered[ MAX_KEYWORDS+1 ];
unsigned long collisions[ MAX_KEYWORDS+1 ];

keyword_t used[ MAX_HASHSIZE ];
keyword_t partial[ MAX_HASHSIZE ];

unsigned freq[ LETTER_MAX+1 ];
unsigned weights[ LETTER_MAX+1 ];
letter_t next[ LETTER_MAX+1 ];
letter_t prev[ LETTER_MAX+1 ];

boolean available[ MAX_WEIGHTS+1 ];

letter_t most_used_character;
unsigned num_keywords;
unsigned hashsize;
unsigned hashmask;
unsigned first_scale;
unsigned last_scale;
unsigned min_len;
unsigned max_len;
unsigned extra;
unsigned long len_mask;

struct {
    unsigned    quiet : 1;      /* no console output req'd */
    unsigned    imperfect : 1;  /* non-minimal perfect hash function allowed */
    unsigned    mask_hash : 1;  /* use mod 2^n */
    unsigned    tiny_output : 1;/* output .gh file for small lists */
    unsigned    align : 1;      /* output strings length mod 4 */
} flags;

FILE *outfile;

char *pick_extension = NULL;

void error( char *msg )
/*********************/
{
    fputs( msg, stderr );
    fputc( '\n', stderr );
}

void fatal( char *msg )
/*********************/
{
    error( msg );
    exit( EXIT_FAILURE );
}

void output( char *msg, ... )
/***************************/
{
    va_list args;

    if( flags.quiet ) {
        return;
    }
    va_start( args, msg );
    vprintf( msg, args );
    va_end( args );
}

letter_t make_letter( char c )
/****************************/
{
    if( c == '\0' ) {
        return( LETTER_NULLCHAR );
    }
    if( c == '$' ) {
        return( LETTER_DOLLAR );
    }
    if( c == '`' ) {
        return( LETTER_BACK_QUOTE );
    }
    if( c == '_' ) {
        return( LETTER__ );
    }
    if( isdigit( c ) ) {
        return( ( c - '0' ) + LETTER_0 );
    }
    if( c >= 'a' && c <= 'i' ) {
        return( ( c - 'a' ) + LETTER_a );
    }
    if( c >= 'j' && c <= 'r' ) {
        return( ( c - 'j' ) + LETTER_j );
    }
    if( c >= 's' && c <= 'z' ) {
        return( ( c - 's' ) + LETTER_s );
    }
    if( c >= 'A' && c <= 'I' ) {
        return( ( c - 'A' ) + LETTER_A );
    }
    if( c >= 'J' && c <= 'R' ) {
        return( ( c - 'J' ) + LETTER_J );
    }
    if( c >= 'S' && c <= 'Z' ) {
        return( ( c - 'S' ) + LETTER_S );
    }
    return( LETTER_UNKNOWN );
}

char make_char( letter_t i )
/**************************/
{
    if( i == LETTER_NULLCHAR ) {
        return( '\0' );
    }
    if( i == LETTER__ ) {
        return( '_' );
    }
    if( i == LETTER_DOLLAR ) {
        return( '$' );
    }
    if( i == LETTER_BACK_QUOTE ) {
        return( '`' );
    }
    if( i >= LETTER_0 && i <= LETTER_9 ) {
        return( ( i - LETTER_0 ) + '0' );
    }
    if( i >= LETTER_a && i <= LETTER_i ) {
        return( ( i - LETTER_a ) + 'a' );
    }
    if( i >= LETTER_j && i <= LETTER_r ) {
        return( ( i - LETTER_j ) + 'j' );
    }
    if( i >= LETTER_s && i <= LETTER_z ) {
        return( ( i - LETTER_s ) + 's' );
    }
    if( i >= LETTER_A && i <= LETTER_I ) {
        return( ( i - LETTER_A ) + 'A' );
    }
    if( i >= LETTER_J && i <= LETTER_R ) {
        return( ( i - LETTER_J ) + 'J' );
    }
    if( i >= LETTER_S && i <= LETTER_Z ) {
        return( ( i - LETTER_S ) + 'S' );
    }
    return( CHAR_UNKNOWN );
}

char *make_define( letter_t i )
/**************************/
{
    static char define_string[2];

    if( i == LETTER_NULLCHAR ) {
        return( "00h" );
    }
    if( i == LETTER_DOLLAR ) {
        return( "DOLLAR" );
    }
    if( i == LETTER_BACK_QUOTE ) {
        return( "BACK_QUOTE" );
    }
    if( i == LETTER__ ) {
        define_string[0] = '_';
    } else if( i >= LETTER_0 && i <= LETTER_9 ) {
        define_string[0] = ( ( i - LETTER_0 ) + '0' );
    } else if( i >= LETTER_a && i <= LETTER_i ) {
        define_string[0] = ( ( i - LETTER_a ) + 'a' );
    } else if( i >= LETTER_j && i <= LETTER_r ) {
        define_string[0] = ( ( i - LETTER_j ) + 'j' );
    } else if( i >= LETTER_s && i <= LETTER_z ) {
        define_string[0] = ( ( i - LETTER_s ) + 's' );
    } else if( i >= LETTER_A && i <= LETTER_I ) {
        define_string[0] = ( ( i - LETTER_A ) + 'A' );
    } else if( i >= LETTER_J && i <= LETTER_R ) {
        define_string[0] = ( ( i - LETTER_J ) + 'J' );
    } else if( i >= LETTER_S && i <= LETTER_Z ) {
        define_string[0] = ( ( i - LETTER_S ) + 'S' );
    } else {
        return( "UNKNOWN" );
    }
    define_string[1] = '\0';
    return define_string;
}

int cmptok( const void *_v1, const void *_v2 )
/****************************************************/
{
    const keyword_t *v1 = _v1;
    const keyword_t *v2 = _v2;

    return( strcmp( tokens[*v1], tokens[*v2] ) );
}

void swap( char **v1, char **v2 )
/*******************************/
{
    char *tmp;

    tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
}

void init_tokens( char **input_file )
/***********************************/
{
    int c;
    char *check;
    keyword_t i,j,k;
    keyword_t size;
    unsigned col;
    unsigned key_len;
    unsigned tok_len;
    FILE *fp;
    auto char keyword[80];
    auto char class[80];

    tokens[0] = "";
    min_len = 80;
    max_len = 0;
    size = 1;
    for( ; *input_file != NULL; ++input_file ) {
        check = *input_file;
        if( check[0] == '/' || check[0] == '-' ) {
            if( tolower( check[1] ) == 'q' && check[2] == '\0' ) {
                flags.quiet = TRUE;
                continue;
            }
            if( tolower( check[1] ) == 'e' && check[2] == '\0' ) {
                ++input_file;
                pick_extension = strdup( *input_file );
                continue;
            }
            if( tolower( check[1] ) == 'i' && check[2] == '\0' ) {
                flags.imperfect = TRUE;
                continue;
            }
            if( tolower( check[1] ) == 'm' && check[2] == '\0' ) {
                flags.mask_hash = TRUE;
                continue;
            }
            if( tolower( check[1] ) == 't' && check[2] == '\0' ) {
                flags.tiny_output = TRUE;
                continue;
            }
            if( tolower( check[1] ) == 'a' && check[2] == '\0' ) {
                flags.align = TRUE;
                continue;
            }
        }
        fp = fopen( *input_file, "r" );
        if( fp == NULL ) {
            fatal( "cannot open keyword file" );
        }
        for(;;) {
            c = fgetc( fp );
            if( c == '#' ) {
                for(;;) {
                    c = fgetc( fp );
                    if( ( c == EOF ) || ( c == '\n' ) ) {
                        break;
                    }
                }
                continue;
            }
            ungetc( c, fp );
            if( fscanf( fp, "%s %s\n", keyword, class ) != 2 )
                break;
            if( size > MAX_KEYWORDS ) {
                fatal( "too many keywords" );
            }
            key_len = strlen( keyword );
            if( key_len < min_len ) {
                min_len = key_len;
            }
            if( key_len > max_len ) {
                max_len = key_len;
            }
            if( key_len >= ( CHAR_BIT * sizeof( len_mask ) ) ) {
                fatal( "keyword is too long" );
            }
            len_mask |= ( 1L << key_len );
            position[size] = size;
            tokens[size] = strdup( keyword );
            if( strcmp( class, "TC_A?" ) == 0 ) {
                sprintf( class, "TC_A%u", 3 - ( key_len & 0x03 ) );
            }
            token_class[size] = strdup( class );
            ++size;
        }
        if( !feof( fp ) ) {
            fatal( "invalid token file" );
        }
        fclose( fp );
    }
    num_keywords = size - 1;
    if( num_keywords == 0 ) {
        fatal( "no keywords" );
    }
    qsort( &position[1], num_keywords, sizeof( int ), cmptok );
    for( i = 1; i < num_keywords; ++i ) {
        for( j = i; j < num_keywords; ++j ) {
            if( position[j] == i ) {
                break;
            }
        }
        k = position[i];
        swap( &tokens[k], &tokens[i] );
        swap( &token_class[k], &token_class[i] );
        position[j] = k;
    }
    col = 1;
    for( i = 1; i <= num_keywords; ++i ) {
        tok_len = strlen( tokens[i] );
        col += tok_len + 1;
        if( col > 79 ) {
            output( "\n" );
            col = tok_len + 2;
        }
        output( "%s ", tokens[i] );
    }
    output( "\n%u keywords min_len=%u max_len=%u\n", num_keywords, min_len, max_len );
}

void init_arrays( unsigned first_index, unsigned last_index )
/***********************************************************/
{
    keyword_t i;
    letter_t c;

    for( c = LETTER_MIN; c <= LETTER_MAX; ++c ) {
        freq[ c ] = 0;
    }
    for( i = 1; i <= num_keywords; ++i ) {
        len[ i ] = strlen( tokens[ i ] );
        if( len[i] > first_index ) {
            first[i] = make_letter( tokens[i][first_index] );
        } else {
            first[i] = make_letter( tokens[i][ len[i] - 1 ] );
        }
        if( len[i] > last_index ) {
            last[i] = make_letter( tokens[i][ len[i] - ( last_index + 1 ) ] );
        } else {
            last[i] = make_letter( tokens[i][0] );
        }
        done[ i ] = 0;
        // these improve the hash function
        init_hash[ i ] = len[ i ] + tokens[i][ min_len ];
        //init_hash[ i ] = len[ i ] + tokens[i][ len[i] >> 1 ];
        //init_hash[ i ] = len[ i ];
        hash[ i ] = init_hash[ i ];
        ++freq[ first[ i ] ];
        ++freq[ last[ i ] ];
    }
    for( i = 1; i <= num_keywords; ++i ) {
        ordered[ i ] = 0;
    }
    for( i = 0; i < hashsize; ++i ) {
        used[ i ] = NULL_KEYWORD;
    }
    // weight 0 is never used so it can be used in weights[] to see
    // what letters have weights
    for( i = 1; i <= MAX_WEIGHTS; ++i ) {
        available[ i ] = TRUE;
    }
}

void sort_frequency( void )
/*************************/
{
    letter_t previous;
    letter_t after_c;
    letter_t c;
    char prt_c;
    boolean change;

    /* find the alphabetic character that occurs the most in the keywords */
    most_used_character = LETTER_MIN;
    for( c = LETTER_MIN; c <= LETTER_MAX; ++c ) {
        if( c == LETTER__ )
            continue;
        if( freq[ c ] > freq[ most_used_character ] ) {
            most_used_character = c;
        }
    }
    /* make the next[] links into a ring */
    for( c = LETTER_MIN; c < LETTER_MAX; ++c ) {
        next[ c ] = c + 1;
    }
    next[ LETTER_MAX ] = LETTER_MIN;
    /* sort the list of characters in descending order of frequency */
    do {
        change = FALSE;
        c = most_used_character;
        for(;;) {
            previous = c;
            c = next[ previous ];
            after_c = next[ c ];
            if( after_c == most_used_character )
                break;
            if( freq[ c ] < freq[ after_c ] ) {
                /* exchange 'c' and 'after_c' */
                next[ c ] = next[ after_c ];
                next[ after_c ] = c;
                next[ previous ] = after_c;
                change = TRUE;
            }
        }
    } while( change );
    /* sort lists of equal frequency characters in ascending order */
    do {
        change = FALSE;
        c = most_used_character;
        for(;;) {
            previous = c;
            c = next[ previous ];
            after_c = next[ c ];
            if( after_c == most_used_character )
                break;
            if( freq[ c ] == freq[ after_c ] ) {
                if( c > after_c ) {
                    /* exchange 'c' and 'after_c' */
                    next[ c ] = next[ after_c ];
                    next[ after_c ] = c;
                    next[ previous ] = after_c;
                    change = TRUE;
                }
            }
        }
    } while( change );
    output( "frequency ordering of characters: " );
    /* update the prev pointers to reflect the new ordering */
    c = most_used_character;
    do {
        if( freq[ c ] != 0 ) {
            prt_c = make_char( c );
            if( isprint( prt_c ) ) {
                output( "%c", prt_c );
            } else {
                output( "\\x%02x", prt_c );
            }
        }
        after_c = next[ c ];
        prev[ after_c ] = c;
        c = after_c;
    } while( c != most_used_character );
    output( "\n" );
}

unsigned do_hash( unsigned x )
{
    x &= hashmask-1;
    if( x >= hashsize )
        x -= hashsize;
    return x;
}

void undo( letter_t c, keyword_t i )
/**********************************/
{
    keyword_t index;
    unsigned first_weight;
    unsigned last_weight;

    /*
        every keyword that had a full hash value calculated
        because of the weight of the character specified
        must deregister its hash value position
    */
    first_weight = first_scale * weights[c];
    last_weight = last_scale * weights[c];
    for( ; i > 0; --i ) {
        if( first[ i ] == c ) {
            --done[ i ];
            if( done[ i ] == 1 ) {      // 2 -> 1 transition
                index = do_hash( hash[ i ] );
                used[ index ] = NULL_KEYWORD;
            }
            hash[ i ] -= first_weight;
        }
        if( last[ i ] == c ) {
            --done[ i ];
            if( done[ i ] == 1 ) {      // 2 -> 1 transition
                index = do_hash( hash[ i ] );
                used[ index ] = NULL_KEYWORD;
            }
            hash[ i ] -= last_weight;
        }
    }
}

boolean share_letter( letter_t c, letter_t *p1, letter_t *p2 )
/************************************************************/
{
    keyword_t i;
    unsigned h;

    for( i = 0; i < hashsize; ++i ) {
        partial[i] = NULL_KEYWORD;
    }
    for( i=1; i <= num_keywords; ++i ) {
        if( done[i] == 1 ) {
            if( p1[i] == c || p2[i] == c ) {
                h = do_hash( hash[i] );
                if( partial[h] != NULL_KEYWORD ) {
                    ++collisions[i];
                    ++collisions[ partial[h] ];
                    return( TRUE );
                }
                partial[h] = i;
            }
        }
    }
    return( FALSE );
}

boolean check( letter_t c )
/*************************/
{
    /*
        - select all keywords with one more weight to add
        - if any keyword in this set shares a letter with
          another keyword in the set and the hash values
          are identical, the current set of weights cannot work
          since adding the same weight to both hashes will
          not make them different
    */

    if( first_scale == 1 && last_scale == 1 ) {
        while( freq[c] != 0 && c != most_used_character ) {
            if( share_letter( c, first, last ) ) {
                return( FALSE );
            }
            c = next[c];
        }
    } else {
        while( freq[c] != 0 && c != most_used_character ) {
            if( share_letter( c, first, first ) ) {
                return( FALSE );
            }
            if( share_letter( c, last, last ) ) {
                return( FALSE );
            }
            c = next[c];
        }
    }
    return( TRUE );
}

boolean try_hash( letter_t c )
/****************************/
{
    unsigned found;
    keyword_t index;
    keyword_t i;
    boolean works;
    unsigned first_weight;
    unsigned last_weight;
    unsigned adjust;

    works = TRUE;
    first_weight = first_scale * weights[c];
    last_weight = last_scale * weights[c];
    for( i = 1; i <= num_keywords; ++i ) {
        if( first[ i ] == c ) {
            if( last[ i ] == c ) {
                adjust = first_weight + last_weight;
                found = 2;
            } else {
                adjust = first_weight;
                found = 1;
            }
        } else if( last[ i ] == c ) {
            adjust = last_weight;
            found = 1;
        } else {
            continue;
        }
        if( done[i] + found == 2 ) {
            index = do_hash( hash[ i ] + adjust );
            if( used[ index ] != NULL_KEYWORD ) {
                works = FALSE;
                break;
            }
            done[ i ] += found;
            hash[ i ] += adjust;
            used[ index ] = i;
        } else {
            done[ i ] += found;
            hash[ i ] += adjust;
        }
    }
    if( works == TRUE ) {
        works = check( next[ c ] );
    }
    if( works == FALSE ) {
        undo( c, i - 1 );
    }
    return( works );
}

unsigned next_weight( void )
/**************************/
{
    int i;

    for( i = 1; i <= MAX_WEIGHTS; ++i ) {
        if( available[i] ) {
            return( i );
        }
    }
    return( 0 );
}

void try_for_hash( void )
/***********************/
{
    boolean     works;
    letter_t    c;
    keyword_t   search;

    c = most_used_character;
    weights[ c ] = 1;
    available[ weights[c] ] = FALSE;
    do {
        works = try_hash( c );
        if( works ) {
            available[ weights[ c ] ] = FALSE;
            c = next[ c ];
            if( c == most_used_character )
                break;
            search = next_weight();
            if( search == 0 )
                break;
            weights[ c ] = search;
        } else {
            do {
                while( weights[c] == num_keywords && c != most_used_character ){
                    c = prev[ c ];
                    available[ weights[c] ] = TRUE;
                    undo( c, num_keywords );
                }
                if( c == most_used_character )
                    break;
                weights[ c ]++;
            } while( ! available[ weights[c] ] );
        }
    } while( ( freq[ c ] != 0 ) && ( c != most_used_character ) );
    while( c != most_used_character ) { /* initialize rest of weights */
        search = next_weight();
        if( search == 0 )
            break;
        available[ search ] = FALSE;
        weights[ c ] = 0;
        c = next[ c ];
    }
}

boolean hash_ok( void )
/*********************/
{
    keyword_t i,j,h;

    for( i = 1; i <= num_keywords; ++i ) {
        h = do_hash( hash[i] );
        for( j = i + 1; j <= num_keywords; ++j ) {
            if( h == do_hash( hash[j] ) ) {
                return( FALSE );
            }
        }
    }
    return( TRUE );
}

boolean quick_failure( void )
/***************************/
{
    keyword_t i,j;

    for( i = 1; i <= num_keywords; ++i ) {
        for( j = i+1; j <= num_keywords; ++j ) {
            if( init_hash[i] == init_hash[j] ) {
                if( first[i] == first[j] ) {
                    if( last[i] == last[j] ) {
                        output( "quick check: %s -- %s\n", tokens[i], tokens[j] );
                        return( TRUE );
                    }
                }
            }
        }
    }
    return( FALSE );
}

boolean hash_func( unsigned first_index, unsigned last_index )
/************************************************************/
{
    keyword_t i,w;

    output( "\ntrying hash function ( len+id[%u]*%u+id[(len-1)-%u]*%u ) mod %u\n",
                                first_index, first_scale,
                                last_index, last_scale,
                                hashsize );
    init_arrays( first_index, last_index );
    sort_frequency();
    if( quick_failure() ) {
        return( FALSE );
    }
    try_for_hash();
    if( hash_ok() ) {
        return( TRUE );
    }
    w = 1;
    for( i = 1; i <= num_keywords; ++i ) {
        if( collisions[i] >= collisions[w] ) {
            w = i;
        }
    }
    output( "keyword '%s' had the most collisions\n", tokens[w] );
    return( FALSE );
}

void dump_common_defs( unsigned first_index, unsigned last_index )
/****************************************************************/
{
    fprintf( outfile, "#define KEYWORD_HASH  %d\n", hashsize );
    fprintf( outfile, "#define FIRST_INDEX   %d\n", first_index );
    fprintf( outfile, "#define FIRST_SCALE   %d\n", first_scale );
    fprintf( outfile, "#define LAST_INDEX    %d\n", last_index );
    fprintf( outfile, "#define LAST_SCALE    %d\n", last_scale );
    fprintf( outfile, "#define LEN_MAX       %d\n", max_len );
    fprintf( outfile, "#define LEN_MIN       %d\n", min_len );
    fprintf( outfile, "#define LEN_MASK      0x%lx\n", len_mask );
}

void dump_weights( unsigned first_index, unsigned last_index )
/************************************************************/
{
    int bad_mask_hash;
    unsigned mask;
    char *prt_c;
    letter_t c;
    keyword_t i;
    unsigned h;

    outfile = fopen( "weights.gh", "w" );
    dump_common_defs( first_index, last_index );
    bad_mask_hash = 0;
    for( mask = 1; mask != 0; mask <<= 1 ) {
        if( mask >= hashsize ) {
            for( i = 1; i <= num_keywords; ++i ) {
                h = hash[i];
                h &= mask - 1;
                if( mask != hashsize ) {
                    if( h >= hashsize ) {
                        h -= hashsize;
                    }
                }
                if( h != do_hash(hash[i]) ) {
                    output( "  %s: %u+%u*%u+%u*%u=%u mod %u = %u (%u)\n",
                            tokens[i],
                            init_hash[i],
                            first_scale, weights[ first[i] ],
                            last_scale, weights[ last[i] ],
                            hash[i],
                            hashsize,
                            do_hash( hash[i] ),
                            h );
                    bad_mask_hash = 1;
                    //break;
                }
            }
            break;
        }
    }
    if( bad_mask_hash ) {
        output( "code will use '%%' operator\n" );
    } else {
        fprintf( outfile, "#define KEYWORD_HASH_MASK  0x%0x\n", mask - 1 );
        if( mask != hashsize ) {
            fprintf( outfile, "#define KEYWORD_HASH_EXTRA\n" );
        }
    }
    fputc( '\n', outfile );
    for( c = LETTER_MIN; c <= LETTER_MAX; ++c ) {
        prt_c = make_define( c );
        fprintf( outfile, "#define W_%s     ", prt_c );
        fprintf( outfile, "%2d\n", weights[c]);
    }
    fclose( outfile );
}

void init_ordered( void )
/***********************/
{
    keyword_t i, j;

    for( i = 1; i <= num_keywords; ++i ) {
        j = do_hash( hash[ i ] );
        ordered[ j + 1 ] = i;
    }
}

unsigned dump_token_name( keyword_t k )
/*************************************/
{
    char *tok;
    unsigned j;
    unsigned n;
    int c;
    char buff[32];

    if( k == 0 ) {
        n = sprintf( buff, "__EXTRA%03u", extra );
        tok = buff;
    } else {
        n = len[ k ];
        tok = tokens[ k ];
    }
    for( j = 0; j < n; ++j ) {
        c = tok[ j ];
        fputc( toupper( c ), outfile );
    }
    return( j );
}

void dump_n_blanks( int n )
/*************************/
{
    if( n <= 0 ) {
        fputc( ' ', outfile );
        return;
    }
    for( ; n != 0; --n ) {
        fputc( ' ', outfile );
    }
}

unsigned dump_string( char *s )
/*****************************/
{
    unsigned b;
    unsigned n;

    fputc( '"', outfile );
    n = 1;
    b = 1;
    while( *s ) {
        fputc( *s, outfile );
        ++b;
        ++n;
        ++s;
    }
    if( flags.align ) {
        while( b % 4 ) {
            fputc( '\\', outfile );
            fputc( '0', outfile );
            n += 2;
            ++b;
        }
    }
    fputc( '"', outfile );
    ++n;
    return( n );
}

void dump_hash( void )
/********************/
{
    keyword_t i;
    unsigned j;
    unsigned ord;
    unsigned k;

    init_ordered();
    outfile = fopen( "keywords.gh", "w" );
    extra = 0;
    for( i = 1; i <= hashsize; ++i ) {
        ord = ordered[i];
        fputs( "pick( T_", outfile );
        if( ord == 0 || ord > num_keywords ) {
            dump_token_name( 0 );
            fprintf( outfile, ",\"???%02u\",TC_UNKNOWN", extra );
            ++extra;
        } else {
            j = dump_token_name( ord );
            ++extra;
            fputc( ',', outfile );
            dump_n_blanks( 15 - j );
            if( ord != 0 ) {
                k = dump_string( tokens[ ord ] ) - 2;
                fputc( ',', outfile );
            } else {
                fprintf( outfile, "NULL," );
                k = 2;
            }
            dump_n_blanks( 15 - k );
            fprintf( outfile, "%s", token_class[ ord ] );
        }
        if( pick_extension != NULL ) {
            fprintf( outfile, " %s", pick_extension );
        }
        fprintf( outfile, " )\n" );
    }
    fclose( outfile );
}

void dump_tiny( unsigned first_index, unsigned last_index )
/*********************************************************/
{
    int i;
    int c;
    letter_t letter;
    int min_char;
    int max_char;
    unsigned ord;

    min_char = INT_MAX;
    max_char = 0;
    for( i = LETTER_MIN; i <= LETTER_MAX; ++i ) {
        if( weights[i] != 0 ) {
            c = make_char( i );
            if( c < min_char ) {
                min_char = c;
            }
            if( c > max_char ) {
                max_char = c;
            }
        }
    }
    assert( min_char != INT_MAX );
    outfile = fopen( "tiny.gh", "w" );
    dump_common_defs( first_index, last_index );
    fprintf( outfile, "#define TINY_MIN_CHAR '%c'\n", min_char );
    fprintf( outfile, "static unsigned char TINY_WEIGHTS[%u] = {\n",
                (( max_char - min_char ) + 4 ) & ~3 );
    for( c = min_char; c <= max_char; ++c ) {
        letter = make_letter( c );
        fprintf( outfile, "%6u, /* '%c' */\n", weights[ letter ], c );
    }
    fputs( "};\n", outfile );
    init_ordered();
    fputs( "static char const * const TINY_IDS[] = {\n", outfile );
    for( i = 1; i <= hashsize; ++i ) {
        ord = ordered[ i ];
        if( ord == 0 || ord > num_keywords ) {
            dump_string( "" );
        } else {
            dump_string( tokens[ ord ] );
        }
        fputs( ",\n", outfile );
    }
    fputs( "};\n", outfile );
    fputs( "#define TINY_DEFS \\\n", outfile );
    extra = 0;
    for( i = 1; i <= hashsize; ++i ) {
        ord = ordered[ i ];
        fputs( "TINY_DEF( ", outfile );
        if( ord == 0 || ord > num_keywords ) {
            dump_token_name( 0 );
        } else {
            dump_token_name( ord );
        }
        ++extra;
        fputs( " ) \\\n", outfile );
    }
    fputs( "/* TINY_DEFS */\n", outfile );
    fclose( outfile );
}

boolean hash_search( void )
/*************************/
{
    unsigned i,j;
    boolean ok;

    hashsize = num_keywords;
    for( hashmask = 1; hashmask < hashsize; hashmask <<= 1 ) {
        // set hashmask to 2^n greater than or equal to hashsize
    }
    if( flags.mask_hash ) {
        hashsize = hashmask;
    }
    do {
        for( i = 0; i < max_len; ++i ) {
            for( j = 0; j < max_len + 1; ++j ) {
                ok = hash_func( j, i );
                if( ok ) {
                    if( hashsize == num_keywords ) {
                        output( "Perfect hash function found!\n" );
                    } else {
                        output( "Hash function found.\n" );
                    }
                    if( flags.tiny_output ) {
                        dump_tiny( j, i );
                    } else {
                        dump_weights( j, i );
                        dump_hash();
                    }
                    return( TRUE );
                }
            }
        }
        if( ! flags.imperfect )
            break;
        ++hashsize;
    } while( hashsize <= hashmask );
    return( FALSE );
}

int main( int argc, char **argv )
/*******************************/
{
    if( argc == 1 ) {
        error( "usage: findhash [-a] [-q] [-i] [-m] [-t] [-e \"text\"] <keyword_file> ..." );
        error( "-a: output aligned strings" );
        error( "-e \"text\": pick macro extension text" );
        error( "-i: allow imperfect hash functions" );
        error( "-m: force 2^n mask hash function" );
        error( "-q: quiet mode" );
        error( "-t: tiny mode: generate tiny.gh" );
        fatal( "" );
    }
    init_tokens( &argv[1] );
    first_scale = 1;
    last_scale = 1;
    if( ! hash_search() ) {
        first_scale = 2;
        if( ! hash_search() ) {
            first_scale = 1;
            last_scale = 2;
            if( ! hash_search() ) {
                fatal( "cannot find a hash function" );
            }
        }
    }
    return( EXIT_SUCCESS );
}

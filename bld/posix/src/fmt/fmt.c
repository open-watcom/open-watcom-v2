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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

#define  DEF_PARA_LEN   128
#define  MIN_PARA_LEN   1
#define  DEF_LINE_LEN   72
#define  DEF_LIST_LEN   512
#define  MIN_WORD_LEN   8

#define  FMT_CENTRE     0x01
#define  FMT_JUSTIFY    0x02
#define  FMT_NOSPACE    0x04

#define  END_LINE       0x01
#define  END_PARA       0x02
#define  END_FILE       0x04
#define  OUT_OF_MEM     0x08

char *OptEnvVar="fmt";

static const char *usageMsg[] = {
    "Usage: fmt [-?Xcnj] [-l length] [-p offset] [@env] [files...]",
    "\tenv                : environment variable to expand",
    "\tfiles              : files to format",
    "\tOptions: -?        : display this message",
    "\t\t -c        : centre",
    "\t\t -j        : right justify",
    "\t\t -n        : do not preserve word spacing",
    "\t\t -l length : set output line length",
    "\t\t -p offset : set output page offset",
    "\t\t -X        : match files by regular expressions",
    NULL
};

typedef struct wordlist {
    unsigned            size;           // Structure for some word text.
    unsigned            len;
    struct wordlist    *next;
    char                buf[ 1 ];
} wordlist;

typedef struct word {                   // Structure for a single Word.
    char        *text;
    unsigned     len;
    unsigned     spc;
} word;

typedef struct para {                   // Structure for a single Paragraph.
    word        *words;
    unsigned     len;
    unsigned     indent;
    unsigned     offset;
    unsigned     size;
} para;

typedef struct range {                  // Structure for a particular Range
    long         cost;                  // of words that might form a line.
    unsigned     start;
    unsigned     left;
} range;


/*
 * Global data.
 */

static char             *w_buff = NULL;     // word buffer
static unsigned          w_size = 0;        // size of word buffer

static int               f_mode = 0;        // formatting mode

/*
 * Local functions.
 */

static void fputspc( int num )
{
    for( ; num > 0; num-- ) {           // simply output a given number of
        fputchar( ' ' );                // spacees
    }
}

static void justifyParagraph( para *p, range *r, int lines, int err )
{
    int         i, j, upper, blanks;
    div_t       s;

    for( i = 0; i < lines; i++ ) {
        if( i < lines - 1 ) {
            upper = r[i+1].start - 1;
        } else if( err ) {                  // If para. break was due to out
            upper = p->len - 1;             // of mem error, then justify last
        } else {                            // line of paragraph.  If not, then
            break;                          // don't justify last line.
        }
        blanks = upper - r[i].start;
        if( blanks == 0 ) {                 // Only one word on the line.
            continue;
        }
        s = div( r[i].left, blanks );               // Divide spaces among the
        for( j = r[i].start; j < upper; j++ ) {     // inter-word blanks.
            p->words[ j ].spc += s.quot;
        }
        if( s.rem != 0 ) {                  // Distribute remaining spaces
            blanks /= s.rem;                // somewhat equally.
            if( i % 2 == 0 ) {
                for( j = r[i].start; s.rem > 0; j += blanks ) {
                    p->words[j].spc += 1;
                    s.rem--;
                }
            } else {
                for( j = upper - 1; s.rem > 0; j -= blanks ) {
                    p->words[j].spc += 1;
                    s.rem--;
                }
            }
        }
    }
}

static void outputParagraph( para *p, range *r, int lines )
{
    int         i, j;

    fputspc( p->indent );
    for( i = 0; i < lines - 1; i++ ) {
        if( f_mode & FMT_CENTRE ) {
            fputspc( r[i].left / 2 );
        }
        for( j = r[ i ].start; j < r[ i + 1 ].start - 1; j++ ) {
            fputs( p->words[ j ].text, stdout );
            fputspc( p->words[ j ].spc );
        }
        fputs( p->words[ j ].text, stdout );
        fputchar( '\n' );
        fputspc( p->offset );
    }
    if( f_mode & FMT_CENTRE ) {
        fputspc( r[lines - 1].left / 2 );
    }
    for( j = r[lines - 1].start; j < p->len - 1; j++ ) {
        fputs( p->words[ j ].text, stdout );
        fputspc( p->words[ j ].spc );
    }
    fputs( p->words[ j ].text, stdout );
    fputchar( '\n' );
}

static void formatParagraph( para *p, int width, int offset, int err )
{
    range      *r;
    int         i, j;
    long        cost, tempcost;

    unsigned    spcleft = 0;
    unsigned    comp    = p->len - 1;
    unsigned    start   = 0;
    long        inicost = width;
    long        length  = p->offset;

    r = (range *) malloc( p->len * sizeof( range ) );

    if( err ) {
        comp++;
    }
    p->words[ 0 ].len += p->indent - p->offset;

    for( j = 0; j < p->len; j++ ) {
        r[j].cost = LONG_MAX;
        length += p->words[ j ].len;
        tempcost = inicost - length;
        p->words[j].len += p->words[j].spc;
        for( i = start; i <= j; i++ ) {
            cost = tempcost;
            tempcost += p->words[i].len;
            if( cost >= 0 ) {
                spcleft = cost;                 // spaces left on line
                if( j < comp ) {
                    cost *= cost;               // calculate cost of range
                } else {
                    cost = 0;                   // so you can ignore last line
                }
                if( i != 0 ) {
                    cost += r[i-1].cost;
                }
                if( cost < r[j].cost ) {
                    r[j].left  = spcleft;
                    r[j].cost  = cost;
                    r[j].start = i;
                }
            } else {
                if( i == j ) {
                    r[j].cost = 0;
                    r[j].start = i;
                }
                length -= p->words[ start ].len;
                start++;
            }
        }
        length += p->words[j].spc;
    }

    j = p->len - 1;
    for( i = j; i >= 0; i-- ) {         // Walk back through table to find
        r[i].start = r[j].start;        // optimal path.  Store path in the
        r[i].left  = r[j].left;         // upper part of the range array.
        if( r[j].start == 0 ) {
            break;
        }
        j = r[i].start - 1;
    }

    // Note:  Centring takes priority over justification.  The output function
    //        actually handles most of the centring - here we only split the
    //        paragraph offsets/indents.  To add right-justification, simply
    //        don't split the offsets/indents (here) and don't split the
    //        spaces left on lines. (in the output fcn.)

    if( f_mode & FMT_CENTRE ) {
        p->offset /= 2;
        p->indent /= 2;
    } else if( f_mode & FMT_JUSTIFY ) {
        justifyParagraph( p, r + i, p->len - i, err );
    }

    p->offset += offset;
    p->indent += offset;

    outputParagraph( p, r + i, p->len - i );

    free( r );
}

static void freeWordlist( wordlist *list )
{
    wordlist    *temp;

    while( list != NULL ) {
        temp = list->next;              // free up all the text memory
        free( list );
        list = temp;
    }
}

static void resetParagraph( para *p )
{
    int         i;

    for( i = 0; i < p->len; i++ ) {     // Reset word lengths and
        p->words[ i ].len = 0;          // space counts
        p->words[ i ].spc = 0;
    }

    p->offset = 0;
    p->indent = 0;
    p->len    = 0;                      // Reset paragraph settings
}

static void resetWordlist( wordlist *list )
{
    for( ; list != NULL; list = list->next ) {  // Reset wordlist settings
        list->len = 0;
    }
}

static void trimParagraph( para *p )
{
    if( p->size > p->len ) {
        p->size  = p->len + 1;
        p->words = (word *) realloc( p->words, p->size * sizeof( word ) );
    }
}

static int expandParagraph( para *p, unsigned inc )
{
    word        *tmp;

    if( p->len >= p->size ) {
        p->size += inc;
        tmp = (word *) realloc( p->words, p->size * sizeof( word ) );
        if( tmp != NULL ) {
            p->words = tmp;
            for( tmp += p->len; tmp < p->words + p->size; tmp++ ) {
                memset( tmp, 0, sizeof( word ) );
            }
            return( 0 );
        } else {
            p->size -= inc;
            return( 1 );
        }
    }
    return( 0 );
}

static int getIndentation( FILE *fp, unsigned *os )
{
    int         ch = 0;

    while( 1 ) {
        ch = fgetc( fp );
        if( ch == ' ' ) {
            *os += 1;
        } else if( ch == '\t' ) {
            *os += 8 - *os % 8;
        } else {
            break;
        }
    }
    if( ch == '\n' ) {
        *os = 0;
        return( END_LINE );
    } else if( ch == EOF ) {
        *os = 0;
        return( END_FILE );
    } else {
        ungetc( ch, fp );
        return( 0 );
    }
}

static char *insertWord( wordlist **list, char *wtext, unsigned wlen )
{
    wordlist    *temp;
    char        *text;
    unsigned     size;

    for( temp = *list; temp != NULL; temp = temp->next ) {
        if( wlen < temp->size - temp->len ) {
            text = temp->buf + temp->len;
            strcpy( text, wtext );
            temp->len += wlen + 1;
            return( text );
        }
    }
    size = wlen + 1;
    if( size < DEF_LIST_LEN )
        size = DEF_LIST_LEN;
    while( size > wlen ) {
        temp = (wordlist *) realloc( temp, sizeof( wordlist ) + size - sizeof( char ) );
        if( temp != NULL ) {
            break;
        } else {
            size /= 2;
        }
    }
    if( temp == NULL ) {
        return( NULL );
    } else {
        temp->size = size;
        temp->next = *list;
        temp->len  = wlen + 1;
        strcpy( temp->buf, wtext );
        *list = temp;
        return( temp->buf );
    }
}

static int getWord( FILE *fp, wordlist **list, word *w, unsigned *os )
{
    static int           ret  = 0;

    char                *tmp  = NULL;
    int                  ch   = 0;
    int                  pv   = 0;
    int                  ppv  = 0;

    ret &= ~OUT_OF_MEM;
    if( w->len == 0  ||  w->len >= w_size ) {
        ret = 0;
        while( 1 ) {
            if( w->len >= w_size ) {
                w_size += MIN_WORD_LEN;
                tmp  = (char *) realloc( w_buff, w_size );
                if( tmp == NULL ) {
                    w_size -= MIN_WORD_LEN;
                    return( OUT_OF_MEM );
                }
                w_buff = tmp;
            }
            ppv = pv;
            pv  = ch;
            ch  = fgetc( fp );
            if( ch == '\n'  ||  ch == '\t'  ||  ch == ' '  ||  ch == EOF ) {
                break;
            } else {
                *os += 1;
                w_buff[ w->len ] = ch;
                w->len++;
            }
        }
        while( 1 ) {
            if( ch == ' ' ) {
                w->spc++;
                *os += 1;
            } else if( ch == '\t' ) {
                w->spc += 8 - *os % 8;
                *os += 8 - *os % 8;
            } else {
                break;
            }
            ch = fgetc( fp );
        }

        if( w->len > 0 ) {
            w_buff[ w->len ] = '\0';
        }
        if( ch == EOF ) {
            *os = 0;
            w->spc = 1;
            ret |= END_FILE;
        } else if( ch == '\n' ) {
            *os = 0;
            w->spc = 1;
            ret |= END_LINE;
        } else {
            ungetc( ch, fp );
        }

        if( w->spc == 0 ) {
            w->spc = 1;
        } else if( f_mode & FMT_NOSPACE ) {
            if( pv == '.'  ||  pv == '?'  ||  pv == '!' ) {
                if( isupper( ppv ) ) {
                    w->spc = 1;
                } else {
                    w->spc = 2;
                }
            } else {
                w->spc = 1;
            }
        }
    }
    w->text = insertWord( list, w_buff, w->len );
    if( w->text == NULL ) {
        ret |= OUT_OF_MEM;
    }
    return( ret );
}

static int getParagraph( FILE *fp, para *p, wordlist **list )
{
    static  unsigned    lastos = 0;
    static  unsigned    lineos = 0;
    static  unsigned    curros = 0;
    static  int         start  = -1;

    int                 status = 0;
    int                 retval = 0;

    while( 1 ) {
        if( expandParagraph( p, MIN_PARA_LEN ) ) {
            return( OUT_OF_MEM );
        }
        if( lineos == 0 ) {
            lastos = curros;
            status = getIndentation( fp, &lineos );
            curros = lineos;
            start++;
        }
        if( (status & END_FILE)  ||  (status & END_LINE) ) {
            curros = 0;
            start  = 0;
            return( status );
        } else if( curros > lastos  &&  start ) {
            lastos = curros;
            return( status );
        }
        if( curros > p->indent ) {
            p->indent = curros;
            p->offset = curros;
        }
        if( curros < p->offset ) {
            p->offset = curros;
        }

        retval = getWord( fp, list, &p->words[ p->len ], &lineos );

        if( retval & OUT_OF_MEM ) {
            return( OUT_OF_MEM );
        }
        p->len++;
        if( retval & END_FILE ) {
            return( END_FILE );
        }
    }
}

static void formatFile( FILE *fp, int width, int offset )
{
    int         ret;

    para        p     = { NULL, 0, 0, 0, 0 };
    int         oldos = 0;
    int         erros = -1;
    wordlist   *list  = NULL;

    expandParagraph( &p, DEF_PARA_LEN );

    for( ; ; ) {
        ret = getParagraph( fp, &p, &list );

        if( ret & OUT_OF_MEM ) {
            if( erros != p.len ) {
                trimParagraph( &p );
                erros = p.len;
                continue;
            } else if( p.len == 0 ) {
                Die( "fmt: out of memory error\n" );
            }
        }

        if( p.len != 0 ) {
            if( p.indent <= oldos ) {       // keep track of offsets & indents
                p.indent = oldos;           // in case of out of mem. errors
                p.offset = oldos;
            }
            oldos = p.offset;
            formatParagraph( &p, width, offset, ret & OUT_OF_MEM );
        }
        if( ret & END_LINE ) {              // no need to format blank line
            fputchar( '\n' );
            oldos = 0;
        }

        resetParagraph( &p );
        resetWordlist( list );

        if( ret & END_FILE ) {
            break;
        }
        erros = -1;
    }
    free( p.words );                    // free the paragraph space
    freeWordlist( list );               // free the text space
}

void main( int argc, char **argv )
{
    FILE       *fp;
    int         ch;

    int         width  = DEF_LINE_LEN;
    int         offset = 0;
    int         regexp = 0;

    argv = ExpandEnv( &argc, argv );

    while( 1 ) {
        ch = GetOpt( &argc, argv, "Xcjnl:p:", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
            case 'c':
                f_mode |= FMT_CENTRE;
                break;
            case 'j':
                f_mode |= FMT_JUSTIFY;
                break;
            case 'n':
                f_mode |= FMT_NOSPACE;
                break;
            case 'l':
                width = atoi( OptArg );
                break;
            case 'p':
                offset = atoi( OptArg );
                break;
            case 'X':
                regexp = 1;
                break;
        }
    }

    if( width <= 0 ) {
        Die( "fmt: invalid line length\n" );
    }
    if( offset < 0 ) {
        Die( "fmt: invalid page offset\n" );
    }

    argv = ExpandArgv( &argc, argv, regexp );
    argv++;

    if( *argv == NULL ) {
        formatFile( stdin, width, offset );
    } else {
        while( *argv != NULL ) {
            fp = fopen( *argv, "r" );
            if( fp == NULL ) {
                fprintf( stderr, "fmt: cannot open input file \"%s\"\n", *argv );
            } else {
                if( argc > 2 ) {
                    fprintf( stdout, "%s:\n", *argv );
                }
                formatFile( fp, width, offset );
                fclose( fp );
            }
            argv++;
        }
    }
    free( w_buff );                     // free the word space
    exit( EXIT_SUCCESS );
}

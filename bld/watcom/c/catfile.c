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
* Description:  CATFILE -- better file concatenator.
*
****************************************************************************/


#if defined( __UNIX__ )
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "bool.h"


typedef struct PathGroup {
    char    *drive;
    char    *dir;
    char    *fname;
    char    *ext;
    char    buffer[FILENAME_MAX + 4];
} PGROUP;

struct {                        // Program switches
    unsigned sort_date  :1;     // - sort by date
    unsigned sort_kluge :1;     // - sort by date (kluged tie-break)
    unsigned sort_alpha :1;     // - sort alphabetically
    unsigned emit_hdr   :1;     // - emit header for file
} switches = {
    false,
    false,
    false,
    false
};

typedef struct {                // Text
    void* next;                 // - next in ring
    time_t time;                // - date/time for file
    char text[1];               // - variable-sized text
} Text;

                                // static data
static Text* file_patterns;     // - ring of file patterns
static Text* files;             // - ring of files


static char* help_text[] = {    // help text
    "catfile switches pattern ... pattern >destination",
    "",
    "Concatenate all files matching the patterns into the destination file.",
    "",
    "File patterns are the usual patterns such as \"*.err\"",
    "",
    "Switches are:",
    "   /a -- sort files alphabetically",
    "   /d -- sort files by date",
    "   /h -- emit header before each file",
    "   /k -- sort files by date (break ties by digits in name)",
    "",
    "Default switch settings are /k",
    NULL
};


static int errMsg               // PRINT ERROR MESSAGE
    ( const char* text          // - text segments
    , ... )
{
    va_list args;               // - error list

    va_start( args, text );
    fputs( "*** CATFILE Error ***", stderr );
    for( ; text != NULL; ) {
        fputc( ' ', stderr );
        fputs( text, stderr );
        text = va_arg( args, char const * );
    }
    fputc( '\n', stderr );
    va_end( args );
    return 1;
}


static void turnOffOrder        // REMOVE ORDER SWITCHES
    ( void )
{
    switches.sort_date = false;
    switches.sort_alpha = false;
}


static void help                // HELP TEXT
    ( void )
{
    char** hp;                  // - current help line

    hp = help_text;
    for( ; *hp != NULL; ++hp ) {
        fputs( *hp, stdout );
        fputc( '\n', stdout );
    }
}


static int textAlloc            // ALLOCATE TEXT ITEM
    ( size_t size               // - text size
    , Text** a_text )           // - addr[ text entry ]
{
    int retn;                   // - return code
    Text *tp;                   // - new Text

    tp = malloc( sizeof( *tp ) + size );
    *a_text = tp;
    if( tp == NULL ) {
        retn = errMsg( "out of memory", NULL );
    } else {
        tp->time = 0;
        retn = 0;
    }
    return retn;
}


static int textForAll           // PROCESS ALL ITEMS IN A TEXT RING
    ( Text* ring                // - text ring
    , void* data                // - data passed to routine
    , int (*rtn)( Text*, void* )) // - routine
{
    int retn;                   // - return code
    Text* curr;                 // - current item

    if( ring == NULL ) {
        retn = 0;
    } else {
        curr = ring;
        do {
            curr = curr->next;
            retn = rtn( curr, data );
            if( retn != 0 ) break;
        } while( curr != ring );
    }
    return retn;
}


static Text* textInsert         // INSERT INTO RING
    ( Text* tp                  // - Text entry
    , Text** a_ring )           // - Text ring
{
    Text *ring;                 // - ring

    ring = *a_ring;
    if( ring == NULL ) {
        tp->next = tp;
    } else {
        tp->next = ring->next;
        ring->next = tp;
    }
    *a_ring = tp;
    return tp;
}



static int processSwitch        // PROCESS SWITCH
    ( char const *sw )          // - switch
{
    int retn;                   // - return code

    switch( sw[1] ) {
      case 'd' :
        turnOffOrder();
        switches.sort_date = true;
        retn = 0;
        break;
      case 'k' :
        turnOffOrder();
        switches.sort_kluge = true;
        retn = 0;
        break;
      case 'a' :
        turnOffOrder();
        switches.sort_alpha = true;
        retn = 0;
        break;
      case 'h' :
        switches.emit_hdr = true;
        retn = 0;
        break;
      default :
        retn = errMsg( "invalid switch", sw, NULL );
        break;
    }
    return retn;
}

#define IS_WILDCARD_CHAR( x ) ((*x == '*') || (*x == '?'))
#if defined( __UNIX__ )
#define FNAMECMPCHAR(a,b) (a-b)
#else
#define FNAMECMPCHAR(a,b) (tolower(a)-tolower(b))
#endif

static int __fnmatch( char *pattern, char *string )
/**************************************************
 * OS specific compare function FNameCmpChr
 * must be used for file names
 */
{
    char    *p;
    int     len;
    int     star_char;
    int     i;

    /*
     * check pattern section with wildcard characters
     */
    star_char = 0;
    while( IS_WILDCARD_CHAR( pattern ) ) {
        if( *pattern == '?' ) {
            if( *string == 0 ) {
                return( 0 );
            }
            string++;
        } else {
            star_char = 1;
        }
        pattern++;
    }
    if( *pattern == 0 ) {
        if( (*string == 0) || star_char ) {
            return( 1 );
        } else {
            return( 0 );
        }
    }
    /*
     * check pattern section with exact match
     * ( all characters except wildcards )
     */
    p = pattern;
    len = 0;
    do {
        if( star_char ) {
            if( string[len] == 0 ) {
                return( 0 );
            }
            len++;
        } else {
            if( FNAMECMPCHAR( *pattern, *string ) != 0 ) {
                return( 0 );
            }
            string++;
        }
        pattern++;
    } while( *pattern && !IS_WILDCARD_CHAR( pattern ) );
    if( star_char == 0 ) {
        /*
         * match is OK, try next pattern section
         */
        return( __fnmatch( pattern, string ) );
    } else {
        /*
         * star pattern section, try locate exact match
         */
        while( *string ) {
            if( FNAMECMPCHAR( *p, *string ) == 0 ) {
                for( i = 1; i < len; i++ ) {
                    if( FNAMECMPCHAR( *(p + i), *(string + i) ) != 0 ) {
                        break;
                    }
                }
                if( i == len ) {
                    /*
                     * if rest doesn't match, find next occurence
                     */
                    if( __fnmatch( pattern, string + len ) ) {
                        return( 1 );
                    }
                }
            }
            string++;
        }
        return( 0 );
    }
}


static int processFilePattern   // PROCESS FILE PATTERN
    ( Text* tp                  // - file pattern
    , void* data )              // - not used
{
    int             retn;       // - return code
    DIR             *dp;        // - directory stuff
    struct dirent   *entry;
    struct stat     buf;
    PGROUP          pg;
    char            path[ _MAX_PATH ];
    char            pattern[ _MAX_PATH ]; // - file pattern

    data = data;
    _splitpath2( tp->text, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( path, pg.drive, pg.dir, ".", NULL );
    _makepath( pattern, NULL, NULL, pg.fname, pg.ext );
    dp = opendir( path );
    if( dp == NULL ) {
        retn = errMsg( "opening directory:", path, NULL );
    } else {
        retn = 0;
        for( entry = readdir( dp ); entry != NULL; entry = readdir( dp ) ) {
#if !defined( __UNIX__ )
            if( ( entry->d_attr & _A_VOLID ) == 0 ) {
#endif
                if( __fnmatch( pattern, entry->d_name ) ) {
                    _makepath( path, pg.drive, pg.dir, entry->d_name, NULL );
                    if( stat( path, &buf ) == 0 && S_ISREG( buf.st_mode ) ) {
                        Text* tp;           // - current entry
                        retn = textAlloc( strlen( path ) + 1, &tp );
                        if( retn != 0 ) break;
                        textInsert( tp, &files );
                        strcpy( files->text, path );
                        files->time = buf.st_mtime;
                    }
                }
#if !defined( __UNIX__ )
            }
#endif
        }
    }
    return retn;
}


static void emitHdr             // EMIT HDR LINE
    ( size_t size )             // - width of header
{
    for( ; size >0; --size ) {
        fputc( '*', stdout );
    }
    fputc( '\n', stdout );
}


static int concFile             // CONCATENATE A FILE
    ( Text* tp                  // - file entry
    , void* data )              // - date: unused
{
    int retn;                   // - return code
    FILE* fp;                   // - file stuff
    char rec[1024];             // - record

    data = data;
    fp = fopen( tp->text, "rt" );
    if( fp == NULL ) {
        retn = errMsg( "cannot open:", tp->text, NULL );
    } else {
        if( switches.emit_hdr ) {
            size_t hs = strlen( tp->text ) + 3;
            emitHdr( hs );
            fputs( "** ", stdout );
            fputs( tp->text, stdout );
            fputc( '\n', stdout );
            emitHdr( hs );
        }
        while( fgets( rec, sizeof( rec ), fp ) != NULL ) {
            fputs( rec, stdout );
        }
        fclose( fp );
        retn = 0;
    }
    return retn;
}


static int concFiles            // CONCATENATE FILES
    ( void )
{
    return textForAll( files, NULL, &concFile );
}


static int countFile            // INCREMENT FILE CTR
    ( Text* tp                  // - text ptr.
    , void* data )              // - data: addr[ ctr ]
{
    unsigned* a_ctr = data;
    tp = tp;
    ++(*a_ctr);
    return 0;
}


static int storeFile            // STORE FILE PTR
    ( Text* tp                  // - text ptr.
    , void* data )              // - data: addr[ ctr ]
{
    Text*** a_tp = (Text***)data;
    **a_tp = tp;
    ++(*a_tp);
    return 0;
}


static int compareFileDates     // COMPARE TWO FILE DATES
    ( Text const *c1            // - comparand[1]
    , Text const *c2 )          // - comparand[2]
{
    return c1->time - c2->time;
}


static unsigned extractDigs     // EXTRACT AS UNSIGNED THE DIGITS IN NAME
    ( char const * name )       // - file name
{
    unsigned value;             // - returned value

    value = 0;
    for( ;; ) {
        char chr = *name++;
        if( chr == '\0' ) break;
        if( chr == '\\'
         || chr == ':' ) {
            value = 0;
        } else if( chr >= '0' && chr <= '9' ) {
            value = value * 10 + chr - '0';
        }
    }
    return value;
}


static int compareFiles         // COMPARE TWO FILES
    ( void const * f1           // - file [1]
    , void const * f2 )         // - file [2]
{
    int retn;                   // - return
    Text const *c1;             // - file [1]
    Text const *c2;             // - file [2]

    c1 = *(Text**)f1;
    c2 = *(Text**)f2;
    if( switches.sort_kluge ) {
        retn = compareFileDates( c1, c2 );
        if( retn == 0 ) {
            retn = strcmp( c1->text, c2->text );
            if( retn != 0 ) {
                retn = extractDigs( c1->text ) - extractDigs( c2->text );
            }
        }
    } else if( switches.sort_alpha ) {
        retn = strcmp( c1->text, c2->text );
    } else {
        retn = compareFileDates( c1, c2 );
    }
    return retn;
}


static int sortFiles            // SORT FILES
    ( void )
{
    unsigned count;             // - # of files
    Text** array;               // - array of file ptrs
    Text** store;               // - next storage point
    int retn;                   // - return code
    int index;                  // - index of "array"

    count = 0;
    textForAll( files, &count, &countFile );
    array = malloc( count * sizeof( files ) );
    if( array == 0 ) {
        retn = errMsg( "out of memory", NULL );
    } else {
        store = array;
        textForAll( files, &store, &storeFile );
        qsort( array, count, sizeof( Text* ), &compareFiles );
        files = 0;
        for( index = 0; index < count; ++index ) {
            textInsert( array[ index ], &files );
        }
        free( array );
        retn = 0;
    }
    return retn;
}



static int processFilePatterns  // PROCESS FILE PATTERNS
    ( void )
{
    int retn;                   // - return code

    if( file_patterns == 0 ) {
        retn = errMsg( "no file patterns", NULL );
    } else {
        retn = textForAll( file_patterns, NULL, &processFilePattern );
        if( retn == 0 ) {
            if( files == NULL ) {
                retn = errMsg( "no files", NULL );
            } else {
                retn = sortFiles();
            }
        }
    }
    return retn;
}


static int processCmdLine       // PROCESS COMMAND LINE
    ( int arg_count             // - # args
    , char const * args[] )     // - arguments
{
    int retn;                   // - return code
    const char* cmd;            // - current command
    int index;                  // - command-line index
    int any_options;

    any_options = 0;
    for( index = 1; index < arg_count; ++ index ) {
        cmd = args[ index ];
        if( *cmd == '-' ) {
            any_options = 1;
            retn = processSwitch( cmd );
        } else {
            Text *tp;           // - text entry
            retn = textAlloc( strlen( cmd ), &tp );
            if( retn == 0 ) {
                textInsert( tp, &file_patterns );
                strcpy( file_patterns->text, cmd );
            }
        }
        if( retn != 0 ) break;
    }
    if( ! any_options ) {
        switches.sort_kluge = true;
    }
    return retn;
}


int main                        // MAIN-LINE
    ( int arg_count             // - # args
    , char const * args[] )     // - arguments
{
    int retn;                   // - return code

    if( arg_count <= 1 ) {
        help();
        retn = 0;
    } else {
        retn = processCmdLine( arg_count, args );
        if( retn == 0 ) {
            retn = processFilePatterns();
            if( retn == 0 ) {
                concFiles();
            }
        }
    }
    return retn;
}

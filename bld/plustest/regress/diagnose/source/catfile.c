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


#ifdef __UNIX__
#include <sys/stat.h>
#else
#include "direct.h"
#endif
#include "malloc.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define TRUE  1
#define FALSE 0


struct {                        // Program switches
    unsigned sort_date  :1;     // - sort by date
    unsigned sort_kluge :1;     // - sort by date (kluged tie-break)
    unsigned sort_alpha :1;     // - sort alphabetically
    unsigned emit_hdr   :1;     // - emit header for file
} switches =
{   FALSE
,   FALSE
,   FALSE
,   FALSE
};

typedef struct {                // Text
    void* next;                 // - next in ring
    unsigned date;              // - date for file
    unsigned time;              // - time for file
    char text[1];               // - variable-sized text
} Text;


                                // static data
static Text* file_patterns;     // - ring of file patterns
static Text* files;             // - ring of files


static char* help_text[] =      // help text
{   "catfile switches pattern ... pattern >destination"
,   ""
,   "Concatenate all files matching the patterns into the destination file."
,   ""
,   "File patterns are the usual patterns such as \"*.err\""
,   ""
,   "Switches are:"
,   "   /a -- sort files alphabetically"
,   "   /d -- sort files by date"
,   "   /h -- emit header before each file"
,   "   /k -- sort files by date (break ties by digits in name)"
,   ""
,   "Default switch settings are /k"
,   NULL
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
    switches.sort_date = FALSE;
    switches.sort_alpha = FALSE;
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
        tp->date = 0;
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
        switches.sort_date = TRUE;
        retn = 0;
        break;
      case 'k' :
        turnOffOrder();
        switches.sort_kluge = TRUE;
        retn = 0;
        break;
      case 'a' :
        turnOffOrder();
        switches.sort_alpha = TRUE;
        retn = 0;
        break;
      case 'h' :
        switches.emit_hdr = TRUE;
        retn = 0;
        break;
      default :
        retn = errMsg( "invalid switch", sw, NULL );
        break;
    }
    return retn;
}


#ifdef __UNIX__
static int processFilePattern   // PROCESS FILE PATTERN
    ( Text* tp                  // - file pattern
    , void* data )              // - not used
{
    char const * pat;           // - file pattern
    int retn;                   // - return code
    struct stat st;             // - directory stuff

    data = data;
    pat = tp->text;
    if( stat( pat, &st ) ) {
        retn = errMsg( "opening file:", pat, NULL );
    } else {
        Text* tp;           // - current entry
        retn = textAlloc( strlen( pat ), &tp );
        if( retn == 0 ) {
            textInsert( tp, &files );
            strcpy( files->text, pat );
            files->time = st.st_mtime;
        }
    }
    return retn;
}
#else
static int processFilePattern   // PROCESS FILE PATTERN
    ( Text* tp                  // - file pattern
    , void* data )              // - not used
{
    char const * pat;           // - file pattern
    int retn;                   // - return code
    struct dirent* dp;          // - directory stuff
    size_t dir_size;            // - size of directory portion

    data = data;
    pat = tp->text;
    dp = opendir( pat );
    if( dp == NULL ) {
        retn = errMsg( "opening directory:", pat, NULL );
    } else {
        for( dir_size = strlen( pat ); dir_size > 0; ) {
            -- dir_size;
            if( pat[ dir_size ] == '\\' ) {
                ++ dir_size;
                break;
            }
            if( pat[ dir_size ] == ':' ) {
                break;
            }
        }
        retn = 0;
        for( ; ; ) {
            Text* tp;           // - current entry
            dp = readdir( dp );
            if( dp == NULL ) break;
            retn = textAlloc( dir_size + strlen( dp->d_name ), &tp );
            if( retn != 0 ) break;
            textInsert( tp, &files );
            memcpy( files->text, pat, dir_size );
            strcpy( &files->text[ dir_size ], dp->d_name );
            files->date = dp->d_date;
            files->time = dp->d_time;
        }
    }
    return retn;
}
#endif


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
    int retn;                   // - return

    retn = c1->date - c2->date;
    if( retn == 0 ) {
        retn = c1->time - c2->time;
    }
    return retn;
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
        switches.sort_kluge = TRUE;
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

/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Mainline for splice program.
*
****************************************************************************/


// This program reads a file and produces another according to the
// directives within the file(s) read.  The directives are:
//  (1) :include file-name
//      - includes a file
//  (2) :segment expr
//      . . .
//      {:elsesegment [expr]}
//      . . .
//      :endsegment
//      - the lines between these directives are a segment
//      - the section that is delimited by the first <expr> that is true,
//        up to the next :elsesegment or :endsegment are output.
//  (3) :keep name
//      - directs that the lines for the indicated segment are to be output,
//        when that segment is subsequently encountered
//  (4) :remove name
//      - directs that the lines for the indicated segment are to be ignored,
//        when that segment is subsequently encountered
//  (5) :: comments
//      - when the first two non-blank characters are "::" the line is treated
//        as a comment and is not written to the output file
//
//  The BNF for a expression looks like this:
//
//      expr            ::= and_expr { "|" and_expr }
//      and_expr        ::= not_expr { "&" not_expr }
//      not_expr        ::= [ "!" ] primary_expr
//      primary_expr    ::= "(" expr ")"
//                      |    name
//
// Additional Rules:
//  - when a number of :keep and :remove directives apply to a segment,
//    the last one encountered before that segment applies
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include <sys/stat.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
#include "bool.h"
#include "watcom.h"
#include "pathgrp2.h"

#include "clibext.h"


#define MAX_DEPS    50

// wsplice expression operators definition
#define OP_OR       '|'
#define OP_AND      '&'
#define OP_NOT      '!'
#define OP_LPAREN   '('
#define OP_RPAREN   ')'

#define ARRAY_SIZE(a)   (sizeof( a ) / sizeof( (a)[0] ))

enum                    // PROCESSING MODES
{
    MODE_DELETE,        // - deleting
    MODE_OUTPUT,        // - outputting
    MODE_SKIPPING       // - skipping to ENDSEGMENT
};

#define KEYWORDS  \
    pick( KW_SEGMENT,     "segment"     ) /* SEGMENT expr */        \
    pick( KW_ELSESEGMENT, "elsesegment" ) /* ELSESEGMENT [expr] */  \
    pick( KW_ENDSEGMENT,  "endsegment"  ) /* ENDSEGMENT */          \
    pick( KW_REMOVE,      "remove"      ) /* REMOVE segment-name */ \
    pick( KW_KEEP,        "keep"        ) /* KEEP segment-name */   \
    pick( KW_INCLUDE,     "include"     ) /* INCLUDE file */

typedef union textent   TEXTENT;
typedef struct segment  SEGMENT;
typedef struct filestk  FILESTK;
typedef struct segmstk  SEGMSTK;
typedef struct ipathlst IPATHLST;
typedef unsigned        PROCMODE;

struct text {                           // define TEXT
    TEXTENT             *next;          // - next element
    char                text_type;      // - type of entry
#define TEXT_DATA 'D'                   // - - data
#define TEXT_CMD  'C'                   // - - command
};

struct segcmd {                         // define CMD
    struct text         base;           // - base element
    unsigned            kw;             // - keyword
    SEGMENT             *segment;       // - segment ptr
};

struct datatext {                       // define DATA
    struct text         base;           // - base element
    char                data[1];        // - text string
};

union textent {                         // define TEXTENT
    struct text         TEXT;           // - base
    struct segcmd       CMD;            // - command
    struct datatext     DATA;           // - data
};

struct segment {                        // define SEGMENT
    SEGMENT             *next;          // - next element
    char                seg_type;       // - segment type
#define SEG_REMOVE    'R'               // - - segment to be removed
#define SEG_KEEP      'K'               // - - segment to be kept
#define SEG_NONE      ' '               // - - keep and remove not specified
    char                name[1];        // - segment name
};

struct segmstk {                         // define SEGMSTK
    SEGMSTK             *next;          // - next entry
    unsigned            rec_def;        // - record number of definition
    PROCMODE            action;         // - saved processing mode
};

struct filestk {                        // define FILESTK
    FILESTK             *next;          // - last element
    FILE                *fp;            // - file ptr.
    SEGMSTK             *segm_stk;      // - saved segment stack
    unsigned            rec_count;      // - current record
    char                name[1];        // - file name
};

struct ipathlst {                       // define ipathlst
    struct ipathlst     *next;          // - next pointer
    char                path[1];        // - path
};

typedef enum            // define KW codes
{
    #define pick(enum,text) enum,
    KEYWORDS
    #undef pick
    KW_COMMENT,         // - comment
    KW_TEXT,            // - text
    KW_EOF,             // - end of file
} KW;

// DATA (READ ONLY)

static const char * const KwTable[] = {     // - key words table
    #define pick(enum,text) text,
    KEYWORDS
    #undef pick
};

// DATA (READ/WRITE)
static unsigned     ErrCount;               // - number of errors
static FILE         *OutputFile;            // - output file
static FILESTK      *Files;                 // - stack of opened files
static SEGMENT      *Segments;              // - list of segments
static SEGMSTK      *SegmStk;               // - active-segments stack
static char         KwChar = { ':' };       // - key word definition character
static TEXTENT      *SourceText;            // - source text
static char         Token[32];              // - scan token
static char         Record[1024];           // - input record
static const char   *Rptr;                  // - ptr into record
static PROCMODE     ProcessMode;            // - processing mode
static const char   *OutFmt = "%s";         // - output format
static unsigned     OutNum = 0;             // - output number
static int          EOL_crlf = -1;          // - end-of-line crlf=1, lf=0, host=-1
static int          TabStop = 0;            // - tab spacing
static IPATHLST     *IncPathList;           // - list of include paths
static bool         RestoreTime = false;    // - set tgt-file timestamp to src-file
static char         OutBuffer[1024];        // - output buffer
static unsigned     OutBufferLen;           // - output buffer current len
static char         *DepFileName[MAX_DEPS];
static int          DepCount = 0;

static FILE         *fp_depdump = NULL;

// LOCAL ROUTINES

static bool     Expr( void );

// ERROR MESSAGE
static void Error( const char *msg, ... )
{
    char        emsg[128];      // - error message
    char        *eptr;          // - ptr. into error message
    va_list     args;           // - var arg processing
    int         count;          // - chars written;

    va_start( args, msg );

    count = sprintf( emsg, "**** WSPLICE ERROR **** " );
    eptr = &emsg[count];
    if( Files != NULL ) {
        count = sprintf( eptr, "%s, line %u ", Files->name, Files->rec_count );
        eptr = &eptr[count];
    }
    vsprintf( eptr, msg, args );
    fprintf( stderr, "%s\n\n", emsg );
    va_end( args );
    ++ErrCount;
}

// GET MEMORY BLOCK
static void *GetMem( size_t size )
{
    void        *block;                     // - new memory
    static bool FirstMemoryError = true;    // - indicates first "out of memory" error

    block = malloc( size );
    if( block == NULL ) {
        if( FirstMemoryError ) {
            Error( "Out of memory" );
            FirstMemoryError = false;
        }
    } else {
        memset( block, 0xFB, size );
    }
    return( block );
}

static SEGMSTK *PushSegStack( void )
// PUSH THE SEGMENT STACK
{
    SEGMSTK     *segm_stk;           // - new stack entry

    segm_stk = (SEGMSTK *)GetMem( sizeof( SEGMSTK ) );
    if( segm_stk != NULL ) {
        segm_stk->action = ProcessMode;
        segm_stk->next = SegmStk;
        segm_stk->rec_def = Files->rec_count;
        SegmStk = segm_stk;
    }
    return( segm_stk );
}

static void PopSegStack( void ) // POP SEGMENTS STACK
{
    SEGMSTK     *segm_stk;           // - top entry on stack

    if( SegmStk == NULL ) {
        Error( "Unexpected ENDSEGMENT statement" );
    } else {
        segm_stk = SegmStk;
        SegmStk = segm_stk->next;
        ProcessMode = segm_stk->action;
        free( segm_stk );
    }
}

static void AddDepTarg( const char *name )
{
    if( name != NULL && fp_depdump != NULL ) {
        fprintf( fp_depdump, "%s :", name );
    }
}

static void AddDepDep( const char *name )
{
    int     i;

    if( name != NULL && fp_depdump != NULL ) {
        for( i = 0; i < DepCount; i++ ) {
            if( strcmp( DepFileName[i], name ) == 0 ) {
                return;
            }
        }
        if( i + 1 < MAX_DEPS ) {
            DepFileName[i] = malloc( strlen( name ) + 1 );
            strcpy( DepFileName[i], name );
            DepCount = i + 1;
        }
    }
}

static void AddDepClose( void )
{
    int     i;
    char    *p;

    if( fp_depdump != NULL ) {
        for( i = 0; i < DepCount; i++ ) {
            p = DepFileName[i];
            fprintf( fp_depdump, " %s", p );
            free( p );
        }
        DepCount = 0;
        fprintf( fp_depdump, "\n" );
        fclose( fp_depdump );
        fp_depdump = NULL;
    }
}

static void OpenDepFile( const char *arg )
{
    char    fname[FILENAME_MAX];
    size_t  size;

    size = strlen( arg );
    if( size > 0 && arg[0] == '"' && arg[size - 1] == '"' ) {
        size -= 2;
        arg++;
    }
    memcpy( fname, arg, size );
    fname[size] = '\0';
    fp_depdump = fopen( fname, "at" );

}

//OPEN FILE, TRUNCATE NAME IF NECESSARY
static FILE *OpenFileTruncate(
    const char *file_name,    // - file to be opened
    const char *mode )        // - file mode
{
    FILE        *fp = NULL;

    fp = fopen( file_name, mode );
    if( fp != NULL )
        AddDepDep( file_name );
    if( fp == NULL ) {
        pgroup2 pg;
        char    new_name[FILENAME_MAX];
        bool    truncated;

        truncated = false;
        _splitpath2( file_name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( strlen( pg.fname ) > 8 ) {
            pg.fname[8] = '\0';
            truncated = true;
        }
        if( strlen( pg.ext ) > 4 ) {
            pg.ext[4] = '\0';
            truncated = true;
        }
        if( truncated ) {
            _makepath( new_name, pg.drive, pg.dir, pg.fname, pg.ext );
            fp = fopen( new_name, mode );
            if( fp != NULL ) {
                AddDepDep( new_name );
            }
        }
    }
    return( fp );
}

static FILE *OpenFilePathList(  //OPEN FILE, TRY EACH LOCATION IN PATH LIST
    const char *file_name,    // - file to be opened
    const char *mode )        // - file mode
{
    FILE        *fp = NULL;

    fp = OpenFileTruncate( file_name, mode );
    if( fp == NULL ) {
        char            buff[FILENAME_MAX];
        IPATHLST        *list;

        for( list = IncPathList; list != NULL; list = list->next ) {
            strcpy( buff, list->path );
            strcat( buff, file_name );
            fp = OpenFileTruncate( buff, mode );
            if( fp != NULL ) {
                break;
            }
        }
    }
    return( fp );
}

// OPEN FILE
static void OpenFileNormal(
    const char *file_name,    // - file to be opened
    const char *mode )        // - file mode
{
    FILE        *fp;            // - new file ptr.
    FILESTK     *file_stk;      // - new stack entry
    char        *p;
    char        c;

    file_stk = (FILESTK *)GetMem( sizeof( FILESTK ) + strlen( file_name ) );
    if( file_stk != NULL ) {
        p = file_stk->name;
        while( (c = *file_name++) != '\0' ) {
#ifndef __UNIX__
            if( c == '/' )
                c = '\\';
#endif
            *p++ = c;
        }
        *p = c;
        file_stk->rec_count = 0;
        fp = OpenFilePathList( file_stk->name, mode );
        if( fp == NULL ) {
            Error( "Can not open '%s'", file_stk->name );
            free( file_stk );
        } else {
            file_stk->next = Files;
            file_stk->fp = fp;
            Files = file_stk;
            Files->segm_stk = SegmStk;
            SegmStk = NULL;
        }
    }
}

// CLOSE CURRENT FILE
static void CloseFile( void )
{
    FILESTK     *file_stk;       // - file stack

    while( SegmStk != NULL ) {
        Error( "Unclosed segment from line %u", SegmStk->rec_def );
        PopSegStack();
    }
    SegmStk = Files->segm_stk;
    fclose( Files->fp );
    file_stk = Files;
    Files = file_stk->next;
    free( file_stk );
}

static void EatWhite( void )
{
    while( isspace( *(unsigned char *)Rptr ) ) {
        ++Rptr;
    }
}

static bool IsOper( char ch )
{
    switch( ch ) {
    case OP_OR:
    case OP_AND:
    case OP_NOT:
    case OP_LPAREN:
    case OP_RPAREN:
        return( true );
    default:
        return( false );
    }
}

// SCAN A STRING
static bool ScanString( void )
{
    char        *eptr;          // - end-of-string ptr.
    char        *cptr;          // - points into string
    const char  *rptr;          // - points into record

    EatWhite();
    rptr = Rptr;
    cptr = Token;
    if( IsOper( *rptr ) ) {
        cptr[0] = *rptr;
        cptr[1] = '\0';
        Rptr = rptr + 1;
        return( true );
    }
    eptr = cptr + sizeof( Token ) - 1;
    for( ;; ) {
        if( isspace( *(unsigned char *)rptr ) )
            break;
        if( IsOper( *rptr ) )
            break;
        if( *rptr == '\0' )
            break;
        if( *rptr == '\n' )
            break;
        if( *rptr == '\r' )
            break;
        if( cptr >= eptr ) {
            cptr = Token;
            break;
        }
        *cptr++ = *rptr++;
    }
    *cptr = '\0';
    Rptr = rptr;
    return( Token != cptr );
}

static bool GetToken( char op )
{
    if( Token[0] != op || Token[1] != '\0' )
        return( false );
    ScanString();
    return( true );
};

// INITIALIZE TO PROCESS RECORD
static KW RecordInitialize( const char *record )
{
    KW          i;
    const char  *token;

    Rptr = record;
    if( !ScanString() )
        return( KW_TEXT );
    if( Token[0] != KwChar )
        return( KW_TEXT );
    if( Token[1] == KwChar ) {
        if( record[0] != KwChar || record[1] != KwChar ) {
            // verify comment started in column one
            // (both checks are necessary (i.e., <TAB>::))
            return( KW_TEXT );
        }
        return( KW_COMMENT );
    }
    token = Token + 1;
    if( *token == '\0' ) {
        ScanString();
        token = Token;
    }
    for( i = 0; i < ARRAY_SIZE( KwTable ); ++i ) {
        if( 0 == stricmp( KwTable[i], token ) ) {
            return( i );
        }
    }
    return( KW_TEXT );
}

// READ A RECORD
static KW ReadInput( void )
{
    char    *p;
    KW      retn;           // - return: type of record

    if( NULL == fgets( Record, sizeof( Record ), Files->fp ) ) {
        retn = KW_EOF;
    } else {
        ++Files->rec_count;
        p = Record + strlen( Record ) - 1;
        if( *p == '\n' )    // turf \n on the end
            *p = '\0';
        retn = RecordInitialize( Record );
    }
    return( retn );
}

// LOOK UP A SEGMENT
static SEGMENT *SegmentLookUp( const char *seg_name )
{
    SEGMENT     *segment;       // - points to current segment
    size_t      size;           // - size of name

    for( segment = Segments; segment != NULL; segment = segment->next ) {
        if( 0 == stricmp( seg_name, segment->name ) ) {
            return( segment );
        }
    }

    size = strlen( seg_name );
    segment = ( SEGMENT *)GetMem( sizeof( SEGMENT ) + size );
    if( segment != NULL ) {
        memcpy( segment->name, seg_name, size + 1 );
        segment->seg_type = ' ';
        segment->next = Segments;
        Segments = segment;
    }
    return( segment );
}

static SEGMENT *ScanSegment( void )// SCAN A SEGMENT
{
    SEGMENT     *segment;           // - new segment

    if( ScanString() ) {
        segment = SegmentLookUp( Token );
    } else {
        Error( "Invalid segment name" );
        segment = NULL;
    }
    return( segment );
}

static bool PrimaryExpr( void )
{
    SEGMENT     *segment;           // - new segment
    bool        ret;

    if( GetToken( OP_LPAREN ) ) {
        ret = Expr();
        if( !GetToken( OP_RPAREN ) ) {
            Error( "Expecting ')'" );
        }
    } else {
        segment = SegmentLookUp( Token );
        ScanString();
        if( segment != NULL && segment->seg_type == SEG_KEEP ) {
            ret = true;
        } else {
            ret = false;
        }
    }
    return( ret );
}

static bool NotExpr( void )
{
    if( GetToken( OP_NOT ) ) {
        return( !PrimaryExpr() );
    } else {
        return( PrimaryExpr() );
    }
}

static bool AndExpr( void )
{
    bool ret;

    ret = NotExpr();
    while( GetToken( OP_AND ) ) {
        ret &= NotExpr();
    }
    return( ret );
}

static bool Expr( void )
{
    bool ret;

    ret = AndExpr();
    while( GetToken( OP_OR ) ) {
        ret |= AndExpr();
    }
    return( ret );
}

// See if a segment section should be output
static void SegmentCheck( void )
{
    ScanString();       // Get next token ready
    if( Expr() ) {
        ProcessMode = MODE_OUTPUT;
    } else {
        ProcessMode = MODE_DELETE;
    }
    EatWhite();
    if( *Rptr != '\0' ) {
        Error( "Expecting end of line" );
    }
}

static void OutputChar( char p )
{
    OutBuffer[OutBufferLen++] = p;
    if( OutBufferLen >= sizeof( OutBuffer ) ) {
        fwrite( OutBuffer, OutBufferLen, 1, OutputFile );
        OutBufferLen = 0;
    }
}

static void PutNL( void )
{
    if( EOL_crlf == 1 ) {
        OutputChar( '\r' );
#if !defined( __UNIX__ )
    } else if( EOL_crlf == -1 ) {
        OutputChar( '\r' );
#endif
    }
    OutputChar( '\n' );
}

static void OutputString( const char *p, const char *record )
{
    const char  *r;
    unsigned    col;
    unsigned    space;
    char        numstr[30];

    for( ; *p != '\0'; ++p ) {
        if( *p == '%' ) {
            ++p;
            switch( *p ) {
            case 's':
                col = 0;
                for( r = record; *r != '\0' && !( *r == '\r' && *( r + 1 ) == '\0' ); ++r ) {
                    if( *r != '\t' || TabStop == 0 ) {
                        OutputChar( *r );
                        ++col;
                    } else {
                        space = ( col + TabStop ) - ( ( col + TabStop ) % TabStop );
                        while( col < space ) {
                            OutputChar( ' ' );
                            ++col;
                        }
                    }
                }
                break;
            case 'n':
                PutNL();
                break;
            case '#':
                sprintf( numstr, "%u", OutNum );
                for( col = 0; col < strlen( numstr ); ++col ) {
                    OutputChar( numstr[col] );
                }
                break;
            case '+':
                ++OutNum;
                break;
            case '-':
                --OutNum;
                break;
            default:
                OutputChar( *p );
                break;
            }
        } else {
            OutputChar( *p );
        }
    }
}

// PROCESS A RECORD OF INPUT
static void ProcessRecord( KW kw, const char *record )
{
    SEGMENT *segment;       // - current segment

    switch( kw ) {
    case KW_SEGMENT:
        PushSegStack();
        switch( ProcessMode ) {
        case MODE_DELETE:
            ProcessMode = MODE_SKIPPING;
            break;
        case MODE_OUTPUT:
            SegmentCheck();
            break;
        }
        break;
    case KW_ELSESEGMENT:
        switch( ProcessMode ) {
        case MODE_DELETE:
            EatWhite();
            if( *Rptr == '\0' ) {
                ProcessMode = MODE_OUTPUT;
            } else {
                SegmentCheck();
            }
            break;
        case MODE_OUTPUT:
            ProcessMode = MODE_SKIPPING;
            break;
        }
        break;
    case KW_ENDSEGMENT:
        PopSegStack();
        break;
    case KW_KEEP:
        switch( ProcessMode ) {
        case MODE_OUTPUT:
            segment = ScanSegment();
            if( segment != NULL )
                segment->seg_type = SEG_KEEP;
            break;
        }
        break;
    case KW_REMOVE:
        switch( ProcessMode ) {
        case MODE_OUTPUT:
            segment = ScanSegment();
            if( segment != NULL )
                segment->seg_type = SEG_REMOVE;
            break;
        }
        break;
    case KW_INCLUDE:
        switch( ProcessMode ) {
        case MODE_OUTPUT:
            if( ScanString() ) {
                OpenFileNormal( Token, "r" );
            } else {
                Error( "Missing or invalid inclusion file" );
            }
            break;
        }
        break;
    case KW_TEXT:
        switch( ProcessMode ) {
        case MODE_OUTPUT:
            OutputString( OutFmt, record );
            PutNL();
            break;
        }
        break;
    case KW_COMMENT:
    case KW_EOF:
        break;
    }
}

// PROCESS SOURCE FILE
static void ProcessSource( const char *src_file ) // - starting file
{
    KW  kw;     // - current key-word

    ProcessMode = MODE_OUTPUT;
    SegmStk = NULL;
    SourceText = NULL;
    Files = NULL;
    OpenFileNormal( src_file, "r" );
    while( Files != NULL ) {
        kw = ReadInput();
        if( kw == KW_EOF ) {
            CloseFile();
        } else {
            ProcessRecord( kw, Record );
        }
    }
}

// ADD TO PATH LIST
static void AddIncludePathList( const char *path )
{
    IPATHLST    *lptr;          // - point to new path entry
    IPATHLST    *p;             // - point to list
    size_t      size;           // - size of path
#ifndef __UNIX__
    char        *ptr;
#endif

    size = strlen( path );
    if( size != 0 && path[0] == '"' && path[size - 1] == '"' ) {
        size -= 2;
        ++path;
    }
    lptr = GetMem( sizeof( IPATHLST ) + size + 1 );
    if( lptr == NULL ) {
        Error( "Unable to allocate %d bytes for path list entry: %s", size, path );
    } else {
        memcpy( lptr->path, path, size );
        lptr->path[size + 1] = 0;
#ifdef __UNIX__
        lptr->path[size] = '/';
#else
        lptr->path[size] = '\\';
        for( ptr = lptr->path; *ptr != '\0'; ++ptr ) {
            if( *ptr == '/' ) {
                *ptr = '\\';
            }
        }
#endif
        lptr->next = NULL;
        p = IncPathList;
        if( p == NULL ) {
            IncPathList = lptr;
        } else {
            while( p->next != NULL ) {
                p = p->next;
            }
            p->next = lptr;
        }
    }
}

int main(               // MAIN-LINE
    int argc,           // - # arguments
    char *argv[] )      // - arguments list
{
    int                 count;          // - current source file #
    char                *p;             // - generic pointer
    SEGMENT             *segment;       // - segment structure
    struct utimbuf      dest_time;
    struct stat         src_time;
    char                *src = NULL;
    char                *tgt = NULL;
    char                *param[32];
    int                 arg_count = 0;
    char                *arg;
    FILE                *fp;

    for( count = 1; count < argc; count++ ) {
        arg = argv[count];
        if( *arg != '@' ) {
            param[arg_count] = malloc( strlen( arg ) + 1 );
            strcpy( param[arg_count++], arg );
        } else {
            char        st[512], separator;
            int         i, j, k;
            size_t      len;
            const char  *env;

            env = getenv( arg + 1 );
            if( env != NULL ) {
                len = strlen( env );
                if( len > sizeof( st ) - 1 ) {
                    len = sizeof( st ) - 1;
                }
                memcpy( st, env, len + 1 );
                st[len] = '\0';
            } else {
                fp = fopen( arg + 1, "r" );
                if( fp == NULL ) {
                    Error( "Unable to open indirect argument file" );
                    continue;
                }

                if( fgets( st, sizeof( st ), fp ) == NULL )
                    Error( "Unable to read indirect argument file" );
                fclose( fp );
            }
            len = strlen( st );
            if( st[len - 1] == '\n' ) {
                --len;
                st[len] = 0;
            }

            i = 0;
            while( i < (int)len ) {
                while( st[i] == ' ' )
                    i++;
                if( st[i] == 0 )
                    break;
                if( st[i] == '"' ) {
                    separator = '"';
                    i++;
                } else {
                    separator = ' ';
                }
                j = i;
                while( ( st[j] != separator ) && ( st[j] != 0 ) ) {
                    if( ( separator == '"' ) && ( st[j] == '\\' ) )
                        j++;
                    j++;
                }

                param[arg_count] = malloc( j - i + 1 );
                for( k = 0; k < j - i; k++ ) {
                    if( ( separator == '"' ) && ( st[i + k] == '\\' ) )
                        i++;
                    param[arg_count][k] = st[i + k];
                }
                param[arg_count][k] = 0;
                arg_count++;

                i = j;
                if( st[i] == '"' ) {
                    i++;
                }
            }
        }
    }

    ErrCount = 0;
    if( arg_count < 2 ) {
        puts( "Usage: wsplice {src-file|option} tgt-file\n" );
        puts( "options are:" );
        puts( "    -i path\t\tcheck <path> for included files" );
        puts( "    -k seg_name\t\tSame as :KEEP" );
        puts( "    -r seg_name\t\tSame as :REMOVE" );
        puts( "    -f string\t\tSet output format to be <string>" );
        puts( "    -o string\t\tOutput <string> to tgt-file" );
        puts( "    -t tabstop\t\tSet tab character spacing" );
        puts( "    -u\t\t\tUse Unix style newlines for output file" );
        puts( "    -u0\t\t\tUse DOS style newlines for output file" );
        puts( "    -p\t\t\tpreserve same time stamp as src-file on tgt-file" );
    } else {
#define src_file arg                    // - name of source file
#define tgt_file param[arg_count - 1]   // - name of modifications file
#define get_value() ( (arg[2]=='\0') ? (param[++count]) : arg + 2)

        tgt = tgt_file;
        if( 0 == strcmp( tgt, "-" ) ) {
            OutputFile = stdout;
        } else {
            OutputFile = fopen( tgt, "wb" );
        }
        if( OutputFile == NULL ) {
            Error( "Unable to open output file" );
        } else {
            OutBufferLen = 0;
            for( count = 0; count < arg_count - 1; ++count ) {
                arg = param[count];
                if( arg[0] == '-' ) {
                    switch( arg[1] ) {
                    case 'i':
                        p = get_value();
                        AddIncludePathList( p );
                        break;
                    case 'k':
                        p = get_value();
                        segment = SegmentLookUp( p );
                        if( segment != NULL )
                            segment->seg_type = SEG_KEEP;
                        break;
                    case 'r':
                        p = get_value();
                        segment = SegmentLookUp( p );
                        if( segment != NULL )
                            segment->seg_type = SEG_REMOVE;
                        break;
                    case 'f':
                        OutFmt = get_value();
                        break;
                    case 'o':
                        p = get_value();
                        OutputString( p, "" );
                        break;
                    case 't':
                        p = get_value();
                        TabStop = strtoul( p, &p, 0 );
                        if( TabStop == 0 || *p != '\0' ) {
                            Error( "Illegal tab value" );
                            exit( 1 );
                        }
                        break;
                    case 'u':
                        if( arg[2] == '0' ) {
                            EOL_crlf = 1;
                        } else {
#if defined( __UNIX__ )
                            EOL_crlf = -1;
#else
                            EOL_crlf = 0;
#endif
                        }
                        break;
                    case 'p':
                        RestoreTime = true;
                        break;
                    case 'd':
                        OpenDepFile( get_value() );
                        AddDepTarg( tgt );
                        break;
                    default:
                        Error( "Unknown option '%c'", arg[1] );
                        break;
                    }
                } else {
                    src = src_file;
                    ProcessSource( src );
                }
            }
            if( OutBufferLen > 0 )
                fwrite( OutBuffer, OutBufferLen, 1, OutputFile );
            fclose( OutputFile );
        }
#undef src_file
#undef tgt_file
#undef get_value
    }
    AddDepClose();
    if( RestoreTime && strcmp( tgt, "-" ) != 0 ) {
        if( stat( src, &src_time ) == 0 ) {
            dest_time.actime = src_time.st_atime;
            dest_time.modtime = src_time.st_mtime;
            utime( tgt, &dest_time );
        }
    }
    for( count = 0; count < arg_count; ++count ) {
        free( param[count] );
    }

    return( ErrCount );
}

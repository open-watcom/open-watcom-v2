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


// WSPLICE.C -- mainline for splice program
//
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
//      - the section that is delimited by the first <expr> that is TRUE,
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
#include <sys/stat.h>
#ifdef __QNX__
#include <utime.h>
#else
#include <sys/utime.h>
#endif

//#define local static
#define local
#define TRUE  1
#define FALSE 0

typedef union textent TEXTENT;
typedef struct segment SEGMENT;
typedef struct filestk FILESTK;
typedef struct kw      KW;
typedef struct segstk  SEGSTK;
typedef struct ipathlst IPATHLST;
typedef unsigned PROCMODE;

struct text                     // define TEXT
{   TEXTENT *next;              // - next element
    char text_type;             // - type of entry
    #define TEXT_DATA 'D'       // - - data
    #define TEXT_CMD  'C'       // - - command
};

struct segcmd                   // define CMD
{   struct text base;           // - base element
    unsigned kw;                // - keyword
    SEGMENT *segment;           // - segment ptr
};

struct datatext                 // define DATA
{   struct text base;           // - base element
    char data[1];               // - text string
};

union textent                   // define TEXTENT
{   struct text     TEXT;       // - base
    struct segcmd   CMD;        // - command
    struct datatext DATA;       // - data
};

struct segment                  // define SEGMENT
{   SEGMENT *next;              // - next element
    char seg_type;              // - segment type
    #define SEG_REMOVE    'R'   // - - segment to be removed
    #define SEG_KEEP      'K'   // - - segment to be kept
    #define SEG_NONE      ' '   // - - keep and remove not specified
    char name[1];               // - segment name
};

struct segstk                   // define SEGSTK
{   SEGSTK *next;               // - next entry
    unsigned rec_def;           // - record number of definition
    PROCMODE action;            // - saved processing mode
};

struct filestk                  // define FILESTK
{   FILESTK *last;              // - last element
    FILE *file_ptr;             // - file ptr.
    SEGSTK *save_stk;           // - saved segment stack
    unsigned rec_count;         // - current record
    char name[1];               // - file name
};

struct kw                       // define KW
{   unsigned code;              // - code
    char *name;                 // - name
};

struct ipathlst                 // define ipathlst
{   struct ipathlst *next;      // - next pointer
    char path[1];               // - path
};

                                // LOCAL ROUTINES
local void ProcessSource();     // - process source file
local void ProcessTarget();     // - produce target file
local void SegmentCheck();      // - check if a seg section should be output
local SEGMENT *ScanSegment();   // - scan a new segment
local void AddText();           // - add text to a ring
local TEXTENT *AddTextEntry();  // - add text entry
local void CmdAdd();            // - execute or add a command
local void CmdExecute();        // - execute a command
local FILE *OpenFileTruncate(); // - open a file, truncate if necessary
local void OpenFile();          // - open a file
local void CloseFile();         // - close a file
local unsigned ReadInput();     // - read input record
local SEGMENT *SegmentLookUp(); // - look up a segment
local SEGSTK *PushSegStack();   // - push the segment stack
local void PopSegStack();       // - pop the segment stack
local void Error(char*,...);    // - write an error
local int  ScanString();        // - scan a string
local void *GetMem();           // - get a block of memory
local unsigned RecordInitialize(); // - initialize for record processing
local void OutputString();      // - send string to output file
local void PutNL();             // - output a newline
local void AddIncludePathList();// - add to list of include paths

                                // DATA (READ ONLY)

enum                            // define KW codes
{   KW_EOF,                     // - end of file
    KW_COMMENT,                 // - comment
    KW_TEXT,                    // - text
    KW_SEGMENT,                 // - SEGMENT
    KW_ELSESEGMENT,             // - ELSESEGMENT
    KW_ENDSEGMENT,              // - ENDSEGMENT
    KW_REMOVE,                  // - REMOVE
    KW_KEEP,                    // - KEEP
    KW_INCLUDE                  // - INCLUDE
};

local KW KwTable[] =            // - key words table
{   {   KW_SEGMENT,     "segment"       }, // SEGMENT expr
    {   KW_ELSESEGMENT, "elsesegment"   }, // ELSESEGMENT [expr]
    {   KW_ENDSEGMENT,  "endsegment"    }, // ENDSEGMENT
    {   KW_REMOVE,      "remove"        }, // REMOVE segment-name
    {   KW_KEEP,        "keep"          }, // KEEP segment-name
    {   KW_INCLUDE,     "include"       }, // INCLUDE file
    {   0,              NULL            }
};

enum {
    OP_OR,
    OP_AND,
    OP_NOT,
    OP_LPAREN,
    OP_RPAREN
};

local char *Oper[] =
{       "|",
        "&",
        "!",
        "(",
        ")",
};


                                // DATA (READ/WRITE)
local unsigned  ErrCount;        // - number of errors
local FILE      *OutputFile;      // - output file
local FILESTK   *Files;           // - stack of opened files
local SEGMENT   *Segments;        // - list of segments
local SEGSTK    *SegStk;          // - active-segments stack
local char      KwChar = { ':' };// - key word definition character
local TEXTENT   *SourceText;      // - source text
local char      Token[32];       // - scan token
local char      Record[256];     // - input record
local char      *Rptr;            // - ptr into record
local PROCMODE  ProcessMode;     // - processing mode
local char      *OutFmt = "%s"; // - output format
local unsigned  OutNum = 0;     // - output number
local int       UnixStyle;      // - Unix style newlines?
local int       TabStop;        // - tab spacing
local IPATHLST  *IncPathList;   // - list of include paths
local int       RestoreTime = FALSE;    // - set tgt-file timestamp to src-file

enum                            // PROCESSING MODES
{   MODE_DELETE,                // - deleting
    MODE_OUTPUT,                // - outputting
    MODE_SKIPPING               // - skipping to ENDSEGMENT
};


int main(                  // MAIN-LINE
    unsigned arg_count,     // - # arguments
    char *param[] )         // - arguments list
{
#define src_file param[ count ]         // - name of source file
#define tgt_file param[ arg_count-1 ]   // - name of modifications file
    int count;              // - current source file #
    char *p;                // - generic pointer
    SEGMENT *seg;           // - segment structure
    struct utimbuf      dest_time;
    struct stat         src_time;
    char                *src;
    char                *tgt;

    ErrCount = 0;
    if( arg_count < 3 ) {
        puts( "Usage: wsplice {src-file|option} tgt-file\n" );
        puts( "options are:" );
        puts( "    -i path\t\tcheck <path> for included files" );
        puts( "    -k seg_name\t\tSame as :KEEP" );
        puts( "    -r seg_name\t\tSame as :REMOVE" );
        puts( "    -f string\t\tSet output format to be <string>" );
        puts( "    -o string\t\tOutput <string> to tgt-file" );
        puts( "    -t tabstop\t\tSet tab character spacing" );
        puts( "    -u\t\t\tUse Unix style newlines for output file" );
        puts( "    -p\t\t\tpreserve same time stamp as src-file on tgt-file" );
    } else {
        if( 0 == stricmp( tgt_file, "-" ) ) {
            OutputFile = stdout;
        } else {
            OutputFile = fopen( tgt_file, "wb" );
            tgt = tgt_file;
        }
        if( OutputFile == NULL ) {
            Error( "Unable to open output file" );
        } else {
#define get_value() ((src_file[2]=='\0') ? (++count,src_file) : &src_file[2])
            for( count = 1; count < arg_count - 1; ++ count ) {
                if( src_file[0] == '-' ) {
                    switch( src_file[1] ) {
                    case 'i':
                        p = get_value();
                        AddIncludePathList( p );
                        break;
                    case 'k':
                        p = get_value();
                        seg = SegmentLookUp( p );
                        if( seg != NULL ) seg->seg_type = SEG_KEEP;
                        break;
                    case 'r':
                        p = get_value();
                        seg = SegmentLookUp( p );
                        if( seg != NULL ) seg->seg_type = SEG_REMOVE;
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
                        UnixStyle = TRUE;
                        break;
                    case 'p':
                        RestoreTime = TRUE;
                        break;
                    default:
                        Error( "Unknown option '%c'", src_file[1] );
                        break;
                    }
                } else {
                    src = src_file;
                    ProcessSource( src_file );
                }
            }
            fclose( OutputFile );
        }
    }
    if( RestoreTime ) {
        if( stat( src, &src_time ) == 0 ) {
            dest_time.actime = src_time.st_atime;
            dest_time.modtime = src_time.st_mtime;
            utime( tgt, &dest_time );
        }
    }

    return( ErrCount );

#undef src_file
#undef tgt_file
}


local void ProcessSource(               // PROCESS SOURCE FILE
    char *src_file )                    // - starting file
{
    int kw;                             // - current key-word

    ProcessMode = MODE_OUTPUT;
    SegStk = NULL;
    SourceText = NULL;
    Files = NULL;
    OpenFile( src_file, "r" );
    while( Files != NULL ) {
        kw = ReadInput();
        if( kw == KW_EOF ) {
            CloseFile();
        } else {
            ProcessRecord( kw, Record );
        }
    }
}


local void ProcessRecord(       // PROCESS A RECORD OF INPUT
    int kw,                     // - key-word for record
    char *record )              // - record
{
    SEGMENT *seg;               // - current segment

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
            seg = ScanSegment();
            if( seg != NULL ) seg->seg_type = SEG_KEEP;
            break;
        }
        break;
      case KW_REMOVE:
        switch( ProcessMode ) {
          case MODE_OUTPUT:
            seg = ScanSegment();
            if( seg != NULL ) seg->seg_type = SEG_REMOVE;
            break;
        }
        break;
      case KW_INCLUDE:
        switch( ProcessMode ) {
          case MODE_OUTPUT:
            if( ScanString() ) {
                OpenFile( Token, "r" );
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
    }
}


local void OutputString( char *p, char *record )
{
    char        *r;
    unsigned    col;
    unsigned    space;

    for( ; *p != '\0'; ++p ) {
        if( *p == '%' ) {
            ++p;
            switch( *p ) {
            case 's':
                col = 0;
                for( r = record; *r != '\0'; ++r ) {
                    if( *r != '\t' || TabStop == 0 ) {
                        fputc( *r, OutputFile );
                        ++col;
                    } else {
                        space = (col + TabStop) - ((col + TabStop) % TabStop);
                        while( col < space ) {
                           fputc( ' ', OutputFile );
                           ++col;
                        }
                    }
                }
                break;
            case 'n':
                PutNL();
                break;
            case '#':
                fprintf( OutputFile, "%u", OutNum );
                break;
            case '+':
                ++OutNum;
                break;
            case '-':
                --OutNum;
                break;
            default:
                fputc( *p, OutputFile );
                break;
            }
        } else {
            fputc( *p, OutputFile );
        }
    }
}

local void PutNL()
{
#if !defined( __QNX__ )
    if( !UnixStyle ) fputc( '\r', OutputFile );
#endif
    fputc( '\n', OutputFile );
}

local int GetToken( int op )
{
    if( stricmp( Token, Oper[op] ) == 0 ) {
        ScanString();
        return( TRUE );
    } else {
        return( FALSE );
    }
}

local int PrimaryExpr()
{
    SEGMENT *new;               // - new segment
    int ret;

    if( GetToken( OP_LPAREN ) ) {
        ret = Expr();
        if( !GetToken( OP_RPAREN ) ) {
            Error( "Expecting ')'" );
        }
    } else {
        new = SegmentLookUp( Token );
        ScanString();
        if( new != NULL && new->seg_type == SEG_KEEP ) {
            ret = TRUE;
        } else {
            ret = FALSE;
        }
    }
    return( ret );
}

local int NotExpr()
{
    if( GetToken( OP_NOT ) ) {
        return( !PrimaryExpr() );
    } else {
        return( PrimaryExpr() );
    }
}

local int AndExpr()
{
    int ret;

    ret = NotExpr();
    while( GetToken( OP_AND ) ) {
        ret &= NotExpr();
    }
    return( ret );
}

local int Expr()
{
    int ret;

    ret = AndExpr();
    while( GetToken( OP_OR ) ) {
        ret |= AndExpr();
    }
    return( ret );
}

local void SegmentCheck()       // See if a segment section should be output
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


local SEGSTK *PushSegStack() // PUSH THE SEGMENT STACK
{
    SEGSTK *stk;            // - new stack entry

    stk = (SEGSTK *)GetMem( sizeof( SEGSTK ) );
    if( stk != NULL ) {
        stk->action = ProcessMode;
        stk->next = SegStk;
        stk->rec_def = Files->rec_count;
        SegStk = stk;
    }
    return( stk );
}



local void PopSegStack()    // POP SEGMENTS STACK
{
    SEGSTK *top;            // - top entry on stack

    if( SegStk == NULL ) {
        Error( "Unexpected ENDSEGMENT statement" );
    } else {
        top = SegStk;
        SegStk = top->next;
        ProcessMode = top->action;
        free( top );
    }
}


local SEGMENT *ScanSegment()// SCAN A SEGMENT
{
    SEGMENT *new;           // - new segment

    if( ScanString() ) {
        new = SegmentLookUp( Token );
    } else {
        Error( "Invalid segment name" );
        new = NULL;
    }
    return( new );
}


local SEGMENT *SegmentLookUp(   // LOOK UP A SEGMENT
        char *seg_name )        // - name of segment to be found
{
    SEGMENT *sptr;              // - points to current segment
    unsigned size;              // - size of name

    sptr = Segments;
    for( ;; ) {
        if( sptr == NULL ) break;
        if( 0 == stricmp( seg_name, sptr->name ) ) return( sptr );
        sptr = sptr->next;
    }

    size = strlen( seg_name );
    sptr = (SEGMENT *)GetMem( sizeof( SEGMENT ) + size );
    if( sptr != NULL ) {
        memcpy( sptr->name, seg_name, size + 1 );
        sptr->seg_type = ' ';
        sptr->next = Segments;
        Segments = sptr;
    }
    return( sptr );
}


local void AddIncludePathList(  // ADD TO PATH LIST
    char *path )                // - path to add
{
    IPATHLST *lptr;             // - point to new path entry
    IPATHLST *p;                // - point to list
    unsigned size;              // - size of path

    size = strlen( path );
    lptr = GetMem( sizeof( IPATHLST ) + size + 1 );
    if( lptr == NULL ) {
        Error( "Unable to allocate %d bytes for path list entry: %s", size, path );
    } else {
        memcpy( lptr->path, path, size + 1 );
        lptr->path[size] = '\\';
        lptr->path[size+1] = 0;
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


local FILE *OpenFileTruncate( //OPEN FILE, TRUNCATE NAME IF NECESSARY
    char *file_name,        // - file to be opened
    char *mode )            // - file mode
{
    FILE *new = NULL;
    new = fopen( file_name, mode );
    if( new == NULL ) {
        char        buff[ _MAX_PATH2 ];
        char        *drive;
        char        *dir;
        char        *fname;
        char        *ext;
        _splitpath2( file_name, buff, &drive, &dir, &fname, &ext );
        if( fname != NULL && strlen( fname ) > 8 ) fname[8] = '\0';
        if( ext != NULL && strlen( ext ) > 3 ) ext[3] = '\0';
        // this is ok, because file_name can only get shorter
        _makepath( file_name, drive, dir, fname, ext );
        new = fopen( file_name, mode );
    }
    return( new );
}


local FILE *OpenFilePathList(//OPEN FILE, TRY EACH LOCATION IN PATH LIST
    char *file_name,        // - file to be opened
    char *mode )            // - file mode
{
    FILE *new = NULL;
    new = OpenFileTruncate( file_name, mode );
    if( new == NULL ) {
        char        buff[ _MAX_PATH2 ];
        IPATHLST    *list;
        list = IncPathList;
        while( list != NULL ) {
            strcpy( buff, list->path );
            strcat( buff, file_name );
            new = OpenFileTruncate( buff, mode );
            if( new != NULL ) break;
            list = list->next;
        }
    }
    return( new );
}


local void OpenFile(        // OPEN FILE
    char *file_name,        // - file to be opened
    char *mode )            // - file mode
{
    FILE *new;              // - new file ptr.
    FILESTK *stk;           // - new stack entry

    stk = (FILESTK *)GetMem( sizeof(FILESTK) + strlen(file_name) );
    if( stk != NULL ) {
        strcpy( stk->name, file_name );
        stk->rec_count = 0;
        new = OpenFilePathList( file_name, mode );
        if( new == NULL ) {
            Error( "Can not open '%s'", file_name );
            free( stk );
        } else {
            stk->last = Files;
            stk->file_ptr = new;
            Files = stk;
            Files->save_stk = SegStk;
            SegStk = NULL;
        }
    }
}


local void CloseFile()      // CLOSE CURRENT FILE
{
    FILESTK *stk;           // - file stack

    while( SegStk != NULL ) {
        Error( "Unclosed segment from line %u", SegStk->rec_def );
        PopSegStack();
    }
    SegStk = Files->save_stk;
    fclose( Files->file_ptr );
    stk = Files;
    Files = stk->last;
    free( stk );
}


local unsigned ReadInput()  // READ A RECORD
{
    unsigned retn;          // - return: type of record

    if( NULL == fgets( Record, sizeof(Record), Files->file_ptr ) ) {
        retn = KW_EOF;
    } else {
        ++Files->rec_count;
        Record[ strlen( Record ) - 1 ] = '\0'; // turf \n on the end
        retn = RecordInitialize( Record );
    }
    return( retn );
}


local unsigned RecordInitialize(        // INITIALIZE TO PROCESS RECORD
    char *record )                      // - record
{
    KW *pkw;                // - ptr. into KW table

    Rptr = record;
    if( !ScanString() ) return( KW_TEXT );
    if( Token[0] != KwChar ) return( KW_TEXT );
    if( Token[1] == KwChar ) {
        if( record[0] != KwChar || record[1] != KwChar ) {
            // verify comment started in column one
            // (both checks are necessary (i.e., <TAB>::))
            return( KW_TEXT );
        }
        return( KW_COMMENT );
    }
    pkw = KwTable;
    while( pkw->code != 0 ) {
        if( 0 == stricmp( pkw->name, &Token[1] ) ) return( pkw->code );
        ++pkw;
    }
    return( KW_TEXT );
}


local void EatWhite()
{
    while( isspace( *Rptr ) ) ++Rptr;
}


local int IsOper( char ch )
{
    int i;

    for( i = 0; i < sizeof( Oper ) / sizeof( Oper[0] ); ++i ) {
        if( Oper[i][0] == ch ) return( TRUE );
    }
    return( FALSE );
}

local int ScanString()      // SCAN A STRING
{
    char *eptr;             // - end-of-string ptr.
    char *cptr;             // - points into string
    char *rptr;             // - points into record

    EatWhite();
    rptr = Rptr;
    cptr = Token;
    if( IsOper( *rptr ) ) {
        cptr[0] = *rptr;
        cptr[1] = '\0';
        Rptr = rptr + 1;
        return( TRUE );
    }
    eptr = cptr + sizeof(Token) - 1;
    for( ;; ) {
        if( isspace( *rptr ) ) break;
        if( IsOper( *rptr ) ) break;
        if( *rptr == '\0' ) break;
        if( *rptr == '\n' ) break;
        if( *rptr == '\r' ) break;
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


local void Error(               // ERROR MESSAGE
    char *msg,                  // - message
    ... )                       // - extra info
{
    char emsg[128];             // - error message
    char *eptr;                 // - ptr. into error message
    va_list args;               // - var arg processing
    int count;                  // - chars written;

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


local void *GetMem(         // GET MEMORY BLOCK
    unsigned size )         // - size
{
    void *block;            // - new memory

static int FirstMemoryError  // - indicates first "out of memory" error
    = { TRUE };

    block = malloc( size );
    if( block == NULL ) {
        if( FirstMemoryError ) {
            Error( "Out of memory" );
            FirstMemoryError = FALSE;
        }
    } else {
        memset( block, 0xFB, size );
    }
    return( block );
}

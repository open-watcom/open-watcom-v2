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


#include "plusplus.h"

#include "memmgr.h"
#include "iosupp.h"
#include "preproc.h"
#include "cppexit.h"

enum {
    EL_NEW_LINE = 0x01,
    EL_NULL     = 0
};

static unsigned skipChars;              // how many initial chars to skip
static unsigned maxLineSize;            // how often to insert newline
static unsigned currLineSize;           // insert newline periodically
static int prevChar;                    // previous char written out

typedef struct id_mangle IDMANGLE;
struct id_mangle {
    IDMANGLE    *next;
    unsigned    index;
    char        id[1];
};
static IDMANGLE *idList;
static unsigned idIndex;


void PpStartFile(               // INDICATE START/CONTINUATION OF A FILE
    void )
{
    skipChars = 1;
}


unsigned PpVerifyWidth(         // VERIFY WIDTH FOR PREPROCESSING
    unsigned width )            // - new width
{
    if( width != 0 ) {
        if( width < 4 ) {
            width = 4;
        } else if( width > 8*1024 ) {
            width = 8*1024;
        }
    }
    return( width );
}


void PpSetWidth(                // SET WIDTH FOR PREPROCESSING
    unsigned width )            // - new width
{
    maxLineSize = PpVerifyWidth( width );
}

#if 0
static void emitPoundLine( LINE_NO line, char *fname, unsigned control )
{
    if( CompFlags.cpp_line_wanted ) {
        if( CppPrinting() ) {
            if( CompFlags.line_comments ) {
                fputc( '/', CppFile );
                fputc( '/', CppFile );
                fputc( ' ', CppFile );
            }
            fprintf( CppFile, "#line %u \"%s\"", line, fname );
            if( control & EL_NEW_LINE ) {
                fputc( '\n', CppFile );
            }
        }
    }
}
#else
static void emitPoundLine( LINE_NO line, char *fname, unsigned control )
{
    if( CompFlags.cpp_line_wanted ) {
        if( CppPrinting() ) {
            if( CompFlags.line_comments ) {
                if( control & EL_NEW_LINE ) {
                    // do zip: the correct position is immediately after comment
                } else {
                    -- line;
                }
                if( line == 0 ) {
                    line = 1;
                }
                fprintf( CppFile
                       , "\n/*@lineinfo:filename=%s*/"
                         "/*@lineinfo:user-code*/"
                         "/*@lineinfo:%u^1*/"
                       , fname
                       , line );
            } else {
                if( line == 0 ) {
                    line = 1;
                }
                fprintf( CppFile, "#line %u \"%s\"", line, fname );
                if( control & EL_NEW_LINE ) {
                    fputc( '\n', CppFile );
                }
            }
        }
    }
}
#endif

void EmitLine(                  // EMIT #LINE DIRECTIVE, IF REQ'D
    LINE_NO line_num,           // - line number
    char *filename )            // - file name
{
    emitPoundLine( line_num, filename, EL_NULL );
}

void EmitLineNL(                // EMIT #LINE DIRECTIVE ON ITS OWN LINE, IF REQ'D
    LINE_NO line_num,           // - line number
    char *filename )            // - file name
{
    emitPoundLine( line_num, filename, EL_NEW_LINE );
}

void PpInit(                    // INITIALIZE PREPROCESSING
    void )
{
    maxLineSize = 128;
    currLineSize = 0;
    idIndex = 0;
    idList = NULL;
    prevChar = '\n';
    PpStartFile();      // sets skipChars

    CppFile = NULL;
    PreProcChar = '#';
}


void PpOpen(                    // OPEN PREPROCESSOR OUTPUT
    void )
{
    char *name;

    if( CompFlags.cpp_output_to_file ) {                    /* 29-sep-90 */
        name = IoSuppOutFileName( OFT_PPO );
        CppFile = SrcFileFOpen( name, SFO_WRITE_TEXT );
        if( CppFile == NULL ) {
            printf( "Unable to open '%s'\n", name );
            CppExit( 1 );
        } else {
            IoSuppSetBuffering( CppFile, 512 );
        }
    } else {
        CppFile = stdout;
    }
}

static void prt_cpp_char(        // PRINT CPP CHARACTER
    int c )                     // - character to be printed
{
    if( c == '\n' ) {
        if( c != prevChar || CompFlags.cpp_line_wanted ) {
            fputc( c, CppFile );
        }
        currLineSize = 0;
    } else {
        fputc( c, CppFile );
        currLineSize++;
        if( maxLineSize != 0 ) {
            if( currLineSize >= maxLineSize ) {
                // output line splice
                fputc( '\\', CppFile );
                fputc( '\n', CppFile );
                currLineSize = 0;
            }
        }
    }
    prevChar = c;
}


void PrtString(                 // PRINT CPP STRING
    const char *str )           // - string to be printed
{
    while( *str ) {
        prt_cpp_char( *str );
        str++;
    }
}


void PrtChar(                   // PRINT PREPROC CHAR IF REQ'D
    int c )                     // - character to be printed
{
    if( CppPrinting() ) {
        if( skipChars ) {
            --skipChars;
            if( c != '\n' ) {
                prt_cpp_char( c );
            }
        } else {
            prt_cpp_char( c );
        }
    }
}

static void printMangledId( IDMANGLE *id )
{
    auto char buff[32];

    prt_cpp_char( '_' );
    ultoa( id->index, buff, 10 );
    PrtString( buff );
    prt_cpp_char( '_' );
}

static IDMANGLE *addNewId( IDMANGLE **head, IDMANGLE *next )
{
    IDMANGLE *new_id;
    size_t len;

    len = strlen( Buffer );
    new_id = CMemAlloc( offsetof( IDMANGLE, id ) + len + 1 );
    new_id->next = next;
    new_id->index = idIndex;
    strcpy( new_id->id, Buffer );
    *head = new_id;
    ++idIndex;
    return( new_id );
}

static void idPrint( void )
{
    IDMANGLE **head;
    IDMANGLE *id;
    int comp;

    if( ! CompFlags.encrypt_preproc_output ) {
        PrtString( Buffer );
        return;
    }
    head = &idList;
    for( id = *head; id != NULL; id = *head ) {
        comp = strcmp( Buffer, id->id );
        if( comp == 0 ) {
            printMangledId( id );
            return;
        }
        if( comp < 0 ) {
            printMangledId( addNewId( head, id ) );
            return;
        }
        head = &(id->next);
    }
    printMangledId( addNewId( head, NULL ) );
}

static void idMangleFini( void )
{
    IDMANGLE *id;
    IDMANGLE *next;

    for( id = idList; id != NULL; id = next ) {
        next = id->next;
        CMemFree( id );
    }
}

void PrtToken(                  // PRINT PREPROC TOKEN IF REQ'D
    void )
{
    if( CppPrinting() ) {
        switch( CurToken ) {
          case T_BAD_CHAR:                        /* 12-apr-89 */
          case T_BAD_TOKEN:                       /* 12-apr-89 */
          case T_CONSTANT:
            PrtString( Buffer );
            break;
          case T_ID:
            idPrint();
            break;
          case T_STRING:
            prt_cpp_char( '\"' );
            PrtString( Buffer );
            prt_cpp_char( '\"' );
            break;
          case T_LSTRING:
            prt_cpp_char( 'L' );
            prt_cpp_char( '\"' );
            PrtString( Buffer );
            prt_cpp_char( '\"' );
            break;
          case T_EOF:
          case T_NULL:
            break;
          default:
            PrtString( Tokens[ CurToken ] );
        }
    }
}


void PpParse(                   // PARSE WHEN PREPROCESSING
    void )
{
    for(;;) {
        GetNextToken();
        if( CurToken == T_EOF ) break;
        PrtToken();
    }
    idMangleFini();
}


const char *TokenString(              // RETURN A PRINTABLE STRING FOR CURRENT TOK
    void )
{
    const char *token;

    switch( CurToken ) {
    case T_BAD_CHAR:
        Buffer[1] = '\0';
    case T_ID:
    case T_STRING:
    case T_CONSTANT:
    case T_BAD_TOKEN:
        token = Buffer;
        break;
    default:
        token = Tokens[CurToken];
    }
    return( token );
}


void Expecting(                 // ISSUE EXPECTING ERROR FOR A TOKEN
    const char *a_token )       // - required token
{
    CErr( ERR_EXPECTING_BUT_FOUND, a_token, TokenString() );
}


bool ExpectingToken(            // ISSUE EXPECTING ERROR FOR A TOKEN
    TOKEN token )               // - required token
{
    TOKEN alt_token;

    /* also accept alternative tokens (digraphs) */
    switch( token ) {
    case T_LEFT_BRACKET:
        alt_token = T_ALT_LEFT_BRACKET;
        break;
    case T_RIGHT_BRACKET:
        alt_token = T_ALT_RIGHT_BRACKET;
        break;
    case T_LEFT_BRACE:
        alt_token = T_ALT_LEFT_BRACE;
        break;
    case T_RIGHT_BRACE:
        alt_token = T_ALT_RIGHT_BRACE;
        break;
    default:
        alt_token = token;
        break;
    }
    if( ( CurToken == token ) || ( CurToken == alt_token ) ) {
        return( TRUE );
    }
    CErr( ERR_EXPECTING_BUT_FOUND, Tokens[token], TokenString() );
    return( FALSE );
}

void MustRecog(                 // REQUIRE A SPECIFIC TOKEN AND SCAN NEXT
    TOKEN token )               // - token to be recognized
{
    ExpectingToken( token );
    if( CurToken != T_EOF ) {
        NextToken();
    }
}

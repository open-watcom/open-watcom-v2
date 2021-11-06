/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Get a character from source stream.
*
****************************************************************************/


#include "cvars.h"
#include <errno.h>
#include "wio.h"
#include "scan.h"


#define MAX_INC_DEPTH   255

/*
 * the following table is used to map three character sequences
 * beginning with ?? followed by 'tri' into the single character 'new'
 */
struct tri_graph {
    char        tri;
    char        new;
};

static struct tri_graph TriGraphs[] = {
    { '=', '#' },
    { '(', '[' },
    { '/', '\\'},
    { ')', ']' },
    { '\'','^' },
    { '<', '{' },
    { '!', '|' },
    { '>', '}' },
    { '-', '~' },
    { '\0','\0'}
};

static const unsigned char  notFilled[] = { '\n', '\0' };

static int      LastChar;
static int      Blank1Count;
static int      Blank2Count;
static int      Tab1Count;

static int      IncFileDepth;


void InitIncFile( void )
{
    IncFileDepth = MAX_INC_DEPTH;
}

static bool ReadBuffer( FCB *srcfcb )
{
    int         last_char;
    size_t      read_amount;

    if( srcfcb->src_fp == NULL ) {          /* in-memory buffer */
        CloseSrcFile( srcfcb );
        return( false );
    }
    if( srcfcb->src_end == notFilled + 1 ) {
        if( srcfcb->typ == FT_HEADER_FORCED ) {
            InitialMacroFlags = MFLAG_NONE;
            if( PCH_FileName != NULL && CompFlags.make_precompiled_header == 0 ) {
                if( CompFlags.ok_to_use_precompiled_hdr ) {
                    CompFlags.use_precompiled_header = true;
                }
            }
            if( CompFlags.use_precompiled_header ) {
                InitBuildPreCompiledHeader();
            }
            if( CompFlags.use_precompiled_header ) {
                CompFlags.use_precompiled_header = false;
                if( UsePreCompiledHeader( srcfcb->src_name ) ) {
                    return( true );
                }
            }
        }
    }

    /*
     * ANSI/ISO C says a non-empty source file must be terminated
     * with a newline. If it's not, we insert one, otherwise
     * whatever comes next will be tacked onto that unterminated
     * line, possibly confusing the hell out of the user.
     */
    if( srcfcb->src_buf != srcfcb->src_end ) {
        last_char = *( srcfcb->src_end - 1 );
    } else {
        last_char = '\n';
    }
    srcfcb->src_end = srcfcb->src_ptr = srcfcb->src_buf;
    read_amount = fread( srcfcb->src_buf, 1, SRC_BUF_SIZE, srcfcb->src_fp );
    if( read_amount != SRC_BUF_SIZE ) {
        if( ferror( srcfcb->src_fp ) ) {
            CErr3p( ERR_IO_ERR, srcfcb->src_name, strerror( errno ) );
            CloseSrcFile( srcfcb );
            return( true );
        }
        if( read_amount == 0 && last_char == '\n' ) {
            CloseSrcFile( srcfcb );
            return( true );
        }
    }
    if( read_amount > 0 ) {
        last_char = srcfcb->src_buf[read_amount - 1];
    }
    if( ( read_amount < SRC_BUF_SIZE ) && ( last_char != '\n' ) ) {
        srcfcb->no_eol = true;                  // emit warning later so line # is right
    }
    srcfcb->src_buf[read_amount] = '\0';
    srcfcb->src_end += read_amount;             // mark end of buffer
    return( false );                            // indicate CurrChar does not contain a character
}


int GetNextChar( void )
{
    int c;

    c = *SrcFile->src_ptr++;
    if(( CharSet[c] & C_EX ) == 0 ) {
//      SrcFile->column++;
        CurrChar = c;
        return( c );
    }
    return( GetCharCheck( c ) );
}

static int getCharAfterOneQuestion( void )
{
    int c;

    // column for 'LastChar' has not been set yet
    NextChar = GetNextChar;
    c = LastChar;
    if( c == '?' ) {
//      SrcFile->column++;
        CurrChar = c;
        return( c );
    }
    return( GetCharCheck( c ) );
}

static int getCharAfterTwoQuestion( void )
{
//  SrcFile->column++;
    CurrChar = '?';
    NextChar = getCharAfterOneQuestion;
    return( CurrChar );
}

int getCharAfterBackSlash( void )
{
    if( Blank1Count != 0 ) {
        --Blank1Count;
        return( CurrChar = ' ' );
    }
    if( Tab1Count != 0 ) {
        --Tab1Count;
        return( CurrChar = '\t' );
    }
    if( Blank2Count != 0 ) {
        --Blank2Count;
        return( CurrChar = ' ' );
    }
    NextChar = GetNextChar;
    return( GetCharCheck( LastChar ) );
}

static int getTestCharFromFile( void )
{
    int c;

    for( ; (c = *SrcFile->src_ptr++) == '\0'; ) {
        /*
         * check to make sure the NUL character we just found is at the
         * end of the buffer, and not an embedded NUL character in the
         * source file.
         */
        if( SrcFile->src_ptr != SrcFile->src_end + 1 )
            break;
        if( ReadBuffer( SrcFile ) ) {
            return( CurrChar );
        }
    }
    return( c );
}

static int getSecondMultiByte( void )
{
    int c;

    c = getTestCharFromFile();
    // should we do this for a multi-byte char?
    NextChar = GetNextChar;
    CurrChar = c;
    return( c );
}

static char translateTriGraph( char c )
{
    struct tri_graph *p;

    for( p = TriGraphs; p->tri != '\0'; ++p ) {
        if( c == p->tri ) {
            if( CompFlags.extensions_enabled ) {
                if( SkipLevel == NestLevel ) {
                    CompFlags.trigraph_alert = true;
                }
            }
            return( p->new );
        }
    }
    return( c );
}

static int tryBackSlashNewLine( void )
{
    int nc;

    // CurrChar is '\\' and SrcFile->column is up to date
    Blank1Count = 0;
    Blank2Count = 0;
    Tab1Count = 0;
    nc = getTestCharFromFile();
    if( CompFlags.extensions_enabled ) {
        while( nc == ' ' ) {
            ++Blank1Count;
            nc = getTestCharFromFile();
        }
        while( nc == '\t' ) {
            ++Tab1Count;
            nc = getTestCharFromFile();
        }
        while( nc == ' ' ) {
            ++Blank2Count;
            nc = getTestCharFromFile();
        }
    }
    if( nc == '\r' ) {
        nc = getTestCharFromFile();
    }
    if( nc == '\n' ) {
        if( CompFlags.scanning_cpp_comment && SkipLevel == NestLevel ) {
            CWarn1( WARN_SPLICE_IN_CPP_COMMENT, ERR_SPLICE_IN_CPP_COMMENT );
        }
        if( CompFlags.cpp_mode ) {
            if( CompFlags.in_pragma ) {
                CppPrtChar( '\\' );
                CppPrtChar( '\n' );
            } else if( CompFlags.cpp_line_wanted ) {
                CppPrtChar( '\n' );
            }
        }
        NewLineStartPos( SrcFile );
        SrcFileLoc = SrcFile->src_loc;
//      SrcFile->column = 0;
        return( GetNextChar() );
    }
    LastChar = nc;
    NextChar = getCharAfterBackSlash;
    return( '\\' );
}

static int tryTrigraphAgain( void )
{
    int c;
    int xc;

//  SrcFile->column++;
    c = getTestCharFromFile();
    if( c != '?' ) {
        xc = translateTriGraph( c );
        if( c != xc ) {
//          SrcFile->column += 2;
            CurrChar = xc;
            NextChar = GetNextChar;
            if( xc == '\\' ) {
                return( tryBackSlashNewLine() );
            }
            return( xc );
        }
        LastChar = c;
        CurrChar = '?';
        NextChar = getCharAfterTwoQuestion;
        return( '?' );
    }
    CurrChar = c;
    /* leave NextChar set here because it could still be a trigraph */
    return( c );
}

int GetCharCheckFile( int c )
{
    int xc;
    int nc;
    int nnc;

    // column has not being changed for 'c' yet
    if( c != '?' ) {
        switch( c ) {
        case '\0':
            /*
             * check to make sure the NUL character we just found is at the
             * end of the buffer, and not an embedded NUL character in the
             * source file.
             */
            CurrChar = '\0';
            if( SrcFile->no_eol ) {
                CurrChar = '\n';
            }
            if( SrcFile->src_ptr == SrcFile->src_end + 1 ) {
                if( !ReadBuffer( SrcFile ) ) {
                    return( GetNextChar() );
                }
            }
            return( CurrChar );
        case '\n':
//            NewLineStartPos( SrcFile );
//          SrcFile->column = 0;
            break;
        case '\t':
//          SrcFile->column +=
//              ( ( TAB_WIDTH + 1 ) - (( SrcFile->column + 1 ) % TAB_WIDTH ) );
            break;
        case '\\':
//          SrcFile->column++;
            CurrChar = c;
            return( tryBackSlashNewLine() );
        case '\r':
            break;
        default:
//          SrcFile->column++;
            if( c > 0x7f && (CharSet[c] & C_DB) ) {
                /*
                 * we should not process the second byte through
                 * normal channels since its value is meaningless
                 * out of context
                 */
                NextChar = getSecondMultiByte;
            }
            break;
        }
        CurrChar = c;
        return( c );
    }
    /* we have one '?' */
//    SrcFile->column++;
    nc = getTestCharFromFile();
    if( nc != '?' ) {
        LastChar = nc;
        CurrChar = c;
        NextChar = getCharAfterOneQuestion;
        return( c );
    }
    /* we have two '?'s */
    nnc = getTestCharFromFile();
    xc = translateTriGraph( nnc );
    if( nnc != xc ) {
//      SrcFile->column += 2;
        CurrChar = xc;
        if( xc == '\\' ) {
            return( tryBackSlashNewLine() );
        }
        return( xc );
    }
    LastChar = nnc;
    CurrChar = c;
    if( nnc == '?' ) {
        /* the next char after this may be a valid trigraph! */
        NextChar = tryTrigraphAgain;
        return( c );
    }
    NextChar = getCharAfterTwoQuestion;
    return( c );
}

static int regetUngotCharAfterOneQuestion( void )
{
    int c;

    // column for 'LastChar' has been set
    NextChar = GetNextChar;
    c = LastChar;
    CurrChar = c;
    return( c );
}

static int regetUngotCharAfterTwoQuestion( void )
{
    int c;

    NextChar = regetUngotCharAfterOneQuestion;
    c = '?';
    CurrChar = c;
    return( c );
}

static int restartDetectOneQuestion( void )
{
    int c;

    NextChar = regetUngotCharAfterOneQuestion;
    c = '?';
    CurrChar = c;
    return( c );
}

static int restartDetectTwoQuestion( void )
{
    int c;

    NextChar = regetUngotCharAfterTwoQuestion;
    c = '?';
    CurrChar = c;
    return( c );
}

static int restartBackSlashWhiteSpace( void )
{
    int c;

    NextChar = getCharAfterBackSlash;
    c = '\\';
    CurrChar = c;
    return( c );
}

void GetNextCharUndo( int c )
{
    if( NextChar == GetNextChar ) {
        // will return 'LastChar' and reset back to 'GetNextChar'
        NextChar = regetUngotCharAfterOneQuestion;
        LastChar = c;
        return;
    }
    if( NextChar == getCharAfterOneQuestion ) {
        // LastChar already has character saved in it
        // c should be '?'
        NextChar = restartDetectOneQuestion;
        return;
    }
    if( NextChar == getCharAfterTwoQuestion ) {
        // LastChar already has character saved in it
        // c should be '?'
        NextChar = restartDetectTwoQuestion;
        return;
    }
    if( NextChar == getCharAfterBackSlash ) {
        // LastChar already has character saved in it
        // c should be '\\'
        NextChar = restartBackSlashWhiteSpace;
        return;
    }
#ifdef FDEBUG
    CFatal( "more than one GetNextCharUndo executed" );
#endif
}

static bool FCB_Alloc( FILE *fp, const char *filename, src_file_type typ )
{
    FCB             *srcfcb;
    unsigned char   *src_buffer;
    FNAMEPTR        flist;

    --IncFileDepth;
    srcfcb = (FCB *)CMemAlloc( sizeof( FCB ) );
    src_buffer = FEmalloc( SRC_BUF_SIZE + 3 );
    if( srcfcb != NULL ) {
        srcfcb->src_buf = src_buffer;
        src_buffer[0] = '\0';
        flist = AddFlist( filename );
        FNameFullPath( flist );
        srcfcb->src_name = flist->name;
        srcfcb->src_line_cnt = 0;
        srcfcb->src_loc.line = 0;
        srcfcb->src_loc.column = 0;
        srcfcb->src_loc.fno = flist->index;
        SrcFileLoc = srcfcb->src_loc;
        srcfcb->src_flist = flist;
        srcfcb->src_fp = fp;
        srcfcb->src_ptr = notFilled + 1;
        srcfcb->src_end = notFilled + 1;
        srcfcb->prev_file = SrcFile;
        srcfcb->prev_currchar = CurrChar;
#if _CPU == 370
        srcfcb->column = 0;     /* init colum, trunc info */
        srcfcb->trunc = 0;
        srcfcb->prevcount = 0;
#endif
        if( SrcFile != NULL ) {
            if( SrcFile == MainSrcFile ) {
                // remember name of included file
                AddIncFileList( filename );
            }
        }
        srcfcb->rseekpos = 0;
        srcfcb->typ = typ;
        srcfcb->no_eol = false;
        SrcFile = srcfcb;
        CurrChar = '\n';    /* set next character to newline */
        return( true );
    }
    return( false );
}

static void FCB_Free( FCB *srcfcb )
{
    ++IncFileDepth;
    FEfree( srcfcb->src_buf );
    CMemFree( srcfcb );
}

bool OpenFCB( FILE *fp, const char *filename, src_file_type typ )
{
    if( IncFileDepth == 0 ) {
        CErr2( ERR_INCDEPTH, MAX_INC_DEPTH );
        CSuicide();
        return( false );
    }
    if( CompFlags.track_includes ) {
        // Don't track the top level file (any semi-intelligent user should
        // have no trouble tracking *that* down)
        if( IncFileDepth < MAX_INC_DEPTH ) {
            CInfoMsg( INFO_INCLUDING_FILE, filename );
        }
    }

    if( FCB_Alloc( fp, filename, typ ) ) {
        return( true );
    }
    CErr1( ERR_OUT_OF_MEMORY );
    return( false );
}

void CloseFCB( FCB *srcfcb )
{
    if( CompFlags.scanning_comment ) {
        CErr2( ERR_INCOMPLETE_COMMENT, CommentLoc.line );
    }
    if( srcfcb->no_eol ) {
        source_loc  err_loc;

        err_loc.line = srcfcb->src_line_cnt;
        err_loc.column = srcfcb->src_loc.column;
        err_loc.fno = srcfcb->src_flist->index;
        SetErrLoc( &err_loc );
        CWarn1( WARN_NO_EOL_BEFORE_EOF, ERR_NO_EOL_BEFORE_EOF );
        InitErrLoc();
    }
    SrcFile = srcfcb->prev_file;
    CurrChar = srcfcb->prev_currchar;
    if( SrcFile != NULL ) {
        if( SrcFile->src_fp == NULL ) {
            // physical file name must be used, not logical
            SrcFile->src_fp = fopen( SrcFile->src_flist->name, "rb" );
            fseek( SrcFile->src_fp, SrcFile->rseekpos, SEEK_SET );
        }
        SrcFileLoc = SrcFile->src_loc;
        IncLineCount += srcfcb->src_line_cnt;
        if( SrcFile == MainSrcFile ) {
            if( CompFlags.make_precompiled_header ) {
                CompFlags.make_precompiled_header = false;
                if( ErrCount == 0 ) {
                    BuildPreCompiledHeader( PCH_FileName );
                }
            }
        }
        if( CompFlags.cpp_mode ) {
            CppEmitPoundLine( SrcFile->src_loc.line, SrcFile->src_name, true );
        }
    } else {
        SrcLineCount = srcfcb->src_line_cnt;
        CurrChar = LCHR_EOF;
    }
    FCB_Free( srcfcb );
}

void SrcPurge( void )
/*******************/
{
    FCB *src_file;

    while( (src_file = SrcFile) != NULL ) {
        SrcFile = src_file->prev_file;
        CClose( src_file->src_fp );
        FCB_Free( src_file );
    }
}

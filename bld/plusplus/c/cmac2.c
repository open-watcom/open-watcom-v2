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


#include <string.h>
#include <stdlib.h>

#include "plusplus.h"
#include "preproc.h"
#include "errdefns.h"
#include "scan.h"
#include "memmgr.h"
#include "fname.h"
#include "ring.h"
#include "vstk.h"
#include "cmdline.h"
#include "cgmisc.h"
#include "name.h"
#include "cmacsupp.h"
#include "brinfo.h"


typedef struct mac_parm MAC_PARM;
struct mac_parm {
    MAC_PARM    *next;
    char        name[1];
};

struct cpp_info {                   // Pre-processor information
    TOKEN_LOCN  locn;
    char        cpp_type;
    char        processing;
};

static struct cpp_info *pp_stack;   // top of pre-processor stack
static VSTK_CTL vstkPp;             // pre-processor stack

static void CSkip( void );
static void CSkipIf( void );
static void CDefine( void );
static void CInclude( void );
static void CIfDef( void );
static void CIfNDef( void );
static void CIf( void );
static void CElif( void );
static void CElse( void );
static void CEndif( void );
static void CUnDef( void );
static void CLine( void );
static void CError( void );
extern void CPragma( void );
static void CIdent( void );
static void CUnknown( void );

typedef struct {
    char        *directive;
    void        (*samelevel)( void );
    void        (*skiplevel)( void );
} PPCTRL;

static unsigned char preprocWeights[] = {
//a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y,z
  2, 0, 0,11, 1, 4, 0, 0, 5, 0, 0,12, 0, 0, 0,13, 0,14, 0, 9,15, 0, 0, 0, 0,0
};
static PPCTRL controlTable[] = {
    { "",       NULL,           NULL },  // 0
    { "line",   CLine,          CSkip }, // 4 +12 + 1 = 17 mod 16 = 1
    { "define", CDefine,        CSkip }, // 6 +11 + 1 = 18 mod 16 = 2
    { "ident",  CIdent,         CSkip }, // 5 + 5 + 9 = 19 mod 16 = 3
    { "error",  CError,         CSkip }, // 5 + 1 +14 = 20 mod 16 = 4
    { "pragma", CPragma,        CSkip }, // 6 +13 + 2 = 21 mod 16 = 5
    { "else",   CElse,          CElse }, // 4 + 1 + 1 = 6
    { "",       NULL,           NULL },  // 7
    { "undef",  CUnDef,         CSkip }, // 5 +15 + 4 = 24 mod 16 = 8
    { "elif",   CElif,          CElif }, // 4 + 1 + 4 = 9
    { "endif",  CEndif,         CEndif },// 5 + 1 + 4 = 10
    { "if",     CIf,            CSkipIf },//2 + 5 + 4 = 11
    { "",       NULL,           NULL },  // 12
    { "include",CInclude,       CSkip }, // 7 + 5 + 1 = 13
    { "ifdef",  CIfDef,         CSkipIf },//5 + 5 + 4 = 14
    { "ifndef", CIfNDef,        CSkipIf },//6 + 5 + 4 = 15
};

enum pp_stack_states {
    PRE_NULL,
    PRE_ELSE,
    PRE_ELIF,
    PRE_IF,
};



static void flush2EOL( void )
{
    while( CurToken != T_NULL ) {
        if( CurToken == T_EOF ) break;
        NextToken();
    }
}

static void CSkip( void )
{
}

static void CSkipIf( void )
{
    IncLevel( 0 );
}

static void CUnknown( void )
{
    if( NestLevel == SkipLevel ) {
        CErr2p( ERR_UNKNOWN_DIRECTIVE, Buffer );
    }
}

static void CInclude( void )
{
    struct {
        unsigned in_macro : 1;
    } flags;
    auto char buf[_MAX_PATH];

    SrcFileGuardStateSig();
    InitialMacroFlag = 0;
    flags.in_macro = 0;
    if( CompFlags.use_macro_tokens ) {
        flags.in_macro = 1;
    }
    PPState = PPS_EOL | PPS_NO_LEX_ERRORS;
    NextToken();
    if( CurToken == T_STRING ) {
        OpenSrcFile( Buffer, FALSE );
    } else if( CurToken == T_LT ) {
        if( ! flags.in_macro ) {
            PPState = PPS_EOL | PPS_NO_LEX_ERRORS | PPS_NO_EXPAND;
        }
        buf[0] = '\0';
        for(;;) {
            NextToken();
            if( CurToken == T_GT ) {
                OpenSrcFile( buf, TRUE );
                break;
            }
            strncat( buf, Buffer, sizeof( buf ) - 2 );
            if( ( flags.in_macro != 0  && ! CompFlags.use_macro_tokens )
                ||  CurToken == T_NULL
                ||  CurToken == T_EOF ) {
                CErr1( ERR_INVALID_INCLUDE );
                break;
            }
        }
    } else {
        CErr1( ERR_INVALID_INCLUDE );
    }
    if( CurToken != T_EOF )  NextToken();
}

static void CDefine( void )
{
    MEPTR define_macro;

    define_macro = MacroScan( MSCAN_DEFINE );
    if( define_macro != NULL ) {
        define_macro->macro_flags |= MACRO_USER_DEFINED;
    }
}

static unsigned addParmName( MAC_PARM **h, boolean add_name )
{
    unsigned index;
    size_t len;
    size_t len_1;
    MAC_PARM *parm_name;

    len = strlen( Buffer );
    len_1 = len + 1;
    index = 0;
    RingIterBeg( *h, parm_name ) {
        ++index;
        if( NameMemCmp( parm_name->name, Buffer, len_1 ) == 0 ) {
            /* already present */
            return( index );
        }
    } RingIterEnd( parm_name );
    if( add_name ) {
        parm_name = CMemAlloc( offsetof( MAC_PARM, name ) + len + 1 );
        memcpy( parm_name->name, Buffer, len_1 );
        RingAppend( h, parm_name );
    }
    return( 0 );
}

static unsigned findParmName( MAC_PARM **h )
{
    if( *h != NULL ) {
        return( addParmName( h, FALSE ) );
    }
    return( 0 );
}

static boolean skipEqualOK( void )
{
    switch( CurToken ) {
    case T_EQ :
        CurToken = T_EQUAL;     // strip = from ==
        break;
    case T_EQUAL :
        NextToken();
        break;
    case T_SHARP :
        NextToken();
        break;
    case T_SHARP_SHARP :
        CurToken = T_SHARP;     // strip # from ##
        break;
    case T_NULL :
        break;
    default :
        BadCmdLine( ERR_INVALID_OPTION );
        return( FALSE );
    }
    return( TRUE );
}


static MEPTR grabTokens(    // SAVE TOKENS IN A MACRO DEFINITION
    MAC_PARM **parm_names,  // - macro parm names
    int parm_cnt,           // - parameter count
    macro_scanning defn,    // - scanning definition
    unsigned name_len,      // - length of macro name
    unsigned mlen,          // - length of macro def'n (so far)
    TOKEN_LOCN* locn )      // - definition point
{
    MEPTR mptr;
    MEPTR mentry;
    unsigned parm_index;
    unsigned prev_token;
    unsigned prev_non_ws_token;

    // MacroOverflow was called for the name of the macro + mentry already
    mentry = (MEPTR) MacroOffset;
    DbgAssert( ( MacroOverflow( mlen, 0 ), MacroOffset == (void*) mentry ) );
    mentry->parm_count = parm_cnt;
    mentry->macro_defn = mlen;
    prev_token = T_NULL;
    prev_non_ws_token = T_NULL;
    if( CurToken != T_NULL ) {
        do {
            CurToken = ScanToken( 0 );
        } while( CurToken == T_WHITE_SPACE );
        if( defn & MSCAN_EQUALS ) {
            if( CurToken == T_NULL ) {
                // -dFOO from a command line
                MacroOffsetAddChar( &mlen, 1, T_PPNUMBER );
                MacroOffsetAddMem( &mlen, "1", 2 );
                prev_non_ws_token = T_PPNUMBER;
                prev_token = T_PPNUMBER;
            } else {
                if( ! skipEqualOK() ) {
                    return( NULL );
                }
            }
        }
        if( CurToken == T_SHARP_SHARP ) {
            CErr1( ERR_MISPLACED_SHARP_SHARP );
            NextToken();
        }
    }
    for(;;) {
        if( CurToken == T_NULL ) break;
        switch( CurToken ) {
          case T_SHARP:
            /* if it is a function-like macro definition */
            if( parm_cnt != 0 ) {
                CurToken = T_MACRO_SHARP;
            }
            MacroOffsetAddChar( &mlen, 1, CurToken );
            break;
          case T_SHARP_SHARP:
            CurToken = T_MACRO_SHARP_SHARP;
            MacroOffsetAddChar( &mlen, 1, CurToken );
            break;
          case T_WHITE_SPACE:
            if( prev_token != T_WHITE_SPACE ) {
                MacroOffsetAddChar( &mlen, 1, CurToken );
            }
            break;
          case T_ID:
            parm_index = findParmName( parm_names );
            if( parm_index != 0 ) {
                CurToken = T_MACRO_PARM;
                MacroOffsetAddChar( &mlen, 2, CurToken, parm_index - 1 );
            } else {
                MacroOffsetAddChar( &mlen, 1, CurToken );
                MacroOffsetAddMem( &mlen, Buffer, TokenLen + 1 );
            }
            break;
          case T_BAD_CHAR:
            if( Buffer[1] != '\0' ) {
                MacroOffsetAddChar( &mlen, 3, CurToken, Buffer[0], T_WHITE_SPACE );
            } else {
                MacroOffsetAddChar( &mlen, 2, CurToken, Buffer[0] );
            }
            break;
          case T_BAD_TOKEN :
          case T_CONSTANT:
          case T_PPNUMBER:
            MacroOffsetAddChar( &mlen, 1, CurToken );
            MacroOffsetAddMem( &mlen, Buffer, TokenLen + 1 );
            break;
          case T_STRING:
          case T_LSTRING:
            // TokenLen includes '\0' for strings
            MacroOffsetAddChar( &mlen, 1, CurToken );
            MacroOffsetAddMem( &mlen, Buffer, TokenLen );
            break;
          default :
            MacroOffsetAddChar( &mlen, 1, CurToken );
            break;
        }
        if( CurToken != T_WHITE_SPACE ) {
            if( prev_non_ws_token == T_MACRO_SHARP && CurToken != T_MACRO_PARM ) {
                CErr1( ERR_MUST_BE_MACRO_PARM );
            }
            prev_non_ws_token = CurToken;
        }
        prev_token = CurToken;
        CurToken = ScanToken( 0 );
        if( ! ( defn & MSCAN_MANY ) ) break;
    }
    if( prev_token == T_WHITE_SPACE ) {
        --mlen;
    }
    MacroOffsetAddChar( &mlen, 1, T_NULL );
    if( prev_non_ws_token == T_MACRO_SHARP_SHARP ) {
        CErr1( ERR_MISPLACED_SHARP_SHARP );
    }
    mentry = (MEPTR) MacroOffset;       // MacroOffset could have changed
    TokenLocnAssign( mentry->defn, *locn );
    mentry->macro_len = mlen;
    mptr = MacroDefine( mentry, mlen, name_len );
    BrinfDeclMacro( mptr );
    if( ! ( defn & MSCAN_MANY ) ) {
        while( CurToken == T_WHITE_SPACE ) {
            CurToken = ScanToken( 0 );
        }
        if( CurToken != T_NULL ) {
            BadCmdLine( ERR_INVALID_OPTION );
        }
    }
    return( mptr );
}


MEPTR MacroScan(                // SCAN AND DEFINE A MACRO (#define, -d)
    macro_scanning defn )       // - scanning definition
{
    int parm_cnt;               // - parameter count
    unsigned name_len;          // - length of macro name
    MEPTR mptr;                 // - final macro defn
    MAC_PARM *parm_names;       // - macro parm names
    int ppscan_mode;            // - previous ppnumber scan mode
    unsigned mlen;              // - current length of macro def'n
    TOKEN_LOCN locn;            // - location for definition

    SrcFileGuardStateSig();
    NextToken();
    if( CurToken != T_ID ) {
        Expecting( "identifier" );
        return( NULL );
    } else {
        SrcFileGetTokenLocn( &locn );
    }
    name_len = TokenLen;
    mlen = offsetof( MEDEFN, macro_name );
    parm_names = NULL;
    MacroOffsetAddMem( &mlen, Buffer, TokenLen + 1 );
    if( CurrChar == '(' ) {         /* parms present */
        if( ! ( defn & MSCAN_MANY ) ) {
            BadCmdLine( ERR_INVALID_OPTION );
            return( NULL );
        }
        NextToken();                /* grab the '(' */
        NextToken();
        parm_cnt = 0;               /* 0 ==> () following */
        for( ;; ) {
            if( CurToken == T_RIGHT_PAREN ) break;
            if( CurToken != T_ID ) {
                Expecting( "identifier" );
                return( NULL );
            }
            if( addParmName( &parm_names, TRUE ) != 0 ) {
                CErr2p( ERR_DUPLICATE_MACRO_PARM, Buffer );
            } else {
                ++parm_cnt;
                MacroOffsetAddMem( &mlen, Buffer, TokenLen + 1 );
            }
            NextToken();
            if( CurToken == T_RIGHT_PAREN ) break;
            if( CurToken == T_NULL ) {
                CErr1( ERR_INVALID_MACRO_DEFN );
                break;
            }
            MustRecog( T_COMMA );
        }
    } else {
        parm_cnt = -1;          /* -1 ==> no () following */
    }
    /* grab replacement tokens */
    ppscan_mode = InitPPScan();         // enable T_PPNUMBER tokens
    mptr = grabTokens( &parm_names
                     , parm_cnt + 1
                     , defn
                     , name_len
                     , mlen
                     , &locn );
    FiniPPScan( ppscan_mode );          // disable T_PPNUMBER tokens

    RingFree( &parm_names );
    return( mptr );
}

static void ppIf( int value )    // PREPROCESSOR IF
{
    if( SrcFileGuardedIf( value ) ) {
        return;
    }
    IncLevel( value );
    ChkEOL();
}

static void ppIfId( int value )  // PREPROCESSOR IFDEF IFNDEF
{
    NextToken();
    ppIf( value );
}

static void CIfDef( void )
{
    SrcFileGuardPpIf();
    NextToken();
    if( CurToken != T_ID ) {
        Expecting( "identifier" );
        IncLevel( 0 );
        return;
    }
    ppIfId( MacroDependsDefined() );
}


static void CIfNDef( void )
{
    SrcFileGuardPpIf();
    NextToken();
    if( CurToken != T_ID ) {
        Expecting( "identifier" );
        IncLevel( 0 );
        return;
    }
    SrcFileGuardPpIfndef( Buffer, TokenLen );
    ppIfId( ! MacroDependsDefined() );
}


static void CIf( void )
{
    SrcFileGuardPpIf();
    // don't set PPS_NO_LEX_ERRORS since we want tokenizing errors diagnosed
    PPState = PPS_EOL;
    NextToken();
    ppIf( PpConstExpr() );
}


static void CElif( void )
{
    int value;

    SrcFileGuardPpElse();
    PPState = PPS_EOL | PPS_NO_LEX_ERRORS;
    NextToken();
    if( NestLevel == 0 || pp_stack->cpp_type == PRE_ELSE ) {
        CErr1( ERR_MISPLACED_ELIF );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;            /* start skipping else part */
            pp_stack->processing = 0;
            pp_stack->cpp_type = PRE_ELIF;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* only evaluate the expression when required */
            if( pp_stack->cpp_type == PRE_IF ) {
                value = PpConstExpr();
                ChkEOL();
                if( value ) {
                    SkipLevel = NestLevel; /* start including else part */
                    pp_stack->processing = 1;
                    pp_stack->cpp_type = PRE_ELIF;
                }
            }
        }
    }
}

static void IncLevel( int value )
{
    struct cpp_info *cpp;

    cpp = VstkPush( &vstkPp );
    pp_stack = cpp;
    SrcFileGetTokenLocn( &cpp->locn );
    cpp->cpp_type = PRE_IF;
    cpp->processing = 0;
    if( NestLevel == SkipLevel ) {
        if( value ) {
            ++SkipLevel;
            cpp->processing = 1;
        }
    }
    ++NestLevel;
}

static void wantEOL( void )
{
    if( CurToken != T_NULL && CurToken != T_EOF ) {
        if( NestLevel == SkipLevel ) {
            CErr1( ANSI_JUNK_FOLLOWS_DIRECTIVE );
        }
        flush2EOL();
    }
}

static void CElse( void )
{
    SrcFileGuardPpElse();
    if( NestLevel == 0  ||  pp_stack->cpp_type == PRE_ELSE ) {
        CErr1( ERR_MISPLACED_ELSE );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;            /* start skipping else part */
            pp_stack->processing = 0;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* cpp_type will be PRE_ELIF if an elif was true */
            if( pp_stack->cpp_type == PRE_IF ) {    /* 19-sep-88 */
                SkipLevel = NestLevel;  /* start including else part */
                pp_stack->processing = 1;
            }
        }
        pp_stack->cpp_type = PRE_ELSE;
    }
    NextToken();
    wantEOL();
}

static void CEndif( void )
{

    if( NestLevel == 0 ) {
        CErr1( ERR_MISPLACED_ENDIF );
    } else {
        struct cpp_info *pp = pp_stack;
        if( pp != NULL && ! SrcFileSame( pp->locn.src_file, SrcFileCurrent() ) ) {
            if( CErr1( WARN_WEIRD_ENDIF_ENCOUNTER ) & MS_PRINTED ) {
                CErr2p( INF_PP_DIRECTIVE_LOCN, &(pp->locn) );
            }
        }
        --NestLevel;
        VstkPop( &vstkPp );
        pp_stack = VstkTop( &vstkPp );
        SrcFileGuardPpEndif();
    }
    if( NestLevel < SkipLevel ) {
        SkipLevel = NestLevel;
    }
    NextToken();
    wantEOL();
}

static void CUnDef( void )
{
    NextToken();
    if( CurToken != T_ID ) {
        Expecting( "identifier" );
        return;
    }
    MacroUndefine( TokenLen );
    NextToken();
    ChkEOL();
}

static void ChkEOL( void )
{
    if( CurToken != T_NULL && CurToken != T_EOF ) {
        Expecting( "end of line" );
    }
}

static void CLine( void )
{
    int adjust;
    LINE_NO line;

    PPState = PPS_EOL | PPS_NO_LEX_ERRORS;
    NextToken();
    if( CurToken == T_CONSTANT ) {
        line = U32Fetch( Constant64 ); // side effects of NextToken
        NextToken();
        if( CurToken == T_NULL ) {
            SrcFileAlias( SrcFileNameCurrent(), line, 0 );
        } else if( CurToken != T_STRING ) {
            Expecting( "string" );
        } else {
            if( CurrChar == '\n' ) {
                // line # has already been incremented
                adjust = 0;
            } else {
                // line # will be incremented
                adjust = -1;
            }
            SrcFileAlias( Buffer, line, adjust );
            NextToken();        // may clobber Buffer's contents
            ChkEOL();           // will increment line # if CurToken != T_NULL
        }
    } else {
        Expecting( "constant" );
    }
}

static void CError( void )
{
    int i;
    int save;

    i = 0;
    while( CurrChar != '\n' && CurrChar != '\r' && CurrChar != LCHR_EOF ) {
        if( i != 0 || CurrChar != ' ' ) {
            Buffer[ i ] = CurrChar;
            ++i;
        }
        NextChar();
    }
    Buffer[ i ] = '\0';
    /* Force #error output to be reported, even with preprocessor */
    save = CompFlags.cpp_output;
    CompFlags.cpp_output = 0;
    CErr2p( ERR_USER_ERROR_MSG, Buffer );
    CompFlags.cpp_output = save;
}

static void CIdent( void )
{
    if( ! CompFlags.extensions_enabled ) {
        CUnknown();
    }
    flush2EOL();
}

static void preProcStmt( void )
{
    PPCTRL      *pp;
    int         hash;

    NextChar();
    NextToken();
    if( CurToken == T_ID ) {
        hash = (TokenLen + preprocWeights[ Buffer[0] - 'a' ]
                 + preprocWeights[ Buffer[TokenLen-1] - 'a' ]) & 15;
        pp = &controlTable[hash];
        if( strcmp( pp->directive, Buffer ) == 0 ) {
            if( NestLevel == SkipLevel ) {
                pp->samelevel();
            } else {
                pp->skiplevel();
            }
        } else {
            CUnknown();
        }
    } else if( CurToken != T_NULL ) {
        CUnknown();
    }
    flush2EOL();
}

// When a new file is opened, it's guarded state will indicate top of file.
// ChkControl() is always called at the start of a file because CurrChar
// will be set to '\n'. ChkControl is always called at the start of every
// line.
// Thus we have complete control over setting the guarded state here.
// We can determine is the first directive is #if, #ifdef or #ifndef.
//
int ChkControl(                 // CHECK AND PROCESS DIRECTIVES
    int expanding )
{
    int lines_skipped;

    while( CurrChar == '\n' ) {
        SrcFileCurrentLocation();
        if( TBreak() ) {
            CErr1( ERR_BREAK_KEY_HIT );
            CSuicide();
        }
        lines_skipped = 0;
        for(;;) {
            if( CompFlags.cpp_output )  PrtChar( '\n' );
            NextChar();
            if( CurrChar != PreProcChar ) {
                SkipAhead();
            }
            if( CurrChar == LCHR_EOF ) break;
            PPState = PPS_EOL | PPS_NO_EXPAND | PPS_NO_LEX_ERRORS;
            if( CurrChar == PreProcChar ) {
                preProcStmt();
            } else if( NestLevel != SkipLevel ) {
                NextToken();
                flush2EOL();
            }
            if( NestLevel == SkipLevel ) break;
            if( CurrChar == LCHR_EOF ) break;
            if( CurrChar == '\n' ) {
                lines_skipped = 1;
            }
        }
        PPState = PPS_NORMAL;
        if( CompFlags.cpp_output ) {
            if( lines_skipped ) {
                EmitLine( SrcFileLine(), SrcFileNameCurrent() );
            }
        }
    }
    // we have already skipped past all white space at the start of the line
    CurToken = ScanToken( expanding );
    // this will be the first significant token on the source line
    SrcFileGuardStateSig();
    return( CurToken );
}


void DirectiveInit(             // INITIALIZE FOR DIRECTIVE PROCESSING
    void )
{
    NestLevel = 0;
    SkipLevel = 0;
    pp_stack = NULL;
    VstkOpen( &vstkPp, sizeof( struct cpp_info ), 8 );
}


void DirectiveFini(             // COMPLETE DIRECTIVE PROCESSING
    void )
{
    struct cpp_info *pp;        // - unclosed entry

    for(;;) {
        pp = VstkPop( &vstkPp );
        if( pp == NULL ) break;
        SetErrLoc( &pp->locn );
        CErr1( ERR_MISSING_CENDIF );
    }
    VstkClose( &vstkPp );
}


int CppPrinting(                // TEST IF AT LEVEL FOR PREPROC PRINTING
    void )
{
    return ( NestLevel == SkipLevel ) && ( CppFile != NULL );
}


unsigned IfDepthInSrcFile(      // COMPUTE #IF DEPTH IN CURRENT SOURCE FILE
    void )
{
    unsigned depth;             // - depth of #if's
    struct cpp_info *pp;        // - current pre-processor stack entry
    SRCFILE curr_src;           // - current SRCFILE

    for( curr_src = SrcFileCurrent(), depth = 0, pp = VstkTop( &vstkPp )
       ; ( pp != NULL ) && ( pp->locn.src_file == curr_src )
       ; ++depth, pp = VstkNext( &vstkPp, pp ) );
    return depth;
}

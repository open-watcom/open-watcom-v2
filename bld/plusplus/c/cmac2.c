/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "preproc.h"
#include "scan.h"
#include "memmgr.h"
#include "fname.h"
#include "ring.h"
#include "vstk.h"
#include "cmdline.h"
#include "cgmisc.h"
#include "name.h"
#include "cmacadd.h"
#include "cscanbuf.h"
#include "brinfo.h"

#include "clibext.h"


#define HasVarArgs(m)       ((m) & MFLAG_HAS_VAR_ARGS)

#define PREPROC_WEIGHT(c)   ((c >= 'a' && c < 'a' + sizeof( preprocWeights )) ? preprocWeights[c - 'a'] : 0)

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
static void CWarning( void );

typedef struct {
    char        *directive;
    void        (*samelevel)( void );
    void        (*skiplevel)( void );
} PPCTRL;

static const unsigned char preprocWeights[] = {
//a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
  2, 0, 0, 9, 1, 4, 3, 0, 5, 0, 0, 7, 0, 0, 0,10, 0,11, 0,13,12, 0,14, 0, 0, 0
};
static const PPCTRL controlTable[] = {
    { "define",     CDefine,    CSkip },
    { "error",      CError,     CSkip },
    { "pragma",     CPragma,    CSkip },
    { "",           NULL,       NULL },
    { "",           NULL,       NULL },
    { "undef",      CUnDef,     CSkip },
    { "else",       CElse,      CElse },
    { "ident",      CIdent,     CSkip },
    { "warning",    CWarning,   CSkip },
    { "elif",       CElif,      CElif },
    { "endif",      CEndif,     CEndif },
    { "if",         CIf,        CSkipIf },
    { "line",       CLine,      CSkip },
    { "include",    CInclude,   CSkip },
    { "ifdef",      CIfDef,     CSkipIf },
    { "ifndef",     CIfNDef,    CSkipIf },
};

enum pp_stack_states {
    PRE_NULL,
    PRE_ELSE,
    PRE_ELIF,
    PRE_IF,
};



static void flush2EOL( void )
{
    while( CurToken != T_NULL && CurToken != T_EOF ) {
        NextToken();
    }
}

static void CSkip( void )
{
}

static void IncLevel( bool value )
{
    struct cpp_info *cpp;

    cpp = VstkPush( &vstkPp );
    pp_stack = cpp;
    SrcFileGetTokenLocn( &cpp->locn );
    cpp->cpp_type = PRE_IF;
    cpp->processing = 0;
    if( SkipLevel == NestLevel ) {
        if( value ) {
            ++SkipLevel;
            cpp->processing = 1;
        }
    }
    ++NestLevel;
}

static void CSkipIf( void )
{
    IncLevel( false );
}

static void CUnknown( void )
{
    if( SkipLevel == NestLevel ) {
        CErr2p( ERR_UNKNOWN_DIRECTIVE, Buffer );
    }
}

static void CInclude( void )
{
    struct {
        boolbit     in_macro    : 1;
    } flags;

    char   buf[_MAX_PATH];

    SrcFileGuardStateSig();
    InitialMacroFlags = MFLAG_NONE;
    flags.in_macro = CompFlags.use_macro_tokens;
    PPCTL_ENABLE_MACROS();
    NextToken();
    PPCTL_DISABLE_MACROS();
    if( CurToken == T_STRING ) {
        OpenSrcFile( Buffer, FT_HEADER );
    } else if( CurToken == T_LT ) {
        if( flags.in_macro ) {
            PPCTL_ENABLE_MACROS();
        }
        buf[0] = '\0';
        for( ;; ) {
            NextToken();
            if( CurToken == T_GT ) {
                OpenSrcFile( buf, FT_LIBRARY );
                break;
            }
            strncat( buf, Buffer, sizeof( buf ) - 2 );
            if( flags.in_macro && !CompFlags.use_macro_tokens || CurToken == T_NULL || CurToken == T_EOF ) {
                CErr1( ERR_INVALID_INCLUDE );
                break;
            }
        }
        if( flags.in_macro ) {
            PPCTL_DISABLE_MACROS();
        }
    } else {
        CErr1( ERR_INVALID_INCLUDE );
    }
    if( CurToken != T_EOF ) {
        NextToken();
    }
}

static void CDefine( void )
{
    MEPTR mentry;

    mentry = MacroScan( MSCAN_DEFINE );
    if( mentry != NULL ) {
        mentry->macro_flags |= MFLAG_USER_DEFINED;
    }
}

#define SYMBOL_VA_ARGS   "__VA_ARGS__"

static unsigned addParmName( MAC_PARM **h, bool add_name )
{
    unsigned    index;
    size_t      len;
    MAC_PARM    *parm_name;

    if( CurToken == T_DOT_DOT_DOT ) {
        len = sizeof( SYMBOL_VA_ARGS );
        memcpy( Buffer, SYMBOL_VA_ARGS, len );
    } else {
        len = strlen( Buffer ) + 1;
    }
    index = 0;
    RingIterBeg( *h, parm_name ) {
        ++index;
        if( NameMemCmp( parm_name->name, Buffer, len ) == 0 ) {
            /* already present */
            return( index );
        }
    } RingIterEnd( parm_name );
    if( add_name ) {
        parm_name = CMemAlloc( offsetof( MAC_PARM, name ) + len );
        memcpy( parm_name->name, Buffer, len );
        RingAppend( h, parm_name );
    }
    return( 0 );
}

static unsigned findParmName( MAC_PARM **h )
{
    if( *h != NULL ) {
        return( addParmName( h, false ) );
    }
    return( 0 );
}

static bool skipEqualOrSharpOK( void )
{
    switch( CurToken ) {
    case T_EQ :
        CurToken = T_EQUAL;     // strip = from ==
        break;
    case T_EQUAL :
        NextToken();
        break;
    case T_SHARP :
    case T_ALT_SHARP :
        NextToken();
        break;
    case T_SHARP_SHARP :
    case T_ALT_SHARP_SHARP :
        CurToken = T_SHARP;     // strip # from ##
        break;
    case T_NULL :
        break;
    default :
        BadCmdLine( ERR_INVALID_OPTION );
        return( false );
    }
    return( true );
}


static MEPTR grabTokens(            // SAVE TOKENS IN A MACRO DEFINITION
    MAC_PARM        **parm_names,   // - macro parm names
    mac_parm_count  parm_count,     // - parameter count
    macro_flags     mflags,         // - macro flags
    macro_scanning  defn,           // - scanning definition
    size_t          name_len,       // - length of macro name
    size_t          mlen,           // - length of macro def'n (so far)
    TOKEN_LOCN      *locn )         // - definition point
{
    MEPTR new_mentry;
    MEPTR mentry;
    unsigned parm_index;
    TOKEN prev_token;
    TOKEN prev_non_ws_token;

    /* unused parameters */ (void)name_len;

    // MacroReallocOverflow was called for the name of the macro + mentry already
    mentry = (MEPTR)MacroOffset;
    DbgAssert( ( MacroReallocOverflow( mlen, 0 ), MacroOffset == (void *)mentry ) );
    mentry->parm_count = parm_count;
    mentry->macro_defn = mlen;
    prev_token = T_NULL;
    prev_non_ws_token = T_NULL;
    if( CurToken != T_NULL ) {
        do {
            CurToken = ScanToken( false );
        } while( CurToken == T_WHITE_SPACE );
        if( defn & MSCAN_EQUALS ) {
            if( CurToken == T_NULL ) {
                // -dFOO from a command line
                MacroSegmentAddToken( &mlen, T_PPNUMBER );
                MacroSegmentAddMem( &mlen, "1", 2 );
                prev_non_ws_token = T_PPNUMBER;
                prev_token = T_PPNUMBER;
            } else {
                if( !skipEqualOrSharpOK() ) {
                    return( NULL );
                }
            }
        }
        if( ( CurToken == T_SHARP_SHARP ) || ( CurToken == T_ALT_SHARP_SHARP ) ) {
            CErr1( ERR_MISPLACED_SHARP_SHARP );
            NextToken();
        }
    }
    for( ; CurToken != T_NULL; ) {
        switch( CurToken ) {
        case T_SHARP:
        case T_ALT_SHARP:
            /* if it is a function-like macro definition */
            if( parm_count > 0 ) {
                CurToken = T_MACRO_SHARP;
            }
            MacroSegmentAddToken( &mlen, CurToken );
            break;
        case T_SHARP_SHARP:
        case T_ALT_SHARP_SHARP:
            CurToken = T_MACRO_SHARP_SHARP;
            MacroSegmentAddToken( &mlen, CurToken );
            break;
        case T_WHITE_SPACE:
            if( prev_token != T_WHITE_SPACE ) {
                MacroSegmentAddToken( &mlen, CurToken );
            }
            break;
        case T_ID:
            parm_index = findParmName( parm_names );
            if( parm_index != 0 ) {
                if( HasVarArgs( mflags ) && ( parm_index + 1 ) == parm_count ) {
                    CurToken = T_MACRO_VAR_PARM;
                } else {
                    CurToken = T_MACRO_PARM;
                }
                MacroSegmentAddToken( &mlen, CurToken );
                MacroSegmentAddChar( &mlen, parm_index - 1 );
            } else {
                MacroSegmentAddToken( &mlen, CurToken );
                MacroSegmentAddMem( &mlen, Buffer, TokenLen + 1 );
            }
            break;
        case T_BAD_CHAR:
            MacroSegmentAddToken( &mlen, CurToken );
            MacroSegmentAddChar( &mlen, Buffer[0] );
            if( Buffer[1] != '\0' ) {
                MacroSegmentAddToken( &mlen, T_WHITE_SPACE );
            }
            break;
        case T_BAD_TOKEN :
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_STRING:
        case T_LSTRING:
            MacroSegmentAddToken( &mlen, CurToken );
            MacroSegmentAddMem( &mlen, Buffer, TokenLen + 1 );
            break;
        default :
            MacroSegmentAddToken( &mlen, CurToken );
            break;
        }
        if( CurToken != T_WHITE_SPACE ) {
            if( prev_non_ws_token == T_MACRO_SHARP && CurToken != T_MACRO_VAR_PARM && CurToken != T_MACRO_PARM ) {
                CErr1( ERR_MUST_BE_MACRO_PARM );
            }
            prev_non_ws_token = CurToken;
        }
        prev_token = CurToken;
        CurToken = ScanToken( false );
        if( (defn & MSCAN_MANY) == 0 ) {
            break;
        }
    }
    if( prev_token == T_WHITE_SPACE ) {
        mlen -= sizeof( TOKEN );
    }
    MacroSegmentAddToken( &mlen, T_NULL );
    if( prev_non_ws_token == T_MACRO_SHARP_SHARP ) {
        CErr1( ERR_MISPLACED_SHARP_SHARP );
    }
    mentry = (MEPTR)MacroOffset;        // MacroOffset could have changed
    TokenLocnAssign( mentry->defn, *locn );
    mentry->macro_len = mlen;
    new_mentry = MacroDefine( mentry, mlen, mflags );
    BrinfDeclMacro( new_mentry );
    if( (defn & MSCAN_MANY) == 0 ) {
        while( CurToken == T_WHITE_SPACE ) {
            CurToken = ScanToken( false );
        }
        if( CurToken != T_NULL ) {
            BadCmdLine( ERR_INVALID_OPTION );
        }
    }
    return( new_mentry );
}


MEPTR MacroScan(                // SCAN AND DEFINE A MACRO (#define, -d)
    macro_scanning defn )       // - scanning definition
{
    mac_parm_count  parm_count;     // - parameter count, end found
    macro_flags     mflags;         // - macro flags
    size_t          name_len;       // - length of macro name
    MEPTR           mentry;         // - final macro defn
    MAC_PARM        *parm_names;    // - macro parm names
    bool            ppscan_mode;    // - previous ppnumber scan mode
    size_t          mlen;           // - current length of macro def'n
    TOKEN_LOCN      locn;           // - location for definition

    SrcFileGuardStateSig();
    NextToken();
    if( !ExpectingToken( T_ID ) ) {
        return( NULL );
    }
    SrcFileGetTokenLocn( &locn );
    name_len = TokenLen;
    mlen = offsetof( MEDEFN, macro_name );
    parm_names = NULL;
    mflags = MFLAG_NONE;
    parm_count = 0;
    MacroSegmentAddMemNoCopy( &mlen, Buffer, TokenLen + 1 );
    if( CurrChar == '(' ) {         /* parms present */
        if( (defn & MSCAN_MANY) == 0 ) {
            BadCmdLine( ERR_INVALID_OPTION );
            return( NULL );
        }
        NextToken();                /* grab the '(' */
        NextToken();
        parm_count = 1;             /* 1 ==> () following */
        for( ; CurToken != T_RIGHT_PAREN; ) {
            if( HasVarArgs( mflags ) ) {
                ExpectingToken( T_RIGHT_PAREN );
                return( NULL );
            }
            if( CurToken != T_DOT_DOT_DOT && !ExpectingToken( T_ID ) ) {
                return( NULL );
            }
            if( addParmName( &parm_names, true ) != 0 ) {
                CErr2p( ERR_DUPLICATE_MACRO_PARM, Buffer );
            } else {
                ++parm_count;
                if( CurToken == T_DOT_DOT_DOT )
                    mflags |= MFLAG_HAS_VAR_ARGS; // can have no further tokens
                MacroSegmentAddMem( &mlen, Buffer, TokenLen + 1 );
            }
            NextToken();
            if( CurToken == T_RIGHT_PAREN )
                break;
            if( CurToken == T_NULL ) {
                CErr1( ERR_INVALID_MACRO_DEFN );
                break;
            }
            MustRecog( T_COMMA );
        }
    }
    /* grab replacement tokens */
    ppscan_mode = InitPPScan();         // enable T_PPNUMBER tokens
    mentry = grabTokens( &parm_names, parm_count, mflags, defn, name_len, mlen, &locn );
    FiniPPScan( ppscan_mode );          // disable T_PPNUMBER tokens

    RingFree( &parm_names );
    return( mentry );
}

static void ChkEOL( void )
{
    if( CurToken != T_EOF ) {
        ExpectingToken( T_NULL );
    }
}

static void ppIf( bool value )      // PREPROCESSOR IF
{
    if( SrcFileGuardedIf( value ) ) {
        return;
    }
    IncLevel( value );
    ChkEOL();
}

static void ppIfId( bool value )    // PREPROCESSOR IFDEF IFNDEF
{
    NextToken();
    ppIf( value );
}

static void CIfDef( void )
{
    SrcFileGuardPpIf();
    NextToken();
    if( !ExpectingToken( T_ID ) ) {
        IncLevel( false );
        return;
    }
    ppIfId( MacroDependsDefined() );
}


static void CIfNDef( void )
{
    SrcFileGuardPpIf();
    NextToken();
    if( !ExpectingToken( T_ID ) ) {
        IncLevel( false );
        return;
    }
    SrcFileGuardPpIfndef( Buffer, TokenLen );
    ppIfId( ! MacroDependsDefined() );
}


static void CIf( void )
{
    SrcFileGuardPpIf();
    PPCTL_ENABLE_LEX_ERRORS();
    PPCTL_ENABLE_MACROS();
    NextToken();
    ppIf( PpConstExpr() );
    PPCTL_DISABLE_MACROS();
    PPCTL_DISABLE_LEX_ERRORS();
}


static void CElif( void )
{
    bool        value;

    SrcFileGuardPpElse();
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( NestLevel == 0 || pp_stack->cpp_type == PRE_ELSE ) {
        CErr1( ERR_MISPLACED_ELIF );
    } else {
        if( SkipLevel == NestLevel ) {
            --SkipLevel;            /* start skipping else part */
            pp_stack->processing = 0;
            pp_stack->cpp_type = PRE_ELIF;
        } else if( SkipLevel + 1 == NestLevel ) {
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
    PPCTL_DISABLE_MACROS();
}

static void wantEOL( void )
{
    if( CurToken != T_NULL && CurToken != T_EOF ) {
        if( SkipLevel == NestLevel ) {
            CErr1( ANSI_JUNK_FOLLOWS_DIRECTIVE );
        }
    }
}

static void CElse( void )
{
    SrcFileGuardPpElse();
    if( NestLevel == 0  ||  pp_stack->cpp_type == PRE_ELSE ) {
        CErr1( ERR_MISPLACED_ELSE );
    } else {
        if( SkipLevel == NestLevel ) {
            --SkipLevel;            /* start skipping else part */
            pp_stack->processing = 0;
        } else if( SkipLevel + 1 == NestLevel ) {
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
    if( SkipLevel > NestLevel ) {
        SkipLevel = NestLevel;
    }
    NextToken();
    wantEOL();
}

static void CUnDef( void )
{
    NextToken();
    if( ExpectingToken( T_ID ) ) {
        MacroUndefine( TokenLen );
        NextToken();
        ChkEOL();
    }
}

static void CLine( void )
{
    int         adjust;
    LINE_NO     line;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( ExpectingToken( T_CONSTANT ) ) {
        line = U32Fetch( Constant64 ); // side effects of NextToken
        NextToken();
        if( CurToken == T_NULL ) {
            if( !CompFlags.cpp_ignore_line ) {
                SrcFileAlias( SrcFileNameCurrent(), line, 0 );
            }
        } else if( ExpectingToken( T_STRING ) ) {
            if( !CompFlags.cpp_ignore_line ) {
                if( CurrChar == '\n' ) {
                    // line # has already been incremented
                    adjust = 0;
                } else {
                    // line # will be incremented
                    adjust = -1;
                }
                SrcFileAlias( Buffer, line, adjust );
            }
            NextToken();        // may clobber Buffer's contents
            ChkEOL();           // will increment line # if CurToken != T_NULL
        }
    }
    PPCTL_DISABLE_MACROS();
}

static void get_arg_message( void )
{
    while( CurrChar == ' ' )
        NextChar();
    TokenLen = 0;
    while( CurrChar != '\n' && CurrChar != '\r' && CurrChar != LCHR_EOF ) {
        WriteBufferChar( CurrChar );
        NextChar();
    }
    WriteBufferNullChar();
}

static void CError( void )
{
    bool save;

    get_arg_message();
    /* Force #error output to be reported, even with preprocessor */
    save = CompFlags.cpp_output;
    CompFlags.cpp_output = false;
    CErr2p( ERR_USER_ERROR_MSG, Buffer );
    CompFlags.cpp_output = save;
}

static void CWarning( void )
{
    bool save;

    if( CompFlags.extensions_enabled || ( CHECK_STD( >= , CXX23 ) ) ) {
        get_arg_message();
        /* Force #warning output to be reported, even with preprocessor */
        save = CompFlags.cpp_output;
        CompFlags.cpp_output = false;
        CErr2p( WARN_USER_WARNING_MSG, Buffer );
        CompFlags.cpp_output = save;
    } else {
        CUnknown();
    }
}

static void CIdent( void )
{
    if( !CompFlags.extensions_enabled ) {
        CUnknown();
    }
}

static void preProcStmt( void )
{
    const PPCTRL    *pp;
    int             hash;

    NextChar();
    NextToken();
    if( CurToken == T_ID ) {
        hash = (TokenLen + PREPROC_WEIGHT( Buffer[0] ) + PREPROC_WEIGHT( Buffer[TokenLen - 1] )) & 0x0f;
        pp = &controlTable[hash];
        if( strcmp( pp->directive, Buffer ) == 0 ) {
            if( SkipLevel == NestLevel ) {
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
}

// When a new file is opened, it's guarded state will indicate top of file.
// ChkControl() is always called at the start of a file because CurrChar
// will be set to '\n'. ChkControl is always called at the start of every
// line.
// Thus we have complete control over setting the guarded state here.
// We can determine is the first directive is #if, #ifdef or #ifndef.
//
TOKEN ChkControl(               // CHECK AND PROCESS DIRECTIVES
    bool expanding )
{
    bool        lines_skipped;
    ppctl_t     old_ppctl;
    TOKEN       token;

    while( CurrChar == '\n' ) {
        SrcFileCurrentLocation();
        if( TBreak() ) {
            CErr1( ERR_BREAK_KEY_HIT );
            CSuicide();
        }
        lines_skipped = false;
        old_ppctl = PPControl;
        for(;;) {
            if( CompFlags.cpp_output )
                PrtChar( '\n' );
            NextChar();
            // look for a #-char or the corresponding digraph (%:)
            if( CurrChar != PreProcChar && CurrChar != '%' ) {
                SkipAhead();
            }
            if( CurrChar == LCHR_EOF )
                break;

            if( CurrChar == '%' ) {
                NextChar();
                if( CurrChar == ':' ) {
                    // replace the digraph (%:) with the preproc-char
                    CurrChar = PreProcChar;
                } else {
                    GetNextCharUndo( CurrChar );
                    CurrChar = '%';
                }
            }
            if( CurrChar == PreProcChar ) {
                PPCTL_ENABLE_EOL();
                PPCTL_DISABLE_MACROS();
                PPCTL_DISABLE_LEX_ERRORS();
                preProcStmt();
                flush2EOL();
                PPControl = old_ppctl;
            } else if( SkipLevel != NestLevel ) {
                PPCTL_ENABLE_EOL();
                PPCTL_DISABLE_MACROS();
                PPCTL_DISABLE_LEX_ERRORS();
                NextToken();
                flush2EOL();
                PPControl = old_ppctl;
            }
            if( SkipLevel == NestLevel )
                break;
            if( CurrChar == LCHR_EOF )
                break;
            if( CurrChar == '\n' ) {
                lines_skipped = true;
            }
        }
        if( CompFlags.cpp_output ) {
            if( lines_skipped ) {
                CppEmitPoundLine( SrcFileLine(), SrcFileNameCurrent(), EL_NULL );
            }
        }
    }
    // we have already skipped past all white space at the start of the line
    token = ScanToken( expanding );
    // this will be the first significant token on the source line
    SrcFileGuardStateSig();
    return( token );
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

    for( ; (pp = VstkPop( &vstkPp )) != NULL; ) {
        SetErrLoc( &pp->locn );
        CErr1( ERR_MISSING_CENDIF );
    }
    VstkClose( &vstkPp );
}


bool CppPrinting(               // TEST IF AT LEVEL FOR PREPROC PRINTING
    void )
{
    return( ( SkipLevel == NestLevel ) && ( CppFile != NULL ) );
}


unsigned IfDepthInSrcFile(      // COMPUTE #IF DEPTH IN CURRENT SOURCE FILE
    void )
{
    unsigned depth;             // - depth of #if's
    struct cpp_info *pp;        // - current pre-processor stack entry
    SRCFILE curr_src;           // - current SRCFILE

    curr_src = SrcFileCurrent();
    depth = 0;
    VstkIterBeg( &vstkPp, pp ) {
        if( pp->locn.src_file != curr_src )
            break;
        ++depth;
    }
    return( depth );
}

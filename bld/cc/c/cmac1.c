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


#include "cvars.h"
#include "scan.h"


#define T_UNEXPANDABLE_ID       T_LAST_TOKEN

struct  tokens {
        struct  tokens  *next;
        int     length;
        char    buf[1];
};

typedef struct macro_token {
        struct macro_token *next;
        char    token;
        char    data[1];
} MACRO_TOKEN;

typedef struct macro_arg {
        char    *arg;
} MACRO_ARG;

typedef struct nested_macros {
        struct nested_macros *next;
        MEPTR   mentry;
        MACRO_ARG *macro_parms;
        char    rescanning;
        char    substituting_parms;
} NESTED_MACRO;

NESTED_MACRO *NestedMacros;
MACRO_TOKEN  *TokenList;
MACRO_TOKEN  *MacroExpansion();
MACRO_TOKEN *NestedMacroExpansion( int );

struct special_macro_names {
        char    *name;
        int     value;
 };
static struct special_macro_names  SpcMacros[] = {
        "__LINE__",     MACRO_LINE,
        "__FILE__",     MACRO_FILE,
        "__DATE__",     MACRO_DATE,
        "__TIME__",     MACRO_TIME,
        "__STDC__",     MACRO_STDC,
        NULL,       0
 };


static MEPTR  MKSpcMacEntry( struct special_macro_names *mac )
{
    int         macro_len;
    MEPTR       mentry;

    macro_len = sizeof( MEDEFN ) + strlen( mac->name );
    mentry = (MEPTR)  CMemAlloc( macro_len );
    mentry->macro_defn = 0; /* indicate special macro */
    mentry->macro_len = macro_len;
    mentry->parm_count = mac->value;
    strcpy( mentry->macro_name, mac->name );
    return( mentry );
}

void MacroInit(void)
{
    int         i;
    struct special_macro_names *mac;
    MEPTR       mentry;

    MTokenLen = 0;
    MacroCount = 0;
    MacroPtr = NULL;
    CppStackInit();
    NestedMacros = NULL;
    TokenList = NULL;
    UndefMacroList = NULL;                              /* 26-may-94 */
    InitialMacroFlag = MACRO_DEFINED_BEFORE_FIRST_INCLUDE;
    AllocMacroSegment( 0x1000 );
    MacHash = (MEPTR *)MacroSegment;
    MacroOffset = MacroSegment + MACRO_HASH_SIZE * sizeof(MEPTR);
    for( i=0; i < MACRO_HASH_SIZE; ++i ) {
        MacHash[i] = NULL;
    }
    for( mac = SpcMacros; mac->name; ++mac ) {
        mentry = MKSpcMacEntry( mac );
        MacroAdd( mentry, Buffer, 0, 0 );
    }
    TimeInit(); /* grab time and date for __TIME__ and __DATE__ */
}

static struct special_macro_names  SpcMacroCompOnly[] = {
        "__FUNCTION__", MACRO_FUNC,
        NULL,       0
 };

void MacroAddComp(void)
{
    struct special_macro_names *mac;
    MEPTR       mentry;

    for( mac = SpcMacroCompOnly; mac->name; ++mac ) {
        mentry = MKSpcMacEntry( mac );
        MacroAdd( mentry, Buffer, 0, 0 );
    }
}

void MacroFini(void)
{

    CppStackFini();
    MacroPurge();
}


void MacroPurge(void)
{
#if 0
    int         i;
    MEPTR       mentry;

    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        for( ; mentry = MacHash[i]; ) {
            MacHash[i] = mentry->next_macro;
            CMemFree( mentry );
        }
    }
#endif
}

static void DeleteNestedMacro(void)
{
    MEPTR       mentry;
    NESTED_MACRO *nested;
    MACRO_ARG   *macro_parms;
    int         i;

    nested = NestedMacros;
    if( nested != NULL ) {
        NestedMacros = nested->next;
        macro_parms =  nested->macro_parms;
        mentry = nested->mentry;
        CMemFree( nested );
        i = mentry->parm_count - 1;
        if( i > 0  &&  macro_parms != NULL ) {
            do {
                --i;
                CMemFree( macro_parms[i].arg );
            } while( i != 0 );
            CMemFree( macro_parms );
        }
    }
}

void GetMacroToken(void)
{
    MACRO_TOKEN *mtok;
    unsigned    i;
    struct {
        unsigned keep_token : 1;
        unsigned next_token : 1;
    } flag;

    CurToken = T_NULL;
    for(;;) {
        flag.keep_token = FALSE;
        flag.next_token = FALSE;
        mtok = TokenList;
        if( mtok == NULL ) {
            MacroPtr = NULL;
            break;
        }
        CurToken = mtok->token;
        i = 0;
        while( Buffer[i] = mtok->data[i] )i++;
        switch( CurToken ) {
        case T_UNEXPANDABLE_ID:
            if( ! CompFlags.doing_macro_expansion ) {
                CalcHash( Buffer, i );
                CurToken = KwLookup();
            }
            break;
        case T_ID:
        case T_SAVED_ID:
            CalcHash( Buffer, i );
            if( CompFlags.doing_macro_expansion ) {     /* 04-nov-94 */
                CurToken = T_ID;
            } else {
                CurToken = KwLookup();
            }
            break;
        case T_CONSTANT:
            ReScanInit( mtok->data );
            ReScanToken();
            break;
        case T_PPNUMBER:
            ReScanInit( mtok->data );
            if( ReScanToken() ){  // didn't finish string bad boy
                char *tcur;
                char *tbeg;

                tbeg = mtok->data;
                tcur = ReScanPos();
                // ppnumber is quite general so it may absorb multiple tokens
                // overlapping src & dst so do our own copy;
                for(;;){
                    *tbeg = *tcur;
                    if( *tcur == '\0' )break;
                    ++tcur;
                    ++tbeg;
                }
                flag.keep_token = TRUE;
            }
            break;
        case T_LSTRING:
            CurToken = T_STRING;
            CompFlags.wide_char_string = 1;
            CLitLength = i + 1;
            break;
        case T_STRING:
            CompFlags.wide_char_string = 0;
            CLitLength = i + 1;
            break;
        case T_BAD_CHAR:
            break;
        case T_NULL:
            if( mtok->data[0] == 'Z' ) {    // if end of macro
                DeleteNestedMacro();
            }
            flag.next_token = TRUE;
            break;
        }
        if( ! flag.keep_token ) {
            TokenList = mtok->next;
            CMemFree( mtok );
        }
        if( ! flag.next_token ) {
            break;
        }
    }
}


local int ExpandMacroToken( int i )
{
    char        *p;

    switch( *MacroPtr ) {
    case T_CONSTANT:
    case T_PPNUMBER:
    case T_ID:
    case T_UNEXPANDABLE_ID:
    case T_SAVED_ID:
    case T_BAD_TOKEN:                                   /* 07-apr-91 */
        ++MacroPtr;
        while( Buffer[i] = *MacroPtr++ ){
            ++i;
            if( i >= BUF_SIZE-2 ){
                CErr1( ERR_TOKEN_TRUNCATED );
                Buffer[i] = '\0';
                break;
            }
        }
        break;
    case T_LSTRING:                                     /* 15-may-92 */
        Buffer[i++] = 'L';
    case T_STRING:                                      /* 15-dec-91 */
        Buffer[i++] = '"';
        ++MacroPtr;
        while( Buffer[i] = *MacroPtr++ )  ++i;
        Buffer[i++] = '"';
        Buffer[i] = '\0';
        break;
    default:                                            /* 28-mar-90 */
        p = Tokens[ *MacroPtr ];
        ++MacroPtr;
        while( Buffer[i] = *p++ )  ++i;
        break;
    }
    return( i );
}


int SpecialMacro( MEPTR mentry )
{
    char        *p;
    char        *bufp;

    CompFlags.wide_char_string = 0;                     /* 16-dec-91 */
    switch( mentry->parm_count ) {
    case MACRO_LINE:
        utoa( TokenLine, Buffer, 10 );
        Constant = TokenLine;
        ConstType = TYPE_INT;
        return( T_CONSTANT );
    case MACRO_FILE:
        if( SrcFile == NULL ) {                 /* 26-sep-94 */
            // SrcFile can be NULL if user is missing a closing ')' on
            // a macro to contains a reference to __FILE__.
            // We end up scanning all the way to the end of the file
            // causing SrcFile to become NULL.
            p = "";
        } else {
            p = SrcFile->src_name;
        }
        bufp = Buffer;
        for(;;) {
            *bufp++ = *p;
            if( *p == '\0' ) break;
            if( *p == '\\' ) *bufp++ = '\\';
            ++p;
        }
        CLitLength = bufp - Buffer;
        return( T_STRING );
    case MACRO_DATE:
        CLitLength = 12;
        strcpy( Buffer, __Date );
        return( T_STRING );
    case MACRO_TIME:
        CLitLength = 9;
        strcpy( Buffer, __Time );
        return( T_STRING );
    case MACRO_STDC:
        Buffer[0] = '1';
        Buffer[1] = '\0';
        Constant = 1;
        ConstType = TYPE_INT;
        return( T_CONSTANT );
    case MACRO_FUNC:
        p = "";
        if( CurFunc != NULL ){
            if( CurFunc->name != NULL ){
                p = CurFunc->name;
            }
        }
        CLitLength = strlen( p ) +1;
        strcpy( Buffer, p );
        return( T_STRING );
    default:
        return( 0 ); // shut up the compiler
    }
}


local int NextMToken()
{
    CompFlags.doing_macro_expansion = 1;
    GetMacroToken();
    if( CurToken == T_NULL ) {
        CurToken = ScanToken();
    }
    CompFlags.doing_macro_expansion = 0;
    return( CurToken );
}

static MACRO_ARG *CollectParms(void)
{
    MEPTR       mentry;
    int         j;
    int         bracket;
    TOKEN       tok;
    TOKEN       prev_tok;
    int         parm_cnt;
    int         total;  /* total # of bytes in token sequence */
    int         ppscan_mode;
    MACRO_ARG   *macro_parms;
    struct tokens **token_tail;
    auto        struct tokens *token_head;

    macro_parms = NULL;
    mentry = NextMacro;
    if( mentry->parm_count != 0 ) { /* if () expected */
        ppscan_mode = InitPPScan();             // enable T_PPNUMBER tokens
        if( mentry->parm_count > 1 ) {
            macro_parms = (MACRO_ARG *)CMemAlloc( (mentry->parm_count - 1) *
                                    sizeof( MACRO_ARG ) );
        }
        parm_cnt = 0;
        do {
            tok = NextMToken();
        } while( tok == T_WHITE_SPACE );
        /* tok will now be a '(' */
        bracket = 0;
        token_tail = &token_head;
        token_head = NULL;
        total = 0;
        MTokenLen = 0;
        for( ;; ) {
            prev_tok = tok;
            do {
                tok = NextMToken();
                if( tok != T_WHITE_SPACE ) break;
            } while( MTokenLen == 0 );
            if( tok == T_EOF || tok == T_NULL ) {       /* 08-dec-94 */
                CErr( ERR_INCOMPLETE_MACRO, mentry->macro_name );
                break;
            }
            if( tok == T_BAD_TOKEN &&                   /* 18-feb-90 */
                BadTokenInfo == ERR_MISSING_QUOTE ) {
                CErr1( ERR_MISSING_QUOTE );
                tok = T_RIGHT_PAREN;
            }
            if( tok == T_LEFT_PAREN ) {
                ++bracket;
            } else if( tok == T_RIGHT_PAREN ) {
                if( bracket == 0 ) break;
                --bracket;
            } else if( tok == T_COMMA  &&  bracket == 0 ) {
                if( prev_tok == T_WHITE_SPACE ) --MTokenLen;
                if( macro_parms != NULL ) {     // if expecting parms
                    SaveParm( mentry, MTokenLen, parm_cnt, macro_parms,
                            token_head, total );
                }
                ++parm_cnt;
                token_tail = &token_head;
                token_head = NULL;
                total = 0;
                MTokenLen = 0;
                continue;
            }
            /* determine size of current token */
            j = 1;
            switch( tok ) {
            case T_WHITE_SPACE:
                if( prev_tok == T_WHITE_SPACE ) j = 0;
                break;
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_STRING:
            case T_BAD_TOKEN:                           /* 07-apr-91 */
                j += strlen( Buffer );
                ++j;
            }
            if( MTokenLen + j >= BUF_SIZE ) { /* if not enough space */
                *token_tail = (struct tokens *)
                    CMemAlloc( sizeof( struct tokens ) + MTokenLen );
                (*token_tail)->length = MTokenLen;
                (*token_tail)->next = NULL;
                memcpy( &(*token_tail)->buf, TokenBuf, MTokenLen );
                token_tail = &(*token_tail)->next;
                total += MTokenLen;
                MTokenLen = 0;
            }
            if( tok == T_STRING  &&  CompFlags.wide_char_string ) {
                tok = T_LSTRING;
            }
            TokenBuf[MTokenLen++] = tok;
            switch( tok ) {
            case T_WHITE_SPACE:
                if( prev_tok == T_WHITE_SPACE ) --MTokenLen;
                break;
            case T_BAD_CHAR:
                TokenBuf[MTokenLen++] = Buffer[0];
                if( Buffer[1] != '\0' ) {
                     TokenBuf[MTokenLen++] = T_WHITE_SPACE;
                }
                break;
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_LSTRING:
            case T_STRING:
            case T_BAD_TOKEN:                           /* 07-apr-91 */
                j = 0;
                while( TokenBuf[MTokenLen++] = Buffer[j++] ) ;
            }
        }
        if( prev_tok == T_WHITE_SPACE ) --MTokenLen;
        if( macro_parms != NULL ) {     // if expecting parms
            SaveParm( mentry, MTokenLen, parm_cnt, macro_parms,
                        token_head, total );
            ++parm_cnt;
        } else if( MTokenLen + total != 0 ) {
            ++parm_cnt;                 // will cause "too many parms" error
        }
        if( parm_cnt < mentry->parm_count - 1 ) {
            CErr( ERR_TOO_FEW_MACRO_PARMS, mentry->macro_name );
        } else if( parm_cnt > mentry->parm_count - 1 ) {
            if( mentry->parm_count - 1 != 0 ){
                CWarn( WARN_PARM_COUNT_MISMATCH, ERR_TOO_MANY_MACRO_PARMS, mentry->macro_name  );
            }
        } else if( far_strcmp( mentry->macro_name, "va_start", 9 ) == 0 ) {
            if( SymLevel != 0  &&  ! VarParm( CurFunc ) ) {
                CErr1( ERR_MUST_BE_VAR_PARM_FUNC );
            }
        }
        FiniPPScan( ppscan_mode );      // disable T_PPNUMBER tokens
    }
    return( macro_parms );
}


local void SaveParm( MEPTR      mentry,
                     int        i,
                     int        parm_cnt,
                     MACRO_ARG  *macro_parms,
                     struct tokens *token_list,
                     int        total )
{
    struct tokens *next_tokens;
    char *p;

    TokenBuf[i++] = T_NULL;
    if( parm_cnt < mentry->parm_count - 1 ) {
        p = CMemAlloc( total + i );
        macro_parms[ parm_cnt ].arg = p;
        if( p != NULL ) {
            total = 0;
            while( token_list != NULL ) {
                memcpy( &p[ total ], token_list->buf, token_list->length );
                total += token_list->length;
                next_tokens = token_list->next;
                CMemFree( token_list );
                token_list = next_tokens;
            }
            memcpy( &p[ total ], TokenBuf, i );
        }
    }
}


#ifndef NDEBUG

void DumpMDefn( char *p )
    {
        int c;

        for(; p;) {
            if( *p == 0 ) break;
            switch( *p ) {
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
                ++p;
                for(;;) {
                    c = *p++;
                    if( c == '\0' ) break;
                    putchar( c );
                }
                continue;
            case T_LSTRING:
                putchar( 'L' );
            case T_STRING:
                ++p;
                putchar( '\"' );
                for(;;) {
                    c = *p++;
                    if( c == '\0' ) break;
                    putchar( c );
                }
                putchar( '\"' );
                continue;
            case T_WHITE_SPACE:
                ++p;
                putchar( ' ' );
                continue;
            case T_BAD_CHAR:
                ++p;
                putchar( *p++ );
                continue;
            case T_MACRO_PARM:
                ++p;
                printf( "parm#%c", '1' + *p++ );
                continue;
            default:
                printf( "%s", Tokens[ *p ] );
                ++p;
                continue;
            }
        }
        putchar( '\n' );
    }
#endif

void DumpMTokens( MACRO_TOKEN *mtok )
{
    while( mtok ) {
        printf( "%s\n", mtok->data );
        mtok = mtok->next;
    }
}
void DumpNestedMacros()
{
    NESTED_MACRO *nested;

    nested = NestedMacros;
    while( nested ) {
        printf( "%s\n", nested->mentry->macro_name );
        nested = nested->next;
    }
}


int TokLength( char __FAR *p )
{
    int len;

    len = 0;
    while( *p++ ) ++len;
    return( len );
}

static MACRO_TOKEN *BuildAToken( char __FAR *p )
{
    int         len;
    MACRO_TOKEN *mtok;

    len = TokLength( p );
    mtok = (MACRO_TOKEN *)CMemAlloc( sizeof(MACRO_TOKEN) + len );
    mtok->next = NULL;
    len = 0;
    for(;;) {
        mtok->data[len] = *p;
        if( *p == '\0' ) break;
        ++p;
        ++len;
    }
    return( mtok );
}

MACRO_TOKEN *AppendToken( MACRO_TOKEN *head, int token, char *data )
{
    MACRO_TOKEN *tail;
    MACRO_TOKEN *new;

    new = BuildAToken( data );
    new->token = token;
    if( head == NULL ) {
        head = new;
    } else {
        tail = head;
        while( tail->next != NULL ) tail = tail->next;
        tail->next = new;
    }
    return( head );
}

int Expandable( MACRO_TOKEN *mtok, int macro_parm )
{
    int         lparen;

    if( NextMacro->macro_defn == 0 ) {  /* if special macro */
        return( 1 );
    }
    if( NextMacro->parm_count == 0 ) { /* if () not expected */
        if( macro_parm ) {                              /* 20-feb-93 */
            if( MacroBeingExpanded( NextMacro ) )  return( 0 );
        }
        return( 1 );
    }
    for(;;) {
        if( mtok == NULL ) break;
        if( mtok->token != T_WHITE_SPACE  &&  mtok->token != T_NULL ) break;
        mtok = mtok->next;
    }
    if( mtok != NULL ) {
        if( mtok->token == T_LEFT_PAREN ) {
            if( MacroDepth == 1  &&  !macro_parm )  return( 1 );
            lparen = 0;
            for(;;) {
                mtok = mtok->next;
                if( mtok == NULL ) break;
                if( mtok->token == T_LEFT_PAREN ) {
                    ++lparen;
                } else if( mtok->token == T_RIGHT_PAREN ) {
                    if( lparen == 0 )  return( 1 );
                    --lparen;
                }
            }
        }
    } else if( ! macro_parm ) {
        SkipAhead();
        if( CurrChar == '(' ) {
            return( 1 );
        } else if( CompFlags.cpp_output ) {
            return( 2 );
        }
    }
    return( 0 );
}

int MacroBeingExpanded( MEPTR mentry )
{
    NESTED_MACRO *nested;

    nested = NestedMacros;
    while( nested ) {
        if( nested->mentry == mentry )  return( 1 );
        if( ! nested->rescanning )  break;
        nested = nested->next;
    }
    return( 0 );
}

MACRO_TOKEN *ExpandNestedMacros( MACRO_TOKEN *head, int rescanning )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *toklist;
    MACRO_TOKEN *prev_tok;
    MACRO_TOKEN *old_tokenlist;
    int         i;

    mtok = head;
    ++MacroDepth;
    prev_tok = NULL;
    for(;;) {
        if( mtok == NULL ) break;
        toklist = NULL;
        if( mtok->token == T_ID ) {
            // if macro and not being expanded, then expand it
            // only tokens available for expansion are those in mtok list
            i = 0;
            while( Buffer[i] = mtok->data[i] ) i++;
            CalcHash( Buffer, i );
            if( IdLookup() == T_MACRO ) {
                if( rescanning ) {
                    if( MacroBeingExpanded( NextMacro ) ) {
                        mtok->token = T_UNEXPANDABLE_ID;
                    } else {
                        toklist = mtok;
                        while( toklist->next != NULL ) {
                            toklist = toklist->next;
                        }
                        toklist->next = TokenList;
                        i = Expandable( mtok->next, 0 );
                        switch( i ) {
                        case 0:         // macro is currently not expandable
                            mtok->token = T_MACRO;
                            toklist->next = NULL;
                            toklist = NULL;
                            break;
                        case 1:         // macro is expandable
                            TokenList = mtok->next;
                            if( head == mtok ) {
                                head = NULL;
                                prev_tok = NULL;
                            }
                            CMemFree( mtok );
                            toklist = MacroExpansion( rescanning );
                            mtok = TokenList;
                            TokenList = NULL;
                            break;
                        case 2:         // we skipped over some white space
                            mtok->token = T_UNEXPANDABLE_ID;
                            toklist->next = NULL;
                            toklist = BuildAToken( " " );
                            toklist->token = T_WHITE_SPACE;
                            toklist->next = mtok->next;
                            mtok->next = toklist;
                            toklist = NULL;
                            break;
                        }
                    }
                } else {        // expanding a macro parm
                    if( Expandable( mtok->next, 1 ) ) {
                        old_tokenlist = TokenList;
                        TokenList = mtok->next;
                        if( head == mtok ) {
                            head = NULL;
                            prev_tok = NULL;
                        }
                        CMemFree( mtok );
                        toklist = NestedMacroExpansion( rescanning );
                        mtok = TokenList;
                        TokenList = old_tokenlist;
                    } else {
                        prev_tok = mtok;
                        mtok = mtok->next;      // advance onto next token
                    }
                }
            } else {
                mtok->token = T_SAVED_ID;       // avoid rechecking this ID
                prev_tok = mtok;
                mtok = mtok->next;      // advance onto next token
            }
        } else if( mtok->token == T_NULL ) {
            toklist = mtok->next;
            if( mtok->data[0] == 'Z' ) {        // end of a macro
                rescanning = NestedMacros->rescanning;
                DeleteNestedMacro();
                CMemFree( mtok );
                mtok = toklist;
            } else {                            // end of a macro parm
                if( toklist != NULL ) {
                    TokenList = toklist;
                }
                CMemFree( mtok );
                mtok = NULL;                    // will cause us to exit
            }
            toklist = NULL;
        } else {                        // advance onto next token
            prev_tok = mtok;
            mtok = mtok->next;
        }
        if( toklist != NULL ) {         // new tokens to insert
            if( prev_tok == NULL ) {
                head = toklist;
            } else {
                prev_tok->next = toklist;
            }
            if( mtok != NULL ) {
                while( toklist->next != NULL )  toklist = toklist->next;
                toklist->next = mtok;
            }
            if( prev_tok == NULL ) {
                mtok = head;
            } else {
                mtok = prev_tok->next;
            }
        } else {
            // either no change, or tokens were deleted
            if( prev_tok == NULL ) {
                head = mtok;
            } else {
                prev_tok->next = mtok;
            }
        }
    }
    for( mtok = head; mtok; mtok = mtok->next ) {       /* 02-nov-93 */
        // change a temporarily unexpandable ID into an ID because it
        // could become expandable in a later rescanning phase
        if( mtok->token == T_MACRO )  mtok->token = T_ID;
    }
    --MacroDepth;
    return( head );
}

int GlueTokenToBuffer( MACRO_TOKEN *first, int pos )
{
    int len;

    len = 0;
    Buffer[pos] = '\0';
    if( first != NULL ) {                               /* 19-apr-93 */
        MacroPtr = &first->token;
        pos = ExpandMacroToken( pos );
    }
    return( pos );
}

static MACRO_TOKEN *ReTokenBuffer10( void )
{ // retokenize starting at Buffer[10]
    MACRO_TOKEN *head;
    MACRO_TOKEN **lnk;
    MACRO_TOKEN *new;
    int         ppscan_mode;

    ppscan_mode = InitPPScan();
    ReScanInit( &Buffer[10] );
    head = NULL;
    lnk = &head;
    for(;;) {
        ReScanToken();
        new = BuildAToken( Buffer );
        new->token = CurToken;
        *lnk = new;
        lnk = &new->next;
        if( CompFlags.rescan_buffer_done )  break;
    }
    FiniPPScan( ppscan_mode );
    return( head );
}


MACRO_TOKEN *GlueTokens( MACRO_TOKEN *head )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN **lnk;
    MACRO_TOKEN *next;

    lnk  = &head;
    mtok = *lnk;
    for(;;) {
        if( mtok == NULL ) break;
        if( mtok->token != T_WHITE_SPACE ) {
            next = mtok->next;
            if( next == NULL ) break;
            if( next->token == T_WHITE_SPACE )  next = next->next;
            if( next == NULL ) break;
            if( next->token == T_MACRO_SHARP_SHARP ) {  // let's paste
                int         pos;
                MACRO_TOKEN *rem;

                next = next->next;
                // glue mtok->token with next->token to make one token
                // untokenize mtok into Buffer at a funny pos to allow non overlapp expandve
                pos = GlueTokenToBuffer( mtok, 10 );
                if( next != NULL ){
                    pos = GlueTokenToBuffer( next, pos ); //paste in next
                    rem = next->next;  // save unseen stuff
                    next->next = NULL; // break link;
                }else{
                    rem = NULL;
                }
                while( mtok != NULL ){ //free old stuff [mtoken,##,{atok,} next]
                    next = mtok->next;
                    CMemFree( mtok );
                    mtok = next;
                }
                mtok = ReTokenBuffer10();
                *lnk = mtok;  // link in new mtok to mtok's link
                while( mtok->next != NULL ){ //position mtok & lnk to last token
                    lnk = &mtok->next;
                    mtok = *lnk;
                };
                mtok->next  = rem; //add in remainder of unseen stuff
                continue;          //ready to go
            }
        }
        lnk = &mtok->next;
        mtok = *lnk;
    }
    return( head );
}

static MACRO_TOKEN **NextString( MACRO_TOKEN **lnk, unsigned i )
{
    MACRO_TOKEN *mtok;
    Buffer[i] = '\0';
    CLitLength = i + 1;
    CurToken = T_STRING;
    mtok = BuildAToken( Buffer );
    mtok->token = T_STRING;
    *lnk = mtok;
    lnk = &mtok->next;
    return( lnk );
}

local MACRO_TOKEN *BuildString(  char *p )
{
    MACRO_TOKEN    *head;
    MACRO_TOKEN   **lnk;
    int             i;
    int             c;
    char           *tokenstr;
    int             len;

    head = NULL;
    lnk = &head;

    i = 0;
    if( p != NULL ) {
        while( *p == T_WHITE_SPACE ) ++p;   //eat leading wspace
        while(  *p != T_NULL ) {
            if( i >= (BUF_SIZE-8) ) {
                lnk = NextString( lnk, i );
                i = 0;
            }
            switch( *p ) {
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_BAD_TOKEN:                           /* 07-apr-91 */
                ++p;
                for(;;) {
                    c = *p++;
                    if( c == '\0' ) break;
                    if( c == '\\' ) Buffer[i++] = c; /* 15-mar-88 */
                    Buffer[i++] = c;
                    if( i >= (BUF_SIZE-8) ) {
                        lnk = NextString( lnk, i );
                        i = 0;
                    }
                }
                break;
            case T_LSTRING:
                Buffer[i++] = 'L';
            case T_STRING:
                ++p;
                Buffer[i++] = '\\';
                Buffer[i++] = '"';
                for(;;) {
                    c = *p++;
                    if( c == '\0' ) break;
                    if( c == '\\'  ||  c == '"' ) Buffer[i++] = '\\';
                    Buffer[i++] = c;
                    if( i >= (BUF_SIZE-8) ) {
                        lnk = NextString( lnk, i );
                        i = 0;
                    }
                }
                Buffer[i++] = '\\';
                Buffer[i++] = '"';
                break;
            case T_WHITE_SPACE:
                while( *p == T_WHITE_SPACE )++p;
                if( *p != T_NULL ){
                    Buffer[i++] = ' ';
                }
                break;
            case T_BAD_CHAR:
                ++p;
                if( *p == '\\' && p[1] == T_NULL ) {    /* 17-nov-94 */
                    CErr1( ERR_INVALID_STRING_LITERAL );
                }
                Buffer[i++] = *p++;
                break;
            default:
                tokenstr = Tokens[ *p ];
                ++p;
                len = strlen( tokenstr );
                if( i >= (BUF_SIZE-len) ) {
                    lnk = NextString( lnk, i );
                    i = 0;
                }
                memcpy( &Buffer[i], tokenstr, len );
                i += len;
                break;
            }
        }
        if( i > 0 ) {
            lnk = NextString( lnk, i );
            i = 0;
        }
    }
    return( head );
}


MACRO_TOKEN *BuildMTokenList( char __FAR *p, MACRO_ARG *macro_parms )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN  *head;
    MACRO_TOKEN **lnk;
    NESTED_MACRO *nested;
    char __FAR  *p2;
    char        buf[2];
    enum TOKEN  prev_token;

    head = NULL;
    lnk = &head;
    nested = NestedMacros;
    buf[1] = '\0';
    prev_token = T_NULL;
    if( p == NULL )  return( NULL );                    /* 12-nov-92 */
    for(;;) {
        if( *p == 0 ) break;
        switch( *p ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
        case T_BAD_TOKEN:
            mtok = BuildAToken( p + 1 );
            mtok->token = *p++;
            while( *p++ ) {;}
            break;
        case T_LSTRING:
        case T_STRING:
            mtok = BuildAToken( p + 1 );
            mtok->token = *p++;
            while( *p++ ) {;}
            break;
        case T_WHITE_SPACE:
            ++p;
            if( prev_token == T_MACRO_SHARP_SHARP ) continue;
            mtok = BuildAToken( " " );
            mtok->token = T_WHITE_SPACE;
            break;
        case T_BAD_CHAR:
            ++p;
            buf[0] = *p++;
            mtok = BuildAToken( buf );
            mtok->token = T_BAD_CHAR;
            break;
        case T_MACRO_SHARP:
            ++p;                // skip over T_MACRO_SHARP
            while( *p == T_WHITE_SPACE ) ++p;
            ++p;                // skip over T_MACRO_PARM
            if( macro_parms ) {
                mtok = BuildString(  macro_parms[*p].arg );
            } else {
                Buffer[0] = '\0';
                mtok = BuildAToken( Buffer );
                mtok->token = T_STRING;
            }
            ++p;
            break;
        case T_MACRO_PARM:
            ++p;
            buf[0] = *p++;
            p2 = p;
            while( *p2 == T_WHITE_SPACE ) ++p2;
            nested->substituting_parms = 1;             /* 09-nov-93 */
            if( macro_parms ) {
                mtok = BuildMTokenList( macro_parms[buf[0]].arg, NULL );
            } else {
                mtok = BuildAToken( "" );
                mtok->token = T_WHITE_SPACE;
            }
            if( *p2 != T_MACRO_SHARP_SHARP  &&
                prev_token != T_MACRO_SHARP_SHARP ) {
                if( mtok != NULL ) {
                    mtok = AppendToken( mtok, T_NULL, "P-<end of parm>" );
                    mtok = ExpandNestedMacros( mtok, 0 );
                }
            }
            nested->substituting_parms = 0;
            break;
        default:
            mtok = BuildAToken( Tokens[*p] );
            mtok->token = *p++;
            break;
        }
        if( mtok != NULL ) {
            if( mtok->token != T_WHITE_SPACE )  prev_token = mtok->token;
            *lnk = mtok;
            while( *lnk != NULL ) lnk = &(*lnk)->next;
        }
    }
    head = GlueTokens( head );
    return( head );
}

MACRO_TOKEN *MacroExpansion( int rescanning )
{
    MEPTR       mentry;
    MACRO_ARG   *macro_parms;
    MACRO_TOKEN *head;
    MACRO_TOKEN *mtok;
    NESTED_MACRO *nested;
    char __FAR  *tokens;
    int         len;

    mentry = NextMacro;
    nested = (NESTED_MACRO *)CMemAlloc( sizeof( NESTED_MACRO ) );
    nested->mentry = mentry;
    nested->rescanning = rescanning;
    nested->substituting_parms = 0;
    nested->macro_parms = NULL;
    if( mentry->macro_defn == 0 ) {     /* if special macro */
        CurToken = SpecialMacro( mentry );
        head = BuildAToken( Buffer );
        head->token = CurToken;
        nested->next = NestedMacros;
        NestedMacros = nested;
    } else {
        macro_parms = CollectParms();
        nested->next = NestedMacros;
        NestedMacros = nested;
        nested->macro_parms = macro_parms;
        tokens = (char *)mentry + mentry->macro_defn;
        head = BuildMTokenList( tokens, macro_parms );
        for( mtok = head; mtok; mtok = mtok->next ) {   /* 26-oct-93 */
            if( mtok->token == T_ID ) {
                len = strlen( mtok->data ) + 1;
                for( nested = NestedMacros; nested; nested = nested->next ) {
                    if( far_strcmp( nested->mentry->macro_name,
                                    mtok->data, len ) == 0 ) {
                        if( nested->substituting_parms == 0 ) { /* 09-nov-93*/
                            // change token so it won't be considered a
                            // candidate as a macro
                            mtok->token = T_UNEXPANDABLE_ID;
                            break;
                        }
                    }
                }
            }
        }
    }
    head = AppendToken( head, T_NULL, "Z-<end of macro>" );
    return( head );
}

MACRO_TOKEN *NestedMacroExpansion( int rescanning )
{
    return( ExpandNestedMacros( MacroExpansion( 0 ), rescanning ) );
}

void DoMacroExpansion(void)             // called from cscan
{
    MacroDepth = 0;
    TokenList = NestedMacroExpansion( 1 );
    // GetMacroToken will feed back tokens from the TokenList
    // when the TokenList is exhausted, then revert back to normal scanning
    if( TokenList == NULL ) {
        MacroPtr = NULL;
    } else {
        MacroPtr = "";
    }
}


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
* Description:  Target independent pragma processing.
*
****************************************************************************/


#include "plusplus.h"
#include <ctype.h>
#include "preproc.h"
#include "memmgr.h"
#include "cgdata.h"
#include "cgback.h"
#include "cmdline.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "template.h"
#include "pcheader.h"
#include "name.h"
#include "stack.h"
#include "ring.h"
#include "fnovload.h"
#include "cginlibs.h"
#include "cginimps.h"
#include "initdefs.h"
#include "asmstmt.h"
#include "ialias.h"
#include "cgfront.h"
// from \watcom\h
#include "rtprior.h"

#include "clibext.h"


#define PCH_LIST_TERM       ((unsigned)-1)
#define PCH_GLOBAL_PACK     ((unsigned)-1)

typedef struct {                // PRAG_EXT_REF -- extref's pragma'd
    void    *next;              // - next in ring
    SYMBOL  symbol;             // - extref symbol
    char    name[1];            // - extref name
} PRAG_EXT_REF;

static PRAG_EXT_REF *pragmaExtrefs; // ring of pragma'd extref symbols

static void init                // MODULE INITIALIZATION
    ( INITFINI* defn )
{
    /* unused parameters */ (void)defn;

    pragmaExtrefs = NULL;
}

static void fini                // MODULE COMPLETION
    ( INITFINI* defn )
{
    /* unused parameters */ (void)defn;

    RingFree( &pragmaExtrefs );
}

INITDEFN( pragma_extref, init, fini );

static bool grabNum( unsigned *val )
{
    if( CurToken == T_CONSTANT ) {
        *val = U32Fetch( Constant64 );
        NextToken();
        return( true );
    }
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( CurToken == T_CONSTANT ) {
            *val = U32Fetch( Constant64 );
            NextToken();
            MustRecog( T_RIGHT_PAREN );
            return( true );
        }
    }
    NextToken();
    return( false );
}

// forms:
//
// #pragma template_depth n
// #pragma template_depth( n )
//
// Used to set the depth, up to which, function templates will be expanded
static void pragTemplateDepth(    // PROCESS #pragma template_depth
    void )
{
    unsigned num;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( grabNum( &num ) ) {
        TemplateSetDepth( num );
    } else {
        CErr1( ERR_PRAG_TEMPLATE_DEPTH );
    }
    PPCTL_DISABLE_MACROS();
}

// forms:
//
// #pragma inline_depth n
// #pragma inline_depth( n )    -- MS compatible
//
// Used to set the depth, up to which, inlining of functions will take place.
// "n" must be number 0:255
//
static void pragInlineDepth(    // PROCESS #pragma inline_depth
    void )
{
    unsigned num;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( grabNum( &num ) && num <= MAX_INLINE_DEPTH ) {
        CgBackSetInlineDepth( num );
    } else {
        CErr1( ERR_PRAG_INLINE_DEPTH );
    }
    PPCTL_DISABLE_MACROS();
}

// forms:
//
// #pragma comment( lib, "libname.lib" )
//
static void pragComment(        // #PRAGMA COMMENT
    void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        if( PragRecog( "lib" ) ) {
            if( ExpectingToken( T_COMMA ) ) {
                NextToken();
            }
            while( IS_ID_OR_KEYWORD( CurToken ) || CurToken == T_STRING ) {
                CgInfoAddUserLib( Buffer );
                NextToken();
            }
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

static void collectStrings( VBUF *vinfo )
{
    VbufInit( vinfo );
    while( CurToken == T_STRING ) {
        VbufConcStr( vinfo, Buffer );
        NextToken();
    }
}

// forms:
//
// #pragma message( "one or more" "message string" );
//
static void pragMessage(        // #PRAGMA MESSAGE
    void )
{
    VBUF str;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        if( CurToken == T_STRING ) {
            collectStrings( &str );
            CErr2p( WARN_USER_WARNING_MSG, VbufString( &str ) );
            VbufFree( &str );
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

// forms:
//
// #pragma error "one or more" "message string" ;
//
static void pragError(          // #PRAGMA ERROR
    void )
{
    VBUF str;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_STRING ) {
        collectStrings( &str );
        CErr2p( ERR_USER_ERROR_MSG, VbufString( &str ) );
        VbufFree( &str );
    }
    PPCTL_DISABLE_MACROS();
}

// forms:
//
// #pragma inline_recursion on
// #pragma inline_recursion off
// #pragma inline_recursion(on)         -- MS compatible
// #pragma inline_recursion(off)        -- MS compatible
//
// Used to indicate whether recursive inlining is permitted or not
//
static void pragInlineRecursion( // PROCESS #pragma inline_recusrion
    void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( PragRecog( "on" ) ) {
        CgBackSetInlineRecursion( true );
    } else if( PragRecog( "off" ) ) {
        CgBackSetInlineRecursion( false );
    } else {
        if( CurToken == T_LEFT_PAREN ) {
            NextToken();
            if( PragRecog( "on" ) ) {
                CgBackSetInlineRecursion( true );
            } else if( PragRecog( "off" ) ) {
                CgBackSetInlineRecursion( false );
            } else {
                MustRecog( T_ID );
            }
            MustRecog( T_RIGHT_PAREN );
        }
    }
    PPCTL_DISABLE_MACROS();
}

// forms:
//
// #pragma code_seg ( segment [, class] )
// #pragma code_seg segment
//
// "segment" sets the default code segment for remaining functions
// "class" is ignored
//
static void pragCodeSeg(        // SET NEW CODE SEGMENT
    void )
{
    char *seg_name;
    char *seg_class;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
            seg_name = strsave( Buffer );
            seg_class = NULL;
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
                if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
                    seg_class = strsave( Buffer );
                    NextToken();
                } else {
                    MustRecog( T_STRING );
                }
            }
            SegmentCode( seg_name, seg_class );
            CMemFree( seg_name );
            CMemFree( seg_class );
        } else if( CurToken == T_RIGHT_PAREN ) {
            // restore back to default behaviour
            SegmentCode( NULL, NULL );
        }
        MustRecog( T_RIGHT_PAREN );
    } else if( CurToken == T_STRING ) {
        SegmentCode( Buffer, NULL );
        NextToken();
    }
    PPCTL_DISABLE_MACROS();
}

// forms:
//
// #pragma data_seg ( segment [, class] )
// #pragma data_seg segment
//
// "segment" sets the default data segment for remaining objects
// "class" is ignored
//
static void pragDataSeg(        // SET NEW DATA SEGMENT
    void )
{
    char *seg_name;
    char *seg_class;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
            seg_name = strsave( Buffer );
            seg_class = NULL;
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
                if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
                    seg_class = strsave( Buffer );
                    NextToken();
                } else {
                    MustRecog( T_STRING );
                }
            }
            SegmentData( seg_name, seg_class );
            CMemFree( seg_name );
            CMemFree( seg_class );
        } else if( CurToken == T_RIGHT_PAREN ) {
            // restore back to default behaviour
            SegmentData( NULL, NULL );
        }
        MustRecog( T_RIGHT_PAREN );
    } else if( CurToken == T_STRING ) {
        SegmentData( Buffer, NULL );
        NextToken();
    }
    PPCTL_DISABLE_MACROS();
}


// forms: #pragma warning # level   (change message # to have level "level)
//      : #pragma warning * level   (change all messages to have level "level)
//
// - "level" must be digit (0-9)
// - "level==0" implies warning will be treated as an error
//
static bool pragWarning(        // PROCESS #PRAGMA WARNING
    void )
{
    unsigned msgnum;            // - message number
    int level;                  // - new level
    bool change_all;            // - true ==> change all levels
    bool ignore;

    ignore = false;
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_TIMES ) {
        msgnum = 0;
        change_all = true;
    } else if( CurToken == T_CONSTANT ) {
        msgnum = U32Fetch( Constant64 );
        change_all = false;
    } else {
        // ignore; MS or other vendor's #pragma
        ignore = true;
    }
    if( !ignore ) {
        NextToken();
        if( CurToken == T_CONSTANT ) {
            level = U32Fetch( Constant64 );
            NextToken();
            if( change_all ) {
                WarnChangeLevels( level );
            } else {
                WarnChangeLevel( level, msgnum );
            }
        } else {
            CErr1( ERR_PRAG_WARNING_BAD_LEVEL );
            NextToken();
        }
    }
    PPCTL_DISABLE_MACROS();
    return( ignore );
}


// forms:
//
// #pragma enable_message( # )
//
static void pragEnableMessage(  // ENABLE WARNING MESSAGE
    void )
{
    unsigned msgnum;
    bool error_occurred;

    error_occurred = false;
    PPCTL_ENABLE_MACROS();
    NextToken();
    MustRecog( T_LEFT_PAREN );
    for( ;; ) {
        if( !grabNum( &msgnum ) ) {
            CErr1( ERR_PRAG_ENABLE_MESSAGE );
            error_occurred = true;
        }

        // Enable message by setting its level to the lowest possible value.
        if( !error_occurred ) {
            WarnChangeLevel( WLEVEL_ENABLE, msgnum );
        }

        if( CurToken != T_COMMA ) {
            break;
        }
        NextToken();
    }
    MustRecog( T_RIGHT_PAREN );
    PPCTL_DISABLE_MACROS();
}


// forms:
//
// #pragma disable_message( # )
//
static void pragDisableMessage( // DISABLE WARNING MESSAGE
    void )
{
    unsigned msgnum;
    bool error_occurred;

    error_occurred = false;
    PPCTL_ENABLE_MACROS();
    NextToken();
    MustRecog( T_LEFT_PAREN );
    for( ;; ) {
        if( !grabNum( &msgnum ) ) {
            CErr1( ERR_PRAG_DISABLE_MESSAGE );
            error_occurred = true;
        }

        // Disable message by setting its level to the highest possible value.
        if( !error_occurred ) {
            WarnChangeLevel( WLEVEL_DISABLE, msgnum );
        }

        if( CurToken != T_COMMA ) {
            break;
        }
        NextToken();
    }
    MustRecog( T_RIGHT_PAREN );
    PPCTL_DISABLE_MACROS();
}

static void endOfPragma(
    void )
{
    if( CurToken == T_SEMI_COLON )
        NextToken();
    ExpectingToken( T_NULL );
    while( CurToken != T_NULL && CurToken != T_EOF ) {
        NextToken();
    }
}

void PragmaSetToggle(           // SET TOGGLE
    bool set_flag )             // - true ==> set flag
{
    #define toggle_pick( x ) \
        if( strcmp( Buffer, #x ) == 0 ) {       \
            PragDbgToggle.x = set_flag;         \
            return;                             \
        }
    #include "dbgtogg.h"
    #undef toggle_pick

    #define toggle_pick( x ) \
        if( strcmp( Buffer, #x ) == 0 ) {       \
            PragToggle.x = set_flag;            \
            return;                             \
        }
    #include "tognam.h"
    #undef toggle_pick
}

const char *SkipUnderscorePrefix( const char *str, size_t *len )
/**************************************************************/
{
    const char  *start;

    start = str;
    if( *str == '_' ) {
        str++;
        if( *str == '_' ) {
            str++;
        }
    }
    if( len != NULL ) {
        *len -= str - start;
    }
    return( str );
}

static bool PragIdRecog(        // RECOGNIZE PRAGMA ID
    const char *what )          // - id
{
    bool ok;

    ok = ( stricmp( SkipUnderscorePrefix( Buffer, NULL ), what ) == 0 );
    if( ok ) {
        NextToken();
    }
    return( ok );
}

bool PragRecog(                 // RECOGNIZE PRAGMA ID
    const char *what )          // - id
{
    if( IS_ID_OR_KEYWORD( CurToken ) ) {
        return( PragIdRecog( what ) );
    }
    return( false );
}

// forms:
//
// #pragma on (toggle)
// #pragma off (toggle)
//
static void pragFlag(           // SET TOGGLES
    bool set_flag )             // - true ==> set flag
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        while( IS_ID_OR_KEYWORD( CurToken ) ) {
            PragmaSetToggle( set_flag );
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}


// forms: #pragma initialize [before/after]* priority
//
//  where priority is:
//      - number 0 - 255
//      - library (32)
//      - program (64)
//
// The pragma is used to set the initialize priority for the module ( when
// it occurs at file scope) or to specify an initialization function to
// be called (when it occurs within a function).
//
// The function must be previously declared to have no parameters and to
// be static.
//
static void pragInitialize(     // #pragma initialize ...
    void )
{
    int adjust;                 // - before/after adjustment
    unsigned priority;          // - initialization priority
    unsigned test;

    adjust = 0;
    PPCTL_ENABLE_MACROS();
    NextToken();
    for( ;; ) {
        /* allow "before before library" */
        if( PragRecog( "after" ) ) {
            ++adjust;
        } else if( PragRecog( "before" ) ) {
            --adjust;
        } else {
            break;
        }
    }
    priority = INIT_PRIORITY_PROGRAM;
    if( CurToken == T_CONSTANT ) {
        test = U32Fetch( Constant64 );
        if( test <= 255 ) {
            priority = test;
        } else {
            CErr1( ERR_PRAG_INITIALIZE_PRIORITY );
        }
    } else if( PragRecog( "library" ) ) {
        priority = INIT_PRIORITY_LIBRARY;
    } else if( PragRecog( "program" ) ) {
        priority = INIT_PRIORITY_PROGRAM;
    } else {
        CErr1( ERR_PRAG_INITIALIZE_PRIORITY );
    }
    NextToken();
    PPCTL_DISABLE_MACROS();
    priority += adjust;
    if( priority > 255 ) {
        CErr1( ERR_PRAG_INITIALIZE_PRIORITY );
        priority = INIT_PRIORITY_PROGRAM;
    }
    CompInfo.init_priority = (unsigned char)priority;
}

static void pushPrag( PRAG_STACK **h, unsigned value )
{
    PRAG_STACK *stack_entry;

    stack_entry = StackPop( &FreePrags );
    if( stack_entry == NULL ) {
        stack_entry = CPermAlloc( sizeof( *stack_entry ) );
    }
    stack_entry->value = value;
    StackPush( h, stack_entry );
}

static bool popPrag( PRAG_STACK **h, unsigned *pvalue )
{
    PRAG_STACK *pack_entry;

    pack_entry = StackPop( h );
    if( pack_entry != NULL ) {
        if( pvalue != NULL ) {
            *pvalue = pack_entry->value;
        }
        StackPush( &FreePrags, pack_entry );
        return( true );
    }
    return( false );
}

// forms: (1) #pragma enum int
//        (2) #pragma enum minimum
//        (3) #pragma enum original
//        (4) #pragma enum pop
//
// The pragma affects the underlying storage-definition for subsequent
// enum declarations.
//
// (1) make int the underlying storage definition (same as -ei)
// (2) minimize the underlying storage definition (same as no -ei)
// (3) reset back to result of command-line parsing
// (4) restore previous value
//
// 1-3 all push previous value before affecting value
//
static void pragEnum            // #pragma enum PARSING
    ( void )
{
    unsigned make_enums_an_int;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( PragRecog( "int" ) ) {
        pushPrag( &HeadEnums, CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = true;
    } else if( PragRecog( "minimum" ) ) {
        pushPrag( &HeadEnums, CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = false;
    } else if( PragRecog( "original" ) ) {
        pushPrag( &HeadEnums, CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = CompFlags.original_enum_setting;
    } else if( PragRecog( "pop" ) ) {
        if( popPrag( &HeadEnums, &make_enums_an_int ) ) {
            CompFlags.make_enums_an_int = ( make_enums_an_int != 0 );
        }
    }
    PPCTL_DISABLE_MACROS();
}


// forms: #pragma init_seg({compiler|lib|user})
//
//      - compiler (16)
//      - library (32)
//      - user (64)
//
static void pragInitSeg(     // #pragma init_seg ...
    void )
{
    unsigned priority;

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        if( PragRecog( "compiler" ) ) {
            priority = INIT_PRIORITY_LIBRARY - 1;
        } else if( PragRecog( "lib" ) ) {
            priority = INIT_PRIORITY_LIBRARY;
        } else {
            priority = INIT_PRIORITY_PROGRAM;
            MustRecog( T_ID );
        }
        CompInfo.init_priority = (unsigned char)priority;
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}


static void parseExtRef(     // PARSE SYMBOL NAME
    void )
{
    PRAG_EXT_REF *entry;

    if( CurToken == T_STRING ) {
        entry = RingAlloc( &pragmaExtrefs, offsetof( PRAG_EXT_REF, name ) + TokenLen + 1 );
        memcpy( entry->name, Buffer, TokenLen + 1 );
        entry->symbol = NULL;
    } else if( IS_ID_OR_KEYWORD( CurToken ) ) {
        SEARCH_RESULT* result;
        NAME name = NameCreateLen( Buffer, TokenLen );

        result = ScopeFindNaked( GetCurrScope(), name );
        if( result == NULL ) {
            CErr2p( ERR_PRAG_EXTREF_NONE, Buffer );
        } else {
            SYMBOL_NAME sname = result->sym_name;
            SYMBOL sym = sname->name_syms;
            ScopeFreeResult( result );
            if( sym == NULL ) {
                CErr2p( ERR_PRAG_EXTREF_BAD, Buffer );
            } else if( SymIsFunction( sym ) ) {
                if( IsOverloadedFunc( sym ) ) {
                    CErr2p( ERR_PRAG_EXTREF_OVERLOADED, sym );
                    sym = NULL;
                }
            } else if( SymIsData( sym ) ) {
                // no checks now
            } else {
                CErr2p( ERR_PRAG_EXTREF_BAD, sym );
                sym = NULL;
            }
            if( sym != NULL ) {
                entry = RingAlloc( &pragmaExtrefs, offsetof( PRAG_EXT_REF, name ) + 1 );
                entry->symbol = sym;
                entry->name[0] = '\0';
            }
        }
    }
}


// forms: #pragma extref( symbol [, ...] )
//        #pragma extref( "name" [, ...] )
//
// causes a external reference to be emitted for the symbol or "name"
//
static void pragExtRef(
    void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_LEFT_PAREN ) {
        do {
            NextToken();
            if( !IS_ID_OR_KEYWORD( CurToken ) && CurToken != T_STRING )
                break;
            parseExtRef();
            NextToken();
        } while( CurToken == T_COMMA );
        MustRecog( T_RIGHT_PAREN );
    } else if( IS_ID_OR_KEYWORD( CurToken ) || CurToken == T_STRING ) {
        parseExtRef();
        NextToken();
    }
    PPCTL_DISABLE_MACROS();
}


void PragmaExtrefsValidate      // VALIDATE EXTREFS FOR PRAGMAS
    ( void )
{
    PRAG_EXT_REF* entry;        // - current entry

    RingIterBeg( pragmaExtrefs, entry ) {
        if( entry->symbol != NULL ) {
            SYMBOL symbol = entry->symbol;
            if( SymIsExtern( symbol ) ) {
                if( IsOverloadedFunc( symbol ) ) {
                    CErr2p( ERR_PRAG_EXTREF_OVERLOADED, symbol );
                    entry->symbol = NULL;
                }
            } else {
                CErr2p( ERR_PRAG_EXTREF_EXTERN, symbol );
                entry->symbol = NULL;
            }
        }
    } RingIterEnd( entry );
}


void PragmaExtrefsInject        // INJECT EXTREFS FOR PRAGMAS
    ( void )
{
    PRAG_EXT_REF* entry;        // - current entry

    RingIterBeg( pragmaExtrefs, entry ) {
        if( entry->symbol != NULL ) {
            CgInfoAddPragmaExtrefS( entry->symbol );
        } else {
            CgInfoAddPragmaExtrefN( entry->name );
        }
    } RingIterEnd( entry );
}


// forms:
//
// #pragma intrinsic (...)
//
static void pragIntrinsic(      // SET FUNCTIONS TO BE (NOT TO BE) INTRINSIC
    bool intrinsic )            // - true ==> function to be intrinsic
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        while( IS_ID_OR_KEYWORD( CurToken ) ) {
            ScopeIntrinsic( intrinsic );
            NextToken();
            if( CurToken != T_COMMA )
                break;
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}


// forms:
//
// #pragma dump_object_model (...)
//
static void pragDumpObjectModel( // DUMP OBJECT MODEL
    void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( IS_ID_OR_KEYWORD( CurToken ) ) {
        NAME name = NameCreateLen( Buffer, TokenLen );
        SEARCH_RESULT* result = ScopeFindNaked( GetCurrScope(), name );
        if( result == NULL ) {
            CErr2p( ERR_DUMP_OBJ_MODEL, Buffer );
        } else {
            SYMBOL_NAME sname = result->sym_name;
            ScopeFreeResult( result );
            if( sname->name_syms == NULL ) {
                TYPE type = sname->name_type->sym_type;
                if( TypeDefined( type ) ) {
                    TYPE dump_type;
                    dump_type = StructType( type );
                    if( dump_type != NULL ) {
                        DumpObjectModelClass( dump_type );
                    } else {
                        dump_type = EnumType( type );
                        if( dump_type != NULL ) {
                            DumpObjectModelEnum( dump_type );
                        } else {
                            CErr2p( ERR_DUMP_OBJ_MODEL, Buffer );
                        }
                    }
                } else {
                    CErr2p( ERR_DUMP_OBJ_MODEL, Buffer );
                }
            } else {
                CErr2p( ERR_DUMP_OBJ_MODEL, Buffer );
            }
        }
        NextToken();
    }
    PPCTL_DISABLE_MACROS();
}


static DT_METHOD parseDtorMethod( // PARSE A DESTRUCTOR METHOD
    void )
{
    DT_METHOD method;           // - method specified

    if( PragRecog( "direct" ) ) {
        method = DTM_DIRECT;
    } else if( PragRecog( "small" ) ) {
        method = DTM_TABLE_SMALL;
    } else if( PragRecog( "table" ) ) {
        method = DTM_TABLE;
    } else {
        method = DTM_COUNT;
    }
    return( method );
}


// forms:
//
// #pragma destruct (...)
//
static void pragDestruct(       // SPECIFY DESTRUCTION MECHANISM
    void )
{
    DT_METHOD method;           // - method
    DT_METHOD next;             // - next method spec

    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        method = DTM_COUNT;
        for( ;; ) {
            if( CurToken == T_RIGHT_PAREN ) {
                NextToken();
                break;
            }
            next = parseDtorMethod();
            switch( next ) {
            default :
                method = DTM_COUNT;
                break;
            case DTM_DIRECT :
                switch( method ) {
                case DTM_TABLE :
                    method = DTM_DIRECT_TABLE;
                    continue;
                case DTM_TABLE_SMALL :
                    method = DTM_DIRECT_SMALL;
                    continue;
                case DTM_COUNT :
                    method = next;
                    continue;
                default :
                    continue;
                }
            case DTM_TABLE :
                switch( method ) {
                case DTM_DIRECT :
                    method = DTM_DIRECT_TABLE;
                    continue;
                case DTM_COUNT :
                    method = next;
                    continue;
                default :
                    continue;
                }
            case DTM_TABLE_SMALL :
                switch( method ) {
                case DTM_DIRECT :
                    method = DTM_DIRECT_SMALL;
                    continue;
                case DTM_DIRECT_TABLE :
                case DTM_TABLE :
                    method = DTM_TABLE_SMALL;
                    continue;
                case DTM_COUNT :
                    method = next;
                    continue;
                default :
                    continue;
                }
            }
            break;
        }
    } else {
        method = parseDtorMethod();
    }
    if( method != DTM_COUNT ) {
        CompInfo.dt_method_speced = method;
        CompFlags.dt_method_pragma = true;
    }
    PPCTL_DISABLE_MACROS();
}

#ifndef NDEBUG
// forms:
//
// #pragma break
//
static void pragBreak()
{
    PPCTL_ENABLE_MACROS();
    NextToken();
  #if defined( __WATCOMC__ )
    __trap();
  #endif
    PPCTL_DISABLE_MACROS();
}
#endif


// forms: (1) #pragma read_only_file
//        (2) #pragma read_only_file "file"*
//
// (1) causes current file to be marked read-only
// (2) causes indicated file to be marked read-only
//      - file must have started inclusion (may have completed)
//
static void pragReadOnlyFile
    ( void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_STRING ) {
        do {
            SrcFileReadOnlyFile( Buffer );
            NextToken();
            if( CurToken == T_SEMI_COLON ) {
                NextToken();
            }
        } while( CurToken == T_STRING );
    } else {
        SrcFileReadOnlyFile( NULL );
    }
    PPCTL_DISABLE_MACROS();
}


// form: #pragma read_only_directory "directory"*
//
// (1) causes all files within directory to be marked read-only
//
static void pragReadOnlyDir
    ( void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    while( CurToken == T_STRING ) {
        SrcFileReadOnlyDir( Buffer );
        NextToken();
        if( CurToken == T_SEMI_COLON ) {
            NextToken();
        }
    }
    PPCTL_DISABLE_MACROS();
}

// form: #pragma include_alias( "alias_name", "real_name" )
//       #pragma include_alias( <alias_name>, <real_name> )
//
// (1) causes include directives referencing alias_name to refer
//     to real_name instead
//
static void pragIncludeAlias( void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        if( CurToken == T_STRING ) {
            char    *alias_name;

            alias_name = CMemAlloc( strlen( Buffer ) + 1 );
            strcpy( alias_name, Buffer );
            NextToken();
            if( ExpectingToken( T_COMMA ) ) {
                NextToken();
            }
            if( CurToken == T_STRING ) {
                IAliasAdd( alias_name, Buffer, false );
                NextToken();
            }
            CMemFree( alias_name );
        } else if( CurToken == T_LT ) {
            char    a_buf[82];
            char    r_buf[82];

            a_buf[0] = '\0';
            for( ;; ) {
                NextToken();
                if( CurToken == T_GT ) {
                    NextToken();
                    break;
                }
                if( CurToken == T_NULL ) {
                    break;
                }
                strncat( a_buf, Buffer, sizeof( a_buf ) - 2 );
            }
            if( ExpectingToken( T_COMMA ) ) {
                NextToken();
            }
            if( CurToken == T_LT ) {
                r_buf[0] = '\0';
                for( ;; ) {
                    NextToken();
                    if( CurToken == T_GT ) {
                        NextToken();
                        break;
                    }
                    if( CurToken == T_NULL ) {
                        break;
                    }
                    strncat( r_buf, Buffer, sizeof( r_buf ) - 2 );
                }
                IAliasAdd( a_buf, r_buf, true );
            }
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

// form: #pragma once
//
// (1) current source file will never be #include'd again
static void pragOnce( void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    SrcFileOnceOnly();
    PPCTL_DISABLE_MACROS();
}

// forms:
//
// #pragma library ( lib ... lib )
//
static void pragLibs(           // #PRAGMA library ( lib ... lib )
    void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        while( IS_ID_OR_KEYWORD( CurToken ) || CurToken == T_STRING ) {
            CgInfoAddUserLib( Buffer );
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    } else {
        CompFlags.pragma_library = true;
    }
    PPCTL_DISABLE_MACROS();
}

// forms:
//
// #pragma pack()
// #pragma pack( <number> )
// #pragma pack( pop )
// #pragma pack( push )
// #pragma pack( push, <number> )
//
static void pragPack(           // #PRAGMA PACK
    void )
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        switch( CurToken ) {
        case T_ID:
            if( PragRecog( "pop" ) ) {
                popPrag( &HeadPacks, &PackAmount );
            } else if( PragRecog( "push" ) ) {
                if( CurToken == T_RIGHT_PAREN ) {
                    pushPrag( &HeadPacks, PackAmount );
                } else {
                    if( ExpectingToken( T_COMMA ) ) {
                        NextToken();
                    }
                    if( CurToken == T_CONSTANT ) {
                        pushPrag( &HeadPacks, PackAmount );
                        PackAmount = VerifyPackAmount( U32Fetch( Constant64 ) );
                        NextToken();
                    } else {
                        MustRecog( T_CONSTANT );
                    }
                }
            } else {
                CErr( ERR_EXPECTING_BUT_FOUND, "push or pop", Buffer );
            }
            break;
        case T_CONSTANT:
            PackAmount = VerifyPackAmount( U32Fetch( Constant64 ) );
            NextToken();
            break;
        case T_RIGHT_PAREN:
            PackAmount = GblPackAmount;
            break;
        default:
            break;
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

static void optionPragSTDC( void )
/********************************/
{
    if( PragRecog( "ON" ) ) {
    } else if( PragRecog( "OFF" ) ) {
    } else if( PragRecog( "DEFAULT" ) ) {
    }
}

// forms:
//
// #pragma STDC (FP_CONTRACT|FENV_ACCESS|CX_LIMITED_RANGE) (ON|OFF|DEFAULT)
//
static void pragSTDC( void )
/**************************/
{
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( PragRecog( "FP_CONTRACT" ) ) {
        optionPragSTDC();
    } else if( PragRecog( "FENV_ACCESS" ) ) {
        optionPragSTDC();
    } else if( PragRecog( "CX_LIMITED_RANGE" ) ) {
        optionPragSTDC();
    }
    PPCTL_DISABLE_MACROS();
}


void CPragma( void )                  // PROCESS A PRAGMA
{
    bool check_end = true;

#define pragmaNameRecog(what)   (strcmp(Buffer, what) == 0)

    SrcFileGuardStateSig();
    CompFlags.in_pragma = true;
    NextToken();
    if( IS_ID_OR_KEYWORD( CurToken ) && pragmaNameRecog( "include_alias" ) ) {
        pragIncludeAlias();
    } else if( CompFlags.cpp_output ) {
        fprintf( CppFile, "#pragma " );
        if( CurToken != T_NULL ) {
            PrtToken();
            PPCTL_ENABLE_MACROS();
            GetNextToken();
            for( ; CurToken != T_NULL; ) {
                PrtToken();
                GetNextToken();
            }
            PPCTL_DISABLE_MACROS();
        }
    } else if( IS_ID_OR_KEYWORD( CurToken ) ) {
        if( pragmaNameRecog( "on" ) ) {
            pragFlag( true );
        } else if( pragmaNameRecog( "off" ) ) {
            pragFlag( false );
        } else if( pragmaNameRecog( "aux" ) || pragmaNameRecog( "linkage" ) ) {
            PragAux();
        } else if( pragmaNameRecog( "library" ) ) {
            pragLibs();
        } else if( pragmaNameRecog( "once" ) ) {
            pragOnce();
        } else if( pragmaNameRecog( "extref" ) ) {
            pragExtRef();
        } else if( pragmaNameRecog( "comment" ) ) {
            pragComment();
        } else if( pragmaNameRecog( "pack" ) ) {
            pragPack();
        } else if( pragmaNameRecog( "warning" ) ) {
            if( pragWarning() ) {
                /* ignore #pragma warning */
                /* skip rest of line */
                check_end = false;
            }
        } else if( pragmaNameRecog( "enable_message" ) ) {
            pragEnableMessage();
        } else if( pragmaNameRecog( "disable_message" ) ) {
            pragDisableMessage();
        } else if( pragmaNameRecog( "code_seg" ) ) {
            pragCodeSeg();
        } else if( pragmaNameRecog( "data_seg" ) ) {
            pragDataSeg();
        } else if( pragmaNameRecog( "initialize" ) ) {
            pragInitialize();
        } else if( pragmaNameRecog( "init_seg" ) ) {
            pragInitSeg();
        } else if( pragmaNameRecog( "inline_depth" ) ) {
            pragInlineDepth();
        } else if( pragmaNameRecog( "template_depth" ) ) {
            pragTemplateDepth();
        } else if( pragmaNameRecog( "inline_recursion" ) ) {
            pragInlineRecursion();
        } else if( pragmaNameRecog( "intrinsic" ) ) {
            pragIntrinsic( true );
        } else if( pragmaNameRecog( "function" ) ) {
            pragIntrinsic( false );
        } else if( pragmaNameRecog( "destruct" ) ) {
            pragDestruct();
        } else if( pragmaNameRecog( "enum" ) ) {
            pragEnum();
        } else if( pragmaNameRecog( "dump_object_model" ) ) {
            pragDumpObjectModel();
        } else if( pragmaNameRecog( "read_only_file" ) ) {
            pragReadOnlyFile();
        } else if( pragmaNameRecog( "read_only_directory" ) ) {
            pragReadOnlyDir();
        } else if( pragmaNameRecog( "message" ) ) {
            pragMessage();
        } else if( pragmaNameRecog( "error" ) ) {
            pragError();
        } else if( pragmaNameRecog( "STDC" ) ) {
            pragSTDC();
#ifndef NDEBUG
        } else if( pragmaNameRecog( "break" ) ) {
            pragBreak();
#endif
        } else {                /* unknown pragma */
            check_end = false;  /* skip rest of line */
        }
    } else {                    /* unknown pragma */
        check_end = false;      /* skip rest of line */
    }
    if( check_end ) {
        endOfPragma();
    }
    CompFlags.in_pragma = false;

#undef pragmaNameRecog
}


void PragInit(
    void )
{
    WatcallInfo.use = 2;

    CdeclInfo    = WatcallInfo;
    PascalInfo   = WatcallInfo;
    FortranInfo  = WatcallInfo;
    SyscallInfo  = WatcallInfo;
    OptlinkInfo  = WatcallInfo;
    StdcallInfo  = WatcallInfo;
    FastcallInfo = WatcallInfo;

    CompInfo.init_priority = INIT_PRIORITY_PROGRAM;
}


static struct {
    const char  *name;
    AUX_INFO    *info;
} magicWords[] = {
    #define pick( a, b, c ) { b , c },
    #include "auxinfo.h"
    #undef pick
};

static magic_word_idx lookupMagicKeyword(   // LOOKUP A MAGIC KEYWORD
    const char *name )                      // - name to be looked up
{
    magic_word_idx  i;

    name = SkipUnderscorePrefix( name, NULL );
    for( i = 0; i < M_UNKNOWN; i++ ) {
        if( strcmp( magicWords[i].name + 2, name ) == 0 ) {
            break;
        }
    }
    return( i );
}

static const char *retrieveName( magic_word_idx m_type )
{
    return( magicWords[m_type].name );
}


#if _INTEL_CPU
static magic_word_idx MagicKeyword( // LOOKUP A MAGIC KEYWORD FROM BUFFER
    void )
{
    return( lookupMagicKeyword( Buffer ) );
}
#endif

static AUX_INFO *MagicKeywordInfo(  // LOOKUP A MAGIC KEYWORD FROM BUFFER
    void )
{
    return( magicWords[lookupMagicKeyword( Buffer )].info );
}


void CreateAux(                 // CREATE AUX ID
    const char *id )            // - id
{
    CurrEntry = CMemAlloc( offsetof( AUX_ENTRY, name ) + strlen( id ) + 1 );
    strcpy( CurrEntry->name, id );
    CurrInfo = CMemAlloc( sizeof( *CurrInfo ) );
    // AuxCopy assumes destination is valid
    CurrInfo->parms = NULL;
    CurrInfo->objname = NULL;
    CurrInfo->code = NULL;
}


static bool setAuxInfo(         // SET CURRENT INFO. STRUCTURE
    magic_word_idx m_type,      // - type to be set
    bool create_new )           // - true if we want a new aux_info
{
    bool found;

    found = true;
    CurrInfo = magicWords[m_type].info;
    if( CurrInfo == NULL ) {
        if( create_new ) {
            CreateAux( Buffer );
        } else {
            found = false;
        }
    }
    return( found );
}

bool PragmaName( AUX_INFO *pragma, const char **id )
/**************************************************/
{
    *id = NULL;
    if( pragma == &DefaultInfo ) {
        return( true );
    }
    if( pragma == &CdeclInfo ) {
        *id = retrieveName( M_CDECL );
    } else if( pragma == &PascalInfo ) {
        *id = retrieveName( M_PASCAL );
    } else if( pragma == &FortranInfo ) {
        *id = retrieveName( M_FORTRAN );
    } else if( pragma == &SyscallInfo ) {
        *id = retrieveName( M_SYSCALL );
    } else if( pragma == &OptlinkInfo ) {
        *id = retrieveName( M_OPTLINK );
    } else if( pragma == &StdcallInfo ) {
        *id = retrieveName( M_STDCALL );
    } else if( pragma == &FastcallInfo ) {
        *id = retrieveName( M_FASTCALL );
    } else if( pragma == &WatcallInfo ) {
        *id = retrieveName( M_WATCALL );
    }
    if( *id != NULL ) {
        return( true );
    }
    *id = AuxRetrieve( pragma );
    return( false );
}


#if _INTEL_CPU
void SetCurrInfo(               // SET CURRENT INFO. STRUCTURE
    void )
{
    setAuxInfo( MagicKeyword(), true );
}
#endif


void PragCurrAlias(             // LOCATE ALIAS FOR PRAGMA
    void )
{
    AUX_ENTRY *search;

    search = NULL;
    CurrAlias = MagicKeywordInfo();
    if( CurrAlias == NULL ) {
        search = AuxLookup( Buffer );
        if( search != NULL ) {
            CurrAlias = search->info;
        } else {
            CurrAlias = &DefaultInfo;
        }
    }
}


static void copyParms(           // COPY PARMS PORTION
    void )
{
    if( CurrInfo->parms != CurrAlias->parms ) {
        /* new parms have already been allocated */
        return;
    }
    if( !IsAuxParmsBuiltIn( CurrInfo->parms ) ) {
        CurrInfo->parms = AuxParmDup( CurrInfo->parms );
    }
}

static void copyObjName(         // COPY OBJECT PORTION
    void )
{
    char *objname;

    objname = CurrInfo->objname;
    if( objname != NULL && objname == CurrAlias->objname ) {
        CurrInfo->objname = AuxObjnameDup( objname );
    }
}

void PragEnding(                // PROCESS END OF PRAGMA
    bool set_sym )              // - true ==> set SYMBOL's aux_info
{
    if( CurrEntry != NULL ) {
        if( CurrAlias != NULL ) {
            CurrInfo->use = CurrAlias->use;
            if( memcmp( CurrAlias, CurrInfo, sizeof( AUX_INFO ) ) == 0 ) {
                CurrEntry->info = CurrAlias;
                CurrAlias->use++;
                CMemFree( CurrInfo );
            } else {
                copyParms();
                AsmSysCopyCode();
                copyObjName();
                CurrInfo->use = 1;
                CurrEntry->info = CurrInfo;
            }
        }
        CurrEntry->next = AuxList;
        AuxList = CurrEntry;
        if( set_sym ) {
            ScopeAuxName( CurrEntry->name, CurrEntry->info );
        }
    }
}


#if _INTEL_CPU
void PragObjNameInfo(           // RECOGNIZE OBJECT NAME INFORMATION
    void )
{
    if( CurToken == T_STRING ) {
        CMemFree( CurrInfo->objname );
        CurrInfo->objname = strsave( Buffer );
        NextToken();
    }
}
#endif


AUX_INFO *PragmaLookup( const char *name, magic_word_idx index )
/**************************************************************/
{
    AUX_ENTRY *ent;

    if( index != M_UNKNOWN ) {
        if( setAuxInfo( index, false ) ) {
            return( CurrInfo );
        }
    }
    if( setAuxInfo( lookupMagicKeyword( name ), false ) ) {
        return( CurrInfo );
    }
    ent = AuxLookup( name );
    if( ent == NULL ) {
        return( NULL );
    }
    return( ent->info );
}


AUX_INFO *GetTargetHandlerPragma    // GET PRAGMA FOR FS HANDLER
    ( void )
{
    AUX_INFO *prag;

    prag = NULL;
    switch( TargetSystem ) {
    case TS_NT:
        prag = &CdeclInfo;
        break;
    case TS_OS2:
        prag = &SyscallInfo;
        break;
    }
    return( prag );
}


int PragRegIndex( const char *registers, const char *name, size_t len, bool ignorecase )
/**************************************************************************************/
{
    int             index;
    const char      *p;
    unsigned char   c;
    unsigned char   c2;
    size_t          i;

    index = 0;
    for( p = registers; *p != '\0'; ) {
        i = 0;
        while( (c = *p++) != '\0' ) {
            if( i > len )
                continue;
            if( i < len ) {
                c2 = name[i++];
                if( c == c2 || ignorecase && tolower( c ) == tolower( c2 ) ) {
                    continue;
                }
            }
            i = len + 1;
        }
        if( i == len )
            return( index );
        ++index;
    }
    return( -1 );
}

int PragRegNumIndex( const char *str, int max_reg )
/*************************************************/
{
    int             index;

    // decode regular register index
    if( isdigit( *str ) ) {
        index = atoi( str );
        if( index < max_reg ) {
            if( str[1] == '\0' ) {
                //  0....9
                if(( index > 0 )
                  || ( index == 0 ) && ( str[0] == '0' )) {
                    return( index );
                }
            } else if( str[2] == '\0' ) {
                // 10....max_reg-1
                if( index > 9 ) {
                    return( index );
                }
            }
        }
    }
    return( -1 );
}


#if _INTEL_CPU
hw_reg_set PragRegList(         // GET PRAGMA REGISTER SET
    void )
{
    hw_reg_set res;             // - resultant set
    hw_reg_set reg;             // - current set
    TOKEN close_token;          // - ending delimiter

    HW_CAsgn( res, HW_EMPTY );
    if( CurToken == T_LEFT_BRACKET ) {
        close_token = T_RIGHT_BRACKET;
    } else if( CurToken == T_LEFT_BRACE ) {
        close_token = T_RIGHT_BRACE;
    } else {
        return( res );
    }
    NextToken();
    for( ; CurToken != close_token; ) {
        reg = PragRegName( Buffer, strlen( Buffer ) );
        HW_TurnOn( res, reg );
        NextToken();
    }
    MustRecog( close_token );
    return( res );
}
#endif


#if _INTEL_CPU
void PragManyRegSets(           // GET PRAGMA REGISTER SETS
    void )
{
    hw_reg_set  buff[MAXIMUM_PARMSETS];
    int         i;
    hw_reg_set  list;
    hw_reg_set  *sets;

    for( i = 0, list = PragRegList(); !HW_CEqual( list, HW_EMPTY ); list = PragRegList(), ++i ) {
        if( i == MAXIMUM_PARMSETS ) {
            CErr1( ERR_TOO_MANY_PARM_SETS );
            break;
        }
        buff[i] = list;
    }
    HW_CAsgn( buff[i], HW_EMPTY );
    i = ( i + 1 ) * sizeof( *sets );
    sets = (hw_reg_set *)CMemAlloc( i );
    memcpy( sets, buff, i );
    if( !IsAuxParmsBuiltIn( CurrInfo->parms ) ) {
        CMemFree( CurrInfo->parms );
    }
    CurrInfo->parms = sets;
}
#endif


#if 0
struct textsegment *LkSegName(  // LOOKUP SEGMENT NAME
    char *name )                // - segment name
{
    struct textsegment  *seg;

    for( seg = TextSegList; seg; seg = seg->next ) {
        if( strcmp( seg->segname, name ) == 0 ) {
            return( seg );
        }
    }
    seg = CMemAlloc( sizeof( struct textsegment ) );
    seg->next = TextSegList;
    seg->segname = strsave( name );
    seg->segment_number = 0;
    TextSegList = seg;
    return( seg );
}
#endif


bool ReverseParms( AUX_INFO *pragma )
/***********************************/
{
    if( pragma->cclass & REVERSE_PARMS ) {
        return( true );
    }
    return( false );
}

bool GetPragAuxAliasInfo( void )
/******************************/
{
    char buff[256];

    CurrAlias = &DefaultInfo;
    if( CurToken != T_LEFT_PAREN )              // #pragma aux symbol ....
        return( IS_ID_OR_KEYWORD( CurToken ) );
    NextToken();
    if( !IS_ID_OR_KEYWORD( CurToken ) )         // error
        return( false );
    PragCurrAlias();
    strcpy( buff, Buffer );
    NextToken();
    if( CurToken == T_RIGHT_PAREN ) {           // #pragma aux (alias) symbol ....
        NextToken();
        return( IS_ID_OR_KEYWORD( CurToken ) );
    }
    if( CurToken == T_COMMA ) {                 // #pragma aux (alias, symbol)
        NextToken();
        if( IS_ID_OR_KEYWORD( CurToken ) ) {
            CreateAux( buff );
            GetPragAuxAlias();
            PragEnding( true );
        }
    }
    return( false );
}

static void writePacks( void )
{
    PRAG_STACK *pack_entry;
    PRAG_STACK *reversed_packs;
    unsigned pack_amount;

    reversed_packs = NULL;
    for( ;; ) {
        pack_entry = StackPop( &HeadPacks );
        if( pack_entry == NULL )
            break;
        StackPush( &reversed_packs, pack_entry );
    }
    for( ;; ) {
        pack_entry = StackPop( &reversed_packs );
        if( pack_entry == NULL )
            break;
        pack_amount = pack_entry->value;
        PCHWriteUInt( pack_amount );
        StackPush( &HeadPacks, pack_entry );
    }
    pack_amount = PCH_GLOBAL_PACK;
    PCHWriteUInt( pack_amount );
    if( HeadPacks != NULL ) {
        pack_amount = PackAmount;
    } else if( PackAmount != GblPackAmount ) {
        pack_amount = PackAmount;
    }
    PCHWriteUInt( pack_amount );
}

static void readPacks( void )
{
    unsigned pack_amount;

    while( HeadPacks != NULL ) {
        popPrag( &HeadPacks, &PackAmount );
    }
    for( ; (pack_amount = PCHReadUInt()) != PCH_LIST_TERM; ) {
        pushPrag( &HeadPacks, pack_amount );
    }
    pack_amount = PCHReadUInt();
    if( pack_amount == PCH_GLOBAL_PACK ) {
        pack_amount = GblPackAmount;
    }
    PackAmount = pack_amount;
}

static void writeEnums( void )
{
    PRAG_STACK *enum_entry;
    PRAG_STACK *reversed_enums;
    unsigned enum_int;

    reversed_enums = NULL;
    for( ;; ) {
        enum_entry = StackPop( &HeadPacks );
        if( enum_entry == NULL )
            break;
        StackPush( &reversed_enums, enum_entry );
    }
    for( ;; ) {
        enum_entry = StackPop( &reversed_enums );
        if( enum_entry == NULL )
            break;
        enum_int = enum_entry->value;
        PCHWriteUInt( enum_int );
        StackPush( &HeadPacks, enum_entry );
    }
    enum_int = PCH_LIST_TERM;
    PCHWriteUInt( enum_int );
    PCHWriteUInt( CompFlags.make_enums_an_int );
}

static void readEnums( void )
{
    unsigned enum_int;

    while( HeadEnums != NULL ) {
        popPrag( &HeadEnums, NULL );
    }
    for( ; (enum_int = PCHReadUInt()) != PCH_LIST_TERM; ) {
        pushPrag( &HeadEnums, enum_int );
    }
    CompFlags.make_enums_an_int = ( PCHReadUInt() != 0 );
}

static void writeExtrefs( void )
{
    unsigned        len;
    PRAG_EXT_REF    *e;

    RingIterBeg( pragmaExtrefs, e ) {
        if( e->symbol != NULL ) {
            SymbolPCHWrite( e->symbol );
        }
    } RingIterEnd( e )
    PCHWriteCVIndexTerm();
    RingIterBeg( pragmaExtrefs, e ) {
        if( e->symbol == NULL ) {
            len = (unsigned)strlen( e->name );
            PCHWriteUInt( len );
            PCHWrite( e->name, len + 1 );
        }
    } RingIterEnd( e )
    PCHWriteUInt( 0 );
}

static void readExtrefs( void )
{
    SYMBOL       sym;
    unsigned     len;
    PRAG_EXT_REF *entry;

    RingFree( &pragmaExtrefs );
    while( (sym = SymbolPCHRead()) != NULL ) {
        entry = RingAlloc( &pragmaExtrefs, offsetof( PRAG_EXT_REF, name ) + 1 );
        entry->symbol = sym;
        entry->name[0] = '\0';
    }
    while( (len = PCHReadUInt()) != 0 ) {
        entry = RingAlloc( &pragmaExtrefs, offsetof( PRAG_EXT_REF, name ) + len + 1 );
        PCHRead( entry->name, len + 1 );
        entry->symbol = NULL;
    }
}

pch_status PCHReadPragmaData( void )
{
    unsigned depth;
    unsigned value;

    PCHReadVar( PragToggle );
    CgInfoLibPCHRead();
    readPacks();
    readEnums();
    readExtrefs();
    PCHReadVar( CompInfo.init_priority );
    PCHReadVar( CompInfo.dt_method_speced );
    depth = PCHReadUInt();
    CgBackSetInlineDepth( depth );
    value = PCHReadUInt();
    CgBackSetInlineRecursion( value != 0 );
    return( PCHCB_OK );
}

pch_status PCHWritePragmaData( void )
{
    unsigned depth;
    unsigned value;

    PCHWriteVar( PragToggle );
    CgInfoLibPCHWrite();
    writePacks();
    writeEnums();
    writeExtrefs();
    PCHWriteVar( CompInfo.init_priority );
    PCHWriteVar( CompInfo.dt_method_speced );
    depth = CgBackGetInlineDepth();
    PCHWriteUInt( depth );
    value = CgBackGetInlineRecursion();
    PCHWriteUInt( value );
    return( PCHCB_OK );
}

pch_status PCHInitPragmaData( bool writing )
{
    /* unused parameters */ (void)writing;

    return( PCHCB_OK );
}

pch_status PCHFiniPragmaData( bool writing )
{
    /* unused parameters */ (void)writing;

    return( PCHCB_OK );
}

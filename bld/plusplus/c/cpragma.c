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


#include <stdlib.h>
#include <string.h>

#include "plusplus.h"
#include "preproc.h"
#include "errdefns.h"
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

// from \watcom\h
#include "rtinit.h"


typedef struct {                // PRAG_EXT_REF -- extref's pragma'd
    void* next;                 // - next in ring
    SYMBOL extref;              // - extref symbol
} PRAG_EXT_REF;

static PRAG_EXT_REF* pragmaExtrefs; // ring of pragma'd extref symbols

extern  void    PragAux();

static void init                // MODULE INITIALIZATION
    ( INITFINI* defn )
{
    defn = defn;
    pragmaExtrefs = NULL;
}

static void fini                // MODULE COMPLETION
    ( INITFINI* defn )
{
    defn = defn;
    RingFree( &pragmaExtrefs );
}

INITDEFN( pragma_extref, init, fini );

static boolean grabNum( unsigned *val )
{
    if( CurToken == T_CONSTANT ) {
        *val = U32Fetch( Constant64 );
        NextToken();
        return( TRUE );
    }
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( CurToken == T_CONSTANT ) {
            *val = U32Fetch( Constant64 );
            NextToken();
            MustRecog( T_RIGHT_PAREN );
            return( TRUE );
        }
    }
    NextToken();
    return( FALSE );
}

// #pragma template_depth n
// #pragma template_depth( n )
//
// Used to set the depth, up to which, function templates will be expanded
static void pragTemplateDepth(    // PROCESS #pragma template_depth
    void )
{
    unsigned num;

    if( grabNum( &num ) ) {
        TemplateSetDepth( num );
    } else {
        CErr1( ERR_PRAG_TEMPLATE_DEPTH );
    }
}

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

    if( grabNum( &num ) && num <= MAX_INLINE_DEPTH ) {
        CgBackSetInlineDepth( num );
    } else {
        CErr1( ERR_PRAG_INLINE_DEPTH );
    }
}

// #pragma comment( lib, "libname.lib" )

static void pragComment(        // #PRAGMA COMMENT
    void )
{
    if( CurToken == T_LEFT_PAREN ) {
        PPState = PPS_EOL;
        NextToken();
        if( PragRecog( "lib" ) ) {
            MustRecog( T_COMMA );
            if( CurToken != T_STRING ) {
                MustRecog( T_STRING );
            } else {
                do {
                    CgInfoAddUserLib( Buffer );
                    NextToken();
                } while( CurToken == T_STRING );
            }
        } else {
            while( CurToken != T_RIGHT_PAREN ) {
                NextToken();
            }
        }
        PPState = PPS_EOL | PPS_NO_EXPAND;
        MustRecog( T_RIGHT_PAREN );
    }
}

static void collectStrings( VBUF *vinfo )
{
    VbufInit( vinfo );
    VStrNull( vinfo );
    while( CurToken == T_STRING ) {
        VStrConcStr( vinfo, Buffer );
        NextToken();
    }
}

// #pragma message( "message" " string" );

static void pragMessage(        // #PRAGMA MESSAGE
    void )
{
    VBUF str;

    if( CurToken == T_LEFT_PAREN ) {
        PPState = PPS_EOL;
        NextToken();
        if( CurToken == T_STRING ) {
            collectStrings( &str );
            CErr2p( WARN_USER_WARNING_MSG, str.buf );
            VbufFree( &str );
        }
        PPState = PPS_EOL | PPS_NO_EXPAND;
        MustRecog( T_RIGHT_PAREN );
    }
}

// #pragma error "message string" ;

static void pragError(          // #PRAGMA ERROR
    void )
{
    VBUF str;

    if( CurToken == T_STRING ) {
        PPState = PPS_EOL;
        collectStrings( &str );
        CErr2p( ERR_USER_ERROR_MSG, str.buf );
        VbufFree( &str );
        PPState = PPS_EOL | PPS_NO_EXPAND;
    }
}


// #pragma inline_recursion on
// #pragma inline_recursion off
// #pragma inline_recursion(on)         -- MS compatible
// #pragma inline_recursion(off)        -- MS compatible
//
// Used to indicate whether recursive inlining is permitted or not
//
static void pragInlineRecursion(// PROCESS #pragma inline_recusrion
    void )
{
    if( PragRecog( "on" ) ) {
        CgBackSetInlineRecursion( TRUE );
    } else if( PragRecog( "off" ) ) {
        CgBackSetInlineRecursion( FALSE );
    } else {
        if( CurToken == T_LEFT_PAREN ) {
            NextToken();
            if( PragRecog( "on" ) ) {
                CgBackSetInlineRecursion( TRUE );
            } else if( PragRecog( "off" ) ) {
                CgBackSetInlineRecursion( FALSE );
            } else {
                MustRecog( T_ID );
            }
            MustRecog( T_RIGHT_PAREN );
        }
    }
}


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

    if( CurToken == T_LEFT_PAREN ) {
        PPState = PPS_EOL;
        NextToken();
        if( CurToken == T_STRING ) {
            seg_name = strsave( Buffer );
            seg_class = NULL;
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
                if( CurToken == T_STRING ) {
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
        PPState = PPS_EOL | PPS_NO_EXPAND;
        MustRecog( T_RIGHT_PAREN );
    } else if( CurToken == T_STRING ) {
        SegmentCode( Buffer, NULL );
        NextToken();
    }
}


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

    if( CurToken == T_LEFT_PAREN ) {
        PPState = PPS_EOL;
        NextToken();
        if( CurToken == T_STRING ) {
            seg_name = strsave( Buffer );
            seg_class = NULL;
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
                if( CurToken == T_STRING ) {
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
        PPState = PPS_EOL | PPS_NO_EXPAND;
        MustRecog( T_RIGHT_PAREN );
    } else if( CurToken == T_STRING ) {
        SegmentData( Buffer, NULL );
        NextToken();
    }
}


// forms: #pragma warning # level   (change message # to have level "level)
//      : #pragma warning * level   (change all messages to have level "level)
//
// - "level" must be digit (0-9)
// - "level==0" implies warning will be treated as an error
//
static boolean pragWarning(     // PROCESS #PRAGMA WARNING
    void )
{
    int msgnum;                 // - message number
    int level;                  // - new level
    boolean change_all;         // - TRUE ==> change all levels
    boolean error_occurred;     // - TRUE ==> error has occurred

    error_occurred = FALSE;
    if( CurToken == T_TIMES ) {
        change_all = TRUE;
    } else if( CurToken == T_CONSTANT ) {
        msgnum = U32Fetch( Constant64 );
        change_all = FALSE;
    } else {
        // ignore; MS or other vendor's #pragma
        return( TRUE );
    }
    NextToken();
    if( CurToken == T_CONSTANT ) {
        level = U32Fetch( Constant64 );
    } else {
        CErr1( ERR_PRAG_WARNING_BAD_LEVEL );
        error_occurred = TRUE;
    }
    NextToken();
    if( ! error_occurred ) {
        if( change_all ) {
            WarnChangeLevels( level );
        } else {
            WarnChangeLevel( level, msgnum );
        }
    }
    return( FALSE );
}


static void endOfPragma(
    void )
{
    if( CurToken == T_SEMI_COLON ) NextToken();
    if( CurToken != T_NULL ) Expecting( Tokens[ T_NULL ] );
    while( CurToken != T_NULL && CurToken != T_EOF ) {
        NextToken();
    }
}

void PragmaSetToggle(           // SET TOGGLE
    boolean set_flag )          // - TRUE ==> set flag
{
    #define toggle_pick( x ) \
        if( strcmp( Buffer, #x ) == 0 ) {       \
            PragDbgToggle.x = set_flag;         \
            return;                             \
        }
    #include "dbgtogg.h"

    #define toggle_pick( x ) \
        if( strcmp( Buffer, #x ) == 0 ) {       \
            PragToggle.x = set_flag;            \
            return;                             \
        }
    #include "tognam.h"
}

static void pragFlag(           // SET TOGGLES
    boolean set_flag )          // - TRUE ==> set flag
{
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        while( PragIdCurToken() ) {
            PragmaSetToggle( set_flag );
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    }
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
    for(;;) {
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
    priority += adjust;
    if( priority > 255 ) {
        CErr1( ERR_PRAG_INITIALIZE_PRIORITY );
        priority = INIT_PRIORITY_PROGRAM;
    }
    CompInfo.init_priority = priority;
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

    if( PragRecog( "int" ) ) {
        pushPrag( &HeadEnums, CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = 1;
    } else if( PragRecog( "minimum" ) ) {
        pushPrag( &HeadEnums, CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = 0;
    } else if( PragRecog( "original" ) ) {
        pushPrag( &HeadEnums, CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = CompFlags.original_enum_setting;
    } else if( PragRecog( "pop" ) ) {
        if( popPrag( &HeadEnums, &make_enums_an_int ) ) {
            CompFlags.make_enums_an_int = make_enums_an_int;
        }
    }
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
    int priority;

    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( PragRecog( "compiler" ) ) {
            priority = INIT_PRIORITY_LIBRARY - 1;
        } else if( PragRecog( "lib" ) ) {
            priority = INIT_PRIORITY_LIBRARY;
        } else {
            priority = INIT_PRIORITY_PROGRAM;
            MustRecog( T_ID );
        }
        CompInfo.init_priority = priority;
        MustRecog( T_RIGHT_PAREN );
    }
}


// forms: #pragma extref symbol
//        #pragma extref ( symbol, ..., symbol )
//
// causes a external reference to be emitted for the symbol
//
static void newPRAG_EXT_REF( SYMBOL sym )
{
    PRAG_EXT_REF* entry = RingAlloc( &pragmaExtrefs, sizeof( PRAG_EXT_REF ) );
    entry->extref = sym;
}

static int parseExtRef(     // PARSE SYMBOL NAME
    void )
{
    int err;                // TRUE ==> syntax error

    if( PragIdCurToken() ) {
        char* name = NameCreateLen( Buffer, TokenLen );
        SEARCH_RESULT* result;
        result = ScopeFindNaked( CurrScope
                               , NameCreateLen( Buffer, TokenLen ) );
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
                newPRAG_EXT_REF( sym );
            }
        }
        NextToken();
        err = 0;
    } else {
        err = 1;
    }
    return err;
}


static void pragExtRef(     // #pragma extref ...
    void )
{
    int err;                // - TRUE ==> error occurred

    if( CurToken == T_LEFT_PAREN ) {
        PPState = PPS_EOL;
        NextToken();
        for( ; ; ) {
            err = parseExtRef();
            if( err ) break;
            if( CurToken != T_COMMA )  break;
            NextToken();
        }
        PPState = PPS_EOL | PPS_NO_EXPAND;
        if( ! err ) {
            MustRecog( T_RIGHT_PAREN );
        }
    } else {
        parseExtRef();
    }
}


void PragmaExtrefsValidate      // VALIDATE EXTREFS FOR PRAGMAS
    ( void )
{
    PRAG_EXT_REF* entry;        // - current entry

    RingIterBeg( pragmaExtrefs, entry ) {
        SYMBOL sym = entry->extref;
        if( SymIsExtern( sym ) ) {
            if( IsOverloadedFunc( sym ) ) {
                CErr2p( ERR_PRAG_EXTREF_OVERLOADED, sym );
                entry->extref = NULL;
            }
        } else {
            CErr2p( ERR_PRAG_EXTREF_EXTERN, sym );
            entry->extref = NULL;
        }
    } RingIterEnd( entry );
}


void PragmaExtrefsInject        // INJECT EXTREFS FOR PRAGMAS
    ( void )
{
    PRAG_EXT_REF* entry;        // - current entry

    RingIterBeg( pragmaExtrefs, entry ) {
        SYMBOL sym = entry->extref;
        if( NULL != sym ) {
            CgInfoAddPragmaExtref( sym );
        }
    } RingIterEnd( entry );
}


static void pragIntrinsic(      // SET FUNCTIONS TO BE (NOT TO BE) INTRINSIC
    boolean intrinsic )         // - TRUE ==> function to be intrinsic
{
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        while( PragIdCurToken() ) {
            ScopeIntrinsic( intrinsic );
            NextToken();
            if( CurToken != T_COMMA )  break;
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    }
}


static void pragDumpObjectModel( // DUMP OBJECT MODEL
    void )
{
    if( PragIdCurToken() ) {
        SEARCH_RESULT* result = ScopeFindNaked( CurrScope
                                              , NameCreateLen( Buffer, TokenLen ) );
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
    return method;
}


static void pragDestruct(       // SPECIFY DESTRUCTION MECHANISM
    void )
{
    DT_METHOD method;           // - method
    DT_METHOD next;             // - next method spec

    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        method = DTM_COUNT;
        for( ; ; ) {
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
        CompFlags.dt_method_pragma = TRUE;
    }
}

#ifndef NDEBUG
static void pragBreak()
{
    __trap();
}
#endif

static boolean startPrag( char *id )
{
    unsigned save;

    // use this function when you want to immediately process macros
    // after you recognize the pragma (PragRecog performs a NextToken
    // so the pre-processing flag has to be set before PragRecog is called)
    save = PPState;
    PPState = PPS_EOL;
    // NextToken inside of PragRecog will process macros on next token
    if( PragRecog( id ) ) {
        return( TRUE );
    }
    PPState = save;
    return( FALSE );
}


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
}


// form: #pragma read_only_directory "directory"*
//
// (1) causes all files within directory to be marked read-only
//
static void pragReadOnlyDir
    ( void )
{
    while( CurToken == T_STRING ) {
        SrcFileReadOnlyDir( Buffer );
        NextToken();
        if( CurToken == T_SEMI_COLON ) {
            NextToken();
        }
    }
}

// form: #pragma once
//
// (1) current source file will never be #include'd again
static void pragOnce( void )
{
    SrcFileOnceOnly();
}

void CPragma()                  // PROCESS A PRAGMA
{
    boolean our_pragma;         // - TRUE ==> one of ours

    SrcFileGuardStateSig();
    if( CompFlags.cpp_output ) {
        if( ! CppPrinting() ) return;
        fprintf( CppFile, "#pragma" );
        PPState = PPS_EOL;
        CompFlags.in_pragma = 1;
        for(;;) {
            GetNextToken();
            if( CurToken == T_NULL ) break;
            PrtToken();
        }
        CompFlags.in_pragma = 0;
    } else {
        our_pragma = FALSE;
        NextToken();
        for(;;) {
            if( ! PragRecog( NULL ) ) break;
            if( PragRecog( "on" ) ) {
                pragFlag( 1 );
            } else if( PragRecog( "off" ) ) {
                pragFlag( 0 );
            } else if( startPrag( "aux" ) || startPrag( "linkage" ) ) {
                PragAux();
            } else if( PragRecog( "library" ) ) {
                pragLibs();
            } else if( PragRecog( "once" ) ) {
                pragOnce();
            } else if( PragRecog( "extref" ) ) {
                pragExtRef();
            } else if( PragRecog( "comment" ) ) {
                pragComment();
            } else if( PragRecog( "pack" ) ) {
                pragPack();
            } else if( PragRecog( "warning" ) ) {
                if( pragWarning() ) {
                    /* ignore #pragma warning */
                    break;
                }
            } else if( PragRecog( "code_seg" ) ) {
                pragCodeSeg();
            } else if( PragRecog( "data_seg" ) ) {
                pragDataSeg();
            } else if( PragRecog( "initialize" ) ) {
                pragInitialize();
            } else if( PragRecog( "init_seg" ) ) {
                pragInitSeg();
            } else if( PragRecog( "inline_depth" ) ) {
                pragInlineDepth();
            } else if( PragRecog( "template_depth" ) ) {
                pragTemplateDepth();
            } else if( PragRecog( "inline_recursion" ) ) {
                pragInlineRecursion();
            } else if( PragRecog( "intrinsic" ) ) {
                pragIntrinsic( TRUE );
            } else if( PragRecog( "function" ) ) {
                pragIntrinsic( FALSE );
            } else if( PragRecog( "destruct" ) ) {
                pragDestruct();
            } else if( PragRecog( "enum" ) ) {
                pragEnum();
            } else if( PragRecog( "dump_object_model" ) ) {
                pragDumpObjectModel();
            } else if( startPrag( "read_only_file" ) ) {
                pragReadOnlyFile();
            } else if( startPrag( "read_only_directory" ) ) {
                pragReadOnlyDir();
            } else if( PragRecog( "message" ) ) {
                pragMessage();
            } else if( PragRecog( "error" ) ) {
                pragError();
#ifndef NDEBUG
            } else if( PragRecog( "break" ) ) {
                pragBreak();
#endif
            } else {
                break;
            }
            our_pragma = TRUE;
        }
        if( our_pragma ) {
            endOfPragma();
        }
    }
}


void PragInitDefaultInfo(
    void )
{
    DefaultInfo._class = 0;
    DefaultInfo.code = NULL;
    DefaultInfo.parms = DefaultParms;
    HW_CAsgn( DefaultInfo.returns, HW_EMPTY );
    HW_CAsgn( DefaultInfo.streturn, HW_EMPTY );
    HW_CAsgn( DefaultInfo.save, HW_FULL );
    DefaultInfo.use = 0;
    DefaultInfo.objname = NULL;
}


void PragInit(
    void )
{
    DefaultInfo.use = 2;

    CdeclInfo = DefaultInfo;
    PascalInfo = DefaultInfo;
    FortranInfo = DefaultInfo;
    SyscallInfo = DefaultInfo;
    OptlinkInfo = DefaultInfo;
    StdcallInfo = DefaultInfo;
#ifdef __OLD_STDCALL
    OldStdcallInfo = DefaultInfo;
#endif

    CompInfo.init_priority = INIT_PRIORITY_PROGRAM;
}


static void pragLibs(           // #PRAGMA library ( lib ... lib )
    void )
{
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        while( PragIdCurToken() || CurToken == T_STRING ) {
            CgInfoAddUserLib( Buffer );
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    } else {
        CompFlags.pragma_library = 1;
    }
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

static boolean popPrag( PRAG_STACK **h, unsigned *pvalue )
{
    PRAG_STACK *pack_entry;

    pack_entry = StackPop( h );
    if( pack_entry != NULL ) {
        if( pvalue != NULL ) {
            *pvalue = pack_entry->value;
        }
        StackPush( &FreePrags, pack_entry );
        return( TRUE );
    }
    return( FALSE );
}

// #pragma pack()
// #pragma pack( <number> )
// #pragma pack( pop )
// #pragma pack( push )
// #pragma pack( push, <number> )

static void pragPack(           // #PRAGMA PACK
    void )
{
    if( CurToken == T_LEFT_PAREN ) {
        PPState = PPS_EOL;
        NextToken();
        switch( CurToken ) {
        case T_ID:
            if( PragRecog( "pop" ) ) {
                popPrag( &HeadPacks, &PackAmount );
                MustRecog( T_RIGHT_PAREN );
            } else if( PragRecog( "push" ) ) {
                if( CurToken == T_RIGHT_PAREN ) {
                    pushPrag( &HeadPacks, PackAmount );
                    MustRecog( T_RIGHT_PAREN );
                } else {
                    MustRecog( T_COMMA );
                    if( CurToken != T_CONSTANT ) {
                        MustRecog( T_CONSTANT );
                    } else {
                        pushPrag( &HeadPacks, PackAmount );
                        PackAmount = VerifyPackAmount( U32Fetch( Constant64 ) );
                        NextToken();
                        MustRecog( T_RIGHT_PAREN );
                    }
                }
            } else {
                CErr( ERR_EXPECTING_BUT_FOUND, "pop", Buffer );
            }
            PPState = PPS_EOL | PPS_NO_EXPAND;
            break;
        case T_CONSTANT:
            PackAmount = VerifyPackAmount( U32Fetch( Constant64 ) );
            PPState = PPS_EOL | PPS_NO_EXPAND;
            NextToken();
            MustRecog( T_RIGHT_PAREN );
            break;
        case T_RIGHT_PAREN:
            PPState = PPS_EOL | PPS_NO_EXPAND;
            NextToken();
            PackAmount = GblPackAmount;
            break;
        default:
            PPState = PPS_EOL | PPS_NO_EXPAND;
            MustRecog( T_RIGHT_PAREN );
        }
    }
}

typedef struct magic_word {
    char        *name;
    int         index;
} MAGIC_WORD;

static MAGIC_WORD magicWords[] = {
    { "cdecl",      M_CDECL   },
    { "default",    M_DEFAULT },
    { "fortran",    M_FORTRAN },
    { "optlink",    M_OPTLINK },
    { "pascal",     M_PASCAL  },
    { "stdcall",    M_STDCALL },
    { "syscall",    M_SYSCALL },
    { "system",     M_SYSCALL },
    { NULL,         M_UNKNOWN },
};


static int lookupMagicKeyword(  // LOOKUP A MAGIC KEYWORD
    char *name )                // - name to be looked up
{
    MAGIC_WORD *mptr;           // - current entry

    if( *name == '_' ) {
        ++name;
        if( *name == '_' ) {
            ++name;
        }
    }
    for( mptr = magicWords; mptr->name != NULL; ++mptr ) {
        if( strcmp( mptr->name, name ) == 0 ) break;
    }
    return( mptr->index );
}

static char *retrieveName( unsigned m_type )
{
    MAGIC_WORD *mptr;           // - current entry

    for( mptr = magicWords; mptr->name != NULL; ++mptr ) {
        if( mptr->index == m_type ) {
            return( mptr->name );
        }
    }
    return( NULL );
}


#if _INTEL_CPU
static int MagicKeyword(        // LOOKUP A MAGIC KEYWORD FROM BUFFER
    void )
{
    return lookupMagicKeyword( Buffer );
}
#endif


void CreateAux(                 // CREATE AUX ID
        char *id )              // - id
{
    CurrEntry = CMemAlloc( sizeof(AUX_ENTRY) + strlen( id ) );
    strcpy( CurrEntry->name, id );
    CurrInfo = CMemAlloc( sizeof( AUX_INFO ) );
    // AuxCopy assumes destination is valid
    CurrInfo->parms = NULL;
    CurrInfo->objname = NULL;
    CurrInfo->code = NULL;
}


static boolean setAuxInfo(          // SET CURRENT INFO. STRUCTURE
    unsigned m_type,                // - type to be set
    boolean create_new )            // - TRUE if we want a new aux_info
{
    boolean found;

    found = TRUE;
    switch( m_type ) {
    case M_DEFAULT:
        CurrInfo = &DefaultInfo;
        break;
    case M_CDECL:
        CurrInfo = &CdeclInfo;
        break;
    case M_PASCAL:
        CurrInfo = &PascalInfo;
        break;
    case M_FORTRAN:
        CurrInfo = &FortranInfo;
        break;
    case M_SYSCALL:
        CurrInfo = &SyscallInfo;
        break;
    case M_OPTLINK:
        CurrInfo = &OptlinkInfo;
        break;
    case M_STDCALL:
        CurrInfo = &StdcallInfo;
        break;
    default:
        if( create_new ) {
            CreateAux( Buffer );
        } else {
            found = FALSE;
        }
        break;
    }
    return( found );
}

boolean PragmaName( void *pragma, char **id )
/*******************************************/
{
    *id = NULL;
    if( pragma == &DefaultInfo ) {
        return( TRUE );
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
    }
    if( *id != NULL ) {
        return( TRUE );
    }
    *id = AuxRetrieve( pragma );
    return( FALSE );
}


#if _INTEL_CPU
void SetCurrInfo(               // SET CURRENT INFO. STRUCTURE
    void )
{
    setAuxInfo( MagicKeyword(), TRUE );
}
#endif


#if _INTEL_CPU
void PragCurrAlias(             // LOCATE ALIAS FOR PRAGMA
    void )
{
    struct aux_entry *search;

    search = NULL;
    CurrAlias = &DefaultInfo;
    switch( MagicKeyword() ) {
    case M_DEFAULT:
        CurrAlias = &DefaultInfo;
        break;
    case M_CDECL:
        CurrAlias = &CdeclInfo;
        break;
    case M_PASCAL:
        CurrAlias = &PascalInfo;
        break;
    case M_FORTRAN:
        CurrAlias = &FortranInfo;
        break;
    case M_SYSCALL:
        CurrAlias = &SyscallInfo;
        break;
    case M_OPTLINK:
        CurrAlias = &OptlinkInfo;
        break;
    case M_STDCALL:
        CurrAlias = &StdcallInfo;
        break;
    default:
        search = AuxLookup( Buffer );
        if( search != NULL ) {
            CurrAlias = search->info;
        }
    }
}
#endif


static void copyParms(           // COPY PARMS PORTION
    void )
{
    if( CurrInfo->parms != CurrAlias->parms ) {
        /* new parms have already been allocated */
        return;
    }
    if( CurrInfo->parms != DefaultParms ) {
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
    boolean set_sym )           // - TRUE ==> set SYMBOL's aux_info
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


boolean PragIdCurToken(         // IS CURTOKEN AN ID?
    void )
{
    unsigned t;

    t = CurToken;
    if( t == T_ID ) {
        return( TRUE );
    }
    if( t >= FIRST_KEYWORD && t <= LAST_KEYWORD ) {
        return( TRUE );
    }
    return( FALSE );
}


boolean PragRecog(              // RECOGNIZE PRAGMA ID
    char *what )                // - id
{
    char *p;

    if( PragIdCurToken() ) {
        if( what == NULL ) {
            return( 1 );
        }
        p = Buffer;
        if( *p == '_' ) {
            ++p;
            if( *p == '_' ) {
                ++p;
            }
        }
        if( stricmp( p, what ) == 0 ) {
            NextToken();
            return( 1 );
        }
    }
    return( 0 );
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


void *PragmaLookup( char *name, unsigned index )
/**********************************************/
{
    struct aux_entry *ent;

    if( index != M_UNKNOWN ) {
        if( setAuxInfo( index, FALSE ) ) {
            return( CurrInfo );
        }
    }
    if( setAuxInfo( lookupMagicKeyword( name ), FALSE ) ) {
        return( CurrInfo );
    }
    ent = AuxLookup( name );
    if( ent == NULL ) {
        return( NULL );
    }
    return( ent->info );
}


void *GetTargetHandlerPragma    // GET PRAGMA FOR FS HANDLER
    ( void )
{
    void *prag;

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


#if _INTEL_CPU
int PragSet(                    // GET ENDING PRAGMA DELIMITER
    void )
{
    int retn;                   // - delimiter

    switch( CurToken ) {
      case  T_LEFT_BRACKET :
        retn = T_RIGHT_BRACKET;
        break;
      case T_LEFT_BRACE :
        retn = T_RIGHT_BRACE;
        break;
      default :
        retn = T_NULL;
        break;
    }
    return( retn );
}
#endif


#if _INTEL_CPU
hw_reg_set PragRegList(         // GET PRAGMA REGISTER SET
    void )
{
    hw_reg_set res;             // - resultant set
    hw_reg_set reg;             // - current set
    int close;                  // - ending delimiter

    HW_CAsgn( res, HW_EMPTY );
    HW_CAsgn( reg, HW_EMPTY );
    close = PragSet();
    if( close != T_NULL ) {
        PPState = PPS_EOL;
        NextToken();
        for(;;) {
            reg = PragRegName( Buffer );
            if( HW_CEqual( reg, HW_EMPTY ) ) break;
            HW_TurnOn( res, reg );
            NextToken();
        }
        PPState = PPS_EOL | PPS_NO_EXPAND;
        MustRecog( close );
    }
    return( res );
}
#endif


#if _INTEL_CPU
void PragManyRegSets(           // GET PRAGMA REGISTER SETS
    void )
{
    hw_reg_set buff[ MAXIMUM_PARMSETS ];
    int i;
    hw_reg_set list;
    hw_reg_set *sets;

    list = PragRegList();
    i = 0;
    while( !HW_CEqual( list, HW_EMPTY ) && (i != MAXIMUM_PARMSETS) ) {
        buff[ i++ ] = list;
        list = PragRegList();
    }
    if( !HW_CEqual( list, HW_EMPTY ) ) {
        CErr1( ERR_TOO_MANY_PARM_SETS );
    }
    HW_CAsgn( buff[i], HW_EMPTY );
    i++;
    i *= sizeof( hw_reg_set );
    sets = (hw_reg_set *)CMemAlloc( i );
    memcpy( sets, buff, i );
    if( CurrInfo->parms != DefaultParms ) {
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
        if( strcmp( seg->segname, name ) == 0 )  return( seg );
    }
    seg = CMemAlloc( sizeof( struct textsegment ) );
    seg->next = TextSegList;
    seg->segname = strsave( name );
    seg->segment_number = 0;
    TextSegList = seg;
    return( seg );
}
#endif


boolean ReverseParms( void *pragma )
/**********************************/
{
    AUX_INFO *aux = pragma;

    if( aux->_class & REVERSE_PARMS ) {
        return( TRUE );
    }
    return( FALSE );
}

boolean AddParmSize( void *pragma )
/*********************************/
{
    if( pragma == &StdcallInfo ) {
        return( TRUE );
    }
    return( FALSE );
}

static void writePacks( void )
{
    PRAG_STACK *pack_entry;
    PRAG_STACK *reversed_packs;
    unsigned pack_amount;

    reversed_packs = NULL;
    for(;;) {
        pack_entry = StackPop( &HeadPacks );
        if( pack_entry == NULL ) break;
        StackPush( &reversed_packs, pack_entry );
    }
    for(;;) {
        pack_entry = StackPop( &reversed_packs );
        if( pack_entry == NULL ) break;
        pack_amount = pack_entry->value;
        PCHWriteUInt( pack_amount );
        StackPush( &HeadPacks, pack_entry );
    }
    pack_amount = -1;
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
    for(;;) {
        pack_amount = PCHReadUInt();
        if( pack_amount == -1 ) break;
        pushPrag( &HeadPacks, pack_amount );
    }
    pack_amount = PCHReadUInt();
    if( pack_amount == -1 ) {
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
    for(;;) {
        enum_entry = StackPop( &HeadPacks );
        if( enum_entry == NULL ) break;
        StackPush( &reversed_enums, enum_entry );
    }
    for(;;) {
        enum_entry = StackPop( &reversed_enums );
        if( enum_entry == NULL ) break;
        enum_int = enum_entry->value;
        PCHWriteUInt( enum_int );
        StackPush( &HeadPacks, enum_entry );
    }
    enum_int = -1;
    PCHWriteUInt( enum_int );
    PCHWriteUInt( CompFlags.make_enums_an_int );
}

static void readEnums( void )
{
    unsigned enum_int;

    while( HeadEnums != NULL ) {
        popPrag( &HeadEnums, NULL );
    }
    for(;;) {
        enum_int = PCHReadUInt();
        if( enum_int == -1 ) break;
        pushPrag( &HeadEnums, enum_int );
    }
    CompFlags.make_enums_an_int = PCHReadUInt();
}

static void writeExtrefs( void )
{
    SYMBOL s;
    PRAG_EXT_REF *e;

    RingIterBeg( pragmaExtrefs, e ) {
        s = SymbolGetIndex( e->extref );
        DbgAssert( s != NULL );
        PCHWrite( &s, sizeof( s ) );
    } RingIterEnd( e )
    s = NULL;
    PCHWrite( &s, sizeof( s ) );
}

static void readExtrefs( void )
{
    SYMBOL s;

    RingFree( &pragmaExtrefs );
    for(;;) {
        PCHRead( &s, sizeof( s ) );
        s = SymbolMapIndex( s );
        if( s == NULL ) break;
        newPRAG_EXT_REF( s );
    }
}

pch_status PCHReadPragmaData( void )
{
    unsigned depth;
    unsigned value;

    PCHRead( &PragToggle, sizeof( PragToggle ) );
    CgInfoLibPCHRead();
    readPacks();
    readEnums();
    readExtrefs();
    PCHRead( &CompInfo.init_priority, sizeof( CompInfo.init_priority ) );
    PCHRead( &CompInfo.dt_method_speced, sizeof( CompInfo.dt_method_speced ) );
    depth = PCHReadUInt();
    CgBackSetInlineDepth( depth );
    value = PCHReadUInt();
    CgBackSetInlineRecursion( value );
    return( PCHCB_OK );
}

pch_status PCHWritePragmaData( void )
{
    unsigned depth;
    unsigned value;

    PCHWrite( &PragToggle, sizeof( PragToggle ) );
    CgInfoLibPCHWrite();
    writePacks();
    writeEnums();
    writeExtrefs();
    PCHWrite( &CompInfo.init_priority, sizeof( CompInfo.init_priority ) );
    PCHWrite( &CompInfo.dt_method_speced, sizeof( CompInfo.dt_method_speced ) );
    depth = CgBackGetInlineDepth();
    PCHWriteUInt( depth );
    value = CgBackGetInlineRecursion();
    PCHWriteUInt( value );
    return( PCHCB_OK );
}

pch_status PCHInitPragmaData( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniPragmaData( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

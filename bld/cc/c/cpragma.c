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
* Description:  Generic (target independent) pragma processing.
*
****************************************************************************/


#include <ctype.h>
#include "cvars.h"
#include "cgswitch.h"
#include "pdefn2.h"
#include "caux.h"
#include "cfeinfo.h"
#include "asmstmt.h"
#include "toggles.h"
#ifndef NDEBUG
    #include "togglesd.h"
#endif

#include "clibext.h"


#define pragmaNameRecog(what)   (strcmp(Buffer, what) == 0)
#define pragmaIdRecog(what)     (stricmp(what, SkipUnderscorePrefix(Buffer, NULL, true)) == 0)

typedef enum {
    #define pick(a,b,c,d)   a,
    #include "auxinfo.h"
    #undef pick
    M_SIZE
} magic_words;

typedef struct prag_stack {
    struct prag_stack   *next;
    unsigned            value;
} prag_stack;

pragma_toggles          PragmaToggles;
#ifndef NDEBUG
pragma_dbg_toggles      PragmaDbgToggles;
#endif

#define pick( x ) static prag_stack *TOGGLE_STK( x );
#include "togdef.h"
#undef pick
static prag_stack       *TOGGLE_STK( pack );
static prag_stack       *TOGGLE_STK( enum );
static prag_stack       *FreePrags;

static struct magic_words_data {
    const char      *name;
    aux_info        *info;
} magicWords[] = {
    #define pick(a,b,c,d) { b, d },
    #include "auxinfo.h"
    #undef pick
};

static prag_stack *stackPush( prag_stack **header, prag_stack *element )
/**********************************************************************/
{
    element->next = *header;
    *header = element;
    return( element );
}

static prag_stack *stackPop( prag_stack **header )
/************************************************/
{
    prag_stack  *element;

    element = *header;
    if( element != NULL ) {
        *header = element->next;
    }
    return( element );
}

static void freeStack( prag_stack **header )
/******************************************/
{
    prag_stack  *element;

    while( (element = *header) != NULL ) {
        *header = element->next;
        CMemFree( element );
    }
}

static void pushPrag( prag_stack **header, unsigned value )
/*********************************************************/
{
    prag_stack *stack_entry;

    stack_entry = stackPop( &FreePrags );
    if( stack_entry == NULL ) {
        stack_entry = CMemAlloc( sizeof( *stack_entry ) );
    }
    stack_entry->value = value;
    stackPush( header, stack_entry );
}

static bool popPrag( prag_stack **header, unsigned *pvalue )
/**********************************************************/
{
    prag_stack *stack_entry;

    stack_entry = stackPop( header );
    if( stack_entry != NULL ) {
        if( pvalue != NULL ) {
            *pvalue = stack_entry->value;
        }
        stackPush( &FreePrags, stack_entry );
        return( true );
    }
    return( false );
}

void CPragmaInit( void )
/**********************/
{
    #define pick( x ) TOGGLE_STK( x ) = NULL;
    #include "togdef.h"
    #undef pick
    TOGGLE_STK( pack ) = NULL;
    TOGGLE_STK( enum ) = NULL;
    FreePrags = NULL;

    TextSegList = NULL;
    AliasHead = NULL;
    HeadLibs = NULL;
    ExtrefInfo = NULL;

    PragmaAuxInit();

    PragmaInit();
}

void CPragmaFini( void )
/**********************/
{
    void    *junk;

    PragmaFini();

    PragmaAuxFini();

    #define pick( x )   freeStack( &TOGGLE_STK( x ) );
    #include "togdef.h"
    #undef pick
    freeStack( &TOGGLE_STK( pack ) );
    freeStack( &TOGGLE_STK( enum ) );
    freeStack( &FreePrags );

    while( (junk = TextSegList) != NULL ) {
        TextSegList = TextSegList->next;
        CMemFree( junk );
    }

    while( (junk = HeadLibs) != NULL ) {
        HeadLibs = HeadLibs->next;
        CMemFree( junk );
    }

    while( (junk = AliasHead) != NULL ) {
        AliasHead = AliasHead->next;
        CMemFree( junk );
    }

    while( (junk = ExtrefInfo) != NULL ) {
        ExtrefInfo = ExtrefInfo->next;
        CMemFree( junk );
    }
}

static void endOfPragma( bool check_end )
/***************************************/
{
    if( check_end ) {
        if( CurToken == T_SEMI_COLON )
            PPNextToken();
        if( CurToken == T_PRAGMA_END )
            return;
        ExpectingToken( T_NULL );
        while( CurToken != T_NULL && CurToken != T_EOF ) {
            PPNextToken();
        }
    }
}

const char *SkipUnderscorePrefix( const char *str, size_t *len, bool iso_compliant_names )
/****************************************************************************************/
{
    const char  *start;

    start = str;
    if( !iso_compliant_names || CompFlags.non_iso_compliant_names_enabled ) {
        if( *str == '_' ) {
            str++;
            if( *str == '_' ) {
                str++;
            }
        }
    } else {
        if( str[0] == '_' && str[1] == '_' ) {
            str += 2;
        } else {
            if( len != NULL ) {
                *len = 0;
            }
            return( "" );
        }
    }
    if( len != NULL ) {
        *len -= str - start;
    }
    return( str );
}

bool PragRecogId( const char *what )
/**********************************/
{
    bool    ok;

    ok = IS_ID_OR_KEYWORD( CurToken );
    if( ok ) {
        ok = pragmaIdRecog(what);
        if( ok ) {
            PPNextToken();
        }
    }
    return( ok );
}

bool PragRecogName( const char *what )
/************************************/
{
    bool    ok;

    ok = IS_ID_OR_KEYWORD( CurToken );
    if( ok ) {
        ok = pragmaNameRecog( what );
        if( ok ) {
            PPNextToken();
        }
    }
    return( ok );
}

static void advanceToken( void )
/******************************/
{
    CMemFree( SavedId );
    SavedId = NULL;
    CurToken = LAToken;
}

bool GetPragmaAuxAliasInfo( void )
/********************************/
{
    if( CurToken != T_LEFT_PAREN )          /* #pragma aux symbol ..... */
        return( IS_ID_OR_KEYWORD( CurToken ) );
    PPNextToken();
    if( !IS_ID_OR_KEYWORD( CurToken ) )     /* error */
        return( false );
    LookAhead();
    if( LAToken == T_RIGHT_PAREN ) {        /* #pragma aux (alias) symbol ..... */
        CurrAlias = PragmaAuxAlias( SavedId );
        advanceToken();
        PPNextToken();
        return( IS_ID_OR_KEYWORD( CurToken ) );
    } else if( LAToken == T_COMMA ) {       /* #pragma aux (symbol, alias) */
        HashValue = SavedHash;
        SetCurrInfo( SavedId );
        advanceToken();
        PPNextToken();
        if( !IS_ID_OR_KEYWORD( CurToken ) )
            return( false );                /* error */
        GetPragmaAuxAlias();
        return( false );                    /* process no more! */
    } else {                                /* error */
        advanceToken();
        return( false );                    /* shut up the compiler */
    }
}

void *AsmQuerySymbol( const char *name )
/**************************************/
{
    return( SymLook( CalcHash( name, strlen( name ) ), name ) );
}

enum sym_state AsmQueryState( void *handle )
/******************************************/
{
    SYM_HANDLE  sym_handle = (SYM_HANDLE)handle;
    SYM_ENTRY   sym;

    if( sym_handle == SYM_NULL )
        return( SYM_UNDEFINED );
    SymGet( &sym, sym_handle );
    if( (sym.flags & SYM_REFERENCED) == 0 ) {
        sym.flags |= SYM_REFERENCED;
        SymReplace( &sym, sym_handle );
    }
    switch( sym.attribs.stg_class ) {
    case SC_AUTO:
    case SC_REGISTER:
        return( SYM_STACK );
    }
    return( SYM_EXTERNAL );
}

static aux_info *lookupMagicKeyword( const char *name )
/*****************************************************/
{
    magic_words     mword;

    name = SkipUnderscorePrefix( name, NULL, true );
    for( mword = 0; mword < M_SIZE; mword++ ) {
        if( strcmp( name, magicWords[mword].name + 2 ) == 0 ) {
            return( magicWords[mword].info );
        }
    }
    return( NULL );
}


void CreateAux( const char *id )
/******************************/
{
    size_t  len;

    len = strlen( id ) + 1;
    CurrEntry = (aux_entry *)CMemAlloc( offsetof( aux_entry, name ) + len );
    memcpy( CurrEntry->name, id, len );
#if _CPU == 370
    CurrEntry->offset = -1;
#endif
}


void SetCurrInfo( const char *name )
/**********************************/
{
    SYM_HANDLE      sym_handle;
    SYM_ENTRY       sym;
    type_modifiers  sym_attrib = FLAG_NONE;

    CurrInfo = lookupMagicKeyword( name );
    if( CurrInfo == NULL ) {
        if( CurrAlias == NULL ) {
            sym_handle = SymLook( HashValue, name );
            if( sym_handle != SYM_NULL ) {
                SymGet( &sym, sym_handle );
                sym_attrib = sym.mods;
            }
            CurrAlias = GetLangInfo( sym_attrib );
        }
        CreateAux( name );
    } else {
        if( CurrAlias == NULL ) {
            CurrAlias = CurrInfo;
        }
    }
}


aux_info *PragmaAuxAlias( const char *name )
/******************************************/
{
    aux_entry   *aux;
    aux_info    *info;

    info = lookupMagicKeyword( name );
    if( info == NULL ) {
        aux = AuxLookup( name );
        if( aux != NULL ) {
            info = aux->info;
        }
    }
    return( info );
}


void XferPragInfo( const char *from, const char *to )
/***************************************************/
{
    aux_entry *ent;

    if( AuxLookup( to ) != NULL )
        return;
    ent = AuxLookup( from );
    if( ent == NULL )
        return;
    CreateAux( to );
    CurrEntry->info = ent->info;
    ent->info->use++;
    CurrEntry->next = AuxList;
    AuxList = CurrEntry;
}


static void CopyLinkage( void )
/*****************************/
{
#if _CPU == 370
    linkage_regs *regs;

    if( CurrInfo->linkage == &DefaultLinkage )
        return;
    if( CurrInfo->linkage == &OSLinkage )
        return;
    if( CurrInfo->linkage != CurrAlias->linkage )
        return;
    regs = (linkage_regs *)CMemAlloc( sizeof( linkage_regs ) );
    memcpy( regs, CurrInfo->linkage, sizeof( linkage_regs ) );
    CurrInfo->linkage = regs;
#endif
}


static void CopyParms( void )
/***************************/
{
    size_t      size;
    hw_reg_set  *regs;

    if( CurrInfo->parms != CurrAlias->parms )
        return;
    if( IsAuxParmsBuiltIn( CurrInfo->parms ) )
        return;
    size = sizeof( hw_reg_set );
    for( regs = CurrInfo->parms; !HW_CEqual( *regs, HW_EMPTY ); ++regs ) {
        size += sizeof( hw_reg_set );
    }
    regs = (hw_reg_set *)CMemAlloc( size );
    memcpy( regs, CurrInfo->parms, size );
    CurrInfo->parms = regs;
}

static void CopyCode( void )
/**************************/
{
    byte_seq    *code;
    unsigned    size;

// TODO deal with reloc list
    if( CurrInfo->code == NULL )
        return;
    if( CurrInfo->code != CurrAlias->code )
        return;
    size = offsetof( byte_seq, data ) + CurrInfo->code->length;
    code = (byte_seq *)CMemAlloc( size );
    memcpy( code, CurrInfo->code, size );
    CurrInfo->code = code;
}

static void CopyObjName( void )
/*****************************/
{
    if( CurrInfo->objname == NULL )
        return;
    if( CurrInfo->objname != CurrAlias->objname )
        return;
    CurrInfo->objname = CStrSave( CurrInfo->objname );
}

#if _CPU == _AXP
static void CopyExceptRtn( void )
/*******************************/
{
    if( CurrInfo->except_rtn == NULL )
        return;
    if( CurrInfo->except_rtn != CurrAlias->except_rtn )
        return;
    CurrInfo->except_rtn = CStrSave( CurrInfo->except_rtn );
}
#endif

void PragmaAuxEnding( void )
/**************************/
{
    if( CurrEntry == NULL )
        return;
    CurrInfo->use = CurrAlias->use; /* for compare */
    if( memcmp( CurrAlias, CurrInfo, sizeof( aux_info ) ) == 0 ) {
        CurrEntry->info = CurrAlias;
        CurrAlias->use++;
        CMemFree( CurrInfo );
    } else {
        CopyParms();
        CopyLinkage();
        CopyCode();
        CopyObjName();
#if _CPU == _AXP
        CopyExceptRtn();
#endif
        CurrInfo->use = 1;
        CurrEntry->info = CurrInfo;
    }

    /* If this pragma defines code, check to see if we already have a function body */
    if( CurrEntry != NULL && CurrEntry->info != NULL && CurrEntry->info->code != NULL ) {
        SYM_HANDLE  sym_handle;
        SYM_ENTRY   sym;

        if( SYM_NULL != (sym_handle = SymLook( CalcHash( CurrEntry->name, strlen( CurrEntry->name ) ), CurrEntry->name )) ) {
            SymGet( &sym, sym_handle );
            if( ( sym.flags & SYM_DEFINED ) && ( sym.flags & SYM_FUNCTION ) ) {
                CErr2p( ERR_SYM_ALREADY_DEFINED, CurrEntry->name );
            }
        }
    }

    CurrEntry->next = AuxList;
    AuxList = CurrEntry;
}


void PragObjNameInfo( char **objname )
/************************************/
{
    if( CurToken == T_STRING ) {
        *objname = CStrSave( Buffer );
        PPNextToken();
    }
}

TOKEN PragRegSet( void )
/**********************/
{
    if( CurToken == T_LEFT_BRACKET )
        return( T_RIGHT_BRACKET );
    if( CurToken == T_LEFT_BRACE )
        return( T_RIGHT_BRACE );
    return( T_NULL );
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

int PragRegNumIndex( const char *str, size_t len, int max_reg )
/*************************************************************/
{
    int             index;

    /* decode regular register index, max 2 digit */
    if( len > 0 && isdigit( (unsigned char)str[0] ) ) {
        if( len == 1 ) {
            index = str[0] - '0';
            if( index < max_reg ) {
                return( index );
            }
        } else if( len == 2 && isdigit( (unsigned char)str[1] ) ) {
            index = ( str[1] - '0' ) * 10 + ( str[0] - '0' );
            if( index < max_reg ) {
                return( index );
            }
        }
    }
    return( -1 );
}

void PragRegNameErr( const char *regname, size_t regnamelen )
/***********************************************************/
{
    char            buffer[20];

    if( regnamelen > sizeof( buffer ) - 1 )
        regnamelen = sizeof( buffer ) - 1;
    memcpy( buffer, regname, regnamelen );
    buffer[regnamelen] = '\0';
    CErr2p( ERR_BAD_REGISTER_NAME, buffer );
}

hw_reg_set PragRegList( void )
/****************************/
{
    hw_reg_set  res, reg;
    TOKEN       close;

    HW_CAsgn( res, HW_EMPTY );
    HW_CAsgn( reg, HW_EMPTY );
    close = PragRegSet();
    if( close != T_NULL ) {
        while( PPNextToken() != close ) {
            reg = PragReg();
            HW_TurnOn( res, reg );
            if( CurToken == close ) {
                break;
            }
        }
        MustRecog( close );
    }
    return( res );
}

hw_reg_set *PragManyRegSets( void )
/*********************************/
{
    int         i;
    hw_reg_set  list;
    hw_reg_set  *sets;
    hw_reg_set  buff[MAXIMUM_PARMSETS + 1];

    for( i = 0, list = PragRegList(); !HW_CEqual( list, HW_EMPTY ); list = PragRegList(), ++i ) {
        if( i == MAXIMUM_PARMSETS ) {
            CErr1( ERR_TOO_MANY_PARM_SETS );
            break;
        }
        buff[i] = list;
    }
    HW_CAsgn( buff[i], HW_EMPTY );
    i = ( i + 1 ) * sizeof( hw_reg_set );
    sets = (hw_reg_set *)CMemAlloc( i );
    memcpy( sets, buff, i );
    return( sets );
}

/*******************************************************
 *
 *  #pragma .... processing
 *
 *******************************************************/

void SetToggleFlag( char const *name, int func, bool push )
/*********************************************************/
{
#ifndef NDEBUG
    #define pick( x ) \
        if( strcmp( name, #x ) == 0 ) { \
            if( func == -1 ) { \
            } else { \
                TOGGLEDBG( x ) = ( func != 0 ); \
            } \
            return; \
        }
    #include "togdefd.h"
    #undef pick
#endif
    #define pick( x ) \
        if( strcmp( name, #x ) == 0 ) { \
            if( func == -1 ) { \
                unsigned    value; \
                if( popPrag( &TOGGLE_STK( x ), &value ) ) { \
                    TOGGLE( x ) = ( value != 0 ); \
                } \
            } else { \
                if( push ) { \
                    pushPrag( &TOGGLE_STK( x ), TOGGLE( x ) ); \
                } \
                TOGGLE( x ) = ( func != 0 ); \
            } \
            return; \
        }
    #include "togdef.h"
    #undef pick
}

/* forms:
 *
 *      #pragma on (<toggle name>)
 *      #pragma off (<toggle name>)
 *      #pragma pop (<toggle name>)
 */
static void pragOptions( int func )
/**********************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        for( PPNextToken(); IS_ID_OR_KEYWORD( CurToken ); PPNextToken() ) {
            SetToggleFlag( Buffer, func, true );
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

void AddLibraryName( const char *name, const char priority )
/**********************************************************/
{
    library_list    **new_owner;
    library_list    **owner;
    library_list    *lib;
    size_t          len;

    for( owner = &HeadLibs; (lib = *owner) != NULL; owner = &lib->next ) {
        if( lib->libname[0] < priority ) {
            break;
        }
        if( FNAMECMPSTR( lib->libname + 1, name ) == 0 ) {
            return;
        }
    }
    new_owner = owner;
    for( ; (lib = *owner) != NULL; owner = &lib->next ) {
        if( FNAMECMPSTR( lib->libname + 1, name ) == 0 ) {
            *owner = lib->next;
            break;
        }
    }
    if( lib == NULL ) {
        len = strlen( name ) + 1;
        lib = CMemAlloc( offsetof( library_list, libname ) + len + 1 );
        memcpy( lib->libname + 1, name, len );
    }
    lib->libname[0] = priority;
    lib->next = *new_owner;
    *new_owner = lib;
}

static void GetLibraryNames( void )
/********************************/
{
    while( IS_ID_OR_KEYWORD( CurToken ) || CurToken == T_STRING ) {
        AddLibraryName( Buffer, USER_LIB_PRIO );
        PPNextToken();
    }
}

/* forms:
 *
 *      #pragma library
 *      #pragma library (<libraries name list>)
 */
static void pragLibs( void )
/**************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( CurToken == T_LEFT_PAREN ) {
        PPNextToken();
        GetLibraryNames();
        MustRecog( T_RIGHT_PAREN );
    } else {
        CompFlags.pragma_library = true;
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 *      #pragma comment ( comment_type [, "comment_string"] )
 */
static void pragComment( void )
/*****************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPNextToken();
        if( PragRecogId( "lib" ) ) {
            if( ExpectingToken( T_COMMA ) ) {
                PPNextToken();
            }
            GetLibraryNames();
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

void SetPackAmount( unsigned amount )
/***********************************/
{
    if( amount <= 1 ) {
        PackAmount = 1;
    } else if( amount <= 2 ) {
        PackAmount = 2;
    } else if( amount <= 4 ) {
        PackAmount = 4;
    } else if( amount <= 8 ) {
        PackAmount = 8;
    } else {
        PackAmount = 16;
    }
}

static void getPackArgs( void )
/****************************/
{
    /* check to make sure it is a numeric token */
    if( PragRecogId( "push" ) ) {
        pushPrag( &TOGGLE_STK( pack ), (unsigned)PackAmount );
        if( CurToken == T_COMMA ) {
            PPNextToken();
            if( ExpectingConstant() ) {
                SetPackAmount( Constant );
            }
            PPNextToken();
        }
    } else if( PragRecogId( "pop" ) ) {
        unsigned    value;

        if( popPrag( &TOGGLE_STK( pack ), &value ) ) {
            PackAmount = (align_type)value;
        }
    } else {
        CErr1( ERR_NOT_A_CONSTANT_EXPR );
    }
}

/* forms:
 *
 *      #pragma pack ()
 *      #pragma pack ( n )
 *      #pragma pack ( push )
 *      #pragma pack ( pop )
 *      #pragma pack ( push, n )
 */
static void pragPack( void )
/**************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPNextToken();
        if( CurToken == T_CONSTANT ) {
            SetPackAmount( Constant );
            PPNextToken();
        } else if( IS_ID_OR_KEYWORD( CurToken ) ) {
            getPackArgs();
        } else if( CurToken == T_RIGHT_PAREN ) {
            PackAmount = GblPackAmount;
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

textsegment *NewTextSeg( const char *name, const char *suffix, const char *classname )
/************************************************************************************/
{
    textsegment     *tseg;
    size_t          len1;
    size_t          len2;
    size_t          len3;

    len1 = strlen( name );
    len2 = strlen( suffix ) + 1;
    len3 = strlen( classname ) + 1;
    tseg = CMemAlloc( offsetof( textsegment, segname ) + len1 + len2 + len3 );
    tseg->class = len1 + len2;
    memcpy( tseg->segname, name, len1 );
    memcpy( tseg->segname + len1, suffix, len2 );
    memcpy( tseg->segname + tseg->class, classname, len3 );
    tseg->next = TextSegList;
    TextSegList = tseg;
    return( tseg );
}

textsegment *LkSegName( const char *segname, const char *classname )
/******************************************************************/
{
    textsegment     *tseg;
    size_t          len1;
    size_t          len2;

    len1 = strlen( segname ) + 1;
    len2 = strlen( classname ) + 1;
    for( tseg = TextSegList; tseg != NULL; tseg = tseg->next ) {
        if( memcmp( tseg->segname, segname, len1 ) == 0 ) {
            if( memcmp( tseg->segname + tseg->class, classname, len2 ) == 0 ) {
                return( tseg );
            }
        }
    }
    return( NewTextSeg( segname, "", classname ) );
}

/* forms:
 *
 *      #pragma alloc_text ( seg_name, fn [, fn] )
 */
static void pragAllocText( void )
/*******************************/
{
    struct textsegment  *tseg;
    SYM_HANDLE          sym_handle;
    SYM_ENTRY           sym;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPNextToken();
        /* current token can be an T_ID or a T_STRING */
        tseg = LkSegName( Buffer, "" );
        PPNextToken();
        for( ;; ) {
            MustRecog( T_COMMA );
            /* current token can be an T_ID or a T_STRING */
            sym_handle = Sym0Look( CalcHash( Buffer, TokenLen ), Buffer );
            if( sym_handle == SYM_NULL ) {
                /* error */
            } else {
                SymGet( &sym, sym_handle );
                if( sym.flags & SYM_FUNCTION ) {
                    sym.seginfo = tseg;
                    SymReplace( &sym, sym_handle );
                } else {
                    /* error, must be function */
                }
            }
            PPNextToken();
            if( CurToken == T_RIGHT_PAREN )
                break;
            if( CurToken == T_EOF )
                break;
            if( CurToken == T_NULL ) {
                break;
            }
        }
#if _CPU == 8086 || _CPU == 386
        CompFlags.multiple_code_segments = true;
#endif
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

static void changeLevel( unsigned level, int msg_index )
/******************************************************/
{
    if( msg_level[msg_index].level != level ) {
        msg_level[msg_index].level = level;
        if( level == WLEVEL_DISABLED ) {
            /* disable message */
            if( msg_level[msg_index].enabled ) {
                msg_level[msg_index].enabled = false;
            }
        } else {
            /* enable message */
            if( !msg_level[msg_index].enabled ) {
                msg_level[msg_index].enabled = true;
            }
        }
    }
}

static void changeStatus( bool enabled, int msg_index )
/*****************************************************/
{
    if( msg_level[msg_index].enabled != enabled ) {
        msg_level[msg_index].enabled = enabled;
    }
}

static void warnChangeLevel( unsigned level, msg_codes msgnum )
/*************************************************************/
/* CHANGE WARNING LEVEL FOR A MESSAGE */
{
    int     msg_index;

    msg_index = GetMsgIndex( msgnum );
    if( msg_index < 0 ) {
        CWarn2( WARN_PRAG_WARNING_BAD_MESSAGE, ERR_PRAG_WARNING_BAD_MESSAGE, msgnum );
        return;
    }
    switch( msg_level[msg_index].type ) {
    case MSG_TYPE_ERROR :
    case MSG_TYPE_INFO :
    case MSG_TYPE_ANSIERR :
        CWarn2( WARN_PRAG_WARNING_BAD_MESSAGE, ERR_PRAG_WARNING_BAD_MESSAGE, msgnum );
        break;
    case MSG_TYPE_WARNING :
    case MSG_TYPE_ANSI :
    case MSG_TYPE_ANSIWARN :
        changeLevel( level, msg_index );
        break;
    }
}

static void warnChangeLevels( unsigned level )
/********************************************/
/* CHANGE WARNING LEVELS FOR ALL MESSAGES */
{
    int     msg_index;          // - index for number

    for( msg_index = 0; msg_index < MESSAGE_COUNT; msg_index++ ) {
        switch( msg_level[msg_index].type ) {
        case MSG_TYPE_WARNING :
        case MSG_TYPE_ANSI :
        case MSG_TYPE_ANSIWARN :
            changeLevel( level, msg_index );
            break;
        }
    }
}

void WarnEnableDisable( bool enabled, msg_codes msgnum )
/******************************************************/
{
    int     msg_index;

    msg_index = GetMsgIndex( msgnum );
    if( msg_index < 0 ) {
        CWarn2( WARN_PRAG_WARNING_BAD_MESSAGE, ERR_PRAG_WARNING_BAD_MESSAGE, msgnum );
        return;
    }
    switch( msg_level[msg_index].type ) {
    case MSG_TYPE_ERROR :
    case MSG_TYPE_INFO :
    case MSG_TYPE_ANSIERR :
        CWarn2( WARN_PRAG_WARNING_BAD_MESSAGE, ERR_PRAG_WARNING_BAD_MESSAGE, msgnum );
        break;
    case MSG_TYPE_WARNING :
    case MSG_TYPE_ANSI :
    case MSG_TYPE_ANSIWARN :
        changeStatus( enabled, msg_index );
        break;
    }
}

/*
 * forms: #pragma warning # level   (change message # to have level "level)
 *      : #pragma warning * level   (change all messages to have level "level)
 *
 *   "level" must be digit (0-5)
 *   "level==0" implies warning will be treated as an error
 */
static bool pragWarning( void )
/*****************************/
/* PROCESS #PRAGMA WARNING */
{
    unsigned msgnum;            // - message number
    unsigned level;             // - new level
    bool change_all;            // - true ==> change all levels
    bool ignore;

    ignore = false;
    change_all = false;
    msgnum = 0;
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( CurToken == T_TIMES ) {
        change_all = true;
    } else if( CurToken == T_CONSTANT ) {
        msgnum = Constant;
    } else {
        // ignore; MS or other vendor's #pragma
        ignore = true;
    }
    if( !ignore ) {
        NextToken();
        if( CurToken == T_CONSTANT ) {
            level = Constant;
            NextToken();
            if( change_all ) {
                warnChangeLevels( level );
            } else {
                warnChangeLevel( level, msgnum );
            }
        } else {
            CWarn1( WARN_PRAG_WARNING_BAD_LEVEL, ERR_PRAG_WARNING_BAD_LEVEL );
            NextToken();
        }
    }
    PPCTL_DISABLE_MACROS();
    return( ignore );
}

/* forms:
 *
 *    #pragma enable_message( messageNo )
 *    #pragma disable_message( messageNo )
 *
 * disable/enable display of selected message number
 */
static void pragEnableDisableMessage( bool enabled )
/**************************************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPNextToken();
        while( CurToken == T_CONSTANT ) {
            WarnEnableDisable( enabled, Constant );
            PPNextToken();
            if( CurToken == T_COMMA ) {
                PPNextToken();
            }
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

static char *collectStrings( char *message )
{
    size_t  len;
    size_t  new_len;

    len = 0;
    while( CurToken == T_STRING ) {
        new_len = len + strlen( Buffer );
        message = CMemRealloc( message, new_len + 1 );
        strcpy( message + len, Buffer );
        len = new_len;
        PPNextToken();
    }
    return( message );
}

/* form:
 *
 * #pragma message ("one or more " "long message " "strings")
 *
 * output these strings to stdout
 * this output is _not_ dependent on setting
 * of #pragma enable_message or disable_message.
 */
static void pragMessage( void )
/*****************************/
{
    char    *message;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPNextToken();
        if( CurToken == T_STRING ) {
            message = CMemAlloc( 1 );
            message[0] = '\0';
            message = collectStrings( message );
            BannerMsg( message );
            CMemFree( message );
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * (1) #pragma enum int
 * (2) #pragma enum minimum
 * (3) #pragma enum original
 * (4) #pragma enum pop
 *
 * The pragma affects the underlying storage-definition for subsequent
 * enum declarations.
 *
 * (1) make int the underlying storage definition (same as -ei)
 * (2) minimize the underlying storage definition (same as no -ei)
 * (3) reset back to result of command-line parsing
 * (4) restore previous value
 *
 * 1-3 all push previous value before affecting value
 */
static void pragEnum( void )
/**************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( PragRecogId( "int" ) ) {
        pushPrag( &TOGGLE_STK( enum ), CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = true;
    } else if( PragRecogId( "minimum" ) ) {
        pushPrag( &TOGGLE_STK( enum ), CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = false;
    } else if( PragRecogId( "original" ) ) {
        pushPrag( &TOGGLE_STK( enum ), CompFlags.make_enums_an_int );
        CompFlags.make_enums_an_int = CompFlags.original_enum_setting;
    } else if( PragRecogId( "pop" ) ) {
        unsigned    value;

        if( popPrag( &TOGGLE_STK( enum ), &value ) ) {
            CompFlags.make_enums_an_int = ( value != 0 );
        }
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * #pragma intrinsic ( fn [, fn] )
 */
static void pragIntrinsic( int intrinsic )
/****************************************/
{
    SYM_HANDLE  sym_handle;
    SYM_ENTRY   sym;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        for( PPNextToken(); IS_ID_OR_KEYWORD( CurToken ); PPNextToken() ) {
            sym_handle = SymLook( HashValue, Buffer );
            if( sym_handle != SYM_NULL ) {
                SymGet( &sym, sym_handle );
                sym.flags &= ~ SYM_INTRINSIC;
                if( intrinsic )
                    sym.flags |= SYM_INTRINSIC;
                SymReplace( &sym, sym_handle );
            }
            PPNextToken();
            if( CurToken != T_COMMA ) {
                break;
            }
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * #pragma code_seg ( seg_name [, class_name] )
 */
static void pragCodeSeg( void )
/*****************************/
{
    textsegment     *tseg;
    char            *segname;
    char            *classname;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( CurToken == T_LEFT_PAREN ) {
        tseg = NULL;
        PPNextToken();
        if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
            segname = CStrSave( Buffer );
            classname = CStrSave( "" );
            PPNextToken();
            if( CurToken == T_COMMA ) {
                PPNextToken();
                if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
                    CMemFree( classname );
                    classname = CStrSave( Buffer );
//                  CodeClassName = CStrSave( Buffer ); */
                    PPNextToken();
                }
            }
            tseg = LkSegName( segname, classname );
            CMemFree( segname );
            CMemFree( classname );
        }
        MustRecog( T_RIGHT_PAREN );
        DefCodeSegment = tseg;
#if _CPU == 8086 || _CPU == 386
        CompFlags.multiple_code_segments = true;
#endif
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * #pragma data_seg ( seg_name [, class_name] )
 */
static void pragDataSeg( void )
/*****************************/
{
    char        *segname;
    segment_id  segid;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( CurToken == T_LEFT_PAREN ) {
        segid = SEG_NULL;
        PPNextToken();
        if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
            segname = CStrSave( Buffer );
            PPNextToken();
            if( CurToken == T_COMMA ) {
                PPNextToken();
                if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
                    segid = AddSegName( segname, Buffer, SEGTYPE_DATA );
                    PPNextToken();
                } else {
                    segid = AddSegName( segname, NULL, SEGTYPE_DATA );
                }
            } else {
                segid = AddSegName( segname, NULL, SEGTYPE_DATA );
            }
            CMemFree( segname );
        }
        MustRecog( T_RIGHT_PAREN );
        DefDataSegment = segid;
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * #pragma unroll ( n )
 */
static void pragUnroll( void )
/****************************/
{
    unroll_type unroll_count;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        unroll_count = 0;
        PPNextToken();
        if( CurToken == T_CONSTANT ) {
            unroll_count = ( Constant > 255 ) ? 255 : (unroll_type)Constant;
            PPNextToken();
        }
        UnrollCount = unroll_count;
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * (1) #pragma read_only_file
 * (2) #pragma read_only_file "file"*
 *
 * (1) causes current file to be marked read-only
 * (2) causes indicated file to be marked read-only
 *      - file must have started inclusion (may have completed)
 */
static void pragReadOnlyFile( void )
/**********************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( CurToken == T_STRING ) {
        do {
            SrcFileReadOnlyFile( Buffer );
            PPNextToken();
            if( CurToken == T_SEMI_COLON ) {
                PPNextToken();
            }
        } while( CurToken == T_STRING );
    } else {
        SrcFileReadOnlyFile( NULL );
    }
    PPCTL_DISABLE_MACROS();
}


/* forms:
 *
 *  #pragma read_only_directory "directory"*
 *
 * (1) causes all files within directory to be marked read-only
 */
static void pragReadOnlyDir( void )
/*********************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    while( CurToken == T_STRING ) {
        SrcFileReadOnlyDir( Buffer );
        PPNextToken();
        if( CurToken == T_SEMI_COLON ) {
            PPNextToken();
        }
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * (1) #pragma include_alias ( "alias_name", "real_name" )
 * (2) #pragma include_alias ( <alias_name>, <real_name> )
 *
 * causes include directives referencing alias_name to be refer
 * to real_name instead
 */
static void pragIncludeAlias( void )
/**********************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPNextToken();
        if( CurToken == T_STRING ) {
            char    *alias_name;

            alias_name = CStrSave( Buffer );
            PPNextToken();
            MustRecog( T_COMMA );
            if( CurToken == T_STRING ) {
                SrcFileIncludeAlias( alias_name, Buffer, false );
                PPNextToken();
            }
            CMemFree( alias_name );
        } else if( CurToken == T_LT ) {
            char    a_buf[82];    /* same size as CInclude() in cmac2.c */
            char    r_buf[82];

            a_buf[0] = '\0';
            for( ; PPNextToken() != T_NULL; ) {
                if( CurToken == T_GT ) {
                    PPNextToken();
                    break;
                }
                strncat( a_buf, Buffer, sizeof( a_buf ) - 2 );
            }
            MustRecog( T_COMMA );
            if( CurToken == T_LT ) {
                r_buf[0] = '\0';
                for( ; PPNextToken() != T_NULL; ) {
                    if( CurToken == T_GT ) {
                        PPNextToken();
                        break;
                    }
                    strncat( r_buf, Buffer, sizeof( r_buf ) - 2 );
                }
                SrcFileIncludeAlias( a_buf, r_buf, true );
            }
        }
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * #pragma once
 *
 * include file once
 */
static void pragOnce( void )
/**************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    SetSrcFNameOnce();
    PPCTL_DISABLE_MACROS();
}

static void OptionPragSTDC( void )
/********************************/
{
    if( PragRecogName( "ON" ) ) {
    } else if( PragRecogName( "OFF" ) ) {
    } else if( PragRecogName( "DEFAULT" ) ) {
    }
}

/* forms:
 *
 * #pragma STDC (FP_CONTRACT|FENV_ACCESS|CX_LIMITED_RANGE) (ON|OFF|DEFAULT)
 */
static void pragSTDC( void )
/**************************/
{
    PPCTL_DISABLE_MACROS();
    PPNextToken();
    if( PragRecogName( "FP_CONTRACT" ) ) {
        OptionPragSTDC();
    } else if( PragRecogName( "FENV_ACCESS" ) ) {
        OptionPragSTDC();
    } else if( PragRecogName( "CX_LIMITED_RANGE" ) ) {
        OptionPragSTDC();
    }
}

void AddExtRefN ( const char *name )
/**********************************/
{
    extref_info     **extref;
    extref_info     *new_extref;
    size_t          len;

    for( extref = &ExtrefInfo; *extref != NULL; extref = &(*extref)->next )
        ; /* nothing to do */
    len = strlen( name ) + 1;
    new_extref = CMemAlloc( sizeof( extref_info ) - 1 + len );
    memcpy( new_extref->name, name, len );
    new_extref->symbol = NULL;
    new_extref->next = NULL;
    *extref = new_extref;
}

void AddExtRefS ( SYM_HANDLE sym )
/********************************/
{
    extref_info  **extref;
    extref_info  *new_extref;

    for( extref = &ExtrefInfo; *extref != NULL; extref = &(*extref)->next )
        ; /* nothing to do */
    new_extref = CMemAlloc( sizeof( extref_info ) );
    new_extref->symbol = sym;
    new_extref->name[0] = '\0';
    new_extref->next = NULL;
    *extref = new_extref;
}

static void parseExtRef ( void )
/******************************/
{
    SYM_HANDLE   extref_sym;

    if( CurToken == T_STRING ) {
        AddExtRefN( Buffer );
    } else {
        extref_sym = SymLook( HashValue, Buffer );
        if( extref_sym != NULL ) {
            AddExtRefS( extref_sym );
        } else {
            CErr2p( ERR_UNDECLARED_SYM, Buffer );
        }
    }
}

/* forms:
 *
 * #pragma extref ( symbolid [, ...] )
 * #pragma extref ( "symbolname" [, ...] )
 */
static void pragExtRef( void )
/****************************/
{
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( CurToken == T_LEFT_PAREN ) {
        do {
            PPNextToken();
            if( !IS_ID_OR_KEYWORD( CurToken ) && CurToken != T_STRING )
                break;
            parseExtRef();
            PPNextToken();
        } while( CurToken == T_COMMA );
        MustRecog( T_RIGHT_PAREN );
    } else if( IS_ID_OR_KEYWORD( CurToken ) || CurToken == T_STRING ) {
        parseExtRef();
        PPNextToken();
    }
    PPCTL_DISABLE_MACROS();
}

/* forms:
 *
 * #pragma alias(id1/"name1", id2/"name2")
 *
 * Causes linker to replace references to id1/name1 with references
 * to id2/name2. Both the alias and the substituted symbol may be defined
 * either as a string name or an id of existing symbol.
 */
static void pragAlias( void )
/***************************/
{
    SYM_HANDLE      alias_sym;
    SYM_HANDLE      subst_sym;
    const char      *alias_name;
    const char      *subst_name;
    alias_list      **alias;
    alias_list      *new_alias;

    alias_name = NULL;
    subst_name = NULL;
    alias_sym = SYM_NULL;
    subst_sym = SYM_NULL;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPNextToken();
        if( CurToken == T_ID ) {
            alias_sym = SymLook( HashValue, Buffer );
            if( alias_sym == SYM_NULL ) {
                CErr2p( ERR_UNDECLARED_SYM, Buffer );
            }
        } else if( CurToken == T_STRING ) {
            alias_name = CStrSave( Buffer );
        }
        PPNextToken();
        MustRecog( T_COMMA );
        if( CurToken == T_ID ) {
            subst_sym = SymLook( HashValue, Buffer );
            if( subst_sym == 0 ) {
                CErr2p( ERR_UNDECLARED_SYM, Buffer );
            }
        } else if( CurToken == T_STRING ) {
            subst_name = CStrSave( Buffer );
        }
        PPNextToken();
        MustRecog( T_RIGHT_PAREN );
    }
    PPCTL_DISABLE_MACROS();

    /* Add a new alias record - if it's valid - to the list */
    if( ( alias_name != NULL || alias_sym != SYM_NULL ) && ( subst_name != NULL || subst_sym != SYM_NULL ) ) {
        for( alias = &AliasHead; *alias != NULL; alias = &(*alias)->next )
            ; /* nothing to do */
        new_alias = (void *)CMemAlloc( sizeof( alias_list ) );
        new_alias->next = NULL;
        if( alias_name ) {
            new_alias->name = alias_name;
        } else {
            new_alias->a_sym = alias_sym;
        }
        if( subst_name ) {
            new_alias->subst = subst_name;
        } else {
            new_alias->s_sym = subst_sym;
        }
        *alias = new_alias;
    }
}

void CPragma( void )
/******************/
{
    bool    check_end;

    /* Note that the include_alias pragma must always be processed
     * because it's intended for the preprocessor, not the compiler.
     */
    CompFlags.in_pragma = true;
    check_end = true;
    PPNextToken();
    if( IS_ID_OR_KEYWORD( CurToken ) && pragmaNameRecog( "include_alias" ) ) {
        pragIncludeAlias();
    } else if( CompFlags.cpp_mode ) {
        if( CompFlags.cpp_output ) {
            CppPuts( "#pragma " );
        }
        if( CurToken != T_NULL ) {
            CppPrtToken( CurToken );
            PPCTL_ENABLE_MACROS();
            for( CurToken = GetNextToken(); CurToken != T_NULL && CurToken != T_PRAGMA_END; CurToken = GetNextToken() ) {
                CppPrtToken( CurToken );
            }
            PPCTL_DISABLE_MACROS();
        }
    } else if( IS_ID_OR_KEYWORD( CurToken ) ) {
        if( pragmaNameRecog( "on" ) ) {
            pragOptions( 1 );
        } else if( pragmaNameRecog( "off" ) ) {
            pragOptions( 0 );
        } else if( pragmaNameRecog( "pop" ) ) {
            pragOptions( -1 );
        } else if( pragmaNameRecog( "aux" ) || pragmaNameRecog( "linkage" ) ) {
            PragAux();
        } else if( pragmaNameRecog( "library" ) ) {
            pragLibs();
        } else if( pragmaNameRecog( "comment" ) ) {
            pragComment();
        } else if( pragmaNameRecog( "pack" ) ) {
            pragPack();
        } else if( pragmaNameRecog( "alloc_text" ) ) {
            pragAllocText();
        } else if( pragmaNameRecog( "code_seg" ) ) {
            pragCodeSeg();
        } else if( pragmaNameRecog( "data_seg" ) ) {
            pragDataSeg();
        } else if( pragmaNameRecog( "warning" ) ) {
            if( pragWarning() ) {
                /* ignore #pragma warning */
                /* skip rest of line */
                check_end = false;
            }
        } else if( pragmaNameRecog( "disable_message" ) ) {
            pragEnableDisableMessage( false );
        } else if( pragmaNameRecog( "enable_message" ) ) {
            pragEnableDisableMessage( true );
        } else if( pragmaNameRecog( "message" ) ) {
            pragMessage();
        } else if( pragmaNameRecog( "intrinsic" ) ) {
            pragIntrinsic( 1 );
        } else if( pragmaNameRecog( "function" ) ) {
            pragIntrinsic( 0 );
        } else if( pragmaNameRecog( "enum" ) ) {
            pragEnum();
        } else if( pragmaNameRecog( "read_only_file" ) ) {
            pragReadOnlyFile();
        } else if( pragmaNameRecog( "read_only_directory" ) ) {
            pragReadOnlyDir();
        } else if( pragmaNameRecog( "once" ) ) {
            pragOnce();
        } else if( pragmaNameRecog( "unroll" ) ) {
            pragUnroll();
        } else if( pragmaNameRecog( "STDC" ) ) {
            pragSTDC();
        } else if( pragmaNameRecog( "extref" ) ) {
            pragExtRef();
        } else if( pragmaNameRecog( "alias" ) ) {
            pragAlias();
        } else {
            check_end = false;
        }
    } else {
        check_end = false;
    }
    endOfPragma( check_end );
    CompFlags.in_pragma = false;
}

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
* Description:  Generic (target independent) pragma processing.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "caux.h"
#include "cfeinfo.h"

struct  pack_info {
    struct pack_info *next;
    int         pack_amount;
} *PackInfo;

struct enums_info {
    struct enums_info *next;
    int    make_enums;
} *EnumInfo;


// local variables
static struct toggle ToggleNames[] = {
    #define TOGDEF( a, b ) {  #a, b },
    #include "togdef.h"
    #undef TOGDEF
        { NULL, 0 }
    };


void CPragmaInit( void )
{
    TextSegList = NULL;
    PackInfo = NULL;
    EnumInfo = NULL;
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

    while( TextSegList != NULL ) {
        junk = TextSegList;
        TextSegList = TextSegList->next;
        CMemFree( junk );
    }

    while( PackInfo != NULL ) {
        junk = PackInfo;
        PackInfo = PackInfo->next;
        CMemFree( junk );
    }

    while( EnumInfo != NULL ) {
        junk = EnumInfo;
        EnumInfo = EnumInfo->next;
        CMemFree( junk );
    }

    while( HeadLibs != NULL ) {
        junk = HeadLibs;
        HeadLibs = HeadLibs->next;
        CMemFree( junk );
    }

    while( AliasHead != NULL ) {
        junk = AliasHead;
        AliasHead = AliasHead->next;
        CMemFree( junk );
    }

    while( ExtrefInfo != NULL ) {
        junk = ExtrefInfo;
        ExtrefInfo = ExtrefInfo->next;
        CMemFree( junk );
    }
}

local void EndOfPragma( void )
/*****************************/
{
    if( CurToken == T_SEMI_COLON )
        NextToken();
    ExpectingToken( T_NULL );
    while( CurToken != T_NULL && CurToken != T_EOF ) {
        NextToken();
    }
}


int SetToggleFlag( char const *name, int const value )
/****************************************************/
{
    int     i;
    char   *pnt;
    int     ret;

    ret = FALSE;
    for( i = 0; (pnt = ToggleNames[ i ].name) != NULL; ++i ) {
        if( strcmp( pnt, name ) == 0 ) {
            if( value == 0 ) {
                Toggles &= ~ToggleNames[ i ].flag;
            } else {
                Toggles |= ToggleNames[ i ].flag;
            }
            ret = TRUE;
            break;
        }
    }
    return( ret );
}

local int PragIdRecog( char *what )
/*********************************/
{
    char    *p = Buffer;
    int     rc;

    if( *p == '_' ) {
        ++p;
        if( *p == '_' ) {
            ++p;
        }
    }
    if( (rc = stricmp( what, p )) == 0 ) {
        NextToken();
    }
    return( rc == 0 );
}


local int startPragRecog( char *what )
/************************************/
{
    int rc;

    PPCTL_ENABLE_MACROS();
    rc = PragIdRecog( what );
    PPCTL_DISABLE_MACROS();
    return( rc );
}


int PragRecog( char *what )
/*************************/
{
    if( IS_ID_OR_KEYWORD( CurToken ) ) {
        return( PragIdRecog( what ) );
    }
    return( 0 );
}

local void PragFlag( int value )
/******************************/
{

    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        while( IS_ID_OR_KEYWORD( CurToken ) ) {
            SetToggleFlag( Buffer, value );
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    }
}

void AddLibraryName( char *name, char priority )
/**********************************************/
{
    library_list    **new_owner;
    library_list    **owner;
    library_list    *lib;
    int             len;

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
        len = strlen( name );
        lib = CMemAlloc( offsetof( library_list, libname ) + len + 2 );
        memcpy( lib->libname + 1, name, len + 1 );
    }
    lib->libname[0] = priority;
    lib->next = *new_owner;
    *new_owner = lib;
}

local void GetLibraryNames( void )
/********************************/
{
    while( IS_ID_OR_KEYWORD( CurToken ) || CurToken == T_STRING ) {
        AddLibraryName( Buffer, USER_LIB_PRIO );
        NextToken();
    }
}

local void PragLibs( void )
/*************************/
{
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        GetLibraryNames();
        MustRecog( T_RIGHT_PAREN );
    } else {
        CompFlags.pragma_library = 1;
    }
}

local void PragComment( void )
/****************************/
{
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        if( PragRecog( "lib" ) ) {
            if( ExpectingToken( T_COMMA ) ) {
                NextToken();
            }
            GetLibraryNames();
        }
        MustRecog( T_RIGHT_PAREN );
    }
}

local void SetPackAmount( void )
/******************************/
{
    PackAmount = Constant;
    switch( PackAmount ) {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:                                    /* 09-oct-92 */
        break;
    default:
        PackAmount = 1;
    }
}

local void getPackArgs( void )
/****************************/
{
    struct pack_info    *pi;

    /* check to make sure it is a numeric token */
    if( PragIdRecog( "push" ) ) {                  /* 29-sep-94 */
        pi = (struct pack_info *)CMemAlloc( sizeof( struct pack_info ) );
        pi->next = PackInfo;
        pi->pack_amount = PackAmount;
        PackInfo = pi;
        if( CurToken == T_COMMA ) {
            PPCTL_ENABLE_MACROS();
            NextToken();
            PPCTL_DISABLE_MACROS();
            if( ExpectingConstant() ) {
                SetPackAmount();
            }
            NextToken();
        }
    } else if( PragIdRecog( "pop" ) ) {
        pi = PackInfo;
        if( pi != NULL ) {
            PackAmount = pi->pack_amount;
            PackInfo = pi->next;
            CMemFree( pi );
        }
    } else {
        CErr1( ERR_NOT_A_CONSTANT_EXPR );
    }
}

local void PragPack( void )
/*************************/
{
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPCTL_ENABLE_MACROS();
        NextToken();
        PPCTL_DISABLE_MACROS();
        if( CurToken == T_CONSTANT ) {
            SetPackAmount();
            NextToken();
        } else if( IS_ID_OR_KEYWORD( CurToken ) ) {
            getPackArgs();
        } else if( CurToken == T_RIGHT_PAREN ) {
            PackAmount = GblPackAmount;
        }
        MustRecog( T_RIGHT_PAREN );
    }
}

struct magic_words {
    char            *name;
    aux_info        *info;
} MagicWords[] = {
    #define pick(a,b,c) { b, c },
    #include "auxinfo.h"
    #undef pick
};

aux_info *MagicKeyword( char *name )
{
    int         i;

    if( *name == '_' ) {
        ++name;
        if( *name == '_' ) {
            ++name;
        }
    }
    for( i = 0; MagicWords[ i ].name != NULL; ++i ) {
        if( strcmp( name, MagicWords[ i ].name ) == 0 ) {
            break;
        }
    }
    return( MagicWords[ i ].info );
}


void CreateAux( char *id )
/************************/
{
    CurrEntry = (aux_entry *)CMemAlloc( offsetof( aux_entry, name ) + strlen( id ) + 1 );
    strcpy( CurrEntry->name, id );
#if _CPU == 370
    CurrEntry->offset = -1;
#endif
}


void SetCurrInfo( char *name )
/****************************/
{
    SYM_HANDLE      sym_handle;
    SYM_ENTRY       sym;
    type_modifiers  sym_attrib = FLAG_NONE;

    CurrInfo = MagicKeyword( name );
    if( CurrInfo == NULL ) {
        if( CurrAlias == NULL ) {
            sym_handle = SymLook( HashValue, name );
            if( sym_handle != 0 ) {
                SymGet( &sym, sym_handle );
                sym_attrib = sym.attrib;
            }
            CurrAlias = GetLangInfo( sym_attrib );
        }
        CreateAux( name );
    } else if( CurrAlias == NULL ) {
        CurrAlias = CurrInfo;
    }
}


void PragCurrAlias( char *name )
/******************************/
{
    aux_entry *search;

    search = NULL;
    CurrAlias = MagicKeyword( name );
    if( CurrAlias == NULL ) {
        search = AuxLookup( name );
        if( search != NULL ) {
            CurrAlias = search->info;
        }
    }
}


void XferPragInfo( char *from, char *to )
/***************************************/
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


local void CopyLinkage( void )
/****************************/
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


local void CopyParms( void )
/**************************/
{
    int         i;
    hw_reg_set  *regs;

    if( CurrInfo->parms != CurrAlias->parms )
        return;
    if( IsAuxParmsBuiltIn( CurrInfo->parms ) )
        return;
    for( i = 1, regs = CurrInfo->parms;
         !HW_CEqual( *regs, HW_EMPTY ); ++i, ++regs )
        ;
    i *= sizeof( hw_reg_set );
    regs = (hw_reg_set *)CMemAlloc( i );
    memcpy( regs, CurrInfo->parms, i );
    CurrInfo->parms = regs;
}

local void CopyCode( void )
/*************************/
{
    byte_seq    *code;
    int         size;
//TODO deal with reloc list
    if( CurrInfo->code == NULL )
        return;
    if( CurrInfo->code != CurrAlias->code )
        return;
    size = offsetof( byte_seq, data ) + CurrInfo->code->length;
    code = (byte_seq *)CMemAlloc( size );
    memcpy( code, CurrInfo->code, size );
    CurrInfo->code = code;
}

local void CopyObjName( void )
/****************************/
{
    if( CurrInfo->objname == NULL )
        return;
    if( CurrInfo->objname != CurrAlias->objname )
        return;
    CurrInfo->objname = CStrSave( CurrInfo->objname );
}

#if _CPU == _AXP
local void CopyExceptRtn( void )
/******************************/
{
    if( CurrInfo->except_rtn == NULL )
        return;
    if( CurrInfo->except_rtn != CurrAlias->except_rtn )
        return;
    CurrInfo->except_rtn = CStrSave( CurrInfo->except_rtn );
}
#endif

void PragEnding( void )
/*********************/
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
    if( CurrEntry->name != NULL && CurrEntry->info != NULL && CurrEntry->info->code != NULL ) {
        SYM_HANDLE  sym_handle;
        SYM_ENTRY   sym;

        if( 0 != (sym_handle = SymLook( CalcHash( CurrEntry->name, strlen( CurrEntry->name ) ), CurrEntry->name )) ) {
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
        NextToken();
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


hw_reg_set PragRegList( void )
/****************************/
{
    hw_reg_set  res, reg;
    TOKEN       close;
    char        buf[ 80 ];

    HW_CAsgn( res, HW_EMPTY );
    HW_CAsgn( reg, HW_EMPTY );
    close = PragRegSet();
    if( close != T_NULL ) {
        PPCTL_ENABLE_MACROS();
        NextToken();
        *buf = '\0';
        for( ; CurToken != close; ) {
            strcat( buf, Buffer );
            if( CurToken != T_BAD_CHAR ) {
                reg = PragRegName( buf );
                HW_TurnOn( res, reg );
                *buf = '\0';
            }
            NextToken();
        }
        PPCTL_DISABLE_MACROS();
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
    hw_reg_set  buff[ MAXIMUM_PARMSETS ];

    list = PragRegList();
    i = 0;
    while( !HW_CEqual( list, HW_EMPTY ) && ( i != MAXIMUM_PARMSETS ) ) {
        buff[ i++ ] = list;
        list = PragRegList();
    }
    if( !HW_CEqual( list, HW_EMPTY ) ) {
        CErr1( ERR_TOO_MANY_PARM_SETS );
    }
    HW_CAsgn( buff[ i ], HW_EMPTY );
    i++;
    i *= sizeof( hw_reg_set );
    sets = (hw_reg_set *)CMemAlloc( i );
    memcpy( sets, buff, i );
    return( sets );
}

textsegment *NewTextSeg( char *name, char *suffix, char *classname )
/******************************************************************/
{
    textsegment     *seg;
    int             len1;
    int             len2;
    int             len3;

    len1 = strlen( name );
    len2 = strlen( suffix ) + 1;
    len3 = strlen( classname ) + 1;
    seg = CMemAlloc( offsetof( textsegment, segname ) + len1 + len2 + len3 );
    memcpy( seg->segname, name, len1 );
    memcpy( &seg->segname[ len1 ], suffix, len2 );
    memcpy( &seg->segname[ len1 + len2 ], classname, len3 );
    seg->next = TextSegList;
    TextSegList = seg;
    return( seg );
}

textsegment *LkSegName( char *segname, char *classname )
/******************************************************/
{
    textsegment     *seg;
    int             len;

    len = strlen( segname ) + 1;
    for( seg = TextSegList; seg != NULL; seg = seg->next ) {
        if( strcmp( seg->segname, segname ) == 0 ) {
            if( strcmp( &seg->segname[ len ], classname ) == 0 ) {
                return( seg );
            }
        }
    }
    return( NewTextSeg( segname, "", classname ) );
}

local void PragAllocText( void )                              /* 26-oct-91 */
/******************************/
{
    struct textsegment  *seg;
    SYM_HANDLE          sym_handle;
    auto SYM_ENTRY      sym;

    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        /* current token can be an T_ID or a T_STRING */
        seg = LkSegName( Buffer, "" );
        NextToken();
        for( ; ; ) {
            MustRecog( T_COMMA );
            /* current token can be an T_ID or a T_STRING */
            sym_handle = Sym0Look( CalcHash( Buffer, strlen( Buffer ) ), Buffer );
            if( sym_handle == 0 ) {
                /* error */
            } else {
                SymGet( &sym, sym_handle );
                if( sym.flags & SYM_FUNCTION ) {
                    sym.seginfo = seg;
                    SymReplace( &sym, sym_handle );
                } else {
                    /* error, must be function */
                }
            }
            NextToken();
            if( CurToken == T_RIGHT_PAREN )
                break;
            if( CurToken == T_EOF )
                break;
            if( CurToken == T_NULL ) {
                break;
            }
        }
#if _CPU == 8086 || _CPU == 386
        CompFlags.multiple_code_segments = 1;
#endif
        MustRecog( T_RIGHT_PAREN );
    }
}

void EnableDisableMessage( int enable, unsigned msg_num )
/*******************************************************/
{
    unsigned char       mask;

    if( msg_num < MESSAGE_COUNT ) {
        if( MsgFlags == NULL ) {
            MsgFlags = CMemAlloc( ( MESSAGE_COUNT + 7 ) / 8 );
        }
        mask = 1 << ( msg_num & 7 );
        msg_num = msg_num >> 3;
        if( enable ) {
            MsgFlags[ msg_num ] &= ~mask;
        } else {
            MsgFlags[ msg_num ] |= mask;
        }
    }
}

// forms:
//    #pragma enable_message( messageNo )
//    #pragma disable_message( messageNo )
//
// dis- enable display of selected message number
//
local void PragEnableDisableMessage( int enable )
/***********************************************/
{
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        while( CurToken == T_CONSTANT ) {
            EnableDisableMessage( enable, Constant );
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
            }
        }
        MustRecog( T_RIGHT_PAREN );
    }
}


// form:
// #pragma message ("one or more " "long message " "strings")
// output these strings to stdout
// this output is _not_ dependent on setting
// of #pragma enable_message or disable_message.
static void PragMessage( void )
/*****************************/
{
    if( ExpectingToken( T_LEFT_PAREN ) ) {
        PPCTL_ENABLE_MACROS();
        while( NextToken() == T_STRING ) {
            printf( "%s", Buffer );
        }
        printf( "\n" );
        PPCTL_DISABLE_MACROS();
        MustRecog( T_RIGHT_PAREN );
    }
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
static void PushEnum( void ) {
/***************************/
    struct enums_info *ei;

    ei = CMemAlloc( sizeof( struct enums_info ) );
    ei->make_enums = CompFlags.make_enums_an_int;
    ei->next = EnumInfo;
    EnumInfo = ei;
}

static void PopEnum( void ) {
/*********************/
    struct enums_info *ei;

    ei = EnumInfo;
    if( EnumInfo != NULL ) {
        CompFlags.make_enums_an_int = ei->make_enums;
        EnumInfo =  ei->next;
        CMemFree( ei );
    }
}

static void PragEnum( void )    // #pragma enum PARSING
/**************************/
{
    if( PragRecog( "int" ) ) {
        PushEnum();
        CompFlags.make_enums_an_int = 1;
    } else if( PragRecog( "minimum" ) ) {
        PushEnum();
        CompFlags.make_enums_an_int = 0;
    } else if( PragRecog( "original" ) ) {
        PushEnum();
        CompFlags.make_enums_an_int = CompFlags.original_enum_setting;
    } else if( PragRecog( "pop" ) ) {
        PopEnum();
    }
}

static void PragIntrinsic( int intrinsic )              /* 09-oct-92 */
/****************************************/
{
    SYM_HANDLE  sym_handle;
    SYM_ENTRY   sym;

    if( ExpectingToken( T_LEFT_PAREN ) ) {
        NextToken();
        while( IS_ID_OR_KEYWORD( CurToken ) ) {
            sym_handle = SymLook( HashValue, Buffer );
            if( sym_handle != 0 ) {
                SymGet( &sym, sym_handle );
                sym.flags &= ~ SYM_INTRINSIC;
                if( intrinsic )
                    sym.flags |= SYM_INTRINSIC;
                SymReplace( &sym, sym_handle );
            }
            NextToken();
            if( CurToken != T_COMMA )
                break;
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    }
}

static void PragCodeSeg( void )                       /* 22-oct-92 */
/*****************************/
{
    textsegment     *seg;
    char            *segname;
    char            *classname;

    if( CurToken == T_LEFT_PAREN ) {
        PPCTL_ENABLE_MACROS();
        seg = NULL;
        NextToken();
        if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
            segname = CStrSave( Buffer );
            classname = CStrSave( "" );
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
                if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
                    CMemFree( classname );
                    classname = CStrSave( Buffer );
//                  CodeClassName = CStrSave( Buffer );  /* 13-apr-93 */
                    NextToken();
                }
            }
            seg = LkSegName( segname, classname );
            CMemFree( segname );
            CMemFree( classname );
        }
        PPCTL_DISABLE_MACROS();
        MustRecog( T_RIGHT_PAREN );
        DefCodeSegment = seg;
        #if _CPU == 8086 || _CPU == 386
            CompFlags.multiple_code_segments = 1;
        #endif
    }
}

static void PragDataSeg( void )                       /* 22-oct-92 */
/*****************************/
{
    char        *segname;
    int         segment;

    if( CurToken == T_LEFT_PAREN ) {
        PPCTL_ENABLE_MACROS();
        segment = 0;
        NextToken();
        if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
            segname = CStrSave( Buffer );
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
                if( ( CurToken == T_STRING ) || ( CurToken == T_ID ) ) {
                    segment = AddSegName( segname, Buffer, SEGTYPE_DATA );
                    NextToken();
                } else {
                    segment = AddSegName( segname, NULL, SEGTYPE_DATA );
                }
            } else {
                segment = AddSegName( segname, NULL, SEGTYPE_DATA );
            }
            CMemFree( segname );
        }
        PPCTL_DISABLE_MACROS();
        MustRecog( T_RIGHT_PAREN );
        DefDataSegment = segment;
    }
}

static void PragUnroll( void )
/****************************/
{
    unsigned    unroll_count;

    if( ExpectingToken( T_LEFT_PAREN ) ) {
        unroll_count = 0;
        NextToken();
        if( CurToken == T_CONSTANT ) {
            unroll_count = Constant;
            NextToken();
        }
        if( unroll_count > 255 )
            unroll_count = 255;
        UnrollCount = unroll_count;
        MustRecog( T_RIGHT_PAREN );
    }
}

// forms: (1) #pragma read_only_file
//        (2) #pragma read_only_file "file"*
//
// (1) causes current file to be marked read-only
// (2) causes indicated file to be marked read-only
//      - file must have started inclusion (may have completed)
//
static void PragReadOnlyFile( void )
/**********************************/
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
static void PragReadOnlyDir( void )
/*********************************/
{
    while( CurToken == T_STRING ) {
        SrcFileReadOnlyDir( Buffer );
        NextToken();
        if( CurToken == T_SEMI_COLON ) {
            NextToken();
        }
    }
}

// forms: (1) #pragma include_alias( "alias_name", "real_name" )
//        (2) #pragma include_alias( <alias_name>, <real_name> ) 
// 
// causes include directives referencing alias_name to be refer
// to real_name instead
//
static void PragIncludeAlias( void )
/**********************************/
{
    if( CurToken == T_LEFT_PAREN ) {
        PPCTL_ENABLE_MACROS();
        NextToken();
        if( CurToken == T_STRING ) {
            char    *alias_name;

            alias_name = CStrSave( Buffer );
            NextToken();
            MustRecog( T_COMMA );
            if( CurToken == T_STRING ) {
                SrcFileIncludeAlias( alias_name, Buffer, FALSE );
                NextToken();
            }
            CMemFree( alias_name );
        } else if( CurToken == T_LT ) {
            char    a_buf[ 82 ];    /* same size as CInclude() in cmac2.c */
            char    r_buf[ 82 ];

            a_buf[ 0 ] = '\0';
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
            MustRecog( T_COMMA );
            if( CurToken == T_LT ) {
                r_buf[ 0 ] = '\0';
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
                SrcFileIncludeAlias( a_buf, r_buf, TRUE );
            }
        }
        PPCTL_DISABLE_MACROS();
        MustRecog( T_RIGHT_PAREN );
    }
}

// form: #pragma once
//
// (1) include file once
//
static void PragOnce( void )
/**************************/
{
    SetSrcFNameOnce();
}

local void PragSTDCOption( void )
/*******************************/
{
    if( PragRecog( "ON" ) ) {
    } else if( PragRecog( "OFF" ) ) {
    } else if( PragRecog( "DEFAULT" ) ) {
    }
}

// form:
// #pragma STDC (FP_CONTRACT|FENV_ACCESS|CX_LIMITED_RANGE) (ON|OFF|DEFAULT)
//
static void PragSTDC( void )
/**************************/
{
    if( PragRecog( "FP_CONTRACT" ) ) {
        PragSTDCOption();
    } else if( PragRecog( "FENV_ACCESS" ) ) {
        PragSTDCOption();
    } else if( PragRecog( "CX_LIMITED_RANGE" ) ) {
        PragSTDCOption();
    }
}

void AddExtRefN ( char *name )
/****************************/
{
    extref_info  **extref;
    extref_info  *new_extref;

    for( extref = &ExtrefInfo; *extref != NULL; extref = &(*extref)->next )
        ; /* nothing to do */
    new_extref = CMemAlloc( sizeof( extref_info ) + strlen( name ) );
    strcpy( new_extref->name, name );
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

// #pragma extref ( symbolid [, ...] )
// #pragma extref ( "symbolname" [, ...] )
//
static void PragExtRef( void )
/****************************/
{
    if( CurToken == T_LEFT_PAREN ) {
        do {
            PPCTL_ENABLE_MACROS();
            CompFlags.pre_processing = 1;
            NextToken();
            PPCTL_DISABLE_MACROS();
            if( CurToken != T_ID && CurToken != T_STRING )
                break;
            parseExtRef();
            NextToken();
        } while( CurToken == T_COMMA );
        MustRecog( T_RIGHT_PAREN );
    } else if( CurToken == T_ID || CurToken == T_STRING ) {
        parseExtRef();
        NextToken();
    }
}

// #pragma alias(id1/"name1", id2/"name2")
//
// Causes linker to replace references to id1/name1 with references
// to id2/name2. Both the alias and the substituted symbol may be defined
// either as a string name or an id of existing symbol.
//
static void PragAlias( void )
/***************************/
{
    SYM_HANDLE      alias_sym;
    SYM_HANDLE      subst_sym;
    const char      *alias_name;
    const char      *subst_name;
    alias_list      **alias;
    alias_list      *new_alias;

    if( CurToken != T_LEFT_PAREN ) 
        return;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    alias_name = subst_name = NULL;
    alias_sym  = subst_sym  = NULL;

    if( CurToken == T_ID ) {
        alias_sym = SymLook( HashValue, Buffer );
        if( alias_sym == 0 ) {
            CErr2p( ERR_UNDECLARED_SYM, Buffer );
        }
    } else if( CurToken == T_STRING ) {
        alias_name = CStrSave( Buffer );
    } else {
        return;     /* error */
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
    } else {
        return;     /* error */
    }
    PPNextToken();
    PPCTL_DISABLE_MACROS();
    MustRecog( T_RIGHT_PAREN );

    /* Add a new alias record - if it's valid - to the list */
    if( ( alias_name != NULL || alias_sym != NULL ) && ( subst_name != NULL || subst_sym != NULL ) ) {
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
    int     check_end = TRUE;

    /* Note that the include_alias pragma must always be processed
     * because it's intended for the preprocessor, not the compiler.
     */
    CompFlags.in_pragma = 1;
    NextToken();
    if( PragRecog( "include_alias" ) ) {
        PragIncludeAlias();
    } else if( CompFlags.cpp_output ) {
        PPCTL_ENABLE_MACROS();
        CppPrtf( "#pragma " );
        for( ; CurToken != T_NULL; ) {
            CppPrtToken();
            GetNextToken();
        }
        PPCTL_DISABLE_MACROS();
    } else if( IS_ID_OR_KEYWORD( CurToken ) ) {
        if( PragIdRecog( "on" ) ) {
            PragFlag( 1 );
        } else if( PragIdRecog( "off" ) ) {
            PragFlag( 0 );
        } else if( PragIdRecog( "aux" ) || PragIdRecog( "linkage" ) ) {
            PragAux();
        } else if( PragIdRecog( "library" ) ) {
            PragLibs();
        } else if( PragIdRecog( "comment" ) ) {
            PragComment();
        } else if( PragIdRecog( "pack" ) ) {
            PragPack();
        } else if( PragIdRecog( "alloc_text" ) ) {
            PragAllocText();
        } else if( PragIdRecog( "code_seg" ) ) {
            PragCodeSeg();
        } else if( PragIdRecog( "data_seg" ) ) {
            PragDataSeg();
        } else if( PragIdRecog( "disable_message" ) ) {
            PragEnableDisableMessage( 0 );
        } else if( PragIdRecog( "enable_message" ) ) {
            PragEnableDisableMessage( 1 );
        } else if( PragIdRecog( "include_alias" ) ) {
            PragIncludeAlias();
        } else if( PragIdRecog( "message" ) ) {
            PragMessage();
        } else if( PragIdRecog( "intrinsic" ) ) {
            PragIntrinsic( 1 );
        } else if( PragIdRecog( "function" ) ) {
            PragIntrinsic( 0 );
        } else if( PragIdRecog( "enum" ) ) {
            PragEnum();
        } else if( startPragRecog( "read_only_file" ) ) {
            PragReadOnlyFile();
        } else if( startPragRecog( "read_only_directory" ) ) {
            PragReadOnlyDir();
        } else if( PragIdRecog( "once" ) ) {
            PragOnce();
        } else if( PragIdRecog( "unroll" ) ) {
            PragUnroll();
        } else if( PragIdRecog( "STDC" ) ) {
            PragSTDC();
        } else if( PragIdRecog( "extref" ) ) {
            PragExtRef();
        } else if( PragIdRecog( "alias" ) ) {
            PragAlias();
        } else {
            check_end = FALSE;
        }
    } else {
        check_end = FALSE;
    }
    if( check_end )
        EndOfPragma();
    CompFlags.in_pragma = 0;
}

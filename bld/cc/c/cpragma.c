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
#include "cgswitch.h"
#include "pragdefn.h"
#include "pdefn2.h"

struct  pack_info {
    struct pack_info *next;
    int         pack_amount;
} *PackInfo;

struct enums_info {
    struct enums_info *next;
    int    make_enums;
} *EnumInfo;

void CPragmaInit( void ){
//********************************//
// Init any general pragma things //
//********************************//
    TextSegList = NULL;
    PragmaInit(); // call traget specific init
}

void CPragma()
{
    if( CompFlags.cpp_output ) {                    /* 29-sep-90 */
        if( ! CppPrinting() ) return;               /* 12-dec-89 */
        CppPrtf( "#pragma" );
        CompFlags.pre_processing = 1;               /* 28-feb-89 */
        CompFlags.in_pragma = 1;
        for(;;) {
            GetNextToken();
            if( CurToken == T_NULL ) break;
            PrtToken();
        }
        CompFlags.in_pragma = 0;
    } else {
        NextToken();
        if( PragRecog( "on" ) ) {
            PragFlag( 1 );
        } else if( PragRecog( "off" ) ) {
            PragFlag( 0 );
        } else if( PragRecog( "aux" ) || PragRecog( "linkage" ) ) {
            PragAux();
        } else if( PragRecog( "library" ) ) {
            PragLibs();
        } else if( PragRecog( "comment" ) ) {
            PragComment();
        } else if( PragRecog( "pack" ) ) {
            PragPack();
        } else if( PragRecog( "alloc_text" ) ) {    /* 26-oct-91 */
            PragAllocText();
        } else if( PragRecog( "code_seg" ) ) {      /* 22-oct-92 */
            PragCodeSeg();
        } else if( PragRecog( "data_seg" ) ) {      /* 22-oct-92 */
            PragDataSeg();
        } else if( PragRecog( "disable_message" ) ) {/* 18-jun-92 */
            PragEnableDisableMessage( 0 );
        } else if( PragRecog( "enable_message" ) ) {/* 18-jun-92 */
            PragEnableDisableMessage( 1 );
        } else if( PragRecog( "message" ) ) {   /* 13-oct-94 */
            PragMessage();
        } else if( PragRecog( "intrinsic" ) ) { /* 09-oct-92 */
            PragIntrinsic( 1 );
        } else if( PragRecog( "function" ) ) {
            PragIntrinsic( 0 );
        } else if( PragRecog( "enum" ) ) {
            PragEnum();
        } else if( PragRecog( "read_only_file" ) ) {
            PragReadOnlyFile();
        } else if( PragRecog( "read_only_directory" ) ) {
            PragReadOnlyDir();
        } else if( PragRecog( "once" ) ) {
            PragOnce();
        } else if( PragRecog( "unroll" ) ) {
            PragUnroll();
        } else {
            return;                     /* don't recognize anything */
        }
        EndOfPragma();
    }
}


local void EndOfPragma()
{
    if( CurToken == T_SEMI_COLON ) NextToken();
    if( CurToken != T_NULL ) ExpectEndOfLine();
    while( CurToken != T_NULL && CurToken != T_EOF ) NextToken();
}



void PragInit()
{
    CdeclInfo   = DefaultInfo;
    PascalInfo  = DefaultInfo;
    FortranInfo = DefaultInfo;
    SyscallInfo = DefaultInfo;
    StdcallInfo = DefaultInfo;
    OptlinkInfo = DefaultInfo;
    DefaultInfo.use = 2;        /* so they don't get freed */
    CdeclInfo.use   = 2;
    PascalInfo.use  = 2;
    SyscallInfo.use = 2;
    OptlinkInfo.use = 2;
    FortranInfo.use = 2;
    PackInfo = NULL;
    EnumInfo = NULL;
}

static struct toggle ToggleNames[] = {
    #define TOGDEF( a, b ) {  #a, b },
    #include "togdef.h"
    #undef TOGDEF
        { NULL, 0 }
    };

extern int SetToggleFlag( char const *name, int const value ){
/************************************************************/
    int     i;
    char   *pnt;
    int     ret;

    ret = FALSE;
    for( i=0; ( pnt = ToggleNames[ i ].name ) != NULL; ++i ) {
        if( strcmp( pnt, name ) == 0 ) {
            if( value == 0 ){
                Toggles &= ~ToggleNames[ i ].flag;
            }else{
                Toggles |= ToggleNames[ i ].flag;
            }
            ret = TRUE;
            break;
        }
    }
    return( ret );
}

local void PragFlag( int value )
{

    if( CurToken != T_LEFT_PAREN ) return;
    NextToken();
    while( CurToken == T_ID ) {
        SetToggleFlag( Buffer, value );
        NextToken();
    }
    MustRecog( T_RIGHT_PAREN );
}

local void GetLibraryNames()
{
    struct library_list **owner;
    struct library_list *new;

    for( owner = &HeadLibs; *owner != NULL; owner = &(*owner)->next )
            ; /* nothing to do */
    while( CurToken == T_ID  ||  CurToken == T_STRING ) {
        new = (void *)CMemAlloc( sizeof( struct library_list ) + TokenLen );
        new->next = NULL;
        new->prio = USER_LIB_PRIO;
        strcpy( new->name, Buffer );
        *owner = new;
        owner  = &new->next;
        NextToken();
    }
    MustRecog( T_RIGHT_PAREN );
}

void PragLibs()
{
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        GetLibraryNames();
    }
}

local void PragComment()
{
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( PragRecog( "lib" ) ) {
            MustRecog( T_COMMA );
            GetLibraryNames();
        }
    }
}

local void SetPackAmount()
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

void PragPack()
{
    struct pack_info    *pi;

    if( CurToken != T_LEFT_PAREN ) return;
    CompFlags.pre_processing = 1;           /* enable macros */
    NextToken();
    CompFlags.pre_processing = 2;
    if( CurToken == T_RIGHT_PAREN ) {
        NextToken();
        if( CurToken == T_SEMI_COLON ) NextToken();
        PackAmount = GblPackAmount;
        return;
    }
    /* check to make sure it is a numeric token */
    if( CurToken == T_CONSTANT ) {                  /* 24-may-89 */
        SetPackAmount();
        NextToken();
    } else if( PragRecog( "push" ) ) {                  /* 29-sep-94 */
        pi = (struct pack_info *)CMemAlloc( sizeof(struct pack_info) );
        pi->next = PackInfo;
        pi->pack_amount = PackAmount;
        PackInfo = pi;
        if( CurToken == T_COMMA ) {
            NextToken();
            if( CurToken == T_CONSTANT ) {
                SetPackAmount();
            } else {
                CErr1( ERR_NOT_A_CONSTANT_EXPR );
            }
            NextToken();
        }
    } else if( PragRecog( "pop" ) ) {
        pi = PackInfo;
        if( pi != NULL ) {
            PackAmount = pi->pack_amount;
            PackInfo = pi->next;
            CMemFree( pi );
        }
    } else {
        CErr1( ERR_NOT_A_CONSTANT_EXPR );
        NextToken();
    }
    MustRecog( T_RIGHT_PAREN );
}

struct magic_words {
        char *  name;
        int     index;
};
enum {
        M_UNKNOWN,
        M_DEFAULT,
        M_CDECL,
        M_PASCAL,
        M_FORTRAN,
        M_SYSTEM,
        M_STDCALL,
};

struct magic_words MagicWords[] = {                     /* 18-aug-90 */
        { "default",    M_DEFAULT },
        { "cdecl",      M_CDECL },
        { "pascal",     M_PASCAL },
        { "fortran",    M_FORTRAN },
        { "system",     M_SYSTEM },
        { "syscall",    M_SYSTEM },
        { "stdcall",    M_STDCALL },
        { "__cdecl",    M_CDECL },
        { "__pascal",   M_PASCAL },
        { "__fortran",  M_FORTRAN },
        { "__system",   M_SYSTEM },
        { "__syscall",  M_SYSTEM },
        { "__stdcall",  M_STDCALL },
        { NULL,         M_UNKNOWN }
};

local int MagicKeyword()
{
    int         i;

    for( i = 0; MagicWords[i].name; ++i ) {
        if( strcmp( Buffer, MagicWords[i].name ) == 0 ) break;
    }
    return( MagicWords[i].index );
}


void CreateAux( char *id )
{
    CurrEntry = (struct aux_entry *)
            CMemAlloc( sizeof(struct aux_entry) + strlen( id ) );
    strcpy( CurrEntry->name, id );
#if _CPU == 370
    CurrEntry->offset = -1;
#endif
    CurrInfo = (struct aux_info *)CMemAlloc( sizeof( struct aux_info ) );
}


void SetCurrInfo()
{
    switch( MagicKeyword() ) {
    case M_DEFAULT:
        CurrInfo = &DefaultInfo;
        break;
    case M_CDECL:
//      CompFlags.cdecl_defined = 1;
        CurrInfo = &CdeclInfo;
        break;
    case M_PASCAL:
//      CompFlags.pascal_defined = 1;
        CurrInfo = &PascalInfo;
        break;
    case M_FORTRAN:
//      CompFlags.fortran_defined = 1;
        CurrInfo = &FortranInfo;
        break;
    case M_SYSTEM:
        CurrInfo = &SyscallInfo;
        break;
    case M_STDCALL:
        CurrInfo = &StdcallInfo;
        break;
    default:
        CreateAux( Buffer );
    }
}


void PragCurrAlias()
{
    struct aux_entry *search;

    search = NULL;
    CurrAlias = &DefaultInfo;
    switch( MagicKeyword() ) {
    case M_CDECL:
        CurrAlias = &CdeclInfo;
        break;
    case M_PASCAL:
        CurrAlias = &PascalInfo;
        break;
    case M_FORTRAN:
        CurrAlias = &FortranInfo;
        break;
    case M_SYSTEM:
        CurrAlias = &SyscallInfo;
        break;
    case M_STDCALL:
        CurrAlias = &StdcallInfo;
        break;
    default:
        search = AuxLookup( Buffer );
        if( search != NULL ) CurrAlias = search->info;
    }
}


void XferPragInfo( char *from, char *to )
{
    struct aux_entry *ent;

    if( AuxLookup( to ) != NULL ) return;
    ent = AuxLookup( from );
    if( ent == NULL ) return;
    CreateAux( to );
    CurrEntry->info = ent->info;
    ent->info->use++;
    CurrEntry->next = AuxList;
    AuxList = CurrEntry;
}


void PragEnding()
{
    if( CurrEntry == NULL ) return;
    CurrInfo->use = CurrAlias->use; /* for compare */
    if( memcmp( CurrAlias, CurrInfo,
                sizeof( struct aux_info ) ) == 0 ) {
        CurrEntry->info = CurrAlias;
        CurrAlias->use++;
        CMemFree( CurrInfo );
    } else {
        CopyParms();
        CopyLinkage();
        CopyCode();
        CopyObjName();
        CurrInfo->use = 1;
        CurrEntry->info = CurrInfo;
    }
    CurrEntry->next = AuxList;
    AuxList = CurrEntry;
}


local void CopyLinkage()
{
#if _CPU == 370
    linkage_regs *regs;

    if( CurrInfo->linkage == &DefaultLinkage ) return;
    if( CurrInfo->linkage == &OSLinkage ) return;
    if( CurrInfo->linkage != CurrAlias->linkage ) return;
    regs = (linkage_regs *)CMemAlloc( sizeof( linkage_regs ) );
    memcpy( regs, CurrInfo->linkage, sizeof( linkage_regs ) );
    CurrInfo->linkage = regs;
#endif
}


local void CopyParms()
{
    int         i;
    hw_reg_set  *regs;

    if( CurrInfo->parms != CurrAlias->parms ) return;
    for( i = 1, regs = CurrInfo->parms;
         !HW_CEqual( *regs, HW_EMPTY ); ++i, ++regs )
        ;
    i *= sizeof( hw_reg_set );
    regs = (hw_reg_set *)CMemAlloc( i );
    memcpy( regs, CurrInfo->parms, i );
    CurrInfo->parms = regs;
}

#if _MACHINE == _ALPHA || _MACHINE == _PPC
local void CopyCode()
{
    risc_byte_seq    *code;
    int              size;
//TODO deal with reloc list
    if( CurrInfo->code == NULL ) return;
    if( CurrInfo->code != CurrAlias->code ) return;
    size = sizeof( risc_byte_seq ) + CurrInfo->code->length;
    code = (risc_byte_seq *)CMemAlloc( size );
    memcpy( code, CurrInfo->code, size );
    CurrInfo->code = code;
}
#else
local void CopyCode()
{
    byte_seq    *code;
    int         size;

    if( CurrInfo->code == NULL ) return;
    if( CurrInfo->code != CurrAlias->code ) return;
    size = sizeof( byte_seq ) + CurrInfo->code->length;
    code = (byte_seq *)CMemAlloc( size );
    memcpy( code, CurrInfo->code, size );
    CurrInfo->code = code;
}
#endif

local void CopyObjName()
{
    char        *name;

    if( CurrInfo->objname == NULL ) return;
    if( CurrInfo->objname != CurrAlias->objname ) return;
    name = CMemAlloc( strlen( CurrInfo->objname ) + 1 );
    strcpy( name, CurrInfo->objname );
    CurrInfo->objname = name;
}


int PragRecog( char *what )
{
    char        *p;

    if( !(CurToken == T_ID
      || (CurToken >= FIRST_KEYWORD && CurToken < T_MACRO_PARM)) ) {
        return( 0 );
    }
    p = Buffer;
    if( *p == '_' ) ++p;
    if( *p == '_' ) ++p;
    if( stricmp( what, p ) == 0 ) {
        NextToken();
        return( 1 );
    }
    return( 0 );
}


void PragObjNameInfo()
{
    if( CurToken == T_STRING ) {
        CurrInfo->objname = CMemAlloc( strlen( Buffer ) + 1 );
        strcpy( CurrInfo->objname, Buffer );
        NextToken();
    }
}

int PragSet()
{
    if( CurToken == T_LEFT_BRACKET ) return( T_RIGHT_BRACKET );
    if( CurToken == T_LEFT_BRACE ) return( T_RIGHT_BRACE );
    return( T_NULL );
}


hw_reg_set PragRegList()
{
    hw_reg_set  res, reg;
    int         close;

    HW_CAsgn( res, HW_EMPTY );
    HW_CAsgn( reg, HW_EMPTY );
    close = PragSet();
    if( close != T_NULL ) {
        CompFlags.pre_processing = 1;       /* enable macros */
        NextToken();
        for(;;) {
            reg = PragRegName( Buffer );
            if( HW_CEqual( reg, HW_EMPTY ) ) break;
            HW_TurnOn( res, reg );
            NextToken();
        }
        CompFlags.pre_processing = 2;
        MustRecog( close );
    }
    return( res );
}

void PragManyRegSets()
{
    int         i;
    hw_reg_set  list, *sets;
    hw_reg_set  buff[ MAXIMUM_PARMSETS ];

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
    CurrInfo->parms = sets;
}

struct textsegment *NewTextSeg( char *name, char *suffix, char *classname )
{
    struct textsegment  *seg;
    int         len1;
    int         len2;
    int         len3;

    len1 = strlen( name );
    len2 = strlen( suffix );
    len3 = strlen( classname ) + 1;
    seg = CMemAlloc( len1 + len2 + len3 + sizeof( struct textsegment ) );
    strcpy( seg->segname, name );
    strcpy( &seg->segname[len1], suffix );
    strcpy( &seg->segname[len1+len2+1], classname );
    seg->next = TextSegList;
    TextSegList = seg;
    return( seg );
}

struct textsegment *LkSegName( char *segname, char *classname )
{
    struct textsegment  *seg;
    int                 len;

    len = strlen( segname ) + 1;
    for( seg = TextSegList; seg; seg = seg->next ) {
        if( strcmp( seg->segname, segname ) == 0 ) {
            if( strcmp( &seg->segname[len], classname ) == 0 ) {
                return( seg );
            }
        }
    }
    return( NewTextSeg( segname, "", classname ) );
}

local void PragAllocText()                              /* 26-oct-91 */
{
    struct textsegment  *seg;
    SYM_HANDLE          sym_handle;
    auto SYM_ENTRY      sym;

    MustRecog( T_LEFT_PAREN );
    /* current token can be an T_ID or a T_STRING */
    seg = LkSegName( Buffer, "" );
    NextToken();
    for(;;) {
        MustRecog( T_COMMA );
        /* current token can be an T_ID or a T_STRING */
        sym_handle = Sym0Look( CalcHash( Buffer, strlen(Buffer) ), Buffer );
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
        if( CurToken == T_RIGHT_PAREN )  break;
        if( CurToken == T_EOF )  break;
        if( CurToken == T_NULL ) break;
    }
    MustRecog( T_RIGHT_PAREN );
    #if _CPU == 8086 || _CPU == 386
        CompFlags.multiple_code_segments = 1;
    #endif
}

void EnableDisableMessage( int enable, unsigned msg_num )
{
    unsigned char       mask;

    if( msg_num <= HIGHEST_MESSAGE_NUMBER ) {
        if( MsgFlags == NULL ) {
            MsgFlags = CMemAlloc( (HIGHEST_MESSAGE_NUMBER + 7) / 8 );
        }
        mask = 1 << (msg_num & 7);
        msg_num = msg_num >> 3;
        if( enable ) {
            MsgFlags[ msg_num ]  &=  ~mask;
        } else {
            MsgFlags[ msg_num ]  |=  mask;
        }
    }
}

local void PragEnableDisableMessage( int enable )
{
    if( CurToken != T_LEFT_PAREN ) return;
    NextToken();
    for(;;) {
        if( CurToken != T_CONSTANT )  break;
        EnableDisableMessage( enable, Constant );
        NextToken();
        if( CurToken == T_COMMA )  NextToken();
    }
    MustRecog( T_RIGHT_PAREN );
}

static void PragMessage()
{
    if( CurToken != T_LEFT_PAREN ) return;
    CompFlags.pre_processing = 1;           /* enable macros */
    for(;;) {
        NextToken();
        if( CurToken != T_STRING ) break;
        printf( "%s", Buffer );
    }
    printf( "\n" );
    CompFlags.pre_processing = 2;
    MustRecog( T_RIGHT_PAREN );
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
static void PushEnum( void ){
/***************************/
    struct enums_info *ei;

    ei = CMemAlloc( sizeof(struct enum_info) );
    ei->make_enums = CompFlags.make_enums_an_int;
    ei->next = EnumInfo;
    EnumInfo = ei;
}

static void PopEnum( void ){
/*********************/
    struct enums_info *ei;

    ei = EnumInfo;
    if( EnumInfo != NULL ){
        CompFlags.make_enums_an_int = ei->make_enums;
        EnumInfo =  ei->next;
        CMemFree( ei );
    }
}

static void PragEnum            // #pragma enum PARSING
    ( void )
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
{
    SYM_HANDLE  sym_handle;
    SYM_ENTRY   sym;

    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        while( CurToken == T_ID ) {
            sym_handle = SymLook( HashValue, Buffer );
            if( sym_handle != 0 ) {
                SymGet( &sym, sym_handle );
                sym.flags &= ~ SYM_INTRINSIC;
                if( intrinsic )  sym.flags |= SYM_INTRINSIC;
                SymReplace( &sym, sym_handle );
            }
            NextToken();
            if( CurToken != T_COMMA )  break;
            NextToken();
        }
        MustRecog( T_RIGHT_PAREN );
    }
}

static void PragCodeSeg()                       /* 22-oct-92 */
{
    struct textsegment  *seg;
    char                *segname;
    char                *classname;

    if( CurToken == T_LEFT_PAREN ) {
        CompFlags.pre_processing = 1;           /* enable macros */
        seg = NULL;
        NextToken();
        if( CurToken == T_STRING ) {
            segname = CStrSave( Buffer );
            classname = CStrSave( "" );
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
                if( CurToken == T_STRING ) {
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
        CompFlags.pre_processing = 2;
        MustRecog( T_RIGHT_PAREN );
        DefCodeSegment = seg;
        #if _CPU == 8086 || _CPU == 386
            CompFlags.multiple_code_segments = 1;
        #endif
    }
}

static void PragDataSeg()                       /* 22-oct-92 */
{
    char        *segname;
    int         segment;

    if( CurToken == T_LEFT_PAREN ) {
        CompFlags.pre_processing = 1;           /* enable macros */
        segment = 0;
        NextToken();
        if( CurToken == T_STRING ) {
            segname = CStrSave( Buffer );
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
                if( CurToken == T_STRING ) {
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
        CompFlags.pre_processing = 2;
        MustRecog( T_RIGHT_PAREN );
        DefDataSegment = segment;
    }
}

static void PragUnroll()
{
    unsigned    unroll_count;

    if( CurToken != T_LEFT_PAREN ) return;
    NextToken();
    if( CurToken == T_RIGHT_PAREN ) {
        unroll_count = 0;
    } else {
        if( CurToken != T_CONSTANT ) return;
        unroll_count = Constant;
        NextToken();
        if( CurToken != T_RIGHT_PAREN ) return;
    }
    NextToken();
    if( unroll_count > 255 ) unroll_count = 255;
    UnrollCount = unroll_count;
#ifndef NEWCFE
    if( QuadIndex != 0 ) {              // if we have started some quads
        GenQuad( unroll_count, T_UNROLL, 0, 0 );
    }
#endif
}
// forms: (1) #pragma read_only_file
//        (2) #pragma read_only_file "file"*
//
// (1) causes current file to be marked read-only
// (2) causes indicated file to be marked read-only
//      - file must have started inclusion (may have completed)
//
static void PragReadOnlyFile( void )
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
// (1) include file once
//
static void PragOnce( void )
{
    SetSrcFNameOnce();
}

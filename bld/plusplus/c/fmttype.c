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
#include "stringl.h"
#include "stack.h"
#include "memmgr.h"
#include "cgfront.h"
#include "carve.h"
#include "vbuf.h"
#include "dbg.h"
#include "fmtsym.h"
#include "fmttype.h"
#include "name.h"
#include "initdefs.h"
#include "ringfns.h"
#include "template.h"

typedef enum {
    LEFT, RIGHT
} FMT_LR;

typedef struct fmt_info FMT_INFO;
struct fmt_info {
    FMT_INFO    *stack;
    TYPE        type;
    unsigned    main_function : 1;
};

typedef struct {
    char        *name;
    type_flag   mask;
} FMT_FLAG_INFO;

#define ENTRY_ERROR             "<error> ",
#define ENTRY_BOOL              "bool ",
#define ENTRY_CHAR              "char ",
#define ENTRY_SCHAR             "signed char ",
#define ENTRY_UCHAR             "unsigned char ",
#define ENTRY_WCHAR             "wchar_t ",
#define ENTRY_SSHORT            "short ",
#define ENTRY_USHORT            "unsigned short ",
#define ENTRY_SINT              "int ",
#define ENTRY_UINT              "unsigned ",
#define ENTRY_SLONG             "long ",
#define ENTRY_ULONG             "unsigned long ",
#define ENTRY_SLONG64           "__int64 ",
#define ENTRY_ULONG64           "unsigned __int64 ",
#define ENTRY_FLOAT             "float ",
#define ENTRY_DOUBLE            "double ",
#define ENTRY_LONG_DOUBLE       "long double ",
#define ENTRY_ENUM              "<enum> ",
#define ENTRY_POINTER           "* ",
#define ENTRY_TYPEDEF           "<typedef> ",
#define ENTRY_CLASS             "<class> ",
#define ENTRY_BITFIELD          "<bitfield> ",
#define ENTRY_FUNCTION          "<function> ",
#define ENTRY_ARRAY             "<array> ",
#define ENTRY_DOT_DOT_DOT       "... ",
#define ENTRY_VOID              "void ",
#define ENTRY_MODIFIER          "<modifier> ",
#define ENTRY_MEMBER_POINTER    "::* ",
#define ENTRY_GENERIC           "? ",

static char *typeName[] = {
    #include "type_arr.h"
};

static char *errFormats[] = {
    #define TFMT_DEF(a,b) b,
    SPECIAL_TYPE_FMTS
    #undef TFMT_DEF
};

static FMT_FLAG_INFO functionFlags[] = {
    { "__loadds ",      TF1_LOADDS },
    { "__saveregs ",    TF1_SAVEREGS },
    { "__interrupt ",   TF1_INTERRUPT },
    { NULL,             TF1_NULL }
};

static FMT_FLAG_INFO modifierFlags[] = {
    { "mutable ",   TF1_MUTABLE },
    { "const ",     TF1_CONST },
    { "volatile ",  TF1_VOLATILE },
    { "__near ",    TF1_NEAR },
    { "__far ",     TF1_FAR },
    { "__far16 ",   TF1_FAR16 },
    { "__huge ",    TF1_HUGE },
    { "__export ",  TF1_DLLEXPORT },
    { "__declspec(dllimport) ",  TF1_DLLIMPORT },
    { "__unaligned ",  TF1_UNALIGNED },
    { NULL,         TF1_NULL }
};

static FMT_FLAG_INFO ushortFlags[] = {
    { "__segment ", TF1_SEGMENT },
    { NULL,         TF1_NULL }
};

static FMT_FLAG_INFO pointerFlags[] = {
    { "& ", TF1_REFERENCE },
    { NULL, TF1_NULL }
};

static FMT_FLAG_INFO classFlags[] = {
    { "<union> ",   TF1_UNION },
    { "<struct> ",  TF1_STRUCT },
    { NULL,         TF1_NULL }
};

static char basedVoid[]         = "void ";
static char basedSelf[]         = "__self ";
static char openBased[]         = "__based( ";
static char closeBased[]        = ") ";
static char openSegname[]       = "__segname( \"";
static char closeSegname[]      = "\" )";
static char openPragma[]        = "__pragma(\"";
static char closePragma[]       = "\") ";
static char openFunction[]      = "( ";
static char closeFunction[]     = ") ";
static char openSquareParen[]   = "[";
static char closeSquareParen[]  = "]";
static char openParen[]         = "(";
static char closeParen[]        = ")";
static char bitfieldSep[]       = " : ";
static char parameterSep[]      = ", ";
static char whiteSpace[]        = " ";
static char nullType[]          = "<null type pointer>";
static char memberPointer[]     = "<member pointer>";

#define BLOCK_FMT       16
static carve_t carveFMT;

FMT_CONTROL FormatTypeDefault = FF_USE_VOID;

static void formatTypeInit( INITFINI* defn )
{
    defn = defn;
    carveFMT = CarveCreate( sizeof( FMT_INFO ), BLOCK_FMT );
}

static void formatTypeFini( INITFINI* defn )
{
    defn = defn;
    DbgStmt( CarveVerifyAllGone( carveFMT, "FMT" ) );
    CarveDestroy( carveFMT );
}

INITDEFN( type_formatting, formatTypeInit, formatTypeFini )

static void fmtTypeArray( TYPE type, VBUF *pvbuf )
/************************************************/
{
    VBUF    working;

    VbufInit( &working );
    VbufConcStr( &working, openSquareParen );
    if( type->u.a.array_size > 0 ) {
        VbufConcDecimal( &working, type->u.a.array_size );
    }
    VbufConcStr( &working, closeSquareParen );
    VbufPrepVbuf( pvbuf, &working );
    VbufFree( &working );
}

static boolean willPrintBased( type_flag flags )
/**********************************************/
{
    return( ( flags & TF1_BASED ) != TF1_NULL );
}

static void fmtTypeBased( TYPE type, VBUF *pvbuf )
/************************************************/
{
    type_flag   flags;
    VBUF        working;

    if( !willPrintBased( type->flag ) ) {
        return;
    }
    flags = type->flag & TF1_BASED;
    VbufConcStr( pvbuf, openBased );
    switch( flags ) {
    case TF1_BASED_STRING:
        VbufConcStr( pvbuf, openSegname );
        VbufConcStr( pvbuf, ((STRING_CONSTANT)(type->u.m.base))->string );
        VbufConcStr( pvbuf, closeSegname );
        break;
    case TF1_BASED_VOID:
        VbufConcStr( pvbuf, basedVoid );
        break;
    case TF1_BASED_SELF:
        VbufConcStr( pvbuf, basedSelf );
        break;
    case TF1_BASED_FETCH:
        FormatSym( (SYMBOL)type->u.m.base, &working );
        VbufConcVbuf( pvbuf, &working );
        VbufConcStr( pvbuf, whiteSpace );
        VbufFree( &working );
        break;
    case TF1_BASED_ADD:
        FormatSym( (SYMBOL)type->u.m.base, &working );
        VbufConcVbuf( pvbuf, &working );
        VbufConcStr( pvbuf, whiteSpace );
        VbufFree( &working );
        break;
    }
    VbufConcStr( pvbuf, closeBased );
}

static void fmtNicePragma( AUX_INFO *pragma, VBUF *pvbuf )
/********************************************************/
{
    char *id;

    if( pragma == NULL ) {
        return;
    }
    if( PragmaName( pragma, &id ) ) {
        if( id != NULL ) {
            VbufConcStr( pvbuf, id );
            VbufConcStr( pvbuf, whiteSpace );
        }
    } else {
        if( id != NULL ) {
            VbufConcStr( pvbuf, openPragma );
            VbufConcStr( pvbuf, id );
            VbufConcStr( pvbuf, closePragma );
        }
    }
}

static void fmtTypePragma( TYPE type, VBUF *pvbuf )
/*************************************************/
{
    AUX_INFO *pragma;

    if( type->id == TYP_MODIFIER ) {
        pragma = type->u.m.pragma;
    } else {
        pragma = type->u.f.pragma;
    }
    fmtNicePragma( pragma, pvbuf );
}

static void fmtTypeBitfield( TYPE type, VBUF *pvbuf )
/***************************************************/
{
    VBUF    working;

    VbufInit( &working );
    VbufConcStr( &working, bitfieldSep );
    VbufConcDecimal( &working, type->u.b.field_width );
    VbufConcStr( &working, whiteSpace );
    VbufPrepVbuf( pvbuf, &working );
    VbufFree( &working );
}

static NAME getArgName( unsigned index )
{
    NAME name;
    SYMBOL sym;

    name = NULL;
    sym = ScopeFuncParm( index );
    if( sym != NULL ) {
        name = sym->name->name;
        if( IsNameDummy( name ) ) {
            name = NULL;
        }
    }
    return( name );
}

static void fmtTypeArgument( TYPE arg_type, unsigned arg_index, VBUF *pvbuf, FMT_CONTROL control )
{
    FMT_CONTROL arg_control;
    VBUF arg_prefix;
    VBUF arg_suffix;
    NAME name;

    arg_control = FormatTypeDefault | control;
    arg_control &= ~FF_ARG_NAMES;
    FormatFunctionType( arg_type, &arg_prefix, &arg_suffix, 0, arg_control );
    VbufConcVbuf( pvbuf, &arg_prefix );
    if( control & FF_ARG_NAMES ) {
        name = getArgName( arg_index );
        if( name != NULL ) {
            VbufConcStr( pvbuf, NameStr( name ) );
        }
    }
    VbufConcVbuf( pvbuf, &arg_suffix );
    VbufFree( &arg_prefix );
    VbufFree( &arg_suffix );
}

static void fmtTypeFlag( type_flag flag, VBUF *pvbuf,
/***************************************************/
    FMT_FLAG_INFO *flag_info )
{
    unsigned    i;
    type_flag   mask;

    for( i = 0 ; flag_info[i].name != NULL ; i++ ) {
        mask = flag_info[i].mask;
        if( (flag & mask) == mask ) {
            VbufConcStr( pvbuf, flag_info[i].name );
        }
    }
}

static void fmtTypeFunction( arg_list *alist, VBUF *pvbuf, int num_def,
/*********************************************************************/
    FMT_CONTROL control )
{
    FMT_CONTROL arg_control;
    unsigned    i;
    unsigned    num_args;
    VBUF        working;

    VbufInit( &working );
    VbufConcStr( &working, openFunction );
    num_args = alist->num_args;
    num_def = num_args - num_def;
    if( num_args == 0 ) {
        if( control & FF_USE_VOID ) {
            VbufConcStr( &working, typeName[TYP_VOID] );
        } else {
            VbufTruncWhite( &working );
        }
    } else {
        // only keep whether we want typedef names
        arg_control = control & ( FF_TYPEDEF_STOP | FF_ARG_NAMES );
        for( i = 0 ; i < num_def ; i++ ) {
            if( i > 0 ) {
                VbufTruncWhite( &working );
                VbufConcStr( &working, parameterSep );
            }
            fmtTypeArgument( alist->type_list[i], i, &working, arg_control );
        }
        if( num_args > num_def ) {
            VbufConcStr( &working, openSquareParen );
            for( ; i < num_args ; i++ ) {
                if( i > 0 ) {
                    VbufTruncWhite( &working );
                    VbufConcStr( &working, parameterSep );
                }
                fmtTypeArgument( alist->type_list[i], i, &working, arg_control );
            }
            VbufConcStr( &working, closeSquareParen );
        }
    }
    VbufConcStr( &working, closeFunction );
    fmtTypeFlag( alist->qualifier, &working, modifierFlags );
    VbufPrepVbuf( pvbuf, &working );
    VbufFree( &working );
}

static void fmtModifierTypeFlag( TYPE type, type_flag flag, VBUF *pvbuf )
/***********************************************************************/
{
    unsigned    i;
    type_flag   mask;

    for( i = 0 ; modifierFlags[i].name != NULL ; i++ ) {
        mask = modifierFlags[i].mask;
        if( (flag & mask) == mask ) {
            if( ( ( mask & TF1_MEM_MODEL ) == 0 ) ||
                  ( ( DefaultMemoryFlag( type ) & mask ) != mask ) ) {
                VbufConcStr( pvbuf, modifierFlags[i].name );
            }
        }
    }
}

static boolean willPrintModifier( TYPE type, type_flag flag )
/***********************************************************/
{
    unsigned    i;
    type_flag   mask;

    for( i = 0 ; modifierFlags[i].name != NULL ; i++ ) {
        mask = modifierFlags[i].mask;
        if( (flag & mask) == mask ) {
            if( ( ( mask & TF1_MEM_MODEL ) == 0 ) ||
                  ( ( DefaultMemoryFlag( type ) & mask ) != mask ) ) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static void fmtTypeChangeState( FMT_LR *curr, FMT_LR new,
/*******************************************************/
    VBUF *pprefix, VBUF *psuffix )
{
    if( new == RIGHT && *curr == LEFT ) {
        VbufConcStr( pprefix, openParen );
        VbufPrepStr( psuffix, closeParen );
    }
    *curr = new;
}

static void fmtTypeScope( SCOPE scope, VBUF *pprefix )
/****************************************************/
{
    VBUF name_scope;

    FormatScope( scope, &name_scope, FALSE );
    VbufConcVbuf( pprefix, &name_scope );
    VbufFree( &name_scope );
}

static void fmtTemplateParms( TYPE class_type, VBUF *pprefix )
/************************************************************/
{
    VBUF parms;

    FormatTemplateParms( &parms, class_type );
    VbufConcVbuf( pprefix, &parms );
    VbufFree( &parms );
}

static void fmtUnboundTemplateParms( VBUF *pprefix, TYPE type )
{
    VBUF parms;

    FormatUnboundTemplateParms( &parms, type );
    VbufConcVbuf( pprefix, &parms );
    VbufFree( &parms );
}

static void fmtTypePush( FMT_INFO **pStackFMT, TYPE type, FMT_CONTROL control )
/*****************************************************************************/
{
    FMT_INFO *entry;
    FMT_INFO *main_function;

    main_function = NULL;
    while( type != NULL ) {
        entry = StackCarveAlloc( carveFMT, pStackFMT );
        entry->type = type;
        entry->main_function = FALSE;
        if( type->id == TYP_ENUM ) break;
        if( type->id == TYP_GENERIC ) break;
        if( type->id == TYP_CHAR ) break;
        if( type->id == TYP_BOOL ) break;
        if( type->id == TYP_TYPEDEF && control & FF_TYPEDEF_STOP ) break;
        if( type->id == TYP_FUNCTION ) {
            if( main_function == NULL ) {
                entry->main_function = TRUE;
                main_function = entry;
            }
            if( control & FF_DROP_RETURN ) break;
        }
        type = type->of;
    }
}

char *FormatErrorType( TYPE err_type )
/************************************/
{
    DbgAssert( err_type != NULL && err_type->id == TYP_ERROR );
    if( err_type->flag & TF1_SPECIAL_FMT ) {
        return( errFormats[ err_type->u.e.fmt ] );
    }
    return( typeName[ TYP_ERROR ] );
}

void FormatFunctionType( TYPE type, VBUF *pprefix, VBUF *psuffix, int num_def,
/****************************************************************************/
    FMT_CONTROL control )
{
    TYPE        top_type;
    TYPE        class_type;
    FMT_LR      lr_state;
    FMT_INFO    *StackFMT;
    FMT_INFO    *top;
    type_flag   flags;
    NAME        name;
    AUX_INFO    *pragma;
    int         use_def;
    FMT_CONTROL fn_control;

    VbufInit( pprefix );
    VbufInit( psuffix );
    if( type == NULL ) {
        VbufConcStr( pprefix, nullType );
    } else {
        lr_state = RIGHT;
        StackFMT = NULL;
        fmtTypePush( &StackFMT, type, control );
        top = StackPop( &StackFMT );
        while( top ) {
            top_type = top->type;
            switch( top_type->id ) {
            case TYP_ERROR:
                VbufConcStr( pprefix, FormatErrorType( top_type ) );
                break;
            case TYP_BOOL:
            case TYP_CHAR:
            case TYP_SINT:
            case TYP_UINT:
            case TYP_SCHAR:
            case TYP_UCHAR:
            case TYP_WCHAR:
            case TYP_SSHORT:
            case TYP_SLONG:
            case TYP_ULONG:
            case TYP_SLONG64:
            case TYP_ULONG64:
            case TYP_FLOAT:
            case TYP_DOUBLE:
            case TYP_LONG_DOUBLE:
            case TYP_DOT_DOT_DOT:
            case TYP_VOID:
                VbufConcStr( pprefix, typeName[top->type->id] );
                break;
            case TYP_GENERIC:
                VbufConcChr( pprefix, '?' );
                VbufConcDecimal( pprefix, top->type->u.g.index );
                VbufConcChr( pprefix, ' ' );
                break;
            case TYP_USHORT:
                flags = top->type->flag;
                if( flags != TF1_NULL ) {
                    fmtTypeFlag( flags, pprefix, ushortFlags );
                } else {
                    VbufConcStr( pprefix, typeName[top->type->id] );
                }
                break;
            case TYP_POINTER:
                fmtTypeChangeState( &lr_state, RIGHT, pprefix, psuffix );
                flags = top->type->flag;
                if( flags != TF1_NULL ) {
                    fmtTypeFlag( flags, pprefix, pointerFlags );
                } else {
                    VbufConcStr( pprefix, typeName[top->type->id] );
                }
                break;
            case TYP_MEMBER_POINTER:
                fmtTypeChangeState( &lr_state, RIGHT, pprefix, psuffix );
                class_type = MemberPtrClass( top->type );
                if( class_type != NULL ) {
                    fmtTypeScope( class_type->u.c.scope->enclosing,
                                  pprefix );
                    name = SimpleTypeName( class_type );
                    if( name != NULL ) {
                        VbufConcStr( pprefix, NameStr( name ) );
                    } else {
                        VbufConcStr( pprefix, memberPointer );
                    }
                } else {
                    VbufConcStr( pprefix, memberPointer );
                }
                VbufConcStr( pprefix, typeName[top->type->id] );
                break;
            case TYP_TYPEDEF:
                if( !(control & FF_TYPEDEF_STOP) ) break;
                // otherwise drop through
            case TYP_ENUM:
                fmtTypeScope( top->type->u.t.scope, pprefix );
                name = SimpleTypeName( top->type );
                if( name == NULL ) {
                    VbufConcStr( pprefix, typeName[top->type->id] );
                } else {
                    VbufConcStr( pprefix, NameStr( name ) );
                    VbufConcStr( pprefix, whiteSpace );
                }
                break;
            case TYP_CLASS:
                fmtTypeScope( top->type->u.c.scope->enclosing, pprefix );
                flags = top->type->flag;
                name = SimpleTypeName( top->type );
                if( name == NULL ) {
                    if( flags != TF1_NULL ) {
                        fmtTypeFlag( flags, pprefix, classFlags );
                    } else {
                        VbufConcStr( pprefix, typeName[top->type->id] );
                    }
                } else {
                    VbufConcStr( pprefix, NameStr( name ) );
                    if( flags & TF1_INSTANTIATION ) {
                        fmtTemplateParms( top->type, pprefix );
                    } else if( flags & TF1_UNBOUND ) {
                        fmtUnboundTemplateParms( pprefix, top->type );
                    }
                    VbufConcStr( pprefix, whiteSpace );
                }
                break;
            case TYP_BITFIELD:
                fmtTypeBitfield( top->type, psuffix );
                break;
            case TYP_FUNCTION:
                fmtTypeChangeState( &lr_state, LEFT, pprefix, psuffix );
                flags = top->type->flag;
                fmtTypeFlag( flags, pprefix, functionFlags );
                fmtTypePragma( top->type, pprefix );
                fn_control = control;
                if( top->main_function ) {
                    use_def = num_def;
                } else {
                    fn_control &= ~FF_ARG_NAMES;
                    use_def = 0;
                }
                fmtTypeFunction( TypeArgList( top->type )
                               , psuffix
                               , use_def
                               , fn_control );
                break;
            case TYP_ARRAY:
                fmtTypeChangeState( &lr_state, LEFT, pprefix, psuffix );
                fmtTypeArray( top->type, psuffix );
                break;
            case TYP_MODIFIER:
                flags = top->type->flag;
                pragma = top->type->u.m.pragma;
                if(( flags & TF1_DISPLAY ) == TF1_NULL && pragma == NULL ) {
                    lr_state = RIGHT;
                } else {
                    if( willPrintModifier( type, flags ) ||
                        willPrintBased( flags ) ) {
                        fmtTypeChangeState( &lr_state, RIGHT, pprefix, psuffix );
                        fmtModifierTypeFlag( type, flags, pprefix );
                        fmtTypeBased( top->type, pprefix );
                    } else {
                        lr_state = RIGHT;
                    }
                    fmtTypePragma( top->type, pprefix );
                }
                break;
            }
            CarveFree( carveFMT, top );
            top = StackPop( &StackFMT );
        }
        while( top ) {
            CarveFree( carveFMT, top );
            top = StackPop( &StackFMT );
        }
        VbufTruncWhite( psuffix );
    }
}


void FormatType( TYPE type, VBUF *pprefix, VBUF *psuffix )
/********************************************************/
{
    FormatFunctionType( type, pprefix, psuffix, 0, FormatTypeDefault );
}

void FormatTypeModFlags( type_flag flags, VBUF *pvbuf )
/*****************************************************/
{
    VbufInit( pvbuf );
    fmtTypeFlag( flags, pvbuf, modifierFlags );
}

void FormatPTreeList( PTREE p, VBUF *pvbuf )
/******************************************/
{
    VbufInit( pvbuf );

    while( ( p != NULL ) && ( p->u.subtree[1] != NULL ) ) {
        PTREE right;

        DbgAssert( ( p->op == PT_BINARY ) && ( p->cgop == CO_LIST ) );
        right = p->u.subtree[1];

        switch( right->op ) {
        case PT_TYPE:
        {
            VBUF prefix, suffix;
            FormatType( right->type, &prefix, &suffix );
            VbufConcVbuf( pvbuf, &prefix );
            VbufConcVbuf( pvbuf, &suffix );
            VbufFree( &prefix );
            VbufFree( &suffix );
        }
        break;

        case PT_INT_CONSTANT:
            VbufConcInteger( pvbuf, right->u.int_constant );
            break;

        case PT_SYMBOL:
        {
            VBUF prefix, suffix;
            FormatType( right->u.symcg.symbol->sym_type, &prefix, &suffix );
            VbufConcVbuf( pvbuf, &prefix );
            VbufConcVbuf( pvbuf, &suffix );
            VbufFree( &prefix );
            VbufFree( &suffix );
        }
        break;

        default:
            DbgAssert( 0 );
        }

        VbufConcStr( pvbuf, ", " );
        p = p->u.subtree[0];
    }

    if( VbufLen( pvbuf ) >= 2 ) {
        VbufSetPosBack( pvbuf, 2 );
        VbufConcStr( pvbuf, "" );
    }
}

static VBUF *vbuf_FormatPTreeId;
static PTREE traverse_FormatPTreeId( PTREE curr )
{
    if( curr->op == PT_BINARY ) {
        if( curr->cgop == CO_COLON_COLON ) {
            VbufConcStr( vbuf_FormatPTreeId, "::" );
        }
    } else if( curr->op == PT_ID ) {
        if( curr->u.id.name != NULL ) {
            VbufConcStr( vbuf_FormatPTreeId, NameStr( curr->u.id.name ) );
        }
    }

    return curr;
}

void FormatPTreeId( PTREE p, VBUF *pvbuf )
/******************************************/
{
    VbufInit( pvbuf );
    vbuf_FormatPTreeId = pvbuf;

    PTreeTraversePostfix( p, traverse_FormatPTreeId );
}

void FormatTemplateInfo( TEMPLATE_INFO *tinfo, VBUF *pvbuf )
/**********************************************************/
{
    TEMPLATE_SPECIALIZATION * const tprimary =
        RingFirst( tinfo->specializations );
    unsigned int i;

    VbufInit( pvbuf );

    VbufConcStr( pvbuf, NameStr( tinfo->sym->name->name ) );
    VbufConcStr( pvbuf, "<" );
    for( i = 0; i < tprimary->num_args; i++ ) {
        VBUF prefix, suffix;

        FormatType( tprimary->type_list[i], &prefix, &suffix );
        VbufConcVbuf( pvbuf, &prefix );
        VbufConcVbuf( pvbuf, &suffix );
        VbufFree( &prefix );
        VbufFree( &suffix );
        VbufConcStr( pvbuf, ", " );
    }

    if( tprimary->num_args > 0 ) {
        VbufSetPosBack( pvbuf, 2 );
        VbufConcStr( pvbuf, "" );
    }

    VbufConcStr( pvbuf, ">" );
}

void FormatTemplateSpecialization( TEMPLATE_SPECIALIZATION *tspec, VBUF *pvbuf)
/*****************************************************************************/
{
    if( tspec->spec_args != NULL ) {
        VBUF prefix;

        VbufInit( pvbuf );

        VbufConcStr( pvbuf, NameStr( tspec->tinfo->sym->name->name ) );
        VbufConcStr( pvbuf, "<" );

        FormatPTreeList( tspec->spec_args, &prefix );
        VbufConcVbuf( pvbuf, &prefix );
        VbufFree( &prefix );

        VbufConcStr( pvbuf, ">" );
    } else {
        FormatTemplateInfo( tspec->tinfo, pvbuf );
    }
}

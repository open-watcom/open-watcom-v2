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
#ifndef NDEBUG
    CarveVerifyAllGone( carveFMT, "FMT" );
#endif
    CarveDestroy( carveFMT );
}

INITDEFN( type_formatting, formatTypeInit, formatTypeFini )

static void fmtTypeArray( TYPE type, VBUF *pvbuf )
/************************************************/
{
    VBUF    working;
    char    sbuf[ 1 + sizeof( long ) * 2 + 1 ];

    VbufInit( &working );
    VStrNull( &working );
    VStrConcStr( &working, openSquareParen );
    if( type->u.a.array_size > 0 ) {
        ultoa( type->u.a.array_size, sbuf, 10 );
        VStrConcStr( &working, sbuf );
    }
    VStrConcStr( &working, closeSquareParen );
    VStrPrepStr( pvbuf, working.buf );
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
    VStrConcStr( pvbuf, openBased );
    switch( flags ) {
    case TF1_BASED_STRING:
        VStrConcStr( pvbuf, openSegname );
        VStrConcStr( pvbuf, ((STRING_CONSTANT)(type->u.m.base))->string );
        VStrConcStr( pvbuf, closeSegname );
        break;
    case TF1_BASED_VOID:
        VStrConcStr( pvbuf, basedVoid );
        break;
    case TF1_BASED_SELF:
        VStrConcStr( pvbuf, basedSelf );
        break;
    case TF1_BASED_FETCH:
        FormatSym( (SYMBOL)type->u.m.base, &working );
        VStrConcStr( pvbuf, working.buf );
        VStrConcStr( pvbuf, whiteSpace );
        VbufFree( &working );
        break;
    case TF1_BASED_ADD:
        FormatSym( (SYMBOL)type->u.m.base, &working );
        VStrConcStr( pvbuf, working.buf );
        VStrConcStr( pvbuf, whiteSpace );
        VbufFree( &working );
        break;
    }
    VStrConcStr( pvbuf, closeBased );
}

static void fmtNicePragma( void *pragma, VBUF *pvbuf )
/****************************************************/
{
    char *id;

    if( pragma == NULL ) {
        return;
    }
    if( PragmaName( pragma, &id ) ) {
        if( id != NULL ) {
            VStrConcStr( pvbuf, id );
            VStrConcStr( pvbuf, whiteSpace );
        }
    } else {
        if( id != NULL ) {
            VStrConcStr( pvbuf, openPragma );
            VStrConcStr( pvbuf, id );
            VStrConcStr( pvbuf, closePragma );
        }
    }
}

static void fmtTypePragma( TYPE type, VBUF *pvbuf )
/*************************************************/
{
    void *pragma;

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
    char    sbuf[ 1 + sizeof( long ) * 2 + 1 ];

    VbufInit( &working );
    VStrNull( &working );
    VStrConcStr( &working, bitfieldSep );
    ultoa( type->u.b.field_width, sbuf, 10 );
    VStrConcStr( &working, sbuf );
    VStrConcStr( &working, whiteSpace );
    VStrPrepStr( pvbuf, working.buf );
    VbufFree( &working );
}

static char *getArgName( unsigned index )
{
    char *name;
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
    char *name;

    arg_control = FormatTypeDefault | control;
    arg_control &= ~FF_ARG_NAMES;
    FormatFunctionType( arg_type, &arg_prefix, &arg_suffix, 0, arg_control );
    if( arg_prefix.buf != NULL ) {
        VStrConcStr( pvbuf, arg_prefix.buf );
    }
    if( control & FF_ARG_NAMES ) {
        name = getArgName( arg_index );
        if( name != NULL ) {
            VStrConcStr( pvbuf, name );
        }
    }
    if( arg_suffix.buf != NULL ) {
        VStrConcStr( pvbuf, arg_suffix.buf );
    }
    VbufFree( &arg_prefix );
    VbufFree( &arg_suffix );
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
    VStrNull( &working );
    VStrConcStr( &working, openFunction );
    num_args = alist->num_args;
    num_def = num_args - num_def;
    if( num_args == 0 ) {
        if( control & FF_USE_VOID ) {
            VStrConcStr( &working, typeName[TYP_VOID] );
        } else {
            VStrTruncWhite( &working );
        }
    } else {
        // only keep whether we want typedef names
        arg_control = control & ( FF_TYPEDEF_STOP | FF_ARG_NAMES );
        for( i = 0 ; i < num_def ; i++ ) {
            if( i > 0 ) {
                VStrTruncWhite( &working );
                VStrConcStr( &working, parameterSep );
            }
            fmtTypeArgument( alist->type_list[i], i, &working, arg_control );
        }
        if( num_args > num_def ) {
            VStrConcStr( &working, openSquareParen );
            for( ; i < num_args ; i++ ) {
                if( i > 0 ) {
                    VStrTruncWhite( &working );
                    VStrConcStr( &working, parameterSep );
                }
                fmtTypeArgument( alist->type_list[i], i, &working, arg_control );
            }
            VStrConcStr( &working, closeSquareParen );
        }
    }
    VStrConcStr( &working, closeFunction );
    fmtTypeFlag( alist->qualifier, &working, modifierFlags );
    VStrPrepStr( pvbuf, working.buf );
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
                VStrConcStr( pvbuf, modifierFlags[i].name );
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
static void fmtTypeFlag( type_flag flag, VBUF *pvbuf,
/***************************************************/
    FMT_FLAG_INFO *flag_info )
{
    unsigned    i;
    type_flag   mask;

    for( i = 0 ; flag_info[i].name != NULL ; i++ ) {
        mask = flag_info[i].mask;
        if( (flag & mask) == mask ) {
            VStrConcStr( pvbuf, flag_info[i].name );
        }
    }
}

static void fmtTypeChangeState( FMT_LR *curr, FMT_LR new,
/*******************************************************/
    VBUF *pprefix, VBUF *psuffix )
{
    if( new == RIGHT && *curr == LEFT ) {
        VStrConcStr( pprefix, openParen );
        VStrPrepStr( psuffix, closeParen );
    }
    *curr = new;
}

static void fmtTypeScope( SCOPE scope, VBUF *pprefix )
/****************************************************/
{
    VBUF name_scope;

    FormatScope( scope, &name_scope, FALSE );
    if( name_scope.buf != NULL ) {
        VStrConcStr( pprefix, name_scope.buf );
    }
    VbufFree( &name_scope );
}

static void fmtTemplateParms( TYPE class_type, VBUF *pprefix )
/************************************************************/
{
    VBUF parms;

    FormatTemplateParms( &parms, class_type );
    VStrConcStr( pprefix, parms.buf );
    VbufFree( &parms );
}

static void fmtUnboundTemplateParms( VBUF *pprefix, TYPE type )
{
    VBUF parms;

    FormatUnboundTemplateParms( &parms, type );
    VStrConcStr( pprefix, parms.buf );
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
    char        *name;
    void        *pragma;
    int         use_def;
    FMT_CONTROL fn_control;

    VbufInit( pprefix );
    VStrNull( pprefix );
    VbufInit( psuffix );
    VStrNull( psuffix );
    if( type == NULL ) {
        VStrConcStr( pprefix, nullType );
    } else {
        lr_state = RIGHT;
        StackFMT = NULL;
        fmtTypePush( &StackFMT, type, control );
        top = StackPop( &StackFMT );
        while( top ) {
            top_type = top->type;
            switch( top_type->id ) {
            case TYP_ERROR:
                VStrConcStr( pprefix, FormatErrorType( top_type ) );
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
            case TYP_GENERIC:
                VStrConcStr( pprefix, typeName[top->type->id] );
                break;
            case TYP_USHORT:
                flags = top->type->flag;
                if( flags != TF1_NULL ) {
                    fmtTypeFlag( flags, pprefix, ushortFlags );
                } else {
                    VStrConcStr( pprefix, typeName[top->type->id] );
                }
                break;
            case TYP_POINTER:
                fmtTypeChangeState( &lr_state, RIGHT, pprefix, psuffix );
                flags = top->type->flag;
                if( flags != TF1_NULL ) {
                    fmtTypeFlag( flags, pprefix, pointerFlags );
                } else {
                    VStrConcStr( pprefix, typeName[top->type->id] );
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
                        VStrConcStr( pprefix, name );
                    } else {
                        VStrConcStr( pprefix, memberPointer );
                    }
                } else {
                    VStrConcStr( pprefix, memberPointer );
                }
                VStrConcStr( pprefix, typeName[top->type->id] );
                break;
            case TYP_TYPEDEF:
                if( !(control & FF_TYPEDEF_STOP) ) break;
                // otherwise drop through
            case TYP_ENUM:
                fmtTypeScope( top->type->u.t.scope, pprefix );
                name = SimpleTypeName( top->type );
                if( name == NULL ) {
                    VStrConcStr( pprefix, typeName[top->type->id] );
                } else {
                    VStrConcStr( pprefix, name );
                    VStrConcStr( pprefix, whiteSpace );
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
                        VStrConcStr( pprefix, typeName[top->type->id] );
                    }
                } else {
                    VStrConcStr( pprefix, name );
                    if( flags & TF1_INSTANTIATION ) {
                        fmtTemplateParms( top->type, pprefix );
                    } else if( flags & TF1_UNBOUND ) {
                        fmtUnboundTemplateParms( pprefix, top->type );
                    }
                    VStrConcStr( pprefix, whiteSpace );
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
        VStrTruncWhite( psuffix );
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
    VStrNull( pvbuf );
    fmtTypeFlag( flags, pvbuf, modifierFlags );
}

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
#include "errdefns.h"
#include "ppops.h"
#include "vbuf.h"
#include "dbg.h"
#include "fmttype.h"
#include "template.h"
#include "fmtsym.h"


static char scopeError[]        = "***SCOPE ERROR***";
static char scopeSep[]          = "::";
static char dtorPrefix[]        = "~";
static char functionDelim[]     = "' in '";
static char constructorName[]   = "constructor";
static char destructorName[]    = "destructor";
static char operatorSuffix[]    = "()";
static char operatorPrefix[]    = "operator ";
static char operatorUnknown[]   = "conversion operator";
static char nullSymbol[]        = "***NULL SYMBOL POINTER***";
static char nullSymname[]       = "<null name>";
static char templateParmStart[] = "<";
static char templateParmNext[]  = ",";
static char templateParmStop[]  = ">";
static char templateParmUnknown[] = "?";

static char *fmtSymCgop( CGOP number )
/************************************/
{
    char *name;                 // - name

    static char *opNames[] ={   // - opcode names (binary,unary)
    #include "ppopssym.h"
    };

    if( number >= ( sizeof( opNames ) / sizeof( opNames[0] ) ) ) {
        name = "***INVALID CGOP***";
    } else if( strlen( opNames[ number ] ) == 0 ) {
        name = "***INVALID CGOP LENGTH***";
    } else {
        name = opNames[ number ];
    }
    return( name );
}

static void fmtSymFunction( SYMBOL sym, VBUF *prefix, VBUF *suffix,
/*****************************************************************/
    FMT_CONTROL control )
{
    unsigned num_def;
    TYPE fn_type;

    num_def = 0;
    for( ; sym->id == SC_DEFAULT; sym = sym->thread ) {
        ++num_def;
    }
    fn_type = sym->sym_type;
    if( num_def != 0 && TypeHasEllipsisArg( fn_type ) ) {
        /* '...' is an extra last argument in the type if it has defargs */
        ++num_def;
    }
    FormatFunctionType( fn_type, prefix, suffix, num_def, control );
}

static void fmtSymOpName( SYMBOL sym, VBUF *pvbuf )
/*************************************************/
{
    VBUF    prefix, suffix;
    TYPE type;

    VbufInit( pvbuf );
    VStrNull( pvbuf );
    type = FunctionDeclarationType( sym->sym_type );
    if( type != NULL ) {
        FormatFunctionType( type->of
                          , &prefix
                          , &suffix
                          , 0
                          , FormatTypeDefault | FF_TYPEDEF_STOP );
        if( suffix.buf != NULL ) {
            VStrConcStrRev( pvbuf, suffix.buf );
        }
        VStrTruncWhite( pvbuf );
        if( prefix.buf != NULL ) {
            VStrConcStrRev( pvbuf, prefix.buf );
        }
        VbufFree( &prefix );
        VbufFree( &suffix );
    }
}

static boolean fmtSymName( SYMBOL sym, char *name, VBUF *pvprefix,
/****************************************************************/
    VBUF *pvbuf, FMT_CONTROL control )
// returns TRUE if sym is CTOR/DTOR so that the caller drop the return type
{
    VBUF    prefix, suffix, op_name;
    CGOP    oper;
    boolean ctordtor = FALSE;

    if( name == NULL ) {
        name = nullSymname;
    }
    if( CppLookupName( name, &oper ) ) {
        switch( oper ) {
        case CO_CONVERT:
            if( sym == NULL ) {
                VStrConcStrRev( pvbuf, operatorSuffix );
                VStrConcStrRev( pvbuf, operatorUnknown );
            } else {
                fmtSymOpName( sym, &op_name );
                fmtSymFunction( sym
                              , &prefix
                              , &suffix
                              , (FormatTypeDefault & ~FF_USE_VOID)
                                                   | FF_DROP_RETURN );
                if( suffix.buf != NULL ) {
                    VStrConcStrRev( pvbuf, suffix.buf );
                }
                VStrConcStr( pvbuf, op_name.buf );
                VStrConcStrRev( pvbuf, operatorPrefix );
                if( prefix.buf != NULL ) {
                    VStrConcStrRev( pvbuf, prefix.buf );
                }
                VbufFree( &op_name );
                VbufFree( &prefix );
                VbufFree( &suffix );
            }
            break;
        case CO_CTOR:
            if( sym == NULL ) {
                VStrConcStrRev( pvbuf, constructorName );
            } else {
                ctordtor = TRUE;
                name = SimpleTypeName( ScopeClass( SymScope( sym ) ) );
                fmtSymFunction( sym, &prefix, &suffix, FormatTypeDefault );
                if( suffix.buf != NULL ) {
                    VStrConcStrRev( pvbuf, suffix.buf );
                }
                if( name != NULL ) {
                    VStrConcStrRev( pvbuf, name );
                }
                if( prefix.buf != NULL ) {
                    VStrConcStrRev( pvprefix, prefix.buf );
                }
                VbufFree( &prefix );
                VbufFree( &suffix );
            }
            break;
        case CO_DTOR:
            if( sym == NULL ) {
                VStrConcStrRev( pvbuf, destructorName );
            } else {
                ctordtor = TRUE;
                name = SimpleTypeName( ScopeClass( SymScope( sym ) ) );
                fmtSymFunction( sym, &prefix, &suffix, FormatTypeDefault );
                if( suffix.buf != NULL ) {
                    VStrConcStrRev( pvbuf, suffix.buf );
                }
                if( name != NULL ) {
                    VStrConcStrRev( pvbuf, name );
                }
                VStrConcStrRev( pvbuf, dtorPrefix );
                if( prefix.buf != NULL ) {
                    VStrConcStrRev( pvprefix, prefix.buf );
                }
                VbufFree( &prefix );
                VbufFree( &suffix );
            }
            break;
        default:
            if( sym == NULL ) {
                VStrConcStrRev( pvbuf, fmtSymCgop( oper ) );
                VStrConcStrRev( pvbuf, operatorPrefix );
            } else {
                fmtSymFunction( sym, &prefix, &suffix, FormatTypeDefault | control );
                if( suffix.buf != NULL ) {
                    VStrConcStrRev( pvbuf, suffix.buf );
                }
                VStrConcStrRev( pvbuf, fmtSymCgop( oper ) );
                VStrConcStrRev( pvbuf, operatorPrefix );
                if( prefix.buf != NULL ) {
                    VStrConcStrRev( pvprefix, prefix.buf );
                }
                VbufFree( &prefix );
                VbufFree( &suffix );
            }
            break;
        }
    } else {
        if( sym == NULL ) {
            VStrConcStrRev( pvbuf, name );
        } else {
            if( SymIsFunction( sym ) ) {
                fmtSymFunction( sym, &prefix, &suffix, FormatTypeDefault | control );
            } else if( !SymIsTypedef( sym ) ) {
                FormatType( sym->sym_type, &prefix, &suffix );
            } else {
                VbufInit( &prefix );
                VbufInit( &suffix );
            }
            if( suffix.buf != NULL ) {
                VStrConcStrRev( pvbuf, suffix.buf );
            }
            VStrConcStrRev( pvbuf, name );
            if( prefix.buf != NULL ) {
                VStrConcStrRev( pvprefix, prefix.buf );
            }
            VbufFree( &prefix );
            VbufFree( &suffix );
        }
    }
    return( ctordtor );
}

static void formatScopedSym( SYMBOL sym, VBUF *pvbuf, FMT_CONTROL control )
/*************************************************************************/
{
    VBUF    prefix;
    boolean ctordtor;

    VbufInit( &prefix );
    VStrNull( &prefix );
    if( sym->name == NULL ) {
        ctordtor = fmtSymName( sym, NULL, &prefix, pvbuf, control );
    } else {
        ctordtor = fmtSymName( sym, sym->name->name, &prefix, pvbuf, control );
    }
    if( !SymIsAnonymous( sym ) ) {
        fmtSymScope( SymScope( sym ), pvbuf, TRUE );
    }
    if( !ctordtor && prefix.buf != NULL ) {
        VStrConcStr( pvbuf, prefix.buf );
    }
    VbufFree( &prefix );
}

static void makeUnknownTemplate( VBUF *parms )
{
    VStrConcStr( parms, templateParmStart );
    VStrConcStr( parms, templateParmUnknown );
    VStrConcStr( parms, templateParmStop );
}

void FormatUnboundTemplateParms( VBUF *parms, TYPE type )
/*******************************************************/
{
    FormatTemplateParms( parms, type );
}

void FormatTemplateParms( VBUF *parms, TYPE class_type )
/******************************************************/
{
    SYMBOL stop;
    SYMBOL curr;
    SYMBOL sym;
    SCOPE parm_scope;
    char *delim;
    TYPE type;
    auto VBUF sym_parm;
    auto VBUF type_parm_prefix;
    auto VBUF type_parm_suffix;
    auto char buff[32];

    VbufInit( parms );
    VStrNull( parms );
    parm_scope = TemplateClassParmScope( class_type );
    if( parm_scope == NULL ) {
        makeUnknownTemplate( parms );
        return;
    }
    delim = templateParmStart;
    curr = NULL;
    stop = ScopeOrderedStart( parm_scope );
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        VStrConcStr( parms, delim );
        type = curr->sym_type;
        if( SymIsConstantInt( curr ) ) {
            if( UnsignedIntType( type ) ) {
                ultoa( curr->u.uval, buff, 10 );
            } else {
                ltoa( curr->u.sval, buff, 10 );
            }
            VStrConcStr( parms, buff );
        } else if( SymIsTypedef( curr ) ) {
            FormatType( type, &type_parm_prefix, &type_parm_suffix );
            VStrTruncWhite( &type_parm_prefix );
            VStrTruncWhite( &type_parm_suffix );
            VStrConcStr( parms, type_parm_prefix.buf );
            VStrConcStr( parms, type_parm_suffix.buf );
            VbufFree( &type_parm_prefix );
            VbufFree( &type_parm_suffix );
        } else {
            sym = SymAddressOf( curr );
            if( sym != NULL ) {
                FormatSym( sym, &sym_parm );
                VStrConcStr( parms, sym_parm.buf );
                VbufFree( &sym_parm );
            }
        }
        delim = templateParmNext;
    }
    VStrConcStr( parms, templateParmStop );
}

static void fmtTemplateParms( VBUF *pvbuf, TYPE class_type )
{
    auto VBUF parms;

    FormatTemplateParms( &parms, class_type );
    VStrConcStrRev( pvbuf, parms.buf );
    VbufFree( &parms );
}

static void fmtSymScope( SCOPE scope, VBUF *pvbuf, boolean include_function )
/***************************************************************************/
{
    TYPE    class_type;
    char    *scope_name;
    SYMBOL  sym;
    VBUF    prefix;

    while( scope != NULL ) {
        switch( scope->id ) {
        case SCOPE_FILE:
            scope_name = ScopeNameSpaceFormatName( scope );
            if( scope_name != NULL ) {
                VStrConcStrRev( pvbuf, scopeSep );
                VStrConcStrRev( pvbuf, scope_name );
            }
            break;
        case SCOPE_CLASS:
            class_type = ScopeClass( scope );
            scope_name = SimpleTypeName( class_type );
            if( scope_name != NULL ) {
                VStrConcStrRev( pvbuf, scopeSep );
                if( class_type->flag & TF1_INSTANTIATION ) {
                    fmtTemplateParms( pvbuf, class_type );
                }
                VStrConcStrRev( pvbuf, scope_name );
            }
            break;
        case SCOPE_FUNCTION:
            if( include_function ) {
                VbufInit( &prefix );
                VStrNull( &prefix );
                sym = ScopeFunction( scope );
                formatScopedSym( sym, &prefix, FF_NULL );
                if( prefix.buf != NULL ) {
                    VStrConcStrRev( &prefix, functionDelim );
                    VStrPrepStr( pvbuf, prefix.buf );
                }
                VbufFree( &prefix );
            }
            return;             // function scope resolved on function name
        case SCOPE_BLOCK:
        case SCOPE_TEMPLATE_DECL:
        case SCOPE_TEMPLATE_INST:
        case SCOPE_TEMPLATE_PARM:
            break;
        case SCOPE_MAX:
        default:
            VStrConcStrRev( pvbuf, scopeSep );
            VStrConcStrRev( pvbuf, scopeError );
            break;
        }
        scope = scope->enclosing;
    }
}

void FormatScope( SCOPE scope, VBUF *pvbuf, boolean include_function )
/********************************************************************/
// include_function - if TRUE, include function scope resolution in formatting
//                  - else terminate scope resolution at function
{
    VbufInit( pvbuf );
    VStrNull( pvbuf );
    fmtSymScope( scope, pvbuf, include_function );
    strrev( pvbuf->buf );
}

static void doFormatSym( SYMBOL sym, VBUF *pvbuf, FMT_CONTROL control )
{
    VbufInit( pvbuf );
    VStrNull( pvbuf );
    if( sym == NULL ) {
        VStrConcStr( pvbuf, nullSymbol );
    } else {
        formatScopedSym( sym, pvbuf, control );
        strrev( pvbuf->buf );
    }
}

void FormatSym( SYMBOL sym, VBUF *pvbuf )
/***************************************/
{
    doFormatSym( sym, pvbuf, FF_NULL );
}

void FormatSymWithTypedefs( SYMBOL sym, VBUF *pvbuf )
/***************************************************/
{
    doFormatSym( sym, pvbuf, FF_TYPEDEF_STOP );
}

void FormatFnDefnWithTypedefs( SYMBOL sym, VBUF *pvbuf )
/******************************************************/
{
    doFormatSym( sym, pvbuf, FF_TYPEDEF_STOP | FF_ARG_NAMES );
}

void FormatFnDefn( SYMBOL sym, VBUF *pvbuf )
/******************************************/
{
    doFormatSym( sym, pvbuf, FF_ARG_NAMES );
}

void FormatName( char *name, VBUF *pvbuf )
/****************************************/
{
    VBUF    prefix;
    boolean ctordtor;

    VbufInit( pvbuf );
    VStrNull( pvbuf );
    VbufInit( &prefix );
    VStrNull( &prefix );
    ctordtor = fmtSymName( NULL, name, &prefix, pvbuf, FF_NULL );
    if( !ctordtor && prefix.buf != NULL ) {
        VStrConcStr( pvbuf, prefix.buf );
    }
    VbufFree( &prefix );
    strrev( pvbuf->buf );
}

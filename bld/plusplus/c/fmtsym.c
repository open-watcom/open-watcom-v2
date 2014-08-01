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
#include "ppops.h"
#include "vbuf.h"
#include "dbg.h"
#include "fmttype.h"
#include "template.h"
#include "fmtsym.h"
#include "clibext.h"


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

static void fmtSymFunction( SYMBOL sym, VBUF *prefix, VBUF *suffix, FMT_CONTROL control )
/***************************************************************************************/
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
    type = FunctionDeclarationType( sym->sym_type );
    if( type != NULL ) {
        FormatFunctionType( type->of
                          , &prefix
                          , &suffix
                          , 0
                          , FormatTypeDefault | FF_TYPEDEF_STOP );
        VbufConcVbufRev( pvbuf, &suffix );
        VbufTruncWhite( pvbuf );
        VbufConcVbufRev( pvbuf, &prefix );
        VbufFree( &prefix );
        VbufFree( &suffix );
    }
}

static bool fmtSymName( SYMBOL sym, NAME name, VBUF *pvprefix, VBUF *pvbuf, FMT_CONTROL control )
/***********************************************************************************************/
// returns TRUE if sym is CTOR/DTOR so that the caller drop the return type
{
    VBUF    prefix, suffix, op_name;
    CGOP    oper;
    bool    ctordtor = FALSE;

    if( CppLookupOperatorName( name, &oper ) ) {
        switch( oper ) {
        case CO_CONVERT:
            if( sym == NULL ) {
                VbufConcStrRev( pvbuf, operatorSuffix );
                VbufConcStrRev( pvbuf, operatorUnknown );
            } else {
                fmtSymOpName( sym, &op_name );
                fmtSymFunction( sym, &prefix, &suffix, (FormatTypeDefault & ~FF_USE_VOID) | FF_DROP_RETURN );
                VbufConcVbufRev( pvbuf, &suffix );
                VbufConcVbuf( pvbuf, &op_name );
                VbufConcStrRev( pvbuf, operatorPrefix );
                VbufConcVbufRev( pvbuf, &prefix );
                VbufFree( &op_name );
                VbufFree( &prefix );
                VbufFree( &suffix );
            }
            break;
        case CO_CTOR:
            if( sym == NULL ) {
                VbufConcStrRev( pvbuf, constructorName );
            } else {
                name = SimpleTypeName( ScopeClass( SymScope( sym ) ) );
                ctordtor = TRUE;
                fmtSymFunction( sym, &prefix, &suffix, FormatTypeDefault );
                VbufConcVbufRev( pvbuf, &suffix );
                if( name != NULL ) {
                    VbufConcStrRev( pvbuf, NameStr( name ) );
                }
                VbufConcVbufRev( pvprefix, &prefix );
                VbufFree( &prefix );
                VbufFree( &suffix );
            }
            break;
        case CO_DTOR:
            if( sym == NULL ) {
                VbufConcStrRev( pvbuf, destructorName );
            } else {
                name = SimpleTypeName( ScopeClass( SymScope( sym ) ) );
                ctordtor = TRUE;
                fmtSymFunction( sym, &prefix, &suffix, FormatTypeDefault );
                VbufConcVbufRev( pvbuf, &suffix );
                if( name != NULL ) {
                    VbufConcStrRev( pvbuf, NameStr( name ) );
                }
                VbufConcStrRev( pvbuf, dtorPrefix );
                VbufConcVbufRev( pvprefix, &prefix );
                VbufFree( &prefix );
                VbufFree( &suffix );
            }
            break;
        default:
            if( sym == NULL ) {
                VbufConcStrRev( pvbuf, fmtSymCgop( oper ) );
                VbufConcStrRev( pvbuf, operatorPrefix );
            } else {
                fmtSymFunction( sym, &prefix, &suffix, FormatTypeDefault | control );
                VbufConcVbufRev( pvbuf, &suffix );
                VbufConcStrRev( pvbuf, fmtSymCgop( oper ) );
                VbufConcStrRev( pvbuf, operatorPrefix );
                VbufConcVbufRev( pvprefix, &prefix );
                VbufFree( &prefix );
                VbufFree( &suffix );
            }
            break;
        }
    } else if( sym != NULL ) {
        if( SymIsFunction( sym ) ) {
            fmtSymFunction( sym, &prefix, &suffix, FormatTypeDefault | control );
        } else if( !SymIsTypedef( sym ) ) {
            FormatType( sym->sym_type, &prefix, &suffix );
        } else {
            VbufInit( &prefix );
            VbufInit( &suffix );
        }
        VbufConcVbufRev( pvbuf, &suffix );
        if( name == NULL ) {
            VbufConcStrRev( pvbuf, nullSymname );
        } else {
            VbufConcStrRev( pvbuf, NameStr( name ) );
        }
        VbufConcVbufRev( pvprefix, &prefix );
        VbufFree( &prefix );
        VbufFree( &suffix );
    } else if( name != NULL ) {
        VbufConcStrRev( pvbuf, NameStr( name ) );
    } else {
        VbufConcStrRev( pvbuf, nullSymname );
    }
    return( ctordtor );
}

static void fmtSymScope( SCOPE scope, VBUF *pvbuf, bool include_function );

static char *formatScopedSym( SYMBOL sym, VBUF *pvbuf, FMT_CONTROL control )
/**************************************************************************/
{
    VBUF    prefix;
    bool    ctordtor;

    VbufInit( &prefix );
    if( sym->name == NULL ) {
        ctordtor = fmtSymName( sym, NULL, &prefix, pvbuf, control );
    } else {
        ctordtor = fmtSymName( sym, sym->name->name, &prefix, pvbuf, control );
    }
    if( !SymIsAnonymous( sym ) ) {
        fmtSymScope( SymScope( sym ), pvbuf, TRUE );
    }
    if( !ctordtor ) {
        VbufConcVbuf( pvbuf, &prefix );
    }
    VbufFree( &prefix );
    return( VbufString( pvbuf ) );
}

static void makeUnknownTemplate( VBUF *parms )
{
    VbufConcStr( parms, templateParmStart );
    VbufConcStr( parms, templateParmUnknown );
    VbufConcStr( parms, templateParmStop );
}

void FormatTemplateParmScope( VBUF *parms, SCOPE parm_scope )
/***********************************************************/
{
    SYMBOL stop;
    SYMBOL curr;
    SYMBOL sym;
    char *delim;
    TYPE type;
    auto VBUF sym_parm;
    auto VBUF type_parm_prefix;
    auto VBUF type_parm_suffix;

    VbufInit( parms );
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
        VbufConcStr( parms, delim );
        type = curr->sym_type;
        if( SymIsConstantInt( curr ) ) {
            if( UnsignedIntType( type ) ) {
                VbufConcDecimal( parms, curr->u.uval );
            } else {
                VbufConcInteger( parms, curr->u.sval );
            }
        } else if( SymIsTypedef( curr ) ) {
            FormatType( type, &type_parm_prefix, &type_parm_suffix );
            VbufTruncWhite( &type_parm_prefix );
            VbufTruncWhite( &type_parm_suffix );
            VbufConcVbuf( parms, &type_parm_prefix );
            VbufConcVbuf( parms, &type_parm_suffix );
            VbufFree( &type_parm_prefix );
            VbufFree( &type_parm_suffix );
        } else {
            sym = SymAddressOf( curr );
            if( sym != NULL ) {
                FormatSym( sym, &sym_parm );
                VbufConcVbuf( parms, &sym_parm );
                VbufFree( &sym_parm );
            }
        }
        delim = templateParmNext;
    }
    if( delim == templateParmStart ) {
        VbufConcStr( parms, templateParmStart );
    }
    VbufConcStr( parms, templateParmStop );
}

void FormatUnboundTemplateParms( VBUF *parms, TYPE type )
/*******************************************************/
{
    FormatTemplateParms( parms, type );
}

void FormatTemplateParms( VBUF *parms, TYPE class_type )
/******************************************************/
{
    SCOPE parm_scope;

    parm_scope = TemplateClassParmScope( class_type );
    FormatTemplateParmScope( parms, parm_scope );
}

static void fmtTemplateParms( VBUF *pvbuf, TYPE class_type )
{
    auto VBUF parms;

    FormatTemplateParms( &parms, class_type );
    VbufConcVbufRev( pvbuf, &parms );
    VbufFree( &parms );
}

static void fmtSymScope( SCOPE scope, VBUF *pvbuf, bool include_function )
/************************************************************************/
{
    TYPE        class_type;
    const char  *scope_name;
    SYMBOL      sym;
    VBUF        prefix;

    while( scope != NULL ) {
        switch( scope->id ) {
        case SCOPE_FILE:
            scope_name = ScopeNameSpaceFormatName( scope );
            if( scope_name != NULL ) {
                VbufConcStrRev( pvbuf, scopeSep );
                VbufConcStrRev( pvbuf, scope_name );
            }
            break;
        case SCOPE_CLASS:
            class_type = ScopeClass( scope );
            scope_name = NameStr( SimpleTypeName( class_type ) );
            if( scope_name != NULL ) {
                VbufConcStrRev( pvbuf, scopeSep );
                if( class_type->flag & TF1_INSTANTIATION ) {
                    fmtTemplateParms( pvbuf, class_type );
                }
                VbufConcStrRev( pvbuf, scope_name );
            }
            break;
        case SCOPE_FUNCTION:
            if( include_function ) {
                VbufInit( &prefix );
                sym = ScopeFunction( scope );
                formatScopedSym( sym, &prefix, FF_NULL );
                if( VbufLen( &prefix ) > 0 ) {
                    VbufConcStrRev( &prefix, functionDelim );
                    VbufPrepVbuf( pvbuf, &prefix );
                }
                VbufFree( &prefix );
            }
            return;             // function scope resolved on function name
        case SCOPE_BLOCK:
        case SCOPE_TEMPLATE_DECL:
        case SCOPE_TEMPLATE_INST:
        case SCOPE_TEMPLATE_PARM:
        case SCOPE_TEMPLATE_SPEC_PARM:
            break;
        case SCOPE_MAX:
        default:
            VbufConcStrRev( pvbuf, scopeSep );
            VbufConcStrRev( pvbuf, scopeError );
            break;
        }
        scope = scope->enclosing;
    }
}

void FormatScope( SCOPE scope, VBUF *pvbuf, bool include_function )
/*****************************************************************/
// include_function - if TRUE, include function scope resolution in formatting
//                  - else terminate scope resolution at function
{
    VbufInit( pvbuf );
    fmtSymScope( scope, pvbuf, include_function );
    strrev( VbufString( pvbuf ) );
}

static char *doFormatSym( SYMBOL sym, VBUF *pvbuf, FMT_CONTROL control )
{
    VbufInit( pvbuf );
    if( sym == NULL ) {
        VbufConcStr( pvbuf, nullSymbol );
        return( VbufString( pvbuf ) );
    } else {
        return( strrev( formatScopedSym( sym, pvbuf, control ) ) );
    }
}

char *FormatSym( SYMBOL sym, VBUF *pvbuf )
/****************************************/
{
    return( doFormatSym( sym, pvbuf, FF_NULL ) );
}

char *FormatSymWithTypedefs( SYMBOL sym, VBUF *pvbuf )
/****************************************************/
{
    return( doFormatSym( sym, pvbuf, FF_TYPEDEF_STOP ) );
}

char *FormatFnDefnWithTypedefs( SYMBOL sym, VBUF *pvbuf )
/*******************************************************/
{
    return( doFormatSym( sym, pvbuf, FF_TYPEDEF_STOP | FF_ARG_NAMES ) );
}

char *FormatFnDefn( SYMBOL sym, VBUF *pvbuf )
/*******************************************/
{
    return( doFormatSym( sym, pvbuf, FF_ARG_NAMES ) );
}

char *FormatName( NAME name, VBUF *pvbuf )
/****************************************/
{
    VBUF    prefix;
    bool    ctordtor;

    VbufInit( pvbuf );
    VbufInit( &prefix );
    ctordtor = fmtSymName( NULL, name, &prefix, pvbuf, FF_NULL );
    if( !ctordtor ) {
        VbufConcVbuf( pvbuf, &prefix );
    }
    VbufFree( &prefix );
    return( strrev( VbufString( pvbuf ) ) );
}

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
#include "cgfront.h"
#include "cgback.h"
#include "errdefns.h"
#include "codegen.h"
#include "cgbackut.h"
#include "cgaux.h"
#include "cginfo.h"
#include "symdbg.h"
#include "dwarfdbg.h"
#include "vstk.h"
#include "carve.h"
#include "objmodel.h"
#include "initdefs.h"


static VSTK_CTL stack_inline_args;  // stack: symbols for inline arg.s
static VSTK_CTL stack_symbol_name;  // stack: symbol names for inline arg.s


static SYMBOL push_inline_sym(  // PUSH AN INLINE SYMBOL
    SYMBOL model )              // - model for symbol
{
    SYMBOL sym;                 // - new symbol
    SYM_TOKEN_LOCN* locn;       // - new location entry for symbol

    sym = VstkPush( &stack_inline_args );
    *sym = *model;
    if( model->locn != NULL ) {
        sym->flag2 &= ~SF2_CG_HANDLE;
        sym->locn = NULL;
        locn = SymbolLocnAlloc( &sym->locn );
        locn->tl = model->locn->tl;
    }
    return sym;
}


static SYMBOL inlineSymbol(     // CREATE TRANSLATION SYMBOL IF REQ'D
    SYMBOL sym )                // - original symbol
{
    SYMBOL old;                 // - same as "sym"
    FN_CTL* fctl;               // - current file control

    fctl = FnCtlTop();
    if( sym == fctl->this_sym ) {
        return( sym );
    }
    if( sym == fctl->cdtor_sym ) {
        return( sym );
    }
    if(  CgBackInlinedDepth() > 0
      && ! SymIsFunction( sym )
      && ! SymIsStaticData( sym ) ) {
        old = sym;
        sym = push_inline_sym( old );
        SymTransPush( old, sym );
        // code used to only have SymIsCatchAlias so we
        // make sure SymIsAlias includes SymIsCatchAlias
        DbgAssert( ! SymIsCatchAlias( sym ) ||
                   ( SymIsCatchAlias( sym ) && SymIsAlias( sym ) ) );
        if( SymIsAlias( sym ) || SymIsAnonymousMember( sym ) ) {
            if( SymIsAutomatic( sym ) ) {
                sym->u.alias = SymTrans( sym->u.alias );
            }
        }
    }
    return( sym );
}


static void declareParameter(   // DEFINE A FUNCTION PARAMETER
    SYMBOL sym )                // - the parameter
{
    cg_type cgtype;             // - type for code generator
    FN_CTL* fctl;               // - function information
    TYPE ftype;                 // - function type

    if( sym != NULL ) {
        fctl = FnCtlTop();
        ftype = FunctionDeclarationType( fctl->func->sym_type );
        sym = inlineSymbol( sym );
        if( ( TF1_PLUSPLUS & ftype->flag )
         && OMR_CLASS_REF == ObjModelArgument( sym->sym_type ) ) {
            cgtype = TY_POINTER;
        } else {
            cgtype = CgTypeOutput( sym->sym_type );
        }
        CGParmDecl( (cg_sym_handle)sym, cgtype );
        if( fctl->debug_info
         && ( GenSwitches & DBG_LOCALS ) ) {
            if( GenSwitches & DBG_DF ){
                DwarfSymDebugGenSymbol( sym, TRUE, cgtype == TY_POINTER );
            }else{
                SymbolicDebugGenSymbol( sym, TRUE, cgtype == TY_POINTER );
            }
        }
        IbpDefineSym( fctl->handle, sym );
    }
}


void CgDeclParms(               // DEFINE ARGS FOR CURRENT FN IN CORRECT ORDER
    FN_CTL *fctl,               // - current function control pointer
    SCOPE scope )               // - argument scope
{
    SYMBOL curr;
    SYMBOL stop;
    SYMBOL *psym;               // - addr[ parameter symbol ]
    TYPE fn_type;
    auto VSTK_CTL sym_stack;
    SYMBOL ret_sym;
    char* ret_name;

    fn_type = FunctionDeclarationType( fctl->func->sym_type );
    VstkOpen( &sym_stack, sizeof( SYMBOL ), 16 );
    stop = ScopeOrderedStart( scope );
    ret_name = CppSpecialName( SPECIAL_RETURN_VALUE );
    ret_sym = NULL;
    curr = NULL;
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        if( ( curr->name != NULL ) && ( ret_name == curr->name->name ) ) {
            ret_sym = curr;
        } else {
            psym = VstkPush( &sym_stack );
            *psym = curr;
        }
    }
    IbpDefineParms();
    declareParameter( fctl->this_sym );
    declareParameter( fctl->cdtor_sym );
    declareParameter( ret_sym );
    switch( PcCallImpl( fn_type ) ) {
      case CALL_IMPL_REV_CPP :
      case CALL_IMPL_REV_C :
        for(;;) {
            psym = VstkPop( &sym_stack );
            if( psym == NULL ) break;
            declareParameter( *psym );
        }
        break;
      case CALL_IMPL_CPP :
      case CALL_IMPL_C :
      case CALL_IMPL_ELL_CPP :
      case CALL_IMPL_ELL_C :
      { int index;                  // - parameter index
        int max_parms;              // - # parameters
        SYMBOL *psym;               // - addr[ parameter symbol ]
        max_parms = VstkDimension( &sym_stack );
        for( index = 0; index <= max_parms; ++index ) {
            psym = VstkIndex( &sym_stack, index );
            declareParameter( *psym );
        }
      } break;
    }
    VstkClose( &sym_stack );
    CGLastParm();
}


SYMBOL CgDeclHiddenParm(        // DECLARE HIDDEN ARG (THIS, CDTOR)
    SCOPE scope,                // - function parameters scope
    TYPE type,                  // - symbol type
    SYMBOL symbol_model,        // - model for symbol
    unsigned specname_index )   // - special name index
{
    SYMBOL_NAME name;           // - new symbol name
    SYMBOL sym;                 // - new symbol

    name = VstkPush( &stack_symbol_name );
    memset( name, 0, sizeof( *name ) );
    name->containing = scope;
    name->name = CppSpecialName( specname_index );
    sym = push_inline_sym( symbol_model );
    sym->sym_type = type;
    sym->name = name;
    return sym;
}


static void declareAuto(        // DECLARE AN AUTO SYMBOL
    SYMBOL sym )                // - symbol to be declared
{
    CGAutoDecl( (cg_sym_handle)sym, CgTypeSym( sym ) );
    if( SymRequiresDtoring( sym ) ) {
        SymScope( sym )->s.dtor_reqd = TRUE;
    }
}


// Scope has dtor_reqd set off initially.
// Turn it on if symbol requiring temp is found
//
void CgDeclSym(                 // PROCESS SYMBOL IN BLOCK-OPEN SCOPE
    SYMBOL sym )                // - current symbol
{
    FN_CTL* fctl;               // - function information
    SYMBOL orig;                // - original symbol

    orig = sym;
    sym = inlineSymbol( sym );
    fctl = FnCtlTop();
    if( CgRetnOptIsOptVar( fctl, orig ) ) {
        sym->flag |= SF_RETN_OPT;
        return;
    }
    switch( sym->id ) {
      case SC_AUTO:
      case SC_REGISTER:
        if( SymIsCatchAlias( sym ) ) {
            if( fctl->debug_info && ( GenSwitches & DBG_LOCALS ) ) {
                SYMBOL base = SymDeAlias( sym );
                switch( base->id ) {
                  case SC_AUTO:
                  case SC_REGISTER:
                    if( ! SymIsAnonymous( sym ) ) {
                        if( !SymIsTemporary( sym ) ) {
                            if( GenSwitches & DBG_DF ){
                                DwarfSymDebugGenSymbol( sym, TRUE, FALSE );
                            }else{
                                SymbolicDebugGenSymbol( sym, TRUE, FALSE );
                            }
                        }
                    }
                    break;
                }
            }
        } else {
            if( ! SymIsAnonymous( sym ) ) {
                if( SymIsTemporary( sym ) ) {
                    if( SymIsReferenced( sym ) || SymIsInitialized( sym ) ) {
                        declareAuto( sym );
                    }
                } else {
                    declareAuto( sym );
                    if( fctl->debug_info && ( GenSwitches & DBG_LOCALS ) ) {
                        if( GenSwitches & DBG_DF ){
                            DwarfDebugGenSymbol( sym, TRUE );
                        }else{
                            SymbolicDebugGenSymbol( sym, TRUE, FALSE );
                        }
                    }
                }
            }
        }
        break;
      case SC_STATIC:
        if( fctl->debug_info
         && ( GenSwitches & DBG_LOCALS ) ) {
            if( ! SymIsAnonymous( sym ) ) {
                if( SymIsReferenced( sym ) || SymIsInitialized( sym ) ) {
                    if( ! CgDeclSkippableConstObj( sym ) ) {
                        if( GenSwitches & DBG_DF ){
                            DwarfDebugGenSymbol( sym, TRUE );
                        }else{
                            SymbolicDebugGenSymbol( sym, TRUE, FALSE );
                        }
                    }
                }
            }
        }
        break;
      case SC_TYPEDEF:
        if( fctl->debug_info
         && ( GenSwitches & DBG_LOCALS ) ) {
            if( GenSwitches & (DBG_CV | DBG_DF ) ){
                DBLocalType( (cg_sym_handle)sym, FALSE );
            }
        }
        break;
    }
}


boolean CgDeclSkippableConstObj(// DETERMINE IF SKIPPABLE CONST OBJ
    SYMBOL sym )                // - symbol
{
    if( SymAddrTaken( sym ) ) {
        return( FALSE );
    }
    if( SymIsAutomatic( sym ) && ( GenSwitches & DBG_LOCALS ) ) {
        // if we are debugging locals; don't skip initialization
        return( FALSE );
    }
    if( SymIsConstantInt( sym ) ) {
        return( TRUE );
    }
    if( SymIsConstantNoCode( sym ) && ! SymIsReferenced( sym ) ) {
        if( SymIsStaticData( sym ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


static void init(               // INITIALIZATION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkOpen( &stack_inline_args, sizeof( struct symbol ), 16 );
    VstkOpen( &stack_symbol_name, sizeof( struct symbol_name ), 16 );
}


static void fini(               // COMPLETION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &stack_inline_args );
    VstkClose( &stack_symbol_name );
}


INITDEFN( cg_decl, init, fini )

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

#include "compcfg.h"

#include "cgfront.h"
#include "codegen.h"
#include "rtfuns.h"
#include "label.h"
#include "fnbody.h"
#include "cgsegid.h"
#include "vstk.h"
#include "name.h"
#include "pcheader.h"


static SYMBOL module_init_func; // function to perform module-initialization
static SCOPE module_init_scope; // block scope for module-initialization
static FUNCTION_DATA module_fd; // module function information


SYMBOL StaticInitSymbol(        // CREATE STATIC INITIALIZATION SYMBOL
    SYMBOL sym )                // - symbol type
{
    TYPE type;                  // - type for symbol
    type_flag flags;            // - modifier flags

    type = sym->sym_type;
    TypeModFlags( type, &flags );
    if(( flags & TF1_CONST ) == 0 ) {
        /* make it a constant symbol */
        type = MakeModifiedType( type, TF1_CONST );
    }
    return SymCreateTempScope( type
                             , SC_STATIC
                             , SF_REFERENCED
                             , CppStaticInitName( sym ) );
}


static SYMBOL staticInitFuncVar(// DEFINE VARIABLE FOR INITIALIZATION FLAGS
    void )
{
    return SymCreateTempScope( MakeExpandableType( TYP_UCHAR )
                             , SC_STATIC
                             , SF_REFERENCED
                             , CppStaticOnceOnlyName() );
}


static void moduleInitVar(      // GENERATE REFERENCE TO MODULE-INIT VAR.
    SYMBOL var,                 // - variable
    bool base_type )            // - base_type or pointer to it
{
    unsigned offset;            // - offset to be tested
    TYPE type;                  // - type for flags variable

    type = var->sym_type;
    offset = type->u.a.array_size - 1;
    if( offset != 0 ) {
        CgSetType( GetBasicType( TYP_UINT ) );
        CgFrontCodeUint( IC_LEAF_CONST_INT, offset );
        CgFrontSymbol( var );
        CgSetType( MakePointerTo( type->of ) );
        CgFrontCodeUint( IC_OPR_BINARY, CO_PLUS );
    } else {
        CgFrontSymbol( var );
    }
    if( base_type ) {
        CgSetType( type->of );
    } else {
        CgSetType( MakePointerTo( type->of ) );
    }
}


unsigned StaticInitFuncBeg(     // START INITIALIZATION OF STATIC IN FUNCTION
    void )
{
    CGLABEL init_label;         // - label #
    INIT_VAR *init_var;

    CgFrontStatInit();
    init_var = FunctionBodyGetInit( NULL );
    if( init_var->var == NULL ) {
        init_var->var = staticInitFuncVar();
    } else {
        init_var->mask <<= 1;
        if( init_var->mask >= 0x100 ) {
            ++init_var->var->sym_type->u.a.array_size;
            init_var->mask = 1;
        }
    }
    if( CompFlags.bm_switch_used ) {
        SYMBOL rtf;
        rtf = RunTimeCallSymbol( RTF_STATIC_INIT );
        CgFrontSymbol( rtf );
        CgSetType( GetBasicType( TYP_SINT ) );
        CgFrontCodePtr( IC_CALL_SETUP, rtf );
        CgSetType( GetBasicType( TYP_SINT ) );
        CgFrontCodeUint( IC_LEAF_CONST_INT, init_var->mask );
        CgFrontCode( IC_CALL_PARM );
        moduleInitVar( init_var->var, false );
        CgFrontCode( IC_CALL_PARM );
        CgFrontCode( IC_CALL_EXEC );
    } else {
        moduleInitVar( init_var->var, true );
        CgFrontCodeUint( IC_OPR_UNARY, CO_FETCH );
        CgFrontCodeUint( IC_LEAF_CONST_INT, init_var->mask );
        CgFrontCodeUint( IC_OPR_BINARY, CO_AND );
    }
    CgFrontCodeUint( IC_LEAF_CONST_INT, 0 );
    CgFrontCodeUint( IC_OPR_BINARY, CO_NE );
    init_label = CgFrontLabelCs();
    CgFrontGotoNear( IC_LABEL_CS, O_IF_TRUE, init_label );
    if( !CompFlags.bm_switch_used ) {
        moduleInitVar( init_var->var, true );
        CgFrontCodeUint( IC_LEAF_CONST_INT, init_var->mask );
        CgFrontCodeUint( IC_OPR_BINARY, CO_OR_EQUAL );
        CgFrontCode( IC_EXPR_DONE );
    }
    return( init_label );
}


void StaticInitFuncEnd(         // END INITIALIZATION OF STATIC IN FUNCTION
    CGLABEL init_label )        // - label #
{
    CgFrontLabdefCs( init_label );
    CgFrontLabfreeCs( 1 );
}


static SCOPE moduleInitSave(     // SAVE ENVIRONMENT BEFORE MOD-INIT.
    void )
{
    SCOPE save_scope;

    save_scope = GetCurrScope();
    LabelSwitchFunc( &module_fd.label_mem );
    return( save_scope );
}


static void moduleInitRestore(  // RESTORE ENVIRONMENT AFTER MOD-INIT.
    SCOPE scope )               // - scope to restore to
{
    SetCurrScope(scope);
    LabelSwitchFunc( &module_fd.label_mem );
}

SCOPE ModuleFnScope(            // SCOPE MOD-INIT FN IS DEF'D IN
    void )
{
    return GetFileScope();
}


void ModuleInitInit(            // START MODULE-INITIALIZATION FUNCTION
    void )
{
    SYMBOL module_init;         // - SYMBOL for mod-init. function
    SCOPE curr_scope;           // - current scope
    TYPE fn_type;               // - type for init function

    curr_scope = moduleInitSave();
    fn_type = TypeVoidFunOfVoid();
    module_init = SymCreateFileScope( fn_type
                                    , SC_STATIC
                                    , 0
                                    , CppSpecialName( SPECIAL_INIT_FUNCTION ) );
    module_init_func = module_init;
    SetCurrScope(GetFileScope());
    ScopeBeginFunction( module_init );
    FunctionBodyStartup( module_init, &module_fd, FUNC_NULL );
    module_fd.retn_opt = false;
    module_init_scope = GetCurrScope();
    ScopeKeep( module_init_scope );
    moduleInitRestore( curr_scope );
}

void ModuleInitUsed(            // FLAG MODULE-INITIALIZATION FUNCTION AS USED
    void )
{
    module_init_func->id = SC_STATIC;
    module_init_func->flag |= SF_INITIALIZED | SF_REFERENCED;
}


SCOPE ModuleInitResume(         // RESUME MODULE-INITIALIZATION FUNCTION
    void )
{
    SCOPE curr_scope;

    curr_scope = moduleInitSave();
    SetCurrScope(module_init_scope);
    return( curr_scope );
}


void ModuleInitResumeScoped(    // RESUME SCOPED MODULE-INITIALIZATION FUNC.
    SCOPE scope )               // - scope before function
{
    ScopeMemberModuleFunction( scope, module_init_scope );
}


void ModuleInitRestore(         // UNDO MAKING MODULE-INIT FUNCTION A MEMBER FUNCTION
    SCOPE restore_scope )       // - value returned by ModuleInitResume
{
    ScopeRestoreModuleFunction( module_init_scope );
    moduleInitRestore( restore_scope );
}


void ModuleInitFini(            // COMPLETE MODULE-INITIALIZATION FUNCTION
    void )
{
    SCOPE save_scope;

    CgFrontSwitchFile( module_init_func );
    save_scope = ModuleInitResume();
    FunctionBodyShutdown( module_init_func, &module_fd );
    ScopeEnd( SCOPE_FUNCTION );
    ModuleInitRestore( save_scope );
    CgFrontModInitFini();
}


static void genInitFiniReference( // GENERATE INIT/FINI REFERENCE TO FUNCTION
    SYMBOL func,                // - function to be called
    unsigned priority,          // - priority
    NAME name,                  // - name for reference
    fe_seg_id tgt_seg )         // - segment # of target segment
{
    SYMBOL init_ref;            // - reference to mod-init. function
    TYPE type;                  // - used to build type

    SegmentMarkUsed( tgt_seg );
    type = MakePointerTo( func->sym_type );
    init_ref = SymCreateFileScope( type
                                 , SC_STATIC
                                 , SF_INITIALIZED | SF_REFERENCED
                                 , name );
    init_ref->segid = tgt_seg;
    if( tgt_seg == SEG_INIT_REF ) {
        CgFrontInitRef();
    } else {
        CgFrontFiniRef();
    }
    CgFrontDataPtr( IC_DATA_LABEL, init_ref );
    #if _INTEL_CPU
        CgFrontDataPtr( IC_SET_TYPE, GetBasicType( TYP_UCHAR ) );
        if( IsBigCode() ) {
            CgFrontDataInt( IC_DATA_INT, 1 );
        } else {
            CgFrontDataInt( IC_DATA_INT, 0 );
        }
        CgFrontDataInt( IC_DATA_INT, priority );
    #elif _CPU == _AXP
        CgFrontDataPtr( IC_SET_TYPE, GetBasicType( TYP_UINT ) );
        CgFrontDataInt( IC_DATA_INT, 0 );
        CgFrontDataInt( IC_DATA_INT, priority );
    #else
        #error BAD _CPU
    #endif
    CgFrontDataPtr( IC_SET_TYPE, type );
    CgFrontDataInt( IC_DATA_PTR_OFFSET, 0 );
    CgFrontDataPtr( IC_DATA_PTR_SYM, func );
    #if _CPU == 8086
        if( !IsBigCode() ) {
            CgFrontDataInt( IC_DATA_INT, 0 );
        }
    #elif COMP_CFG_COFF == 1
        CgFrontDataPtr( IC_SET_TYPE, GetBasicType( TYP_USHORT ) );
        CgFrontDataInt( IC_DATA_INT, 0 );
    #endif
}


void ModuleInitConnect(         // CODE GENERATION FOR MODULE-INIT. CONNECTION
    void )
{
    genInitFiniReference( module_init_func
                        , CompInfo.init_priority
                        , CppSpecialName( SPECIAL_MODULE_INIT )
                        , SEG_INIT_REF );
}


SYMBOL ModuleInitFuncSym(       // GET SYMBOL FOR MODULE-INITIALIZATION FUNC.
    void )
{
    return( module_init_func );
}


SCOPE ModuleInitScope(          // GET BLOCK SCOPE FOR MODULE INITIALIZATION
    void )
{
    return( module_init_scope );
}

pch_status PCHWriteModuleData( void )
{
    uint_8 has_state_table;

    SymbolPCHWrite( module_init_func );
    ScopePCHWrite( module_init_scope );
    ScopePCHWrite( module_fd.fn_scope );
    has_state_table = module_fd.has_state_tab;
    PCHWriteVar( has_state_table );
    LabelPCHWrite( &module_fd.label_mem );
    return( PCHCB_OK );
}

pch_status PCHReadModuleData( void )
{
    uint_8 has_state_table;

    module_init_func = SymbolPCHRead();
    module_init_scope = ScopePCHRead();
    module_fd.fn_scope = ScopePCHRead();
    PCHReadVar( has_state_table );
    module_fd.has_state_tab = ( has_state_table != 0 );
    LabelPCHRead( &module_fd.label_mem );
    return( PCHCB_OK );
}

pch_status PCHInitModuleData( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniModuleData( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

bool ModuleIsZap1( CGFILE_INS *p )
{
    if( p->block != module_fd.init_state_tab.block ) {
        return( false );
    }
    if( p->offset != module_fd.init_state_tab.offset ) {
        return( false );
    }
    return( true );
}

void *ModuleIsZap2( CGFILE_INS *p )
{
    return( LabelBlockOpenFindZap( &module_fd.label_mem, p ) );
}

void ModuleAdjustZap1( CGFILE *cgf )
{
    module_fd.init_state_tab = CgioLastWrite( cgf );
}

void ModuleAdjustZap2( CGFILE *cgf, void *h )
{
    CGFILE_INS *dest;

    dest = LabelBlockOpenAdjustZap( &module_fd.label_mem, h );
    if( dest != NULL ) {
        *dest = CgioLastWrite( cgf );
    }
}

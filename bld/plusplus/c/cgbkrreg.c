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
#include "memmgr.h"
#include "codegen.h"
#include "cgbackut.h"
#include "initdefs.h"
#include "rtfuns.h"
#include "name.h"

#if _CPU != _AXP

typedef struct {                // OPT_DEFN -- optimization definition
    SYMBOL sym;                 // - symbol for definition
    char name[1];               // - name
} OPT_DEFN;


static struct {
    SYMBOL sym; 
    char name[];
} optFuncReg = {   // OPT_DEFN for function registration
     NULL, "_wint_thread_data"
};


static NAME optName(            // NAME OF OPTIMIZATION SYMBOL
    const OPT_DEFN *odef )      // - optimization definition
{
    return( NameCreateNoLen( odef->name ) );
}


static void defineOptSym(       // DEFINE SYMBOL FOR THREAD_CTL
    OPT_DEFN *odef )            // - optimization definition
{
    SYMBOL var;                 // - new variable

    var = SymCreateFileScope( TypePtrToVoid()
                            , SC_EXTERN
                            , SF_REFERENCED
                            , optName( odef ) );
    odef->sym = var;
    LinkageSet( var, "C" );
}


static SYMBOL lookupOptSym(     // LOOK UP OPTIMIZATION SYMBOL
    OPT_DEFN *odef )            // - optimization definition
{
    SEARCH_RESULT* result;      // - lookup result
    SYMBOL var;                 // - name defined in compiled code

    result = ScopeFindNaked( GetFileScope(), optName( odef ) );
    if( result == NULL ) {
        var = NULL;
    } else {
        var = result->sym_name->name_syms;
        odef->sym = var;
        ScopeFreeResult( result );
    }
    return var;
}


static cg_name pointOptSym(     // POINT AT OPTIMIZED SYMBOL, IF OPTIMIZABLE
    OPT_DEFN *odef )            // - optimization definition
{
    cg_name expr;               // - optimized expression

    if( CompFlags.target_multi_thread ) {
        expr = NULL;
    } else {
        if( odef->sym == NULL ) {
            lookupOptSym( odef );
            if( odef->sym == NULL ) {
                defineOptSym( odef );
            }
        }
        expr = CgSymbol( odef->sym );
    }
    return expr;
}


static void rtRegister(         // REGISTER A FUNCTION BY R/T CALL
    SYMBOL rw,                  // - symbol for R/W block
    SYMBOL ro )                 // - symbol for R/O block
{
    RT_DEF def;                 // - R/T call control

    CgRtCallInit( &def, RTF_REG_FUN );
    CgRtParamAddrSym( &def, ro );
    CgRtParamAddrSym( &def, rw );
    CgRtCallExecDone( &def );
}


static SYMBOL registerHandler(  // REGISTER THE HANDLER
    SYMBOL rw,                  // - symbol for R/W block
    unsigned rtn_code )         // - R/T handler code
{
#if 0
    CgAssignPtr( CgSymbolPlusOffset( rw, CgbkInfo.size_data_ptr )
               , CgAddrSymbol( RunTimeCallSymbol( rtn_code ) ) );
#else
    CgAssign( CgSymbolPlusOffset( rw, CgbkInfo.size_data_ptr )
            , CgAddrSymbol( RunTimeCallSymbol( rtn_code ) )
            , TY_CODE_PTR );
#endif
    return rw;
}


void CgFunDeregister(           // DE-REGISTER A FUNCTION
    SYMBOL rw )                 // - symbol for R/W block
{
    RT_DEF def;                 // - R/T call control
    cg_name opt_thr;            // - optimized expression for addr[THREAD_CTL]

    DbgVerify( CompFlags.rw_registration, "not R/W Registration" );
    if( CompFlags.fs_registration ) {
        cg_name expr;
        CgRtCallInit( &def, RTF_FS_POP );
        expr = CgSymbolPlusOffset( rw, 0 );
        expr = CgFetchPtr( expr );
        CgRtParam( expr, &def, TY_POINTER );
        CgRtCallExecDone( &def );
    } else {
        opt_thr = pointOptSym( (OPT_DEFN *)&optFuncReg );
        if( opt_thr == NULL ) {
            CgRtCallExecNoArgs( RTF_DEREGISTER );
        } else {
            CgAssignPtr( opt_thr, CgFetchPtr( CgSymbolPlusOffset( rw, 0 ) ) );
        }
    }
}

#endif


void CgFunRegister(             // REGISTER A FUNCTION
    FN_CTL* fctl,               // - function information
    SYMBOL rw,                  // - symbol for R/W block
    SYMBOL ro )                 // - symbol for R/O block
{
    SE* se;                     // - current position

#if _CPU == _AXP
    CgAssignPtr( CgSymbolPlusOffset( rw, 0 ), CgAddrSymbol( ro ) );
    CompFlags.inline_fun_reg = TRUE;
#else
    RT_DEF def;                 // - R/T call control
    cg_name opt_thr;            // - optimized expression for addr[THREAD_CTL]

    DbgVerify( CompFlags.rw_registration, "not R/W Registration" );
    if( CompFlags.fs_registration ) {
        CompFlags.inline_fun_reg = TRUE;
        CgRtCallInit( &def, RTF_FS_PUSH );
        CgRtParamAddrSym( &def, rw );
        CgRtCallExecDone( &def );
        rw = registerHandler( rw, RTF_FS_HANDLER );
    } else {
        opt_thr = pointOptSym( (OPT_DEFN *)&optFuncReg );
        if( opt_thr == NULL ) {
            rtRegister( rw, ro );
        } else {
            CompFlags.inline_fun_reg = TRUE;
            CgAssignPtr( CgSymbolPlusOffset( rw, 0 )
                       , CgFetchPtr( opt_thr ) );
            CgAssignPtr( CgSymbol( optFuncReg.sym )
                       , CgAddrSymbol( rw ) );
            rw = registerHandler( rw, RTF_FS_HANDLER_RTN );
        }
    }
    CgAssignPtr( CgSymbolPlusOffset( rw, CgbkInfo.size_data_ptr + CgbkInfo.size_fs_hand )
               , CgAddrSymbol( ro ) );
#endif
    if( fctl->is_dtor ) {
        se = BlkPosnCurr();
    } else {
        se = NULL;
    }
    FstabAssignStateVar( se );
}


void CgCtorTestTempsRegister(   // REGISTER DTORING TEMPS FOR CTOR
    FN_CTL* fctl )              // - function control
{
    CondInfoSetCtorTest( fctl, TRUE );
}


static void init(               // MODULE INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
#if _CPU != _AXP
    optFuncReg.sym = NULL;
#endif
}

INITDEFN( fun_registration, init, InitFiniStub )

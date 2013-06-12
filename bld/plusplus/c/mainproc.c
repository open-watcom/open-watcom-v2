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
#include "specfuns.h"
#include "decl.h"


boolean MainProcedure(          // DETECT IF FUNCTION SYMBOL IS "main" procedure
    SYMBOL sym )                // - symbol
{
    return SPFN_MAIN == SpecialFunction( sym );
}


// SETUP FOR "main", "WinMain", "LibMain", "DllMain"
void MainProcSetup(             // SETUP FOR MAIN PROC
    SYMBOL sym )                // - symbol
{
    TYPE fn_type;               // - function type for main procedure
    arg_list *args;             // - arg. list for main procedure

    if( ScopeIsGlobalNameSpace( SymScope( sym ) ) != NULL ) {
        if( CompFlags.dump_prototypes ) {
            DefAddPrototype( sym );
        }
        switch( SpecialFunction( sym ) ) {
          case SPFN_wMAIN :
            CompFlags.has_wide_char_main = TRUE;
            /* fall through */
          case SPFN_MAIN :
            CompFlags.has_main = TRUE;
            fn_type = FunctionDeclarationType( sym->sym_type );
            args = TypeArgList( fn_type );
            if( args->num_args != 0 ) {
                CompFlags.main_has_parms = TRUE;
            }
            if( fn_type->u.f.pragma != NULL ) {
                // quietly remove stray __cdecl/__stdcall modifiers
                sym->sym_type = RemoveFunctionPragma( sym->sym_type );
            }
            DeclVerifyNoOtherCLinkage( sym, sym );
            LinkageSet( sym, "C" );
            break;
          case SPFN_wWINMAIN :
            CompFlags.has_wide_char_main = TRUE;
            /* fall through */
          case SPFN_WINMAIN :
            CompFlags.has_winmain = TRUE;
            DeclVerifyNoOtherCLinkage( sym, sym );
            LinkageSet( sym, "C" );
            break;
          case SPFN_wLIBMAIN :
            CompFlags.has_wide_char_main = TRUE;
            /* fall through */
          case SPFN_LIBMAIN :
            CompFlags.has_libmain = TRUE;
            DeclVerifyNoOtherCLinkage( sym, sym );
            LinkageSet( sym, "C" );
            break;
          case SPFN_wDLLMAIN :
            CompFlags.has_wide_char_main = TRUE;
            /* fall through */
          case SPFN_DLLMAIN :
            CompFlags.has_dllmain = TRUE;
            DeclVerifyNoOtherCLinkage( sym, sym );
            LinkageSet( sym, "C" );
            break;
          default :
            if( LinkageIsC( sym ) ) {
                CompFlags.extern_C_defn_found = 1;
            }
            break;
        }
    }
}

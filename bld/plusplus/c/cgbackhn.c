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
#include "cgback.h"
#include "codegen.h"
#include "rtfuns.h"
#include "pstk.h"
#include "feprotos.h"

#ifndef NDEBUG
#   include "pragdefn.h"
#endif


static PSTK_CTL nonFileScopeSyms;


void CgBackStatHandlesInit(     // INITIALIZE FOR STATIC HANDLES
    void )
{
    PstkOpen( &nonFileScopeSyms );
}


void CgBackStatHandlesFini(     // INITIALIZE FOR STATIC HANDLES
    void )
{
    PstkClose( &nonFileScopeSyms );
}


static void cgBackStatHandleAdd(// ADD SYMBOL TO STATIC HANDLES
    SYMBOL sym )                // - symbol to be added
{
    PstkPush( &nonFileScopeSyms, sym );
}


static void cgBackStatHandlesFree(     // FREE STATIC HANDLES
    void )
{
    SYMBOL top;                 // - addr( symbol to be freed )

    for(;;) {
        top = PstkPopElement( &nonFileScopeSyms );
        if( top == NULL ) break;
        CgBackFreeHandle( top );
    }
}


back_handle FEBack(             // GET BACK HANDLE FOR A SYMBOL
    SYMBOL sym )                // - the symbol
{
    SCOPE scope;                // - scope for symbol
    SYMBOL check_sym;           // - SYMBOL temp
    back_handle cg_handle;      // - handle for symbol
#ifndef NDEBUG
    SYMBOL orig                 // - original symbol
        = sym;
#endif

    check_sym = SymIsAnonymous( sym );
    if( check_sym != NULL ) {
        sym = check_sym;
    }
    if( sym->flag2 & SF2_CG_HANDLE ) {
        cg_handle = sym->locn->u.cg_handle;
    } else {
        cg_handle = BENewBack( (cg_sym_handle)sym );
        if( NULL != cg_handle ) {
            SYM_TOKEN_LOCN* locn = SymbolLocnAlloc( &sym->locn );
            locn->u.cg_handle = cg_handle;
            scope = SymScope( sym );
            if( scope != NULL ) {
                switch( scope->id ) {
                  case SCOPE_CLASS :
                    cgBackStatHandleAdd( sym );
                    break;
                  case SCOPE_FUNCTION :
                  case SCOPE_BLOCK :
                    if( SymIsStaticData( sym ) ) {
                        cgBackStatHandleAdd( sym );
                    }
                    break;
                }
            }
            sym->flag2 &= ~SF2_HDL_MASK;
            sym->flag2 |= SF2_CG_HANDLE;
        }
    }
#ifndef NDEBUG
    if( PragDbgToggle.auxinfo ) {
        printf( "FEBack( %p ) -> bh[%p]\n", orig, cg_handle );
    }
#endif
    return cg_handle;
}


void CgBackFreeHandle(          // FREE A BACK HANDLE FOR A SYMBOL
    SYMBOL sym )                // - the symbol
{
    if( ( sym->flag2 & SF2_CG_HANDLE )
      &&( ! SymIsAnonymous( sym ) )
      &&( ! SymIsAutomatic( sym ) ) ) {     // - only because of RO,RW-DTORS
        sym->flag2 &= ~SF2_CG_HANDLE;
        BEFreeBack( sym->locn->u.cg_handle );
    }
}


void CgBackFreeFileHandles(     // FREE HANDLES FOR FILE SCOPE
    void )
{
    ScopeWalkSymbols( GetFileScope(), &CgBackFreeHandle );
    cgBackStatHandlesFree();
}

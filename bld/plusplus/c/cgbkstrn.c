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

#include <float.h>

#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "vstk.h"
#include "initdefs.h"

typedef struct                  // SYM_TRANS -- symbol translation
{   SYMBOL src;                 // - source SYMBOL
    SYMBOL tgt;                 // - target SYMBOL
    unsigned id;                // - id for translation block
} SYM_TRANS;

#ifndef NDEBUG

    #include "toggle.h"
    #include <stdio.h>
    #include "pragdefn.h"

    void dump( const char* msg, SYM_TRANS* tr )
    {
        if( PragDbgToggle.dump_exec_ic ) {
            printf( "SYM_TRANS( %s ) src(%p) tgt(%p) id(%x)\n"
                  , msg
                  , tr->src
                  , tr->tgt
                  , tr->id );
        }
    }

#else

    #define dump( a, b )

#endif



static VSTK_CTL stack_sym_trans;    // stack: symbol translation
static unsigned sym_trans_id;       // identifies sym. translation block


void* SymTrans(                 // TRANSLATE SYMBOL/SCOPE
    void *src )                 // - source value
{
    SYM_TRANS *tr;              // - translation element (current)
    unsigned bound;             // - bounding id

    bound = FnCtlTop()->sym_trans_id;
    for( tr = VstkTop( &stack_sym_trans )
       ; tr != NULL && bound <= tr->id
       ; tr = VstkNext( &stack_sym_trans, tr ) ) {
        if( src == tr->src ) {
            src = tr->tgt;
            dump( "used", tr );
            break;
        }
    }
    return( src );
}


void SymTransPush(              // ADD A SYMBOL TO BE TRANSLATED
    SYMBOL src,                 // - source SYMBOL
    SYMBOL tgt )                // - target SYMBOL
{
    SYM_TRANS *tr;              // - translation element

    tr = VstkPush( &stack_sym_trans );
    tr->src = src;
    tr->tgt = tgt;
    tr->id  = sym_trans_id;
    dump( "defined", tr );
}


#ifndef NDEBUG
void SymTransEmpty(             // DEBUG: VERIFY SYMBOL TRANSLATIONS OVER
    void )
{
    DbgVerify( NULL == VstkTop( &stack_sym_trans )
             , "SymTransEmpty -- stack not empty" );
    DbgVerify( 0 == sym_trans_id
             , "SymTransEmpty -- id != 0" );
}
#endif


void SymTransNewBlock(          // START NEW BLOCK OF TRANSLATIONS
    void )
{
    ++ sym_trans_id;
}


void SymTransFuncBeg(           // START NEW FUNCTION TRANSLATION
    FN_CTL* fctl )              // - function control
{
    fctl->sym_trans_id = sym_trans_id;
}


void SymTransFuncEnd(           // COMPLETE FUNCTION TRANSLATION
    FN_CTL* fctl )              // - function control
{
    SYM_TRANS* top;             // - top translation

    sym_trans_id = fctl->sym_trans_id;
    for( ; ; VstkPop( &stack_sym_trans ) ) {
        top = VstkTop( &stack_sym_trans );
        if( top == NULL ) break;
        if( top->id < sym_trans_id ) break;
        dump( "freed", top );
    }
}


static void symTransInit(       // INITIALIZE CGBKSTRN
    INITFINI* defn )            // - definition
{
    defn = defn;
    sym_trans_id = 0;
    VstkOpen( &stack_sym_trans, sizeof( SYM_TRANS ), 16 );
}


static void symTransFini(       // COMPLETE CGBKSTRN
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &stack_sym_trans );
}


INITDEFN( sym_trans, symTransInit, symTransFini )

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
#include "carve.h"
#include "ring.h"
#include "class.h"
#include "initdefs.h"
#include "pcheader.h"
#include "rtngen.h"
#include "context.h"
#include "template.h"

typedef struct routine_gen RTN_GEN;
struct routine_gen {
    RTN_GEN             *next;
    unsigned            index;
    void                *parm;
};

#define BLOCK_RTN_GEN           16
static carve_t carveRTN_GEN;

// scheduled routines with their parm types
static RTN_GEN *useSYMBOL;
static RTN_GEN *useTYPE;

static void (*execSYMBOL[])( SYMBOL ) = {
#define RTN_GEN_SYMBOL( name )  RtnGenCallBack##name,
#include "rtnglist.h"
};
static void (*execTYPE[])( TYPE ) = {
#define RTN_GEN_TYPE( name )    RtnGenCallBack##name,
#include "rtnglist.h"
};

static RTN_GEN *findGenRoutine( RTN_GEN **head, unsigned index, void *parm )
{
    RTN_GEN *r;

#ifndef NDEBUG
    if( parm == NULL ) {
        CFatal( "rtn gen parm must be non-NULL" );
    }
#endif
    for( r = *head; r != NULL; r = r->next ) {
        if( index == r->index && parm == r->parm ) {
            return( r );
        }
    }
    return( NULL );
}

static void addGenRoutine( RTN_GEN **head, unsigned index, void *parm )
{
    RTN_GEN *r;

    r = StackCarveAlloc( carveRTN_GEN, head );
    r->index = index;
    r->parm = parm;
}

void RtnGenAddSymbol( RTNGEN_SYMBOL index, SYMBOL parm )
/******************************************************/
{
    RTN_GEN *r;

    r = findGenRoutine( &useSYMBOL, index, parm );
    if( r == NULL ) {
        addGenRoutine( &useSYMBOL, index, parm );
    }
}

void RtnGenAddType( RTNGEN_TYPE index, TYPE parm )
/************************************************/
{
    RTN_GEN *r;

    r = findGenRoutine( &useTYPE, index, parm );
    if( r == NULL ) {
        addGenRoutine( &useTYPE, index, parm );
    }
}

void RtnGenerate( void )
/**********************/
{
    RTN_GEN *c;
    boolean keep_going = TRUE;

    SetCurrScope( GetFileScope() );
    while( keep_going ) {
        keep_going = FALSE;

        CtxSetContext( CTX_FUNC_GEN );
        keep_going = ClassDefineRefdDefaults();

        ScopeEmitIndexMappings();

        while( useSYMBOL != NULL ) {
            c = useSYMBOL;
            useSYMBOL = c->next;
            keep_going = TRUE;
            (*execSYMBOL[ c->index ])( c->parm );
            CarveFree( carveRTN_GEN, c );
        }
        while( useTYPE != NULL ) {
            c = useTYPE;
            useTYPE = c->next;
            keep_going = TRUE;
            (*execTYPE[ c->index ])( c->parm );
            CarveFree( carveRTN_GEN, c );
        }
        CtxSetContext( CTX_SOURCE );

        keep_going = TemplateProcessInstantiations() || keep_going;
        SetCurrScope( GetFileScope() );
    }

    TemplateFreeDefns();
}


static void rtnGenInit(         // INITIALIZE FOR ROUTINE GENERATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    useSYMBOL = NULL;
    useTYPE = NULL;
    carveRTN_GEN = CarveCreate( sizeof( RTN_GEN ), BLOCK_RTN_GEN );
}


static void rtnGenFini(         // COMPLETE ROUTINE GENERATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveRTN_GEN );
}


INITDEFN( rtn_gen, rtnGenInit, rtnGenFini )

pch_status PCHReadGenerators( void )
{
    SYMBOL read_symbol;
    TYPE read_type;

    carveRTN_GEN = CarveRestart( carveRTN_GEN );
    useSYMBOL = NULL;
    useTYPE = NULL;
    for( ; (read_symbol = SymbolMapIndex( (SYMBOL)(pointer_int)PCHReadCVIndex() )) != NULL; ) {
        addGenRoutine( &useSYMBOL, PCHReadUInt(), read_symbol );
    }
    for( ; (read_type = TypeMapIndex( (TYPE)(pointer_int)PCHReadCVIndex() )) != NULL; ) {
        addGenRoutine( &useTYPE, PCHReadUInt(), read_type );
    }
    return( PCHCB_OK );
}

pch_status PCHWriteGenerators( void )
{
    RTN_GEN *c;

    for( c = useSYMBOL; c != NULL; c = c->next ) {
        PCHWriteCVIndex( (cv_index)(pointer_int)SymbolGetIndex( c->parm ) );
        PCHWriteUInt( c->index );
    }
    PCHWriteCVIndexTerm();
    for( c = useTYPE; c != NULL; c = c->next ) {
        PCHWriteCVIndex( (cv_index)(pointer_int)TypeGetIndex( c->parm ) );
        PCHWriteUInt( c->index );
    }
    PCHWriteCVIndexTerm();
    return( PCHCB_OK );
}

pch_status PCHInitGenerators( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniGenerators( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

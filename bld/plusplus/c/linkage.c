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

#include <stddef.h>

#include "memmgr.h"
#include "carve.h"
#include "linkage.h"
#include "pragdefn.h"
#include "initdefs.h"

typedef struct linkage_name {
    LINKAGE             next;
    AUX_INFO            *pragma;
    char                name[1];
} linkage_name;

typedef struct linkage_stack *LINKAGE_STACK;

struct linkage_stack {
    LINKAGE_STACK       prev;
    LINKAGE             linkage;
    unsigned            block : 1;
};

static LINKAGE validLinkages;
static LINKAGE_STACK nestedLinkages;
static LINKAGE_STACK resetLinkages;

#define BLOCK_STACK             4
static carve_t carveSTACK;

LINKAGE CurrLinkage;
LINKAGE CppLinkage;
LINKAGE CLinkage;

static LINKAGE findLinkage( char *name )
{
    LINKAGE p;

    for( p = validLinkages; p != NULL; p = p->next ) {
        if( strcmp( name, p->name ) == 0 ) {
            return( p );
        }
    }
    return( NULL );
}

LINKAGE LinkageAdd( char *name, AUX_INFO *defn )
/**********************************************/
{
    LINKAGE p;
    size_t len;

    p = findLinkage( name );
    if( p != NULL ) {
        p->pragma = defn;
        return( p );
    }
    len = strlen( name );
    p = CPermAlloc( offsetof( linkage_name, name ) + len + 1 );
    memcpy( p->name, name, len + 1 );
    p->pragma = defn;
    p->next = validLinkages;
    validLinkages = p;
    return( p );
}

static LINKAGE checkLinkage( char *name )
{
    LINKAGE p;

    p = findLinkage( name );
    if( p != NULL ) {
        return( p );
    }
    CErr2p( ERR_UNKNOWN_LINKAGE, name );
    return( CppLinkage );
}

void LinkagePushC( void )
/***********************/
{
    LinkagePush( "C" );
}

void LinkagePushCpp( void )
/*************************/
{
    LinkagePush( "C++" );
}

void LinkagePush( char *name )
/****************************/
{
    LINKAGE next_linkage;
    LINKAGE_STACK top;

    next_linkage = checkLinkage( name );
    CurrLinkage = next_linkage;
    top = CarveAlloc( carveSTACK );
    top->prev = nestedLinkages;
    top->linkage = next_linkage;
    top->block = false;
    nestedLinkages = top;
}

void LinkageBlock( void )
/***********************/
{
    if( nestedLinkages != NULL ) {
        nestedLinkages->block = true;
    }
}

void LinkagePop( void )
/*********************/
{
    LINKAGE_STACK top;

    if( nestedLinkages != NULL ) {
        top = nestedLinkages;
        nestedLinkages = top->prev;
        if( nestedLinkages != NULL ) {
            CurrLinkage = nestedLinkages->linkage;
        } else {
            CurrLinkage = NULL;
        }
        CarveFree( carveSTACK, top );
    }
}

LINKAGE LinkageMergePop( void )
/*****************************/
{
    LINKAGE top_linkage;

    /* extern "C" extern "C++" ==> extern "C++" */
#ifndef NDEBUG
    // both linkages must be non-block linkage scopes
    if(!(( nestedLinkages != NULL && nestedLinkages->block == 0 ) &&
         ( nestedLinkages->prev != NULL && nestedLinkages->prev->block == 0 ))) {
        CFatal( "invalid linkage merge" );
    }
#endif
    top_linkage = CurrLinkage;
    LinkagePop();
    nestedLinkages->linkage = top_linkage;
    CurrLinkage = top_linkage;
    return( top_linkage );
}

void LinkageReset( void )
/***********************/
{
    while( nestedLinkages != NULL && nestedLinkages != resetLinkages ) {
        LinkagePop();
    }
}

static void linkageInit( INITFINI* defn )
{
    defn = defn;
    validLinkages = NULL;
    nestedLinkages = NULL;
    carveSTACK = CarveCreate( sizeof( struct linkage_stack ), BLOCK_STACK );

    CppLinkage = LinkageAdd( "C++", &DefaultInfo );
    CLinkage = LinkageAdd( "C",   &DefaultInfo );

    LinkagePush( "C++" );
    resetLinkages = nestedLinkages;
    LinkageBlock();
}

static void linkageFini( INITFINI* defn )
{
    defn = defn;
    while( nestedLinkages != NULL ) {
        LinkagePop();
    }
    DbgStmt( CarveVerifyAllGone( carveSTACK, "linkage STACK" ) );
    CarveDestroy( carveSTACK );
}

INITDEFN( linkage, linkageInit, linkageFini )

#ifndef NDEBUG
static void verifyFileScopeSym( SYMBOL sym )
{
    if( ! ScopeType( SymScope( sym ), SCOPE_FILE ) ) {
        CFatal( "linkage symbol is not from file-scope" );
    }
}
#else
#define verifyFileScopeSym( s )
#endif

void LinkageSet( SYMBOL sym, char *language )
/*******************************************/
{
    LINKAGE linkage;

    verifyFileScopeSym( sym );
    switch( sym->id ) {
    case SC_DEFAULT:
        return;
    case SC_FUNCTION_TEMPLATE:
    case SC_EXTERN_FUNCTION_TEMPLATE:
    case SC_STATIC_FUNCTION_TEMPLATE:
        linkage = CppLinkage;
        break;
    default:
        if( sym->flag & SF_TEMPLATE_FN ) {
            linkage = CppLinkage;
        } else {
            linkage = CurrLinkage;
            if( language != NULL ) {
                linkage = findLinkage( language );
            }
        }
    }
    if( linkage == CppLinkage ) {
        sym->flag |= SF_PLUSPLUS;
    } else {
        sym->flag &= ~SF_PLUSPLUS;
    }
}

bool LinkageIsCpp( SYMBOL sym )
/*****************************/
{
    verifyFileScopeSym( sym );
    return(( sym->flag & SF_PLUSPLUS ) != 0 );
}

bool LinkageIsC( SYMBOL sym )
/***************************/
{
    verifyFileScopeSym( sym );
    return(( sym->flag & SF_PLUSPLUS ) == 0 );
}

bool LinkageIsCurr( SYMBOL sym )
/******************************/
{
    verifyFileScopeSym( sym );
    if( CurrLinkage == CppLinkage ) {
        return(( sym->flag & SF_PLUSPLUS ) != 0 );
    }
    return(( sym->flag & SF_PLUSPLUS ) == 0 );
}

bool LinkageSpecified( void )
/***************************/
{
    return( nestedLinkages != resetLinkages );
}

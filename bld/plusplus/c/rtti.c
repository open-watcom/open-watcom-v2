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


#include <stdio.h>

#include "plusplus.h"
#include "errdefns.h"
#include "carve.h"
#include "pcheader.h"
#include "initdefs.h"
#include "ring.h"
#include "cgfront.h"
#include "cgback.h"
#include "rtti.h"

#define BLOCK_RTTI_CLASS        16
#define BLOCK_RTTI_TYPEID       16
#define BLOCK_RTTI_VFPTR        16
static carve_t carveRTTI_CLASS;         // - allocations for RTTI_CLASSs
static carve_t carveRTTI_TYPEID;        // - allocations for RTTI_TYPEIDs
static carve_t carveRTTI_VFPTR;         // - allocations for RTTI_VFPTRs
static RTTI_CLASS *rttiClasses;         // - rtti classes so far
static RTTI_TYPEID *rttiTypeids;        // - rtti typeids so far


static RTTI_CLASS *newClass( TYPE class_type )
{
    RTTI_CLASS *new_class;

    new_class = CarveAlloc( carveRTTI_CLASS );
    RingPush( &rttiClasses, new_class );
    new_class->vfptrs = NULL;
    new_class->class_type = class_type;
    new_class->sym = NULL;
    new_class->offset = 0;
    new_class->done = FALSE;
    new_class->gen = FALSE;
    new_class->cg_gen = FALSE;
    new_class->free = FALSE;
    new_class->too_big = FALSE;
    return( new_class );
}

static RTTI_CLASS *findClass( TYPE class_type )
{
    RTTI_CLASS *curr;

    RingIterBeg( rttiClasses, curr ) {
        if( curr->class_type == class_type ) {
            return( curr );
        }
    } RingIterEnd( curr )
    return( NULL );
}

static RTTI_TYPEID *newTypeid( TYPE type )
{
    RTTI_TYPEID *new_typeid;

    new_typeid = CarveAlloc( carveRTTI_TYPEID );
    RingPush( &rttiTypeids, new_typeid );
    new_typeid->type = type;
    new_typeid->sym = MakeTypeidSym( type );
    new_typeid->free = FALSE;
    return( new_typeid );
}

static RTTI_TYPEID *findTypeid( TYPE type )
{
    RTTI_TYPEID *curr;

    RingIterBeg( rttiTypeids, curr ) {
        if( curr->type == type ) {
            return( curr );
        }
    } RingIterEnd( curr )
    return( NULL );
}

static RTTI_VFPTR *newVfptr( RTTI_CLASS *class_entry, CLASS_TABLE *location )
{
    RTTI_VFPTR *new_vfptr;

    new_vfptr = CarveAlloc( carveRTTI_VFPTR );
    RingAppend( &(class_entry->vfptrs), new_vfptr );
    new_vfptr->control = RA_NULL;
    new_vfptr->free = FALSE;
    if( location->delta != location->exact_delta ) {
        /* vfptr is in a virtual base */
        if( location->ctor_disp ) {
            new_vfptr->control |= RA_CDISP;
            new_vfptr->delta = location->delta;
        } else {
            new_vfptr->delta = location->exact_delta;
        }
    } else {
        new_vfptr->delta = location->delta;
    }
    new_vfptr->offset = class_entry->offset;
    class_entry->offset += RttiAdjustSize();
    return( new_vfptr );
}

SYMBOL RttiBuild( SCOPE host, CLASS_TABLE *location, target_offset_t *poffset )
/*****************************************************************************/
{
    SYMBOL sym;
    TYPE class_type;
    RTTI_CLASS *class_entry;
    RTTI_VFPTR *vfptr_entry;

    if( ! CompFlags.rtti_enabled ) {
        *poffset = 0;
        return( NULL );
    }
    class_type = ScopeClass( host );
    class_entry = findClass( class_type );
    if( class_entry == NULL ) {
        class_entry = newClass( class_type );
        sym = MakeVATableSym( host );
        class_entry->sym = sym;
    }
    vfptr_entry = newVfptr( class_entry, location );
    *poffset = vfptr_entry->offset;
    return( class_entry->sym );
}

void RttiDone( SCOPE host )
/*************************/
{
    TYPE class_type;
    RTTI_CLASS *class_entry;

    class_type = ScopeClass( host );
    if( ! CompFlags.rtti_enabled ) {
        return;
    }
    class_entry = findClass( class_type );
    DbgAssert( class_entry != NULL );
    // class_entry can be NULL if there is a bug in name mangling
    // not creating unique enough names (rtti08.c)
    if( class_entry != NULL ) {
        class_entry->done = TRUE;
    }
}

static target_size_t rttiInfoSize( RTTI_CLASS *class_entry )
{
    TYPE class_type;
    unsigned nbases;
    target_size_t size;

    DbgAssert( class_entry->done );
    class_type = class_entry->class_type;
    nbases = ScopeRttiClasses( class_type );
    DbgAssert( nbases >= 1 );
    size = class_entry->offset;
    size += RttiClassSize();
    size += ( nbases - 1 ) * RttiLeapSize();
#if _CPU == 8086
    if( size > 0x010000 ) {
        CErr( ERR_TOO_MUCH_FOR_RTTI, class_type, nbases );
        size = 1;
        class_entry->too_big = TRUE;
    }
#endif
    return( size );
}


void RttiRef( SYMBOL sym )
/************************/
{
    target_size_t size;
    TYPE table_type;
    RTTI_CLASS *curr;

    if( sym == NULL ) {
        return;
    }
    RingIterBeg( rttiClasses, curr ) {
        if( curr->sym == sym ) {
            if( ! curr->gen ) {
                curr->gen = TRUE;
                DbgAssert( sym->segid == SEG_NULL );
                table_type = TypedefModifierRemove( sym->sym_type );
                DbgAssert( table_type->id == TYP_ARRAY );
                size = rttiInfoSize( curr );
                table_type->u.a.array_size = size;
                CgSegId( sym );
                SegmentMarkUsed( sym->segid );
            }
            return;
        }
    } RingIterEnd( curr )
    DbgNever();
    return;
}


void RttiWalk( void (*walker)( RTTI_CLASS * ) )
/*********************************************/
{
    RTTI_CLASS *curr;

    RingIterBegSafe( rttiClasses, curr ) {
        (*walker)( curr );
    } RingIterEndSafe( curr )
}

SYMBOL TypeidAccess( TYPE type )
/******************************/
{
    TYPE ref_type;
    RTTI_TYPEID *typeid_entry;
    SYMBOL sym;

    // this needs to function even if RTTI is not enabled since type signatures
    DbgAssert( type != NULL );
    type = TypedefModifierRemoveOnly( type );
    ref_type = TypeReference( type );
    if( ref_type != NULL ) {
        type = TypedefModifierRemoveOnly( ref_type );
    }
    typeid_entry = findTypeid( type );
    if( typeid_entry == NULL ) {
        typeid_entry = newTypeid( type );
    }
    sym = typeid_entry->sym;
    DbgAssert( sym != NULL );
    return( sym );
}

SYMBOL TypeidICAccess( TYPE type )
/********************************/
{
    SYMBOL sym;

    sym = TypeidAccess( type );
    CgFrontCodePtr( IC_TYPEID_REF, sym );
    if( ModuleInitScope() == CurrScope ) {
        // useless if module fn is generated (which it always will be if there
        // is any code to execute) but if there is no code, any static links
        // to typeids must be satisfied
        TypeidRef( sym );
    }
    return( sym );
}

void TypeidRef( SYMBOL sym )
/**************************/
{
    if( sym != NULL ) {
        sym->flag |= SF_REFERENCED;
        CgSegId( sym );
        SegmentMarkUsed( sym->segid );
    }
}

void TypeidWalk( void (*walker)( RTTI_TYPEID * ) )
/************************************************/
{
    RTTI_TYPEID *curr;

    RingIterBegSafe( rttiTypeids, curr ) {
        (*walker)( curr );
    } RingIterEndSafe( curr )
}

unsigned TypeidSize( unsigned len )
/*********************************/
{
    len += CgDataPtrSize() + 1;
    return( len );
}

static void init( INITFINI* defn )
{
    defn = defn;
    rttiClasses = NULL;
    rttiTypeids = NULL;
    carveRTTI_CLASS = CarveCreate( sizeof( RTTI_CLASS ), BLOCK_RTTI_CLASS );
    carveRTTI_TYPEID = CarveCreate( sizeof( RTTI_TYPEID ), BLOCK_RTTI_TYPEID );
    carveRTTI_VFPTR = CarveCreate( sizeof( RTTI_VFPTR ), BLOCK_RTTI_VFPTR );
}

static void fini( INITFINI* defn )
{
    defn = defn;
    CarveDestroy( carveRTTI_CLASS );
    CarveDestroy( carveRTTI_TYPEID );
    CarveDestroy( carveRTTI_VFPTR );
}

INITDEFN( rtti_descriptors, init, fini )

static RTTI_CLASS *rttiClassGetIndex( RTTI_CLASS *e )
{
    return( CarveGetIndex( carveRTTI_CLASS, e ) );
}

static RTTI_CLASS *rttiClassMapIndex( RTTI_CLASS *e )
{
    return( CarveMapIndex( carveRTTI_CLASS, e ) );
}

static RTTI_VFPTR *rttiVfptrGetIndex( RTTI_VFPTR *e )
{
    return( CarveGetIndex( carveRTTI_VFPTR, e ) );
}

static RTTI_VFPTR *rttiVfptrMapIndex( RTTI_VFPTR *e )
{
    return( CarveMapIndex( carveRTTI_VFPTR, e ) );
}

static RTTI_TYPEID *rttiTypeidGetIndex( RTTI_TYPEID *e )
{
    return( CarveGetIndex( carveRTTI_TYPEID, e ) );
}

static RTTI_TYPEID *rttiTypeidMapIndex( RTTI_TYPEID *e )
{
    return( CarveMapIndex( carveRTTI_TYPEID, e ) );
}

pch_status PCHReadRttiDescriptors( void )
{
    cv_index i;
    RTTI_CLASS *c;
    RTTI_VFPTR *v;
    RTTI_TYPEID *t;
    auto cvinit_t data;

    // NYI: use read in place optimizations
    PCHRead( &rttiClasses, sizeof( rttiClasses ) );
    rttiClasses = rttiClassMapIndex( rttiClasses );
    PCHRead( &rttiTypeids, sizeof( rttiTypeids ) );
    rttiTypeids = rttiTypeidMapIndex( rttiTypeids );
    CarveInitStart( carveRTTI_CLASS, &data );
    for(;;) {
        i = PCHReadCVIndex();
        if( i == CARVE_NULL_INDEX ) break;
        c = CarveInitElement( &data, i );
        PCHRead( c, sizeof( *c ) );
        c->next = rttiClassMapIndex( c->next );
        c->vfptrs = rttiVfptrMapIndex( c->vfptrs );
        c->class_type = TypeMapIndex( c->class_type );
        c->sym = SymbolMapIndex( c->sym );
    }
    CarveInitStart( carveRTTI_VFPTR, &data );
    for(;;) {
        i = PCHReadCVIndex();
        if( i == CARVE_NULL_INDEX ) break;
        v = CarveInitElement( &data, i );
        PCHRead( v, sizeof( *v ) );
        v->next = rttiVfptrMapIndex( v->next );
    }
    CarveInitStart( carveRTTI_TYPEID, &data );
    for(;;) {
        i = PCHReadCVIndex();
        if( i == CARVE_NULL_INDEX ) break;
        t = CarveInitElement( &data, i );
        PCHRead( t, sizeof( *t ) );
        t->next = rttiTypeidMapIndex( t->next );
        t->type = TypeMapIndex( t->type );
        t->sym = SymbolMapIndex( t->sym );
    }
    return( PCHCB_OK );
}

static void markFreeClass( void *p )
{
    RTTI_CLASS *s = p;

    s->free = TRUE;
}

static void markFreeVfptr( void *p )
{
    RTTI_VFPTR *s = p;

    s->free = TRUE;
}

static void markFreeTypeid( void *p )
{
    RTTI_TYPEID *s = p;

    s->free = TRUE;
}

static void saveClass( void *e, carve_walk_base *d )
{
    RTTI_CLASS *s = e;
    RTTI_CLASS *save_next;
    RTTI_VFPTR *save_vfptrs;
    TYPE save_class_type;
    SYMBOL save_sym;

    if( s->free ) {
        return;
    }
    save_next = s->next;
    s->next = rttiClassGetIndex( save_next );
    save_vfptrs = s->vfptrs;
    s->vfptrs = rttiVfptrGetIndex( save_vfptrs );
    save_class_type = s->class_type;
    s->class_type = TypeGetIndex( save_class_type );
    save_sym = s->sym;
    s->sym = SymbolGetIndex( save_sym );
    PCHWriteCVIndex( d->index );
    PCHWrite( s, sizeof( *s ) );
    s->next = save_next;
    s->vfptrs = save_vfptrs;
    s->class_type = save_class_type;
    s->sym = save_sym;
}

static void saveVfptr( void *e, carve_walk_base *d )
{
    RTTI_VFPTR *s = e;
    RTTI_VFPTR *save_next;

    if( s->free ) {
        return;
    }
    save_next = s->next;
    s->next = rttiVfptrGetIndex( save_next );
    PCHWriteCVIndex( d->index );
    PCHWrite( s, sizeof( *s ) );
    s->next = save_next;
}

static void saveTypeid( void *e, carve_walk_base *d )
{
    RTTI_TYPEID *s = e;
    RTTI_TYPEID *save_next;
    TYPE save_type;
    SYMBOL save_sym;

    if( s->free ) {
        return;
    }
    save_next = s->next;
    s->next = rttiTypeidGetIndex( save_next );
    save_type = s->type;
    s->type = TypeGetIndex( save_type );
    save_sym = s->sym;
    s->sym = SymbolGetIndex( save_sym );
    PCHWriteCVIndex( d->index );
    PCHWrite( s, sizeof( *s ) );
    s->next = save_next;
    s->type = save_type;
    s->sym = save_sym;
}

pch_status PCHWriteRttiDescriptors( void )
{
    RTTI_CLASS *list_head;
    RTTI_TYPEID *head_typeid;
    cv_index terminator = CARVE_NULL_INDEX;
    auto carve_walk_base data;

    list_head = rttiClassGetIndex( rttiClasses );
    PCHWrite( &list_head, sizeof( list_head ) );
    head_typeid = rttiTypeidGetIndex( rttiTypeids );
    PCHWrite( &head_typeid, sizeof( head_typeid ) );
    CarveWalkAllFree( carveRTTI_CLASS, markFreeClass );
    CarveWalkAll( carveRTTI_CLASS, saveClass, &data );
    PCHWriteCVIndex( terminator );
    CarveWalkAllFree( carveRTTI_VFPTR, markFreeVfptr );
    CarveWalkAll( carveRTTI_VFPTR, saveVfptr, &data );
    PCHWriteCVIndex( terminator );
    CarveWalkAllFree( carveRTTI_TYPEID, markFreeTypeid );
    CarveWalkAll( carveRTTI_TYPEID, saveTypeid, &data );
    PCHWriteCVIndex( terminator );
    return( PCHCB_OK );
}

pch_status PCHInitRttiDescriptors( boolean writing )
{
    cv_index n;

    if( writing ) {
        n = CarveLastValidIndex( carveRTTI_CLASS );
        PCHWriteCVIndex( n );
        n = CarveLastValidIndex( carveRTTI_VFPTR );
        PCHWriteCVIndex( n );
        n = CarveLastValidIndex( carveRTTI_TYPEID );
        PCHWriteCVIndex( n );
    } else {
        carveRTTI_CLASS = CarveRestart( carveRTTI_CLASS );
        n = PCHReadCVIndex();
        CarveMapOptimize( carveRTTI_CLASS, n );
        carveRTTI_VFPTR = CarveRestart( carveRTTI_VFPTR );
        n = PCHReadCVIndex();
        CarveMapOptimize( carveRTTI_VFPTR, n );
        carveRTTI_TYPEID = CarveRestart( carveRTTI_TYPEID );
        n = PCHReadCVIndex();
        CarveMapOptimize( carveRTTI_TYPEID, n );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniRttiDescriptors( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveRTTI_CLASS );
        CarveMapUnoptimize( carveRTTI_VFPTR );
        CarveMapUnoptimize( carveRTTI_TYPEID );
    }
    return( PCHCB_OK );
}

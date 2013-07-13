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

#include "cgback.h"
#include "cgbackut.h"
#include "typesig.h"
#include "ctexcept.h"
#include "carve.h"
#include "ring.h"
#include "rtti.h"
#include "initdefs.h"

#ifndef NDEBUG
    #include <stdio.h>
    #include "dbg.h"
    #include "toggle.h"
    #include "pragdefn.h"
#endif

static carve_t carveTYPE_SIG_ENT;   // allocations for TYPE_SIG_ENT
static boolean type_sig_gened;      // TRUE ==> a type signature was gen'ed


TYPE_SIG *BeTypeSignature(      // GET TYPE_SIG FOR A TYPE
    TYPE type )                 // - input type
{
    TYPE_SIG *sig;              // - type signature for type
    TYPE_SIG *base;             // - base type signatures for type
    boolean errors;             // - TRUE set if error occurred

    sig = TypeSigFind( TSA_GEN, type, NULL, &errors );
    DbgVerify( ! errors, "BeTypeSignature -- unexpected errors" );
    base = sig->base;
    if( NULL == base ) {
        sig->cgref = TRUE;
    } else {
        for( ; base != NULL; base = base->base ) {
            base->cgref = TRUE;
        }
    }
    return sig;
}


void BeGenTsRef(                // GENERATE REFERENCE TO TYPE-SIGNATURE
    TYPE_SIG* ts )              // - type signature
{
    SYMBOL sym;                 // - symbol for reference
    target_offset_t offset;     // - offset for reference

    TypeSigSymOffset( ts, &sym, &offset );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        VBUF vbuf;
        printf( " typsig=%s+%x"
              , DbgSymNameFull( sym, &vbuf )
              , offset );
        VbufFree( &vbuf );
    }
#endif
    DgPtrSymDataOffset( sym, offset );
}


static void genName(            // OPTIONALLY GENERATE MANGLED NAME
    THROBJ thr,                 // - category of object
    TYPE type )                 // - type to be mangled
{
    SYMBOL typeid_sym;

    switch( thr ) {
      case THROBJ_SCALAR :
      case THROBJ_PTR_FUN :
      case THROBJ_CLASS :
      case THROBJ_CLASS_VIRT :
        typeid_sym = TypeidAccess( type );
        TypeidRef( typeid_sym );
        DgPtrSymDataOffset( typeid_sym, TypeidRawNameOffset() );
        break;
      case THROBJ_PTR_SCALAR :
      case THROBJ_PTR_CLASS :
      case THROBJ_REFERENCE :
      case THROBJ_VOID_STAR :
      case THROBJ_ANYTHING :
        break;
      DbgDefault( "genName -- bad THROBJ_..." );
    }
}


static void genBaseHdr(         // GENERATE BASE HEADER FOR TYPE-SIGNATURE
    THROBJ thr,                 // - actual object type
    uint_8 flags )              // - flags
{
    DgByte( thr );
    DgByte( THROBJ_PTR_CLASS );
    DgByte( THROBJ_REFERENCE );
    DgByte( flags );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( " base=%d,%d,%d,%d"
              , thr
              , THROBJ_PTR_CLASS
              , THROBJ_REFERENCE
              , flags );
    }
#endif
}


static void genScalarHdr(       // GENERATE SCALAR HDR
    TYPE_SIG *ts )              // - to be gen'ed
{
    unsigned size;              // - size of type

    size = CgMemorySize( ts->type );
    DgOffset( size );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( " size=%d\n", size );
    }
#endif
}


#if 0
static void genTsPtr(           // GENERATE PTR TO TYPE SIGNATURE
    TYPE type )                 // - type for type signature
{
    TYPE_SIG *ts;               // - type signature

    ts = BeTypeSignature( type );
    DgPtrSymData( ts->sym );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( " sig=%x\n", ts );
    }
#endif
}
#endif


static void genTypeSig(         // GENERATE A TYPE_SIG
    TYPE_SIG *ts )              // - to be gen'ed
{
    THROBJ thr;                 // - category of object

    if( ! ts->cgref ) return;
    if( ts->cggen ) return;
    ts->cggen = TRUE;
    type_sig_gened = TRUE;
    thr = ThrowCategory( ts->type );
    if( thr == THROBJ_ANYTHING ) return;
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        const char* code;
        switch( thr ) {
          case THROBJ_SCALAR :      code = "THROBJ_SCALAR";     break;
          case THROBJ_PTR_CLASS :   code = "THROBJ_PTR_CLASS";  break;
          case THROBJ_PTR_SCALAR :  code = "THROBJ_PTR_SCALAR"; break;
          case THROBJ_PTR_FUN :     code = "THROBJ_PTR_FUN";    break;
          case THROBJ_VOID_STAR :   code = "THROBJ_VOID_STAR";  break;
          case THROBJ_REFERENCE :   code = "THROBJ_REFERENCE";  break;
          case THROBJ_CLASS :       code = "THROBJ_CLASS";      break;
          case THROBJ_CLASS_VIRT :  code = "THROBJ_CLASS_VIRT"; break;
          case THROBJ_ANYTHING :    code = "THROBJ_ANYTHING";   break;
          default:                  code = "****** BAD ******"; break;
        }
        printf( "Type Signature[%x] %s(%x)"
              , ts
              , code
              , thr );
    }
#endif
    BESetSeg( ts->sym->segid );
    DGLabel( FEBack( ts->sym ) );
    switch( thr ) {
      case THROBJ_PTR_SCALAR :
      case THROBJ_PTR_CLASS :
        genBaseHdr( thr, 1 );
        BeGenTsRef( ts );
#ifndef NDEBUG
        if( PragDbgToggle.dump_stab ) {
            printf( "\n" );
        }
#endif
        break;
      case THROBJ_VOID_STAR :
        genBaseHdr( thr, 0 );
        genScalarHdr( ts );
        break;
      case THROBJ_PTR_FUN :
      case THROBJ_SCALAR :
        genBaseHdr( thr, 0 );
        genScalarHdr( ts );
        genName( thr, ts->type );
        break;
      case THROBJ_CLASS :
      case THROBJ_CLASS_VIRT :
      { unsigned size;
        genBaseHdr( thr, 0 );
        DgPtrSymCode( ts->default_ctor );
        DgPtrSymCode( ts->copy_ctor );
        DgPtrSymCode( ts->dtor );
        size = CgMemorySize( ts->type );
        DgOffset( size );
        genName( thr, ts->type );
#ifndef NDEBUG
        if( PragDbgToggle.dump_stab ) {
            VBUF vbuf1;
            VBUF vbuf2;
            VBUF vbuf3;
            printf( " size=%x\n"
                    "     ctor = %s\n"
                    "     copy = %s\n"
                    "     dtor = %s\n"
                  , size
                  , DbgSymNameFull( ts->default_ctor, &vbuf1 )
                  , DbgSymNameFull( ts->copy_ctor, &vbuf2 )
                  , DbgSymNameFull( ts->dtor, &vbuf3 ) );
            VbufFree( &vbuf1 );
            VbufFree( &vbuf2 );
            VbufFree( &vbuf3 );
        }
#endif
      } break;
      DbgDefault( "\ngenTypeSig -- invalid throw category" );
    }
}


void BeGenTypeSignatures(       // GENERATE ALL TYPE SIGNATURES
    void )
{
    do {
        type_sig_gened = FALSE;
        TypeSigWalk( &genTypeSig );
    } while( type_sig_gened );
}


static TYPE_SIG_ENT* typeSigEnt(// ALLOCATE A TYPE_SIG_ENT, FOR TYPE_SIG
    TYPE_SIG* sig )             // - type signature pointer
{
    TYPE_SIG_ENT* ent;          // - new entry

    ent = CarveAlloc( carveTYPE_SIG_ENT );
    ent->sig = sig;
    return ent;
}


TYPE_SIG_ENT* BeTypeSigEnt(     // ALLOCATE A TYPE_SIG_ENT, FOR TYPE
    TYPE type )                 // - type for signature
{
    return typeSigEnt( BeTypeSignature( type ) );
}


TYPE_SIG_ENT* BeTypeSigEntsCopy(// MAKE COPY OF TYPE-SIGNATURE entries
    TYPE_SIG_ENT* orig )        // - original entries
{
    TYPE_SIG_ENT* curr;         // - current entry
    TYPE_SIG_ENT* ring;         // - ring of entries added

    ring = NULL;
    RingIterBeg( orig, curr ) {
        RingAppend( &ring, typeSigEnt( curr->sig ) );
    } RingIterEnd( curr )
    return ring;
}


void BeGenTypeSigEnts(          // EMIT TYPE_SIG_ENT RING
    TYPE_SIG_ENT* ring )        // - ring of entries
{
    TYPE_SIG_ENT* curr;         // - current entry
    unsigned count;             // - # entries

    count = RingCount( ring );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( "%d\n", count );
    }
#endif
    DgOffset( count );
    RingIterBegSafe( ring, curr ) {
       BeGenTsRef( curr->sig );
#ifndef NDEBUG
        if( PragDbgToggle.dump_stab ) {
            printf( "\n" );
        }
#endif
       CarveFree( carveTYPE_SIG_ENT, curr );
    } RingIterEndSafe( curr )
}


static void typesigInit(        // INITIALIZATION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveTYPE_SIG_ENT = CarveCreate( sizeof( TYPE_SIG_ENT ), 32 );
}


static void typesigFini(        // COMPLETION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveTYPE_SIG_ENT );
}


INITDEFN( type_sigs, typesigInit, typesigFini )

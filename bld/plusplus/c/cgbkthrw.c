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
#include "name.h"
#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "ctexcept.h"
#include "initdefs.h"

#ifndef NDEBUG
    #include "errdefns.h"
    #include "pragdefn.h"
#endif


static THROW_RO *ring_throw_ro; // THROW R/O blocks defined
static carve_t carveTHROW_RO;   // allocations for THROW_ROs


static target_size_t sizeOfThrowCnv( // GET SIZE OF A THROW TYPE CONVERSION
    void )
{
    return CgbkInfo.size_code_ptr + CgbkInfo.size_offset;
}


static THROW_RO *throwRoGet(    // GET THROW R/O BLOCK
    TYPE type )                 // - type being thrown
{
    THROW_RO *srch;             // - search R/O block for symbol
    THROW_RO *ro;               // - R/O block for symbol
    SYMBOL sym;                 // - symbol for throw object
    THROW_CNV_CTL ctl;          // - control area
    target_size_t offset;       // - offset ( not used )

    type = TypeCanonicalThr( type );
    ro = NULL;
    RingIterBeg( ring_throw_ro, srch ) {
        if( TypesIdentical( srch->sig->type, type ) ) {
            ro = srch;
            break;
        }
    } RingIterEnd( srch );
    if( ro == NULL ) {
        ro = RingCarveAlloc( carveTHROW_RO, &ring_throw_ro );
        ro->sig = BeTypeSignature( type );
        ro->emitted = FALSE;
        sym = CgVarRo( CgbkInfo.size_offset
                        + sizeOfThrowCnv() * ThrowCnvInit( &ctl, type )
                     , SC_PUBLIC
                     , CppNameThrowRo( type ) );
        ro->sym = sym;
        for( ; ; ) {
            type = ThrowCnvType( &ctl, &offset );
            if( type == NULL ) break;
            BeTypeSignature( type );
        }
        ThrowCnvFini( &ctl );
    }
    return ro;
}


cg_name ThrowRo(                // CREATE/ACCESS THROW R/O BLOCK
    TYPE type )                 // - type being thrown
{
    return CgSymbol( throwRoGet( type )->sym );
}


static void cgGenThrowCnv(      // GENERATE THROW CONVERSION
    TYPE type,                  // - target type
    target_size_t offset )      // - offset within class
{
    TYPE_SIG *sig;              // - signature for type

    sig = BeTypeSignature( type );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( "   conversion: " );
    }
#endif
    BeGenTsRef( sig );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( " offset=%x\n"
              , offset );
    }
#endif
    DgOffset( offset );
}


void ThrowRoGen(                // GENERATE A THROW R/O BLOCK
    void )
{
    THROW_RO *ro;               // - R/O block for symbol
    segment_id old;             // - old segment
    TYPE type;                  // - type being converted
    THROW_CNV_CTL ctl;          // - control for conversions
    target_size_t offset;       // - offset within class

    RingIterBeg( ring_throw_ro, ro ) {
        if( ! ro->emitted ) {
            ro->emitted = TRUE;
            old = CgBackGenLabelInternal( ro->sym );
            offset = ThrowCnvInit( &ctl, ro->sig->type );
            DbgVerify( ! ctl.error_occurred
                     , "cgGenThrowRo -- failure in ThrowCnvInit" );
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "ThrowRo[%x] offset=%x sig=%x\n"
                      , ro
                      , ro->sig
                      , offset );
            }
#endif
            DgOffset( offset );
            for( ; ; ) {
                type = ThrowCnvType( &ctl, &offset );
                if( type == NULL ) break;
                cgGenThrowCnv( type, offset );
            }
            ThrowCnvFini( &ctl );
        }
    } RingIterEnd( ro );
}


static void throwInit(          // INITIALIZE CGBKTHRW
    INITFINI* defn )            // - definition
{
    defn = defn;
    ring_throw_ro = NULL;
    carveTHROW_RO = CarveCreate( sizeof( THROW_RO ), 4 );
}


static void throwFini(          // COMPLETE CGBKTHRW
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveTHROW_RO );
}


INITDEFN( throw, throwInit, throwFini )

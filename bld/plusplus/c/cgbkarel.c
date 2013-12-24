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
#include "initdefs.h"

typedef struct auto_rel         AUTO_REL; // A RELOCATION
struct auto_rel {
    AUTO_REL* next;             // - next relocation
    target_offset_t* a_off;     // - addr[ offset to be relocated ]
};

typedef struct auto_sym         AUTO_SYM; // RELOCATIONS FOR AN AUTO SYMBOL
struct auto_sym {
    AUTO_SYM* next;             // - next symbol entry
    SYMBOL sym;                 // - symbol being relocated
    AUTO_REL* rels;             // - relocations
};

#ifndef NDEBUG
    #include <stdio.h>
    #include "toggle.h"
    #include "pragdefn.h"

    void __dump_sym( AUTO_SYM*asym, const char* msg )
    {
        if( PragDbgToggle.dump_auto_rel ) {
            printf( "AUTO_SYM[%p] rels(%p) sym(%p) %s\n"
                  , asym
                  , asym->rels
                  , asym->sym
                  , msg );
        }
    }

#else
    #define __dump_sym(a,b)
#endif


                                // STATIC DATA
static carve_t carve_sym;       // - memory: AUTO_SYM's
static carve_t carve_rel;       // - memory: AUTO_REL's
static AUTO_SYM* ring_syms;     // - ring of symbols to be relocated


static AUTO_SYM* autoRelLookup( // FIND AUTO_SYM FOR A SYMBOL
    SYMBOL sym )                // - the symbol
{
    AUTO_SYM* retn;             // - returned entry
    AUTO_SYM* curr;             // - current entry

    retn = NULL;
    RingIterBeg( ring_syms, curr ) {
        if( sym == curr->sym ) {
            retn = curr;
            break;
        }
    } RingIterEnd( curr )
    return retn;
}


void AutoRelRegister(           // REGISTER A RELOCATION
    SYMBOL sym,                 // - symbol
    target_offset_t* a_off )    // - addr[ offset to be relocated ]
{
    AUTO_SYM* sym_entry;        // - entry for symbol
    AUTO_REL* rel_entry;        // - entry for relocation

    sym_entry = autoRelLookup( sym );
    if( sym_entry == NULL ) {
        sym_entry = RingCarveAlloc( carve_sym, &ring_syms );
        sym_entry->sym = sym;
        sym_entry->rels = NULL;
        __dump_sym( sym_entry, "-- added" );
    } else {
        __dump_sym( sym_entry, "-- used" );
    }
    rel_entry = RingCarveAlloc( carve_rel, &sym_entry->rels );
    rel_entry->a_off = a_off;
    *a_off = UNDEF_AREL;
}


void CgBackDtorAutoOffset(      // RELOCATION FOR A SYMBOL
    SYMBOL sym,                 // - symbol
    unsigned offset )           // - offset
{
    AUTO_SYM* sym_entry;        // - entry for symbol
    AUTO_REL* rel_entry;        // - current relocation entry

    sym_entry = autoRelLookup( sym );
    if( sym_entry != NULL ) {
        __dump_sym( sym_entry, "-- relocated" );
#ifndef NDEBUG
        if( PragDbgToggle.dump_auto_rel ) {
            printf( "  --- offset = %x/%d\n", offset, offset );
        }
#endif
        RingIterBeg( sym_entry->rels, rel_entry ) {
            *rel_entry->a_off = offset;
        } RingIterEnd( rel_entry )
        RingCarveFree( carve_rel, &sym_entry->rels );
    }
}


void AutoRelFree(               // FREE ALL RELOCATIONS
    void )
{
#ifndef NDEBUG
    AUTO_SYM* curr;             // - current entry
    RingIterBeg( ring_syms, curr ) {
        DbgVerify( curr->rels == NULL, "AutoRelFree -- missed relocation" );
    } RingIterEnd( curr )
#endif
    RingCarveFree( carve_sym, &ring_syms );
}


static void autoRelInit(        // INITIALIZE CGBKAREL
    INITFINI* defn )            // - definition
{
    defn = defn;
    carve_sym = CarveCreate( sizeof( AUTO_SYM ), 32 );
    carve_rel = CarveCreate( sizeof( AUTO_REL ), 32 );
}


static void autoRelFini(        // COMPLETE CGBKAREL
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carve_sym );
    CarveDestroy( carve_rel );
}


INITDEFN( auto_rel, autoRelInit, autoRelFini )

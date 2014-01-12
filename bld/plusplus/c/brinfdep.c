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

#ifdef OPT_BR

#   include "brinfoim.h"
#   include "ring.h"
#   include "pstk.h"
#   include "srcfile.h"

#ifndef NDEBUG
#   include "dbg.h"
#   include "pragdefn.h"
#endif

struct MACDEP                   // MACDEP -- macro dependency
{   MACDEP* next;               // - next in ring
    MEPTR macro;                // - macro
    MACVALUE* value;            // - value
    MAC_VTYPE type;             // - type of dependency
    uint_8 written          :1; // - TRUE ==> written out
    unsigned :0;                // - alignment
};

struct SRCDEP                   // SRCDEP -- macro dependency for source file
{   SRCDEP* next;               // - next in ring
    MACDEP* deps;               // - ring of dependencies
    SRCFILE srcfile;            // - the source file
};

static PSTK_CTL active_srcfiles;// active source files
static carve_t carve_src_dep;   // carver: source dependencies
static carve_t carve_mac_dep;   // carver: macro dependencies
static SRCDEP* ring_src_deps;   // ring of source dependencies


void BrinfDepInit               // MODULE INITIALIZATION
    ( void )
{
    carve_src_dep = CarveCreate( sizeof( SRCDEP ), 16 );
    carve_mac_dep = CarveCreate( sizeof( MACDEP ), 64 );
    PstkOpen( &active_srcfiles );
    ring_src_deps = NULL;
}


void BrinfDepFini               // MODULE COMPLETION
    ( void )
{
    CarveDestroy( carve_src_dep );
    CarveDestroy( carve_mac_dep );
    PstkClose( &active_srcfiles );
}


void BrinfDepRestart            // MODULE RESTART DURING PCH READ
    ( void )
{
    BrinfDepFini();
    BrinfDepInit();
}


SRCDEP* BrinfDepSrcBeg          // ALLOCATE A SRCDEP
    ( SRCFILE srcfile )         // - file name
{
    SRCDEP* sd = RingCarveAlloc( carve_src_dep, &ring_src_deps);
    sd->deps = NULL;
    sd->srcfile = srcfile;
    PstkPush( &active_srcfiles, sd );
    IfDbgToggle( browse ) {
        DbgStmt(
            printf( "BrowseDep: add %x SRCFILE %x\n"
                  , sd
                  , srcfile )
        );
    }
    return sd;
}


void BrinfDepSrcEnd             // END OF SOURCE-FILE DEPENDENCIES
    ( void )
{
    PstkPop( &active_srcfiles );
}


char const * BrinfDepSrcFname   // GET FILE NAME FOR SOURCE-DEPENDENCY
    ( SRCDEP const *sd )        // - dependency for source file
{
    return SrcFileFullName( sd->srcfile );
}


void BrinfDepWrite              // WRITE DEPENDENCY INFORMATION
    ( SRCDEP const *sd )        // - dependency for source file
{
    MACDEP *cd;                 // - current macro dependency

    DbgVerify( NULL != sd, "NULL dependency ptr" );
    RingIterBeg( sd->deps, cd ) {
        if( ! cd->written ) {
            switch( cd->type ) {
              case MVT_VALUE :
                BrinfWriteDepMacVal( cd->value );
                break;
              case MVT_DEFINED :
                BrinfWriteDepMacDefed( cd->value );
                break;
              case MVT_UNDEFED :
                BrinfWriteDepMacUndefed( cd->value );
                break;
              DbgDefault( "bad type of macro dependency" );
            }
            cd->written = TRUE;
        }
    } RingIterEnd( cd );
}


static bool srcfilePrecedes     // SEE IF A SOURCE FILE PRECEDES ANOTHER
    ( SRCFILE curr              // - current source file
    , SRCFILE pred )            // - potential predecessor or NULL
{
    return curr != pred
        && ( pred == NULL
          || SrcFileIndex( curr ) > SrcFileIndex( pred ) );
}


void BrinfDepMacAdd             // ADD A MACRO DEPENDENCY
    ( MEPTR macro               // - the macro
    , MACVALUE* value           // - value
    , MAC_VTYPE type )          // - dependency type
{
    SRCFILE defn;               // - source where last value set
    SRCDEP* sd;                 // - source-dependency file

    sd = PstkTopElement( &active_srcfiles );
    DbgVerify( NULL != sd, "No active srcfile" );
    switch( type ) {
      case MVT_VALUE :
        defn = macro->defn.src_file;
        break;
      case MVT_DEFINED :
        defn = macro->defn.src_file;
        macro = NULL;
        break;
      case MVT_UNDEFED :
        defn = BrinfMacUndefSource( BrinfMacValueName( value ) );
        macro = NULL;
        break;
      DbgDefault( "Impossible MAC_VTYPE" );
    }
    if( srcfilePrecedes( sd->srcfile, defn ) ) {
        // dependency only when macro set before the current source file
        MACDEP* md = NULL;
        MACDEP* cd;
        RingIterBeg( sd->deps, cd ) {
            if( cd->value == value
             && cd->macro == macro
             && cd->type == type ) {
                md = cd;
                IfDbgToggle( browse ) {
                    DbgStmt(
                        printf( "BrowseDep: macold %x SRCFILE %x MACRO %x"
                                " VALUE %x TYPE %d\n"
                              , sd
                              , sd->srcfile
                              , macro
                              , value
                              , type )
                    );
                }
                break;
            }
        } RingIterEnd( cd );
        if( NULL == md ) {
            md = RingCarveAlloc( carve_mac_dep, &sd->deps );
            md->macro = macro;
            md->value = value;
            md->type = type;
            md->written = FALSE;
            IfDbgToggle( browse ) {
                DbgStmt(
                    printf( "BrowseDep: macadd %x SRCFILE %x MACRO %x"
                            " VALUE %x TYPE %d\n"
                          , sd
                          , sd->srcfile
                          , macro
                          , value
                          , type )
                );
            }
        }
    }
}


SRCFILE BrinfDepSrcfile         // GET SRCFILE FOR DEPENDENCY
    ( SRCDEP const * sd )       // - dependency
{
    return sd->srcfile;
}


#endif // OPT_BR

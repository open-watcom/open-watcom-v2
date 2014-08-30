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
* Description:  Support for automated initialization/completion.
*
****************************************************************************/

/*
    EXIT_REG entries are initialized when the exit point is acquired and
    are completed when that point is released.  Completions are in reverse
    order of the initializations.

    Split entries generate a SPLITINIT entry which cause splitInit to be
    invoked as the initialization routine.

    SplitInit invokes the actual initialization routine and then links the
    SPLITFINI entry contained within the SPLITINIT entry into a pending set.

    The SPLITFINI entry points at both the initialization and completion
    exit points.  When either of these is released, the SPLITFINI entry is
    removed from the pending set and the indicated completion routine is
    invoked.

    This ensures that completion is done through normal and through suicidal
    execution paths.

    Split entries are done after the completion of the EXIT_REG entries, in
    reverse order of initialization.

    this module depend on initspec.h

*/

#include "initdefs.h"
#include "ringcarv.h"

typedef void (*ac_callback)( EXIT_POINT * );

static EXIT_POINT *exit_stack;              // stack of active exit points
static ac_callback acquisition_callback;    // call-back at exit-point acquisition

#ifdef SPLIT_REQD
#define SPLIT_NAME( name, fini )    split_ ## name ## _ ## fini
#else
#define SPLIT_NAME( name, fini )    INIT_FINI_NAME( name )
#endif

// The following sets up definitions for any split entries

#ifdef SPLIT_REQD

typedef struct split_init       SPLITINIT;
typedef struct split_fini       SPLITFINI;

struct split_fini {             // SPLITFINI -- entry for completion
    SPLITFINI       *next;      // - next entry in pending ring
    INITFINI        *orig;      // - actual entry
    EXIT_POINT      *init;      // - exit point for initialization
    EXIT_POINT      *fini;      // - exit point for completion
};

struct split_init {             // SPLITINIT -- entry for initialization
    INITFINI        defn;       // - standard entry for initialization
    SPLITFINI       fini;       // - entry for completion
};

static void splitInit( INITFINI *d );
static void splitFini( INITFINI *d );

// The following sets up static definitions for any split entries

#define SPLIT_INIT( name, fini )                    \
    static SPLITINIT SPLIT_NAME( name, fini ) = {   \
      { &splitInit                                  \
        , &InitFiniStub                             \
      }                                             \
      , { NULL                                      \
        , &INIT_FINI_NAME( name )                   \
        , NULL                                      \
        , (EXIT_POINT *)&EXIT_POINT_NAME( fini )    \
      }                                             \
    };

#define EXIT_BEG( name )
#define EXIT_REG( name )
#define EXIT_END

#include "initspec.h"           // supplied by front-end

#undef EXIT_BEG
#undef EXIT_REG
#undef EXIT_END
#undef SPLIT_INIT

#endif // SPLIT_REQD

// The following sets up the standard tables prototypes

#define EXIT_BEG( name )                    \
struct EXIT_POINT_NAME( name ) {            \
    EXIT_POINT      *previous;              \
    unsigned        subsequent :1;          \
    INITFINI        *registered[ 0
#define EXIT_REG( name )          +1
#define SPLIT_INIT( name, fini )  +1
#define EXIT_END                  +1 ];     \
};

#include "initspec.h"           // supplied by front-end

#undef EXIT_BEG
#undef EXIT_REG
#undef EXIT_END
#undef SPLIT_INIT

// The following sets up the standard tables

#define EXIT_BEG( name )                    \
struct EXIT_POINT_NAME( name ) EXIT_POINT_NAME( name ) = { \
    NULL, 0, {

#define EXIT_REG( name )                    \
        &INIT_FINI_NAME( name ),

#define EXIT_END                            \
        NULL                                \
    }                                       \
};

#define SPLIT_INIT( name, fini )    (INITFINI *)&SPLIT_NAME( name, fini ),

#include "initspec.h"           // supplied by front-end

#undef EXIT_BEG
#undef EXIT_REG
#undef EXIT_END
#undef SPLIT_INIT

#ifdef SPLIT_REQD

static SPLITFINI *split_hdr;        // ring of active split entries

static void splitInit(              // INITIALIZE SPLIT ENTRY
    INITFINI *d )                   // - initialization entry
{
    SPLITINIT *init;                // - initialization entry
    SPLITFINI *fptr;                // - points at completion

    init = (SPLITINIT *)d;
    fptr = &init->fini;
    (*fptr->orig->init_rtn)( fptr->orig );
    fptr->init = exit_stack;
    RingInsert( &split_hdr, fptr, NULL );
}

static void splitFini(              // COMPLETE SPLIT ENTRY
    INITFINI *d )                   // - completion entry
{
    SPLITFINI *fptr;                // - completion entry

    fptr = (SPLITFINI *)d;
    RingPrune( &split_hdr, fptr );
    (*fptr->orig->fini_rtn)( fptr->orig );
}

#endif // SPLIT_REQD

void ExitPointAcquireRtn(       // ESTABLISH EXIT_POINT
    EXIT_POINT *est )           // - point to be established
{
    INITFINI **iptr;            // - current init/completion entry ptr.
    INITFINI *ient;             // - current init/completion entry

#if 0
//**** this is a problem (maybe solved by a front-end callback?)
    CompFlags.dll_subsequent = est->subsequent;
#else
//**** yes
    acquisition_callback( est );
#endif
    est->previous = exit_stack;
    exit_stack = est;
    for( iptr = est->registered; *iptr != NULL; ++iptr ) {
        ient = *iptr;
        (*ient->init_rtn)( ient );
    }
    est->subsequent = 1;
}


static void releaseExitPoint(   // CALL COMPLETION RTN.S FOR AN EXIT_POINT
    EXIT_POINT *rel )           // - point being released
{
    INITFINI **iptr;            // - current init/completion entry ptr.
    INITFINI *ient;             // - current init/completion entry
#ifdef SPLIT_REQD
    SPLITFINI *fptr;            // - current pending split entry
#endif

    for( iptr = rel->registered; *iptr != NULL; ++iptr );
    for( ; iptr != rel->registered; ) {
        --iptr;
        ient = *iptr;
        (*ient->fini_rtn)( ient );
    }
#ifdef SPLIT_REQD
    RingIterBegSafe( split_hdr, fptr ) {
        if( fptr->init == rel || fptr->fini == rel ) {
            splitFini( (INITFINI *)fptr );
        }
    } RingIterEndSafe( fptr )
#endif
}


void ExitPointReleaseRtn(       // RELEASE EXIT_POINT
    EXIT_POINT *rel )           // - point to be released
{
    EXIT_POINT *top;            // - point on top of exit stack
    EXIT_POINT *previous;       // - next point on top of exit stack

    previous = exit_stack;
    do {
        top = previous;
        previous = top->previous;
        releaseExitPoint( top );
    } while( top != rel );
    exit_stack = previous;
}


void InitFiniStub(              // STUB FOR NULL INIT/FINI
    INITFINI *defn )            // - definition
{
    defn = defn;
}


static void defaultAcquisitionCallback( // default call back at acquisition
    EXIT_POINT *ex )                    // - acquisition
{
    ex = ex;
}


void InitFiniStartup(           // START-UP FOR INIT/FINI
    ac_callback cb )            // - call-back at aquisition
{
    if( NULL == cb ) {
        acquisition_callback = &defaultAcquisitionCallback;
    } else {
        acquisition_callback = cb;
    }
    exit_stack = NULL;
#ifdef SPLIT_REQD
    split_hdr = NULL;
#endif
}

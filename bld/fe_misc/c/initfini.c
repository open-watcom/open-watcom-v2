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


// make this module depend on initspec.h
#define INITSPEC_TABLES


#include "initdefs.h"
#include "ringcarv.h"

typedef struct split_init SPLIT_INIT;
typedef struct split_fini SPLIT_FINI;

struct split_fini {                 // SPLIT_FINI -- entry for completion
    SPLIT_FINI* next;               // - next entry in pending ring
    INITFINI* orig;                 // - actual entry
    EXIT_POINT* init;               // - exit point for initialization
    EXIT_POINT* fini;               // - exit point for completion
};

struct split_init {                 // SPLIT_INIT -- entry for initialization
    INITFINI defn;                  // - standard entry for initialization
    SPLIT_FINI fini;                // - entry for completion
};


static EXIT_POINT *exit_stack;  // stack of active exit points
static void (*acquisition_callback) // call-back at exit-point acquisition
    ( EXIT_POINT* );

#ifdef SPLIT_REQD

static SPLIT_FINI *split_hdr;   // ring of active split entries

static void splitInit(              // INITIALIZE SPLIT ENTRY
    INITFINI *d )                   // - initialization entry
{
    SPLIT_INIT *init;               // - initialization entry
    SPLIT_FINI *fptr;               // - points at completion

    init = (SPLIT_INIT*) d;
    fptr = &init->fini;
    (*fptr->orig->init_rtn)( fptr->orig );
    fptr->init = exit_stack;
    RingInsert( &split_hdr, fptr, NULL );
}

static void splitFini(              // COMPLETE SPLIT ENTRY
    INITFINI *d )                   // - completion entry
{
    SPLIT_FINI* fptr;               // - completion entry

    fptr = (SPLIT_FINI*) d;
    RingPrune( &split_hdr, fptr );
    (*fptr->orig->fini_rtn)( fptr->orig );
}


// The following sets up static definitions for any split entries

#define SPLIT_NAME( name, fini ) split_ ## name ## _ ## fini

#define SPLIT_INIT( name, fini )                    \
    static SPLIT_INIT SPLIT_NAME( name, fini )      \
    =   { { &splitInit                              \
          , &InitFiniStub                           \
          }                                         \
        , { NULL                                    \
          , &INIT_FINI_NAME( name )                 \
          , NULL                                    \
          , &EXIT_POINT_NAME( fini )                \
          }                                         \
        };

#endif // SPLIT_REQD

#define EXIT_BEG( name )
#define EXIT_REG( name )
#define EXIT_END

#include "initfini.h"


// The following sets up the standard tables

#define EXIT_BEG( name )                \
    EXIT_POINT EXIT_POINT_NAME( name ) =\
    {   NULL, 0

#define EXIT_REG( name )                \
    ,   &INIT_FINI_NAME( name )

#define EXIT_END                        \
    ,   NULL                            \
    };

#define SPLIT_INIT( name, fini )        \
    ,   (INITFINI*)&SPLIT_NAME( name, fini )

#include "initspec.h"       // front-end dependent initialization


void ExitPointAcquireRtn(       // ESTABLISH EXIT_POINT
    EXIT_POINT* est )           // - point to be established
{
    INITFINI** iptr;            // - current init/completion entry ptr.
    INITFINI* ient;             // - current init/completion entry

#if 0

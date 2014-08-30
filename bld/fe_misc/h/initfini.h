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
* Description:  Automated Initialization/Completion processing.
*
****************************************************************************/


#ifndef __INITFINI_H__
#define __INITFINI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct initfini     INITFINI;
typedef struct exit_point   EXIT_POINT;

struct initfini {                   // INITFINI -- register init/fini
    void (*init_rtn)( INITFINI * ); // - initialization routine
    void (*fini_rtn)( INITFINI * ); // - completion routine
};

struct exit_point {                 // EXIT_POINT -- registration point
    EXIT_POINT  *previous;          // - previous exit point
    unsigned    subsequent : 1;     // - TRUE ==> not first time
    INITFINI    *registered[1];     // - registrations
};


// naming conventions:

#define EXIT_POINT_NAME( name )  ExitPoint_ ## name
#define INIT_FINI_NAME(name)     InitFini_ ## name

#define ExitPointAcquire( name ) \
    ExitPointAcquireRtn( (EXIT_POINT *)&EXIT_POINT_NAME( name ) )

#define ExitPointRelease( name ) \
    ExitPointReleaseRtn( (EXIT_POINT *)&EXIT_POINT_NAME( name ) )

#ifdef __cplusplus
#define INITDEFN( defn, init, fini )                        \
    extern "C" INITFINI INIT_FINI_NAME( defn ) = { &init, &fini };
#else
#define INITDEFN( defn, init, fini ) \
    INITFINI INIT_FINI_NAME( defn ) = { &init, &fini };
#endif


// prototypes:

void ExitPointAcquireRtn(       // ESTABLISH EXIT_POINT
    EXIT_POINT* est )           // - point to be established
;
void ExitPointReleaseRtn(       // RELEASE EXIT_POINT
    EXIT_POINT* rel )           // - point to be released
;
void InitFiniStartup(           // START-UP FOR INIT/FINI
    void (*callback)            // - call back at acquisition
        ( EXIT_POINT* ) )       // - - acquisition
;
void InitFiniStub(              // STUB FOR NULL INIT/FINI
    INITFINI* defn )            // - definition
;

#include "initspec.h"           // supplied by front-end

#ifdef __cplusplus
};
#endif

#endif

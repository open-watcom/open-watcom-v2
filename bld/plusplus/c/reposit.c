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
#include "reposit.h"
#include "ring.h"
#include "initdefs.h"


static carve_t carveFunRepository;  // carver for functions
static REPO_REC_FUN* funRepository; // repository for functions


static void init(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    funRepository = NULL;
    carveFunRepository = CarveCreate( sizeof( REPO_REC_FUN ), 16 );
}


static void fini(               // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveFunRepository );
}


INITDEFN( fun_repository, init, fini );



REPO_REC_FUN* RepoFunAdd(       // ADD FUNCTION TO REPOSITORY
    const char* name,           // - function name
    REPO_FUN_FLAGS flags )      // - function flags
{
    REPO_REC_FUN* frec;         // - new function record

    frec = RingCarveAlloc( carveFunRepository, &funRepository );
    frec->name = name;
    frec->flags = flags;
    return frec;
}


REPO_REC_FUN* RepoFunRead(      // GET FUNCTION RECORD FROM REPOSITORY
    const char* name )          // - function name
{
    REPO_REC_FUN* curr;         // - current entry
    REPO_REC_FUN* retn;         // - entry for function

    retn = NULL;
    RingIterBeg( funRepository, curr ) {
        if( 0 == strcmp( name, curr->name ) ) {
            retn = curr;
            break;
        }
    } RingIterEnd( curr );
    return retn;
}


void RepoFunRelease(            // RELEASE FUNCTION RECORD
    REPO_REC_FUN* rec )         // - record to be released
{
    rec = rec;
}

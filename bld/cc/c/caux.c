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
* Description:  Management of #pragma aux information.
*
****************************************************************************/


#include "cvars.h"
#include "pragdefn.h"
#include "pdefn2.h"


struct aux_entry *AuxLookup( char *name )
{
    struct aux_entry    *ent;

    for( ent = AuxList; ent; ent = ent->next ) {
        if( strcmp( ent->name, name ) == 0 ) {
            break;
        }
    }
    return( ent );
}


static void FreeAuxInfo( struct aux_info *info )
{
    if( info->code != NULL ) {
        CMemFree( info->code );
        info->code = NULL;
    }
    if( !IsAuxParmsBuiltIn( info->parms ) ) {
        CMemFree( info->parms );
        info->parms = NULL;
    }
#if _CPU == 370
    if( info->linkage != &DefaultLinkage && info->linkage != &OSLinkage ) {
        if( info->linkage != NULL ) {
            CMemFree( info->linkage );
            info->linkage = NULL;
        }
    }
#endif
    if( info->objname != NULL ) {
        CMemFree( info->objname );
        info->objname = NULL;
    }
}

void PragmaAuxInit( void )
{
    WatcallInfo.use = 2;        /* so they don't get freed */

    CdeclInfo   = WatcallInfo;
    PascalInfo  = WatcallInfo;
    SyscallInfo = WatcallInfo;
    StdcallInfo = WatcallInfo;
    OptlinkInfo = WatcallInfo;
    FortranInfo = WatcallInfo;
    FastcallInfo= WatcallInfo;

#if _INTEL_CPU
    PragmaAuxInfoInit( CompFlags.use_stdcall_at_number );
#endif

    SetAuxDefaultInfo();
}

void PragmaAuxFini( void )
{
    void    *junk;

    while( AuxList != NULL ) {
        junk = AuxList;
        if( AuxList->info != NULL ) {
            if( AuxList->info->use != 1 ) {
                AuxList->info->use--;
            } else {
                FreeAuxInfo( AuxList->info );
                if( !IsAuxInfoBuiltIn( AuxList->info ) ) {
                    CMemFree( AuxList->info );
                }
            }
        }
        AuxList = AuxList->next;
        CMemFree( junk );
    }

    FreeAuxInfo( &DefaultInfo );
    FreeAuxInfo( &WatcallInfo );
    FreeAuxInfo( &CdeclInfo );
    FreeAuxInfo( &PascalInfo );
    FreeAuxInfo( &SyscallInfo );
    FreeAuxInfo( &OptlinkInfo );
    FreeAuxInfo( &FortranInfo );
    FreeAuxInfo( &StdcallInfo );
    FreeAuxInfo( &FastcallInfo );
#if _CPU == 386
    FreeAuxInfo( &Far16CdeclInfo );
    FreeAuxInfo( &Far16PascalInfo );
#endif
}

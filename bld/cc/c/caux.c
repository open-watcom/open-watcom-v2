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


#include "cvars.h"
#include "pragdefn.h"
#include "pdefn2.h"
extern  void    CMemFree();


struct aux_entry *AuxLookup( char *name )
{
    struct aux_entry *ent;

    for( ent = AuxList; ent; ent = ent->next ) {
        if( strcmp( ent->name, name ) == 0 ) break;
    }
    return( ent );
}


local void FreeInfo( struct aux_info *info )            /* 18-aug-90 */
{
    if( info->code != NULL ) {
        CMemFree( info->code );
        info->code = NULL;
    }
    if( info->parms != NULL && info->parms != DefaultParms ) {
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


void PragmaFini()
{
    struct aux_entry    *next;
    void                *junk;

    next = AuxList;
    while( next != NULL ) {
        junk = next;
        if( next->info != NULL ) {
            if( next->info->use != 1 ) {
                next->info->use--;
            } else {
                FreeInfo( next->info );
                if( next->info != &DefaultInfo )  CMemFree( next->info );
            }
        }
        next = next->next;
        CMemFree( junk );
    }
    FreeInfo( &DefaultInfo );
    FreeInfo( &CdeclInfo );
    FreeInfo( &PascalInfo );
    FreeInfo( &SyscallInfo );
    FreeInfo( &OptlinkInfo );
    FreeInfo( &FortranInfo );
    AuxList = NULL;
    while( HeadLibs != NULL ) {
        junk = HeadLibs;
        HeadLibs = HeadLibs->next;
        CMemFree( junk );
    }
}


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


#include <string.h>

#include "plusplus.h"
#include "memmgr.h"
#include "ring.h"
#include "cginimps.h"

typedef struct imp_list IMP_LIST;
struct imp_list {
    IMP_LIST    *next;
    char        name[1];
};

static IMP_LIST *importRing;

static IMP_LIST *addNewImport( char *name )
{
    size_t len;
    IMP_LIST *new_import;

    len = strlen( name );
    new_import = CMemAlloc( sizeof( *new_import ) + len );
    strcpy( new_import->name, name );
    RingAppend( &importRing, new_import );
    return( new_import );
}

void CgInfoAddImport( char *name )
/********************************/
{
    addNewImport( name );
}

void *CgInfoImportNext( void *h )
/*******************************/
{
    return( RingStep( importRing, h ) );
}

char *CgInfoImportName( void *h )
/*******************************/
{
    return( ((IMP_LIST *)h)->name );
}

void CgInfoFreeImports( void )
/****************************/
{
    RingFree( &importRing );
}

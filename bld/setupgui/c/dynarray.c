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
* Description:  Dynamic array functions.
*
****************************************************************************/


#include "gui.h"
#include "setup.h"
#include "dynarray.h"

#define INCREMENT       10

void InitArray( void **array, size_t esize, array_info *info )
/*************************************************************/
{
    if( info->increment == 0 ) {
        info->increment = INCREMENT;
    }
    if( info->alloc == 0 ) {
        info->alloc = info->increment;
    }
    *array = GUIMemAlloc( esize * info->alloc );
    info->esize = esize;
    info->array = array;
}

bool BumpArray( array_info *info )
/********************************/
{
    if( ++info->num >= info->alloc ) {
        info->alloc = info->num + info->increment;
        *(info->array) = GUIMemRealloc( *(info->array), info->esize * info->alloc );
        if( *(info->array) == NULL ) return( FALSE );
    }
    return( TRUE );
}

bool BumpDownArray( array_info *info )
/************************************/
{
    if( info->alloc - info->num >= info->increment ) {
        info->alloc -= info->increment;
        *(info->array) = GUIMemRealloc( *(info->array), info->esize * info->alloc );
        if( *(info->array) == NULL ) return( FALSE );
    }
    return( TRUE );
}


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
#include "linkstd.h"
#include "alloc.h"
#include "virtmem.h"

typedef struct vmemblock {
    struct vmemblock *  next;
    struct vmemblock *  prev;
    char                mem[1];
} vmemblock;

static vmemblock *      VMemBlocks;

#define TINY_ALLOC_CUTOFF (2*1024)

extern void VirtMemInit( void )
/*****************************/
{
    VMemBlocks = NULL;
}

extern virt_mem AllocStg( unsigned long size )
/********************************************/
{
    vmemblock * ptr;

    if( size == 0 ) return 0;
    if( size < TINY_ALLOC_CUTOFF ) {
        _PermAlloc( ptr, size + sizeof(vmemblock) - 1 );
        ptr->next = ptr;
    } else {
        _ChkAlloc( ptr, size + sizeof(vmemblock) - 1 );
        ptr->prev = NULL;
        ptr->next = VMemBlocks;
        if( VMemBlocks != NULL ) {
            VMemBlocks->prev = ptr;
        }
        VMemBlocks = ptr;
    }
    return (virt_mem) ptr->mem;
}

extern void ReleaseInfo( virt_mem v )
/**********************************/
{
    vmemblock * ptr;

    if( v == 0 ) return;
    if( VMemBlocks == NULL ) return;
    ptr = (vmemblock *) (v - sizeof(vmemblock *) * 2);
    if( ptr->next == ptr ) return;
    if( ptr->prev == NULL ) {
        VMemBlocks = ptr->next;
        if( VMemBlocks != NULL ) {
            VMemBlocks->prev = NULL;
        }
    } else {
        ptr->prev->next = ptr->next;
        if( ptr->next != NULL ) {
            ptr->next->prev = ptr->prev;
        }
    }
    _LnkFree( ptr );
}

extern bool SwapOutVirt( void )
/*****************************/
{
    return( FALSE );
}

extern void FreeVirtMem( void )
/*****************************/
{
    FreeList( VMemBlocks );
    VMemBlocks = NULL;
}

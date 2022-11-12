/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Stubbed virtual memory support functions (no VM support).
*
****************************************************************************/


#include <string.h>
#include "drpriv.h"
#include "drleb128.h"


typedef struct alloc_struct {
    struct alloc_struct *next;
    char                data[1];
} alloc_struct;

static alloc_struct *AllocHead;       /* head of list of allocated chunks */

void DWRVMInit( void )
/********************/
{
    /* no action */
    AllocHead = NULL;
}

void DWRVMReset( void )
/*********************/
{
    /* nothing to do here */
}

drmem_hdl DWRVMAlloc( unsigned long len, int sect )
/*************************************************/
{
    alloc_struct *nChunk;

    if( len == 0 ) {
        return( 0 );
    }

    nChunk = (alloc_struct *)DWRALLOC( len - 1 + sizeof( alloc_struct ) );
    if( nChunk == NULL ) {
        DWREXCEPT( DREXCEP_OUT_OF_MMEM );
        return( 0 );
    }

    nChunk->next = AllocHead;
    AllocHead = nChunk;

    DWRSEEK( DWRCurrNode->file, sect, 0 );
    DWRREAD( DWRCurrNode->file, sect, nChunk->data, len );

    return( (drmem_hdl)nChunk->data );
}

bool DWRVMSectDone( drmem_hdl base, unsigned_32 size )
/****************************************************/
{
    alloc_struct    *walk;
    alloc_struct    **lnk;
    bool            ret;

    /* unused parameters */ (void)size;

    lnk = &AllocHead;
    ret = false;
    while( (walk = *lnk) != NULL ) {
        if( (drmem_hdl)walk->data == base ) {
            *lnk = walk->next;
            DWRFREE( walk );
            ret = true;
            break;
        }
        lnk = &walk->next;
    }
    return( ret );
}


void DWRVMDestroy( void )
/***********************/
{
    alloc_struct    *walk;
    alloc_struct    *prev;

    for( walk = AllocHead; walk != NULL; ) {
        prev = walk;
        walk = walk->next;
        DWRFREE( prev );
    }
    AllocHead = NULL;
}

bool DRSwap( void )
/*****************/
{
    // swap requests are ignored.

    return( false );
}

static uint_8 readLEB( void **h )
/*******************************/
{
    return( *(*(drmem_hdl *)h)++ );
}

int_64 DWRVMReadSLEB128( drmem_hdl *vmptr )
/*****************************************/
{
    return( SLEB128( (void **)vmptr, readLEB ) );
}

uint_64 DWRVMReadULEB128( drmem_hdl *vmptr )
/******************************************/
{
    return( ULEB128( (void **)vmptr, readLEB ) );
}

#if 0
static void DWRVMGetString( char *buf, drmem_hdl *hdlp )
/******************************************************/
{
    uint len;

    len = DWRVMStrLen( *hdlp ) + 1;
    memcpy( buf, *hdlp, len );
    *hdlp += len;
}
#endif

char *DWRVMCopyString( drmem_hdl *info )
/**************************************/
{
    size_t      len;
    char        *dst;
    drmem_hdl   src;

    src = *info;
    len = strlen( src ) + 1;
    *info += len;
    dst = DWRALLOC( len );
    return( memcpy( dst, src, len ) );
}

size_t DWRVMGetStrBuff( drmem_hdl str, char *buf, size_t max )
/************************************************************/
{
    size_t      len;

    len = 0;
    for( ;; ) {
        if( len < max ) {
           *buf++ = *str;
        }
        ++len;
        if( *str == '\0' )
            break;
        ++str;
    }
    return( len );
}

unsigned_16 DWRVMReadWord( drmem_hdl hdl )
/****************************************/
{
    unsigned_16     word = *((unsigned_16 _WCUNALIGNED *)(hdl));

    if( DWRCurrNode->byte_swap ) {
        SWAP_16( word );
    }
    return( word );
}

unsigned_32 DWRVMReadDWord( drmem_hdl hdl )
/*****************************************/
{
    unsigned_32    dword = *((unsigned_32 _WCUNALIGNED *)(hdl));

    if( DWRCurrNode->byte_swap ) {
        SWAP_32( dword );
    }
    return( dword );
}

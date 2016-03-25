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
* Description:  Check DOS memory blocks for consistency.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <i86.h>
#include "bool.h"
#include "tinyio.h"
#include "doschk.h"
#include "doschkx.h"


#define MEMORY_BLOCK        'M'
#define END_OF_CHAIN        'Z'
#define MCB_PTR(curr)       ((dos_mem_block __based( curr ) *)0)
#define NEXT_MCB(curr)      (curr + MCB_PTR( curr )->size + 1)

#include "pushpck1.h"
typedef struct {
    char            chain;  /* 'M' memory block, 'Z' is last in chain */
    unsigned short  owner;  /* 0x0000 ==> free, otherwise psp address */
    unsigned short  size;   /* in paragraphs, not including header  */
} dos_mem_block;
#include "poppck.h"

static dos_mem_block saveMem;
static __segment     savePtrMem;
static __segment     savePtrChk;

bool CheckPointMem( where_parm where, unsigned max, char *f_buff )
{
    __segment       mem;
    __segment       start;
    __segment       end;
    __segment       next;
    __segment       chk;
    unsigned        size;
    unsigned        psp;

    if( max == 0 )
        return( false );
    psp = TinyGetPSP();
    for( start = psp - 1; MCB_PTR( start )->owner == psp; start = NEXT_MCB( start ) ) {
        if( MCB_PTR( start )->chain == END_OF_CHAIN ) {
            return( false );
        }
    }
    for( mem = start; ; mem = NEXT_MCB( mem ) ) {
        if( MCB_PTR( mem )->owner == 0 && MCB_PTR( mem )->size >= max )
            return( false );
        if(  MCB_PTR( mem )->chain == END_OF_CHAIN ) {
            break;
        }
    }
    end = NEXT_MCB( mem );
    size = end - start;
    if( size < 0x1000 )
        return( false );

    if( size > max )
        size = max;
    chk = end - size - 1;
    for( mem = start; ; mem = next ) {
        next = NEXT_MCB( mem );
        if( next > chk ) {
            break;
        }
    }

    savePtrMem = mem;
    savePtrChk = chk;
    _fmemcpy( &saveMem, MCB_PTR( mem ), sizeof( dos_mem_block ) );

    if( !XchkOpen( where, f_buff ) ) {
        return( false );
    }
    for( next = chk; next < end; next += size ) {
        size = end - next;
        if( !XchkWrite( where, next, &size ) ) {
            XcleanUp( where );
            return( false );
        }
    }
    XchkClose( where );

    MCB_PTR( mem )->chain = MEMORY_BLOCK;
    MCB_PTR( mem )->size = chk - mem - 1;
    MCB_PTR( chk )->size = end - chk - 1;
    MCB_PTR( chk )->chain = END_OF_CHAIN;
    MCB_PTR( chk )->owner = 0;
    return( true );
}

void CheckPointRestore( where_parm where )
{
    __segment       chk;

    _fmemcpy( MCB_PTR( savePtrMem ), &saveMem, sizeof( dos_mem_block ) );

    chk = savePtrChk;
    if( !XchkOpen( where, NULL ) ) {
        while( XchkRead( where, &chk ) )
            ;
        XcleanUp( where );
    }
}

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


/*
 *  STRTAB : routines for creating string tables.
 *
*/

#include "linkstd.h"
#include "strtab.h"
#include "alloc.h"
#include "ring.h"
#include "msg.h"
#include "wlnkmsg.h"
#include <string.h>

#define STR_BLOCK_SIZE   (4*1024)

typedef struct stringblock {
    STRINGBLOCK *next;
    unsigned    size;
    char        data[STR_BLOCK_SIZE];
} stringblock;

static stringblock * AllocNewBlock( stringtable *strtab )
/*******************************************************/
{
    stringblock *blk;

    _ChkAlloc( blk, sizeof(stringblock) );
    blk->next = NULL;
    RingAppend( &strtab->data, blk );
    blk->size = 0;
    return blk;
}

extern void InitStringTable( stringtable *strtab, bool dontsplit )
/****************************************************************/
{
    strtab->data = NULL;
    if( dontsplit ) {
        strtab->currbase = 1;
    } else {
        strtab->currbase = 0;
    }
    AllocNewBlock( strtab );
}

extern void CharStringTable( stringtable *strtab, char data )
/***********************************************************/
{
    AddStringTable( strtab, &data, sizeof(char) );
}

extern char * StringStringTable( stringtable *strtab, char *data )
/****************************************************************/
{
    return AddStringTable( strtab, data, strlen(data) + 1 );
}

extern void ReserveStringTable( stringtable *strtab, unsigned len )
/*****************************************************************/
{
    stringblock *blk;
    unsigned    diff;

    blk = RingLast( strtab->data );
    if( blk->size + len > STR_BLOCK_SIZE && strtab->currbase & 1 ) {
        diff = STR_BLOCK_SIZE - blk->size;
        if( diff != 0 ) {
            memset( &blk->data[blk->size], 0, diff );
        }
        blk->size = STR_BLOCK_SIZE;
        strtab->currbase += STR_BLOCK_SIZE;
        blk = AllocNewBlock( strtab );
    }
}

extern char * AddStringTable( stringtable *strtab, char *data, unsigned len )
/***************************************************************************/
{
    stringblock *blk;
    unsigned    diff;
    char *      dest;

    blk = RingLast( strtab->data );
    if( strtab->currbase & 1 && len > STR_BLOCK_SIZE ) {
        LnkMsg( ERR+MSG_SYMBOL_NAME_TOO_LONG, "s", data );
        len = STR_BLOCK_SIZE;
    }
    while( blk->size + len > STR_BLOCK_SIZE ) {
        diff = STR_BLOCK_SIZE - blk->size;
        if( diff != 0 ) {
            if( strtab->currbase & 1 ) {        // then don't split
                memset( &blk->data[blk->size], 0, diff );
            } else {
                memcpy( &blk->data[blk->size], data, diff );
                len -= diff;
                data += diff;
            }
        }
        blk->size = STR_BLOCK_SIZE;
        strtab->currbase += STR_BLOCK_SIZE;
        blk = AllocNewBlock( strtab );
    }
    dest = &blk->data[blk->size];
    memcpy( dest, data, len );
    blk->size += len;
    return dest;
}

extern void ZeroStringTable( stringtable *strtab, unsigned len )
/**************************************************************/
{
    stringblock *blk;

    blk = RingLast( strtab->data );
    DbgAssert( blk->size + len <= STR_BLOCK_SIZE );
    memset( &blk->data[blk->size], 0, len );
    blk->size += len;
}

typedef struct {
    void (*fn)(void *, char *, unsigned);
    void * info;
} strblkparam;

static bool WriteStringBlock( stringblock *blk, strblkparam *param )
/******************************************************************/
{
    param->fn( param->info, blk->data, blk->size );
    return FALSE;
}

extern void WriteStringTable( stringtable *strtab,
                              void (*fn)(void *, char *,unsigned), void *info )
/*****************************************************************************/
{
    strblkparam param;

    param.fn = fn;
    param.info = info;
    RingLookup( strtab->data, WriteStringBlock, &param );
}

extern void FiniStringTable( stringtable *strtab )
/************************************************/
{
    RingFree( &strtab->data );
}

extern unsigned GetStringTableSize( stringtable *strtab )
/*******************************************************/
{
    stringblock *blk;

    blk = RingLast( strtab->data );
    return blk->size + (strtab->currbase & ~1);
}

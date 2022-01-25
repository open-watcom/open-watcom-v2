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
* Description:  linker spill file i/o  routines
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "ideentry.h"
#include "spillio.h"


static virt_mem_size    tmp_filesize;
static FILE             *tmp_fp;

static void tmp_IOerror( const char *str )
/****************************************/
{
    if( str == NULL )
        str = strerror( errno );
    LnkMsg( ERR+MSG_IO_PROBLEM, "12", "Spill temporary file", str );
}

void InitSpillFile( void )
/************************/
{
    tmp_fp = NULL;
    tmp_filesize = 0;
    SetBreak();
}

virt_mem_size SpillAlloc( virt_mem_size amt )
/*******************************************/
{
    virt_mem_size           stg;

    if( tmp_fp == NULL ) {
        tmp_fp = tmpfile();
        if( tmp_fp == NULL ) {
            LnkMsg( FTL+MSG_CANT_OPEN_SPILL, NULL );
        }
        LnkMsg( INF+MSG_USING_SPILL, NULL );
    }
    /* round up storage start to a disk sector boundry -- assumed power of 2 */
    tmp_filesize = ROUND_UP( tmp_filesize, SECTOR_SIZE );
    stg = tmp_filesize;
    tmp_filesize += amt;
    return( stg );
}

static int tmp_seek( long offset )
/********************************/
{
    int     rc;

    rc = fseek( tmp_fp, offset, SEEK_SET );
    if( rc ) {
        tmp_IOerror( NULL );
    }
    return( rc );
}

static size_t tmp_read( void *buffer, size_t len )
/************************************************/
{
    size_t  rc;

    if( len == 0 )
        return( 0 );

    CheckBreak();
    rc = fread( buffer, 1, len, tmp_fp );
    if( rc != len ) {
        if( ferror( tmp_fp ) ) {
            tmp_IOerror( NULL );
        }
    }
    return( rc );
}

static size_t tmp_write( const void *buffer, size_t len )
/*******************************************************/
{
    size_t  rc;
    char    rc_buff[RESOURCE_MAX_SIZE];

    if( len == 0 )
        return( 0 );
    CheckBreak();
    rc = fwrite( buffer, 1, len, tmp_fp );
    if( rc != len ) {
        if( ferror( tmp_fp ) ) {
            tmp_IOerror( NULL );
        } else {
            Msg_Get( MSG_IOERRLIST_7, rc_buff );
            tmp_IOerror( rc_buff );
        }
    }
    return( rc );
}

static void tmp_write_nulls( size_t len )
/***************************************/
{
    static unsigned NullArray[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for( ; len > sizeof( NullArray ); len -= sizeof( NullArray ) ) {
        tmp_write( NullArray, sizeof( NullArray ) );
    }
    if( len > 0 ) {
        tmp_write( NullArray, len );
    }
}

void SpillNull( virt_mem_size base, size_t off, size_t size )
/***********************************************************/
{
    tmp_seek( base + (unsigned long)off - 1 );
    tmp_write_nulls( size );
}

void SpillWrite( virt_mem_size base, size_t off, const void *mem, size_t size )
/*****************************************************************************/
{
    tmp_seek( base + (unsigned long)off - 1 );
    tmp_write( mem, size );
}

void SpillRead( virt_mem_size base, size_t off, void *mem, size_t size )
/**********************************************************************/
{
    tmp_seek( base + (unsigned long)off - 1 );
    tmp_read( mem, size );
}

void CloseSpillFile( void )
/*************************/
/*  Close temporary file.  */
{
    if( tmp_fp != NULL ) {
        RestoreBreak();
        fclose( tmp_fp );
        tmp_fp = NULL;
    }
}

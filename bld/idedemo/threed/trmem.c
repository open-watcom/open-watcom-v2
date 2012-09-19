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
 Description:
 ============
     This module defines the memory allocation functions which access
     the system independent allocation functions. It is built for
     Windows and OS/2 PM
*/

#include <stdlib.h>
#include <stdio.h>

#ifdef PLAT_OS2
#define INCL_PM
#include <os2.h>
#include <bsedos.h>
#else
#include <windows.h>
#endif
#include "wpi.h"

#include "dbgmem.h"
#define TRACK_MEM
#include "mem.h"

enum {
    DBG_MSG_CONT,
    DBG_MSG_NO_MORE
};

static int Lmem_msg = DBG_MSG_CONT;
static int Gmem_msg = DBG_MSG_CONT;

static void * DFAR      Lmem;
static void * DFAR      Gmem;

static WPI_INST         Inst;

static char Dbg_file_env[]="DBGFILE";

static void DFAR *local_lmem_alloc(
/*********************************/

    unsigned long       size
) {
    return( lmem_alloc( size ) );
}

static void local_lmem_free(
/**************************/

    void DFAR           *mem
) {
    lmem_free( (void *)( mem ) );
}

static void DFAR * local_lmem_realloc(
/************************************/

    void DFAR           *mem,
    unsigned long       size
) {
    return( lmem_realloc( (void *)mem, size ) );
}

void near * tr_lmem_alloc(
/************************/

    unsigned int        size
) {
    return( (void near *)dbg_mem_alloc( Lmem, size ) );
}

void tr_lmem_free(
/****************/

    void                *mem
) {
    dbg_mem_free( Lmem, ((void near *)mem == NULL ) ? NULL : mem );
}

void near *tr_lmem_realloc(
/*************************/

    void                *mem,
    unsigned int        size
) {
    return( (void near *)dbg_mem_realloc( Lmem,
                ((void near *)mem == NULL ) ? NULL : mem, size ) );
}

static void DFAR *local_gmem_alloc(
/*********************************/

    unsigned long       size
) {
    return( gmem_alloc( size ) );
}

static void local_gmem_free(
/**************************/

    void DFAR           *mem
) {
    gmem_free( mem );
}

static void DFAR * local_gmem_realloc(
/************************************/

    void DFAR           *mem,
    unsigned long       size
) {
    return( gmem_realloc( mem, size ) );
}

void *tr_gmem_alloc(
/*******************/

    unsigned int        size
) {
    return( dbg_mem_alloc( Gmem, size ) );
}

void tr_gmem_free(
/****************/

    void                *mem
) {
    dbg_mem_free( Gmem, mem );
}

void *tr_gmem_realloc(
/********************/

    void                *mem,
    unsigned int        size
) {
    return( dbg_mem_realloc( Gmem, mem, size ) );
}

static dbg_level get_level(
/*************************/

    void
) {
    char                *env;
    dbg_level           level;

    env = getenv( "DBGMEM" );
    level = DBG_MEM_0;
    if( env != NULL ) {
        level = atoi( env );
        if( level >= DBG_MEM_MAX ) {
            level = DBG_MEM_MAX - 1;
        } else if( level < 0 ) {
            level = 0;
        }
    }

    return( level );
}

static void print_to_file(
/************************/

    char                *prefix,
    char                *line,
    int                 error
) {
    char                *env;
    FILE                *f;

    error = error;
    env = getenv( Dbg_file_env );
    if( env != NULL ) {
        f = fopen( env, "a" );
        if( prefix != NULL ) {
            fprintf( f, "%s: " );
        }
        fprintf( f, "%s\n", line );
        fclose( f );
    }
}

static void clear_file(
/*********************/

    void
) {
    char                *env;
    FILE                *f;

    env = getenv( Dbg_file_env );
    if( env != NULL ) {
        f = fopen( env, "w" );
        fclose( f );
    }
}

static void lmem_print_line(
/**************************/

    char                *line,
    int                 error
) {
    int                 ret;

    print_to_file( NULL, line, error );

    if( error ) {
        /* this is an error message. Display an appropriate message box,
           if the user wants it. */
        if( Lmem_msg == DBG_MSG_CONT ) {
            ret = _wpi_messagebox( NULL, line, "Local Memory Debug Tracker",
                            MB_SYSTEMMODAL | MB_YESNOCANCEL | MB_ICONHAND );
            if( ret == IDNO ) {
                Lmem_msg = DBG_MSG_NO_MORE;
            } else if( ret == IDCANCEL ) {
                exit( -1 );
            }
        }
    }
}

static void gmem_print_line(
/**************************/

    char                *line,
    int                 error
) {
    int                 ret;

    print_to_file( NULL, line, error );

    if( error ) {
        /* this is an error message. Display an appropriate message box,
           if the user wants it. */
        if( Gmem_msg == DBG_MSG_CONT ) {
            ret = _wpi_messagebox( NULL, line, "Global Memory Debug Tracker",
                            MB_SYSTEMMODAL | MB_YESNOCANCEL | MB_ICONHAND );
            if( ret == IDNO ) {
                Gmem_msg = DBG_MSG_NO_MORE;
            } else if( ret == IDCANCEL ) {
                exit( -1 );
            }
        }
    }
}

void tr_mem_open(
/***************/

    WPI_INST            inst,
    char                *title
) {
    dbg_info            info;

    Inst = inst;

    info.alloc = local_lmem_alloc;
    info.free = local_lmem_free;
    info.realloc = local_lmem_realloc;
    info.level = get_level();
    info.prt_line = lmem_print_line;
    sprintf( info.rpt_title, "%s-Local Memory", title );
    Lmem = dbg_mem_init( &info );

    info.alloc = local_gmem_alloc;
    info.free = local_gmem_free;
    info.realloc = local_gmem_realloc;
    info.level = get_level();
    info.prt_line = gmem_print_line;
    sprintf( info.rpt_title, "%s-Global Memory", title );
    Gmem = dbg_mem_init( &info );

    clear_file();
}

void tr_mem_close(
/****************/

    void
) {
    if( dbg_mem_report( Lmem ) > 0 ) {
        dbg_mem_line( Lmem, "Unfreed chunks. See the report file", TRUE );
    }

    if( dbg_mem_report( Gmem ) > 0 ) {
        dbg_mem_line( Gmem, "Unfreed chunks. See the report file", TRUE );
    }

    dbg_mem_close( Lmem );
    dbg_mem_close( Gmem );
}

void tr_mem_report(
/*****************/

    void
) {
    dbg_mem_report( Lmem );
    dbg_mem_report( Gmem );
}

void tr_mem_check(
/****************/

    void
) {
    dbg_mem_check( Lmem );
    dbg_mem_check( Gmem );
}

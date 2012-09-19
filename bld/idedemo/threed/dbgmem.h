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


#ifndef DBGMEM_H
#define DBGMEM_H
/*
 Description:
 ============
     This is a system indepedent debug memory package. Use it when
     for modules using dbgmem.c
*/

#define DFAR

typedef enum {
    DBG_MEM_0,      // no debug facility: just use plain allocs and frees
    DBG_MEM_1,      // debugging info in, no checks on each free and alloc
    DBG_MEM_2,      // simple checks on each free and alloc
    DBG_MEM_3,      // complete memory check on each free and alloc
    DBG_MEM_MAX
} dbg_level;

typedef struct {
    void                DFAR *(*alloc)( unsigned long );
    void                (*free)( void DFAR * );
    void                DFAR *(*realloc)( void DFAR *, unsigned long );
    void                (*prt_line)( char * line, int error );
    dbg_level           level;
    char                rpt_title[100];
} dbg_info;

#ifndef DBG_INTERNAL
#define dbg_list void

extern int dbg_mem_check( dbg_list DFAR *mem_ctl );
extern int dbg_mem_report( dbg_list DFAR *mem_ctl );
extern dbg_list DFAR *dbg_mem_init( dbg_info *info );
extern void dbg_mem_close( void DFAR *mem_ctl );
extern void dbg_mem_set_level( void DFAR *mem_ctl, dbg_level level );
extern void DFAR *dbg_mem_alloc( void DFAR *mem_ctl, unsigned long size );
extern void dbg_mem_free( void DFAR *mem_ctl, void DFAR *mem );
extern void DFAR *dbg_mem_realloc( void DFAR *mem_ctl, void DFAR *mem, unsigned long size );
extern void dbg_mem_line( void DFAR *mem_ctl, char *msg, int err );

#endif


#endif

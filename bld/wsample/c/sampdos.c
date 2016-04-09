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
* Description:  DOS performance sampling core.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <dos.h>
#include <malloc.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "wmsg.h"

#include "ovldbg.h"


typedef struct {
    struct samp_block_prefix    pref;
    struct samp_ovl_load        ovl;
} overlay_record_t;

typedef struct pblock {
    seg         envstring;
    seg_offset  commandln;
    seg_offset  fcb01;
    seg_offset  fcb02;
    seg_offset  startsssp;
    seg_offset  startcsip;
} pblock;

typedef struct {
    void __far  *addr;
    uint_16     sect;
} ovl_addr;

typedef void __based( __segname( "_CODE" ) ) (*report_fn_ptr)( void );

extern bool     FirstSample;

extern seg_offset __far SysCallerAddr;
extern unsigned char __far SysCaught;
extern unsigned char __far SysNoDOS;

extern void StartTimer( void );
extern void InstallDOSIntercepts( void );
extern void StopTimer( void );
extern void RemoveDOSIntercepts( void );
extern int AddInterrupt( unsigned );

extern seg                  GetPSP( void );
extern void                 DOSLoadProg( char *, pblock *, report_fn_ptr );
extern void                 DOSRunProg( seg_offset * );
extern ovl_dbg_hook_func    ovl_handler;

char                FAR_PTR *CommonAddr = NULL;
unsigned            OvlSize;
overlay_record_t    FAR_PTR *OvlStruct;
ovl_dbg_req_func    *OvlHandler;

int                 FirstHook = TRUE;

void WriteOvl( unsigned req_ovl, char is_return, unsigned offset, unsigned seg )
{
    overlay_record_t    FAR_PTR *ovl;
    struct {
        struct samp_block_prefix    pref;
        struct samp_remap_sect      remap;
    } remap_blk;
    ovl_addr            xlat_addr;
    unsigned            size;

    SamplerOff++;

    if( FirstHook ) {
        FirstHook = FALSE;
        req_ovl = 0;    /* first req_ovl is garbage */
        SamplerOff--;   /* start sampling after this routine */
    }

    size =  ( sizeof( overlay_record_t ) - 1 ) + OvlSize;
    ovl = OvlStruct;

    ovl->pref.tick = CurrTick;
    ovl->pref.length = size;
    ovl->pref.kind = SAMP_OVL_LOAD;
    ovl->ovl.req_section = req_ovl;
    if( is_return ) {
        ovl->ovl.req_section |= OVL_RETURN;
    }
    ovl->ovl.addr.segment = seg;
    ovl->ovl.addr.offset = offset-1;
    Info.d.count[SAMP_OVL_LOAD].size += size;
    Info.d.count[SAMP_OVL_LOAD].number += 1;
    OvlHandler( OVLDBG_GET_OVERLAY_STATE, ovl->ovl.ovl_map );
    SampWrite( ovl, ovl->pref.length );
    /* find out what overlays moved */
    remap_blk.pref.tick = CurrTick;
    remap_blk.pref.length = sizeof( remap_blk );
    remap_blk.pref.kind = SAMP_REMAP_SECTION;
    xlat_addr.sect = 0;
    for( ;; ) {
        if( !OvlHandler( OVLDBG_GET_MOVED_SECTION, &xlat_addr ) )
            break;
        remap_blk.remap.data[0].section = xlat_addr.sect;
        remap_blk.remap.data[0].segment = FP_SEG( xlat_addr.addr );
        Info.d.count[SAMP_REMAP_SECTION].size += remap_blk.pref.length;
        Info.d.count[SAMP_REMAP_SECTION].number += 1;
        SampWrite( &remap_blk, remap_blk.pref.length );
    }
    SamplerOff--;
}

void StopProg( void )
{
    StopTimer();
    RemoveDOSIntercepts();
}

void StartProg( char *cmd, char *prog, char *full_args, char *dos_args )
{
    struct  SREGS       segs;
    seg_offset          ovl_tbl;
    struct ovl_header   __far *ovl;
    pblock              parms;
    overlay_record_t    FAR_PTR *ovl_struct;

    cmd = cmd;
    InstallDOSIntercepts();
    StartTimer();
    segread( (struct SREGS *)&segs );
    parms.envstring = 0;
    parms.commandln.segment = segs.ss;
    parms.commandln.offset = (off) dos_args;
    parms.fcb01.segment = GetPSP();
    parms.fcb01.offset  = 0x5c; /* formatted parameter area 1 (FCB) */
    parms.fcb02.segment = parms.fcb01.segment;
    parms.fcb02.offset  = 0x6c; /* formatted parameter area 2 (FCB) */
    DOSLoadProg( prog, &parms, (report_fn_ptr)report );
    /* Will load prog to memory ready to DOSRunProg.
       Will also cause prog to chain to report after its execution.
    */

    ovl_tbl.segment = 0;
    ovl_tbl.offset  = 0;

    ovl = MK_FP( parms.startcsip.segment, parms.startcsip.offset );
    if( ovl->signature == OVL_SIGNATURE ) {
        OvlHandler = MK_FP( parms.startcsip.segment, ovl->handler_offset );
        OvlSize = OvlHandler( OVLDBG_GET_STATE_SIZE, NULL );
        ovl_struct = alloca( ( sizeof( overlay_record_t ) - 1 ) + OvlSize );
        if( ovl_struct == NULL ) {
            Output( MsgArray[MSG_SAMPLE_1 - ERR_FIRST_MESSAGE] );
            Output( "\r\n" );
            fatal();
        }
        OvlStruct = ovl_struct;
        OvlHandler( OVLDBG_GET_OVL_TBL_ADDR, &ovl_tbl );
        ovl->hook = &ovl_handler;
        SamplerOff++;   /* don't time overlay initialization */
    }
    WriteCodeLoad( ovl_tbl, ExeName, SAMP_MAIN_LOAD );
    WriteAddrMap( 0, GetPSP() + 0x10, 0  );
    SampleIndex = 0;
    SampleCount = 0;
    CurrTick  = 0L;
    SysCaught = 0;
    FirstSample = TRUE;

    DOSRunProg( &parms.startsssp );
}

void RecordSample( union INTPACK FAR_PTR *r )
{
    seg segment;
    off offset;

    if( SysCaught ) {
        offset = SysCallerAddr.offset;
        segment = SysCallerAddr.segment;
    } else {
        offset = r->x.ip;
        segment = r->x.cs;
    }
    Samples->d.sample.sample[SampleIndex].offset = offset;
    Samples->d.sample.sample[SampleIndex].segment = segment;
    LastSampleIndex = SampleIndex;
    ++SampleIndex;
    ++SampleCount;
    if( CallGraphMode ) {
        RecordCGraph();
    }
}

void GetCommArea( void )
{
    if( CommonAddr == NULL ) {  /* can't get the common region yet */
        Comm.cgraph_top = 0;
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;       /* don't record this sample */
    } else {
        _fmemcpy(&Comm, CommonAddr, sizeof( Comm ) );
    }
}

void GetNextAddr( void )
{
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;

    if( CommonAddr == NULL ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        _fmemcpy( &stack_entry, (void FAR_PTR *)Comm.cgraph_top, sizeof( stack_entry ) );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}

void RememberComm( char FAR_PTR *address )
{
    CommonAddr = address;
}

void ResetCommArea( void )
{
    if( CommonAddr != NULL ) {  /* reset common variables */
        Comm.pop_no = 0;
        Comm.push_no = 0;
        _fmemcpy( CommonAddr + 9, &Comm.pop_no, 4 );
    }
}


void SysDefaultOptions( void )
{
    SysNoDOS = 0;
}


void SetInterruptWatch( char **cmd )
{
    unsigned intr_num;

    intr_num = GetNumber( 0x20, 0xff, cmd, 16 );
    if( ( intr_num >= 0x34 ) && ( intr_num <= 0x3d ) ) {
        Output( MsgArray[MSG_SAMPLE_2 - ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
    if( intr_num != 0x21 ) {    /* the DOS interrupt is already monitored */
        if( AddInterrupt( intr_num ) ) {
            Output( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );
            Output( "\r\n" );
            fatal();
        }
    }
}


void SysParseOptions( char c, char **cmd )
{
    char buff[2];

    switch( c ) {
    case 'r':
        SetTimerRate( cmd );
        break;
    case 'i':
        SetInterruptWatch( cmd );
        break;
    case 'd':
        SysNoDOS = 1;
        break;
    default:
        Output( MsgArray[MSG_INVALID_OPTION - ERR_FIRST_MESSAGE] );
        buff[0] = c;
        buff[1] = '\0';
        Output( buff );
        Output( "\r\n" );
        fatal();
        break;
    }
}

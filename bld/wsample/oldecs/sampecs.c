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


#include "sample.h"
#include "smpstuff.h"
#include "wmsg.h"
#include "ecsproto.h"
#include "os386.h"
#include <dos.h>

extern void SetTimerRate( char ** );
extern void fatal( void );
extern void Output( char FAR_PTR *str );
extern void StartTimer();
extern void InstallDOSIntercepts();
extern void StopTimer();
extern void REPORT_TYPE report();
extern void RemoveDOSIntercepts();
extern void WriteCodeLoad( seg_offset ovl_tbl, char *name, samp_block_kinds );
extern void WriteAddrMap( seg map_start,  seg load_start, off load_offset );
extern void WriteMark( char FAR_PTR *str, seg_offset where );
extern void RecordCGraph( void );
extern char  FAR_PTR    *MsgArray[ERR_LAST_MESSAGE-ERR_FIRST_MESSAGE+1];

extern seg_offset FAR * FAR OS386_Get_Ptr_to_Intr();

seg_offset CommonAddr = { 0, 0 };
seg_offset NoAddr = { 0xBEEFEAD0, 0xDEAD };
seg_offset                      FAR *IntrPtr = &NoAddr;
BITS                            TaskID;
BITS                            TSSSel;
TSS_386                         Mach;

extern unsigned char far SysNoDos;
extern short _PSP;

#pragma off(unreferenced);
void WriteOvl( unsigned req, char is_return, unsigned offset, unsigned seg ) {
#pragma on(unreferenced);
}

StopProg()
{
}

static int DBErrors[] = {

        1,
        2,
        3,
        4,
        5,
        6,
        7,
        0,
        0,
        0,
        8,
        9,
        10,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        11,
        12,
        13,
        14,
        0,
        0,
        0,
        0,
        0,
        0,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        0,
        24,
        25,
        26,

        };


#include "exceptv.h"

int BadRC( int rc ) {

    if( rc != R_EXIT && rc != 0 ) {
        if( rc > 0x110 ) {
            Output( MsgArray[MSG_SAMPLE_1-ERR_FIRST_MESSAGE] );
            Output( "\r\n" );
        } else if( rc >= 0x100 ) {
            Output( MsgArray[MSG_SAMPLE_2-ERR_FIRST_MESSAGE] );
            Output( MsgArray[Exceptions[rc-0x100]+MSG_EXCEPT_0-ERR_FIRST_MESSAGE] );
            Output( "\r\n" );
            return( 1 );
        } else {
            Output( MsgArray[DBErrors[rc]+MSG_DBERROR_0-ERR_FIRST_MESSAGE] );
            Output( "\r\n" );
            return( 1 );
        }
    }
    return( 0 );
}


#define BSIZE   256

StartProg( char *cmd, char *prog, char *args )
{
    seg_offset          where;
    int                 rc;
    char                buff[BSIZE];
    int                 len;
    char                far *env;

    --args;
    args[ 0 ] = args[ 1 ];
    args[ 1 ] = 0;
    cmd = cmd;
    rc = OS386_Init_Machine( 0 );
    if( BadRC( rc ) ) return;
    env = MK_FP( *(short far *)MK_FP( _PSP, 0x2C ), 0 );
    rc = OS386_Create_Task( prog, 0, args, env, &TaskID );
    if( BadRC( rc ) ) return;
    TSSSel = OS386_Get_TSS( TaskID );
    rc = OS386_Read_Protected_Mem( TaskID, TSSSel, 0, sizeof( Mach ), &Mach );
    if( BadRC( rc ) ) return;
    where.segment = 0;
    where.offset  = 0;
    WriteCodeLoad( where, ExeName, SAMP_MAIN_LOAD );
    WriteAddrMap( 1, Mach.cs, 0  );
    SampleIndex = 0;
    SampleCount = 0;
    CurrTick  = 0L;
    InstallDOSIntercepts();
    StartTimer();
    IntrPtr = OS386_Get_Ptr_to_Intr();
    for( ;; ) {
        rc = OS386_Task_Control( TaskID, T_GO );
        if( rc != R_BP && rc != 256+3 ) break;
        OS386_Read_Protected_Mem( TaskID, TSSSel, 0, sizeof(Mach), &Mach );
        if(( Mach.edx & 0xffff ) != 0 ) {               /* this is a mark */
            len = 0;
            for( ;; ) {
                rc = OS386_Read_Protected_Mem(TaskID,Mach.edx,Mach.eax,1,buff+len);
                if( rc != 0 ) buff[ len ] = '\0';
                if( len == BSIZE ) buff[ len ] = '\0';
                if( buff[ len ] == '\0' ) break;
                ++len;
                Mach.eax++;
            }
            where.segment = Mach.cs;
            where.offset = Mach.eip;
            WriteMark( buff, where );
        } else {                /* we're being passed CommonAddr */
            CommonAddr.segment = Mach.ecx & 0xffff;
            CommonAddr.offset = Mach.ebx;
        }
    }
    if( !BadRC( rc ) ) {
        rc = OS386_Task_Control( TaskID, T_KILL );
        BadRC( rc );
    }
    StopTimer();
    RemoveDOSIntercepts();
    report();
}

#pragma off(unreferenced);
void RecordSample( void FAR_PTR *r ) {
#pragma on(unreferenced);
        LastSampleIndex = SampleIndex;
        Samples->d.sample.sample[ SampleIndex ].offset = IntrPtr->offset;
        Samples->d.sample.sample[ SampleIndex ].segment = IntrPtr->segment;
        ++SampleIndex;
        ++SampleCount;
        if( CallGraphMode ) {
            RecordCGraph();
        }
}


void GetCommArea( void )
{
    if( CommonAddr.segment == 0 ) {     /* can't get the common region yet */
        Comm.cgraph_top = 0;
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;       /* don't record this sample */
    } else {
        OS386_Read_Protected_Mem( TaskID, CommonAddr.segment,
                                CommonAddr.offset, sizeof( Comm ), &Comm );
    }
}


void ResetCommArea( void )
{
    if( CommonAddr.segment != 0 ) {     /* reset common variables */
        Comm.pop_no = 0;
        Comm.push_no = 0;
        CommonAddr.offset += 11;
        OS386_Write_Protected_Mem( TaskID, CommonAddr.segment,
                                 CommonAddr.offset + 10, 4, &Comm.pop_no );
        CommonAddr.offset -= 11;
    }
}


void GetNextAddr( void )
{
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;

    if( CommonAddr.segment == 0 ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        OS386_Read_Protected_Mem( TaskID, CommonAddr.segment,
                 Comm.cgraph_top, sizeof( stack_entry ), &stack_entry );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}

void RememberComm( void )
{
}

void SysDefaultOptions( void )
{
    SysNoDos = 1;
}


void SysParseOptions( char c, char **cmd )
{
    char buff[2];

    if( c != 'r' ) {
        Output( MsgArray[MSG_INVALID_OPTION-ERR_FIRST_MESSAGE] );
        buff[0] = c;
        buff[1] = '\0';
        Output( buff );
        Output( "\r\n" );
        fatal();
    }
    SetTimerRate( cmd );
}

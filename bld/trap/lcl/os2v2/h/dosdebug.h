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


typedef struct  {
/*  0 */unsigned long   Pid;        /* Debuggee Process ID */
/*  4 */unsigned long   Tid;        /* Debuggee Thread ID */
/*  8 */long            Cmd;        /* Command or Notification */
/*  C */long            Value;      /* Generic Data Value */
/* 10 */unsigned long   Addr;       /* Debuggee Address */
/* 14 */unsigned long   Buffer;     /* Debugger Buffer Address */
/* 18 */unsigned long   Len;        /* Length of Range */
/* 1C */unsigned long   Index;      /* Generic Identifier Index */
/* 20 */unsigned long   MTE;        /* Module Table Entry Handle */
/* 24 */unsigned long   EAX;        /* Register Set */
/* 28 */unsigned long   ECX;
/* 2C */unsigned long   EDX;
/* 30 */unsigned long   EBX;
/* 34 */unsigned long   ESP;
/* 38 */unsigned long   EBP;
/* 3C */unsigned long   ESI;
/* 40 */unsigned long   EDI;
/* 44 */unsigned long   EFlags;
/* 48 */unsigned long   EIP;
/* 4C */unsigned long   CSLim;      /* Byte Granular Limits */
/* 50 */unsigned long   CSBase;     /* Byte Granular Base */
/* 54 */unsigned char   CSAcc;      /* Access Bytes */
/* 55 */unsigned char   CSAtr;      /* Attribute Bytes */
/* 56 */unsigned short  CS;
/* 58 */unsigned long   DSLim;
/* 5C */unsigned long   DSBase;
/* 60 */unsigned char   DSAcc;
/* 61 */unsigned char   DSAtr;
/* 62 */unsigned short  DS;
/* 64 */unsigned long   ESLim;
/* 68 */unsigned long   ESBase;
/* 6C */unsigned char   ESAcc;
/* 6D */unsigned char   ESAtr;
/* 6E */unsigned short  ES;
/* 70 */unsigned long   FSLim;
/* 74 */unsigned long   FSBase;
/* 78 */unsigned char   FSAcc;
/* 79 */unsigned char   FSAtr;
/* 7A */unsigned short  FS;
/* 7C */unsigned long   GSLim;
/* 80 */unsigned long   GSBase;
/* 84 */unsigned char   GSAcc;
/* 85 */unsigned char   GSAtr;
/* 86 */unsigned short  GS;
/* 88 */unsigned long   SSLim;
/* 8C */unsigned long   SSBase;
/* 90 */unsigned char   SSAcc;
/* 91 */unsigned char   SSAtr;
/* 92 */unsigned short  SS;
/* 94 */unsigned short  segv;
} dos_debug;

#define DBG_C_Null              0       /* Null */
#define DBG_C_ReadMem           1       /* Read Word */
#define DBG_C_ReadMem_I         1       /* Read Word */
#define DBG_C_ReadMem_D         2       /* Read Word (same as 1) */
#define DBG_C_ReadReg           3       /* Read Register Set */
#define DBG_C_WriteMem          4       /* Write Word */
#define DBG_C_WriteMem_I        4       /* Write Word */
#define DBG_C_WriteMem_D        5       /* Write Word (same as 4) */
#define DBG_C_WriteReg          6       /* Write Register Set */
#define DBG_C_Go                7       /* Go */
#define DBG_C_Term              8       /* Terminate */
#define DBG_C_SStep             9       /* Single Step */
#define DBG_C_Stop              10      /* Stop */
#define DBG_C_Freeze            11      /* Freeze Thread */
#define DBG_C_Resume            12      /* Resume Thread */
#define DBG_C_NumToAddr         13      /* Object Number to Address */
#define DBG_C_ReadCoRegs        14      /* Read Coprocessor Registers */
#define DBG_C_WriteCoRegs       15      /* Write Coprocessor Registers */
/* #define Reserved             16      PRICKS took this out */
#define DBG_C_ThrdStat          17      /* Get Thread Status */
#define DBG_C_MapROAlias        18      /* Map Read-Only Alias */
#define DBG_C_MapRWAlias        19      /* Map Read-Write Alias */
#define DBG_C_UnMapAlias        20      /* Unmap Alias */
#define DBG_C_Connect           21      /* Connect to Debuggee */
#define DBG_C_ReadMemBuf        22      /* Read Memory Buffer */
#define DBG_C_WriteMemBuf       23      /* Write Memory Buffer */
#define DBG_C_SetWatch          24      /* Set Watchpoint */
#define DBG_C_ClearWatch        25      /* Clear Watchpoint */
#define DBG_C_RangeStep         26      /* Range Step */
#define DBG_C_Continue          27      /* Continue (Set Exception State) */
#define DBG_C_AddrToObject      28      /* Get Memory Object Information */
#define DBG_C_XchngOpcode       29      /* Exchange Opcode, Step, and Go */
#define DBG_C_LinToSel          30      /* Translate Linear Address to Segment:Offset */
#define DBG_C_SelToLin          31      /* Translate Segment:Offset to Linear Address */

#define DBG_C_GetLibName        100     /* get library name */

#define DBG_N_Success           0       /* Successful command completion */
#define DBG_N_Error             -1      /* Error detected during command */
#define DBG_N_ProcTerm          -6      /* Process termination - DosExitList done */
#define DBG_N_Exception         -7      /* Exception detected */
#define DBG_N_ModuleLoad        -8      /* Module loaded */
#define DBG_N_CoError           -9      /* Coprocessor not in use error */
#define DBG_N_ThreadTerm        -10     /* Thread termination - not in DosExitList */
#define DBG_N_AsyncStop         -11     /* Async Stop detected */
#define DBG_N_NewProc           -12     /* New Process started */
#define DBG_N_AliasFree         -13     /* Alias needs to be freed */
#define DBG_N_Watchpoint        -14     /* Watchpoint hit */
#define DBG_N_ThreadCreate      -15     /* Thread creation */
#define DBG_N_ModuleFree        -16     /* Module freed */
#define DBG_N_RangeStep         -17     /* Range Step detected */

#define DBG_N_Breakpoint        -100
#define DBG_N_SStep             -101
#define DBG_N_Signal            -102

#define XCPT_DIV        1
#define XCPT_OVERFLOW   2
#define XCPT_BOUND      3
#define XCPT_OP         4
#define XCPT_GP         5
#define XCPT_FPERR      6
#define XCPT_GPF        7
#define XCPT_GPAF       8
#define XCPT_1X_PARITY  9

extern ULONG CallDosDebug( PVOID );
extern ULONG MakeFlatPointer( PVOID ptr );
int IsFlatSeg( int seg );

#define DBG_L_386       1

#define DBG_W_Global    0x00000001
#define DBG_W_Local     0x00000002
#define DBG_W_Execute   0x00010000
#define DBG_W_Write     0x00020000
#define DBG_W_ReadWrite 0x00030000

#define DBG_CO_387      1
#define DBG_CO_SIZE     108

#define DBG_X_PRE_FIRST_CHANCE  0
#define DBG_X_FIRST_CHANCE      1
#define DBG_X_LAST_CHANCE       2
#define DBG_X_STACK_INVALID     3

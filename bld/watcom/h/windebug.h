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


/* ptrace commands */
#define     DBG_C_Null                  0       // Null
#define     DBG_C_ReadMem               1       // Read Word
#define     DBG_C_ReadReg               3       // Read Register Set (unsupported)
#define     DBG_C_WriteMem              4       // Write Word
#define     DBG_C_WriteReg              6       // Write Register Set (unsupported)
#define     DBG_C_Go                    7       // Go
#define     DBG_C_Term                  8       // Terminate the debug session (unsupported)
#define     DBG_C_SStep                 9       // Single Step
#define     DBG_C_Stop                  10      // Stop
#define     DBG_C_NumToAddr             13      // Object Number to Address
#define     DBG_C_GetLibName            16      // Get Module Name
#define     DBG_C_Connect               21      // Connect to Debuggee
#define     DBG_C_Disconnect            28      // Disconnect from debuggee
/* windows3 specific */
#define     DBG_C_GetInfo              100      // Request global heap
#define     DBG_C_SendChar             101      // Used to respond to DBG_N_InfoReq
#define     DBG_C_HandleToAddr         102      // Used to request info about objects
#define     DBG_C_KillTask             104      // Terminate debugee

#define     DBG_SC_DumpGblHeap          1
#define     DBG_SC_DumpLocHeap          2
#define     DBG_SC_DumpModList          5
#define     DBG_SC_DumpQuit             -1

#define     DBG_SC_GlobalHandle         0
#define     DBG_SC_LocalHandle          1


/* ptrace notifications */
#define     DBG_N_Success                0      // Command completed successfully
#define     DBG_N_Error                 -1      // Error detected during command
#define     DBG_N_SStep                 -3      // Single Step detected
#define     DBG_N_Breakpoint            -4      // Breakpoint detected
#define     DBG_N_ProcTerm              -6      // Process termination - DosExitList done
#define     DBG_N_Exception             -7      // Exception detected
#define     DBG_N_LibLoad               -8      // Library module loaded
#define     DBG_N_AsyncStop             -11     // Async Stop detected
/* windows3 specific */
#define     DBG_N_Seg                  -100     // Segment info (unused)
#define     DBG_N_InfoAvail            -101     // Windows wants to output a string
#define     DBG_N_InfoReq              -102     // Windows wants to read a character
#define     DBG_N_ModStart             -103     // Start of module reached
#define     DBG_N_LibUnload            -104     // Library or application module unloaded

/* DBG_N_Exception subcodes (in Value field) */
#define DBG_T_DIVIDE                0
#define DBG_T_INVALID_OPCODE        6
#define DBG_T_GP_FAULT              13

/* ptrace struct */
struct dbg {
   unsigned long   Pid;           /* Debuggee Process id          */
   unsigned long   Tid;           /* Debuggee Thread id           */
   unsigned long   Cmd;           /* Command or Notification      */
   unsigned long   Value;         /* Generic Data Value           */
   unsigned long   Addr;          /* Debuggee Offset Address      */
   unsigned int    AddrSeg;       /* Debuggee Segment Address     */
   unsigned long   Buffer;        /* Debugger Buffer Address      */
   unsigned long   Len;           /* Length of Range              */
   unsigned long   Index;         /* Generic Identifier Index     */
   unsigned long   MTE;           /* Module Table Entry Handle    */
   unsigned long   EAX;           /* Register Set                 */
   unsigned long   EBX;
   unsigned long   ECX;
   unsigned long   EDX;
   unsigned long   ESI;
   unsigned long   EDI;
   unsigned long   EBP;
   unsigned long   ESP;
   unsigned long   EIP;
   unsigned long   EFlags;
   unsigned short  CS;
   unsigned char   CSAcc;         /* Access Bytes   Read Only(RO) */
   unsigned char   CSAtr;         /* Attribute Bytes         (RO) */
   unsigned long   CSBase;        /* Base                    (RO) */
   unsigned long   CSLim;         /* Byte Granular Limits.   (RO) */
   unsigned short  DS;
   unsigned char   DSAcc;
   unsigned char   DSAtr;
   unsigned long   DSBase;
   unsigned long   DSLim;
   unsigned short  ES;
   unsigned char   ESAcc;
   unsigned char   ESAtr;
   unsigned long   ESBase;
   unsigned long   ESLim;
   unsigned short  FS;
   unsigned char   FSAcc;
   unsigned char   FSAtr;
   unsigned long   FSBase;
   unsigned long   FSLim;
   unsigned short  GS;
   unsigned char   GSAcc;
   unsigned char   GSAtr;
   unsigned long   GSBase;
   unsigned long   GSLim;
   unsigned short  SS;
   unsigned char   SSAcc;
   unsigned char   SSAtr;
   unsigned long   SSBase;
   unsigned long   SSLim;
};

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


pick( "ASsembly\0", WND_ASSEMBLY, WndAsmOpen, AsmInfo, WndWideLong, WindowAssembly )
pick( "ALl\0", WND_ALL, WndNoOpen, NoInfo, WndNormal, WindowAll )
pick( "BReak\0", WND_BREAK, WndBrkOpen, BrkInfo, WndWide, WindowBrk )
pick( "Calls\0", WND_CALL, WndCallOpen,CallInfo,WndLong, WindowCalls )
pick( "Watch\0", WND_WATCH, WndWatOpen, VarInfo, WndLong, WindowWatches )
pick( "FIle\0", WND_FILE, WndNoOpen, FileInfo,WndFileMetrics, WindowFileView )
pick( "FPu\0", WND_FPU, WndFPUOpen, MadRegInfo, WndNormal, WindowFPU )
pick( "FUnctions\0", WND_FUNCTIONS, WndFuncOpen,FuncInfo,WndLong, WindowFunctions )
pick( "FILEScope\0", WND_FILESCOPE, WndFSVOpen, VarInfo, WndLong, WindowFile_Variables )
pick( "LOCals\0", WND_LOCALS, WndLclOpen, VarInfo, WndLong, WindowLocals )
pick( "LOG\0", WND_DIALOGUE, WndDlgOpen, LogInfo, WndLong, WindowLog )
pick( "MEmory\0", WND_MEMORY, WndMemOpen, MemInfo, WndMemMetrics, WindowMemory )
pick( "MOdules\0", WND_MODULES, WndModOpen, ModInfo, WndLong, WindowModules )
pick( "Register\0", WND_REGISTER, WndRegOpen, MadRegInfo, WndNormal, WindowRegisters )
pick( "RunThread\0", WND_RUN_THREAD, WndRunTrdOpen, RunTrdInfo, WndLong, WindowThreads )
pick( "SOurce\0", WND_SOURCE, WndSrcOpen, FileInfo,WndFileMetrics, WindowSource )
pick( "STack\0", WND_STACK, WndStkOpen, MemInfo, WndLong, WindowStack )
pick( "Thread\0", WND_THREAD, WndTrdOpen, TrdInfo, WndLong, WindowThreads )
pick( "IO\0", WND_IO, WndIOOpen, IOInfo, WndNormal, WindowIO_Ports )
pick( "Globals\0", WND_GLOBALS, WndGlobOpen,GlobInfo,WndLong, WindowGlobals )
pick( "Variable\0", WND_VARIABLE, WndVarOpen, VarInfo, WndLong, WindowVariable )
pick( "BInary\0", WND_BINARY, WndNoOpen, BinInfo, WndMemMetrics, WindowBinary )
pick( "IMage\0", WND_IMAGE, WndImgOpen, ImgInfo, WndLong, WindowImages )
pick( "GLobalfunctions\0", WND_GBLFUNCTIONS,WndGblFuncOpen,FuncInfo,WndLong, WindowGlobal_Functions )
pick( "ACcelerator\0",WND_MACRO, WndMacOpen, MacInfo, WndLong, WindowAccelerator )
pick( "TMPFile\0", WND_TMPFILE, WndNoOpen, FileInfo,WndFileMetrics, WindowTmpFile )
pick( "REPlay\0", WND_REPLAY, WndRepOpen, RepInfo, WndLong, WindowReplay )
pick( "MMX\0", WND_MMX, WndMMXOpen, MadRegInfo, WndNormal, WindowMMX )
pick( "XMM\0", WND_XMM, WndXMMOpen, MadRegInfo, WndNormal, WindowXMM )
// WND_CURRENT must appear at end so we can exclude if from some window lists
pick( "CUrrent\0", WND_CURRENT, WndNoOpen, NoInfo, WndLong, WindowCurrent )
pick( "SEarch\0", WND_SEARCH, WndNoOpen, SrchInfo, WndLong, WindowSearch )

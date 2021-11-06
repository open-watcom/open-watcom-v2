/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


pick( "ASsembly",        WND_ASSEMBLY,     WndAsmOpen,     AsmInfo,    WndWideLong,    WindowAssembly )
pick( "ALl",             WND_ALL,          WndNoOpen,      NoInfo,     WndNormal,      WindowAll )
pick( "BReak",           WND_BREAK,        WndBrkOpen,     BrkInfo,    WndWide,        WindowBrk )
pick( "Calls",           WND_CALL,         WndCallOpen,    CallInfo,   WndLong,        WindowCalls )
pick( "Watch",           WND_WATCH,        WndWatOpen,     VarInfo,    WndLong,        WindowWatches )
pick( "FIle",            WND_FILE,         WndNoOpen,      FileInfo,   WndFileMetrics, WindowFileView )
pick( "FPu",             WND_FPU,          WndFPUOpen,     MadRegInfo, WndNormal,      WindowFPU )
pick( "FUnctions",       WND_FUNCTIONS,    WndFuncOpen,    FuncInfo,   WndLong,        WindowFunctions )
pick( "FILEScope",       WND_FILESCOPE,    WndFSVOpen,     VarInfo,    WndLong,        WindowFile_Variables )
pick( "LOCals",          WND_LOCALS,       WndLclOpen,     VarInfo,    WndLong,        WindowLocals )
pick( "LOG",             WND_DIALOGUE,     WndDlgOpen,     LogInfo,    WndLong,        WindowLog )
pick( "MEmory",          WND_MEMORY,       WndMemOpen,     MemInfo,    WndMemMetrics,  WindowMemory )
pick( "MOdules",         WND_MODULES,      WndModOpen,     ModInfo,    WndLong,        WindowModules )
pick( "Register",        WND_REGISTER,     WndRegOpen,     MadRegInfo, WndNormal,      WindowRegisters )
pick( "RunThread",       WND_RUN_THREAD,   WndRunTrdOpen,  RunTrdInfo, WndLong,        WindowThreads )
pick( "SOurce",          WND_SOURCE,       WndSrcOpen,     FileInfo,   WndFileMetrics, WindowSource )
pick( "STack",           WND_STACK,        WndStkOpen,     MemInfo,    WndLong,        WindowStack )
pick( "Thread",          WND_THREAD,       WndTrdOpen,     TrdInfo,    WndLong,        WindowThreads )
pick( "IO",              WND_IO,           WndIOOpen,      IOInfo,     WndNormal,      WindowIO_Ports )
pick( "Globals",         WND_GLOBALS,      WndGlobOpen,    GlobInfo,   WndLong,        WindowGlobals )
pick( "Variable",        WND_VARIABLE,     WndVarOpen,     VarInfo,    WndLong,        WindowVariable )
pick( "BInary",          WND_BINARY,       WndNoOpen,      BinInfo,    WndMemMetrics,  WindowBinary )
pick( "IMage",           WND_IMAGE,        WndImgOpen,     ImgInfo,    WndLong,        WindowImages )
pick( "GLobalfunctions", WND_GBLFUNCTIONS, WndGblFuncOpen, FuncInfo,   WndLong,        WindowGlobal_Functions )
pick( "ACcelerator",     WND_MACRO,        WndMacOpen,     MacInfo,    WndLong,        WindowAccelerator )
pick( "TMPFile",         WND_TMPFILE,      WndNoOpen,      FileInfo,   WndFileMetrics, WindowTmpFile )
pick( "REPlay",          WND_REPLAY,       WndRepOpen,     RepInfo,    WndLong,        WindowReplay )
pick( "MMX",             WND_MMX,          WndMMXOpen,     MadRegInfo, WndNormal,      WindowMMX )
pick( "XMM",             WND_XMM,          WndXMMOpen,     MadRegInfo, WndNormal,      WindowXMM )
// WND_CURRENT must appear at end so we can exclude if from some window lists
pick( "CUrrent",         WND_CURRENT,      WndNoOpen,      NoInfo,     WndLong,        WindowCurrent )
pick( "SEarch",          WND_SEARCH,       WndNoOpen,      SrchInfo,   WndLong,        WindowSearch )

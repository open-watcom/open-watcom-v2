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
* Description:  WFC and WFL common options definition data
*
****************************************************************************/


//                      File Management Options
opt( "LISt",       OPT_LIST,         CMD|SRC,     NULL,         &BitOption )
opt( "PRint",      OPT_PRINT,        CMD,         NULL,         &BitOption )
opt( "TYpe",       OPT_TYPE,         CMD,         NULL,         &BitOption )
opt( "DIsk",       DISK_MASK,        CMD|NEG,     NULL,         &NegOption )
opt( "INCList",    OPT_INCLIST,      CMD,         NULL,         &BitOption )
opt( "ERrorfile",  OPT_ERRFILE,      CMD,         NULL,         &BitOption )
opt( "FR",         OPT_ERRFILE,      CMD,         NULL,         &BitOption )
opt( "INCPath",    OPT_INCPATH,      CMD|VAL,     &PathOption,  NULL )
opt( "I",          OPT_INCPATH,      CMD|VAL,     &PathOption,  NULL )
opt( "FO",         CGOPT_OBJ_NAME,   CMD|VAL|CG,  &FOOption,    NULL )
//                      Diagnostic Options
opt( "EXtensions", OPT_EXT,          CMD|SRC,     NULL,         &BitOption )
opt( "Reference",  OPT_REFERENCE,    CMD|SRC,     NULL,         &BitOption )
opt( "WArnings",   OPT_WARN,         CMD|SRC,     NULL,         &BitOption )
opt( "EXPlicit",   OPT_EXPLICIT,     CMD,         NULL,         &BitOption )
opt( "DEBug",      OPT_DEBUG,        CMD,         NULL,         &BitOption )
opt( "TRace",      OPT_TRACE,        CMD,         NULL,         &BitOption )
opt( "BOunds",     OPT_BOUNDS,       CMD,         NULL,         &BitOption )
opt( "STack",      CGOPT_STACK_CHK,  CMD|CG,      NULL,         &CGOption )
//                      Debugging Options
opt( "D1",         CGOPT_DB_LINE,    CMD|CG,      NULL,         &CGOption )
opt( "D2",         CGOPT_DB_LOCALS,  CMD|CG,      NULL,         &CGOption )
opt( "HC",         CGOPT_DI_CV,      CMD|CG,      NULL,         &CGOption )
opt( "HD",         CGOPT_DI_DWARF,   CMD|CG,      NULL,         &CGOption )
opt( "HW",         CGOPT_DI_WATCOM,  CMD|CG,      NULL,         &CGOption )
//opt( "DB",         OPT_BROWSE,       CMD,         NULL,         &BitOption, MS_OPT_BROWSE )
#if _CPU == 8086 || _CPU == 386
//                      Floating-Point Options
opt( "FPC",        CPUOPT_FPC,       CMD|CPU,     NULL,         &CPUOption )
opt( "FPI",        CPUOPT_FPI,       CMD|CPU,     NULL,         &CPUOption )
opt( "FPI87",      CPUOPT_FPI87,     CMD|CPU,     NULL,         &CPUOption )
opt( "FP2",        CPUOPT_FP287,     CMD|CPU,     NULL,         &CPUOption )
opt( "FP3",        CPUOPT_FP387,     CMD|CPU,     NULL,         &CPUOption )
opt( "FP5",        CPUOPT_FP5,       CMD|CPU,     NULL,         &CPUOption )
opt( "FP6",        CPUOPT_FP6,       CMD|CPU,     NULL,         &CPUOption )
opt( "FPR",        CPUOPT_FPR,       CMD|CPU,     NULL,         &CPUOption )
opt( "FPD",        CPUOPT_FPD,       CMD|CPU,     NULL,         &CPUOption )
#endif
//                      Optimization Options
#if _CPU == 386
opt( "OB",         OZOPT_O_BASE_PTR, CMD|OZ,      NULL,         &OZOption )
#endif
opt( "OBP",        OZOPT_O_BRANCH_PREDICTION,
                                     CMD|OZ,      NULL,         &OZOption )
opt( "OC",         OZOPT_O_CALL_RET, CMD|OZ,      NULL,         &OZOption )
opt( "OD",         OZOPT_O_DISABLE,  CMD|OZ,      NULL,         &OZOption )
#if _CPU == 386
opt( "OF",         OZOPT_O_FRAME,    CMD|OZ,      NULL,         &OZOption )
#endif
opt( "OH",         OZOPT_O_SUPER_OPTIMAL,
                                     CMD|OZ,      NULL,         &OZOption )
opt( "OI",         OZOPT_O_INLINE,   CMD|OZ,      NULL,         &OZOption )
opt( "OK",         OZOPT_O_FLOW_REG_SAVES,
                                     CMD|OZ,      NULL,         &OZOption )
opt( "OL",         OZOPT_O_LOOP,     CMD|OZ,      NULL,         &OZOption )
opt( "OL+",        OZOPT_O_UNROLL,   CMD|OZ,      NULL,         &OZOption )
#if COMP_CFG_CG_FPE
opt( "OLF",        OZOPT_O_LOOP_INVAR,
                                     CMD|OZ,      NULL,         &OZOption )
#endif
opt( "OM",         OZOPT_O_MATH,     CMD|OZ,      NULL,         &OZOption )
opt( "ON",         OZOPT_O_NUMERIC,  CMD|OZ,      NULL,         &OZOption )
opt( "OP",         OZOPT_O_VOLATILE, CMD|OZ,      NULL,         &OZOption )
opt( "OR",         OZOPT_O_INSSCHED, CMD|OZ,      NULL,         &OZOption )
opt( "OS",         OZOPT_O_SPACE,    CMD|OZ,      NULL,         &OZOption )
opt( "OT",         OZOPT_O_TIME,     CMD|OZ,      NULL,         &OZOption )
opt( "ODO",        OZOPT_O_FASTDO,   CMD|OZ,      NULL,         &OZOption )
opt( "OX",         OZOPT_O_X,        CMD|OZ,      NULL,         &OZOption )
//                      Memory Models
#if _CPU == 8086 || _CPU == 386
#if _CPU == 8086
opt( "MM",         CGOPT_M_MEDIUM,   CMD|CG,      NULL,         &CGOption )
opt( "ML",         CGOPT_M_LARGE,    CMD|CG,      NULL,         &CGOption )
opt( "MH",         CGOPT_M_HUGE,     CMD|CG,      NULL,         &CGOption )
#endif
#if _CPU == 386
opt( "MF",         CGOPT_M_FLAT,     CMD|CG,      NULL,         &CGOption )
opt( "MS",         CGOPT_M_SMALL,    CMD|CG,      NULL,         &CGOption )
opt( "MC",         CGOPT_M_COMPACT,  CMD|CG,      NULL,         &CGOption )
opt( "MM",         CGOPT_M_MEDIUM,   CMD|CG,      NULL,         &CGOption )
opt( "ML",         CGOPT_M_LARGE,    CMD|CG,      NULL,         &CGOption )
#endif
//                      CPU Targets
#if _CPU == 8086
opt( "0",          CPUOPT_8086,      CMD|CPU,     NULL,         &CPUOption )
opt( "1",          CPUOPT_80186,     CMD|CPU,     NULL,         &CPUOption )
opt( "2",          CPUOPT_80286,     CMD|CPU,     NULL,         &CPUOption )
#endif
opt( "3",          CPUOPT_80386,     CMD|CPU,     NULL,         &CPUOption )
opt( "4",          CPUOPT_80486,     CMD|CPU,     NULL,         &CPUOption )
opt( "5",          CPUOPT_80586,     CMD|CPU,     NULL,         &CPUOption )
opt( "6",          CPUOPT_80686,     CMD|CPU,     NULL,         &CPUOption )
#endif
//                      Application Types
opt( "BW",         CGOPT_BW,         CMD|CG,      NULL,         &CGOption )
#if _CPU == 386 || _CPU == _AXP || _CPU == _PPC
opt( "BM",         CGOPT_BM,         CMD|CG,      NULL,         &CGOption )
opt( "BD",         CGOPT_BD,         CMD|CG,      NULL,         &CGOption )
#endif
#if _CPU == 386 || _CPU == 8086
opt( "WINdows",    CGOPT_WINDOWS,    CMD|CG,      NULL,         &CGOption )
#endif
//                      Character Set Options
opt( "CHInese",    OPT_CHINESE,      CMD,         NULL,         &ChiOption )
opt( "Japanese",   OPT_JAPANESE,     CMD,         NULL,         &JapOption )
opt( "KOrean",     OPT_KOREAN,       CMD,         NULL,         &KorOption )
//                      Miscellaneous Options
opt( "SHort",      OPT_SHORT,        CMD,         NULL,         &BitOption )
opt( "XFloat",     OPT_EXTEND_REAL,  CMD,         NULL,         &BitOption )
opt( "XLine",      OPT_XLINE,        CMD,         NULL,         &XLOption )
opt( "DEFine",     OPT_DEFINE,       CMD|VAL,     &DefOption,   NULL )
opt( "FORmat",     OPT_EXTEND_FORMAT,CMD,         NULL,         &BitOption )
opt( "WILd",       OPT_WILD,         CMD,         NULL,         &BitOption )
opt( "TErminal",   OPT_TERM,         CMD,         NULL,         &BitOption )
opt( "Quiet",      OPT_QUIET,        CMD,         NULL,         &BitOption )
opt( "ZQ",         OPT_QUIET,        CMD,         NULL,         &BitOption )
opt( "RESources",  OPT_RESOURCES,    CMD,         NULL,         &BitOption )
opt( "CC",         OPT_UNIT_6_CC,    CMD,         NULL,         &BitOption )
opt( "LFwithff",   OPT_LF_WITH_FF,   CMD,         NULL,         &BitOption )
opt( "DEPendency", OPT_DEPENDENCY,   CMD,         NULL,         &BitOption )
#if _CPU == 8086 || _CPU == 386
opt( "SR",         CGOPT_SEG_REGS,   CMD|CG,      NULL,         &CGOption )
opt( "FSfloats",   CGOPT_FS_FLOATS,  CMD|CG,      NULL,         &CGOption )
opt( "GSfloats",   CGOPT_GS_FLOATS,  CMD|CG,      NULL,         &CGOption )
#endif
#if _CPU == 8086
opt( "SSfloats",   CGOPT_SS_FLOATS,  CMD|CG,      NULL,         &CGOption )
#endif
#if _CPU == 386
opt( "SC",         CGOPT_STK_ARGS,   CMD|CG,      NULL,         &CGOption )
opt( "EZ",         CGOPT_EZ_OMF,     CMD|CG,      NULL,         &CGOption )
opt( "SG",         CGOPT_STACK_GROW, CMD|CG,      NULL,         &CGOption )
#endif
opt( "SYntax",     OPT_SYNTAX,       CMD,         NULL,         &BitOption )
opt( "LIBinfo",    OPT_DFLT_LIB,     CMD,         NULL,         &BitOption )
opt( "DT",         CGOPT_DATA_THRESH,CMD|VAL|CG,  &DTOption,    NULL )
opt( "AUtomatic",  OPT_AUTOMATIC,    CMD,         NULL,         &BitOption )
opt( "DEScriptor", OPT_DESCRIPTOR,   CMD,         NULL,         &BitOption )
opt( "SAve",       OPT_SAVE,         CMD,         NULL,         &BitOption )
#if _CPU == 386 || _CPU == 8086
opt( "COde",       CGOPT_CONST_CODE, CMD|CG,      NULL,         &CGOption )
#endif
opt( "ALign",      CGOPT_ALIGN,      CMD|CG,      NULL,         &CGOption )
opt( "MAngle",     CGOPT_MANGLE,     CMD|CG,      NULL,         &CGOption )
opt( "IPromote",   OPT_PROMOTE,      CMD,         NULL,         &BitOption )
opt( "SEpcomma",   OPT_COMMA_SEP,    CMD,         NULL,         &BitOption )
#if _CPU == _AXP || _CPU == _PPC
opt( "LGA",        CGOPT_GENASM,      CMD|CG,     NULL,         &CGOption )
opt( "LGO",        CGOPT_LOGOWL,      CMD|CG,     NULL,         &CGOption )
#endif
opt( "LGC",        CGOPT_ECHOAPI,     CMD|CG,     NULL,         &CGOption )
opt( NULL,         0,                0,           NULL,         NULL )

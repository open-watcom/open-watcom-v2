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

#include "compcfg.h"

opt_entry       CompOptns[] = {
#if defined( __WFL__ )
//                      Compile and Link Options
opt( "",           0,                CTG,         NULL,         NULL,       CL_CTG_COMPILE_AND_LINK ),
opt( "C",          0,                0,           NULL,         NULL,       CL_COMPILE_ONLY ),
opt( "Y",          0,                0,           NULL,         NULL,       CL_IGNORE_WFL ),
opt( "P",          0,                0,           NULL,         NULL,       CL_RUN_PROTECT ),
opt( "FM[=<fn>]",  0,                0,           NULL,         NULL,       CL_GENERATE_MAP ),
opt( "FE=<fn>",    0,                0,           NULL,         NULL,       CL_NAME_EXECUTABLE ),
opt( "FD[=<fn>]",  0,                0,           NULL,         NULL,       CL_KEEP_DIRECTIVE ),
opt( "FI=<fn>",    0,                0,           NULL,         NULL,       CL_INCLUDE_DIRECTIVE ),
#if _CPU == 8086
opt( "LP",         0,                0,           NULL,         NULL,       CL_LINK_FOR_OS2 ),
opt( "LR",         0,                0,           NULL,         NULL,       CL_LINK_FOR_DOS ),
#endif
opt( "L=<system>", 0,                0,           NULL,         NULL,       CL_LINK_FOR_SYSTEM ),
opt( "K=<n>",      0,                0,           NULL,         NULL,       CL_STACK_SIZE ),
opt( "\"<drctvs>\"", 0,              0,           NULL,         NULL,       CL_LINKER_DIRECTIVES ),
#endif
//                      File Management Options
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_FILE_MANAGEMENT ),
opt( "LISt",       OPT_LIST,         CMD|SRC,     NULL,         &BitOption, MS_OPT_LIST ),
opt( "PRint",      OPT_PRINT,        CMD,         NULL,         &BitOption, MS_OPT_PRINT ),
opt( "TYpe",       OPT_TYPE,         CMD,         NULL,         &BitOption, MS_OPT_TYPE ),
opt( "DIsk",       DISK_MASK,        CMD|NEG,     NULL,         &NegOption, MS_OPT_DISK ),
opt( "INCList",    OPT_INCLIST,      CMD,         NULL,         &BitOption, MS_OPT_INCLIST ),
opt( "ERrorfile",  OPT_ERRFILE,      CMD,         NULL,         &BitOption, MS_OPT_ERRFILE ),
opt( "INCPath",    OPT_INCPATH,      CMD|VAL,     &PathOption,  NULL,       MS_OPT_INCPATH ),
opt( "FO",         CGOPT_OBJ_NAME,   CMD|VAL|CG,  &FOOption,    NULL,       MS_CGOPT_OBJ_NAME ),
//                      Diagnostic Options
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_DIAGNOSTIC ),
opt( "EXtensions", OPT_EXT,          CMD|SRC,     NULL,         &BitOption, MS_OPT_EXT ),
opt( "Reference",  OPT_REFERENCE,    CMD|SRC,     NULL,         &BitOption, MS_OPT_REFERENCE ),
opt( "WArnings",   OPT_WARN,         CMD|SRC,     NULL,         &BitOption, MS_OPT_WARN ),
opt( "EXPlicit",   OPT_EXPLICIT,     CMD,         NULL,         &BitOption, MS_OPT_EXPLICIT ),
opt( "DEBug",      OPT_DEBUG,        CMD,         NULL,         &BitOption, MS_OPT_DEBUG ),
opt( "TRace",      OPT_TRACE,        CMD,         NULL,         &BitOption, MS_OPT_TRACE ),
opt( "BOunds",     OPT_BOUNDS,       CMD,         NULL,         &BitOption, MS_OPT_BOUNDS ),
opt( "STack",      CGOPT_STACK_CHK,  CMD|CG,      NULL,         &CGOption,  MS_CGOPT_STACK_CHK ),
//                      Debugging Options
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_DEBUGGING ),
opt( "D1",         CGOPT_DB_LINE,    CMD|CG,      NULL,         &CGOption,  MS_CGOPT_DB_LINE ),
opt( "D2",         CGOPT_DB_LOCALS,  CMD|CG,      NULL,         &CGOption,  MS_CGOPT_DB_LOCALS ),
opt( "HC",         CGOPT_DI_CV,      CMD|CG,      NULL,         &CGOption,  MS_CGOPT_DI_CV ),
opt( "HD",         CGOPT_DI_DWARF,   CMD|CG,      NULL,         &CGOption,  MS_CGOPT_DI_DWARF ),
opt( "HW",         CGOPT_DI_WATCOM,  CMD|CG,      NULL,         &CGOption,  MS_CGOPT_DI_WATCOM ),
//opt( "DB",       OPT_BROWSE,       CMD,         NULL,         &BitOption, MS_OPT_BROWSE ),
#if _CPU == 8086 || _CPU == 386
//                      Floating-Point Options
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_FPUOPTS ),
opt( "FPC",        CPUOPT_FPC,       CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FPC ),
opt( "FPI",        CPUOPT_FPI,       CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FPI ),
opt( "FPI87",      CPUOPT_FPI87,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FPI87 ),
opt( "FP2",        CPUOPT_FP287,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FP287 ),
opt( "FP3",        CPUOPT_FP387,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FP387 ),
opt( "FP5",        CPUOPT_FP5,       CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FP5 ),
opt( "FP6",        CPUOPT_FP6,       CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FP6 ),
opt( "FPR",        CPUOPT_FPR,       CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FPR ),
opt( "FPD",        CPUOPT_FPD,       CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_FPD ),
#endif
//                      Optimization Options
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_OPTIMIZATIONS ),
#if _CPU == 386
opt( "OB",         OZOPT_O_BASE_PTR, CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_BASE_PTR ),
#endif
opt( "OBP",        OZOPT_O_BRANCH_PREDICTION,
                                     CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_BRANCH_PREDICTION ),
opt( "OC",         OZOPT_O_CALL_RET, CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_CALL_RET ),
opt( "OD",         OZOPT_O_DISABLE,  CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_DISABLE ),
#if _CPU == 386
opt( "OF",         OZOPT_O_FRAME,    CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_NEED_FRAME ),
#endif
opt( "OH",         OZOPT_O_SUPER_OPTIMAL,
                                     CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_SUPER_OPTIMAL ),
opt( "OI",         OZOPT_O_INLINE,   CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_INLINE ),
opt( "OK",         OZOPT_O_FLOW_REG_SAVES,
                                     CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_FLOW_REG_SAVES ),
opt( "OL",         OZOPT_O_LOOP,     CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_LOOP ),
opt( "OL+",        OZOPT_O_UNROLL,   CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_UNROLL ),
#if COMP_CFG_CG_FPE
opt( "OLF",        OZOPT_O_LOOP_INVAR,
                                     CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_LOOP_INVAR ),
#endif
opt( "OM",         OZOPT_O_MATH,     CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_MATH ),
opt( "ON",         OZOPT_O_NUMERIC,  CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_NUMERIC ),
opt( "OP",         OZOPT_O_VOLATILE, CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_VOLATILE ),
opt( "OR",         OZOPT_O_INSSCHED, CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_INSSCHED ),
opt( "OS",         OZOPT_O_SPACE,    CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_SPACE ),
opt( "OT",         OZOPT_O_TIME,     CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_TIME ),
opt( "ODO",        OZOPT_O_FASTDO,   CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_FASTDO ),
opt( "OX",         OZOPT_O_X,        CMD|OZ,      NULL,         &OZOption,  MS_OZOPT_O_X ),
//                      Memory Models
#if _CPU == 8086 || _CPU == 386
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_MEMORY_MODELS ),
#if _CPU == 8086
opt( "MM",         CGOPT_M_MEDIUM,   CMD|CG,      NULL,         &CGOption,  MS_CGOPT_M_MEDIUM ),
opt( "ML",         CGOPT_M_LARGE,    CMD|CG,      NULL,         &CGOption,  MS_CGOPT_M_LARGE ),
opt( "MH",         CGOPT_M_HUGE,     CMD|CG,      NULL,         &CGOption,  MS_CGOPT_M_HUGE ),
#endif
#if _CPU == 386
opt( "MF",         CGOPT_M_FLAT,     CMD|CG,      NULL,         &CGOption,  MS_CGOPT_M_FLAT ),
opt( "MS",         CGOPT_M_SMALL,    CMD|CG,      NULL,         &CGOption,  MS_CGOPT_M_SMALL ),
opt( "MC",         CGOPT_M_COMPACT,  CMD|CG,      NULL,         &CGOption,  MS_CGOPT_M_COMPACT ),
opt( "MM",         CGOPT_M_MEDIUM,   CMD|CG,      NULL,         &CGOption,  MS_CGOPT_M_MEDIUM ),
opt( "ML",         CGOPT_M_LARGE,    CMD|CG,      NULL,         &CGOption,  MS_CGOPT_M_LARGE ),
#endif
//                      CPU Targets
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_CPU ),
#if _CPU == 8086
opt( "0",          CPUOPT_8086,      CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_8086 ),
opt( "1",          CPUOPT_80186,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_80186 ),
opt( "2",          CPUOPT_80286,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_80286 ),
#endif
opt( "3",          CPUOPT_80386,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_80386 ),
opt( "4",          CPUOPT_80486,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_80486 ),
opt( "5",          CPUOPT_80586,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_80586 ),
opt( "6",          CPUOPT_80686,     CMD|CPU,     NULL,         &CPUOption, MS_CPUOPT_80686 ),
#endif
//                      Application Types
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_APPTYPE ),
opt( "BW",         CGOPT_BW,         CMD|CG,      NULL,         &CGOption,  MS_CGOPT_BW ),
#if _CPU == 386 || _CPU == _AXP || _CPU == _PPC
opt( "BM",         CGOPT_BM,         CMD|CG,      NULL,         &CGOption,  MS_CGOPT_BM ),
opt( "BD",         CGOPT_BD,         CMD|CG,      NULL,         &CGOption,  MS_CGOPT_BD ),
#endif
#if _CPU == 386 || _CPU == 8086
opt( "WINdows",    CGOPT_WINDOWS,    CMD|CG,      NULL,         &CGOption,  MS_CGOPT_WINDOWS ),
#endif
//                      Character Set Options
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_DBCS ),
opt( "CHInese",    OPT_CHINESE,      CMD,         NULL,         &ChiOption, MS_OPT_CHINESE ),
opt( "Japanese",   OPT_JAPANESE,     CMD,         NULL,         &JapOption, MS_OPT_JAPANESE ),
opt( "KOrean",     OPT_KOREAN,       CMD,         NULL,         &KorOption, MS_OPT_KOREAN ),
//                      Miscellaneous Options
opt( "",           0,                CTG,         NULL,         NULL,       MS_CTG_MISC ),
opt( "SHort",      OPT_SHORT,        CMD,         NULL,         &BitOption, MS_OPT_SHORT ),
opt( "XFloat",     OPT_EXTEND_REAL,  CMD,         NULL,         &BitOption, MS_OPT_EXTEND_REAL ),
opt( "XLine",      OPT_XLINE,        CMD,         NULL,         &XLOption,  MS_OPT_XLINE ),
opt( "DEFine",     OPT_DEFINE,       CMD|VAL,     &DefOption,   NULL,       MS_OPT_DEFINE ),
opt( "FORmat",     OPT_EXTEND_FORMAT,CMD,         NULL,         &BitOption, MS_OPT_EXTEND_FORMAT ),
opt( "WILd",       OPT_WILD,         CMD,         NULL,         &BitOption, MS_OPT_WILD ),
opt( "TErminal",   OPT_TERM,         CMD,         NULL,         &BitOption, MS_OPT_TERM ),
opt( "Quiet",      OPT_QUIET,        CMD,         NULL,         &BitOption, MS_OPT_QUIET ),
opt( "RESources",  OPT_RESOURCES,    CMD,         NULL,         &BitOption, MS_OPT_RESOURCES ),
opt( "CC",         OPT_UNIT_6_CC,    CMD,         NULL,         &BitOption, MS_OPT_UNIT_6_CC ),
opt( "LFwithff",   OPT_LF_WITH_FF,   CMD,         NULL,         &BitOption, MS_OPT_LF_WITH_FF ),
opt( "DEPendency", OPT_DEPENDENCY,   CMD,         NULL,         &BitOption, MS_OPT_DEPENDENCY ),
#if _CPU == 8086 || _CPU == 386
opt( "SR",         CGOPT_SEG_REGS,   CMD|CG,      NULL,         &CGOption,  MS_CGOPT_SEG_REGS ),
opt( "FSfloats",   CGOPT_FS_FLOATS,  CMD|CG,      NULL,         &CGOption,  MS_CGOPT_FS_FLOATS ),
opt( "GSfloats",   CGOPT_GS_FLOATS,  CMD|CG,      NULL,         &CGOption,  MS_CGOPT_GS_FLOATS ),
#endif
#if _CPU == 8086
opt( "SSfloats",   CGOPT_SS_FLOATS,  CMD|CG,      NULL,         &CGOption,  MS_CGOPT_SS_FLOATS ),
#endif
#if _CPU == 386
opt( "SC",         CGOPT_STK_ARGS,   CMD|CG,      NULL,         &CGOption,  MS_CGOPT_STK_ARGS ),
opt( "EZ",         CGOPT_EZ_OMF,     CMD|CG,      NULL,         &CGOption,  MS_CGOPT_EZ_OMF ),
opt( "SG",         CGOPT_STACK_GROW, CMD|CG,      NULL,         &CGOption,  MS_CGOPT_STACK_GROW ),
#endif
opt( "SYntax",     OPT_SYNTAX,       CMD,         NULL,         &BitOption, MS_OPT_SYNTAX ),
opt( "LIBinfo",    OPT_DFLT_LIB,     CMD,         NULL,         &BitOption, MS_OPT_DFLT_LIB ),
opt( "DT",         CGOPT_DATA_THRESH,CMD|VAL|CG,  &DTOption,    NULL,       MS_CGOPT_DATA_THRESH ),
opt( "AUtomatic",  OPT_AUTOMATIC,    CMD,         NULL,         &BitOption, MS_OPT_AUTOMATIC ),
opt( "DEScriptor", OPT_DESCRIPTOR,   CMD,         NULL,         &BitOption, MS_OPT_DESCRIPTOR ),
opt( "SAve",       OPT_SAVE,         CMD,         NULL,         &BitOption, MS_OPT_SAVE ),
#if _CPU == 386 || _CPU == 8086
opt( "COde",       CGOPT_CONST_CODE, CMD|CG,      NULL,         &CGOption,  MS_CGOPT_CONST_CODE ),
#endif
opt( "ALign",      CGOPT_ALIGN,      CMD|CG,      NULL,         &CGOption,  MS_CGOPT_ALIGN ),
opt( "MAngle",     CGOPT_MANGLE,     CMD|CG,      NULL,         &CGOption,  MS_CGOPT_MANGLE ),
opt( "IPromote",   OPT_PROMOTE,      CMD,         NULL,         &BitOption, MS_OPT_PROMOTE ),
opt( "SEpcomma",   OPT_COMMA_SEP,    CMD,         NULL,         &BitOption, MS_OPT_COMMA_SEP ),
#if _CPU == _AXP || _CPU == _PPC
opt( "LGA",        CGOPT_GENASM,      CMD|CG,     NULL,         &CGOption,  0 ),
opt( "LGO",        CGOPT_LOGOWL,      CMD|CG,     NULL,         &CGOption,  0 ),
#endif
opt( "LGC",        CGOPT_ECHOAPI,     CMD|CG,     NULL,         &CGOption,  0 ),
opt( NULL,         0,                0,           NULL,         NULL,       0 )
};

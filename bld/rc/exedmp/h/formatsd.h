/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  PE Dump Utility PE data macros definitions.
*
****************************************************************************/


#define ED_PE_CPUS \
    ED_PE_CPU( PE_CPU_UNKNOWN,      "UNKNOWN" ) \
    ED_PE_CPU( PE_CPU_386,          "80386" ) \
    ED_PE_CPU( PE_CPU_I860,         "I860" ) \
    ED_PE_CPU( PE_CPU_MIPS_R3000,   "MIPS R3000" ) \
    ED_PE_CPU( PE_CPU_MIPS_R4000,   "MIPS R4000" ) \
    ED_PE_CPU( PE_CPU_ALPHA,        "ALPHA" ) \
    ED_PE_CPU( PE_CPU_POWERPC,      "POWERPC" )

#define ED_PE_FLGS \
    ED_PE_FLG( PE_FLG_PROGRAM,          "PROGRAM" ) \
    ED_PE_FLG( PE_FLG_RELOCS_STRIPPED,  "RELOCS STRIPPED" ) \
    ED_PE_FLG( PE_FLG_IS_EXECUTABLE,    "EXECUTABLE" ) \
    ED_PE_FLG( PE_FLG_LINNUM_STRIPPED,  "LINE STRIPPED" ) \
    ED_PE_FLG( PE_FLG_LOCALS_STRIPPED,  "LOCALS STRIPPED" ) \
    ED_PE_FLG( PE_FLG_MINIMAL_OBJ,      "MINIMAL OBJ" ) \
    ED_PE_FLG( PE_FLG_UPDATE_OBJ,       "UPDATE OBJ" ) \
    ED_PE_FLG( PE_FLG_16BIT_MACHINE,    "16 BIT" ) \
    ED_PE_FLG( PE_FLG_REVERSE_BYTE_LO,  "REVERSE BYTE LO" ) \
    ED_PE_FLG( PE_FLG_32BIT_MACHINE,    "32 BIT" ) \
    ED_PE_FLG( PE_FLG_FIXED,            "FIXED" ) \
    ED_PE_FLG( PE_FLG_FILE_PATCH,       "FILE PATCH" ) \
    ED_PE_FLG( PE_FLG_FILE_SYSTEM,      "FILE SYSTEM" ) \
    ED_PE_FLG( PE_FLG_LIBRARY,          "LIBRARY" ) \
    ED_PE_FLG( PE_FLG_REVERSE_BYTE_HI,  "REVERSE BYTE HI" )

#define ED_PE_SSS \
    ED_PE_SS( PE_SS_UNKNOWN,        "UNKNOWN" ) \
    ED_PE_SS( PE_SS_NATIVE,         "NATIVE" ) \
    ED_PE_SS( PE_SS_WINDOWS_GUI,    "WINDOWS GUI" ) \
    ED_PE_SS( PE_SS_WINDOWS_CHAR,   "WINDOWS CHARACTER" ) \
    ED_PE_SS( PE_SS_OS2_CHAR,       "OS/2 CHARACTER" ) \
    ED_PE_SS( PE_SS_POSIX_CHAR,     "POSIX CHARACTER" ) \
    ED_PE_SS( PE_SS_PL_DOSSTYLE,    "PL DOS STYLE" )

#define ED_PE_DLLS \
    ED_PE_DLL( PE_DLL_PERPROC_INIT, "PER-PROCESS INITIALIZATION" ) \
    ED_PE_DLL( PE_DLL_PERPROC_TERM, "PER-PROCESS TERMINATION" ) \
    ED_PE_DLL( PE_DLL_PERTHRD_INIT, "PER-THREAD INITIALIZATION" ) \
    ED_PE_DLL( PE_DLL_PERTHRD_TERM, "PER-THREAD TERMINATION" )

#define ED_PE_OBJS \
    ED_PE_OBJ( PE_OBJ_DUMMY,        "DUMMY" ) \
    ED_PE_OBJ( PE_OBJ_NOLOAD,       "NO LOAD" ) \
    ED_PE_OBJ( PE_OBJ_GROUPED,      "GROUPED" ) \
    ED_PE_OBJ( PE_OBJ_NOPAD,        "NO PAD" ) \
    ED_PE_OBJ( PE_OBJ_TYPE_COPY,    "TYPE COPY" ) \
    ED_PE_OBJ( PE_OBJ_CODE,         "CODE" ) \
    ED_PE_OBJ( PE_OBJ_INIT_DATA,    "INIT DATA" ) \
    ED_PE_OBJ( PE_OBJ_UNINIT_DATA,  "UNINIT DATA" ) \
    ED_PE_OBJ( PE_OBJ_OTHER,        "OTHER" ) \
    ED_PE_OBJ( PE_OBJ_LINK_INFO,    "LINK INFO" ) \
    ED_PE_OBJ( PE_OBJ_OVERLAY,      "OVERLAY" ) \
    ED_PE_OBJ( PE_OBJ_REMOVE,       "REMOVE" ) \
    ED_PE_OBJ( PE_OBJ_COMDAT,       "COMDAT" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_1,      "ALIGN 1" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_2,      "ALIGN 2" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_4,      "ALIGN 4" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_8,      "ALIGN 8" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_16,     "ALIGN 16" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_32,     "ALIGN 32" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_64,     "ALIGN 64" ) \
    ED_PE_OBJ( PE_OBJ_DISCARDABLE,  "DISCARDABLE" ) \
    ED_PE_OBJ( PE_OBJ_NOT_CACHED,   "NOT CACHED" ) \
    ED_PE_OBJ( PE_OBJ_NOT_PAGABLE,  "NOT PAGABLE" ) \
    ED_PE_OBJ( PE_OBJ_SHARED,       "SHARED" ) \
    ED_PE_OBJ( PE_OBJ_EXECUTABLE,   "EXECUTABLE" ) \
    ED_PE_OBJ( PE_OBJ_READABLE,     "READABLE" ) \
    ED_PE_OBJ( PE_OBJ_WRITABLE,     "WRITABLE" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_MASK,   "ALIGN MASK" ) \
    ED_PE_OBJ( PE_OBJ_ALIGN_SHIFT,  "ALIGN SHIFT" )

#define ED_PE_RESS \
    ED_PE_RES( RT_NONE,            "" ) \
    ED_PE_RES( RT_CURSOR,          "Cursor" ) \
    ED_PE_RES( RT_BITMAP,          "Bitmap" ) \
    ED_PE_RES( RT_ICON,            "Icon" ) \
    ED_PE_RES( RT_MENU,            "Menu" ) \
    ED_PE_RES( RT_DIALOG,          "Dialog" ) \
    ED_PE_RES( RT_STRING,          "String" ) \
    ED_PE_RES( RT_FONTDIR,         "FontDir" ) \
    ED_PE_RES( RT_FONT,            "Font" ) \
    ED_PE_RES( RT_ACCELERATOR,     "Accelerator" ) \
    ED_PE_RES( RT_RCDATA,          "RcData" ) \
    ED_PE_RES( RT_11,              "" ) \
    ED_PE_RES( RT_GROUP_CURSOR,    "GroupCursor" ) \
    ED_PE_RES( RT_13,              "" ) \
    ED_PE_RES( RT_GROUP_ICON,      "GroupIcon" )

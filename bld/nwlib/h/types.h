/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2024 The Open Watcom Contributors. All Rights Reserved.
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


#define WL_PROCS \
    WL_PROC( WL_PROC_NONE, 0,           "" ) \
    WL_PROC( WL_PROC_AXP,  EM_ALPHA,    "AXP" ) \
    WL_PROC( WL_PROC_MIPS, EM_MIPS,     "MIPS" ) \
    WL_PROC( WL_PROC_PPC,  EM_PPC,      "PPC" ) \
    WL_PROC( WL_PROC_X86,  EM_386,      "X86" ) \
    WL_PROC( WL_PROC_X64,  EM_X86_64,   "X64" )

#define WL_FTYPES \
    WL_FTYPE( WL_FTYPE_NONE, 0,         "" ) \
    WL_FTYPE( WL_FTYPE_ELF,  0,         "ELF" ) \
    WL_FTYPE( WL_FTYPE_COFF, 0,         "COFF" ) \
    WL_FTYPE( WL_FTYPE_OMF,  0,         "OMF" )

#define WL_LTYPES \
    WL_LTYPE( WL_LTYPE_NONE, 0,         "" ) \
    WL_LTYPE( WL_LTYPE_AR,   0,         "AR" ) \
    WL_LTYPE( WL_LTYPE_MLIB, 0,         "MLIB" ) \
    WL_LTYPE( WL_LTYPE_OMF,  0,         "OMF" )

typedef uint_8          ar_len;

// local header types
typedef uint_32         arch_date;
typedef uint_16         arch_uid;
typedef uint_16         arch_gid;
typedef uint_32         arch_mode;
typedef uint_32         arch_file_size;

typedef enum {
    #define WL_LTYPE(e,p,n) e,
    WL_LTYPES
    #undef WL_LTYPE
} lib_type;

typedef enum {
    #define WL_PROC(e,p,n)  e,
    WL_PROCS
    #undef WL_PROC
} processor_type;

typedef enum {
    #define WL_FTYPE(e,p,n) e,
    WL_FTYPES
    #undef WL_FTYPE
} file_type;

typedef struct arch_dict {
    char                *fnametab;
    char                *ffnametab;
    char                *nextffname;
    char                *lastffname;
} arch_dict;

typedef struct arch_header {
    char                *name;
    char                *ffname; // Full filename
    arch_date           date;
    arch_uid            uid;
    arch_gid            gid;
    arch_mode           mode;
    arch_file_size      size;
    lib_type            libtype;
} arch_header;

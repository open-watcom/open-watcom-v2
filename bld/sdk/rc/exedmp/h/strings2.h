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


/* -- string representation for resource types --
   when update these arrays, update the corresponding arrays in
   format.h as well */

const char *resTypes[] = {
    "",
    "Cursor",
    "Bitmap",
    "Icon",
    "Menu",
    "Dialog",
    "String",
    "FontDir",
    "Font",
    "Accelerator",
    "RcData",
    "",
    "GroupCursor",
    "",
    "GroupIcon",
    NULL
};

char *cpu_flags_labels[] = {
    NULL,
    "UNKNOWN",
    "80386",
    "I860",
    "MIPS R3000",
    "MIPS R4000",
    "ALPHA",
    "POWERPC"
};

char *hdr_flags_labels[] = {
    NULL,
    "PROGRAM",
    "RELOCS STRIPPED",
    "EXECUTABLE",
    "LINE STRIPPED",
    "LOCALS STRIPPED",
    "MINIMAL OBJ",
    "UPDATE OBJ",
    "16 BIT",
    "REVERSE BYTE LO",
    "32 BIT",
    "FIXED",
    "FILE PATCH",
    "FILE SYSTEM",
    "LIBRARY",
    "REVERSE BYTE HI"
};

char *ss_flags_labels[] = {
    NULL,
    "UNKNOWN",
    "NATIVE",
    "WINDOWS GUI",
    "WINDOWS CHARACTER",
    "OS/2 CHARACTER",
    "POSIX CHARACTER",
    "PL DOS STYLE"
};

char *dll_flags_labels[] = {
    NULL,
    "PER-PROCESS INITIALIZATION",
    "PER-PROCESS TERMINATION",
    "PER-THREAD INITIALIZATION",
    "PER-THREAD TERMINATION"
};

char *obj_flags_labels[] = {
    NULL,
    "DUMMY",
    "NO LOAD",
    "GROUPED",
    "NO PAD",
    "TYPE COPY",
    "CODE",
    "INIT DATA",
    "UNINIT DATA",
    "OTHER",
    "LINK INFO",
    "OVERLAY",
    "REMOVE",
    "COMDAT",
    "ALIGN 1",
    "ALIGN 2",
    "ALIGN 4",
    "ALIGN 8",
    "ALIGN 16",
    "ALIGN 32",
    "ALIGN 64",
    "DISCARDABLE",
    "NOT CACHED",
    "NOT PAGABLE",
    "SHARED",
    "EXECUTABLE",
    "READABLE",
    "WRITABLE",
    "ALIGN MASK",
    "ALIGN SHIFT"
};


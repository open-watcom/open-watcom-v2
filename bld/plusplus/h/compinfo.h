/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  This file contains the structure definition for the global
*               compiler info.
*
****************************************************************************/

#ifndef _COMPINFO_H_
#define _COMPINFO_H_

typedef struct                          // DLL_DATA -- data for DLL
{   void (*print_str)( char const* );   // - print: string
    void (*print_chr)( char );          // - print: character
    void (*print_line)( char const* );  // - print: line
    char *cmd_line;                     // - command line
    int argc;                           // - argc/argv command line
    char **argv;
} DLL_DATA;

typedef struct comp_info {              // Compiler information
    TYPE            ptr_diff_near;      // - type from near ptr subtraction
    TYPE            ptr_diff_far;       // - type from far ptr subtraction
    TYPE            ptr_diff_far16;     // - type from far16 ptr subtraction
    TYPE            ptr_diff_huge;      // - type from huge ptr subtraction
    uint_8          init_priority;      // - initialization priority
    DT_METHOD       dt_method;          // - destruction method
    DT_METHOD       dt_method_speced;   // - specified destruction method
    unsigned        :0;                 // - alignment
    DLL_DATA*       dll_data;           // - data for DLL
    void*           exit_jmpbuf;        // - jmpbuf for DLL exit
    IDECBHdl        idehdl;             // - IDE handle
    IDECallBacks*   idecbs;             // - IDE call backs
    int             compfile_max;       // - max # files to compile
    int             compfile_cur;       // - current # of file to compile
    void*           primary_srcfile;    // - SRCFILE for primary file
    unsigned        fc_file_line;       // - line # in -fc file
    char*           pch_buff_cursor;    // - PCH read: buffer cursor
    char*           pch_buff_end;       // - PCH read: end of buffer
} comp_info;

#ifndef global
#define global  extern
#endif

global comp_info    CompInfo;       // - compiler information

#endif

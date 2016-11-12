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
* Description:  Structure definitions for the OSI loader.
*
****************************************************************************/


#include "watcom.h"

#include "pushpck1.h"

typedef struct {
    uint_32     off;
    uint_16     seg;
} addr_48;

typedef struct rex_hdr {
    char        sig[2];
    uint_16     file_size1;
    uint_16     file_size2;
    uint_16     reloc_cnt;
    uint_16     file_header;
    uint_16     min_data;
    uint_16     max_data;
    uint_32     initial_esp;
    uint_16     checksum;
    uint_32     initial_eip;
    uint_16     first_reloc;
    uint_16     overlay_number;
    uint_16     one;
} rex_exe;

typedef struct dos_hdr {
    uint_16     sig;
    uint_16     len_of_load_mod;
    uint_16     x;
    uint_16     reloc_count;
    uint_16     size_of_DOS_header_in_paras;
} dos_hdr;

typedef struct w32_hdr {
    char        sig[4];
    uint_32     start_of_W32_file;
    uint_32     size_of_W32_file;
    uint_32     offset_to_relocs;
    uint_32     memory_size;
    uint_32     initial_EIP;
} w32_hdr;

#include "poppck.h"

extern  void    PrintMsg( char *fmt,... );

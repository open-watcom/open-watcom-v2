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


#include <stdlib.h>

#define SHMEM_NAME      "WATCOM$CG$STATISTICS"

#define MAX_RTN         256

typedef struct {
    char        debug_info;             // do we have debug info
    char        src_name[ _MAX_PATH ];  // name of src file currently genning code for
    unsigned_32 src_line;               // line no we are on in current source file
} debug_info;

typedef struct {
    unsigned_32 frl_ins;                // number of items in instruction frl
} mem_info;

typedef struct {
    unsigned_32 nothing;                // just a placeholder
} blip_info;

typedef struct {
    char        file_name[ _MAX_PATH ]; // name of file being compiled
    char        rtn_name[ MAX_RTN ];    // name of routine being genned
    debug_info  debug;
    mem_info    mem;
    blip_info   blip;
} file_mapping;

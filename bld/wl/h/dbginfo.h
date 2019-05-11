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
* Description:  Watcom style debugging information internal structures
*
****************************************************************************/


#include "pushpck1.h"

typedef struct {
    struct {
        virt_mem_ptr    u;
    }               init;
    struct {
        virt_mem_ptr    u;
    }               curr;
    virt_mem_size   size;
    virt_mem_size   start;
} dbi_section;


typedef struct {
    dbi_section     locallinks;
    dbi_section     local;
    dbi_section     typelinks;
    dbi_section     type;
    dbi_section     linelinks;
    dbi_section     line;
    dbi_section     mod;
    dbi_section     global;
    dbi_section     addr;
    virt_mem        dump_addr;
    signed_16       modnum;
    class_entry     *LocalClass;
    class_entry     *TypeClass;
} debug_info;

typedef struct {
    unsigned_32         segment;
    unsigned_16         num;
} lineseg;

typedef struct dbinode {
    struct dbinode      *next;
    unsigned_16         segidx;
} dbinode;

#include "poppck.h"

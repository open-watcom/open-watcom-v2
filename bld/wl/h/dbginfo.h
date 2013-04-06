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


#define DBG_SIGNATURE   0x8386
#define FOX1_SIGNATURE  0x8300
#define FOX2_SIGNATURE  0x8301
#define RES_SIGNATURE   0x8302

#pragma pack(1)

typedef struct sectheader {
    unsigned_32         mod_offset;
    unsigned_32         gbl_offset;
    unsigned_32         addr_offset;
    unsigned_32         section_size;
    unsigned_16         section_id;
} sectheader;

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


typedef struct debug_info {
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


typedef struct dbgheader {
    unsigned_16         signature;
    unsigned_8          exe_major_ver;
    unsigned_8          exe_minor_ver;
    unsigned_8          obj_major_ver;
    unsigned_8          obj_minor_ver;
    unsigned_16         lang_size;
    unsigned_16         seg_size;
    unsigned_32         debug_size;
} dbgheader;

typedef struct gblinfo {
    unsigned_32         off;
    unsigned_16         seg;
    unsigned_16         mod_idx;
    unsigned_8          flags;
    char                name[ 1 ];
} gblinfo;

#define DBG_GBL_STATIC  0x1
#define DBG_GBL_DATA    0x2
#define DBG_GBL_CODE    0x4

typedef struct demand_info {
    unsigned_32         off;
    unsigned_16         len;
} demand_info;

typedef struct modinfo {
    unsigned_16         language;
    demand_info         locals;
    demand_info         types;
    demand_info         lines;
    char                name[ 1 ];
} modinfo;

typedef struct lineseg {
    unsigned_32         segment;
    unsigned_16         num;
} lineseg;

typedef struct segheader {
    unsigned_32         off;
    unsigned_16         seg;
    unsigned_16         num;
} segheader;

typedef struct addrinfo {
    unsigned_32         size;
    unsigned_16         mod_idx;
} addrinfo;

typedef struct dbinode {
    struct dbinode      *next;
    unsigned_16         segidx;
} dbinode;

#pragma pack()

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
* Description:  Watcom debugging information. 
*
****************************************************************************/


/*********************************************************************


    Debugging Info Layout
    =====================

        +-----------------------+
        |                       |
        |       EXE file        |
        |                       |
        +=======================+
        |                       |
        |       Overlays        |
        |                       |
        +=======================+
        |    Any Other Stuff    |
        +=======================+ <-- start of debugging info
        | source language table |
        +-----------------------+
        | segment address table |
        +-----------------------+
        |                       | <--\
        |  section debug info   |     +-- repeated for each overlay & root
        |                       | <--/
        +-----------------------+
        |  master debug header  |
        +=======================+ <-- end of file


    Section Debug Info
    ==================

        +-----------------------+
        |    section header     |
        +-----------------------+
        |     local section     | ====+
        +-----------------------+      \
        |     type section      |       >===> demand loaded info
        +-----------------------+      /
        |  line number section  | ====+
        +-----------------------+
        |  module info section  | ====+
        +-----------------------+      \
        | global symbol section |       >===> permanently loaded info
        +-----------------------+      /
        | address info section  | ====+
        +-----------------------+

**********************************************************************/


#define VALID_SIGNATURE 0x8386
#define FOX_SIGNATURE1  0x8300
#define FOX_SIGNATURE2  0x8301
#define WAT_RES_SIG     0x8302
#define OLD_EXE_MAJOR_VERSION 2
#define EXE_MAJOR_VERSION   3
#define EXE_MINOR_VERSION   0
#define OBJ_MAJOR_VERSION   1
#define OBJ_MINOR_VERSION   3

#include "pushpck1.h"

typedef struct {
    word            signature;      /* == 0x8386                    */
    byte            exe_major_ver;  /* == 2 or 3                    */
    byte            exe_minor_ver;  /* == 0                         */
    byte            obj_major_ver;  /* == 1                         */
    byte            obj_minor_ver;  /* == 1                         */
    word            lang_size;
    word            segment_size;
    dword           debug_size;
} _WCUNALIGNED master_dbg_header;

typedef struct {
    dword           mod_offset;     /* --\                          */
    dword           gbl_offset;     /*    +--> from section start   */
    dword           addr_offset;    /* --/                          */
    dword           section_size;
    word            section_id;
} _WCUNALIGNED section_dbg_header;


typedef struct {
    dword           info_off;       /* from debug header */
    union {
        word        size;           /* for V2 - size of demand info */
        word        entries;        /* for V3 - # entries in link table */
    }               u;
} _WCUNALIGNED demand_info;

typedef enum {
    DMND_LOCALS = 0,
    DMND_TYPES,
    DMND_LINES,
    MAX_DMND
} demand_kind;

typedef struct {
    word            language;       /* offset from source language table */
    demand_info     di[MAX_DMND];
    char            name[1];
} _WCUNALIGNED mod_info;

typedef struct {
    word            line_number;
    dword           code_offset;    /* offset from segment base */
} _WCUNALIGNED line_info;

typedef struct {
    word            segment;        /* offset from addr info class */
    word            num;
    line_info       line[1];        /* repeated 'num' times */
} _WCUNALIGNED v2_line_segment;

typedef struct {
    dword       segment;        /* offset from addr info class */
    word        num;
    line_info   line[1];        /* repeated 'num' times */
} _WCUNALIGNED v3_line_segment;

typedef struct {
    addr48_ptr      addr;
    word            mod;            /* offset from mod info class for V2 */
                                    /* module index number for V3 */
    char            name[1];
} _WCUNALIGNED gbl_info;

#define GBL_KIND_STATIC         0x01
#define GBL_KIND_DATA           0x02
#define GBL_KIND_CODE           0x04

typedef struct {
    addr48_ptr          addr;
    word                mod;
    byte                kind;
    char                name[1];
} _WCUNALIGNED v3_gbl_info;


typedef struct {
    dword           size;
    word            mod;            /* offset from mod info class for V2 */
                                    /* module index number for V3 */
} _WCUNALIGNED addr_info;

typedef struct {
    addr48_ptr      base;
    word            num;
    addr_info       addr[1];
} _WCUNALIGNED seg_info;

#define SEG_NUM_MASK    0x7fff

#include "poppck.h"

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


/* the 80386 task state segment (TSS) */

#include "watcom.h"

typedef struct {
    unsigned_16 backlink;
    unsigned_16 _zero1;
    unsigned_32 esp0;
    unsigned_16 ss0;
    unsigned_16 _zero2;
    unsigned_32 esp1;
    unsigned_16 ss1;
    unsigned_16 _zero3;
    unsigned_32 esp2;
    unsigned_16 ss2;
    unsigned_16 _zero4;
    unsigned_32 cr3;
    unsigned_32 eip;
    unsigned_32 eflags;
    unsigned_32 eax;
    unsigned_32 ecx;
    unsigned_32 edx;
    unsigned_32 ebx;
    unsigned_32 esp;
    unsigned_32 ebp;
    unsigned_32 esi;
    unsigned_32 edi;
    unsigned_16 es;
    unsigned_16 _zero5;
    unsigned_16 cs;
    unsigned_16 _zero6;
    unsigned_16 ss;
    unsigned_16 _zero7;
    unsigned_16 ds;
    unsigned_16 _zero8;
    unsigned_16 fs;
    unsigned_16 _zero9;
    unsigned_16 gs;
    unsigned_16 _zero10;
    unsigned_16 ldt;
    unsigned_16 _zero11;
    unsigned_16 t;
    unsigned_16 iobase;
} TSS;

typedef struct {
    unsigned_16 limitlow;
    unsigned_16 baselow;
    unsigned_8  basemid;
    unsigned_8  bits1;
    unsigned_8  bits2;
    unsigned_8  basehigh;
} descriptor;

// interesting flags in bits1 field

#define DESC_READWRITE          0x02
#define DESC_CODE               0x08
#define DESC_APPLICATION        0x10
#define DESC_PRESENT            0x80
#define DESC_TSS                0x09

// interesting flags in bits2 field

#define DESC_GRANULARITY_BIT    0x80
#define DESC_GENERAL            0x40
#define DESC_LIMIT_HIGH_MASK    0x0F

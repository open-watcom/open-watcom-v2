/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  PE/COFF Import libraries code
*               shared by librarian, linker and ORL projects
*
****************************************************************************/


/*
 * ALPHA transfer code
 */
static unsigned_32  CoffImportAxpText[] = {
    0x277F0000,     // ldah     r27,hioff(r31)
    0xA37B0000,     // ldl      r27,looff(r27)
    0x6BFB0000      // jmp      r31,0(r27)
};

/*
 * MIPS transfer code
 */
static unsigned_32  CoffImportMipsText[] = {
    0x3C080000,     // lui      r8,hioff(r0)
    0x8D080000,     // lw       r8,looff(r8)
    0x01000008,     // jr       r8
};

/*
 * PPC transfer code
 *
 * Register aliases: $sp -> $r1, $rtoc -> $r2
 *
 * Reloc for first instruction:
 *    TOCREL16,TOCDEFN __imp_<function_name>
 */
static unsigned_32  CoffImportPpcText[] = {
    0x81620000,     // lwz      r11,[tocv]__imp_<function_name>(r2)
    0x818B0000,     // lwz      r12,(r11)
    0x90410004,     // stw      r2,0x4(r1)
    0x7D8903A6,     // mtctr    r12
    0x804B0004,     // lwz      r2,0x4(r11)
    0x4E800420      // bctr
};

#ifndef INSIDE_WLINK
static unsigned_32  CoffImportPpcPdata[] = {
    0x00000000,
    0x00000018,
    0x00000000,
    0x00000003,
    0x0000000D,
};
#endif

#include "pushpck1.h"
static unsigned_8   CoffImportX64Text[] = {
    0xFF,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static unsigned_8   CoffImportX86Text[] = {
    0xFF,0x25,0x00,0x00,0x00,0x00
};
#include "poppck.h"

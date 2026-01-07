/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Declararions for linker executable formats and appropriate
*               message output.
*
****************************************************************************/

/* UTF-8 encoding, ¥ */


#define FORMATS() \
pick_format( 0x00000001, MK_OS2_NE,        0,  "OS/2",                 "OS/2" ) \
pick_format( 0x00000002, MK_OS2_LE,        1,  "OS/2 linear",          "OS/2 リニア" ) \
pick_format( 0x00000004, MK_OS2_LX,        2,  "OS/2 linear extended", "OS/2 リニア拡張" ) \
pick_format( 0x00000008, MK_WIN_NE,        3,  "Windows",              "Windows" ) \
pick_format( 0x00000010, MK_PE,            4,  "PE",                   "PE" ) \
pick_format( 0x00000020, MK_DOS_EXE,       5,  "DOS",                  "DOS" ) \
pick_format( 0x00000040, MK_COM,           6,  "DOS .COM",             "DOS .COM" ) \
pick_format( 0x00000080, MK_OVERLAYS,      7,  "DOS overlayed",        "DOS オーバレイ" ) \
pick_format( 0x00000100, MK_NOVELL,        8,  "Novell Netware",       "Novell Netware" ) \
pick_format( 0x00000200, MK_QNX_16,        9,  "QNX",                  "QNX" ) \
pick_format( 0x00000400, MK_PHAR_SIMPLE,   10, "Phar Lap simple",      "Phar Lap シンプル" ) \
pick_format( 0x00000800, MK_PHAR_FLAT,     11, "Phar Lap extended",    "Phar Lap 拡張" ) \
pick_format( 0x00001000, MK_PHAR_REX,      12, "Phar Lap relocatable", "Phar Lap リロケータブル" ) \
pick_format( 0x00002000, MK_PHAR_MULTISEG, 13, "Phar Lap segmented",   "Phar Lap セグメント" ) \
pick_format( 0x00004000, MK_QNX_FLAT,      14, "QNX 386",              "QNX 386" ) \
pick_format( 0x00008000, MK_ELF,           15, "ELF",                  "ELF" ) \
pick_format( 0x00010000, MK_WIN_VXD,       16, "Windows VxD",          "Windows VxD" ) \
pick_format( 0x00020000, MK_DOS16M,        17, "DOS/16M",              "DOS/16M" ) \
pick_format( 0x00040000, MK_RAW,           18, "RAW Binary Image",     "Raw" ) \
pick_format( 0x00080000, MK_RDOS_32,       19, "RDOS",                 "RDOS" ) \
pick_format( 0x00100000, MK_RDOS_16,       20, "RDOS 16-bit",          "RDOS 16-bit" )

/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Declararions for linker message output machinery.
*
****************************************************************************/


#define FORMATS() \
pick1( 0,  MK_OS2_NE,        "OS/2",                 "OS/2" ) \
pick1( 1,  MK_OS2_LE,        "OS/2 linear",          "OS/2 リニア" ) \
pick1( 2,  MK_OS2_LX,        "OS/2 linear extended", "OS/2 リニア拡張" ) \
pick1( 3,  MK_WIN_NE,        "Windows",              "Windows" ) \
pick1( 4,  MK_PE,            "PE",                   "PE" ) \
pick1( 5,  MK_DOS_EXE,       "DOS",                  "DOS" ) \
pick1( 6,  MK_COM,           "DOS .COM",             "DOS .COM" ) \
pick1( 7,  MK_OVERLAYS,      "DOS overlayed",        "DOS オーバレイ" ) \
pick1( 8,  MK_NOVELL,        "Novell Netware",       "Novell Netware" ) \
pick1( 9,  MK_QNX_16,        "QNX",                  "QNX" ) \
pick1( 10, MK_PHAR_SIMPLE,   "Phar Lap simple",      "Phar Lap シンプル" ) \
pick1( 11, MK_PHAR_FLAT,     "Phar Lap extended",    "Phar Lap 拡張" ) \
pick1( 12, MK_PHAR_REX,      "Phar Lap relocatable", "Phar Lap リロケータブル" ) \
pick1( 13, MK_PHAR_MULTISEG, "Phar Lap segmented",   "Phar Lap セグメント" ) \
pick1( 14, MK_QNX_FLAT,      "QNX 386",              "QNX 386" ) \
pick1( 15, MK_ELF,           "ELF",                  "ELF" ) \
pick1( 16, MK_WIN_VXD,       "Windows VxD",          "Windows VxD" ) \
pick1( 17, MK_DOS16M,        "DOS/16M",              "DOS/16M" ) \
pick1( 18, MK_ZDOS,          "ZDOS",                 "ZDOS" ) \
pick1( 19, MK_RAW,           "RAW Binary Image",     "Raw" ) \
pick1( 20, MK_RDOS_32,       "RDOS",                 "RDOS" ) \
pick1( 21, MK_RDOS_16,       "RDOS 16-bit",          "RDOS 16-bit" )

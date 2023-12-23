/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Executable format signatures.
*
****************************************************************************/


#ifndef _EXESIGNS_H_INCLUDED
#define _EXESIGNS_H_INCLUDED

#define EXESIGN_BW      0x5742          /* 'BW' DOS16M */
#define EXESIGN_C5      0x3543          /* 'C5' */
#define EXESIGN_DX      0x5844          /* 'DX' */
#define EXESIGN_LE      0x454c          /* 'LE' OS/2 LE 32-bit */
#define EXESIGN_LX      0x584c          /* 'LX' OS/2 LX 32-bit */
#define EXESIGN_MP      0x504d          /* 'MP' */
#define EXESIGN_MQ      0x514d          /* 'MQ' REX Pharlap */
#define EXESIGN_MZ      0x5a4d          /* 'MZ' DOS */
#define EXESIGN_NE      0x454e          /* 'NE' New Executable 16-bit */
#define EXESIGN_P2      0x3250          /* 'P2' */
#define EXESIGN_P3      0x3350          /* 'P3' */
#define EXESIGN_P6      0x3650          /* 'P6' */
#define EXESIGN_PE      0x4550          /* 'PE' Windows 32/64-bit */
#define EXESIGN_PL      0x4c50          /* 'PL' Pharlap TNT */
#define EXESIGN_PX      0x5850          /* 'PX' */

#define EXESIGN_DOS     EXESIGN_MZ
#define EXESIGN_DOS16M  EXESIGN_BW
#define EXESIGN_ELF     0x464c457f      /* '\x7fELF' ELF */
#define EXESIGN_REX     EXESIGN_MQ
#define EXESIGN_ZERO    0

#endif

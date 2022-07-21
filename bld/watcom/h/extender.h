/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#if defined(__DOS__)

/*
 * Values for '_Extender'
 */
#define DOSX_ERGO       0
#define DOSX_RATIONAL   1
#define DOSX_PHAR_V2    2
#define DOSX_PHAR_V3    3
#define DOSX_PHAR_V4    4
#define DOSX_PHAR_V5    5
#define DOSX_PHAR_V6    6
#define DOSX_PHAR_V7    7
#define DOSX_PHAR_V8    8
#define DOSX_INTEL      9
#define DOSX_WIN386     10

/*
 * Values for '_ExtenderSubtype'
 */
#define DOSX_PHARLAP_NORMAL         0
#define DOSX_PHARLAP_FMTOWNS        1
#define DOSX_RATIONAL_ZEROBASE      0
#define DOSX_RATIONAL_NONZEROBASE   1  /* Only in DOS4G Pro */

#define _IsOS386()               ( _Extender == DOSX_ERGO )
#define _IsRational()            ( _Extender == DOSX_RATIONAL )
#define _IsPharLap()             ( _Extender >= DOSX_PHAR_V2 && _Extender <= DOSX_PHAR_V8 )
#define _IsPharLapFMTOWNS()      ( _IsPharLap() && _ExtenderSubtype == DOSX_PHARLAP_FMTOWNS )
#define _IsCodeBuilder()         ( _Extender == DOSX_INTEL )
#define _IsWin386()              ( _Extender == DOSX_WIN386 )
#define _IsRationalNonZeroBase() ( _Extender == DOSX_RATIONAL && _ExtenderSubtype == DOSX_RATIONAL_NONZEROBASE )
#define _IsRationalZeroBase()    ( _Extender == DOSX_RATIONAL && _ExtenderSubtype == DOSX_RATIONAL_ZEROBASE )
#define _IsFlashTek()            ( _Extender == DOSX_PHAR_V3 && __X32VM != 0 )

extern  char            _Extender;
extern  char            _ExtenderSubtype;

extern  char    __X32VM;
#pragma aux     __X32VM "*";
extern  short   __x386_zero_base_selector;
#pragma aux     __x386_zero_base_selector "*";

#endif

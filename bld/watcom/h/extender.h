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


extern  char            _Extender;
extern  char            _ExtenderSubtype;
extern  unsigned short  _ExtenderRealModeSelector;

extern  char    __X32VM;
#pragma aux     __X32VM "*";
extern  short   __x386_zero_base_selector;
#pragma aux     __x386_zero_base_selector "*";

#define _IsOS386()               ( _Extender == 0 )
#define _IsRational()            ( _Extender == 1 )
#define _IsPharLap()             ( _Extender >= 2 && _Extender <= 8 )
#define _IsCodeBuilder()         ( _Extender == 9 )
#define _IsWin386()              ( _Extender == 10 )
#define _IsRationalNonZeroBase() ( _IsRational() && _ExtenderSubtype == 1 )
#define _IsRationalZeroBase()    ( _IsRational() && _ExtenderSubtype == 0 )
#define _IsFlashTek()            ( _Extender == 3 && __X32VM != 0 )
//#define _PharLapVer()   ( _Extender )  Not a valid thing to do!!!

#if !defined(__DOS_EXT__)
#if defined(__386__) &&                 \
   !defined(__WINDOWS_386__) &&         \
   !defined(__WINDOWS__) &&             \
   !defined(__OS2__) &&                 \
   !defined(__NT__) &&                  \
   !defined(__OSI__) &&                 \
   !defined(__QNX__) &&                 \
   !defined(__LINUX__)
#define __DOS_EXT__
#endif
#endif


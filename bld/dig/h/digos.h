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
* Description:  MAD OS constants pick file.
*
****************************************************************************/


// debugger internal variable dbg$os relate to this definition
// don't change existing items value, only add new one after last

//    enum              desc
pick( DIG_OS_IDUNNO,    "Unknown" )
pick( DIG_OS_DOS,       "DOS" )
pick( DIG_OS_OS2,       "OS/2" )
pick( DIG_OS_PHARLAP,   "Phar Lap 386 DOS Extender" )
pick( DIG_OS_ECLIPSE,   "Eclipse 386 DOS Extender" )
pick( DIG_OS_NW386,     "NetWare 386" )
pick( DIG_OS_QNX,       "QNX 4.x" )
pick( DIG_OS_RATIONAL,  "DOS/4G or compatible" )
pick( DIG_OS_WINDOWS,   "Windows 3.x" )
pick( DIG_OS_PENPOINT,  "PenPoint" )
pick( DIG_OS_NT,        "Win32" )
pick( DIG_OS_AUTOCAD,   "Autocad" )
pick( DIG_OS_NEUTRINO,  "QNX 6.x Neutrino" )
pick( DIG_OS_LINUX,     "Linux" )
pick( DIG_OS_FREEBSD,   "FreeBSD" )
pick( DIG_OS_NT64,      "Windows 64-bit" )

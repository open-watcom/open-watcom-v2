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
* Description:  target constant definitions
*
****************************************************************************/


// Define character sets:
// ======================

#define _ASCII  0
#define _EBCDIC 1
#define _KANJI  2

// Define CPU targets ( _CPU macro ):
// ==================================

#define _VAX    0
#define _AXP    1
#define _PPC    2
#define _SPARC  3
#define _MIPS   4
#define _370    370     // on a IBM S370
#define _80286  286     // >= 286 running 16-bit protected-mode ( OS/2 or DOS/16M )
#define _80386  386     // >= 386 running 32-bit
#define _8086   8086    // >= 8086 machine 16-bit real-mode

#define _INTEL_CPU      ( _CPU == 8086 || _CPU == 386 )
#define _RISC_CPU       ( _CPU == _AXP || _CPU == _PPC || _CPU == _MIPS )

#if 0
// Define Operating Systems:
// ==========================

#define _VMCMS          0
#define _PCDOS          1
#define _VMS            2
#define _QNX            3
#define _MVS            4
#define _HP             5
#define _OS2            6
#define _PENPOINT       7
#define _NT             8
#define _NETWARE        9
#define _OSI            10

#endif

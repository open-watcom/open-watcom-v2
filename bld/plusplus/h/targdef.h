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


//
// host configuration
//

/*
    _CHARSET: indicates the host compiler's character set
        _ASCII  for ASCII (default if not defined)
        _EBCDIC for EBCDIC
*/
#define _ASCII  0
#define _EBCDIC 1

/*
    _OS: indicates what OS the host compiler will run on
*/
#define _DOS    0
#define _OS2    1
#define _NT     2
#define _QNX    3
#define _OSI    4
#define _CMS    5

//
// target configuration
//

/*
    _CPU: indicates the architecture that we are generating code for

        8086    16-bit 8086/80186/80286/80386 code
        386     32-bit 386 code
        370     IBM 370 code
        _AXP    DEC Alpha
        _PPC    IBM/Motorola PowerPC
*/
#define _AXP    1
#define _PPC    2

// macros for architecture classes
#define _INTEL_CPU      ( _CPU == 8086 || _CPU == 386 )
#define _INTEL_HOST     ( _HOST == 8086 || _HOST == 386 )

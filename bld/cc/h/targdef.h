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


/*      for VERSION macro */

#define PRODUCTION      0
#define LOAD_N_GO       1

/*      for MACHINE macro */

#define _PC             0
#define _VAX            1
#define _S370           3
#define _PPC            4
#define _ALPHA          5
#define _SPARC          6

/*      for _OS macro */

#define _DOS            0
#define _OS2            1
#define _QNX            2
#define _CMS            3
#define _MVS            4
#define _NT             5

/*      for _HOST macro
        8086    on an 8086 machine running DOS
        286     on a 286 (or 386) running OS/2 or DOS/16M
        386     on a 386 running Phar Lap DOS Extender
        370     on a 370
        601     on a PPC 601
        000     on an Alpha
        8       on a  SPARC V8+
*/

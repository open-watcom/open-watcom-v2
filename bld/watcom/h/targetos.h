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


#ifndef _TARGETOS_H_INCLUDED

#define _OS_DOS         1       /* MSDOS */
#define _OS_OS2         2       /* OS/2 */
#define _OS_BND         3       /* both DOS and OS/2 */
#define _OS_NOV         4       /* Novell Netware 386 */
#define _OS_PLS         5       /* PharLap Systems 386|DOS-extender */
#define _OS_ECS         6       /* Eclipse 386 extender */
#define _OS_QNX         7       /* QNX */
#define _OS_CMS         8       /* IBM 370 CMS */
#define _OS_WIN         9       /* Microsoft Windows */
#define _OS_NT          10      /* NT */

#ifndef _OS
#define _OS             _OS_DOS /* default to MSDOS */
#endif

#define _TARGETOS_H_INCLUDED
#endif

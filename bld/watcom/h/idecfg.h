/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  IDE configuration file format version macros
*
****************************************************************************/


#ifndef _IDECFG_H_INCLUDED
#define _IDECFG_H_INCLUDED

/*
 * 39 and 40 are written the same, but read in differently
 * 39 messed up .BEFORE / .AFTER containing CR/LF's
 * since this is the same as the separator char.
 */

/*
 * current ide.cfg file format
 */
#define IDE_CFG_VERSION             5
/*
 * increment LATEST_SUPPORTED_VERSION macro
 * when some change in file formats is done
 */
#define LATEST_SUPPORTED_VERSION    42
#define OLDEST_SUPPORTED_VERSION    23
/*
 * target mask size
 */
#define MASK_SIZE                   5

#endif


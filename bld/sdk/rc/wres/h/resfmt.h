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


#ifndef RESDEFN_INCLUDED
#define RESDEFN_INCLUDED

#ifdef WIN_GUI
// the following is a temporary measure to get around the fact
// that winreg.h defines a type called ppvalue
#define _WINREG_
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "resutil.h"

#include "resnamor.h"
#include "resdiag.h"
#include "resaccel.h"
#include "resmenu.h"
#include "resstr.h"
#include "resraw.h"
#include "resiccu.h"
#include "resbitmp.h"
#include "resfont.h"
#include "resver.h"


/* Wes Nelson -- Added the following #ifndef to allow windows.h
 *               to be included before this file
 */
#ifndef WIN_GUI

/*** predefined type numbers ***/
#define RT_CURSOR           1
#define RT_BITMAP           2
#define RT_ICON             3
#define RT_MENU             4
#define RT_DIALOG           5
#define RT_STRING           6
#define RT_FONTDIR          7
#define RT_FONT             8
#define RT_ACCELERATOR      9
#define RT_RCDATA           10
#define RT_GROUP_CURSOR     12      /* note: no type 13 resource */
#define RT_GROUP_ICON       14

#endif

// these symbols are not defined in windows.h
#ifndef RT_VERSIONINFO
#define RT_VERSIONINFO      16
#endif

#ifndef RT_MESSAGETABLE
#define RT_MESSAGETABLE     11      /* for NT res files only */
#endif
#ifndef RT_ERRTABLE
#define RT_ERRTABLE         11
#endif
#ifndef RT_NAMETABLE
#define RT_NAMETABLE        15
#endif
#ifndef RT_VERSION
#define RT_VERSION          16
#endif
#ifndef RT_DLGINCLUDE
#define RT_DLGINCLUDE       17
#endif

#define RT_TOOLBAR          0xF1

typedef uint_16 ResMemFlags;
#define MEMFLAG_MOVEABLE            0x0010
#define MEMFLAG_PURE                0x0020
#define MEMFLAG_PRELOAD             0x0040
#define MEMFLAG_DISCARDABLE         0x1000

#endif

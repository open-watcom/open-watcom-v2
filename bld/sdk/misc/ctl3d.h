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
* Description:  Constants for 3D controls.
*
****************************************************************************/


/* This header is included to provide definitions of these constants for building
 * the source tree with OW 1.8 and earlier, which do not include a standard
 * implementation of ctl3d.h in w32api.
 */

/* Ctl3dSubclassDlg() flags */
#define CTL3D_BUTTONS           0x0001
#define CTL3D_LISTBOXES         0x0002
#define CTL3D_EDITS             0x0004
#define CTL3D_COMBOS            0x0008
#define CTL3D_STATICTEXTS       0x0010
#define CTL3D_STATICFRAMES      0x0020
#define CTL3D_ALL               0xffff

/* Ctl3dSubclassDlgEx() flags */
#define CTL3D_NODLGWINDOW       0x00010000

/* 3D control messages */
#define WM_DLGBORDER    (WM_USER + 3567)
#define WM_DLGSUBCLASS  (WM_USER + 3568)

/* WM_DLGBORDER return codes */
#define CTL3D_NOBORDER  0
#define CTL3D_BORDER    1

/* WM_DLGSUBCLASS return codes */
#define CTL3D_NOSUBCLASS    0
#define CTL3D_SUBCLASS      1

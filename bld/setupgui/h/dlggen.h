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


enum {
    CTL_NULL = 0,
    CTL_OK,
    CTL_CANCEL,
    CTL_OPTIONS,
    CTL_PREVIOUS,
    CTL_FIRST,
    CTL_STATLINE0,
    CTL_STATLINE1,
    CTL_HELP,
    CTL_SKIP,
    CTL_DONE,
    FIRST_UNUSED_ID = 100
};

#define ID2VH(x)    ((vhandle)(x - FIRST_UNUSED_ID))
#define VH2ID(x)    ((gui_ctl_id)(x + FIRST_UNUSED_ID))

extern int      NominalButtonWidth;

#define C0      0
#define R0      0
#define BW      (NominalButtonWidth)

#define W       45

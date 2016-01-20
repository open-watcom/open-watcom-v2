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
* Description:  About dialog interface.
*
****************************************************************************/


#ifndef _ABOUTDLG_H_INCLUDED
#define _ABOUTDLG_H_INCLUDED

#include "banner.h"

typedef struct {
    HINSTANCE   inst;
    HWND        owner;
    LPSTR       title;
    LPSTR       name;
    LPSTR       version;
} about_info;

#ifdef _M_I86
typedef about_info __far *LPABOUTINFO;
#else
typedef about_info *LPABOUTINFO;
#endif

extern void DoAbout( LPABOUTINFO ai );

#endif /* _ABOUTDLG_H_INCLUDED */

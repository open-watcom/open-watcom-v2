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


#include "variety.h"
#define NOCOVERSENDS
#define NOCOVERRC
#define NOAUTOPROCS
#include <windows.h>

#define MAGICCOOKIE 0xdeadbeefL

#define PROC_16         16
#define PROC_32         32

struct wndprocs {
    struct wndprocs     *next;
    LONG                procaddr;
    HWND                hwnd;
    unsigned short      proctype;       // PROC_16 or PROC_32
};

extern void *SetProc( FARPROC fp, int type );
extern BOOL TryAlias( HWND hwnd, WORD msg, LONG *lparam );
extern void *RealProcAddr( FARPROC fp );
extern DWORD _WCFAR *_DataSelectorSizeAddr;
#ifdef INCLUDE_PENWIN_H
extern void __InitPenWinRC( LPRC lprc );
#endif

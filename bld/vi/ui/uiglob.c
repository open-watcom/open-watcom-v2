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


#include "vi.h"
#include "win.h"
#include "dosx.h"
#include "regexp.h"
#if defined( __UNIX__ )
    #include "stdui.h"
#endif

char        MinSlots[MAX_MIN_SLOTS];
#ifdef __CURSES__
char_info   WindowNormalAttribute = { ' ', 0 };
#else
char_info   WindowNormalAttribute = { ' ', 7 };
#endif
wind        *Windows[MAX_WINDS];

char        WindowBordersNG[] = {
#undef vi_pick
#if defined( __UNIX__ )
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) UnixNG,
#else
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) DosNG,
#endif
#include "borders.h"
};
char        WindowBordersG[] =  {
#undef vi_pick
#if defined( __UNIX__ )
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) UnixG,
#else
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) DosG,
#endif
#include "borders.h"
#undef vi_pick
};
char        *GadgetString;

char        _FAR *Scrn;
char        _FAR *ClockStart;
char        _FAR *SpinLoc;
char        *ScreenImage;

#ifndef NOXTD
int             XMemBlockArraySize;
unsigned char   *XMemBlocks = NULL;
#endif
#ifndef NOEMS
int             MaxEMSBlocks   = 2048;
int             TotalEMSBlocks = 0;
int             EMSBlocksInUse = 0;
#endif
#ifndef NOXMS
int             MaxXMSBlocks   = 2048;
int             TotalXMSBlocks = 0;
int             XMSBlocksInUse = 0;
#endif
